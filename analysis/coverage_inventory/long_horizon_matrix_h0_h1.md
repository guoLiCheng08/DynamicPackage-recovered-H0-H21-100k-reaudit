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

## H22 增量记录：补齐既有 H15 的 Global `y`

H22 是本轮取证工作标签，**不是第 23 个独立输入场景**。它保持既有 H15 的跨年高偏心输入合同（2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、姿态 `(0.5,0.5,-0.5,0.5)`、固定非零角速度、零设备命令与 `srand(12345)`），仅补齐此前 H15-100／H15-1000 未观察的 global `y[33]`。

| 既有场景 | 增加的观察块 | 长度 | 原 ELF 双采集 | 恢复端逐步比较 | global `y` SHA-256 |
|---|---|---:|---|---|---|
| **H15** | global `y[33]`（264 B/步） | 100 步 | 两次四块文件逐文件 `cmp` PASS；state/main/IPC 同既有 H15 gold | 四块逐步、0-byte mismatch、PASS | `70b28a78d1d38cf28115173fafb3fa585a97aabafc5655bfb9c15ebc6f70f7d2` |
| **H15** | global `y[33]`（264 B/步） | 1000 步 | 两次四块文件逐文件 `cmp` PASS；state/main/IPC 同既有 H15 gold | 四块逐步、0-byte mismatch、PASS | `bf376ad8f7c5a57f9662acf246486058c18519e6d34d0670d4d7babcac65e7cf` |

新增 1,100 个内部状态步骤、290,400 B 独有 global `y` 观察。两个 H22 包装器以共享 H15 比较器的编译期开关启用 global-y 读取和每步 bitwise 比较，均已接入默认 `selftest`，并另提供 `make check-h15-global-y`。空构建执行 `make clean && make selftest` 展开 **324 个测试可执行项**，输出 315 条 PASS、非零字节差异 0、显式失败 0；完整日志为 `analysis/coverage_inventory/full_clean_selftest_with_h22_h15_global_y_gate.log`。

> **H22 的适用边界。** H22 只将 H15 的已定义输入路径从三块观察提升为四块观察，并不增加新的随机种子、命令序列或输入初态，也不证明任意输入或平台上的全域等价。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H23 首批增量记录：补齐既有 H16/H17 的 Global `y`

H23 为覆盖补齐工作标签，**不是第 24 个或第 25 个独立输入场景**。本批维持既有 H16（闰日普通近地轨道、替代姿态、固定非零角速度、零柔性/零命令、`srand(12345)`）与 H17（近圆 LEO、单位四元数、三轴初始角速度、零柔性/零命令、`srand(1)`）的输入合同，仅补写并比较 `global y[33]`。

| 既有场景 | 增加的观察块 | 长度 | 原 ELF 双采集与既有三块交叉核验 | 恢复端逐步比较 | global `y` SHA-256 |
|---|---|---:|---|---|---|
| **H16** | global `y[33]`（264 B/步） | 100 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `757284c39a6ab29e9cbd0fdde7d5903592464d401c6641883eb4027ad699fb7f` |
| **H16** | global `y[33]`（264 B/步） | 1000 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `8f7efc94caeaa0e222cb0e98eac67bf1c7cf49332ec0ec3f38dbe36564678e57` |
| **H17** | global `y[33]`（264 B/步） | 100 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `e40e9c2201c6efd5952c91169dfe22003b93e7577505803d55ba10db89535d25` |
| **H17** | global `y[33]`（264 B/步） | 1000 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `2e57874a6684f5283caf8dbc7cf2bcae6a34752db62c746d477bd8abf00c6e1c` |

H16/H17 的既有长期比较器均已直接扩展为四块逐步差分；新增 `make check-h16-h17-global-y`。从空构建执行此门禁得到 4 条 PASS 与 8,800 条零字节四块记录；再执行 `make clean && make selftest` 展开 324 个测试可执行项，315 条 PASS、非零字节差异 0、显式失败 0。global `y` 覆盖因此升至 15/22，余下未覆盖的既有场景为 H3/H4/H5/H7/H8/H9/H10。

