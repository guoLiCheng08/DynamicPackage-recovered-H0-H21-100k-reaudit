# DynamicPackage 受控长时域覆盖矩阵：H0 与 H1

**记录范围。** 本文仅记录在 Linux x86-64、默认严格 C11 构建、离线固定输入及保留的原 ELF gold 下完成的行为差分结果。所有 “PASS” 均表示列出的可观察块在列出的有限步数内逐字节相同；不构成任意输入、并发、异常、平台、编译器或数学全域等价证明。

## 统一比较口径

| 项目 | 值 |
|---|---:|
| 恢复端编译 | `-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off` |
| 每步调用输出比较 | caller `CoreDynamic` 前 `0x108`，264 B |
| 每步全局积分状态比较 | `y[33]`，264 B |
| 每步主输出比较 | telemetry frame，544 B |
| 每步 IPC 比较 | payload，3000 B |
| 原 ELF保护 | 权限 `0400`；SHA-256 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403` |
| 金标采集 | 每个列出的 1000 步场景均至少两次原 ELF采集，并逐文件 `cmp` 验证一致 |

## 场景矩阵

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H0** | 高偏心 `a=26,560,000`、`e=0.65`；2031-12-30 23:59:50；四元数 `(0.5,-0.5,0.5,0.5)`；全柔性状态；零设备命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |
| **H1** | 近圆 LEO `a=7,000,000`、`e=0.01`；2025-01-02 03:04:05；单位四元数；前 9 次调用覆盖轮、MTQ、两组 SADA、推力器和惯量更新；其后零命令；`srand(1)` | PASS | PASS | PASS | 已接入 |

## H0-1000 原 ELF gold 哈希

| 文件 | 大小 | SHA-256 |
|---|---:|---|
| `gold_high_ecc_shadow_adjacent_date_thousand_step_state.bin` | 264000 B | `43c8396c1c6b40d6011ac476e9a1a2aaf683434500da3e66c8fdd1eae5344009` |
| `gold_high_ecc_shadow_adjacent_date_thousand_step_global_y.bin` | 264000 B | `a28cc71a925db48df22acde2334327c52444bed817e4dda8ba64d24222a8d065` |
| `gold_high_ecc_shadow_adjacent_date_thousand_step_out.bin` | 544000 B | `fdae7c3a2c4eca07427fb1f76ffb1847a0c63308d3133789fb39e567058df7a6` |
| `gold_high_ecc_shadow_adjacent_date_thousand_step_ipc_payload.bin` | 3000000 B | `45326a9db90401b32a3050d08092f51957c8611840281e5bcf839517ae6e8b47` |

## H1-1000 原 ELF gold 哈希

| 文件 | 大小 | SHA-256 |
|---|---:|---|
| `gold_mixed_command_thousand_step_state.bin` | 264000 B | `d3b6fccd5eccb5d8be5c58ceec04cf30e685012171f74b32c767c757d7aff6c6` |
| `gold_mixed_command_thousand_step_global_y.bin` | 264000 B | `0ec6311b4214cdac6d95c3ccb6b54241128f3f7a0051a2066f7d3c4a531defe1` |
| `gold_mixed_command_thousand_step_out.bin` | 544000 B | `b871b630c32da0457d55a121c66304e2e202b9d150ce90e040da08d58d3675af` |
| `gold_mixed_command_thousand_step_ipc_payload.bin` | 3000000 B | `35dc27a0c5959b9e3b44385b1ca2035dbfdd5a62ba12f5c38c7ba4a2dae6f60a` |

## 最新完整门禁

新增 H1 100／1000 步比较器后，`make clean && make selftest` 已在默认严格 C11 标志下通过。`make -n selftest` 统计为 **280 个**测试可执行项，其中包括 H0/H1 的四个长期逐步 bitwise 比较器。

> **使用限制。** 本工程只用于离线研究和仿真。不得用于飞行、控制、实体执行机构或任何安全关键用途。后续应分别对不同日期、初始姿态、轨道、命令序列和异常输入建立新的双采集 gold 与独立差分场景，而不能用现有有限矩阵替代全空间证明。

## H2 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H2** | 2024-02-29 12:34:56；`a=7,078,137 m`、`e=0.05`；替代四元数 `(0.5,-0.5,0.5,0.5)`；完整非零 20 维柔性状态；零设备命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H2-1000 原 ELF gold：state `346a83e713dee0040755c65a4b71ad9ddec7882f58d673d742daba7407477e1f`，global-y `3aca30edeb9e91f72fd8cee29b3e5db2923e2caf8293b0b6c55ec28465b3d55b`，main `a90dbcdcec63d1edfc5f13900b13c8f221600f6f1c6cd53a80574f0e134f9b28`，IPC `004c58987cceeea3ab2f0cb7dfc14cb1c7befe27de146c7cf24102c04267d2f6`。纳入 H2 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **282 个**测试可执行项。

## H3 增量记录

| 场景 | 初始／传感器特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H3** | 2025-01-02 03:04:05 近圆 LEO；单位四元数；零设备命令；两路 DSS 自定义异号 mean/sigma；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H3-1000 原 ELF gold：state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`，main `2920f9c473f98cdb99e80592fffe36d894977b21ba774bbaa9269c19d897d822`，IPC `6b388b65446e9afda48e52ec7a6a00862f79577cd6228d9ea6c01f3ac9717330`。纳入 H3 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **284 个**测试可执行项。

