# H26：高偏心跨年、完整柔性与有效多执行机构命令联合场景

H26 是新增独立输入场景。它以 H0/H15 的高偏心跨年环境为基础：`a=26,560,000 m`、`e=0.65`、2031-12-31 23:59:50、caller-state 位置 `(-12,000,000, 65,000,000, 28,000,000)` m、速度 `(0,2000,0)` m/s、三轴角速度 `(0.0125,-0.00875,0.00425)`；同时叠加 H21 的替代姿态、完整 20 维交错柔性状态、DSS0 高斯噪声开启/DSS1 关闭、`srand(12345)`、RWheel0 预置，以及种子 `0x2468ace1` 的有限有效 LCG 多执行机构命令映射。

原 ELF 始终保持 mode `0400`、SHA-256 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`；动态采集仅运行 `analysis/debug_runner/DynamicPackage.exec_copy`。H26 的 100 与 1000 步均完成 first/second 独立采集，四个逐步块（caller-state、global `y[33]`、main、IPC）及七个终态执行机构快照共 11 个文件均逐文件 `cmp` 一致。

| 长度 | caller-state SHA-256 | global y SHA-256 | main SHA-256 | IPC SHA-256 |
|---:|---|---|---|---|
| 100 | `10e57af9570bcd95b9bf7480d7397e4f620731585b5d2999ac3a67c568247ddc` | `9fc186a44143cb049051e981164814406d42034dc2314f2b9f89359143939eaf` | `afc60f9f2fd25092fc97d00cfcf0a932c1cfd85b5a56ca1e62cb19de3593bd65` | `3ed5f9338c9b7897af021f673f7ff6104a14846de33c4f13ef922e8f65096ae6` |
| 1000 | `58e319eda7f7ed3d77cb885975b70ece96352c9e554f3415d2890fabc1112109` | `4082236cd2ab73dcfdb85e708c32d7be2cd56d927831a794159e282b78cbde92` | `d59d1473c1ad2325b1a3550c927da6a27d665c158bfb9c8ee6ee345aaf76e8d8` | `0e737bcd211261b5792686265c3e9e81d73b3b80c73b51b850f55240d57a0f87` |

恢复 C 的 H26 100/1000 步比较器均逐步比较四块及终态执行机构快照，记录分别为 411 与 4,011 条 `mismatched bytes`，全部为 0。新增 `make check-h26-high-ecc-cross-command`，空构建下输出两条 bitwise PASS；随后 `make clean && make selftest` 通过，326 个测试可执行项、317 条 PASS、非零字节差异 0、显式失败 0。

结论仅适用于列明的有限输入合同、当前 Linux x86-64、当前运行时和严格 C11 构建；不构成任意轨道、连续命令、异常输入、并发、其他平台或未观察对象上的全域等价承诺。本工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。
