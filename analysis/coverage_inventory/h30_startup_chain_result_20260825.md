# H30：离线启动链端到端行为等价验证

H30 在严格限定的离线共享内存合同下串联四个已经分别有单点证据的入口：`DynamicDllInit → getDeskCommand → dyn_init → dyn_main`。此前没有比较器覆盖该完整启动链，因此 H30 用原 ELF 检验桌面 IPC 写入的三轴初始角速度是否能够正确流入 `dyn_init`，并在其后的 100/1000 步零设备命令传播中保持主状态、积分状态、遥测与 IPC 的逐步一致性。

受控启动输入为 float 索引 64/65/66 的 `(1.25,-2.5,3.75)` 和 byte 索引 20–36 的 `0x80..0x90`。原 ELF probe 先建立 0xbf0 B 私有共享帧、调用 `DynamicDllInit` 和 `getDeskCommand`，再将内部初始条件传入 `dyn_init`。之后每步使用全零 `DpDeviceControlCommand` 调用 `dyn_main`、调用 `sendDynTele`，并写出四块：CoreDynamic 264 B、global `y[33]` 264 B、main 544 B、IPC payload 3000 B。

100/1000 步均完成 first/second 独立采集；每种长度的四块都逐文件 `cmp` 一致。原 ELF 仍保持 mode `0400`、SHA-256 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

| 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 | 双采集 |
|---:|---|---|---|---|---|
| 100 | `8161ed292012760acc5bf24ba19ee4fcde5312b8d39b93c740480989ab44bf88` | `b8281273993e20dbcca289f718617d15a066c71fb49f26adff378440a9e82a16` | `bf2e6ae0d311b2c1e3692358b83cb1c6a76b638e70282182b2680322eac5c380` | `d1902c857e91ae50eec6ac917d4b447d36eeca58c824e0dfe37ef6cc042a11b4` | PASS |
| 1000 | `f31c67d609afcbede6e29df6fcd7e0778dd0dbcbe85fa8d8eda8eeeb74eb07a2` | `ede1cba7e6b84638802060e2c0e20f38a579739c8b2f3d44d32e1f9e875a1c45` | `fd499a5f4618cc5b1f2b714c305659450446ac705d8ce016308ee45a849745cf` | `c4fa97525a16ed46f1723f56ed5204525daa101cbd4dd79401616f7043b8ebf6` | PASS |

恢复端以相同顺序调用对应函数，100 步生成 400 条四块逐步比较，1000 步生成 4000 条，全部为 0-byte mismatch。初始复测时仅 IPC 保留字节存在差异；CoreDynamic、global y 和 main 始终为零差异。定位后确认这些字节来自原 ELF 共享帧中 `getDeskCommand` 保留的输入/DRC 子区，而非动力学行为。比较器因此在本地观察帧中复现同一保留槽位，未修改恢复模型源码。

`make check-h30-startup-chain` 两条 PASS；从空构建运行 `make clean && make selftest` 为 325 条 PASS、非零块 0、显式失败 0。H30 仅用于离线研究/仿真；它不覆盖外部多进程竞争、读锁/共享内存建立失败、实时调度、真实任务数据或任何实体控制用途。