## H4 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H4** | 替代四元数；固定初始轮速／角动量；前 9 次组合多轴轮扭矩、MTQ、推力器及惯量更新；其后零命令；`srand(1)` | PASS | PASS | PASS | 已接入 |

H4-1000 原 ELF gold：state `f5dad3b6b21f7d430585671ef63317d301c4d5218c209841f28e1d2729c1e5fd`，main `01fe5a86b301f072446ff5bc278654adb5a25b5ebfb93df14ad37ae48c7f0b1e`，IPC `a06616bb72dc89d4e9575fa42659bccf73d022d59a437636bc06fd4dab08db5f`。纳入 H4 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **286 个**测试可执行项。

## H5 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H5** | 近圆 LEO；单位四元数；持续 `sada_command_flag=1`；极端双轴角请求 `(1.0,-1.0)`；`srand(1)` | PASS | PASS | PASS | 已接入 |

H5-1000 原 ELF gold：state `15566885a8a123302f53a8a439cc915d2b793152d54b172ee7202d84f2692ea2`，main `d380041dcd6592337b28fca490b19c886f25f10b78cafbf1ff511ec9a8d0ea84`，IPC `b9c813ef31a0d404a65da150c80eb763642ed7cfc3de7982dd204128e6b1c25a`。纳入 H5 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **288 个**测试可执行项。

## H6 增量记录

| 场景 | 初始／环境特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H6** | 2024-02-29；阴影侧 GCI 位置；替代四元数；完整非零 20 维柔性状态；零设备命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H6-1000 原 ELF gold：state `f2cab04e4229844dfdd538a3f48e9101f05cedc006a44a53fa4fa130d8ff0615`，global-y `0eeecf135559df4b56010535d5c59daf3a1ed14613032e61e5f757f30c881d87`，main `59be09a989a4328d6619f1e020b1e505eeab4f0e90e5eece89aae1bda8ba73a6`，IPC `10cde4dd3d61e34def6d4b3c87fad3ecdb6bad56db047b268300f5798d662488`。纳入 H6 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **290 个**测试可执行项。

## H7 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---|---|---|---|
| **H7** | 近圆 LEO；替代四元数；六个分散非零柔性状态；非共线 SADA `(0.01,-0.01)`；SADA flag1 与惯量更新；`srand(1)` | PASS | PASS | PASS | 已接入 |

H7-1000 原 ELF gold：state `2af310a51d93524ab57ada3ec321a8761b0cda7d2725a04d77e8740df744f2b0`，main `949fee298b60dfa457024388743e98ad2d6473cdb7acc802150799d17012f949`，IPC `8fd160f129143a78ed454c3dc52731339e57de219c15ea1bc4c723691a1e3a96`。纳入 H7 后，`make clean && make selftest` 通过，`make -n selftest` 统计为 **292 个**测试可执行项。

## H8 增量记录

| 场景 | 初始／传感器特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H8** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；单位四元数；零设备命令；两路 DSS 的 `gaussian_noise_flag` 均清零；`srand(1)` | PASS | PASS | PASS | 已接入 |

H8 是对 H3“两路 DSS 自定义噪声”覆盖的独立有限补充：它保持相同类别的主状态传播输入，但令两路 DSS 的高斯噪声开关均为关闭状态。原 ELF 100 步与 1000 步的每个 gold 文件均已连续独立采集两次并逐文件 `cmp` 一致。恢复端随后在**每一步**逐块比较 `CoreDynamic` 前 `0x108`／264 B、main／544 B 与 IPC payload／3000 B，均为 bitwise PASS；此场景**未建立或比较 global `y[33]` gold**，不得将其表述为 global `y` 已验证。

| H8-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa` |
| main 输出 | 544,000 B | `e343f7469589ebcb72b59d36046b7c323502fe91951e3c0543a3be46b77d9586` |
| IPC payload | 3,000,000 B | `b495c13cf661cd9d7402fbb2e74b4f0d1bb7fc6dacef634bb77381aa9dbfdfe8` |

接入 H8 的 100／1000 步比较器后，已从空构建以默认严格 C11 标志完成 `make clean && make selftest`，结果通过。按 `make -n selftest` 的完整命令行展开统计，当前为 **588 行构建／执行命令**（对应 **294 个测试可执行项**）；原 ELF 在回归前后均保持 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H8 的适用边界。** 此证据仅覆盖已写明的 Linux x86-64、固定编译选项、初始条件、固定 `srand(1)`、零命令与两路 DSS 噪声标志关闭的有限轨迹。它证明这些观察块在该受控域内逐步 bitwise 一致；不构成任意输入、并发行为、其他平台／编译器或完整数学输入域的 100% 证明。本工程仅用于离线研究和仿真，不得用于飞行、控制、实体执行机构或安全关键用途。

## H9 增量记录

| 场景 | 初始／传感器特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H9** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；单位四元数；零设备命令；`DSS0.gaussian_noise_flag=1`、`DSS1.gaussian_noise_flag=0`；`srand(1)` | PASS | PASS | PASS | 已接入 |

H9 对 H8（两路 DSS 高斯噪声均关闭）与 H3（两路自定义 DSS 噪声）的有限覆盖作了单路状态分离：仅 DSS0 消耗随机序列并加入其既有高斯噪声模型，DSS1 保持噪声关闭。原 ELF 的 100 步与 1000 步 state、main、IPC gold 均连续独立采集两次、逐文件 `cmp` 一致。恢复端在每一步对 `CoreDynamic` 前 `0x108`／264 B、main／544 B 和 IPC payload／3000 B 比较，均为 bitwise PASS；本场景**未建立或比较 global `y[33]` gold**。

| H9-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa` |
| main 输出 | 544,000 B | `c6f62f261eaad6183fa923da64366bfcf585af9cfc3ab8cefee125b2777a0244` |
| IPC payload | 3,000,000 B | `51b29a6be9305269dcc1461bdf0622de46513e36cc0162fbebf555a93174305c` |