> **H23 的适用边界。** H23 只将两条既有受控路径从三块观察扩展为四块观察，不新增随机种子、命令时间表或输入初态，不能被解释为全域或跨平台等价证明。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H24 增量记录：补齐既有 H4/H5/H7 的 Global `y`

H24 是覆盖补齐工作标签，**不是新增的独立输入场景**。它分别维持 H4 的预置飞轮与前九步多执行机构命令、H5 的持续饱和 SADA 请求，以及 H7 的非零柔性/非共线 SADA flag1/惯量更新合同，仅将此前三块观察扩展为 global `y[33]` 第四块。

| 既有场景 | 增加的观察块 | 长度 | 原 ELF 双采集与既有三块交叉核验 | 恢复端逐步比较 | global `y` SHA-256 |
|---|---|---:|---|---|---|
| **H4** | global `y[33]`（264 B/步） | 100 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `c00b7a56487047576648492a4c65924330bddd49711422f35cde48b6dae80b2b` |
| **H4** | global `y[33]`（264 B/步） | 1000 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `b903d27b03184a7cc1da8711c84e2a611138a2cda125034c929608ef74e47169` |
| **H5** | global `y[33]`（264 B/步） | 100 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `6431bb6f8e98bdf3a284eb22e8671a77411dc834fb8d20f4e0a47c4096d127dd` |
| **H5** | global `y[33]`（264 B/步） | 1000 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `33717097afa1209425ade3e81ffe66a568dc4d18587562db7a77ae19084ccc1e` |
| **H7** | global `y[33]`（264 B/步） | 100 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `a9e37ea232f7e3f33459570a641c74f68e1ee5a08597a549ad4f3bd5a03fd22a` |
| **H7** | global `y[33]`（264 B/步） | 1000 步 | 四块 first/second `cmp` PASS；state/main/IPC 同既有 gold | 四块逐步、0-byte mismatch、PASS | `5d303542abaa083636856b536de8df7f67e0508c4b884bfde21b1f99421ec200` |

H4/H7 的共享长期比较器和 H5 的独立比较器均已扩展为四块逐步差分；新增 `make check-h4-h5-h7-global-y`。空构建运行该门禁得到 6 条 PASS 与 13,200 条零字节四块记录；随后 `make clean && make selftest` 展开 324 个测试可执行项、315 条 PASS、非零字节差异 0、显式失败 0。global `y` 覆盖升至 18/22；仅 H3/H8/H9/H10 的 DSS 噪声组合仍未建此内部块 gold。

> **H24 的适用边界。** H24 只扩展既有有限路径的观察块，不新增命令、随机种子或初态，也不证明全域或跨平台等价。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H26 新增独立场景：高偏心跨年、完整柔性与有效多执行机构命令

| 场景 | 初始／命令特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---:|
| **H26** | 2031-12-31 23:59:50；高偏心轨道 `a=26,560,000 m`、`e=0.65`；高偏心 caller-state 位置/速度；替代姿态；完整 20 维交错柔性状态；DSS0 噪声开/DSS1 关；`srand(12345)`；RWheel0 预置；LCG 种子 `0x2468ace1` 的有限有效轮/MTQ/SADA/推力器/惯量更新命令 | PASS | PASS | PASS | 已接入 |

H26 是 H0/H15 高偏心跨年环境与 H21 完整柔性、传感器和有效多执行机构 LCG 命令的新增联合路径，并非对原有场景的 global-y 补洞。原 ELF 的 100/1000 步均采用 first/second 两次独立采集；四个逐步块和七个终态执行机构快照共 11 个文件在每种长度均逐文件 `cmp` 一致。恢复端逐步四块和终态快照均为 0-byte mismatch；`make check-h26-high-ecc-cross-command` 与完整严格 selftest 均通过。

