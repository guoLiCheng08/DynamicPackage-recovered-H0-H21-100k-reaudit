# H31：`getDeskCommand` 共享 IPC 生命周期验证

H31 补充 H29 的连续有效读路径，验证共享 IPC 的离线生命周期：创建映射、写入固定输入、调用 `getDeskCommand`、关闭映射、重新打开同一共享对象、再次调用 `getDeskCommand`。它只覆盖当前 Linux x86-64 下的无竞争成功生命周期，不构造外部对抗、真实多进程竞争或任务控制场景。

原 ELF probe 在 `main` 断点执行 `shm_unlink("/sharedvars_example")`、`init_shared`，向真实共享映射写入 float 索引 64/65/66 的 `(6.25,-7.5,8.75)` 和 byte 索引 20–36 的 `0xa0..0xb0`，再调用 `DynamicDllInit` 与 `getDeskCommand`。第一阶段导出 desk flags（4 B）、初始化角速度尾部（12 B）和 DRC（112 B）。随后调用 `close_shared`、`init_shared`，不重写输入，第二次 `getDeskCommand` 后导出同样三块。

两次独立原 ELF 采集在开映射后与重开映射后均逐文件一致；两个阶段的值也一致，说明在该受控对象未 unlink 的合同下，`close_shared` 后重新映射保留输入数据。恢复端用同一顺序调用 `close_shared`、`shm_unlink`、`init_shared`、测试 seed、`getDeskCommand`、关闭、重开和再次读取，六个块均为 0-byte mismatch。

| 阶段 | flags SHA-256 | init-tail SHA-256 | DRC SHA-256 | first/second |
|---|---|---|---|---|
| 打开后 | `a57d29e1bf3bda74302c8f8148977cab1499a1c3f239146a75d185b6f25bf9e1` | `f22b426ff83c22af9814c35e460e550fe1b48dfd729506003804807041138c13` | `5b1fc66c257e31114be672fe96c9686633c3dd49078d386fa429a4eb9d2cb5e1` | PASS |
| 重开后 | `a57d29e1bf3bda74302c8f8148977cab1499a1c3f239146a75d185b6f25bf9e1` | `f22b426ff83c22af9814c35e460e550fe1b48dfd729506003804807041138c13` | `5b1fc66c257e31114be672fe96c9686633c3dd49078d386fa429a4eb9d2cb5e1` | PASS |

`make check-h31-get-desk-lifecycle` 为 1 条 PASS；从空构建运行 `make clean && make selftest` 为 326 条 PASS、非零块 0、显式失败 0。H31 仅证明上述固定映射名称、无竞争、成功 `shm_open/mmap` 与受控输入的行为等价；不证明权限错误、锁错误、资源耗尽、并发读写或实体控制场景。