接入 H9 的 100／1000 步比较器后，已从空构建按默认严格 C11 标志运行 `make clean && make selftest` 并通过。`make -n selftest` 当前展开为 **296 个测试可执行项**（592 行编译／执行命令）。原 ELF 在回归前后仍为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H9 的适用边界。** 此证据仅覆盖已明确的 Linux x86-64、固定编译选项、固定初始条件、固定 `srand(1)`、零命令与 `DSS0=1`／`DSS1=0` 的高斯噪声标志组合。它证明上述三个观察块在该受控轨迹上的逐步 bitwise 一致，而不证明任意随机数实现、噪声参数、传感器组合、并发、平台、编译器或数学输入域上的等价性。本工程仅用于离线研究和仿真，不得用于飞行、控制、实体执行机构或安全关键用途。

## H10 增量记录

| 场景 | 初始／传感器特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H10** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；单位四元数；零设备命令；`DSS0.gaussian_noise_flag=0`、`DSS1.gaussian_noise_flag=1`；`srand(1)` | PASS | PASS | PASS | 已接入 |

H10 是 H9 的互补单路噪声状态：仅 DSS1 消耗既有高斯噪声随机序列，DSS0 保持噪声关闭；它与 H8 双关闭及 H3 双路自定义噪声共同覆盖两路 DSS 高斯噪声开关的四种有限开关组合。原 ELF 的 100 步与 1000 步 state、main、IPC gold 都连续独立采集两次且逐文件 `cmp` 一致。恢复端在每一步对 `CoreDynamic` 前 `0x108`／264 B、main／544 B 和 IPC payload／3000 B 比较，均为 bitwise PASS；本场景**未建立或比较 global `y[33]` gold**。