| H26 长度 | caller-state SHA-256 | global y SHA-256 | main SHA-256 | IPC SHA-256 |
|---:|---|---|---|---|
| 100 | `10e57af9570bcd95b9bf7480d7397e4f620731585b5d2999ac3a67c568247ddc` | `9fc186a44143cb049051e981164814406d42034dc2314f2b9f89359143939eaf` | `afc60f9f2fd25092fc97d00cfcf0a932c1cfd85b5a56ca1e62cb19de3593bd65` | `3ed5f9338c9b7897af021f673f7ff6104a14846de33c4f13ef922e8f65096ae6` |
| 1000 | `58e319eda7f7ed3d77cb885975b70ece96352c9e554f3415d2890fabc1112109` | `4082236cd2ab73dcfdb85e708c32d7be2cd56d927831a794159e282b78cbde92` | `d59d1473c1ad2325b1a3550c927da6a27d665c158bfb9c8ee6ee345aaf76e8d8` | `0e737bcd211261b5792686265c3e9e81d73b3b80c73b51b850f55240d57a0f87` |

> **H26 的适用边界。** H26 仅证明列明的高偏心根数、时间、state、柔性模板、DSS 开关/种子、飞轮预置和有限有效命令映射，在当前 Linux x86-64 和严格 C11 构建下的 bitwise 一致性；不证明任意轨道、连续命令、异常、并发、其他平台或未观察对象的全域等价。本工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H28：旧式 `dyn_main_array` 数组 ABI（新增独立入口，2026-08-25）

H28 不新增结构化 `dyn_main` 初态场景，而是验证公开旧式数组 ABI：30 个 `double` 初始化数组、16 个 `double` 设备命令数组、0x1e8 B legacy output 与 0x200 B legacy state。命令同时覆盖轮扭矩、MTQ、SADA、推力器与惯量更新。原 ELF 100/1000 步 output/state 两块均 first/second 双采集并 `cmp` PASS；恢复端最初在第 1 步发现分叉，随后以原 ELF `dyn_init_array` 及首帧后设备快照定位并补齐 SADA、前三 MTQ 限幅/安装轴与 Thruster 静态默认值。修复后 100 步 200 条、1000 步 2000 条 legacy 缓冲比较均为 0-byte mismatch；`make check-h28-dyn-main-array` 两条 PASS，空构建完整 selftest 为 321 条 PASS、非零差异 0、显式失败 0。该入口证据不改变 24 个结构化主场景计数，但确认主 `dyn_main` 通过不能自动外推到数组 ABI。

| 长度 | 原 ELF output SHA-256 | 原 ELF state SHA-256 | 恢复端结论 |
|---:|---|---|---|
| 100 | `94eed78188a46a359e560a4758325eacc3d06b5c87724f61cf0e8b66f370feb5` | `54b0af090a09a37e8a072badf22e0899b5e21067243b1dd8bd7c4f2c43832a5e` | bitwise PASS |
| 1000 | `ed7a94197228f313dfe7c131926073c5e52effeac8e7590c32131e6c09ae2776` | `a2eb4eb72813988f6511f47a7d2efcc79ec89b1572c5874c88256f040ab47694` | bitwise PASS |

相关证据：`analysis/coverage_inventory/h28_dyn_main_array_abi_diagnostic_20260825.md`。

## H29：`getDeskCommand` 连续共享 IPC 入口（新增独立入口，2026-08-25）

H29 验证 `getDeskCommand` 的连续共享输入读取路径。每步受控写入 float 索引 64/65/66 与 byte 索引 20–36，连续调用后逐步比较四个 desk flag（4 B）、`DynamicDllInit` 初始条件的三轴 float 尾部（12 B）及 DRC 状态（112 B）。100/1000 步三块均 first/second 原 ELF 双采集并逐文件 `cmp` PASS；恢复端分别建立 300/3000 条逐步比较，均为 0-byte mismatch。`make check-h29-get-desk-command` 通过两条比较器，空构建完整 selftest 为 323 条 PASS、非零差异 0、显式失败 0。该证据覆盖受控无竞争共享映射，不覆盖锁失败、并发竞争、IPC 建立失败或任意索引。

