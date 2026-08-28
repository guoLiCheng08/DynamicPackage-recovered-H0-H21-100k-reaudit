# H29：getDeskCommand 连续共享 IPC 入口验证

H29 覆盖公开 `getDeskCommand` 入口，而非结构化 `dyn_main` 或旧式 `dyn_main_array`。该函数从共享 IPC 映射读取浮点索引 64/65/66 和字节索引 20–36，将四个 desk flag、`DynamicDllInit` 初始条件中的三轴 `float` 角速度尾部，以及 DRC 数据块的指定槽位更新到持久全局状态。

每一步均向原 ELF 的受控共享映射写入一组变化值：三个浮点分别为 `step+0.25`、`-step-0.5`、`step*0.125+1.0`；17 个字节为 `(step*17+index+0x80) mod 256`。随后调用 `getDeskCommand`，并导出 flags（4 B）、初始角速度尾部（12 B）与 DRC（112 B）。该合同连续覆盖浮点/字节 IPC 读取、槽位重排、持久全局覆盖及共享读锁相关路径。

原 ELF 的 100 与 1000 步均执行 first/second 独立采集，三块均逐文件 `cmp` 一致。原 ELF 始终为 mode `0400`、SHA-256 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

| 长度 | flags SHA-256 | init-tail SHA-256 | DRC SHA-256 | 双采集 |
|---:|---|---|---|---|
| 100 | `790c0c6651e15a3d83988fd9a460ca8baac644591db1dcbb72fcd4873a2777e7` | `d7a5057ef21932cc628e20f4c8e3a3c43d528aeeecfa471e6dd898ed3378582a` | `744a5770a2e1206655a3ef3e5816ab93b56f9503bc4dee3e81bd48a65e27a5fd` | PASS |
| 1000 | `3acf1272ce18e8a67b80e637bee5340159568a0752880744bdceab0e033b43bc` | `6f422a68da2337704769912dedb0aaaacb9789fd1229c7bb34df9275d77b8d58` | `d69989f689f4890b56f6920ad4a70493e6d7b3ba9610c2fa8fada7731d9901a1` | PASS |

恢复端以同一序列调用 `dp_ipc_seed_float_for_test`、`dp_ipc_seed_u8_for_test` 和 `getDeskCommand`。100 步对三个观察块建立 300 条逐步比较，1000 步建立 3000 条，均为 0-byte mismatch。`make check-h29-get-desk-command` 通过两条比较器；从空构建运行完整 selftest 为 323 条 PASS、非零块 0、显式失败 0。

H29 已加入默认严格门禁。该结果仅覆盖当前 Linux x86-64、受控共享映射、无竞争写入、固定索引和有限 100/1000 调用数；不证明外部多进程竞争、读锁失败、IPC 建立失败、任意索引或其他平台下的全域等价。工程仅限离线研究与仿真。