| H10-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa` |
| main 输出 | 544,000 B | `a80b4f83f3534d2b1a111bea7d4cc08b50d50cf3cc934c224d2089beb067fec1` |
| IPC payload | 3,000,000 B | `9f7e8a4520c38daa151faa7b711a87f4a417c0a2f0aaaf6fcea436afb0c0dc3f` |

接入 H10 的 100／1000 步比较器后，已从空构建按默认严格 C11 标志运行 `make clean && make selftest` 并通过。`make -n selftest` 当前展开为 **298 个测试可执行项**（596 行编译／执行命令）。原 ELF 在回归前后仍为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H10 的适用边界。** 此证据仅覆盖已明确的 Linux x86-64、固定编译选项、固定初始条件、固定 `srand(1)`、零命令与 `DSS0=0`／`DSS1=1` 的高斯噪声标志组合。它证明上述三个观察块在该受控轨迹上的逐步 bitwise 一致，而不证明任意随机数实现、噪声参数、传感器组合、并发、平台、编译器或数学输入域上的等价性。本工程仅用于离线研究和仿真，不得用于飞行、控制、实体执行机构或安全关键用途。

## H11 增量记录

| 场景 | 初始／动力学特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H11** | 2031-12-31 23:59:50；高偏心轨道（`a=26,560,000 m`、`e=0.65`）；第三四元数 `(0.5,0.5,0.5,-0.5)`；20 维反相柔性初态；零命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H11 在初始百步比较中只于步骤 68、73、85 的两路 `DSS_VALID` 遥测字段分叉；core、global `y[33]`、其余 main 与 IPC 字节均已一致。原 ELF反汇编表明 `UpdateSunSensorValidFlag` 在栈槽中构造基准向量 `(0,0,1)` 并将其传给两次 `vector2angle`。仅驻留调试副本内存的调用点跳板进一步记录到上述三步两通道的角度返回均为 `0.9491289345377069`、`1.021994114358845`、`1.0309815219893705`，均小于阈值 `pi/3`；同一运行期 `isEarthShadow` trace 返回均为 `0`。据此移除没有充分证据的 legacy-NaN 模拟，恢复端按量测相对 `(0,0,1)` 的实际夹角执行有效性判断。H11-100 随后四块逐步 bitwise PASS。

H11-1000 四块原 ELF gold 均独立连续采集两次并逐文件 `cmp` 一致：

| H11-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `d7743612eae0fa8e15127fdd94896249927c85c7eb47042cd409bcbd605f306c` |
| global `y[33]` | 264,000 B | `a1fabb17aca8130de168e05964d03525b0bd3134ba8b70d68f843163be55cc8b` |
| main 输出 | 544,000 B | `ab484fb2fb6f24fe8d578a20d1f20b943f01dd8cb8c928bc66731559c4b49e6b` |
| IPC payload | 3,000,000 B | `10dba40b7f4c3ab2dc8e25d8aa3a1e1ecfee07798d5ced9ff64dba43ae362271` |

默认严格 C11 恢复端在 H11 的 100／1000 每一步均比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，全部 0-byte mismatch。H11 两个比较器已接入 Makefile；从空构建执行 `make clean && make selftest` 通过，`make -n selftest` 展开为 **300 个测试可执行项**（600 行编译／执行命令）。原 ELF 在回归前后仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H11 的适用边界。** 该结论仅覆盖以上固定日期、轨道、四元数、柔性初态、命令、种子、Linux x86-64 平台与严格编译标志下的四块可观察行为。它不是任意日期、传感器参数、异常输入、并发、平台、编译器或数学输入全域的 100% 证明。本工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H12 增量记录

| 场景 | 初始／联合作用特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H12** | 2024-02-29 12:34:56；阴影侧 GCI 位置；替代四元数；完整非零 20 维柔性状态；零设备命令；`DSS0.gaussian_noise_flag=1`、`DSS1.gaussian_noise_flag=0`；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H12 交叉覆盖了 H6 的闰日阴影／完整柔性状态与 H9 的单 DSS0 高斯噪声开关：它不是单独维度的重复验证，而是阴影几何、全柔性传播和随机量测序列的联合有限轨迹。原 ELF 的 100 步和 1000 步 state、global `y`、main、IPC 四块均连续独立采集两次并逐文件 `cmp` 一致。恢复端在每一步均比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，全部 bitwise PASS。

| H12-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `f2cab04e4229844dfdd538a3f48e9101f05cedc006a44a53fa4fa130d8ff0615` |
| global `y[33]` | 264,000 B | `0eeecf135559df4b56010535d5c59daf3a1ed14613032e61e5f757f30c881d87` |
| main 输出 | 544,000 B | `27b161b70c55c0d5755c376a5fdeaa6f1b265e286ffd7931b8c79fa27605dd07` |
| IPC payload | 3,000,000 B | `8cf11eb7705df131b225dae299ba94d148ada7eb2a0971fe7a515ad830211103` |

H12 100／1000 比较器已加入 Makefile。从空构建执行默认严格 C11 `make clean && make selftest` 通过；`make -n selftest` 展开为 **302 个测试可执行项**（604 行编译／执行命令）。回归前后原 ELF 均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H12 的适用边界。** 该证据只覆盖列出的固定日期、阴影位置、姿态、柔性初态、DSS 噪声开关、种子、平台及严格编译标志下的四块观察范围。它不证明任意噪声参数、阴影几何、异常输入、并发、平台、编译器或数学输入全域上的等价性。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H13 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H13** | 2025-01-02 03:04:05；近圆 LEO；替代四元数 `(0.5,-0.5,0.5,0.5)`；完整交错非零柔性振幅；持续 SADA `(0.01,-0.01)` 指令及惯量更新；`srand(1)` | PASS | PASS | PASS | 已接入 |

H13 将替代姿态、完整柔性振幅分布和持续 SADA／惯量更新置于同一有限轨迹中。它与 H7 的稀疏柔性／非共线 SADA 联合路径不同，并补充检验了完整 20 维柔性状态与指令持续作用的长时域耦合。原 ELF 的 100 步与 1000 步 state、global `y`、main、IPC 四块均连续独立采集两次并逐文件 `cmp` 一致；恢复端在每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，全部 bitwise PASS。

| H13-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `7f21d30353c78c8a104d8e438f0e6f87013467420a7bea136d5d55c33d562b61` |
| global `y[33]` | 264,000 B | `11a87a3910ae7a247f05aaa0818ccbe00e510d058d61a100856eeba76f86859b` |
| main 输出 | 544,000 B | `875d112c51ed017e069ee78547353bf78639f22d94953825975ed431cf6454d2` |
| IPC payload | 3,000,000 B | `0290d1dc273f3930ae4d110b358b13b318823f531cef41e8f58e54e302417111` |

H13 100／1000 比较器已加入 Makefile。从空构建执行默认严格 C11 `make clean && make selftest` 通过；`make -n selftest` 展开为 **304 个测试可执行项**（608 行编译／执行命令）。原 ELF在回归前后均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H13 的适用边界。** 该证据只覆盖列出的固定时间、轨道、姿态、柔性初态、持续命令、种子、平台及严格编译标志下的四块观察范围。它不证明任意柔性参数、命令序列、异常输入、并发、平台、编译器或完整数学输入空间上的等价性。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H14 增量记录

| 场景 | 初始／动力学特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H14** | 2031-12-31 23:59:50；高偏心轨道（`a=26,560,000 m`、`e=0.65`）；替代四元数 `(0.5,-0.5,0.5,0.5)`；完整非零 20 维柔性状态；零命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H14 将 H0 的高偏心跨日动力学环境与 H2／H6 的替代姿态和全柔性初态组合为独立有限路径。原 ELF 100 步与 1000 步的 state、global `y`、main、IPC 四块均连续独立采集两次并逐文件 `cmp` 一致。恢复端在每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，全部 bitwise PASS。

| H14-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `1fa8ec2d30b4fb0ab1ab3041746bfd3491b72936ee36006a6d499a63f4697ead` |
| global `y[33]` | 264,000 B | `52e096393d9be5bced3d894a82228007dcea21e7dc6e51e091cdc3edf4a47a36` |
| main 输出 | 544,000 B | `c417b08f0c58fc8f1f8961ab0238a04b26fe09eac5151d02814b333368c9f826` |
| IPC payload | 3,000,000 B | `387f325a3b458381ab506133e121c9a859c26937769c07347d68f9ad6e630dac` |

H14 100／1000 比较器已加入 Makefile。从空构建执行默认严格 C11 `make clean && make selftest` 通过；`make -n selftest` 展开为 **306 个测试可执行项**（612 行编译／执行命令）。原 ELF在回归前后均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H14 的适用边界。** 该证据只覆盖列出的固定日期、轨道、姿态、柔性初态、命令、种子、平台及严格编译标志下的四块观察范围。它不证明任意高偏心参数、姿态、柔性参数、异常输入、并发、平台、编译器或完整数学输入空间上的等价性。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H15 增量记录

| 场景 | 初始／时间特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H15** | 2031-12-31 23:59:50 跨年时间边界；高偏心轨道（`a=26,560,000 m`、`e=0.65`）；第三姿态四元数 `(0.5,0.5,-0.5,0.5)`；零柔性、零命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H15 独立检验了时间边界传播下的轨道、环境与传感器更新路径。它使用第三姿态四元数和零柔性状态，因而不同于 H11 的第三四元数／反相柔性路径，也不同于 H0／H14 的替代姿态组合。该短时基线传统上只观察三块，因此 H15 的长时域金标与逐步比较范围为 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B；**没有为 H15 新增 global `y` gold，不能将本场景表述为已核验 global `y`**。

为扩展共享比较器，新增 `DP_ORBIT_TIME_BOUNDARY_HUNDRED_STEP` 与 `DP_ORBIT_TIME_BOUNDARY_THOUSAND_STEP` 优先分支；两个包装器同时定义 `DP_ORBIT_TIME_BOUNDARY`，从而 gold／步数和相应的初始时间、轨道、姿态、随机种子路径同步选择，避免把测试装配差异误判为恢复库错误。

| H15-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `dd04631d8912590010eed11aa5415ee663aa7f73ca438f7b920f5ac5246fa028` |
| main 输出 | 544,000 B | `828ada1d4872a81636313b903903a29569cf004d0c536266245db89356861d67` |
| IPC payload | 3,000,000 B | `456ff515ada45c073a33a71fdb8b66d6a672c38165fa0650ba3dbd45ad1b0e23` |

H15 100／1000 比较器已加入 Makefile。从空构建执行默认严格 C11 `make clean && make selftest` 通过；`make -n selftest` 展开为 **308 个测试可执行项**（616 行编译／执行命令）。原 ELF在回归前后均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H15 的适用边界。** 该证据只覆盖列出的固定跨年时刻、轨道、姿态、零柔性、命令、种子、平台及严格编译标志下的三个观察块。它不证明 global `y`，也不证明任意日期、历法边界、轨道参数、姿态、异常输入、并发、平台、编译器或完整数学输入空间上的等价性。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H16 增量记录

| 场景 | 初始／轨道特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H16** | 2024-02-29 12:34:56；普通近地轨道（`a=7,078,137 m`、`e=0.05`）；第三姿态四元数 `(0.5,0.5,-0.5,0.5)`；零柔性、零命令；`srand(12345)` | PASS | PASS | PASS | 已接入 |

H16 覆盖闰日普通近地轨道、第三姿态及非零三轴位置／速度传播的组合。该短时基线传统上只观察三块，故 H16 的长时域金标与逐步比较范围是 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B；**没有建立 global `y` gold，不可将 H16 表述为已核验 global `y`**。

共享比较器新增 `DP_LEAP_DAY_LEO_HUNDRED_STEP` 与 `DP_LEAP_DAY_LEO_THOUSAND_STEP` 优先分支。两个包装器同时定义 `DP_LEAP_DAY_LEO`，保证百步／千步 gold 与既有闰日时间、轨道、姿态、种子初始化同步选择。

| H16-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `905b8e793ac0016b83e7cd5da0c98361755b897a8231c480ed84133dc863082b` |
| main 输出 | 544,000 B | `4c781c13dfcbc104792dcc4130fd350b5b108b0c468679d9cfcc5f9ce126050d` |
| IPC payload | 3,000,000 B | `935c7074e9039d00c84c74197ccf3e02174209bb99fe0576e896cecbb973e0e2` |

H16 100／1000 比较器已加入 Makefile。从空构建执行默认严格 C11 `make clean && make selftest` 通过；`make -n selftest` 展开为 **310 个测试可执行项**（620 行编译／执行命令）。原 ELF在回归前后均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H16 的适用边界。** 该证据只覆盖列出的固定闰日时刻、近地轨道、姿态、零柔性、命令、种子、平台及严格编译标志下的三个观察块。它不证明 global `y`，也不证明任意日期、轨道、姿态、异常输入、并发、平台、编译器或完整数学输入空间上的等价性。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H17 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H17** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；单位四元数；三轴初始角速度 `(0.001,-0.002,0.003)`；零柔性状态；全程零设备命令；默认传感器配置；`srand(1)` | PASS | PASS | PASS | 已接入 |

H17 将既有的单位四元数／近圆 LEO 基线扩展为**全程**默认设备控制和零柔性状态的 100／1000 步有限轨迹，以区别于 H1 的前九次组合设备／惯量命令路径以及 DSS 开关发生改变的 H8-H10。原 ELF 的 H17-100 与 H17-1000 state、main、IPC 三块均连续独立采集两次并逐文件 `cmp` 一致。恢复端在每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B，全部为 0-byte mismatch；**H17 未建立或比较 global `y[33]` gold，不能称为该块已核验。**

| H17-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa` |
| main 输出 | 544,000 B | `32c5602300c873aeffea4ff2b6335d384133ef2e07a5dc4d67f065b0fd4e9e64` |
| IPC payload | 3,000,000 B | `39ca467a6704401633a7944129e5fa95c7a5d8f4195d188f095ebbfd69969892` |