| 长度 | flags SHA-256 | init-tail SHA-256 | DRC SHA-256 | 恢复端结论 |
|---:|---|---|---|---|
| 100 | `790c0c6651e15a3d83988fd9a460ca8baac644591db1dcbb72fcd4873a2777e7` | `d7a5057ef21932cc628e20f4c8e3a3c43d528aeeecfa471e6dd898ed3378582a` | `744a5770a2e1206655a3ef3e5816ab93b56f9503bc4dee3e81bd48a65e27a5fd` | bitwise PASS |
| 1000 | `3acf1272ce18e8a67b80e637bee5340159568a0752880744bdceab0e033b43bc` | `6f422a68da2337704769912dedb0aaaacb9789fd1229c7bb34df9275d77b8d58` | `d69989f689f4890b56f6920ad4a70493e6d7b3ba9610c2fa8fada7731d9901a1` | bitwise PASS |

相关证据：`analysis/coverage_inventory/h29_get_desk_command_continuous_result_20260825.md`。

## H30：离线启动链 `DynamicDllInit → getDeskCommand → dyn_init → dyn_main`（新增主路径，2026-08-25）

H30 将固定共享 IPC 启动输入经 `getDeskCommand` 写入 `DynamicDllInit` 初始条件，再调用 `dyn_init` 并连续运行零命令 `dyn_main`。它是此前 H28（数组 ABI）、H29（共享 IPC）和主传播场景之外的端到端启动路径。100/1000 步均对 CoreDynamic 264 B、global `y[33]` 264 B、main 544 B 与 IPC 3000 B 执行原 ELF first/second 双采集和恢复端逐步差分。原 ELF 每种长度四块均 `cmp` PASS；恢复端分别输出 400/4000 条 0-byte 比较记录。`make check-h30-startup-chain` 两条 PASS，空构建完整 selftest 为 325 条 PASS、非零差异 0、显式失败 0。

| 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 | 恢复端结论 |
|---:|---|---|---|---|---|
| 100 | `8161ed292012760acc5bf24ba19ee4fcde5312b8d39b93c740480989ab44bf88` | `b8281273993e20dbcca289f718617d15a066c71fb49f26adff378440a9e82a16` | `bf2e6ae0d311b2c1e3692358b83cb1c6a76b638e70282182b2680322eac5c380` | `d1902c857e91ae50eec6ac917d4b447d36eeca58c824e0dfe37ef6cc042a11b4` | bitwise PASS |
| 1000 | `f31c67d609afcbede6e29df6fcd7e0778dd0dbcbe85fa8d8eda8eeeb74eb07a2` | `ede1cba7e6b84638802060e2c0e20f38a579739c8b2f3d44d32e1f9e875a1c45` | `fd499a5f4618cc5b1f2b714c305659450446ac705d8ce016308ee45a849745cf` | `c4fa97525a16ed46f1723f56ed5204525daa101cbd4dd79401616f7043b8ebf6` | bitwise PASS |

相关证据：`analysis/coverage_inventory/h30_startup_chain_result_20260825.md`。该入口仅在受控无竞争共享映射和离线仿真范围内验证；不得外推到真实飞行、控制或安全关键用途。

## H31：`getDeskCommand` 共享 IPC 生命周期（新增公开入口状态序列，2026-08-25）

H31 覆盖固定无竞争共享 IPC 的 `init_shared → getDeskCommand → close_shared → init_shared → getDeskCommand` 序列。受控输入为 float 索引 64/65/66 的 `(6.25,-7.5,8.75)` 与 byte 索引 20–36 的 `0xa0..0xb0`。两个阶段分别比较 desk flags 4 B、`DynamicDllInit` 初始角速度尾部 12 B 和 DRC 112 B；first/second 原 ELF 双采集的六个文件全体 `cmp` PASS。恢复端六块均 0-byte mismatch，`make check-h31-get-desk-lifecycle` PASS，空构建完整 selftest 为 326 条 PASS、非零差异 0、显式失败 0。该结论只覆盖对象未 unlink、`shm_open/mmap` 成功且无竞争读写的固定离线合同；不覆盖权限、锁、资源耗尽或并发失败路径。

