# H33：共享 IPC getter 无效索引与自动重开错误合同

**日期：** 2026-08-25

**范围：** Linux x86-64、当前运行时库、默认严格 C11 构建；仅离线研究、受控仿真与回归。

H33 不修改恢复模型源码，而是为此前只验证成功读取、`getDeskCommand` 连续读取和无竞争打开/关闭/重开序列的共享 IPC 路径，补充安全且无并发的 getter 错误返回合同。原 ELF 仅通过只读文件的调试副本运行；正式 `input/DynamicPackage.elf` 保持 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

## 固定调用序列

探针先 `shm_unlink("/sharedvars_example")`，再调用 `init_shared()` 建立一个新的、零初始化的真实映射。它以浮点哨兵 `123.5f` 与字节哨兵 `0x5a` 执行下表中的受控调用，不建立竞争、不注入资源耗尽，也不接触任何实体设备。

| 顺序 | 调用 | 原 ELF 返回值 | 输出可观察结果 |
|---:|---|---:|---|
| 1 | `init_shared()` | `0` | 建立空映射 |
| 2 | `get_float_value(-1, &float_sentinel)` | `-21` | `float_sentinel` 仍为 `123.5f` |
| 3 | `get_float_value(0x258, &float_sentinel)` | `-21` | `float_sentinel` 仍为 `123.5f` |
| 4 | `get_float_value(0, NULL)` | `-22` | 无输出指针解引用 |
| 5 | `get_uint8_value(-1, &u8_sentinel)` | `-21` | `u8_sentinel` 仍为 `0x5a` |
| 6 | `get_uint8_value(0x258, &u8_sentinel)` | `-21` | `u8_sentinel` 仍为 `0x5a` |
| 7 | `get_uint8_value(0, NULL)` | `-22` | 无输出指针解引用 |
| 8 | `close_shared(); get_float_value(0, &reopen_out)` | `0` | 自动重开后从新零映射读取 `0.0f` |

> 这里的负索引与 `0x258` 均落在原 ELF 已观察到的 `0x000..0x257` 有效闭区间之外。H33 只陈述上述具体调用、输出对象、运行时与映射准备条件下的可观察行为；它不把负索引行为泛化为任何未定义 ABI 输入的通用安全承诺。

## 双采集与恢复端差分

first/second 原 ELF GDB probe 各自创建、关闭和解除同名共享对象，并分别导出 32 B 返回码数组、4 B 无效 float 哨兵、4 B 自动重开输出和 1 B uint8 哨兵。四个 first/second 文件均逐文件 `cmp` 一致。恢复端严格 C11 比较器使用完全相同的调用顺序、初值和清理序列，四个观察块均为 0-byte mismatch。

| gold 块 | 大小 | first SHA-256 | first/second | 恢复端 |
|---|---:|---|---|---|
| 8 个 `int32_t` 返回码 | 32 B | `afafd35cae487a6699cb19515903c734529269e17c1f996876881be4d0e475b5` | PASS | 0/32 B |
| 无效 float 索引输出哨兵 | 4 B | `367da4259c7beab7a7ce59235298dd373143ba8100b46498843f0a3fdb821e85` | PASS | 0/4 B |
| 关闭后自动重开 float 输出 | 4 B | `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119` | PASS | 0/4 B |
| 无效 uint8 索引输出哨兵 | 1 B | `bbeebd879e1dff6918546dc0c179fdde505f2a21591c9a9c96e36b054ec5af83` | PASS | 0/1 B |

对应原 ELF 探针为 `analysis/coverage_inventory/p0_command_family/ipc_getter_h33_invalid_{first,second}_probe.gdb`，恢复端比较器为 `analysis/ipc_getter_h33_invalid_compare.c`，局部门禁为 `make check-h33-ipc-getter-invalid`。该比较器以 `-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off` 构建，使用 `-pthread -lrt` 链接。修订调用合同后已从空构建运行 `make clean && make selftest`：338 个测试可执行项、365 条编译调用、329 条 PASS、非零 mismatch 计数 0、显式失败计数 0；可提交的回归摘要日志为 `analysis/coverage_inventory/h33_full_selftest_summary_20260825.log`；完整运行仅在本次本地执行中使用，不纳入版本库以避免重复的超大输出。

## 结论与边界

H33 直接证实：在已建立的真实共享映射中，两个 getter 对负索引和 `0x258` 返回 `-21` 且保持提供的输出对象；对有效索引与空输出指针返回 `-22`；关闭全局映射后，随后 `get_float_value(0, ...)` 会自动重开并在零初始化帧中成功返回 `0.0f`。恢复实现已与这 41 B 可观察合同 bitwise 一致，因而**不应基于猜测修改**现有 getter 或 `getDeskCommand` 对 getter 返回码的处理。

H33 没有覆盖共享对象打开失败、文件权限、锁初始化失败、资源耗尽、损坏的跨进程锁、竞态、并发读写、其他 getter/setter、任意索引、任意指针或其他平台。它也不构成对真实卫星飞行、姿态控制、实体执行机构、任务决策或任何安全关键用途的验证；工程仅限离线研究、教学、受控仿真和回归。