H17 两个比较器已加入 Makefile。从空构建按默认严格 C11 选项执行 `make clean && make selftest` 已通过；`make -n selftest` 展开为 **312 个测试可执行项**（624 行编译／执行命令），日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h17_long_horizon_gates.log`，其中 `PASS` 行计数为 303、错误模式计数为 0。回归前后原 ELF 均为 mode `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H17 的适用边界。** 此证据仅覆盖以上固定日期、轨道、初始状态、默认传感器配置、固定 `srand(1)`、全程零命令、Linux x86-64 平台和严格编译标志下的三个列明观察块。它不证明 global `y`，也不构成任意传感器参数、随机数实现、命令序列、异常输入、并发、平台、编译器或数学输入全域的 100% 行为等价证明。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H18 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H18** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；四元数 `(0.5,0.5,-0.5,0.5)`；零柔性；预置 RWheel0 `omega=628.4185307179587`、角动量 `7.981915339118876`；`srand(1)`；前九次调用按固定脉冲序列覆盖轮扭矩、MTQ、推力器使能及惯量更新，后续零命令 | PASS | PASS | PASS | 已接入 |

H18 选择先前仅有十步 state／main／IPC 与终态执行机构快照的多执行机构候选，因其设备命令时间表不同于 H1 的组合控制路径及 H17 的全程零命令路径。短时文件名中的 `global_y` 原先并不代表实际 global `y` 观察；本轮在原 ELF调试执行副本的每次 `dyn_main` 后，显式增加 `&y` 的 264 B dump，并在恢复端增加同一时刻的逐步比较。因此 H18 的长时域观察范围为 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，外加最终 `RWheel`／`MTQ`／`Thruster` 及其四个三元向量快照。