| 阶段 | flags SHA-256 | init-tail SHA-256 | DRC SHA-256 | 恢复端结论 |
|---|---|---|---|---|
| 打开后 | `a57d29e1bf3bda74302c8f8148977cab1499a1c3f239146a75d185b6f25bf9e1` | `f22b426ff83c22af9814c35e460e550fe1b48dfd729506003804807041138c13` | `5b1fc66c257e31114be672fe96c9686633c3dd49078d386fa429a4eb9d2cb5e1` | bitwise PASS |
| 重开后 | `a57d29e1bf3bda74302c8f8148977cab1499a1c3f239146a75d185b6f25bf9e1` | `f22b426ff83c22af9814c35e460e550fe1b48dfd729506003804807041138c13` | `5b1fc66c257e31114be672fe96c9686633c3dd49078d386fa429a4eb9d2cb5e1` | bitwise PASS |

相关证据：`analysis/coverage_inventory/h31_get_desk_command_ipc_lifecycle_result_20260825.md`。

## H32：安全高偏心根数组合（新增主路径，2026-08-25）

H32 是在 H0/H11/H14/H15/H26/H27 之外的高偏心数值边界路径。固定合同为 `a=42,000,000 m`、`e=0.75`、轨道角 `(0.85,2.3,1.1,0.0)`、2032-06-30 23:59:50、位置 `(10,500,000,0,0) m`、速度 `(0,8,150,0) m/s`、四元数 `(0.5,-0.5,0.5,0.5)`、完整非零柔性、零设备命令与 `srand(12345)`。在长期采集前，原 ELF 单步 state/main/global y 浮点文本扫描未出现 `NaN` 或 `Inf`。

100/1000 步四块（CoreDynamic 264 B、global `y[33]` 264 B、main 544 B、IPC 3000 B）均完成 first/second 原 ELF 双采集并逐文件 `cmp` PASS。恢复端分别为 400/4000 条逐步 0-byte 差分记录；`make check-h32-high-ecc-safe-boundary` 为 2 条 PASS，空构建全量 selftest 为 328 条 PASS、非零块 0、显式失败 0。

| 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 | 恢复端结论 |
|---:|---|---|---|---|---|
| 100 | `3787f366e0751cf7c5c1ce36f79dc6e2e2ef8a88ed7779d71bed0a760b2de30e` | `abc3834b022fa2c8f1e849becab650df01d2070a4420ada6f54e18961c56ff9d` | `e0821d7582707f5456115cbcf231f54f4cf23511b062b83b58ab6ceb22f7f776` | `a7caa86a75cf6d33e0272100fcd4670a122721fd6639a873ecdc08dd369c6eb5` | bitwise PASS |
| 1000 | `c2936333d77d91621577001fdb79cc53c8ab7b4c802fcc48c99822d2ebdcf117` | `789d4a9f553fec9fc53b5c3ff677489c8a90d9576a88d0132950d7a9675024b0` | `0dbb7287f70a864fc9232709f064d5338d229863b9976821588fedca2787a76c` | `8b22441aa4133c4fb95c1880863fabe20f1f9a8ae530b2a744e38e20c46380e0` | bitwise PASS |

相关证据：`analysis/coverage_inventory/h32_high_ecc_safe_boundary_result_20260825.md`。这只覆盖当前平台、固定根数与非有限预筛通过的受控离线轨迹，不能外推到任意高偏心参数或安全关键用途。

## H33 增量记录：共享 IPC getter 错误合同

H33 不属于 `dyn_main` 长时域场景；它针对此前成功读取与无竞争生命周期之外的共享 IPC 公共 getter 分支，采用一个新建、零初始化的真实共享映射，执行无副作用、单进程、无竞争调用序列。原 ELF first/second GDB probe 对四个导出块逐文件 `cmp` 一致，恢复端严格 C11 比较器亦全部 bitwise PASS。