H18-100 与 H18-1000 各自的四个逐步流和七个最终执行机构文件均连续独立采集两次、逐文件 `cmp` 一致。恢复端在 100／1000 的每个步骤对四个流逐字节比较均为 0-byte mismatch；终态执行机构快照亦全部 PASS。

| H18-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `df72bb03261aff0c9c8fcc90aca2193d4ec386e315b29c473fa840a0ed670059` |
| global `y[33]` | 264,000 B | `be8a0ffb03f6515e17c53b9b3101522d1bd835defc666fd5926be193977fba47` |
| main 输出 | 544,000 B | `1c82c5c6eb78c5f0ba0fb7678ff729a047c824da80ac89bec4873286e52e1092` |
| IPC payload | 3,000,000 B | `38b6fa8c5dbf1d9be92b7ec8e67f775f898f2af87c67d3067138c03f64ec38ab` |
| 最终 RWheel／MTQ／Thruster 及四个推力器向量 | 480 B + 336 B + 176 B + 4 × 24 B | 7 个文件均已双采集并纳入终态位级比较 |

H18 的百步／千步比较器已加入 Makefile。按默认严格 C11 选项从空构建执行 `make clean && make selftest` 已通过；`make -n selftest` 展开为 **314 个测试可执行项**（628 行编译／执行命令），日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h18_long_horizon_gates.log`，其中 `PASS` 行计数为 305、错误模式计数为 0。原 ELF 在回归前后仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H18 的适用边界。** 此证据仅覆盖以上固定日期、轨道、姿态、轮预置、命令脉冲时间表、种子、Linux x86-64 平台和严格编译标志下的四个逐步观察块及列明终态设备快照。它不构成任意设备参数、命令序列、传感器配置、异常输入、并发、平台、编译器或数学输入全域的 100% 行为等价证明。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H19 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H19** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；四元数 `(0.5,0.5,-0.5,0.5)`；零柔性；H18 同样的 RWheel0 预置；传感器 `srand(1)`；命令由种子 `0x13579bdf`、`state=state*1664525+1013904223 (mod 2^32)` 的 LCG 在每步生成 | PASS | PASS | PASS | 已接入 |

H19 是组合／固定种子随机差分计划的首个批次。它不使用无界浮点 fuzz；而是把 LCG 的固定状态位映射到有限有效命令集：一个必选轮通道及符号、可选第二轮通道、可选一个 MTQ 通道及符号、可选 SADA 开关与双轴离散目标、推力器使能和惯量更新。原 ELF GDB probe 与恢复端 C 比较器都实现同一递推式、种子、字段映射和离散数值表，避免把输入装配差异误判为恢复逻辑问题。

H19-100 和 H19-1000 的每组四个逐步流与七个最终执行机构快照均连续独立采集两次并逐文件 `cmp` 一致。恢复端在每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，均为 0-byte mismatch；最终 RWheel／MTQ／Thruster 与四个推力器三元向量快照也全部 bitwise PASS。

| H19-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `36eda1837c9265dd81d20869b0f444ff9dd5c3fac621b7a282ab117ad4861cc1` |
| global `y[33]` | 264,000 B | `448fe43cf847b017913ff5c50c843d15c0bf38bd242632faa3ad6d1eb427bb68` |
| main 输出 | 544,000 B | `f6bf82cd88cd7305b3204d6c7fae7e6136bbdcde8269e89a1b334d2f4897317f` |
| IPC payload | 3,000,000 B | `57c8d506f38613beb59f20674cb8d7eb4b7d1a232f20401a3f0f231613043739` |
| 最终 RWheel／MTQ／Thruster 及四个推力器向量 | 480 B + 336 B + 176 B + 4 × 24 B | 7 个文件均已双采集并纳入终态位级比较 |

H19 两个比较器已加入 Makefile。当前 Linux x86-64、当前运行时库、默认严格 C11 选项下，从空构建执行 `make clean && make selftest` 已通过；`make -n selftest` 展开为 **316 个测试可执行项**（632 行编译／执行命令），日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h19_long_horizon_gates.log`，其中 `PASS` 行计数为 307、错误模式计数为 0。原 ELF 在回归前后仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H19 的适用边界。** H19 证明的是当前平台、当前编译／运行时环境、这个 LCG、这个种子、这个离散有效命令映射及所列观察块下的有限序列行为一致。它不覆盖其他发生器、种子、离散表、连续命令值、异常输入、并发或未建立 gold 的内部对象，也不构成对当前平台任意输入的数学穷尽证明。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H20 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H20** | 2025-01-02 03:04:05；近圆 LEO（`a=7,000,000 m`、`e=0.01`）；第三四元数 `(0.5,0.5,-0.5,0.5)`；H13 的完整正负 20 维柔性模板；DSS0 `gaussian_noise_flag=1`／DSS1=0；传感器 `srand(12345)`；H19 的 LCG 种子 `0x13579bdf` 有效多执行机构命令序列；RWheel0 预置 | PASS | PASS | PASS | 已接入 |

H20 是 H12／H13／H19 三个已独立取证维度的交叉场景：完整 20 维柔性初态、单路 DSS 高斯噪声开关和固定种子有效设备命令时间表首次在同一长时域流程中出现。柔性值、DSS 开关偏移、传感器种子与 LCG 命令映射均来自已有成功探针；本轮只组合已审计的有限有效输入，不采用无界浮点 fuzz。

H20-100 与 H20-1000 的四个逐步流和七个最终执行机构快照均连续独立采集两次并逐文件 `cmp` 一致。恢复端在每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B，全部为 0-byte mismatch；最终 RWheel／MTQ／Thruster 与四个推力器三元向量快照也全部 bitwise PASS。

| H20-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `577f6f398d0fc182513b9c3fbf42e98e87b2c1c93129afa18bdd5314fcbeeb23` |
| global `y[33]` | 264,000 B | `d8b3b8dcdc389bf9b59e1c76fa4b75f5d95a03863b876d941bcc104b3302d849` |
| main 输出 | 544,000 B | `24509dd5f88086b705e33a03cfc976383c062f90d317e8366b17911126849417` |
| IPC payload | 3,000,000 B | `54f31020cbcb8514e0404c3853195eeb8c5ec44146237829fbe2823eceda6803` |
| 最终 RWheel／MTQ／Thruster 及四个推力器向量 | 480 B + 336 B + 176 B + 4 × 24 B | 7 个文件均已双采集并纳入终态位级比较 |