| 场景 | 调用合同 | 原 ELF 双采集 | 恢复端比较块 | Makefile 门禁 |
|---|---|---|---|---|
| **H33** | `init_shared()` 后，对 `get_float_value` 与 `get_uint8_value` 分别传入 `-1`、`0x258` 和有效 index/`NULL` 输出；然后 `close_shared()` 后以有效 float getter 自动重开 | PASS | 8 个返回码（32 B）、无效 float 输出哨兵（4 B）、自动重开 float 输出（4 B）、无效 uint8 输出哨兵（1 B）均 0-byte mismatch | `check-h33-ipc-getter-invalid`，已接入 `selftest` |

原 ELF 返回码数组为 `[0, -21, -21, -22, -21, -21, -22, 0]`：负索引和 `0x258` 无效索引返回 `-21` 并保持调用方输出哨兵；有效索引加 `NULL` 输出返回 `-22`；关闭后 `get_float_value(0, ...)` 自动重开并从新零初始化帧返回 `0.0f`。相应 first gold SHA-256 为：返回码 `afafd35cae487a6699cb19515903c734529269e17c1f996876881be4d0e475b5`，无效 float 哨兵 `367da4259c7beab7a7ce59235298dd373143ba8100b46498843f0a3fdb821e85`，自动重开 float 输出 `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119`，无效 uint8 哨兵 `bbeebd879e1dff6918546dc0c179fdde505f2a21591c9a9c96e36b054ec5af83`。

> **H33 的适用边界。** 证据仅覆盖已列出的真实共享映射、单进程、无竞争、当前 Linux x86-64 运行时和严格 C11 标志下的固定调用序列。它不证明共享对象创建失败、权限、资源耗尽、损坏锁、并发读写、任意指针／索引、其他 getter/setter、其他平台或安全关键用途的行为。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H34 增量记录：圆赤道根数启动链主传播

H34 从 `DynamicDllInit` 的默认初始块出发，覆写轨道根数后调用 `dyn_init`，使 `Elements2PosVel_M` 的结果写入实际传播全局状态，再以零 caller `core`、零设备命令和 `srand(1)` 连续调用 `dyn_main`。根数固定为 `a=7,000,000 m`、`e=0`、`i=0`、RAAN `0.73 rad`、近地点幅角 `1.17 rad`、平近点角 `2.41 rad`。该合同以正常量级的有限值进入圆/赤道退化面；RAAN 与近地点幅角仅作为有限 ABI 输入，不被表述为该退化几何下的唯一物理参数。

在建立长轨迹前，原 ELF 单步预筛已对初始化 `y`、一步 caller state、global `y`、main 和 IPC payload 扫描浮点文本，未见 `NaN` 或 `Inf`。随后 100/1000 步的四个逐步流各自 first/second 独立采集并逐文件 `cmp` 一致。恢复端每一步比较 caller `CoreDynamic` 前 264 B、global `y[33]` 264 B、main 544 B 和 IPC payload 3000 B，100 步 400 条、1000 步 4000 条均为 0-byte mismatch。

| 场景 | 初始／边界特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---|
| **H34** | `a=7,000,000 m`、`e=0`、`i=0`；有限 RAAN/`ω`/`M`；`DynamicDllInit → dyn_init → dyn_main`；零命令；`srand(1)` | PASS | PASS | PASS | `check-h34-circular-equatorial`，已接入 |