H20 两个比较器已加入 Makefile。当前 Linux x86-64、当前运行时库、默认严格 C11 选项下，从空构建执行 `make clean && make selftest` 已通过；`make -n selftest` 展开为 **318 个测试可执行项**（636 行编译／执行命令），日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h20_long_horizon_gates.log`，其中 `PASS` 行计数为 309、错误模式计数为 0。原 ELF 在回归前后仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H20 的适用边界。** H20 仅证明当前平台、当前运行时／严格编译条件、列明柔性模板、DSS 开关、传感器随机种子、LCG 命令种子及其离散映射下的四块逐步输出和列明终态设备快照一致；它不穷尽其他柔性幅值、传感器参数、随机种子、连续命令值、异常输入、并发或未观察对象。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H21 增量记录

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H21** | 保持 H20 的 2025-01-02 03:04:05 近圆 LEO、第三四元数 `(0.5,0.5,-0.5,0.5)`、完整正负 20 维柔性模板、DSS0 `gaussian_noise_flag=1`／DSS1=0、`srand(12345)` 与 RWheel0 预置；设备命令 LCG 种子改为 `0x2468ace1`，递推 `state=state*1664525+1013904223 (mod 2^32)` | PASS | PASS | PASS | 已接入 |

H21 在不改变已验证命令字段映射和值集合的前提下，以第二个固定 32 位 LCG 种子形成独立的 100／1000 步多设备输入时间序列。原 ELF 仅通过 `analysis/debug_runner/DynamicPackage.exec_copy` 执行；H21-100 与 H21-1000 各产生 11 个 gold 文件，均连续独立采集两次并逐文件 `cmp` 一致。恢复端每一步比较 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B；最终 RWheel／MTQ／Thruster 和四个推力器三元向量快照也均为 0-byte mismatch。

| H21-1000 原 ELF gold 块 | 文件大小 | SHA-256 |
|---|---:|---|
| state／CoreDynamic 前 `0x108` | 264,000 B | `74fdd1de1069bdeaf2acf52ca92c5cadabb75d8d0fa5de221b5114a501aa83b7` |
| global `y[33]` | 264,000 B | `f1475a022cc2643dee5aef9afa3f56fba787032b62cc3a9b7cab158ba454f4fe` |
| main 输出 | 544,000 B | `404708340fb0823fae6147434e0201bd461d647a9bd3f2325d38b6aca3f7a760` |
| IPC payload | 3,000,000 B | `36e50182c2b304a0ce5c0abc6ab33fa50abb0802cff6b2e3ba1d5a37cf1f6f2f` |
| 最终 RWheel／MTQ／Thruster 及四个推力器向量 | 480 B + 336 B + 176 B + 4 × 24 B | 每个步长各 7 个文件，均已双采集并纳入终态位级比较 |

H21 百步比较器产生 411 条零字节比较记录，千步比较器产生 4,011 条；两者均输出 `original-ELF compare: PASS (bitwise)`。H21 两个比较器已接入默认门禁；从空构建以默认严格 C11 选项执行 `make clean && make selftest` 通过，`make -n selftest` 展开为 **320 个测试可执行项**（640 条编译／执行命令），日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h21_long_horizon_gates.log`，其中 PASS 行计数为 311、错误模式计数为 0。原 ELF 在 H21 取证前后均保持 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **H21 的适用边界。** H21 仅证明当前平台、当前运行时／严格编译条件、列明柔性模板、DSS 开关、传感器随机种子、第二 LCG 命令种子及其离散有效映射下的四块逐步输出和列明终态设备快照一致；它不穷尽其他 LCG 种子、连续命令值、传感器参数、异常输入、并发或未观察对象。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。


## H21 超长时域增量记录（10,000／100,000 步）

H21 保持前述第二 LCG 种子 `0x2468ace1`、2025-01-02 03:04:05 近圆 LEO、第三四元数、完整正负 20 维柔性模板、DSS0 噪声开启／DSS1 关闭、`srand(12345)` 及 RWheel0 预置。在不改变命令字段映射和值集合的前提下，按 `state=state*1664525+1013904223 (mod 2^32)` 继续生成设备命令。

| H21 长度 | 原 ELF 执行位置 | 双采集 | 主轨迹逐步观察 | 终态观察 | 恢复端结论 |
|---:|---|---|---|---|---|
| 10,000 步 | 仅 `analysis/debug_runner/DynamicPackage.exec_copy` | 11 文件、两次逐文件 `cmp` PASS | state 前 264 B、global `y[33]` 264 B、main 544 B、IPC payload 3000 B | RWheel、MTQ、Thruster 及四个推力器三元向量 | 40,011 条 0-byte 记录，bitwise PASS |
| 100,000 步 | 仅 `analysis/debug_runner/DynamicPackage.exec_copy` | 11 文件、两次逐文件 `cmp` PASS | state 前 264 B、global `y[33]` 264 B、main 544 B、IPC payload 3000 B | RWheel、MTQ、Thruster 及四个推力器三元向量 | 400,011 条 0-byte 记录，bitwise PASS |

10,000 步四块文件 SHA-256：state `b259860daabc172973ecfa9c35a26a02ae3b8455b69d1762ae2b3ce6439df4d9`，global `y` `f8d7363c852b5e765ce25a19c0f9ddc66b96a4d1b18228ae5d9a752b10e8fdb9`，main `5e7019d0f2ceaa9ab3404c29a9ed0999cfa7d9899cfd1bb0566dd4f14c619d5e`，IPC `6f85c20c67b8d429efb3ec4dec2a21bc592f0539678e418239a895e87cc33cd8`。

100,000 步四块文件 SHA-256：state `8bc705db223fda6627bef87e4873c001eb4b7627cb6c26f5928292478a1708f8`，global `y` `a2ca5a4a914ba34406a21d67e4fe1d52d91f5448677c530f02f65ce463e8a2ad`，main `b022873d1b3411d72dc643e51154e345665389837705f4b0482297c9c0af4c1b`，IPC `8b69c88e0e15896793808ddf4682a78c34ded3e14bfe364b12037e0b23eeac8b`。

两个超长时域比较器已加入默认严格门禁。从空构建执行 `make clean && make selftest` 成功；`make -n selftest` 展开 322 个测试可执行项／644 条编译执行命令，PASS 行计数 313、错误模式计数 0。H21 100,000 步仅证明当前平台、当前运行时／严格编译条件、列明初态、传感器随机种子和第二 LCG 离散命令时间序列下的上述观察块一致；它仍不穷尽其他初态、其他随机种子、连续命令值、异常输入、并发或未观察对象。本工程仅用于离线研究和仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。