| H34 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 |
|---:|---|---|---|---|
| 100 | `0020b6ef808f8147fd134b43b016376043b792ea92b1bd266964548171e96e60` | `33bc14549e803ef2008f0ea7ed7fa68f6047bb93a26b1c7208d4c04562a85af0` | `ea40e249f39358c85f944f3024cd2eab632e745ab762ad88b939ef8ba5e486f7` | `46f0acf67347c0d7474aaf0ce30308cca0209fb3638556ad1d37f6ce4ac8b302` |
| 1000 | `7ff99e926fa995ecd50ef5e9998985e26f780818089a261c1fe17e07d940909a` | `55466b7bbcaf035ba642a8d8e3a4b123919a7d479ba5d977ce0f3e0efe89876f` | `1ad67a6c114afe29b722530c8a47907f2f5ea16793c2b650256c89c9a2db8b6d` | `d9d5f6aa1cea010253600e19f8fc81510b052b96c342ecd7ab968d0ae68afe1f` |

> **H34 的适用边界。** 该证据只覆盖列出的一个圆赤道根数合同、当前 Linux x86-64、当前运行时、默认严格 C11 构建、单进程无竞争和列明四个观察块。它不证明所有圆/赤道根数、近抛物 `e≈1`、近零半径、任意异常输入、并发、其他平台/编译器或安全关键用途。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H35 增量记录：`e=0.99` 近抛物高偏心根数启动链

H35 在 `DynamicDllInit` 后覆写根数为 `a=700,000,000 m`、`e=0.99`、`i=0.85 rad`、RAAN `2.30 rad`、近地点幅角 `1.10 rad`、平近点角 `0.37 rad`，再调用 `dyn_init`，以零 caller `core`、零设备命令和 `srand(1)` 连续调用 `dyn_main`。该合同保持 `7,000,000 m` 近地点、`1,393,000,000 m` 远地点和 `sqrt(1-e²)=0.14106735979665884425`，在不越过 `e=1` 的前提下扩展 H32 的高偏心覆盖。

预筛时，main offset `0x1e0` 已按 `DP_TM_ORBIT_ELEMENTS` 的对齐 double 数组解释，而不是把任意 double 半字误扫为 float。初始化 `y`、caller state、global `y` 与 main 已知 double 字段均无 `NaN/Inf`；main 的第一组根数反算为有限的 `a=699999999.9992301`、`e=0.9900000000017579`、`i=0.849999999922629`、RAAN `2.2999999998647374`。随后 100/1000 步的 state、global `y`、main、IPC 四流均完成 first/second 独立采集并逐文件一致；恢复端每步四块比较为 100 步 400 条、1000 步 4000 条，均 0-byte mismatch。

| 场景 | 初始／边界特征 | 100 步 | 1000 步 | gold 双采集 | Makefile 门禁 |
|---|---|---:|---:|---:|---|
| **H35** | `a=700,000,000 m`、`e=0.99`、`i=0.85`；近地点 `7,000,000 m`；零命令；`srand(1)`；启动链根数初始化 | PASS | PASS | PASS | `check-h35-near-parabolic`，已接入 |

| H35 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 |
|---:|---|---|---|---|
| 100 | `0c07c099320b234409a4d75cda5ff388f0c9a9e23a0bdee462722c9e02894031` | `e86e101b23050c4f075cffcd85c0dc975a0b5f03e2bda3ac025d2ac75bc416b0` | `a18c69872d190a77904a69752af68bc740134e91581920d65711311574729b71` | `4502751ebb629d903f9b296e20c0da824d0cabe52a97828d8983f4373aac925e` |
| 1000 | `f0ffe3cde232aed45d8aae14ba00d907f678ae094bfc0d0db56f01cd8b6be0a7` | `f5275542a1219837f9c9ac31cc3c3a01e64dbf392974d57b1127129c48c4df41` | `5e9ba0e34731780c4c8cbb7deef1415f195430d2ed217cbdc662fce84daf535e` | `c556309e04234746be5680eccbadb02d5f2b452197080a9f6f8a44d1523b47d9` |

> **H35 的适用边界。** 该证据仅覆盖列明的 `e=0.99`、当前 Linux x86-64、当前运行时、默认严格 C11 构建、单进程无竞争和四个列明观察块。它不证明 `e=1`、双曲根数、其他近抛物相位、任意半长轴/时长、异常输入、并发、其他平台/编译器或安全关键用途。本工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。
