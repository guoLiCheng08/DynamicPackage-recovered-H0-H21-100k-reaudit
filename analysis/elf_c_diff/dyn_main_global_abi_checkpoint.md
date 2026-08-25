# dyn_main 全局 ABI 与两步 IPC 端到端检查点

> **验证范围。** 本文仅陈述明确列出的原 ELF 快照、固定 `srand(1)` 初始随机状态及两步连续运行中的逐字节结果。它不是任意输入、任意运行时状态或全系统功能空间已完全等价的证明；项目继续仅用于离线仿真研究。

## 顶层调度 ABI 审计

原 ELF `dyn_main` 位于 `0x4020`，大小 `0x81`。寄存器 ABI 为 `rdi` 0x220 主输出对象、`rsi` 0x148 CoreDynamic 输出对象、`rdx` 0x78 UpdateDeviceControl 命令帧。栈上分配 0xa0 字节临时 CoreDynamic 输入，并严格执行以下顺序：

| 顺序 | 原调用 | 恢复端同名入口 |
|---:|---|---|
| 1 | `UpdateDeviceControl(command, 0.01)` | `UpdateDeviceControl` |
| 2 | `UpdateCoreDynInput(stack+0x00)` | `UpdateCoreDynInput` |
| 3 | `CoreDynamic(core_output, stack+0x00)` | `CoreDynamic` |
| 4 | `UpdateDeviceMeasure(core_output)` | `UpdateDeviceMeasure` |
| 5 | `UpdateMainOut(main_output, core_output)` | `UpdateMainOut` |

`src/dynamic_dyn_main_globals.c` 以相同顺序发布同名 `dyn_main`。端到端证据确认 CoreDynamic 消费 `DynamicInit` 建立的全局 `y`；其 `rsi` 输出对象在返回时被覆盖，不能以调用前对象内容播种全局积分状态。

## 新发布的 UpdateCoreDynInput

`UpdateCoreDynInput` 原 ELF 位于 `0x3920`。其输出为 0x90 字节：WheelGroup 的 +0x30 到输出 +0x00、+0x08 到输出 +0x18、MTQ_Group +0x08 到 +0x30、Thruster +0x68/+0x90 到 +0x48/+0x60，并跳转 `getSADAangle` 写入 +0x78/+0x80。`getSADAangle` 不写 +0x88；因此恢复端只写这两个 SADA 槽位，调用方决定最后 8 字节的预置值。

## 轨道初始化末位校准

真实 `dyn_init` 返回后的原 ELF `y` 显示速度 x/y 与恢复端相差各 2 ULP。GDB 在 `Elements2PosVel_M` 记录到：M2E 位模式一致，但原汇编先形成 `a·(1-e cos E)` 分母，并以 `n·a²` 相除。旧 C 表达式 `a*n/denom` 产生不同舍入。`dynamic_orbit.c` 已按原顺序改为 `denominator *= a; factor = a*n; factor *= a; factor /= denominator;`。原有 Elements2PosVel 函数级金标仍全部通过。

## 有限端到端金标

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| 零设备命令 | 2 | 每步 CoreDynamic 前 0x108（33 doubles）、完整 0x220 主输出、IPC +0x38..0xbf0（3000 bytes） | 全部 0 字节差异 |
| 第一飞轮 +0.001 扭矩命令 | 2 | 每步 CoreDynamic 前 0x108、完整 0x220 主输出、IPC +0x38..0xbf0 | 全部 0 字节差异 |

比较器分别为 `analysis/dyn_main_global_step1_original_gold_compare.c` 与 `analysis/dyn_main_global_rw1_pos_two_step_compare.c`，均被纳入 `make CC=gcc AR=ar selftest`。最新完整严格自检通过，出现 67 条 `PASS` 标签。

## 仍未覆盖的边界

当前端到端新覆盖尚未包括推进器启用、惯量旗标切换、SADA 速度命令、多个传感器噪声组合、连续超过两步的随机数消耗、非默认 `dyn_init` 设备构型和锁对象本身的 pthread 语义。后续应针对这些已有或新建原 ELF 探针逐一扩展，不应将有限场景推广为全部输入空间。

## MTQ 与 SADA 位置命令扩展（2026-08-20）

现有 MTQ X 正命令两步门禁已持续通过。本轮对 SADA 位置命令 `flag=1`、命令角 `[+0.01,-0.01]` 进行四阶段 RK4 差分：原 ELF 第一子阶段的输入状态与恢复端一致，但恢复端的刚柔导数最先偏离。`dynamics_flex+0x948` 原 ELF 栈快照显示，`H_total` 已逐位相同；差异来自恢复端在 `dp_global_apply_sat_inertia_flag0` 后错误将 SADA 反作用映射重绑至 `Sat+0x448`，并将模态预投影重绑至 `Sat+0x508`。原反汇编中模态支路的 `blas_gemv` 使用 `Sat+0x4a8`，而刚体反作用使用已单独校准的 3×3 backing。恢复端已据此修正。

独立 `UpdateDeviceControl` 金标进一步表明，它直接使用调用者给出的 SADA 采样尺度；`dyn_main` 才在调用边界给出 `0.01` 秒。该边界修正后，独立控制 ABI 与顶层路径不再相互冲突。

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| MTQ X 正命令 | 2 | 每步 33 维状态、0x220 主输出、IPC payload | 全部 0 字节差异 |
| SADA 位置命令 `[+0.01,-0.01]` | 2 | 每步 33 维状态、0x220 主输出、0x68 SADA 对象、IPC payload | 全部 0 字节差异 |

SADA 比较器 `analysis/dyn_main_global_sada_position_two_step_compare.c` 已加入 `make selftest`。本轮最终 `make CC=gcc AR=ar selftest` 产生 **69 条 PASS**，且无编译错误、运行失败或非零字节差异标记。以上仅是明确列出的受控输入证据，不构成全输入空间等价证明。

## 推进器启用两步顶层扩展（2026-08-20）

推进器阶段使用同一 `dyn_init`、固定 `srand(1)` 与两步连续运行，仅令命令帧 `thruster_work_status=1`。原 ELF 探针除状态、主遥测帧和 IPC 外，还保存了推进器的标量字段及四个 descriptor backing 向量。因为 descriptor 内部数据指针属于进程地址，不把其地址差异视为行为差异；验收改为逐字节比较 `force_scale`、`work_status`、力臂、输入力、输出力和输出力矩四个向量。原 ELF 第一步和第二步的这些向量相同，分别为力臂 `[0,0,0.01]`、输入 `[0,-1,0]`、输出 `[0,-0.0002254,0]`、力矩 `[+2.254e-6,0,-0]`。

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| 推进器启用 | 2 | 每步 33 维状态、0x220 主输出、推力标量/四个三维 backing 向量、IPC payload | 全部 0 字节差异 |

比较器 `analysis/dyn_main_global_thruster_on_two_step_compare.c` 和原 ELF 采集脚本 `analysis/time_orbit/dyn_main_thruster_on_two_step_probe.gdb` 已纳入工程。最终 `make CC=gcc AR=ar selftest` 产生 **70 条 PASS**，无失败标记。该结论仅覆盖此固定默认推进器构型与启用命令，不能外推到其它推力输入、脉冲时序或构型。

## 惯量旗标 1 与组合执行机构扩展（2026-08-20）

`inertia_update_flag=1` 的初始顶层差分显示，J/J⁻¹ 已与原 ELF 一致，但首步 CoreDynamic 仍有 54/264 字节差异。这表明仅复现 `Update_sat_inertia_xw(flag=1)` 的惯量重置不足。原 ELF 在 `CoreDynamic+0x4a6`（RK4 前）截取的 Sat descriptor backing 表明，flag=1 还重建了 `m3_e8`、`m3_448`、`m3_4a8`、`m3_508`、`m3_568`、`m6`、耦合矩阵、`m9`、模态 A、模态 D。恢复端据此以精确 double 位模式装配这十个模型块，并重建其描述符；J/J⁻¹ 仍由现有 flag=1 惯量重置函数写入。独立 `UpdateDeviceControl` 复验还发现该装配不得调用全局 reset，否则会破坏独立 ABI 预置的 SADA/J 状态；现改为只初始化私有模型 backing。

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| 惯量旗标 1 | 2 | 每步 33 维状态、0x220 主输出、推进器不变量、IPC payload | 全部 0 字节差异 |
| 惯量旗标 1 + 推进器启用 | 2 | 每步 33 维状态、0x220 主输出、推力标量/四个 backing 向量、IPC payload | 全部 0 字节差异 |

对应比较器为 `analysis/dyn_main_global_inertia_flag1_two_step_compare.c` 与 `analysis/dyn_main_global_inertia_flag1_thruster_on_two_step_compare.c`；原 ELF 采集脚本分别为 `analysis/time_orbit/dyn_main_inertia_flag1_two_step_probe.gdb` 和 `analysis/time_orbit/dyn_main_inertia_flag1_thruster_on_two_step_probe.gdb`。两者均已纳入 `make selftest`。最后完整严格自检产生 **72 条 PASS**，无非零差异、编译错误或运行失败标记。该验证仅覆盖默认构型下固定 flag=1 与组合命令的两步连续运行，不构成任意惯量配置、执行器组合或更长时域的等价结论。

## 十步随机传感器与 IPC 连续回归（2026-08-20）

为扩展两步证据链，原 ELF 以相同默认初态、零设备命令和固定 `srand(1)` 连续运行十步。每步都保存积分后的 33 维状态、完整 0x220 主遥测帧以及 3000 字节 IPC payload。恢复端在每一步先执行 `dyn_main`，再执行相同的 `sendDynTele` 映射；十个状态块、十个遥测帧和十个 payload 块均为 0 字节差异。该结果同时覆盖了 STS 等传感器噪声对 `rand()` 消耗顺序的十步连续影响。

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| 零命令，固定 `srand(1)` | 10 | 每步 33 维状态、0x220 主输出、3000 字节 IPC payload | 30 个比较块全部 0 字节差异 |

原 ELF 采集脚本为 `analysis/time_orbit/dyn_main_zero_ten_step_probe.gdb`，比较器为 `analysis/dyn_main_global_zero_ten_step_compare.c`，已加入 `make selftest`。当前完整严格自检产生 **73 条 PASS**，没有失败标记。该有限十步随机序列只验证明确的初态与种子，不能外推至其他种子、噪声参数或任意运行时长。

### 干净构建检查点

本轮执行了 `make clean` 后的 `make CC=gcc AR=ar selftest`。从无构建产物状态重新编译的结果仍为 **73 条 PASS**，未发现编译错误、运行失败或非零字节差异。原 ELF 原始样本保持只读，所有探针均使用 `analysis/debug_runner/DynamicPackage.exec_copy`。

## 十步混合控制序列与 SADA 历史分支（2026-08-20）

新增的十步序列在固定 `srand(1)` 下依次覆盖：零命令、第一飞轮正扭矩、MTQ X 正命令、SADA 位置命令、推进器启用加惯量旗标 1、第一飞轮负扭矩、第二个 SADA 位置命令、MTQ Y 负命令、推进器启用和零命令。首轮差分在第五码点第一次偏离；该步前的飞轮、MTQ、SADA、J/J⁻¹、H、L、外力均已与原 ELF 一致，因此继续截取 `CoreDynamic+0x4a6` 的 Sat 模型。

原 ELF 证据显示：当此前 SADA 位置控制留下非零 `angular_acceleration` 时，`inertia_update_flag=1` 仍使用 flag=1 的 J/J⁻¹，但刚柔模型块改为与 SADA 姿态相关的默认分支。恢复端此前无条件加载静态 flag=1 矩阵，因此第五码点出现 56/264 状态字节差异。现将该明确分支实现为：保留 flag=1 J/J⁻¹，以已审计 flag=0 装配恢复 SADA 相关矩阵 backing，再恢复保存的惯量。十步输出随即完全收敛。

| 场景 | 连续步数 | 逐字节比较对象 | 结果 |
|---|---:|---|---|
| 飞轮、MTQ、两次 SADA 位置、推进器、惯量旗标切换的固定序列 | 10 | 每步 33 维状态、0x220 主输出、3000 字节 IPC payload | 30 个比较块全部 0 字节差异 |

原 ELF 探针为 `analysis/time_orbit/dyn_main_mixed_command_ten_step_probe.gdb`，比较器为 `analysis/dyn_main_global_mixed_ten_step_compare.c`，已纳入 `make selftest`。本轮完整严格自检产生 **74 条 PASS**，无失败标记。该条件分支只由所列 SADA 历史状态和有限原 ELF 快照证实；不应外推至全部 SADA 轨迹或任意执行机构时序。

## 第二随机种子十步传感器回归（2026-08-20）

为排除仅固定 `srand(1)` 的偶然覆盖，使用同一初态、零命令和 `srand(12345)` 从原 ELF 采集另一条十步序列。恢复端在每步状态、完整主输出和 3000 字节 IPC payload 上均为 0 字节差异，说明已验证的传感器随机数调用顺序也覆盖这条明确的第二种子路径。

| 随机种子 | 连续步数 | 逐字节比较对象 | 结果 |
|---:|---:|---|---|
| 12345 | 10 | 每步 33 维状态、0x220 主输出、3000 字节 IPC payload | 30 个比较块全部 0 字节差异 |

原 ELF 探针为 `analysis/time_orbit/dyn_main_zero_ten_step_seed12345_probe.gdb`，比较器为 `analysis/dyn_main_global_zero_ten_step_seed12345_compare.c`，已纳入 `make selftest`。当前完整严格自检产生 **75 条 PASS**，无失败标记。此证据仍只覆盖两个固定种子、默认噪声参数与十步时域。

## SADA 两轴限幅十步连续回归（2026-08-20）

使用固定 `srand(1)`、持续 `sada_command_flag=1` 与两轴超限目标角 `[1.0, -1.0]`，从原 ELF 采集十步连续金标。该输入迫使 SADA 目标按各轴 `command_limit` 钳位，并连续经过角加速度、角速度和位置积分路径。恢复端十步中的每个 33 维状态、0x220 主遥测帧和 3000 字节 IPC payload 均为 0 字节差异；比较器 `analysis/dyn_main_global_sada_saturation_ten_step_compare.c` 已纳入 `make selftest`。

当前完整严格自检产生 **76 条 PASS**，未出现构建错误、运行失败或非零字节差异。该测试仅证明给定初态、固定种子及该两轴限幅指令下的十步行为，不构成其他 SADA 指令轨迹或任意时域的等价结论。

### 本轮干净构建检查点

在新增混合命令、第二随机种子和 SADA 限幅场景后，执行 `make clean` 再执行 `make CC=gcc AR=ar selftest`，结果为 **76 条 PASS**，失败标记为零。该检查点只陈述已保存原 ELF 输入／金标场景的逐字节一致性，不将有限回归外推为整个输入空间或安全关键用途的正确性保证。

## 二十步混合控制连续回归（2026-08-20）

在既有十步混合序列基础上，原 ELF 继续运行十步零命令，以检验执行机构切换后的惯量模型、MTQ 缓存、SADA 内部状态、传感器随机数消耗及 IPC 映射的长期延续。二十个状态块、二十个 0x220 主输出帧与二十个 3000 字节 IPC payload 块均为 0 字节差异。比较器 `analysis/dyn_main_global_mixed_twenty_step_compare.c` 已加入严格自检；当前完整自检为 **77 条 PASS**、无失败标记。该结果限定于所记录的初态、命令序列与二十步时域。

## 非共线四元数初态 MTQ 两步回归（2026-08-20）

新增规范化非共线初态四元数 `[0.5, 0.5, -0.5, 0.5]` 与 MTQ X 正命令的两步顶层场景。原 ELF 与恢复端在两步的 33 维状态、0x220 主输出和 IPC payload 均为 0 字节差异。比较器 `analysis/dyn_main_global_noncollinear_mtq_two_step_compare.c` 已纳入严格自检；当前全量回归为 **78 条 PASS**、无失败标记。该验证仅覆盖明确的四元数、初始轨道和两步命令，不外推为任意姿态或任意时间跨度的结论。

### 当前干净构建检查点

在二十步混合控制及非共线姿态 MTQ 场景纳入后，执行 `make clean` 与 `make CC=gcc AR=ar selftest`。从无构建产物状态的结果为 **78 条 PASS**，编译错误、运行失败和非零字节差异标记均为零。

### 未闭环诊断：非共线 SADA + 惯量旗标 1

已采集非共线四元数 `[0.5, 0.5, -0.5, 0.5]`、SADA 位置命令 `[0.01, -0.01]` 和惯量旗标 1 的两步原 ELF 金标。恢复端首轮比较在第一步出现 63/264 状态字节、129/544 主输出字节和 28/3000 IPC payload 字节差异，第二步差异继续扩大。因此该场景**未**加入 `make selftest`，也不应视为已验证。下一步将截取该场景 `CoreDynamic+0x4a6` 的 J/J⁻¹ 及 Sat 矩阵 backing，并与恢复端实际 descriptor 比较，以确认 flag=1 刚柔模型是否还依赖当前姿态。

补充取证：非共线 SADA + 惯量旗标 1 场景在首步 `CoreDynamic+0x4a6` 捕获的 J/J⁻¹ 与既有 flag=1 原 ELF 金标均为 0 字节差异。因此当前首步差异不来自 J/J⁻¹，而应继续检查姿态相关 Sat 描述符 backing、SADA 投影或力矩装配；该场景仍保持未通过状态。
进一步取证：非共线 SADA 旗标首步的 0x90 CoreDynamic 输入与已通过默认 SADA 位置场景对应输入为 0 字节差异，且 J/J⁻¹ 也一致。因而差异不在顶层执行机构输入装配或惯量 backing；后续应直接采集/比较 `CoreDynamic` 所用的姿态相关 Sat 描述符、环境／力矩临时量及 RK4 首个 RHS。
补充模型取证：非共线 SADA + 惯量旗标 1 首步实际使用的 m4a8、m6、耦合 C、m9、modal A、modal D 六个 Sat descriptor backing，与既有静态 flag=1 原 ELF 模型逐块均为 0 字节差异；首步预积分全局 y、J/J⁻¹ 和 CoreDynamic 输入也一致。剩余调查重点收敛至 SADA 设备对象的恢复端实际值、未捕获的反作用矩阵 backing，或 CoreDynamic 环境／力矩的临时写入顺序。
补充设备取证：非共线 SADA + 惯量旗标 1 场景首步后，恢复端与原 ELF 的完整 0x68 SADA 对象为 0 字节差异。至此，预积分 y、核心输入、J/J⁻¹、关键刚柔 descriptor 和 SADA 设备对象均已排除；下一轮需截取 RK4 首阶段环境/力矩临时量与未映射的 SADA 反作用 backing。
下一轮诊断计划：恢复端可通过 `DP_DIAGNOSTIC_FLEX_RHS_TRACE` 记录刚柔基础 RHS、SADA 反作用向量、反作用扣除后 RHS 与模态项后 RHS；原 ELF 将在同一 RK4 首阶段采集对应 `dynamics_flex` 局部栈三元向量。该步骤尚未完成，未改变任何通过门禁。
门禁状态核验：未闭环的 `noncollinear-SADA-flag1` 比较器未写入 `Makefile`；最近一次干净构建的既有严格自检仍为 78 条 PASS。
取证脚本审计：既有 `dynamics_flex_rhs_terms_real_model.gdb` 会在 `dynamics_flex` 入口人工覆写 rate/模态状态，不能直接用于顶层非共线场景。后续须从非共线 `dyn_main` 探针派生专用脚本，仅在首个真实 RK4 阶段采样、不得写入原 ELF 状态。
当前未闭环检查点汇总：非共线 SADA + flag1 比较器的第一步状态/主输出/IPC 差异分别为 63/264、129/544、28/3000 字节；第二步为 69/264、142/544、38/3000 字节。已排除首步预积分 y、CoreDyn 输入、J/J⁻¹、关键刚柔 descriptor、完整 SADA 对象；既有 78 条严格门禁仍未受影响。下一步是从该真实顶层场景采集首个 RK4 阶段的 `dynamics_flex` 临时向量，不对原 ELF 状态执行人工覆写。
调用点映射已复核：`differential_equation+0x6e78` 调用 `dynamics_flex` 前，`rdi=[rsp+0x10]`、`rsi=r12`、`rdx=[rsp+0x70]`、`rcx=[rsp+0x60]`，并通过栈传入 H_w_B、L_c_B、J_c_B、J_c_B_inv；返回后 `differential_equation+0x6e7d` 读取刚体／模态输出。专用取证脚本应以此调用点为基础，只读记录首个真实阶段参数和函数内部局部向量。
续接检查：非共线 SADA 旗标首步模型与状态金标均已落盘，既有干净构建基线仍为 78 条 PASS；工作集可直接进入真实 RK4 首阶段刚柔临时量取证。
首阶段寄存器快照解释修正：`dynamics_flex` 对应 C 接口的前七个参数分别为刚体角加速度输出、模态加速度 descriptor、模态速度输出 descriptor、body rate、模态位置 descriptor、模态速度 descriptor 和 config；因此先前对 rdi/rsi/rdx 的直接 double 解码仅可作为地址/对象快照，不可当作三个纯数值向量。后续探针将按 descriptor 的 data 指针导出 backing。
已采集非共线 SADA + flag1 首个真实 `dynamics_flex` 调用的栈、寄存器容器头及两个模态 backing 快照。初步解码显示寄存器参数包含 descriptor/状态对象，不应按纯向量解释；下一步将结合调用 ABI 与恢复端同阶段跟踪，校正各 backing 的精确语义后进行比较。
当前非共线 SADA 旗标调查状态：首个真实 `dynamics_flex` 调用的栈、容器头与模态 backing 已捕获；descriptor 语义校正尚待完成。该失败场景仍未纳入 Makefile，既有干净构建基线保持 78 条 PASS。
首阶段 ABI 取证进展：rsi 的首个 descriptor backing 精确为角速度 `(0.0010000000474974513,-0.0020000000949949026,0.003000000026077032)`；相邻三维 backing 以及 r8/r9 十维 backing 在该首阶段为零。r8/r9 仍需通过容器头和返回写回进一步确定其具体输入／输出角色，rdx/rcx 也需补充 descriptor backing 后方可进行恢复端逐项对照。
补充首阶段 ABI 取证：rcx 的十维 backing 在首阶段也为零。当前 rdx、rcx、r8、r9 的容器角色不能仅凭零 backing 区分；后续须在 `dynamics_flex` 返回写回点或非零后续阶段采样，不能据此猜测公式或修改恢复实现。
关键新证据：非共线 SADA + flag1 场景首个真实 `dynamics_flex` 返回后的刚体角加速度为 `(5.389359250436255e-06, 2.8745555289724137e-06, 9.98381995720527e-07)`，已保存为 `gold_noncollinear_sada_flag1_stage1_flex_rigid_out_data.bin`。后续将让恢复端记录同一首阶段角加速度和 RHS 分项进行逐项比较；该证据不代表场景已闭环。
补充模型闭环取证：非共线 SADA + flag1 场景的 m3_e8、m448、m508、m568 也逐块与既有原 ELF flag=1 静态模型 0 字节差异；连同先前 m4a8、m6、C、m9、MA、MD 的一致性，全部已映射 Sat 模型块均已排除。恢复端首阶段角加速度仍显著偏离原 ELF，调查重点因此收敛到独立 SADA 反作用 backing 或未映射的全局装配分支。
补充容器审计：非共线 SADA + flag1 原 ELF `SatTorque` 的 0x168 原始快照首段为描述符容器（计数/指针交错），不能直接以 f64 格式解读为力矩。若继续检验全局力矩装配，必须沿各 descriptor 的 `+0x08` data 指针导出对应三元 backing。

更正 SatTorque ABI 审计：运行时验证确认 `DpVector` descriptor 为 0x10 字节，首向量 data 指针位于 `&SatTorque+0x08`；`+0x10` 是下一 descriptor 的计数值 3，不能解引用为地址。沿 `+0x08` 导出的首三维 backing 在该场景 RK4 前为零。非共线 SADA+flag1 的后续力矩取证应始终沿各 descriptor 的 `+0x08` data 指针导出，而不能仅保存 descriptor header。

## 非共线 SADA + 惯量旗标 1 闭环（2026-08-20）

此前未通过的受控场景为：初始四元数 `[0.5, 0.5, -0.5, 0.5]`，SADA 位置命令 `[+0.01, -0.01]`，连续两步均请求 `inertia_update_flag=1`。首步的 63/264 状态字节差异经原 ELF `dynamics_flex` 首个真实返回值取证后，确认恢复端在 flag=1 静态 Sat 模型分支仍错误注入了独立 SADA 加速度反作用项。以金标 J/C 计算，该错误项导致的理论角加速度增量为 `[-3.91673616e-6, -1.05825866e-6, +2.95802474e-5]`，与观测的恢复端减原 ELF增量逐分量同号、同量级且数值相符；在该静态分支禁用该项后，第一步的状态、主帧与 IPC 全部收敛为 0 字节差异。

第二步初始仍偏离。原 ELF 第二步 `CoreDynamic+0x4a6` 的 m4a8、m6、耦合、m9、MA、MD 逐块与 flag=1 静态金标 0 字节差异，而恢复端因“存在 SADA 历史即走 flag=0 刚柔模型”的过宽条件保留了 flag=0 模型。原 ELF 第二步首个 `dynamics_flex` 调用在陀螺叉乘前的 `H_total` 栈向量为 `[0.12522462317129088, -0.7240266223032008, 1.409347403022203]`；错误恢复端为 `[-0.12027343, -0.96566265, 17.55801824]`。证据说明连续 flag=1 请求即使已有非零 SADA 角加速度，也必须重装 flag=1 静态 Sat 模型；只有从 flag=0 首次切换至 flag=1 且保留 SADA 历史时，才使用已验收的 flag=0 刚柔模型分支。

恢复端在 `dp_global_apply_sat_inertia_flag1` 中新增私有连续 flag=1 标记：首次切换的 SADA 历史分支保持 flag=0 模型及反作用项；连续 flag=1 直接装配静态 flag=1 模型并禁用独立反作用项。该实现未改变任何公开 ABI。比较器 `analysis/dyn_main_global_noncollinear_sada_flag1_two_step_compare.c` 已加入 `make selftest`；其两步 33 维状态、完整 0x220 主输出和 IPC payload 均为 0 字节差异。移除临时诊断钩子后的 `make clean && make CC=gcc AR=ar selftest` 输出 **79 条 PASS**，没有编译错误、失败标记或非零字节差异。该结论仅适用于所记录的有限命令、初态和两步时域，不构成任意输入空间或安全关键用途的正确性证明。

## 非共线 SADA + 连续惯量旗标 1 十步回归（2026-08-20）

为验证本轮连续 flag=1 状态机修复不只覆盖两步路径，新增固定初始四元数 `[0.5, 0.5, -0.5, 0.5]`、SADA 位置命令 `[+0.01, -0.01]`、每步持续 `inertia_update_flag=1` 的十步连续原 ELF 金标。探针 `analysis/time_orbit/dyn_main_noncollinear_sada_flag1_ten_step_probe.gdb` 在固定 `srand(1)` 下逐步保存 33 维状态、完整 0x220 主帧与 3000 字节 IPC payload；重复采集三类聚合金标均为逐字节一致，证明该受控采集可复现。

恢复端比较器 `analysis/dyn_main_global_noncollinear_sada_flag1_ten_step_compare.c` 在十步的全部 30 个比较块中均得到 0 字节差异，覆盖连续 flag=1 下 SADA 角加速度历史、静态 Sat 模型重复装配、RK4、传感器随机数消耗与 IPC 输出的受控十步路径。该比较器已加入 `make selftest`；移除构建产物后的 `make clean && make CC=gcc AR=ar selftest` 输出 **80 条 PASS**，无编译错误、失败标记或非零字节差异。上述结论严格限定于所列初态、指令、随机种子和十步时域，不外推为任意输入空间或安全关键用途正确性证明。

## 非共线 SADA 历史 + 旗标往返混合十步闭环（2026-08-20）

新增受控混合序列采用初始四元数 `[0.5, 0.5, -0.5, 0.5]`，在十步内覆盖 SADA 两组位置目标、`inertia_update_flag` 的 0/1 往返、飞轮正负扭矩、MTQ X/Y 命令及推进器启用。固定 `srand(1)` 的原 ELF 探针对状态、完整主帧和 IPC payload 重复采集均逐字节一致。恢复端最初在第四步首次偏离，随后持续累积差异。

根因经原 ELF `Update_sat_inertia_xw` 反汇编和第四步 `CoreDynamic+0x4a6` backing 取证闭环。`edi==1` 分支直接跳到仅复制 3×3 惯量的代码，跳过全部 Sat 刚柔模型写回；因此 flag=1 的真实语义是**保留此前模型**，而不是恢复端此前实现的“按本次旗标重新装配静态模型”。第四步 m4a8、m6、coupling、m9 均与 flag=0 SADA 位置历史模型 0 字节差异。恢复实现现以私有 `dp_global_sat_model_is_sada_history` 区分模型来源：初始无 SADA 历史的首个 flag=1 继续装配已验收静态常量以补足恢复端初始化差异；一旦 flag=0 已建立 SADA 历史模型，后续 flag=1 严格保留该模型，并维持与其匹配的独立 SADA 加速度反作用项。

比较器 `analysis/dyn_main_global_noncollinear_flag_transition_ten_step_compare.c` 已加入 `make selftest`。十步 30 个比较块（每步 33 维状态、0x220 主帧、3000 字节 IPC payload）全部 0 字节差异；`make clean && make CC=gcc AR=ar selftest` 输出 **81 条 PASS**，无编译错误、失败标记或非零字节差异。本结论仅适用于所记录的离线受控输入、固定随机种子与十步时域，不外推为任意输入或安全关键用途的正确性证明。

## 非共线 SADA 历史 + 旗标往返混合二十步闭环（2026-08-20）

在已通过的十步非共线混合旗标序列基础上，新增两个连续十步周期的二十步场景。每个周期覆盖 SADA A/B/C 三组位置目标、`inertia_update_flag` 的 0/1 往返、飞轮正负命令、MTQ X/Y 命令和推进器启用。原 ELF 的状态（5280 字节）、完整主帧（10880 字节）及 IPC payload（60000 字节）在固定 `srand(1)` 下重复采集逐字节一致。

二十步比较器初版在第 11 步出现差异。取证显示第十步结束时恢复端与原 ELF 的完整 SADA、J 与 Sat 模型状态均已一致；第 11 步后分叉来自比较器自身将 `switch(step)` 的默认分支误用于第二周期，而原 ELF 探针按 `step % 10` 重放命令。将比较器统一为 `switch(step % 10u)` 后，二十步全部 60 个比较块（每步状态、主帧、IPC）均为 0 字节差异。该差异属于测试夹具命令时序错误，未修改恢复动力学实现。

`analysis/dyn_main_global_noncollinear_flag_transition_twenty_step_compare.c` 已加入 `make selftest`，复用经审计的十步命令编排并显式启用二十步金标模式。移除构建产物后的 `make clean && make CC=gcc AR=ar selftest` 输出 **82 条 PASS**，无编译错误、失败标记或非零字节差异。该验证仍限定于记录的离线初态、控制序列、随机种子和二十步范围，不外推为任意输入空间或安全关键用途的正确性证明。

## 非共线旗标往返二十步：随机种子 12345（2026-08-20）

为扩展传感器随机路径覆盖，在非共线 SADA 历史与旗标往返二十步混合序列上增加固定 `srand(12345)`。原 ELF 状态、完整主帧和 IPC payload 分别保存为 5280、10880、60000 字节独立金标；重复原 ELF 采集三者均逐字节一致。比较器通过二十步通用实现的编译期模式选择 seed=12345、对应金标文件和同一命令周期，不复制或修改恢复动力学代码。

种子 12345 的二十步共 60 个比较块全部 0 字节差异。该比较器已纳入 `make selftest`。一次干净构建中既有混合十步比较器出现瞬态非零差异；随即重新运行其原 ELF 探针，得到与已有金标逐字节相同的输出，并在重新干净构建后全部通过，确认金标未被改写且最终门禁稳定。最终 `make clean && make CC=gcc AR=ar selftest` 输出 **83 条 PASS**，无编译错误、失败标记或非零字节差异。

本轮仅在受控离线初态、固定指令序列、种子 1 与 12345、二十步范围内证明已采样行为等价；不构成任意随机序列、任意初态或安全关键用途的正确性保证。

## 非共线姿态 + 非零柔性模态 + SADA + 连续旗标 1 十步闭环（2026-08-20）

新增端到端场景采用四元数 `[0.5, 0.5, -0.5, 0.5]`、非零刚体角速度、SADA 位置目标 `[0.01, -0.01]`、连续 `inertia_update_flag=1`，并显式设置 33 维状态中的柔性区：`y[13]=1e-4`、`y[16]=-2e-4`、`y[20]=5e-5`、`y[23]=1e-3`、`y[27]=-5e-4`、`y[32]=2.5e-4`。恢复端 `DpState` 的 ABI 将 `y[13..32]` 表示为单一 `flexible_state[20]` 数组，前十项与后十项对应刚柔核的两组模态输入；比较器已按此真实布局赋值。

固定 `srand(1)` 的原 ELF 十步金标在状态（2640 字节）、完整主帧（5440 字节）及 IPC payload（30000 字节）上重复采集逐字节一致。新比较器 `analysis/dyn_main_global_noncollinear_flex_sada_flag1_ten_step_compare.c` 复用连续 SADA 加旗标验证骨架，并已加入 `make selftest`。十步 30 个比较块全部 0 字节差异；`make clean && make CC=gcc AR=ar selftest` 最终输出 **84 条 PASS**，无编译错误、失败标记或非零字节差异。

该结论仅覆盖记录的非零模态幅值、姿态、命令、随机种子及十步离线时域；不外推为任意模态幅值、任意输入空间或安全关键用途的正确性证明。

## 非共线姿态：飞轮正向超限与反向脱离两步闭环（2026-08-20）

新增端到端边界场景将第 0 只飞轮的初始转速注入为 `omega_limit + 0.1 = 628.4185307179587`，在非共线姿态下第一步施加正扭矩 `+0.01`，第二步切换为负扭矩 `-0.01`。原 ELF 快照证明该不对称分支的可观察结果：第一步在正向超限时将加速度与扭矩置零并保持超限转速；第二步负向命令允许脱离饱和，得到 `acceleration=-0.7885837509758672`、`torque=-0.010015013637393513`。

原 ELF 两步状态、主帧、IPC payload 与完整 4×0x78 飞轮对象均已重复采集并逐字节一致。专用比较器 `analysis/dyn_main_global_noncollinear_wheel_limit_two_step_compare.c` 已纳入 `make selftest`；两步共 6 个状态／主帧／IPC 比较块均为 0 字节差异。`make clean && make CC=gcc AR=ar selftest` 输出 **85 条 PASS**，无编译错误、失败标记或非零字节差异。结论限定于记录的离线初态、边界命令与两步时域，不外推为任意执行器状态或安全关键用途的保证。

## 系统化覆盖矩阵与二十步非零柔性模态长时域验证（2026-08-20）

建立 `analysis/elf_c_diff/systematic_coverage_matrix.md`，将覆盖工作按初态、刚柔状态、惯量状态机、执行机构、随机路径、轨道／时间及异常 ABI 分层，并明确每个单元须遵循“重复原 ELF 金标 → 最小比较器 → 逐字节比较 → selftest”流程。

作为矩阵 P0 长时域单元，非共线姿态 `[0.5,0.5,-0.5,0.5]`、非零柔性状态 `y[13..32]`、SADA 位置命令和连续 flag1 场景由十步扩展为二十步。原 ELF 状态（5280 字节）、主帧（10880 字节）与 IPC payload（60000 字节）重复采集逐字节一致；恢复端二十步 60 个比较块全部 0 字节差异。专用比较器 `analysis/dyn_main_global_noncollinear_flex_sada_flag1_twenty_step_compare.c` 已加入 `make selftest`。

`make clean && make CC=gcc AR=ar selftest` 当前输出 **86 条 PASS**，无编译错误、失败标记或非零字节差异。本记录仅增加对指定离线初态与二十步时域的证据，不外推为任意状态空间或安全关键用途保证。

## 非共线多飞轮限幅 + MTQ + 推进器组合十步闭环（2026-08-20）

新增 P1 组合覆盖场景以非共线四元数 `[0.5,0.5,-0.5,0.5]` 起始，并将第 0 飞轮置于 `omega_limit+0.1`。十步命令周期覆盖：正向超限阻断、负向脱离饱和、其余飞轮异号扭矩、MTQ 通道 0–5 的正负组合、推进器开关和一次惯量 flag1 更新。原 ELF 状态（2640 字节）、主帧（5440 字节）、IPC payload（30000 字节），以及末态完整飞轮／MTQ／推进器对象，重复采集均逐字节一致。

首轮比较出现首步偏离后，审计 `DpDeviceControlCommand` ABI 确认飞轮命令起始偏移为 `+0x08`、MTQ 命令起始偏移为 `+0x28`；原 ELF 探针此前按 `+0x00/+0x20` 写入造成采集命令与恢复端结构体不一致。校正探针全部字段偏移并重新采集后，恢复端十步状态、0x220 主帧和 IPC payload 的 30 个比较块全部为 0 字节差异。比较器 `analysis/dyn_main_global_noncollinear_multi_actuator_ten_step_compare.c` 已加入 `make selftest`。

`make clean && make CC=gcc AR=ar selftest` 当前输出 **87 条 PASS**，无编译错误、失败标记或非零字节差异。本记录仅覆盖指定离线姿态、飞轮初态、控制序列、随机种子和十步时域，不外推为任意执行器组合、输入空间或安全关键用途保证。

## 飞轮精确速度边界与负向超限阻断闭环（2026-08-20）

新增隔离原 ELF 探针覆盖三组飞轮对象路径：`omega == omega_limit` 且预置非零 acceleration 的保持语义；`omega=-2*omega_limit` 下超出负转矩上限的命令；以及 `omega=2*omega_limit` 下超出正转矩上限的命令。三组 0x78 对象金标均重复采集逐字节一致。

比较器以原 ELF `pre_rwheel_4.bin` 作为完整对象保留字节基线，覆写仅受控的上限、惯量、速度和加速度字段后调用 `dp_set_wheel_acc`。首次比较揭示恢复端此前仅阻断正向超限加速，而原 ELF 对负向超限且继续负向加速同样将 acceleration 置零；已在 `src/dynamic_devices.c` 中补充对称条件，且保留 `omega == +omega_limit` 时不写 acceleration 的精确相等语义。修复后全部三组完整对象 0 字节差异。

`analysis/time_orbit/wheel_exact_boundary_gold_compare.c` 已纳入 `make selftest`。`make clean && make CC=gcc AR=ar selftest` 当前输出 **88 条 PASS**，无编译错误、失败标记或非零字节差异。该结论仅覆盖所列离线飞轮边界状态及隔离调用，不外推为任意执行器状态或安全关键用途保证。

## 高偏心跨日期三维轨道十步与 DSS 遗留 ABI 闭环（2026-08-20）

新增受控端到端场景以日历时间 `2031-12-31 23:59:50`、半长轴 `26,560,000 m`、偏心率 `0.65` 及非零轨道根数初始化；初始姿态为规范化非共线四元数 `[0.5,0.5,-0.5,0.5]`，角速度为 `[0.0125,-0.00875,0.00425] rad/s`，位置为 `[15,-90,-39]×10^6 m`、速度为 `[-2500,5500,1000] m/s`。连续十步以 `srand(12345)` 执行，逐步比较 33 维状态、完整 `0x220` 主帧与 3000 字节 IPC payload。

| 取证或验收环节 | 产物／结论 |
|---|---|
| 原始环境向量 | `GetSunVector` 的跨日期直接金标与恢复端逐位一致；首步 `CoreDynamic` 的 DSS 输入太阳向量和位置向量也逐分量一致。 |
| 首次差异 | 状态均一致，主帧与 IPC 仅 `dss_valid[0..1]` 的两个字节不同。 |
| 反汇编根因 | 原 `vector2angle` 在完成点积累加后，以入口 `XMM2` 覆盖累加值；`UpdateDSS` 的正常 `blas_gemv` 末轮保留零值 `XMM2`，紧随其后的零参考向量调用形成 `0/0`、`acos(NaN)`。`ucomisd` 对 NaN 不跳转阴影支路，直接将两个 DSS 有效标志写为 `1`。 |
| 恢复策略 | `dp_update_dss` 仅为紧随其后的 `dp_update_dss_valid_flag` 发布一次性 legacy-NaN 标记；独立调用有效标志 API 时仍采用常规零向量／阴影几何路径，避免污染函数级 ABI。 |
| 十步验收 | 十步共 30 个比较块（状态、主帧、IPC）全部 0 字节差异。 |

比较器为 `analysis/dyn_main_global_orbit_time_boundary_ten_step_compare.c`，原 ELF 采集脚本为 `analysis/time_orbit/dyn_main_orbit_time_boundary_ten_step_probe.gdb`；所需金标位于 `analysis/time_orbit/gold_orbit_time_boundary_*`。该场景已经加入 `make selftest`。在从空 `build/` 目录开始的 `make clean && make CC=gcc AR=ar selftest` 中，共出现 **89 条 PASS**。

> 此证据仅覆盖上述固定高偏心、跨日期、三维位置速度、固定随机种子与十步时域。它确认了该调用序列下观察到的遗留寄存器效应，不能概括为任意调用约定、任意优化器、任意传感器构型或全输入空间的等价性。

## 闰日近地三维轨道十步环境回归（2026-08-20）

在高偏心跨日期场景之外，新增第二条彼此独立的日期／GMST／环境传感器路径。该场景从闰日 `2024-02-29 12:34:56` 起始，初始轨道根数为 `a=7,078,137 m`、`e=0.05`、`i=0.9 rad`、`Ω=1.2 rad`、`ω=0.7 rad`、`M=4.2 rad`；初态采用非共线四元数 `[0.5,0.5,-0.5,0.5]`、角速度 `[-0.006,0.011,-0.009] rad/s`、位置 `[6.5,-2.2,3.5]×10^6 m` 与速度 `[2000,6200,3000] m/s`。设备命令全部为零，随机状态固定为 `srand(12345)`，连续运行十步。

| 金标步骤 | 产物 | 结果 |
|---|---|---|
| 原 ELF 采集 | `gold_leap_day_leo_ten_step_state.bin`（2640 bytes）、主帧（5440 bytes）、IPC（30000 bytes） | 重复采集的三项 SHA-256 完全相同。 |
| 恢复端比较 | 每步 33 维状态、完整 `0x220` 主帧、3000 字节 IPC payload | 10 步共 30 个比较块均为 0 字节差异。 |
| 严格回归 | `analysis/dyn_main_global_leap_day_leo_ten_step_compare.c` | 已加入 `make selftest`。 |

原 ELF 探针为 `analysis/time_orbit/dyn_main_leap_day_leo_ten_step_probe.gdb`；该探针亦导出首步 DSS 对象及太阳／位置环境输入，供后续传感器分支审计。自空 `build/` 目录执行 `make clean && make CC=gcc AR=ar selftest` 后，严格门禁共输出 **90 条 PASS**。

> 本场景仅证明固定闰日、近地三维位置速度、给定轨道根数、零设备命令、固定种子及十步时域内的观测一致性。它不覆盖其它日期、根数退化、日食几何、异常 descriptor 或未采样控制命令。



## 单 DSS 噪声通道关闭十步随机路径回归（2026-08-20）

传感器对象审计首先确认默认零命令十步场景的两个 DSS `gaussian_noise_flag` 已均为 `1`；因此，仅重复写入该值所得状态、主帧及 IPC 金标与既有零命令金标 SHA-256 完全一致，未被计为新增覆盖。随后建立真正的非默认配置：在 `DynamicInit` 返回后、`srand(1)` 前，保持第 0 个 DSS 标志为 `1`，并将第 1 个 DSS 标志（原对象 `DSS+0x170`）显式置为 `0`。

| 验收项 | 结果 |
|---|---|
| 原 ELF 金标可重复性 | 状态、主帧与 IPC 十步文件重复采集 SHA-256 完全一致。 |
| 相对默认路径可观察性 | 33 维动力学状态金标保持一致；主帧与 IPC payload 的 SHA-256 改变，证明单通道开关形成了非重复的测量／遥测随机路径。 |
| 恢复端比较 | 每步 33 维状态、完整 `0x220` 主帧、3000 字节 IPC payload；十步共 30 个比较块均为 0 字节差异。 |
| 严格回归 | `analysis/dyn_main_global_dss0_noise_ten_step_compare.c` 已纳入 `make selftest`。 |

原 ELF 探针为 `analysis/time_orbit/dyn_main_dss0_noise_ten_step_probe.gdb`，金标以 `gold_dss0_noise_*` 命名。自空 `build/` 目录执行完整严格自检后，共有 **91 条 PASS**。

> 本证据仅覆盖默认 DSS 安装矩阵及均值／方差下的“通道 0 启用、通道 1 关闭”、固定 `srand(1)` 和十步时域。尚未验证通道 0 关闭、修改噪声参数、不同姿态／太阳机体系方向或更长时域。

## 反向单 DSS 噪声通道关闭十步回归（2026-08-20）

为避免仅验证一个通道的随机分支，新增互补配置：第 0 个 DSS `gaussian_noise_flag` 显式置为 `0`，第 1 个 DSS 保持 `1`。原 ELF 状态十步金标仍与默认路径相同，但主帧和 IPC payload 的 SHA-256 均与默认路径不同，说明测量／遥测路径确实发生了可观察变化。状态、主帧及 IPC 三类金标均被重复采集并验证确定性。

恢复端比较器 `analysis/dyn_main_global_dss1_noise_ten_step_compare.c` 按同一初始化时点配置两个标志；十步 30 个状态／主帧／IPC 比较块全部为 0 字节差异。该比较器和已有的通道 1 关闭比较器均已纳入 `make selftest`。从空 `build/` 目录执行完整严格自检，当前输出 **92 条 PASS**。

> 两个互补单通道配置仅覆盖默认投影矩阵与既有均值／方差、固定 `srand(1)`、零设备命令和十步时域；并不覆盖不同噪声参数、联合姿态／轨道环境或其他传感器开关组合。

## DSS 自定义高斯均值与方差十步回归（2026-08-20）

为覆盖随机测量分支的参数化调用，两个 DSS 通道均保持启用，并在 `DynamicInit` 返回后、`srand(1)` 前设置以下参数：通道 0 的 `(mean_x, mean_y, sigma_x, sigma_y)=(0.002,-0.003,0.020,0.030)`；通道 1 的相应参数为 `(-0.004,0.005,0.025,0.015)`。原 ELF 的状态十步金标与默认动力学状态保持一致，而主帧和 IPC payload 与默认路径不同；三类金标重复采集的 SHA-256 完全一致。

恢复端比较器 `analysis/dyn_main_global_dss_custom_noise_ten_step_compare.c` 写入相同八个双精度参数及双通道标志。十步共 30 个状态、主帧和 IPC payload 比较块均为 0 字节差异，并已加入 `make selftest`。从空 `build/` 目录完成完整严格自检后，当前输出 **93 条 PASS**。

> 该回归覆盖一个受控的非零均值、非默认方差组合，但不代表任意数值范围、分布实现替换、随机种子或更长时域已被验证。

## DSS 自定义参数第二随机种子十步回归（2026-08-20）

为避免仅在单一伪随机序列上验证参数化测量路径，沿用上一场景的两个 DSS 高斯均值与方差，改用 `srand(12345)`。原 ELF 十步状态、主帧和 IPC payload 金标均重复采集并获得完全相同的 SHA-256；恢复端在相同种子、参数、初态和设备命令下的 30 个比较块均为 0 字节差异。

比较器为 `analysis/dyn_main_global_dss_custom_noise_seed12345_ten_step_compare.c`，已纳入严格自检。自空 `build/` 目录完成 `make clean && make CC=gcc AR=ar selftest` 后，当前输出 **94 条 PASS**。

> 该项增加了第二条确定性随机序列证据，但仍不能将两个固定种子外推为全部随机状态、平台库或随机算法实现的等价性。

### 撤回的非共线 DSS 联合探针说明（2026-08-20）

曾以现有零命令顶层初始化顺序，在 `dyn_main` 调用前将输入状态四元数预写为 `[0.5,0.5,-0.5,0.5]`，同时施加自定义 DSS 高斯参数。两次原 ELF 金标采集均成功，但与相同参数的基线路径在 33 维状态、主帧和 IPC payload 上逐字节相同；首步输出四元数也均为接近单位四元数的同一数值。因此该探针不具备独立可观察性，相关临时脚本、比较器和金标已删除，**不计入覆盖矩阵或 PASS 总数**。根因仍应在后续通过 `dyn_main` 的状态装配／初始化调用链隔离审计后再下结论。

## dyn_main 状态输入 ABI 审计与全局 y 非共线初态回归（2026-08-20）

本轮反汇编与最小探针确定了顶层状态语义。`dyn_main(out, state_buffer, command)` 将 `state_buffer` 作为 `CoreDynamic` 的第一个参数传入；原 ELF `CoreDynamic` 在 RK4 返回后才将姿态、角速度、位置、速度及柔性状态写入该缓冲区。其读取的第二参数是由 `UpdateCoreDynInput` 在栈上装配的执行机构／模型输入。`DynamicInit` 则依据初始条件调用 `intergrator_init` 建立全局 RK4 状态 `y[33]`。因此，在 `dyn_main` 前预写 `state_buffer` 四元数并不注入积分初态；此结论也由先前已撤回的非共线探针逐字节同基线的结果支持。

为验证真实初态通路，原 ELF 探针 `analysis/time_orbit/dyn_main_global_y_noncollinear_two_step_probe.gdb` 在 `DynamicInit` 后直接将全局 `y[0..3]` 设为 `[0.5,0.5,-0.5,0.5]`，使用默认零命令与 `srand(1)` 连续运行两步。首步输出四元数为约 `[0.4998499900713309,0.49999998905502535,-0.5000999842324928,0.5000500016411613]`，证明非共线初态实际参与积分。状态（528 bytes）、主帧（1088 bytes）与 IPC payload（6000 bytes）金标重复采集 SHA-256 完全一致。

恢复端比较器 `analysis/dyn_main_global_y_noncollinear_two_step_compare.c` 在同一时点写入导出的全局 `y[0..3]`。两步共 6 个状态／主帧／IPC 比较块均为 0 字节差异，并已加入 `make selftest`。从空 `build/` 目录运行完整严格自检后，当前为 **95 条 PASS**。

> 该证据界定了此 ABI 下“状态输出缓冲”与“全局积分初态”的不同职责；它只验证给定四元数、零命令、两步时域和固定种子，不代表任意全局状态修改或多线程重入语义已被证明等价。

## 真实全局 y 非共线姿态与自定义 DSS 参数联合十步回归（2026-08-20）

在已审计的 ABI 下，联合场景在 `DynamicInit` 后直接设置全局 RK4 状态 `y[0..3]=[0.5,0.5,-0.5,0.5]`，并同时启用两个 DSS 通道、设置通道 0 参数 `(0.002,-0.003,0.020,0.030)` 与通道 1 参数 `(-0.004,0.005,0.025,0.015)`，固定 `srand(1)`、零设备命令并连续运行十步。原 ELF 首步输出四元数为非共线演化值，证明该场景同时经过真实姿态变换与参数化随机传感器链。

状态（2640 bytes）、主帧（5440 bytes）和 IPC payload（30000 bytes）原 ELF 金标均重复采集并通过 SHA-256 一致性检查。恢复端比较器 `analysis/dyn_main_global_y_dss_custom_ten_step_compare.c` 的十步 30 个状态／主帧／IPC 比较块均为 0 字节差异，已纳入 `make selftest`。空构建目录的完整严格回归当前输出 **96 条 PASS**。

> 该证据只覆盖一个真实全局非共线四元数、一个自定义 DSS 参数点、零命令、固定种子和十步时域；不能外推至其它全局初态、噪声参数、控制组合、线程模型或时间跨度。

## 真实全局 y 非共线姿态与非零柔性模态十步回归（2026-08-20）

现有柔性初态比较器曾将非零模态写入 `dyn_main` 的状态输出缓冲；本轮按已审计的 ABI 改为在 `DynamicInit` 后直接修改真实 RK4 状态 `y`。注入值为非共线姿态 `y[0..3]=[0.5,0.5,-0.5,0.5]`，以及柔性索引 `13,16,20,23,27,32` 分别为 `[0.0001,-0.0002,0.00005,0.001,-0.0005,0.00025]`。固定 `srand(1)`、零设备命令下，原 ELF 十步状态、主帧和 IPC payload 金标均重复采集并取得一致 SHA-256。

恢复端比较器为 `analysis/dyn_main_global_y_noncollinear_flex_ten_step_compare.c`。十步共 30 个状态／主帧／IPC 比较块均为 0 字节差异，已加入 `make selftest`。从空构建目录执行完整严格自检，当前为 **97 条 PASS**。

> 该回归验证了给定真实全局初态下的非零刚柔耦合十步轨迹；不表示其它柔性模式、幅值、阻尼参数、控制组合或长期稳定性已被穷尽验证。

## 真实全局 y 非共线姿态与多执行机构十步回归（2026-08-20）

本场景在 `DynamicInit` 后将真实 RK4 初态 `y[0..3]` 置为 `[0.5,0.5,-0.5,0.5]`，并复用已审计的十步组合命令 ABI：四飞轮正负扭矩、六通道 MTQ 命令、三次推进器使能及一次惯量旗标更新；飞轮 0 同时以接近正向速度边界的已验证状态预置。原 ELF 对状态、主帧、IPC payload、完整 RWheel、完整 MTQ、Thruster 标量／状态、四个向量计数及 lever/input/force/torque backing 分别采集金标。所有文件重复采集 SHA-256 一致。

Thruster 原始对象中的四个 `DpVector.data` 为进程地址相关指针，因而不将这些指针本身作为跨二进制数值语义验收项；比较器转而逐字节比较其标量字段、状态、向量计数与四个 3-double backing。恢复端比较器 `analysis/dyn_main_global_y_multi_actuator_ten_step_compare.c` 的 30 个状态／主帧／IPC 块，以及 RWheel、MTQ 与 Thruster 所有可移植数值终态块均为 0 字节差异。

该场景已加入 `make selftest`。空构建目录全量严格回归目前输出 **98 条 PASS**。

> 该项覆盖给定非共线全局初态、一个组合控制时序、十步时域和固定随机种子；不能据此推定所有控制幅值、通道排列、执行机构参数、长期轨迹或地址相关对象表示的全空间等价。

## 真实全局 y 非共线姿态与高偏心跨日期十步回归（2026-08-20）

在已闭环的高偏心跨日期环境中，保留 `2031-12-31 23:59:50`、`a=26,560,000 m`、`e=0.65`、固定 `srand(12345)` 与零设备命令，并在 `DynamicInit` 后将真实积分器状态 `y[0..3]` 设置为 `[0.5,0.5,-0.5,0.5]`。该场景同时覆盖跨日期时间、远地点附近环境计算、非共线机体姿态、太阳矢量转换与 DSS 有效性调度。

原 ELF 十步状态、主帧和 IPC payload 金标均重复采集并取得相同 SHA-256。恢复端比较器 `analysis/dyn_main_global_y_orbit_time_boundary_ten_step_compare.c` 的 30 个状态／主帧／IPC 比较块全部为 0 字节差异，并已经进入 `make selftest`。从空构建目录执行的完整严格回归当前输出 **99 条 PASS**。

> 该结果仅适用于已采样的轨道根数、日期、真实全局姿态、十步时域与固定随机种子；并不覆盖任意轨道、历元、初始姿态或光照几何。

## 真实全局 y 非共线姿态与闰日近地十步回归（2026-08-20）

在闰日近地环境中，保留 `2024-02-29 12:34:56`、`a=7,078,137 m`、`e=0.05`、固定 `srand(12345)` 与零设备命令，并在 `DynamicInit` 后直接设置真实全局 RK4 状态 `y[0..3]=[0.5,0.5,-0.5,0.5]`。该场景覆盖闰日日期转换、近地轨道环境、GMST／坐标链、非共线姿态变换及传感器调度。

原 ELF 状态、主帧和 IPC payload 十步金标均重复采集并取得一致 SHA-256。恢复端比较器 `analysis/dyn_main_global_y_leap_day_leo_ten_step_compare.c` 的 30 个状态／主帧／IPC 比较块均为 0 字节差异，并已纳入 `make selftest`。空构建目录完整严格回归当前输出 **100 条 PASS**。

> 该项仅验证一个闰日历元、一个近地轨道参数点、给定全局姿态、十步时域和固定随机种子；不得将结果外推到任意日期、轨道或姿态初值。

### 撤回的高偏心自定义 DSS 参数联合探针（2026-08-20）

在真实全局非共线姿态、高偏心跨日期环境下尝试设置两个 DSS 的非默认高斯均值与方差。原 ELF 两次采集均可重复，但与同一真实姿态环境基线相比，十步状态、主帧和 IPC payload 完全相同；仅首步 DSS 对象配置字段不同，未形成可观察的测量／随机输出分支。因此相关临时探针、比较器和金标已删除，**不计入覆盖矩阵或 PASS 总数**。后续随机测量覆盖应选择已知能产生正投影并使 DSS 输出改变的真实姿态／太阳几何组合。

## 真实全局 y 非共线姿态、SADA 位置命令与惯量旗标十步回归（2026-08-20）

在默认轨道环境中，`DynamicInit` 后将真实积分状态 `y[0..3]` 置为 `[0.5,0.5,-0.5,0.5]`，并持续施加 SADA 位置命令 `[0.01,-0.01]` 与 `inertia_update_flag=1`。该路径进入 SADA 运动、惯量模型选择及刚柔反作用计算链，而不再依赖仅预写 `dyn_main` 输出缓冲的无效初态方法。

原 ELF 十步状态、主帧和 IPC payload 金标重复采集 SHA-256 一致。恢复端比较器 `analysis/dyn_main_global_y_sada_flag1_ten_step_compare.c` 的 30 个状态／主帧／IPC 比较块均为 0 字节差异，并已纳入 `make selftest`。空构建目录完整严格回归当前输出 **101 条 PASS**。

> 该项覆盖一个 SADA 目标、连续惯量旗标、给定真实姿态和十步时域，不能外推为任意 SADA 命令、惯量历史或长期刚柔演化的全空间等价。

### 撤回的真实全局 y 柔性模态＋SADA 联合探针（2026-08-20）

在真实全局非共线姿态与连续 SADA／惯量旗标路径中，对 `y[13]`、`y[16]`、`y[20]`、`y[23]`、`y[27]`、`y[32]` 写入非零值。原 ELF 重复采集可复现，但十步状态、主帧与 IPC 金标均与同一无柔性基线逐字节相同。因此该预置在当前顶层调度和十步窗口内未产生可观察差异；临时探针、比较器及金标已删除，**不计入覆盖或 PASS 总数**。后续需要先以函数级或内存快照证据确认可驱动的柔性状态输入位置／使能条件，再扩大柔性初态测试。

## 柔性状态可观察性十步回归：完整全局 y 金标（2026-08-20）

柔性审计确认，顶层 `dyn_main` 的 `core_output` 状态缓冲未完整反映 RK4 全局状态的柔性部分；因此，仅比较该输出缓冲会漏检内部 `y[13..32]` 的演化。原 ELF 探针直接在 `DynamicInit` 后写入真实全局状态：模态位置段 `y[13..15]=[1.0,-0.5,0.25]`，模态速度段 `y[23..25]=[0.1,-0.2,0.3]`，并保持非共线四元数与 SADA／惯量旗标命令。调用前快照确认这些值已写入全局 `y`；首步后原 ELF 的模态速度已演化，证明柔性积分路径实际被执行，尽管标准主帧和 IPC 在该窗口内与基线相同。

探针每一步导出完整 `y[33]` 原 ELF 金标。恢复端 `analysis/dyn_main_flex_observability_ten_step_compare.c` 同时逐字节比较标准状态缓冲、主帧、IPC 和**每一步完整全局 `y[33]`**：40 个比较块均 0 字节差异。该场景已纳入 `make selftest`，空构建目录完整严格回归当前输出 **102 条 PASS**。

> 此项验证一个大幅度模态位置／速度初态、SADA／惯量旗标控制时序及十步内部状态演化；它不代表所有模态初态、刚柔参数、控制时序或长期稳定性的完整证明。

## 柔性内部状态二十步完整 `y[33]` 时域闭环（2026-08-21）

在既有柔性可观察性十步场景的**相同受控初态**上，将连续运行时间扩展至二十步。该场景保留非零柔性区、SADA 位置命令 `[+0.01,-0.01]` 与惯量旗标路径，使标准顶层状态、遥测和 IPC 之外原本不可由 `core_output` 完整表达的 `y[13..32]` 也成为逐步验收对象。原 ELF 探针 `analysis/time_orbit/dyn_main_flex_observability_twenty_step_probe.gdb` 每步依次调用 `dyn_main` 与 `sendDynTele`，并导出全局积分状态、CoreDynamic 输出缓冲、完整主帧及 IPC payload。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 | 用途 |
|---|---:|---|---|
| `gold_flex_observability_twenty_step_global_y.bin` | 5,280 bytes | `a714926aaf104efbb7ddb291acf2a8e7f5c9b010d25973e2fab81d3790793823` | 每步真实全局积分状态 `y[33]` |
| `gold_flex_observability_twenty_step_state.bin` | 5,280 bytes | `3e283297d39f16b3ed47b6ce9682b6fea0b4bf7c136be2ac30608c56299613be` | 每步 `CoreDynamic` 前 `0x108` bytes |
| `gold_flex_observability_twenty_step_out.bin` | 10,880 bytes | `d24f9939ecfbea40d2d09e485373ed2cb1524ec933feba1e35858d4dfc3fc170` | 每步完整 `0x220` 主输出帧 |
| `gold_flex_observability_twenty_step_ipc_payload.bin` | 60,000 bytes | `f1803d73b465043cae7d520a930dfb3d5f0da8f0ce4876d107bb0aca537ebc4c` | 每步 3,000-byte IPC payload |

恢复端比较器 `analysis/dyn_main_flex_observability_twenty_step_compare.c` 对每一步依次比较四类对象。因此本场景共有 80 个独立比较块：二十个 CoreDynamic 状态块、二十个完整主帧块、二十个 IPC payload 块和二十个完整全局 `y[33]` 块；全部为 **0 字节差异**。其中全局 `y[33]` 的结果直接覆盖柔性位置和速度内部状态，避免将顶层输出缓冲误当作 RK4 积分初态或完整内部状态镜像。

该比较器已加入 `Makefile` 的 `selftest`。从空构建目录执行 `make clean && make CC=gcc AR=ar selftest` 后，严格门禁共输出 **103 条 PASS**，且无编译错误、运行失败或非零差异标记。

> 此项证据仅适用于上述离线固定初态、非零柔性幅值、SADA／惯量命令路径、随机状态与二十步时域。它进一步增强已采样路径的行为等价证据，但不构成任意姿态、任意柔性初态、任意时域或安全关键用途的正确性保证。

## 下一优先级覆盖方向

后续 P2 将以新的真实全局 `y[33]` 初态扩展姿态与柔性耦合的多点覆盖：优先选取不同于现有 `[0.5,0.5,-0.5,0.5]` 的规范化四元数，随后在不改变已验收设备 ABI 的前提下增大并重分配柔性模态位置／速度幅值。每个单元继续遵循“原 ELF 重复金标采集、最小 C11 比较器、逐字节比较、空构建目录完整回归”的顺序；空 descriptor、计数不匹配和无效命令等异常 ABI 留作 P3，单独隔离，避免污染正常路径金标。

[1]: ../time_orbit/dyn_main_flex_observability_twenty_step_probe.gdb
[2]: ../dyn_main_flex_observability_twenty_step_compare.c
[3]: ../../Makefile

## 正交姿态与全分量高幅值柔性十步 P2 闭环（2026-08-21）

为避免仅在既有非共线姿态点和稀疏模态初态上得到一致结论，新增独立 P2 场景。积分初态直接写入原 ELF 与恢复端的全局 `y[33]`：姿态取规范化四元数 `[0.5,-0.5,0.5,0.5]`。其与既有 `[0.5,0.5,-0.5,0.5]` 的内积为零，因而提供了一个正交的姿态变换采样点。柔性位置 `y[13..22]` 设为 `[2,-1,0.5,-3,1.5,-0.75,0.375,-0.1875,0.09375,-0.046875]`，柔性速度 `y[23..32]` 设为 `[0.2,-0.4,0.6,-0.8,0.4,-0.2,0.1,-0.05,0.025,-0.0125]`，以完整覆盖二十个柔性内部量；其余初始条件、SADA 位置命令 `[+0.01,-0.01]`、惯量旗标和 `srand(1)` 与已审计柔性路径保持一致。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 | 逐步验收对象 |
|---|---:|---|---|
| `gold_altq_flex_amplitude_ten_step_global_y.bin` | 2,640 bytes | `853b5c2bca924a792a69afcea4423bd781160d91fb43a5b87cac90e83414cfb2` | 完整全局积分状态 `y[33]` |
| `gold_altq_flex_amplitude_ten_step_state.bin` | 2,640 bytes | `d55240cc6682827d42a9f6f89000055f024524af9fbdc09133403bdbf0809b2e` | CoreDynamic 前 `0x108` bytes |
| `gold_altq_flex_amplitude_ten_step_out.bin` | 5,440 bytes | `48c0b825093650d31533154c556903caecd4999809203a0cd2c859a8d4e5262b` | 完整 `0x220` 主输出帧 |
| `gold_altq_flex_amplitude_ten_step_ipc_payload.bin` | 30,000 bytes | `17f0a32a8b4d9ddf54ca32facbf6091b741390b90b65155c8b66f7d4921e9de4` | 3,000-byte IPC payload |

原 ELF 探针为 `analysis/time_orbit/dyn_main_altq_flex_amplitude_ten_step_probe.gdb`，恢复端比较器为 `analysis/dyn_main_altq_flex_amplitude_ten_step_compare.c`。十步中的四类对象共 40 个比较块均为 **0 字节差异**；特别地，每一步的完整 `y[13..32]` 已直接纳入位级比较，而非仅由顶层 `core_output` 间接观察。比较器已写入 `Makefile` 严格门禁。从空构建目录执行 `make clean && make CC=gcc AR=ar selftest` 后，得到 **104 条 PASS**，无编译错误、运行失败或非零差异标记。

> 本结果只增加一个明确的正交姿态点、固定二十维柔性幅值分布、既定命令与十步时域下的离线行为证据。它不证明任意四元数、任意柔性幅值、长期运行或安全关键用途的正确性；后续仍应扩展随机、环境／阴影和异常 ABI 路径。

[4]: ../time_orbit/dyn_main_altq_flex_amplitude_ten_step_probe.gdb
[5]: ../dyn_main_altq_flex_amplitude_ten_step_compare.c

## 正交姿态与全分量高幅值柔性：第二随机种子十步闭环（2026-08-21）

在同一正交姿态和全二十维柔性初态的 P2 场景上，新增 `srand(12345)` 随机路径，独立于原 `srand(1)` 金标。原 ELF 的刚柔积分状态与 CoreDynamic 输出在两个种子下保持一致，而传感器噪声驱动的主帧和 IPC 金标改变；这与动力学不消费传感器随机数、测量与遥测路径消费随机数的已审计调度相一致。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 | 结果 |
|---|---:|---|---|
| `gold_altq_flex_amplitude_seed12345_ten_step_global_y.bin` | 2,640 bytes | `853b5c2bca924a792a69afcea4423bd781160d91fb43a5b87cac90e83414cfb2` | 与种子 1 相同的完整 `y[33]` 轨迹，恢复端逐步 0 字节差异 |
| `gold_altq_flex_amplitude_seed12345_ten_step_state.bin` | 2,640 bytes | `d55240cc6682827d42a9f6f89000055f024524af9fbdc09133403bdbf0809b2e` | 与种子 1 相同的 CoreDynamic 输出，恢复端逐步 0 字节差异 |
| `gold_altq_flex_amplitude_seed12345_ten_step_out.bin` | 5,440 bytes | `112dce8674ad96e84202f65d5db7ab8add93f98ed3bd2be39dbd93466f47cb04` | 独立随机主帧，恢复端逐步 0 字节差异 |
| `gold_altq_flex_amplitude_seed12345_ten_step_ipc_payload.bin` | 30,000 bytes | `28b16915e84bb5a64dfb639004e441746f41f4fb7284d5ef4157afd5f7acd46b` | 独立随机 IPC payload，恢复端逐步 0 字节差异 |

原 ELF 探针为 `analysis/time_orbit/dyn_main_altq_flex_amplitude_ten_step_seed12345_probe.gdb`，比较器为 `analysis/dyn_main_altq_flex_amplitude_ten_step_seed12345_compare.c`。十步完整 `y[33]`、CoreDynamic、主帧和 IPC 共 40 个比较块全部为 **0 字节差异**。该比较器已经纳入严格自检；空构建目录下的 `make clean && make CC=gcc AR=ar selftest` 输出 **105 条 PASS**。

> 此结论仅覆盖两个明确的 C 库随机种子、记录的正交姿态与柔性初态、同一设备命令和十步离线时域。它不证明随机数种子空间、传感器构型或任意运行时间上的全面等价。

[6]: ../time_orbit/dyn_main_altq_flex_amplitude_ten_step_seed12345_probe.gdb
[7]: ../dyn_main_altq_flex_amplitude_ten_step_seed12345_compare.c

## P3：非致命 vector descriptor 计数异常 ABI 闭环（2026-08-21）

为将异常 ABI 覆盖从正常顶层路径中隔离，新增基础向量函数单元。探针 `analysis/time_orbit/vector_descriptor_nonfatal_p3_probe.gdb` 只使用有效三元素 backing，改变 descriptor 的计数字段；因此不会把无效地址访问、堆布局或进程崩溃混入本项验收。它覆盖左计数为 3、右计数为 2 的 `vector_add`、`vector_memcpy` 与 `vector_dot`，以及两侧均为 `{ count=0, data=NULL }` 的 `vector_add`、`vector_scale`、`vector_dot`。

原 ELF `vector_memcpy`（`0x9850`）反汇编显示其先读取**源** `count`，仅在源计数为零时跳过循环；非零时不读取目标计数，按源计数从 `src->data` 向 `dst->data` 拷贝，最后无条件返回零。先前恢复实现错误要求两者计数相等，导致该异常 ABI 的返回码与写回均不一致。现已改为按 `uint32_t src->count` 循环并恒返回零，保留原 ELF 对非零源计数的无检查语义。

| 原 ELF 金标 | SHA-256 | 受控观察 |
|---|---|---|
| `gold_vector_descriptor_nonfatal_p3_results.bin`（48 bytes） | `68df9b386781708ec7c3b585b93f13629cbcbed3287eb0f4d6e29180b50ba24b` | 计数不匹配下 `vector_add=-1`、`vector_memcpy=0`、`vector_dot=-1`；零计数三项均返回 `0`，且零计数 dot 写入 `0.0`。 |
| `gold_vector_descriptor_nonfatal_p3_lhs_data.bin`（24 bytes） | `5799c51a9a8cbc523f41de4762f1cbdc3dc5e85027488539ea29a22f34f17d3e` | `vector_memcpy` 在目标计数为 3、源计数为 2 时仅写入前两元素，终态为 `[10,20,3]`。 |

原 ELF 金标重复采集 SHA-256 完全一致。最小恢复端比较器 `analysis/vector_descriptor_nonfatal_p3_gold_compare.c` 对 48-byte 结果对象和 24-byte目标向量逐字节比较，结果均为 **0 字节差异**。其已加入 `Makefile` 的严格自检；从空构建目录执行 `make clean && make CC=gcc AR=ar selftest` 后，共得到 **106 条 PASS**。

> 此 P3 单元仅刻画具有有效 backing 的计数不匹配和零计数 descriptor 调用。此前的无效 `data=NULL` 且正计数 `vector_memcpy` 原 ELF 探针会在解引用时触发 `SIGSEGV`，因此不将该进程终止路径混入正常运行回归；若需复现该类错误，应另行使用隔离子进程的死亡测试，不得在顶层仿真路径执行。

[8]: ../time_orbit/vector_descriptor_nonfatal_p3_probe.gdb
[9]: ../vector_descriptor_nonfatal_p3_gold_compare.c

### P3 补充：正计数空源 descriptor 的隔离终止语义

对 `vector_memcpy(dst={count=2,data=valid}, src={count=2,data=NULL})` 的原 ELF GDB 隔离调用，在进入 `vector_memcpy` 后于其源 backing 解引用处收到 `SIGSEGV`。这与 `0x9850` 的反汇编完全一致：该函数只测试源 `count` 是否为零，正计数时立即加载 `src->data`，不含空指针防护。

恢复端现以独立子进程运行同一调用；`analysis/vector_memcpy_null_source_p3_death_test.c` 仅在子进程收到 `SIGSEGV` 时通过，避免使正常仿真或总回归进程终止。该隔离测试已加入 `Makefile`。在空构建目录执行 `make clean && make CC=gcc AR=ar selftest` 后，完整严格门禁为 **107 条 PASS**，同时包括非致命计数金标和该死亡测试。

> 该条目只复现该单一函数、正计数、空源数据指针所观察到的 Linux 信号边界；并不对其它空对象、映射页、信号处理器、优化级别或整个程序的异常恢复策略作概括。

[10]: ../vector_memcpy_null_source_p3_death_test.c

## P3：矩阵 descriptor 形状不匹配非致命 ABI 闭环（2026-08-21）

在 vector descriptor 单元之后，进一步隔离 `matrix_memcpy` 的矩阵形状异常。原 ELF `matrix_memcpy`（`0x8970`）反汇编表明其读取 `src.rows`、`src.cols`、`src.row_stride` 和 `dst.row_stride`，但**不读取也不验证** `dst.rows` 或 `dst.cols`；外层循环由源行数控制，内层循环由源列数控制，目标索引采用目标行步长。

探针 `analysis/time_orbit/matrix_descriptor_nonfatal_p3_probe.gdb` 设置源 `{rows=2, cols=2, row_stride=3}`，目标 `{rows=3, cols=1, row_stride=4}`，并为双方提供充足有效 backing。原 ELF 无条件返回 `0`，并将源可见元素写到目标 offsets `0,1,4,5`，其他八个 `-1.0` sentinel 保持不变。先前恢复端错误要求行列相等并返回 `-1`；现已取消该形状检查，按反汇编的源维度／各自步长循环复现。

| 原 ELF 金标 | SHA-256 | 恢复端验收 |
|---|---|---|
| `gold_matrix_descriptor_nonfatal_p3_result.bin`（8 bytes） | `af5570f5a1810b7af78caf4bc70a660f0df51e42baf91d4de5b2328de0e83dfc` | 返回对象逐字节一致。 |
| `gold_matrix_descriptor_nonfatal_p3_destination.bin`（96 bytes） | `704a2ce0195a319eaf9894bd5d0405b7f25ca77be02f9049b72aea84134d1acb` | 完整十二元素目标 backing 逐字节一致。 |

两项金标重复采集 SHA-256 一致。比较器 `analysis/matrix_descriptor_nonfatal_p3_gold_compare.c` 已加入严格自检；空构建目录运行 `make clean && make CC=gcc AR=ar selftest` 后输出 **108 条 PASS**，无编译错误、运行失败或非零差异标记。

> 此单元仅覆盖具有有效 backing 的指定矩阵行列／步长不匹配。负维度、空 backing、重叠区域和其它矩阵函数的异常行为尚未验收，不能据此推广。

[11]: ../time_orbit/matrix_descriptor_nonfatal_p3_probe.gdb
[12]: ../matrix_descriptor_nonfatal_p3_gold_compare.c

## 正交姿态、全分量高幅值柔性与第二随机种子二十步闭环（2026-08-21）

将已通过的正交姿态 `[0.5,-0.5,0.5,0.5]`、全二十维非零柔性状态、SADA 位置命令、连续惯量旗标和 `srand(12345)` 场景由十步延长至二十步。该单元同时扩展了刚柔 RK4 积分、SADA／惯量历史、传感器随机数消耗及 IPC 映射的时域，并继续把完整全局 `y[33]` 作为每步独立验收对象。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_altq_flex_amplitude_seed12345_twenty_step_global_y.bin` | 5,280 bytes | `61e725723c81a4ec51dc91839fb7fc28804a43194b934fd662844d61f8a6fd7d` |
| `gold_altq_flex_amplitude_seed12345_twenty_step_state.bin` | 5,280 bytes | `01568bf577104872b9330dab7fa8c068ac7cd4dc64cc26e52163736e947d9cfc` |
| `gold_altq_flex_amplitude_seed12345_twenty_step_out.bin` | 10,880 bytes | `010b14947f69f777f4e3d5f6758a5a632648c3c372cf9d27dfba324a54f2d55b` |
| `gold_altq_flex_amplitude_seed12345_twenty_step_ipc_payload.bin` | 60,000 bytes | `6668814c934270acad0f1e25237dc62cd57912a2feaaf60e635ed5b9e338c480` |

原 ELF 探针 `analysis/time_orbit/dyn_main_altq_flex_amplitude_seed12345_twenty_step_probe.gdb` 与恢复端比较器 `analysis/dyn_main_altq_flex_amplitude_seed12345_twenty_step_compare.c` 在二十步中比较完整 `y[33]`、CoreDynamic 前 `0x108`、完整 `0x220` 主帧和 3000-byte IPC payload。总计 80 个比较块全部为 **0 字节差异**。比较器已加入严格自检；空构建目录下的 `make clean && make CC=gcc AR=ar selftest` 输出 **109 条 PASS**。

> 此记录限定于指定正交姿态、全分量柔性幅值、固定设备命令、`srand(12345)` 与二十步离线时域。它不能替代其它姿态、环境、模态幅值、随机序列或更长运行期的独立验证。

[13]: ../time_orbit/dyn_main_altq_flex_amplitude_seed12345_twenty_step_probe.gdb
[14]: ../dyn_main_altq_flex_amplitude_seed12345_twenty_step_compare.c

## 高偏心跨日期＋正交姿态＋全柔性状态联合闭环及 DSS 分支精化（2026-08-21）

新增十步环境联合单元：历元 `2031-12-31 23:59:50`、`a=26,560,000 m`、`e=0.65`、三维初始位置／速度、正交规范化四元数 `[0.5,-0.5,0.5,0.5]`、全二十维高幅值柔性 `y[13..32]`、零设备命令、`srand(12345)`。探针与恢复端均将真实全局 `y` 注入 `DynamicInit` 后的积分状态，而非误将 `dyn_main` 状态缓冲作为积分初态。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_high_ecc_altq_fullflex_ten_step_global_y.bin` | 2,640 bytes | `0e4a70bdb5dc32e217bb6060c55e6d0083c06c5b31299a0671f563e958bafaf0` |
| `gold_high_ecc_altq_fullflex_ten_step_state.bin` | 2,640 bytes | `b8110d6c0da7ce5d9fd8ea1e82b53f3b1c87a06ac096502f02bafadc28d4becd` |
| `gold_high_ecc_altq_fullflex_ten_step_out.bin` | 5,440 bytes | `424867719251d9d071be3c14e1aca8c58880fca2aa54931050f9cf023282ffda` |
| `gold_high_ecc_altq_fullflex_ten_step_ipc_payload.bin` | 30,000 bytes | `53e16ef31f4fccbd8e03117b19f000a8a9527c30692bf4d6773ed3800d0149b3` |

初版恢复比较仅在主帧 `0xc0/0xc4` 与 IPC `0x968/0x969` 发生差异，均为两个 DSS 有效标志。两条场景的首步太阳 GCI 与位置 GCI 输入逐字节相同；进一步只读导出原 ELF 的 DSS measure backing，发现既有高偏心基线第二通道投影 `z=-1.3317228598024435e11`，新正交姿态联合场景归一化后为 `[−0.4325093027968096,−0.18492676549067608,1]`。由此将先前过宽的“无条件 legacy NaN”实现精化为已观察的末通道归一化小视场例外；所有既有 109 条门禁仍保持通过，新联合单元也在十步中每步完整 `y[33]`、CoreDynamic 前 `0x108`、完整主帧与 IPC 共 40 个比较块 **0 字节差异**。

比较器 `analysis/dyn_main_high_ecc_altq_fullflex_ten_step_compare.c` 已纳入 Makefile。空构建目录执行 `make clean && make CC=gcc AR=ar selftest` 输出 **110 条 PASS**，没有非零比较块。

> DSS 例外条件目前只由上述两条高偏心原 ELF 调用链及历史门禁约束。它是对可观察 ABI 副作用的有限复现，尚不构成对任意安装矩阵、太阳矢量、投影边界、编译器寄存器分配或异常 descriptor 的一般定理。

[15]: ../time_orbit/dyn_main_high_ecc_altq_fullflex_ten_step_probe.gdb
[16]: ../dyn_main_high_ecc_altq_fullflex_ten_step_compare.c

## 闰日近地＋正交姿态＋全柔性状态联合闭环（2026-08-21）

按覆盖矩阵下一优先级，在独立近地环境中复放正交四元数 `[0.5,-0.5,0.5,0.5]`、全二十维高幅值柔性 `y[13..32]`、零设备命令与 `srand(12345)`。环境初值为 `2024-02-29 12:34:56`、`a=7,078,137 m`、`e=0.05`、`i=0.9`、三维位置／速度与体角速率来自既有闰日近地基线。该单元与高偏心联合单元互补，覆盖不同时间转换、轨道尺度、太阳／磁场几何和 DSS 投影输入。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_leap_day_altq_fullflex_ten_step_global_y.bin` | 2,640 bytes | `42ac6e22d48347496a091a136333889c50a027a2b7a962e72196f87445e86165` |
| `gold_leap_day_altq_fullflex_ten_step_state.bin` | 2,640 bytes | `391189e815932815dada32a0869c07967781b234da0c0f83e0422ceb7ff07515` |
| `gold_leap_day_altq_fullflex_ten_step_out.bin` | 5,440 bytes | `18bcc439bc626296c9c167280f3a004f4e5a0f028eb005739f99b0764e563ea8` |
| `gold_leap_day_altq_fullflex_ten_step_ipc_payload.bin` | 30,000 bytes | `2026c6693fa7face021c8f4ce449eba6e33e839e96351369547f675eb187c4ff` |

原 ELF 探针 `analysis/time_orbit/dyn_main_leap_day_altq_fullflex_ten_step_probe.gdb` 重复采集四类聚合金标 SHA-256 一致。恢复端比较器 `analysis/dyn_main_leap_day_altq_fullflex_ten_step_compare.c` 在每步比较完整全局 `y[33]`、CoreDynamic 前 `0x108`、完整 `0x220` 主帧和 3000-byte IPC payload；十步 40 个比较块均为 **0 字节差异**。比较器已纳入 Makefile；空构建目录运行 `make clean && make CC=gcc AR=ar selftest` 后输出 **111 条 PASS**，没有非零比较块。

> 这验证指定闰日近地、姿态、柔性幅值、随机种子和十步时域的可观察行为；不外推至所有历元、轨道根数、姿态、柔性模型、传感器配置或长期运行。

[17]: ../time_orbit/dyn_main_leap_day_altq_fullflex_ten_step_probe.gdb
[18]: ../dyn_main_leap_day_altq_fullflex_ten_step_compare.c

## 高偏心跨日期＋正交姿态＋全柔性状态二十步长时域闭环（2026-08-21）

在已通过的高偏心跨日期环境联合十步单元上，仅将时域延长至二十步，保持 `2031-12-31 23:59:50`、高偏心轨道、正交四元数、全二十维高幅值柔性、零设备命令及 `srand(12345)` 不变。原 ELF 采用独立探针重复采集，四类聚合金标哈希一致。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_high_ecc_altq_fullflex_twenty_step_global_y.bin` | 5,280 bytes | `e1117818f664693c7d00063a90cd2b4d60a6a720668850d2b3cc75636e681535` |
| `gold_high_ecc_altq_fullflex_twenty_step_state.bin` | 5,280 bytes | `b97178c94bed0fca721717b53016868b883a4ee182434a871f9dfba2f403de68` |
| `gold_high_ecc_altq_fullflex_twenty_step_out.bin` | 10,880 bytes | `56759d97f0f40496abbf3a1d5a6b8a0e987d79e6ec3ca9be070d71dddb8cc7a9` |
| `gold_high_ecc_altq_fullflex_twenty_step_ipc_payload.bin` | 60,000 bytes | `3c7e728040592ea65e26d860ac13f19a2cf86782e21eafdf555d190c0f5514a8` |

恢复端比较器 `analysis/dyn_main_high_ecc_altq_fullflex_twenty_step_compare.c` 每步检查完整 `y[33]`、CoreDynamic 前 `0x108`、完整 `0x220` 主帧和 3000-byte IPC payload；二十步合计 **80 个比较块为 0 字节差异**。比较器已纳入 `Makefile`。空构建目录运行完整严格自检后，门禁为 **112 条 PASS**，没有非零比较块。

> 这一结果增加了指定高偏心环境和指定内部状态下的时域证据，但不外推到未采样的长期运行、轨道几何、柔性幅值或传感器配置。

[19]: ../time_orbit/dyn_main_high_ecc_altq_fullflex_twenty_step_probe.gdb
[20]: ../dyn_main_high_ecc_altq_fullflex_twenty_step_compare.c

## 闰日近地＋正交姿态＋全柔性状态二十步长时域闭环（2026-08-21）

将闰日近地环境联合单元由十步扩展至二十步，保持 `2024-02-29 12:34:56`、`a=7,078,137 m`、`e=0.05`、正交四元数、全二十维高幅值柔性、零设备命令及 `srand(12345)` 不变。原 ELF 重复采集证明四类聚合金标稳定。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_leap_day_altq_fullflex_twenty_step_global_y.bin` | 5,280 bytes | `59f4ae3d114e164799c9149b56357804f7086d27149cb388163fc8cb2eeebbae` |
| `gold_leap_day_altq_fullflex_twenty_step_state.bin` | 5,280 bytes | `fd7e0f028d8aa9b605b61a4ff85e1d68cdd0630cbf84ba7d4466d1024b35fbd0` |
| `gold_leap_day_altq_fullflex_twenty_step_out.bin` | 10,880 bytes | `ca1fb11672569c4a7bb0b909583adf04dad6be4323b063b6c262fa396a8eeffa` |
| `gold_leap_day_altq_fullflex_twenty_step_ipc_payload.bin` | 60,000 bytes | `72388d1850daabd5a6dbf0ef03e437f3a1bdcd8f35f379cc4e04533428888f80` |

`analysis/dyn_main_leap_day_altq_fullflex_twenty_step_compare.c` 在连续二十步中逐步比较完整 `y[33]`、CoreDynamic 前 `0x108`、完整 `0x220` 主帧和 3000-byte IPC payload；总计 **80 个比较块均为 0 字节差异**。比较器已纳入严格自检。空构建目录运行 `make clean && make CC=gcc AR=ar selftest` 后为 **113 条 PASS**，且没有非零比较块。

> 该结果是指定闰日近地环境、指定姿态／柔性状态和二十步运行期的有限位级证据；不能代替其它环境、输入或更长运行期的验证。

[21]: ../time_orbit/dyn_main_leap_day_altq_fullflex_twenty_step_probe.gdb
[22]: ../dyn_main_leap_day_altq_fullflex_twenty_step_compare.c

## 高偏心跨日期＋第三正交姿态＋反相全柔性十步闭环（2026-08-21）

新增不同于前两点的规范化四元数 `[0.5,0.5,0.5,-0.5]`，其与早期非共线基准 `[0.5,0.5,-0.5,0.5]` 内积为零；同时将全二十维高幅值柔性状态逐分量反相。环境保持高偏心跨日期路径，设备命令为零、随机序列为 `srand(12345)`。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_high_ecc_thirdq_inverseflex_ten_step_global_y.bin` | 2,640 bytes | `46182d4b5f9133493761cf1839de6f32d0452d2454f633226cbb877ae990d2f3` |
| `gold_high_ecc_thirdq_inverseflex_ten_step_state.bin` | 2,640 bytes | `d8d3d6e390d2554fac645ef9dbfc261ab3499c9d9b80886db21d09f446ac6505` |
| `gold_high_ecc_thirdq_inverseflex_ten_step_out.bin` | 5,440 bytes | `1c0292ded9c92a6f441a9d05b6e816ec47657cf9c76b0428177c532c78782ea0` |
| `gold_high_ecc_thirdq_inverseflex_ten_step_ipc_payload.bin` | 30,000 bytes | `6f3b3667bdca69fde5740c01a3d90a5926c4e6d57e428b2b78259489f0b48a9e` |

比较器 `analysis/dyn_main_high_ecc_thirdq_inverseflex_ten_step_compare.c` 在十步中比较完整 `y[33]`、CoreDynamic 前 `0x108`、完整主帧和 IPC payload；40 个比较块均 **0 字节差异**。已接入 Makefile。空构建目录完整自检为 **114 条 PASS**，没有非零比较块。

> 该场景增加了姿态和柔性符号分布的离散覆盖，但仍仅证明所列受控初态和十步时域上的行为一致性。

[23]: ../time_orbit/dyn_main_high_ecc_thirdq_inverseflex_ten_step_probe.gdb
[24]: ../dyn_main_high_ecc_thirdq_inverseflex_ten_step_compare.c

## 闰日近地背日阴影＋正交姿态＋全柔性状态十步闭环（2026-08-21）

新增顶层阴影几何单元。闰日近地环境与正交姿态、全二十维高幅值柔性、零设备命令和 `srand(12345)` 保持不变；`DynamicInit` 后将真实全局积分位置设为 `[-6,600,000, 2,200,000, 1,000,000] m`，即相对于当日原 ELF Sun GCI 向量的背日近地视轴，速度设为 `[1000,5000,0] m/s`。这避免了将 `dyn_main` 输出状态缓冲误当作积分输入。

首步原 ELF 主帧偏移 `0xc0/0xc4` 的两个 DSS 有效标志均为 `1`。随后与同姿态无阴影场景直接对照，二者也均为 `1`：这说明当前闰日正交姿态调用链仍受已知 DSS legacy NaN 有效标志副作用支配，**不能**将该顶层标志表述为地影几何分支已独立可观察。该场景仍以完整 DSS 快照、状态、主帧和 IPC 金标固定背日近地位置进入环境与传感器输入的行为。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_leap_day_shadow_altq_fullflex_ten_step_global_y.bin` | 2,640 bytes | `a629c5c38dd523a704597f1191f243a7790d085bc7fd5a0a0f088035c150e6b0` |
| `gold_leap_day_shadow_altq_fullflex_ten_step_state.bin` | 2,640 bytes | `648a66016432c08a6ea0def69c6790dfb3b9babdb88dcdc3fc06421587ecec5a` |
| `gold_leap_day_shadow_altq_fullflex_ten_step_out.bin` | 5,440 bytes | `15477fee3a541c980e6c2e0d5ef1b7ba5c87360cd32a3b998ebc8004ddff0279` |
| `gold_leap_day_shadow_altq_fullflex_ten_step_ipc_payload.bin` | 30,000 bytes | `efda7c37f8c173bbd046c9bc2fddab3c7667551a5756888550a8f1aec076155a` |

比较器 `analysis/dyn_main_leap_day_shadow_altq_fullflex_ten_step_compare.c` 在连续十步逐步比较完整 `y[33]`、CoreDynamic 前 `0x108`、完整 `0x220` 主帧和 3000-byte IPC payload；40 个比较块均为 **0 字节差异**。该比较器已纳入 Makefile。空构建目录完整严格自检为 **115 条 PASS**，没有非零比较块。

> 该单元证明指定背日近地构造下的顶层状态、环境与传感器输入行为，与独立 `isEarthShadow` 函数级金标互补；由于 DSS legacy 标志掩蔽，尚未独立证明顶层有效标志的地影分支。它也不取代对地影边界附近、半影模型或其它姿态／传感器安装矩阵的连续域验证。

[25]: ../time_orbit/dyn_main_leap_day_shadow_altq_fullflex_ten_step_probe.gdb
[26]: ../dyn_main_leap_day_shadow_altq_fullflex_ten_step_compare.c

## 闰日近地背日阴影＋DSS 通道 0 噪声十步闭环（2026-08-21）

在已通过的背日近地、正交姿态、全柔性阴影场景上，按已验收 ABI 偏移于 `DynamicInit` 后设置 `DSS[0].gaussian_noise_flag=1`、`DSS[1].gaussian_noise_flag=0`，保持 `srand(12345)`、环境、真实积分位置／速度、姿态和柔性初态不变。该单元同时覆盖单通道随机测量、阴影几何、DSS 有效标志与 IPC 打包。

| 原 ELF 聚合金标 | 尺寸 | 重复采集 SHA-256 |
|---|---:|---|
| `gold_leap_day_shadow_dss0_noise_ten_step_global_y.bin` | 2,640 bytes | `a629c5c38dd523a704597f1191f243a7790d085bc7fd5a0a0f088035c150e6b0` |
| `gold_leap_day_shadow_dss0_noise_ten_step_state.bin` | 2,640 bytes | `648a66016432c08a6ea0def69c6790dfb3b9babdb88dcdc3fc06421587ecec5a` |
| `gold_leap_day_shadow_dss0_noise_ten_step_out.bin` | 5,440 bytes | `d5c0822293c5ee2c29ce1379ca649730f53c45731ab404822ee5f2673c0a61c6` |
| `gold_leap_day_shadow_dss0_noise_ten_step_ipc_payload.bin` | 30,000 bytes | `c9fbb901460e8114b2e8b8b9dcb1e6297bb6f8846717004297372db3cb5e1a05` |

动力学聚合金标与无噪声阴影场景一致，主帧与 IPC 金标则不同，表明此开关确实进入可观察随机测量／打包路径。恢复比较器 `analysis/dyn_main_leap_day_shadow_dss0_noise_ten_step_compare.c` 在十步中检查完整 `y[33]`、CoreDynamic 前 `0x108`、完整主帧与 IPC；40 个比较块均为 **0 字节差异**。比较器已纳入 Makefile，空构建目录完整严格自检为 **116 条 PASS**。

> 此结果仅覆盖所述单通道噪声旗标、种子和背日几何；不外推至其他传感器、噪声参数、种子或光照边界。

[27]: ../time_orbit/dyn_main_leap_day_shadow_dss0_noise_ten_step_probe.gdb
[28]: ../dyn_main_leap_day_shadow_dss0_noise_ten_step_compare.c

## 进行中：高偏心反太阳位置的常规 DSS 地影分支与轨道 ULP 偏差

为避开闰日场景中 DSS legacy NaN 对有效标志的掩蔽，使用已验证的高偏心正交姿态 `[0.5,-0.5,0.5,0.5]`（其无阴影首步 DSS 标志为 `0,0`）将真实全局位置设为近似反太阳高轨 `[-12,000,000,65,000,000,28,000,000] m`。原 ELF 首步 DSS 标志变为 `1,1`，因此该设计已在原程序中使常规地影几何分支对顶层输出可见。四类十步原 ELF 聚合金标已重复采集，SHA-256 分别为：

| 文件 | SHA-256 |
|---|---|
| `gold_high_ecc_shadow_altq_fullflex_ten_step_global_y.bin` | `e368332d73ea7afc5eec6f398bbbbdd7d28c4b0ac16347b62e42f1c3d6047bfb` |
| `gold_high_ecc_shadow_altq_fullflex_ten_step_state.bin` | `bed1a8bc0523f167cd1a5d98d0ca0784a36eee78695f5864c2550c29b53f2e67` |
| `gold_high_ecc_shadow_altq_fullflex_ten_step_out.bin` | `dad5c9bb18ebbf898e6850f0f51dbf30881083c317986376cb774800c481c8ca` |
| `gold_high_ecc_shadow_altq_fullflex_ten_step_ipc_payload.bin` | `de42f332d469788faf0dd5fe41b127f52c75832020c5eb39457ee4f2e290d1b8` |

恢复端顶层比较当前仅在全局 `y[10]`／`y[12]`（以及对应 CoreDynamic／主帧字段）出现 1–2 ULP 差异；IPC payload 无差异。最小隔离确认 `GCI2ECEF` 在该位置已与原 ELF 逐位一致，而独立 `orbit_dynamic` 加速度 x 分量为 `0x3f8a7e04672bb1bb`，原 ELF 为 `0x3f8a7e04672bb1bd`。因此差异位于 ECEF 重力展开或 ECEF→GCI 之后的浮点数据流，而非前向坐标变换。该失败场景**尚未加入 Makefile 自检，不计入 PASS 总数**；后续将继续以原 ELF ECEF 加速度中间量或反汇编顺序收敛。

进一步隔离表明：反太阳位置的 `GCI2ECEF` 三分量与原 ELF 位级一致；将恢复端 ECEF 重力结果 `(-0x1.23e6ab7f07353p-4, -0x1.f72a56719c0f9p-11, -0x1.ef767157a8f75p-6)` 输入**原 ELF** `ECEF2GCI` 后，得到的 GCI 位模式为 `(3f8a7e04672bb1bb,bfb1f019465fb334,bf9f02303c5d0ff9)`，与恢复端最终结果一致而非原 ELF `orbit_dynamic` 结果 `(3f8a7e04672bb1bd,bfb1f019465fb335,bf9f02303c5d0ffb)`。因此当前证据把偏差收敛至 ECEF 中心项／J2/J3/J4 重力展开及其累加顺序，而不是前向或逆坐标变换。保留该隔离失败以指导后续反汇编审计；未修改已通过实现，也未影响 116 条既有严格门禁。

高轨隔离进一步确认：在反太阳位置，若将中心项分母由 `r * (x²+y²+z²)` 改为原 ELF `r * r * r` 的重算顺序，最小无 zonal 及完整 `orbit_dynamic` 金标均可逐位通过，且高偏心反太阳十步顶层金标也可通过。这一修改却使既有 DSS 随机场景从中途开始发生大范围状态、主帧和 IPC 差异；因此表明不同既有调用上下文中仍有未审计的半径／中间量数据流差异，不能以单一全局替换覆盖。该全局修改已撤回，高偏心反太阳金标、最小比较器和汇编证据作为**待闭环 P2**保留，未加入 Makefile；回滚后空构建严格自检恢复为 **116 条 PASS**。后续应以 `orbit_dynamic` 调用上下文、输入 vector backing 与原 ELF `vector_nrm2`／临时量复用为线索，而不是牺牲已验收随机轨迹。

## P3：`vector_add` 非致命 descriptor ABI 闭环（2026-08-21）

反汇编 `vector_add`（`0x9890`）确认：先比较右向量 `count` 与左向量 `count`，不一致立即返回 `-1` 且不解引用数据指针；相等的零计数直接返回 `0`；正计数则逐元素执行 `left[i] += right[i]`，返回 `0`。通过有效三元素 backing 的独立原 ELF 探针连续调用：`left.count=2/right.count=3`、两者均为零、两者均为三，返回序列为 `[-1,0,0]`，最终左 backing 为 `[5.0,3.0,-2.5]`。

| 金标 | SHA-256 |
|---|---|
| `gold_vector_add_nonfatal_p3_result.bin` | `3414421b183188342209566fad4e0c0723070b4030630dde685ba70281e92725` |
| `gold_vector_add_nonfatal_p3_left.bin` | `e57b613c0e96982a7cecdc97a55878e8d1ffd763c144d0dcfb3cfbd7318f0893` |

恢复端现有实现无需修改；比较器 `analysis/vector_add_nonfatal_p3_gold_compare.c` 的返回码和 backing 均为 **0 字节差异**，已纳入 Makefile。空构建目录严格自检更新为 **117 条 PASS**。

## P3：`vector_axpy` 非致命 descriptor ABI 闭环（2026-08-21）

反汇编 `vector_axpy`（`0x7e40`）确认：`alpha` 为精确零时立即返回 `-1`；NaN 不走零分支。对于非零 `alpha`，循环边界只读取 source descriptor 的 `count`，不读取或校验 destination 的 `count`；source 零计数直接返回 `0`，正计数执行 `destination[i] = source[i] * alpha + destination[i]`。以 source 三元素、destination 声明计数二但 backing 足够的探针分别覆盖精确零系数、source 零计数及 `alpha=-0.5` 的源计数驱动路径，原 ELF 返回序列为 `[-1,0,0]`，最终 destination 为 `[9.25,21.0,-30.125]`。

| 金标 | SHA-256 |
|---|---|
| `gold_vector_axpy_nonfatal_p3_result.bin` | `3414421b183188342209566fad4e0c0723070b4030630dde685ba70281e92725` |
| `gold_vector_axpy_nonfatal_p3_destination.bin` | `1a62ca906cf5bbf4f2ea64a7381fb6b69ecd66dcc73708d8186b58d148bd3d1e` |

恢复实现无需修改；比较器 `analysis/vector_axpy_nonfatal_p3_gold_compare.c` 的返回码和目标 backing 均为 **0 字节差异**，已纳入 Makefile。空构建目录严格自检更新为 **118 条 PASS**。

补充 `vector_axpy` 正计数空源边界：反汇编显示非零 `alpha` 且 source `count>0` 时立即读取 source backing；恢复端以独立子进程调用 `source={2,0,NULL}`、`alpha=1.0`，得到 **SIGSEGV**，而主进程保持存活。该死亡测试已纳入 Makefile；空构建目录严格自检更新为 **119 条 PASS**。该结论仅描述隔离进程终止 ABI，不将无效 descriptor 作为正常仿真输入。

## P3：`vector_axpby` 参数 ABI 修复与非致命 descriptor 闭环（2026-08-21）

原 ELF `vector_axpby`（`0x7ea0`）反汇编显示现有恢复签名的 source/destination 语义被反置：第一 descriptor 提供循环上界和 source 数据，第三 descriptor 提供 destination backing，实际写回为 `destination[i] = source[i] * alpha + destination[i] * beta`，且仅当 `alpha` 精确为零时返回 `-1`。原实现误将第一参数作为写回目标并错误校验两者 count；此 P3 探针以 source count=3、destination count=2 但 backing 有效，`alpha=2`、`beta=-0.5` 确认 destination 变为 `[-3,-14,21]`，source 保持 `[1,-2,3]`，返回 `[-1,0,0]`。

| 金标 | SHA-256 |
|---|---|
| `gold_vector_axpby_nonfatal_p3_result.bin` | `3414421b183188342209566fad4e0c0723070b4030630dde685ba70281e92725` |
| `gold_vector_axpby_nonfatal_p3_source.bin` | `b3ba8a10860ed8fcc6f73ddec0652ad0f157590a19e8f1c5319c7d53d1f9eef8` |
| `gold_vector_axpby_nonfatal_p3_destination.bin` | `88d6b872db2be40b373e7d9a080956bb58740b3025cfd25c305dbaf46c38dcca` |

恢复端已按该 ABI 修正 `vector_axpby`，并同步推进器和传感器内部调用参数；最小比较器的三个比较块均为 **0 字节差异**。空构建目录完整自检为 **120 条 PASS**，没有非零比较块。

随后以 `vector_axpby_null_source_p3_death_test` 在隔离子进程验证正 source count 且 `source.data == NULL` 的异常边界；恢复端按照原 ELF 的 source-first 读取顺序触发 **SIGSEGV**，父进程明确检验信号而非将崩溃当作测试成功。该门禁已写入 `Makefile`。再次从空 `build/` 目录运行 `make CC=gcc AR=ar selftest`，结果为 **121 条 PASS**，无非零字节比较或构建失败。此异常测试证明的是该一组 descriptor 前置条件下的终止语义，不应外推为所有非法指针或未对齐访问的完整等价性。

## P3：`blas_gemv` descriptor／零维度闭环（2026-08-21）

原 ELF `blas_gemv`（`0x8130`）先比较 `matrix.rows` 与 `y.count`；不相等立即返回 `-1`，不读取 matrix、x 或 y backing。若行数相等，`alpha` 精确为零时调用 `vector_scale(y, beta)` 后返回 `-1`；NaN 不走该分支。非零 `alpha` 时，只由 `matrix.rows`、`matrix.cols`、`matrix.row_stride` 控制循环，完全不读取或校验 `x.count`。行数为零时返回 `0`；列数为零时每行 dot 为正零，仍按 `y[i] = alpha * 0 + beta * y[i]` 写回。

| 金标 | SHA-256 |
|---|---|
| `gold_blas_gemv_descriptor_nonfatal_p3_result.bin` | `927acef6d5bc15586c99ec4f293dd691eaa5f243d47dc4d2f7a28a937c31b413` |
| `gold_blas_gemv_descriptor_nonfatal_p3_snapshots.bin` | `071de62d9d17f4953c17ab24b879405ac7105178cd52e1809ce8db9e5ca518cb` |

探针连续覆盖：(1) row/y count 不匹配；(2) `alpha=0` 的 y 缩放；(3) `x.count=1` 但 matrix 有两列的 source backing 读取；(4) `cols=0` 且 `x.data=NULL`；(5) `rows=0/y.count=0`。五次原 ELF 返回为 `[-1,-1,0,0,0]`，八个 y 快照均已逐字节保存。恢复端比较器两个块均为 **0 字节差异**，已写入 `Makefile`；空构建目录完整严格自检为 **122 条 PASS**，未出现构建失败或非零字节比较。该结论限于明确列出的结构和数值，未声称对任意异常指针、溢出维度或别名输入完成证明。

## P2：高偏心反太阳常规地影路径与中心重力半径顺序闭环（2026-08-21）

反太阳高轨输入 `[-12e6,65e6,28e6] m`、时间 `2031-12-31 23:59:50` 的原 ELF `orbit_dynamic` 金标曾在 x 分量出现 2 ULP 差异。GCI→ECEF 位置先逐字节确认一致；此路径中 `sum=x²+y²+z²=0x43324ea04e871001`、`r=0x41911d5f3dad6ff9`。原汇编 `0x9c88..0x9ca2` 以 `r*r` 后再乘原始 `r`，产生 `r³=0x44d39525cf67dd7e`；旧恢复端使用 `r*sum`，产生 `0x44d39525cf67dd7f`。由此中心缩放从 `0xbe128339bea3a2ad` 校正为原 ELF `0xbe128339bea3a2ae`。

原 ELF 动态打印还采集到完整 ECEF 加速度 `[(bfb23e6ab7f07354),(bf4f72a56719c0fa),(bf9ef767157a8f77)]`，最终 GCI 输出为 `[3f8a7e04672bb1bd,bfb1f019465fb335,bf9f02303c5d0ffb]`。恢复端按 `r*r*r` 顺序后均逐位一致。三个既有 `orbit_dynamic` 用例亦仍通过；四用例半径调查确认它们的 `r*sum` 与 `r*r*r` 恰好同位，只有该高偏心用例发生舍入分叉。

`analysis/time_orbit/orbit_dynamic_high_ecc_shadow_gold_compare.c` 已加入 `Makefile`。从空构建目录执行完整严格自检，结果为 **123 条 PASS**，无构建失败或非零比较块；相应的高偏心反太阳十步顶层场景在同一修复下也保持逐字节通过。该闭环仅陈述这一明确时间、位置、模型与无外力条件的证据，仍不构成对所有半径、历元或环境输入空间的完全证明。

## P2：DSS 双通道噪声均关闭十步端到端闭环（2026-08-21）

在既有默认、仅 DSS0 开启、仅 DSS1 开启及自定义噪声覆盖之外，新增两通道 `gaussian_noise_flag=0` 组合。原 ELF 以默认 `dyn_init` 初态、零设备命令、`srand(1)` 和十步连续 `dyn_main` 运行采集；仅将 DSS[0] `+0x18` 与 DSS[1] `+0x170` 改为零。两次独立采集的四类金标 SHA-256 完全一致：DSS 对象 `6c7f362daa34cff4bf186c0f3de1f5c14b6c555b22332a5c3c61c9cfafbd4708`，状态 `9b891cd19aad572ed9da6371a0b0ee6e3cb49ad2a38ed3953fcff6ab2d089758`，主帧 `85a2ff6ebe68355654b1febf9bdc19e76fa91346ada59b7d6052d035ec568f56`，IPC `d4617f28aa50d5d141ad976e05ba41268e0f0ea063150de11e47a6798ebe8a68`。

恢复端比较器在每一步分别比较 33 维 CoreDynamic 状态、完整 0x220 主帧和 3000-byte IPC payload，合计 **30 个比较块均为 0 字节差异**。`analysis/dyn_main_global_dss_both_off_ten_step_compare.c` 已纳入 `Makefile`；空构建目录严格自检现为 **124 条 PASS**，未出现构建失败或非零比较块。该证据只覆盖给定初态、种子、十步时域与两个精确标志值，不能推断其它传感器参数、随机序列或运行时长的完整等价。

## P2：GPS 初始化旗标状态机隔离闭环（2026-08-21）

原 ELF `UpdateGPS`（`0xcd60`）无条件覆写 `GPS_Kalman+0x28..+0x6f`：三个 position double、三个 velocity double、六个以 `cvttsd2si` 截断的时间 `int32`。随后只比较全局 `init_flag` 是否**精确等于** `1`；相等时写为 `0`，任何其它值直接保留。四次独立调用分别以入旗标 `[1,0,2,-1]` 与不同 position/velocity/time 输入验证，原 ELF 输出旗标为 `[0,0,2,-1]`。二次原 ELF 金标采集一致：完整 GPS 对象快照 SHA-256 为 `d3f87ecd7918d6e08f8d1310216e3faf32d19c4b1f4894462934061bd4e3af40`，旗标序列为 `fde47961dc7031f130d234123f2399dff7f70ad9fa019be31cdbb77c57ab5c68`。

比较器按原 ELF 实际写入范围比较四个 `+0x28..+0x6f` tail，共 288 字节，并比较 16 字节旗标序列；全为 **0 字节差异**。`+0x00..+0x27` 为该函数未写的全局保留区，故明确不将其进程初始内容纳入该函数调用的行为断言。门禁已加入 `Makefile`；空构建目录严格自检更新为 **125 条 PASS**，无失败或非零比较块。该证据覆盖函数级受控调用，不替代 GPS 在所有顶层时间调度、内存初始模式或极端转换输入下的完整证明。

## P3：`vector_dot` 非致命 descriptor 闭环（2026-08-21）

原 ELF `vector_dot`（`0x7d50`）先比较两个 `count`；不相等立即返回 `-1` 且不写 `out_dot`。相等且零计数时直接写 IEEE 正零并返回 `0`；正计数从零按升序执行 `result += left[i] * right[i]`，写回 `out_dot` 后返回 `0`。原 ELF 探针以 count `(2,3)`、`(0,0)`、`(3,3)` 连续调用，返回 `[-1,0,0]`，输出快照为 `[123.5,+0.0,3.0]`。重复金标 SHA-256：返回 `3414421b183188342209566fad4e0c0723070b4030630dde685ba70281e92725`，输出 `02913749d53c5386ca96fea00c66820b7ca4381de027c5344ef2dc51340cba4e`。

恢复端两个比较块均为 **0 字节差异**，并已纳入 `Makefile`。从空构建目录运行完整严格自检为 **126 条 PASS**，无构建失败或非零比较块。该结论仅覆盖有效 output pointer、指定 backing 和三种 count 情形，未对空输出指针或溢出计数作外推。

## P3：`vector3_cross` 非致命 descriptor 闭环（2026-08-21）

原 ELF `vector3_cross`（`0x7dc0`）仅比较 left/right 的 `count`；不相等立即返回 `-1`，不读写 backing。相等后不验证 count 是否为三，也不读取 output 的 count，而是固定读取两个输入的前三个 double 并写 output 前三个 double。以 left count=2、right count=3 的拒绝调用后，将 right count 改为 2 且 output count 保持 1，原 ELF 返回 `[-1,0]`，output 为 `[-27,-6,13]`。两份金标重复采集一致：返回 `72a4fa3544e43a836ffcb268ce06ccdbc55d44d5e6b1b1c19216a53ea98301fd`，output `8987bef81e55198f08d1514427d4533dcf025fc665226131de74d1841cac7ee7`。

恢复端的返回与 output 两个比较块均为 **0 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检达到 **127 条 PASS**，无构建失败或非零比较块。该结论仅适用于 backing 至少提供三分量的已测 descriptor，不能将其解释为安全长度校验或对越界输入的完整行为保证。

## P3：`vector_sub` 非致命 descriptor 闭环（2026-08-21）

原 ELF `vector_sub`（`0x98e0`）读取 right `count` 并与 left `count` 比较；不相等返回 `-1`，不写 left backing。相等零计数返回 `0`；正计数按升序执行 `left[i] -= right[i]` 并返回 `0`。连续 probe 覆盖 count `(2,3)`、`(0,0)`、`(3,3)`，返回 `[-1,0,0]`，最终 left backing `[4,7,-3]`。两次采集 SHA-256 一致：返回 `3414421b183188342209566fad4e0c0723070b4030630dde685ba70281e92725`，left `e7a7014f71248ae19a3d0b76b32956d956e5908351489ece0983d259f84a8ce2`。

恢复端返回和 left backing 均为 **0 字节差异**；比较器已纳入 `Makefile`。空构建目录完整严格自检更新为 **128 条 PASS**，无构建失败或非零比较块。该结论仅覆盖具有有效 backing 的固定 count/数值组合，不外推为任意无效 descriptor 的等价性。

## P3：`vector_scale` 非致命 descriptor 闭环（2026-08-21）

原 ELF `vector_scale`（`0x9930`）只读取 vector `count`；零计数不读 backing、返回 `0`，正计数逐元素按升序乘标量并返回 `0`。原 ELF probe 先以 count=0 调用，再以 count=3、scalar=-3 调用，返回 `[0,0]`，data 从 `[1.5,-2,+0]` 变为 `[-4.5,6,-0]`，明确保留乘法生成的负零符号位。重复采集 SHA-256：返回 `af5570f5a1810b7af78caf4bc70a660f0df51e42baf91d4de5b2328de0e83dfc`，data `849bd863ae40beb30863b3ef7d8c831dec065187eacdd6a9bd664c285ff8902c`。

恢复端返回和 data 比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **129 条 PASS**，无失败或非零比较块。该结论仅覆盖有效三元素 backing、零计数及给定有限标量，不外推为所有 NaN/无穷或非法指针情形。

## P3：`vector_nrm2` 非致命 descriptor 闭环（2026-08-21）

原 ELF `vector_nrm2`（`0x7f00`）只读取 vector `count`；零计数直接返回 IEEE 正零，正计数按升序累加平方后开方。原 ELF probe 覆盖 count=0 和三元素 `[3,-4,12]`，输出分别为 `+0.0` 与 `13.0`；重复金标 SHA-256 为 `e86c53bb89aa483106711f6d2578ee744fffbf5fa35c07681c0ac20ba612dc0b`。恢复端 16 字节结果块为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **130 条 PASS**，无失败或非零比较块。该测试未覆盖正 count 空 backing、NaN／无穷或溢出平方和边界。

## P3：`matrix_trans` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_trans`（`0x89f0`）不比较 src/dst 形状；外层循环由 `src.rows` 驱动，内层由 `src.cols` 驱动，读取 src 的 `row_stride`、写入 dst 的 `row_stride`，恒返回 `0`。原 ELF probe 使用 src `{rows=2,cols=2,stride=3}` 与 dst `{rows=1,cols=3,stride=4}`，验证 dst 声明形状不阻止转置写回；12-double backing 结果为 `[1,3,-1,-1,2,4,-1,-1,-1,-1,-1,-1]`。两次采集 SHA-256 一致：返回 `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119`，destination `4e6a37348d715d6d51ef7a1afe5aefd678b2147fff7861d7b5f7ae0f69356331`。

恢复端 return 和 destination 比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **131 条 PASS**，无失败或非零比较块。该结论只覆盖有效 backing 与明确的 stride/维度组合，不外推为任意别名、负维度或越界内存语义。

## P3：`matrix_set_zero` 返回 ABI 修复与 descriptor 闭环（2026-08-21）

原 ELF `matrix_set_zero`（`0x88d0`）按 rows、cols、row_stride 清零有效矩阵元素并保留行间填充；更关键的是函数尾部**不**将 `EAX` 清零。正 rows/cols 时返回最后一次内层循环的退出线性索引 `(rows-1)*row_stride+cols`。原 ELF probe `{rows=2,cols=2,stride=4}` 返回 `6`，并将 12-double backing 的索引 `[0,1,4,5]` 清为正零，填充 `[2,3,6..11]` 保持。旧恢复端恒返回 `0`，这是本轮识别并修正的可观察 ABI 偏差。

金标重复采集 SHA-256：返回 `7aa8ca4a02506da9133d8f889678b76f716ce45d02e22fdb7b70a15e56a0eff8`，data `b99c06761f5c70f192074cab86a11c66b741f5e272b8c15f27d6283e2a95b3e1`。修复后 return 和 data 均为 **0 字节差异**，并已写入 `Makefile`。从空构建目录运行完整严格自检为 **132 条 PASS**，无失败或非零比较块。零 rows/cols 下原 ELF 未定义的残留返回寄存器内容不在本次可移植 C11 断言范围内。

## P2：高偏心反太阳常规地影相邻日期十步闭环（2026-08-21）

在已通过的 `2031-12-31 23:59:50` 高偏心反太阳位置基础上，仅将历元向前移动一天至 **`2031-12-30 23:59:50`**，保留位置 `[-12e6,65e6,28e6] m`、正交四元数、全二十维非零柔性状态、零命令与 `srand(12345)`。该变化形成常规 DSS 地影路径的第二独立太阳向量／历元输入。原 ELF 全局 y、状态、主帧与 IPC 金标的二次采集 SHA-256 分别为 `4575ffe94228594ee82670204ef0fa005868585faab98a987f44f2125187a613`、`096e46571fdf5952739e81a4ec52e0ddb8096cd5cc1c0774b93a000f112623dd`、`64f7758afcb4ee5a7424e3475a994346b66e240626bb601d607b51646f6b692c`、`2adead902eaff5b3309138aed0d7bc023dab9c0da438737fadb10869172a8440`。

恢复端每步完整全局 y、CoreDynamic 前 0x108、完整 0x220 主帧和 3000-byte IPC payload 共 **40 个比较块均为 0 字节差异**。比较器已纳入 `Makefile`；空构建目录完整严格自检现为 **133 条 PASS**，无失败或非零比较块。该证据只覆盖相邻的一天与该固定反太阳位置，尚不构成连续日食边界或任意日地几何输入的完整证明。

## P2：高偏心反太阳相邻日期二十步时域闭环（2026-08-21）

将已通过的 `2031-12-30 23:59:50` 高偏心反太阳常规 DSS 地影单元由十步延长至二十步，保持同一位置、正交姿态、全二十维柔性状态、零设备命令和 `srand(12345)`。原 ELF 二次采集的 20-step 全局 y、状态、主帧及 IPC 金标 SHA-256 分别为 `4a9116345734082a9aadeb20157f1598ade477be620e7bdf40534c4eaaddb578`、`903133090b646a735455859bd04994d5d0b72c6a18d9d87872d582996bfb1219`、`d14ac0f4da14dc989b14b050e7dbab106fa25d8b27981c9a261c31ad73b2f6b0`、`ce64084cfd5af4e85badb812d8f8ad3552289099189bc1e59da0c92a2478eaae`。

恢复端每步比较全局 y、CoreDynamic 前 0x108、完整 0x220 主帧和 3000-byte IPC payload，共 **80 个比较块均为 0 字节差异**。该门禁已加入 `Makefile`；空构建目录完整严格自检更新为 **134 条 PASS**，无失败或非零比较块。该结果延长了已选日期／几何单元的时间证据，但不能替代连续日食交界、其它轨道位置或其它随机序列的完整覆盖。

## P3：`blas_gemm` alpha 零优先拒绝 ABI 修复（2026-08-21）

原 ELF `blas_gemm`（`0x8210`）在读取或比较矩阵 shape 前，先以 `ucomisd` 测试 alpha；alpha **精确为零**时立即返回 `-1`，不缩放且不写 output。恢复端此前错误地通过计算路径把 output 乘以 beta，因而本轮完成修复。随后原 ELF 按 `left.rows==out.rows`、`right.cols==out.cols`、`left.cols==right.rows` 的顺序检查 shape；成功路径按 `alpha * left[i,k] * right[k,j]` 累加后再加 `beta*out[i,j]`。

原 ELF 探针连续覆盖有效形状 alpha=0、有效形状 alpha=1/beta=2、以及 `out.rows` 不匹配，返回 `[-1,0,-1]`，output 首元素快照 `[7,37,37]`。重复金标 SHA-256：返回 `9d4b936a67c21ab14da09aa480239b43ea122c9c933d876013ee3012e464c16b`，快照 `8a4ba4bb5b1ca6d2aec2bcb59b45a160c801303492d15ecbc7928bbbab2414f8`，output `e743068562378d242052a716475aa7c731bf801e64e1725a3595521005149513`。

修复后返回、快照和 output 三个比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检现为 **135 条 PASS**，无失败或非零比较块。该闭环覆盖精确零 alpha、一个有效乘法点和一种 output shape 不匹配；尚未外推 NaN alpha、零维度、别名或无效 backing 行为。

## P3：`matrix_add_constant` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_add_constant`（`0x8870`）以 rows、cols 与 `row_stride` 遍历，只修改每行有效列并保留 padding，尾部显式返回 `0`。原 ELF probe `{rows=2,cols=2,stride=4}`、12-double backing、常数 `-0.5` 验证有效索引 `[0,1,4,5]` 变为 `[0.5,1.5,4.5,5.5]`，其余 backing 不变。重复金标 SHA-256：返回 `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119`，data `c835fb70fc7158c8114c0370aa84ed28e33364a4e5717a7a847e553c435197fd`。

恢复端 return 与完整 96-byte backing 两个比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。从空构建目录的完整严格自检为 **136 条 PASS**，无失败或非零比较块。该证据覆盖一种正维度、带填充 descriptor 与有限常数，未对零维度、NaN 常数、别名或无效 backing 外推。

## P3：`matrix_scale` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_scale`（`0x8810`）按 rows、cols 和 `row_stride` 缩放有效元素，保留行间 padding，并显式返回 `0`。原 ELF probe `{rows=2,cols=2,stride=4}` 使用 scalar `-2`；有效元素缩放，初始 `+0.0` 变为 IEEE `-0.0`，而 padding 完全保持。重复金标 SHA-256：返回 `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119`，data `735baab23aa67eb1147643cbd9755d62fb2e3d5ffb99b60eb2b2a9f9f8f375d5`。

恢复端 return 与完整 96-byte backing 比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **137 条 PASS**，无失败或非零比较块。该场景只覆盖一个正维度、有限标量及有效 backing，不外推零维度、NaN/无穷和内存别名路径。

## P3：`matrix_add` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_add`（`0x8590`）先比较 rows，再比较 cols；任一不等返回 `-1` 且不写 left backing。匹配时，left/right 分别使用自身 `row_stride` 做行地址推进，逐元素写回 left，最终返回 `0`。原 ELF probe 先用 row mismatch `(2,3)`，再改 right.rows=2 并保留 left stride=3/right stride=4，返回 `[-1,0]`；left backing 变为 `[11,22,90,53,64,91,0,0]`。重复金标 SHA-256：返回 `72a4fa3544e43a836ffcb268ce06ccdbc55d44d5e6b1b1c19216a53ea98301fd`，left `8f7e44e1d71fc02233e89ff9776dbfd659cdf9001bb9e681561e0048c6e7e5fc`。

恢复端 return 与 64-byte left backing 两个比较块均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **138 条 PASS**，无失败或非零比较块。该证据涵盖 rows 优先拒绝与一个带不同 stride 的有效矩阵组合，尚未对 cols 不匹配、零维度、别名或无效 backing 作外推。

## P3：`matrix_sub` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_sub`（`0x8630`）与 `matrix_add` 一致，先比较 rows、再比较 cols；不匹配返回 `-1` 且不写 left，匹配时分别采用 left/right 的 `row_stride` 就地减法，返回 `0`。原 ELF probe 先用 row mismatch `(2,3)`，再令 right.rows=2，使用 left stride=3/right stride=4；返回 `[-1,0]`，left backing 变为 `[1,2,90,3,4,91,0,0]`。重复金标 SHA-256：返回 `72a4fa3544e43a836ffcb268ce06ccdbc55d44d5e6b1b1c19216a53ea98301fd`，left `45b43c508f9ae753edb9c3667ca1a0602d738b616a5fa2bd7d9fbd1f56b4bb38`。

恢复端返回及 64-byte left backing 均为 **0 字节差异**，并已纳入 `Makefile`。首次空构建执行中，一个既有飞轮两步比较器出现地址形态的非持久性失配；随后在相同新构建中单独复现该比较器通过、完整非清理回归通过，最后重新执行空构建严格回归亦通过。最终空构建严格自检为 **139 条 PASS**，包括 `matrix_sub` 与该飞轮门禁，且无失败或非零比较块。该证据不外推 cols mismatch、零维度、别名或无效 backing。

## P3：`matrix_mul_elements` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_mul_elements`（`0x86d0`）先比较 rows、再比较 cols；不匹配返回 `-1` 且不写 left，匹配时分别用 left/right 的 `row_stride` 逐元素相乘并返回 `0`。原 ELF probe 先用 row mismatch `(2,3)`，再令 right.rows=2，以 left stride=3/right stride=4 完成有效写回；返回 `[-1,0]`，left backing 为 `[3,6,90,-15,-240,91,0,0]`。重复金标 SHA-256：返回 `72a4fa3544e43a836ffcb268ce06ccdbc55d44d5e6b1b1c19216a53ea98301fd`，left `8f18a268c9cd53b9b3876002958560ea402f4ffdae0a0dfb6f4aa8538b6994b6`。

恢复端 return 与 64-byte left backing 均为 **0 字节差异**，比较器已纳入 `Makefile`。从空构建目录的完整严格自检为 **140 条 PASS**，无失败或非零比较块。该证据覆盖 rows 优先拒绝与一个不同 stride 的有效组合，尚未外推 cols mismatch、零维度、NaN 乘积或无效 backing。

## P3：`matrix_div_elements` 非致命 descriptor 闭环（2026-08-21）

原 ELF `matrix_div_elements`（`0x8770`）先比较 rows、再比较 cols；不匹配返回 `-1` 且不写 left。匹配时使用 left/right 各自的 `row_stride` 做 IEEE `divsd`，保留零除结果，最终返回 `0`。原 ELF probe 先用 row mismatch `(2,3)`，再令 right.rows=2，以 left stride=3/right stride=4 执行；返回 `[-1,0]`，left 有效分量为 `[1.5,2,1,+inf]`，padding 保持。重复金标 SHA-256：返回 `72a4fa3544e43a836ffcb268ce06ccdbc55d44d5e6b1b1c19216a53ea98301fd`，left `3be6f244cf1ed072e20d5359e94e1577911b18e55730cb7c4910ca95d5f13d3b`。

恢复端 return 与 64-byte left backing 均为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **141 条 PASS**，无失败或非零比较块。该证据覆盖 rows 优先拒绝、一个正零除法点和不同 stride；尚未外推负零、NaN、cols mismatch、零维度或无效 backing。

## P3：`vector_unit` 双 descriptor ABI 修复（2026-08-21）

原 ELF `vector_unit`（`0x7fd0`）并非单参数原地归一化：ABI 为 `(destination, source)`。它先计算 source 范数；仅当范数严格大于零时，按 destination 自身 count 调用 `vector_scale(destination,0)`，再次计算 source 范数，再按 **source count** 执行 `destination[i] += source[i] / norm`。零、负或 NaN 范数均直接返回而不写 destination；函数不设置规范返回值。旧恢复端错误实现为单参数原地归一化，已修正为双 descriptor ABI。

原 ELF probe 使用 destination count=1、source count=3，先测试全零 source（destination 保持），再测试 `[3,4,12]`；最终 destination 为 `[3/13, 8+4/13, 7+12/13]`，证实 destination count 仅影响初始清零长度而不限制后续 source-count 写入。重复金标 SHA-256：返回寄存器快照 `565a84351e3243cea8d3902b7e165cc9e9d4511c31471710fc9f357252a1d36e`，destination `6157ca94e5453329753105b75661c31900b0cddb5abd60725ed2d643e20ae08a`。

修复后 24-byte destination 比较块为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检更新为 **142 条 PASS**，无失败或非零比较块。该闭环不对空 descriptor、负 count 或 NaN source 分量作外推。

## P3：新增缺失导出 `vector_set_zero`（2026-08-21）

原 ELF 导出 `vector_set_zero`（`0x97a0`）此前尚未出现在恢复端。反汇编显示其为 void ABI：零 count 不访问 backing，正 count 将每个元素写为 IEEE 正零。已新增 C11 实现和头文件声明。原 ELF probe 以 data `[1.5,-2,+0]` 先调用 count=0、再调用 count=3；快照依次为 `[1.5,-2,+0]` 与 `[+0,+0,+0]`。二次采集 SHA-256 一致：`8b66f00953fe9507a9ab6de916c5b937348eee5b69ffe79b778904a66620f723`。

恢复端 48-byte 快照比较块为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **143 条 PASS**，无失败或非零比较块。该新增导出闭环仅覆盖零和三元素有效 backing，不外推负 count 或空 backing。

## P3：新增缺失导出 `vector_square`（2026-08-21）

原 ELF 导出 `vector_square`（`0x7f80`）此前未出现在恢复端。反汇编确认其按 count 升序累加平方并以 `xmm0` 返回 double，不执行开方；零 count 返回 IEEE 正零。已补充 C11 实现及头文件声明。原 ELF probe 覆盖 count=0 和 `[3,-4,12]`，返回 `[+0.0,169.0]`。二次采集 SHA-256 一致：`1f62b6fabf12b0faaf5e5b460fb7f914bacb8d1db948e60c8f0d113cfa2b6ab0`。

恢复端 16-byte 返回块为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **144 条 PASS**，无失败或非零比较块。该新增导出闭环仅覆盖零及三元素有限输入，不外推 NaN、无穷、溢出或空 backing。

## P3：新增缺失导出 `vector_abs_max`（2026-08-21）

原 ELF 导出 `vector_abs_max`（`0x99b0`）此前未出现在恢复端。反汇编确认其以绝对值掩码后通过 `maxsd` 累计最大幅值，零 count 返回 IEEE 正零；NaN 不会替换已有最大值。已新增 C11 实现及头文件声明。原 ELF probe 覆盖 count=0 与 `[-2,3.5,-1]`，返回 `[+0.0,3.5]`。二次采集 SHA-256 一致：`5315a9db6ec52325507d29b95b81cb2a5e9a60457dfd7644a1994d6b4690fed2`。

恢复端 16-byte 返回块为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **145 条 PASS**，无失败或非零比较块。该新增导出闭环未对 NaN、无穷、负零或空 backing 作外推。

## P3：新增缺失导出 `matrix_set_all`（2026-08-21）

原 ELF 导出 `matrix_set_all`（`0x8920`）此前未出现在恢复端。反汇编确认其为 void ABI，按 rows、cols 与 `row_stride` 写入有效元素，保留 row padding，零 rows 时不写 backing。已补充 C11 实现及头文件声明。原 ELF probe 使用 cols=2/stride=4/backing 12 doubles，先运行 rows=0 再运行 rows=2、value=-1.25；首次完整 backing 保持，第二次仅索引 `[0,1,4,5]` 变为 `-1.25`。二次采集 SHA-256 一致：`c09accaae8008837ca18b0e836716a43d88c9c21f4d0cb777588c6ba135d8c64`。

恢复端 192-byte 两阶段快照比较块为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **146 条 PASS**，无失败或非零比较块。该新增导出闭环未对零 cols、负维度、NaN value 或无效 backing 作外推。

## P3：新增缺失导出 `matrix_set_col`（2026-08-21）

原 ELF 导出 `matrix_set_col`（`0x8b10`）此前未出现在恢复端。ABI 为 `(matrix, column, source)`；它先以无符号比较拒绝 `column >= matrix.cols`，再验证 `source.count == matrix.rows`。成功时按 **matrix.cols**（而非 `row_stride`）推进目标地址，写入指定列，返回 `0`；任一拒绝返回 `-1` 且不写。已补充 C11 实现及头文件声明。

原 ELF probe 使用 matrix `{rows=2,cols=2,row_stride=4}`：column=2 时即使 source count=3 也优先返回 `-1`；column=1/source count=3 返回 `-1`；source count=2 时返回 `0` 并将 backing 的索引 1、3 改为 `[10,20]`，确证按 cols 而非 stride 步进。重复金标 SHA-256：返回 `65ea24b012287d121bdf43f4f468ed09ddbf9f5a7d20521071761412580c2e91`，data `bb919a94576f96bae9bf697ace9d5988b6418d62d3727d7ad5ae49bbda2662c2`。

恢复端 return 与 96-byte backing 均为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **147 条 PASS**，无失败或非零比较块。该新增导出闭环未对负 rows、负 column、零 cols 或无效 backing 作外推。

## P3：新增缺失导出 `vector_add_constant`（2026-08-21）

原 ELF 导出 `vector_add_constant`（`0x9970`）此前未出现在恢复端。反汇编确认其按 count 对 vector backing 原地执行 `addsd`，零 count 不写且显式返回 `0`。已新增 C11 实现及头文件声明。原 ELF probe 以 `[1.5,-2,+0]` 和 value=-1.5 先运行 count=0、再运行 count=3：两次返回均为 0；第一快照保持，第二快照为 `[+0,-3.5,-1.5]`。二次采集 SHA-256 一致：返回 `af5570f5a1810b7af78caf4bc70a660f0df51e42baf91d4de5b2328de0e83dfc`，快照 `de021a9e2278c5ed8274256e7cb768554b55596187904853ded0cff398f7804e`。

恢复端返回与 48-byte 两阶段快照均为 **0 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **148 条 PASS**，无失败或非零比较块。该新增导出闭环未对 NaN、无穷、负 count 或无效 backing 作外推。
## P3：新增缺失导出 `matrix_print`（2026-08-21）
原 ELF 导出 `matrix_print`（`0x8a60`）此前未出现在恢复端。反汇编确认其 ABI 为 `void matrix_print(const DpMatrix *)`；先输出 `"\nSize: %d * %d\n"`，随后逐行输出 `"%10.5f "`，每行末输出换行，结束后额外输出一行换行。行内元素地址按 **`row * matrix.cols + col`** 计算，并不使用 descriptor 的 `row_stride`。已补充 C11 实现、头文件声明和 stdout 比较器。

原 ELF stdout probe 使用 `{rows=2, cols=2, row_stride=4}` 与 backing `[1,2,3,4,5,6,7,8]`，输出的两行数值为 `[1,2]` 和 `[3,4]`，确证采用紧密 cols 索引。由于 GDB 的 inferior tty 在批处理环境产生一行外部终端警告，金标仅剥离该警告行，保留函数产生的完整 60-byte stdout 序列；纯函数输出金标 SHA-256 为 `32621ae92ee448e2871cbf44d24aaf2ec82e7c89a5d278bfd73c74e57dd640f8`。

恢复端 stdout 比较器得到 **0 字节差异**（actual=60、expected=60），新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **149 条 PASS**，无失败或非零比较块。该新增导出闭环仅覆盖正维度、有限值和 `cols != row_stride` 的格式/索引语义；不外推零维、负维、NaN/无穷或无效 backing。
## P3：新增缺失导出 `vector_print`（2026-08-21）
原 ELF 导出 `vector_print`（`0x97e0`）此前未出现在恢复端。反汇编确认其 ABI 为 `int vector_print(const DpVector *)`：读取 descriptor 的 `count`（+0x00）和 data 指针（+0x08），先输出 `"\nSize: %d\n"`，随后按 **无符号 count** 对每个元素输出 `"%10.5f\n"`，最后额外输出换行，且所有已测试路径均在 `eax` 返回 `1`。已补充 C11 实现、头文件声明和 stdout/返回 ABI 比较器。

原 ELF probe 先以 count=0，再以相同 backing `[1.0,-2.5,3.25]` 的 count=3 调用。stdout 金标覆盖标题、零计数的空行、三行元素格式及尾随空行；两次返回值均为 `1`。因 GDB inferior tty 的外部终端警告，金标仅剥离该警告行，保留函数产生的完整 53-byte stdout 序列。纯 stdout SHA-256 为 `bbac6cb68a21d1910e5377514754825858a7bb58f96a9cc88656b558c4a847c9`；8-byte 返回块 SHA-256 为 `64ed86b909d6d0502b64b28db0ea1272ffb358e20e9b1d88b63ccb07fa900cf5`。

恢复端 stdout 与返回 ABI 比较器得到 **0 字节差异**（stdout actual=53、expected=53），新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **150 条 PASS**，无失败或非零比较块。该新增导出闭环仅覆盖零与正 count、有限值及有效 backing；不外推负 count（原 ELF 无符号循环）、NaN/无穷、空 descriptor 或无效 backing。
## P2：`UpdateDeviceMeasure` 顶层 GPS `init_flag` 状态机（2026-08-21）
此前 `dp_update_gps` 的函数级金标已覆盖 `init_flag` 的精确一值清零与非一值保持；本单元将其提升至原 ELF 顶层 `UpdateDeviceMeasure` 调度路径。原 ELF probe 在 `Gyro_Init`、`MagMeter_Init`、`STS_Init`、`DSS_Init` 后显式写入全局 `init_flag=1`，以受控 CoreDynamic 输出连续调用两次 `UpdateDeviceMeasure`。第一步使用位置 `[7000000,1000,-500]`、速度 `[1.5,-2.25,3.75]`、时间 `[2025.9,1.1,2.2,3.3,4.4,5.5]`；第二步改为 `[7100000.25,-2000.5,3000.75]`、`[-4.5,5.25,-6.75]` 和 `[2031.9,12.8,31.7,23.6,59.5,58.4]`。

两次独立原 ELF 采集对三份金标均逐字节一致：step1 GPS 全对象 SHA-256 `95e0b8bbc30656d3c058e2c65e4adb79a0ba0d6efdf58ae65255b4f6ebacd394`，step2 为 `52ae3afb577898a7d9e8b0e755383df23cec8a8488c9d908388445df94756465`，两步全局标志 `[0,0]` 为 `af5570f5a1810b7af78caf4bc70a660f0df51e42baf91d4de5b2328de0e83dfc`。反汇编和既有对象布局审计确认 `UpdateGPS` 仅写 `GPS_Kalman +0x28..+0x6f`；原对象 `+0x00..+0x27` 为初始化元数据，恢复端尚未将该保留区作为同一全局对象布局复现，故顶层比较器严格比较每步 **72-byte 已写尾区**及 4-byte `gps_init_flag`，而不将未写保留字节误归因于本调用的行为差异。

恢复端两步复放中，step1/step2 GPS 已写尾区均为 **0/72 差异**，两个标志快照均为 **0/4 差异**，并已纳入 `Makefile`。空构建目录完整严格自检为 **151 条 PASS**，无失败或非零比较块。该单元证明给定已初始化传感器对象和两组有限 CoreDynamic 输出下，`init_flag=1→0` 后持续为 0 的顶层调度路径；不外推 GPS 保留元数据、其他初始标志值、空 CoreDynamic 指针或完整随机传感器对象全空间。
## P2：`UpdateMainOut` STS 噪声开关组合（2026-08-21）
既有 seeded-STS 门禁覆盖默认三个通道均开启噪声的路径；本单元补充两个互补的顶层遥测组合。原 ELF probe 先用 `DynamicInit` 完成卫星全局对象初始化，并初始化 Gyro、MagMeter、DSS、STS（`UpdateMainOut` 会读取这些对象的 descriptor）；然后将三个 `STS.gaussian_noise_flag` 分别置为 `[0,0,0]` 与 `[0,1,0]`，每个场景均在调用前固定 `srand(1)`，使用同一零填充 CoreDynamic 输入调用 `UpdateMainOut` 并保存完整 `0x220` 帧。

两个场景各自重复采集均逐字节一致。全关闭帧 SHA-256 为 `607c3915693d4054e7eac13eb4e9a98d891898ad91aa757fdfc358f72a8d2df0`，仅 STS[1] 启用帧为 `57e1d5cb8c4aa72ad9b2da3f0248e2b44a94a4beaa97c4d2fb397a4e58f5e915`，确认单通道开关会改变顶层帧。比较器对每个场景严格比较 `UpdateMainOut` 的 STS 四元数遥测区 `+0x000..+0x05f`（三个连续 32-byte 区域）；全关闭和仅 STS[1] 开启均为 **0/96 字节差异**。

该比较器已纳入 `Makefile`，空构建目录完整严格自检为 **152 条 PASS**，无失败或非零比较块。本单元覆盖固定种子、已初始化对象、零 CoreDynamic 输入下的三通道全关闭与中间通道单独开启；不外推其他通道的单独开启、多个连续测量周期、STS 有效标志几何边界或经 `dyn_main` 的完整主帧/IPC 长时域路径。
## P0/P2：高偏心反太阳相邻日期 50 步长时域（2026-08-21）
在已通过的 10 步和 20 步高偏心反太阳相邻日期场景基础上，本单元保持完全相同的受控初态和命令：`2031-12-30 23:59:50`、`a=26,560,000 m`、`e=0.65`、反太阳高轨位置 `[-12e6,65e6,28e6] m`、规范化四元数 `[0.5,-0.5,0.5,0.5]`、全二十维非零柔性初态、零设备命令和 `srand(12345)`，将原 ELF `dyn_main` 连续复放扩展至 50 步。每步分别保留 CoreDynamic 回写的 `0x108`／33-double 状态、完整 `0x220` 主遥测帧、3000-byte IPC payload 以及全局 `y[33]`。

原 ELF 金标独立采集两次，四个聚合文件均逐字节一致：global `y`（13,200 bytes）SHA-256 `69db925af7d812665c9d8c16e91d954ee26c697cac84e34201825855dd027668`；状态（13,200 bytes）`fa7f2386c1080fbc603b7570e0356d3efb18d745b154c6325d630bbac2da2b81`；主帧（27,200 bytes）`0d08ec9a8390aea645de8fe4e3b99abfeb85537b50883331a426d1002d3c90c0`；IPC（150,000 bytes）`de757375e226415fbfce9da073cf9a95161f7a55f6b827bf89d24483335daa8f`。

恢复端在 50 个时间点的四类比较均为 0 字节差异，共 **200 个逐步比较块**；第 50 步状态 `0/264`、主帧 `0/544`、IPC `0/3000`、全局 `y` `0/264`。新比较器已纳入 `Makefile`，空构建目录完整严格自检为 **153 条 PASS**，无失败或非零比较块。该长时域证据仅适用于指定日期、轨道、姿态、柔性初态、零命令和固定随机种子；不将有限 50 步轨迹外推为其他环境、命令、时间长度或异常输入空间的等价性保证。
## P2：DSS 地影日食边界隔离扫描（2026-08-21）
为补足高偏心反太阳场景中常规地影分支的边界证据，本单元直接审计原 ELF `isEarthShadow`／`UpdateSunSensorValidFlag`。两个 DSS 的 `valid_angle_threshold` 均暂设为 `100.0`，使阈值比较必定落入地影几何分支；DSS measure descriptor 维持 3 元有效 backing。太阳方向固定为 `[1,0,0]`，位置模长固定为 70,000,000 m，并围绕反太阳方向采样偏转角 `0.080`、`0.090`、`0.093`、`0.100` rad，对应精确位置由离线 `sin/cos` 计算后写入探针。

原 ELF 在前两个角点返回地影真值并将双通道标志写为 `[1,1]`，在后两个角点返回非地影并写为 `[0,0]`；直接 `isEarthShadow` 4×int 金标 SHA-256 为 `b35c194be488b20ad2a256a46fbf301037f6625ef185287c9c7ff5191ca3904`，双通道 8×int 标志金标为 `b0da5b1bd2e6f3d76a2e9cc5e7ac9abba91737dd877305d6e06e252d024e40ef`。位置、角度、直接返回值和标志金标均经第二次独立采集逐字节一致。该翻转与 70,000 km 处理论角宽相符，但验收仅以原 ELF 实测为准。

恢复端以同一四个位置复放，直接地影结果比较为 **0/16 字节差异**，双 DSS 有效标志比较为 **0/32 字节差异**，新比较器已纳入 `Makefile`。空构建目录完整严格自检为 **154 条 PASS**，无失败或非零比较块。本单元覆盖隔离有效标志调用、强制地影分支、指定太阳方向和指定距离；不外推 `UpdateDSS` 产生的 legacy NaN 快捷路径、太阳方向随历时演化、DSS 安装矩阵变化或完整 `dyn_main`／IPC 日食穿越轨迹。
## P3：新增未映射导出 `quat_cpy`（2026-08-21）
原 ELF 导出 `quat_cpy`（`0x2e00`）此前未映射到恢复端。反汇编确认其 ABI 为 `void quat_cpy(double *destination, const double *source)`，并按前向顺序执行四次 `movsd` 载入/写回：仅访问连续的 `+0x00/+0x08/+0x10/+0x18`，无 descriptor、形状检查或返回值。该 ABI 与 `DpQuatAbi` 的标量＋嵌入向量 descriptor 布局不同，故在头文件中明确声明为裸 `4×double` 数组函数。

原 ELF probe 使用 `[1.25, -0.0, 3.0, -4.5]`，其中第二元素通过直接写入 IEEE `0x8000000000000000` 构造负零，避免 GDB 文本赋值丢失符号位。输出 32-byte 金标经两次采集一致，SHA-256 为 `a7ebf6f92aef9c474c76a90c03f87942ffa1eeb6bc56ecd05d2c2dfd8d29d41b`；第二元素的负零位模式完整保留。

恢复端四元素顺序写回的比较器得到 **0 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检为 **155 条 PASS**，无失败或非零比较块。该闭环覆盖非重叠的有效连续四元素 backing 和负零；不外推部分重叠指针、空指针、非 4 元 backing 或未定义内存区域。
## P3：新增未映射导出 `quat_print`（2026-08-21）
原 ELF 导出 `quat_print`（`0x9170`）此前未映射到恢复端。反汇编确认其参数采用 `DpQuatAbi` 布局：scalar 从 `rdi+0x00` 读取，xyz backing 指针从 `rdi+0x10` 读取；依次输出 `"\nqs: %f\n"`、`"qv.x: %f\n"`、`"qv.y: %f\n"`、`"qv.z: %f\n\n"`。前三次 `printf` 的返回值被忽略，函数通过尾跳转最后一次 `printf`，故返回该最后格式串的字符数。

原 ELF stdout probe 使用 scalar `1.25` 与 xyz `[-2.5,3.25,-4.5]`。为剥离 GDB inferior tty 的外部警告，金标仅保留函数产生的 62-byte stdout；重复采集 stdout SHA-256 为 `05c04ca8695bdf251651896a94a91a51d978d6bad3650b9462d7aedc4a589c13`，4-byte 返回金标（值 17）SHA-256 为 `84fc05949dc1e486652a4ed316afb6434e9437eb30b714594a1d0b4205776602`。

恢复端 stdout 比较为 **0 字节差异**（actual=62、expected=62），返回值 17 一致，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **156 条 PASS**，无失败或非零比较块。该闭环覆盖有效 xyz descriptor、有限值和所测 stdout 格式；不外推空 pointer、非 3 元 xyz descriptor、浮点特殊值、格式化错误或 I/O 失败行为。
## P3：新增未映射导出 `array_copy`／`array_sum`（2026-08-21）
原 ELF 的 `array_copy`（`0x66b0`）和 `array_sum`（`0x66d0`）均为未映射的固定状态长度辅助导出。反汇编确认二者没有 count 参数、固定循环 33 次（字节偏移 `0x00..0x100`）：`array_copy(destination, source)` 以四字节？不，按四次 8-byte `movsd` 前向复制 33 个 double；`array_sum(source, destination, alpha)` 从 XMM0 接收 alpha，并按 `destination[i] = source[i] * alpha + destination[i]` 的顺序前向写回 33 项。两者均为 void。

原 ELF 联合 probe 使用 `source[i]=i+0.25`、`destination[i]=100-i` 和 `alpha=0.5`，并直接将 source[1] 设为 IEEE 负零以覆盖位模式复制。两个 264-byte 金标均重复采集一致：copy SHA-256 为 `d98dbc978452afcb4c79c14cd342644f2ee22b5f7e56ee53f6479aabf0c6e8b0`，sum SHA-256 为 `998c76f634b5d8c1c6fe6a475d72efe76b3a88675c4736e85f5523d9b8eb3105`。

恢复端比较结果为 `array_copy` **0/264** 和 `array_sum` **0/264** 字节差异，已纳入 `Makefile`。空构建目录完整严格自检为 **157 条 PASS**，无失败或非零比较块。本单元覆盖有效、非重叠、至少 33 double 的裸 backing；未定义的空指针、短 backing、部分重叠及越界内存不外推为已验证行为。
## P3：新增未映射标量导出 `Frac`／`rng_uniform_pos`（2026-08-21）
原 ELF `Frac`（`0xe780`）使用 XMM0 传入并返回 double；反汇编的截断转换与负数修正路径等价于有限常规输入的 `value-floor(value)`。原 ELF `rng_uniform_pos`（`0xf580`）无参数，循环调用 libc `rand`，计算 `(double)rand()/RAND_MAX`，拒绝精确 0 和 1 后返回正开区间内随机数。

原 ELF probe 覆盖 `Frac(1.25)`、`Frac(-1.25)`、正负整数、`2^52` 和显式负零；输出依次为 `[0.25,0.75,0,0,0,0]`，48-byte 金标重复 SHA-256 为 `2a97c1ef1c49bb270b4af754ef5070230476613453d88eb3b44059e4cf5a2dfc`。`srand(1)` 后前四个 `rng_uniform_pos` 结果也重复确定，32-byte 金标 SHA-256 为 `5b2533e93a565b1ba0e9a76625b6aae427436d288c2fbaf60172c2b67904e2ce`。

恢复端比较为 `Frac` **0/48**、`rng_uniform_pos` **0/32** 字节差异，已纳入 `Makefile`。空构建目录完整严格自检为 **158 条 PASS**，无失败或非零比较块。本证据覆盖所列有限值、负零和 GNU libc 固定种子序列；不外推 NaN／无穷、极端指数、强制产生 rand 端点或不同 libc 随机实现下的完整输入空间行为。
## P3：新增未映射协议编码导出 `var2buff`／`Put_*_In_Buff`（2026-08-21）
原 ELF 的 `var2buff`（`0x1bc0`）及 `Put_UINT16_In_Buff`、`Put_UINT32_In_Buff`、`Put_FLOAT32_In_Buff`、`Put_FLOAT64_In_Buff` 此前未映射。反汇编确认 `var2buff(destination, source, byte_count, endian_flag)`：flag 精确为 0 时以反向字节顺序写入；flag 精确为 1 时按内存顺序前向复制；任何其它 flag 或非正 byte_count 不写。四个 `Put_*` 包装函数分别采用整数值／字节序和字节序／浮点值的不同 SysV 参数寄存器顺序，并调用该核心完成 2、4、4、8 byte 写入。

原 ELF probe 将 64-byte 缓冲预置为 `0xA5`，对 `0x1234`、`0x12345678`、`1.25f`、`-2.5` 各以 flag 0 与 1 调用。金标显示 flag 0 为高字节优先、flag 1 为主机内存顺序；64-byte 聚合金标经两次采集一致，SHA-256 为 `52b7f2041659ab29c42eeac18920eb60a86b46aec2dd3257451308bf7d53a284`。

恢复端对完整 64-byte 金标为 **0 字节差异**，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **159 条 PASS**，无失败或非零比较块。本证据覆盖所测四种宽度、有限位模式、flag 0/1 和未触碰填充；不外推空指针、负 byte_count、非 0/1 flag、重叠 buffer 或不同主机字节序的全部未定义行为。
## P3：新增未映射协议解码导出 `read_Uint8`／`read_float32`／`read_long`（2026-08-21）
原 ELF `read_Uint8`（`0x1de0`）直接零扩展输入首字节。`read_float32`（`0x1cc0`）和 `read_long`（`0x1d40`）均读取四个字节：flag 精确为 1 时按小端组合，任何其它 flag 按大端组合；前者将得到的 32-bit 位模式作为 float 返回，后者以零扩展 32-bit 无符号值返回。该 flag 约定与 `var2buff` 的 0/1 写入方向一致。

原 ELF probe 使用字节 `[0x12,0x34,0x56,0x78]`。flag 0/1 的 float 位模式分别为 `0x12345678`／`0x78563412`；long 值分别为同一 32-bit 数值的零扩展；uint8 为 `0x12`。三个金标均两次采集一致：float 8-byte SHA-256 `cfd44df72dbe92164e8046e011c234582da6af3cd7bc13ea39f88b4f5b53aab3`，long 16-byte `86ac661d095b5ccae9c5908592e466ec4ce7b86030097bd9ad089f332bb267c4`，uint8 `f299791cddd3d6664f6670842812ef6053eb6501bd6282a476bbbf3ee91e750c`。

恢复端比较为 float **0/8**、long **0/16**、uint8 **0/1** 字节差异，已纳入 `Makefile`。空构建目录完整严格自检为 **160 条 PASS**，无失败或非零比较块。本单元覆盖有效四字节 backing 和 flag 0/1；空指针、短 backing、NaN 表示以外的浮点异常、其它 flag 的全部调用方行为不外推为已验证输入空间。
## P3：新增未映射字节序导出 `Change_Endian`（2026-08-21）
原 ELF `Change_Endian`（`0x2010`）的 ABI 为 `void Change_Endian(destination, source, int32_t byte_count)`。反汇编确认正 byte_count 时按 `destination[i]=source[byte_count-1-i]` 前向目标／反向源写入；byte_count 为零或负值立即返回，不写目标。

原 ELF probe 用 source `[0x10..0x17]` 和 16-byte target，先以长度 8 调用、再对后半 target 以长度 0 调用。前八字节写为 `[0x17..0x10]`，后八字节预置 `0x5A` 保持不变。16-byte 金标经两次采集一致，SHA-256 为 `ac16199434a929f9761845268b1dea2ea399ab60ebf7e22aee2add1a2bd15f6b`。

恢复端比较为 **0 字节差异**，已纳入 `Makefile`；空构建目录完整严格自检为 **161 条 PASS**，无失败或非零比较块。本单元覆盖有效非重叠 8-byte 翻转与零长度；不外推负长度、空指针、短 backing 或重叠区域。
## P3：新增未映射协议参数导出 `get_*_para`（2026-08-21）
原 ELF `get_uint8_para`、`get_uint32_para`、`get_float32_para` 与 `get_float64_para` 已按原反汇编完成恢复。对字节数组 `[0x12,0x34,0x56,0x78]` 及八字节扩展数组，flag=0 输出大端位序、flag=1 输出主机顺序位序；float 与 double 通过相同位模式重解释返回。原 ELF 40-byte 聚合金标 SHA-256 为 `5b51ec497f720dc8de3a9f439592f330a7c83018fe059faa5e5f9399c4a45775`，恢复端逐字节通过。比较器已纳入 `Makefile`，空构建目录完整严格自检现为 **162 条 PASS**，无失败或非零比较块。该证据覆盖有效 4/8-byte backing 与 flag 0/1，不外推空指针、短 backing 或其它 flag。
## P3：新增未映射几何导出 `AxisVector2Matrix`（2026-08-21）
原 ELF `AxisVector2Matrix`（`0x50e0`）的 ABI 为 `(DpMatrix *out, const DpVector *axis_x, const DpVector *axis_y, const DpVector *axis_z)`。反汇编只使用 `out->data` 和各向量 `data` 指针，忽略 descriptor 的计数与矩阵 shape/stride 字段；连续写入九个 double：axis_x 三项、axis_y 三项、axis_z 三项依次位于 out data 的 `[0..2]`、`[3..5]`、`[6..8]`。

原 ELF probe 使用三向量 `[1,2,3]`、`[4,5,6]`、`[7,8,9]`，九 double 金标 SHA-256 为 `6cf91a6516fb9202004c5ff854ddd4a37aa22e792b817b03cfbc763a4a1a3de1`。恢复端比较为 **0/72 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检为 **163 条 PASS**，无失败或非零比较块。本单元覆盖有效连续 backing 的三元素读写；不外推 descriptor 不匹配、stride padding、短 backing 或空指针。
## P3：新增未映射姿态导出 `euler_angle_312_to_quat`（2026-08-21）
原 ELF `euler_angle_312_to_quat`（`0x9210`）接受 `DpQuatAbi *out` 和经 XMM0–XMM2 传递的三个角度。反汇编确认三次半角三角函数调用及 3-1-2 四元数乘加／减法公式。恢复端以严格 C11 `sin`／`cos` 调用复放该数值路径；对输入 `(0.3,-0.4,0.7)`，原 ELF xyz 金标为 `[-0.23474953511944813,0.30440023509091957,0.07022156093803689]`。

原 ELF 的完整 descriptor 金标含调用期 `xyz.data` 地址，因此比较时仅比较 w、count/padding 的前 16 bytes 与独立 xyz backing 的 24 bytes，而不将不同进程地址误判为语义差异。descriptor 和 xyz 金标 SHA-256 分别为 `4c84e6b2a7dc0060c480e607fd243a62bb4ba148c372335d40da76776cfd5c80`、`d83246e89ced45f28b0d9d1f524bcd724687d8bc353b1f219ff29bb9c45e4ef5`。恢复端可比较范围为 **0 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检为 **164 条 PASS**，无失败或非零比较块。该证据不外推特殊角度、NaN／无穷或无效 descriptor backing。
## P3：新增未映射坐标转换导出 `Cartesian2Polar`（2026-08-21）
原 ELF `Cartesian2Polar`（`0x4fc0`）ABI 为 `(double *elevation, double *azimuth, double *radius, const DpVector *cartesian)`。对 `[x,y,z]`，它依次写入 `atan2(z,sqrt(x²+y²))`、`atan2(y,x)`、`sqrt(z²+x²+y²)`；当 SSE sqrt 的受控异常分支触发时回退 libc sqrt。正常非退化路径以输入 `[3,4,12]` 采集，输出 `[1.176005207095135,0.9272952180016122,13]`。

24-byte 原 ELF金标 SHA-256 为 `25569aaf67fb7e0741a96e514474876c3a68324f78455819a3cfb580c9854977`；恢复端为 **0/24 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检为 **165 条 PASS**，无失败或非零比较块。该证据覆盖所列非退化向量；不外推零向量、NaN／无穷或负平方和异常路径。
## P3：新增未映射协议帧导出 `escape_character_procotol`（2026-08-21）
原 ELF `escape_character_procotol`（`0x1df0`）ABI 为 `(uint8_t *destination, const uint8_t *source, int32_t byte_count)`，返回编码后总长度。函数固定写入开头 `0x7D` 和尾部 `0x7E`；source `0x7D`、`0x7E`、`0x7F` 分别编码为 `[0x7F,0x00]`、`[0x7F,0x01]`、`[0x7F,0x02]`，其它字节原样复制。对 `[0x11,0x7D,0x7E,0x7F,0x22]`，原 ELF 输出为 `[7D,11,7F,00,7F,01,7F,02,22,7E]` 且返回 10。16-byte 聚合金标 SHA-256 为 `a85aae2ee425ec8ea06a1bbfdc6863c1a61e593045f6a74588b28c1f757ba186`。

恢复端为 **0/16 字节差异**，已纳入 `Makefile`。空构建目录完整严格自检为 **166 条 PASS**，无失败或非零比较块。本单元覆盖零以外正长度、三种特殊字节和普通字节；不外推负长度、容量不足、重叠缓冲或空指针。
## P3：新增未映射姿态导出 `matrix2quat`（2026-08-21）
原 ELF `matrix2quat`（`0x9360`）已完成有限 3×3 输入的 ABI 与四主要分支恢复：函数签名为 `(DpQuatAbi *out, const DpMatrix *matrix)`，直接读取 matrix data 的连续九项；以 trace 与三个对角线项选择分支，并以“先乘 0.5、再除 root”的顺序写回被动旋转约定的四元数。输出 descriptor 中的 xyz pointer 是调用期地址，比较仅覆盖 w、count/padding 与独立 xyz backing。

原 ELF 金标覆盖单位阵、绕 X/Y/Z 180° 的三个最大对角线分支，以及绕 Z 90° 的非退化正 trace 分支。五场景恢复端均逐字节匹配相应原 ELF 金标，比较器已纳入 `Makefile`。空构建目录完整严格自检为 **167 条 PASS**，无失败或非零比较块。此证据覆盖有限正交矩阵和已列分支；不外推非正交、接近退化、NaN/无穷、特殊负根号或无效 descriptor。

## 进程全局共享 IPC getter 隔离闭环（2026-08-21）

为恢复 `init_shared`、`close_shared`、`get_float_value`、`get_uint8_value`、`get_all_floats` 与 `get_all_uint8s`，本轮在测试前后删除唯一的本机 POSIX 临时对象 `/sharedvars_example`，在原 ELF 的新进程中调用初始化，并只向映射的测试 payload 注入两个受控值：浮点区 index 7 的 IEEE-754 位型 `0xc0200000`（`-2.5f`）与字节区 index 7 的 `0xa5`。原 ELF 金标由 `analysis/time_orbit/shared_ipc_getters_p2_probe.gdb` 采集；读取后立即关闭映射，探针和恢复端都不保留该对象。

| 导出或路径 | 固定输入与检验对象 | 原 ELF 观察 | 恢复端位级结果 |
|---|---|---|---|
| `init_shared` | 空全局状态、新建 0xbf0-byte POSIX 对象 | 返回 0；映射/初始化共享读写锁 | 返回码 0，与后续读取态一致 |
| `get_float_value(7)` | payload `+0x38+4*7=0xc0200000` | 返回 0，输出 4 bytes | 0/4 bytes 差异 |
| `get_uint8_value(7)` | payload `+0x998+7=0xa5` | 返回 0，输出 1 byte | 0/1 bytes 差异 |
| `get_all_floats` | `+0x38..+0x997` | 读锁下复制 0x960 bytes，返回 0 | 0/2400 bytes 差异 |
| `get_all_uint8s` | `+0x998..+0xbef` | 读锁下复制 0x258 bytes，返回 0 | 0/600 bytes 差异 |
| `get_float_value(600)` | 无符号超出 `0..599` | 返回 `-21` | 返回码 0/24 bytes 差异（含完整返回码表） |

恢复实现保持原错误优先级：未映射时初始化失败映射为 `-20`，有效映射下 index 越界为 `-21`，随后空输出为 `-22`。`close_shared` 按原顺序解除 0xbf0-byte 映射、清空指针、关闭非负文件描述符并置 `-1`。比较程序 `analysis/shared_ipc_getters_p2_gold_compare.c` 已纳入 `make selftest`；从空构建目录运行的完整严格自检得到 **168 条 PASS**，其中该新增场景的返回码 24 bytes、单值 4+1 bytes、浮点批量 2400 bytes、字节批量 600 bytes 都是 **0 字节差异**。

> **边界。** 这项证据仅覆盖 Linux x86-64、POSIX 共享内存创建态、上述两个写入位置、单进程锁未竞争路径以及一个浮点越界值。它不证明跨进程竞争调度、共享对象已存在且来自其他生产者、系统调用错误路径或任意 payload 内容均已穷尽等价；本工程继续仅限离线仿真研究，不用于飞行、控制、执行机构或安全关键用途。


## P3：`quat_to_euler_angle_312` 的异常终止 ABI 闭环（2026-08-21）

原 ELF `quat_to_euler_angle_312`（`0x2f40`）的表面控制流是：复制 `DpQuatAbi`、`quat_reunit`、`quat2matrix`，随后理论上将 `atan2(-m[3],m[4])`、`asin(m[5])`、`atan2(-m[2],m[8])` 分别写入三个 `double *`。然而，合法输入 `q={w=1, xyz={count=3,data=[0,0,0]}}` 在这些三角函数之前必然进入内部 `quat_psi` 的局部 matrix descriptor 故障。

在 `quat_psi` 入口的寄存器注入快照中，输出 matrix descriptor 的有效 backing 指针位于 `rdi+0x08`，但 `quat_psi` 固定从 `rdi+0x10` 读取；后者是相邻 descriptor 的元数据 `0x0000000300000004`。`quat_psi+0x0c` 首次尝试将 `q.w` 写入此未映射低地址，因此原 ELF 触发 **SIGSEGV**，并且三个欧拉角输出均未写回。该现象也解释了此前直接调用 `quat2matrix` 的相同 SIGSEGV 观察。

恢复端以可见首写相同的 `q.w → (volatile double *)0x0000000300000004` 表达这一平台限定的异常 ABI。隔离子进程测试 `analysis/quat_to_euler_312_p3_death_test.c` 用有效四元数 descriptor 调用并由父进程明确验证 `WTERMSIG(status)==SIGSEGV`；已纳入 `Makefile`。从空构建目录完成 `make clean && make selftest` 后，门禁为 **169 条 PASS**。

> **范围。** 这不是一个“欧拉角数值转换已通过”的结论。当前证据仅确认本原 ELF、Linux x86-64、所观察到的局部栈布局和有效 descriptor 调用条件下的 SIGSEGV 终止语义；不外推至其它编译器、ASLR/栈布局、信号处理器、无效输入、或假定修正内部 descriptor 后的数学输出。该工程继续仅限离线仿真研究。


## P3：`vector3_to_matrix` 真实单参数矩阵 ABI 闭环（2026-08-21）

此前将 `vector3_to_matrix` 的单参数误解为向量 descriptor，因而直接调用出现 SIGSEGV。本轮反汇编 `0x8370` 确认：唯一的 `rdi` 参数实际是 **3×3 输出 `DpMatrix` descriptor**。函数在栈上构造 `left={3,1,stride=1,data=[0,0,0]}` 与 `right={1,3,stride=3,data=[0,0,0]}`，以 `alpha=1.0`、`beta=0.0` 调用 `blas_gemm(left,right,rdi,...)`；没有外部输入向量。

原 ELF 探针以 `out={rows=3,cols=3,row_stride=4}` 和 12-double backing 调用：9 个有效元素初始为 `[1,-2,3;4,-5,6;7,-8,9]`，三个 padding 为 `[99,98,97]`。金标 `gold_vector3_to_matrix_p3_data.bin` SHA-256 为 `c299aeac4287f952fa74618d15e04028d84d20e2749423cc10ba6c4e0c47d9eb`；有效元素均变为正零，三个 padding 位模式保持不变。恢复端保留原 `blas_gemm` 调用而非以简单清零替换，因此同样保留 shape 检查、stride、`beta=0` 与 NaN 的浮点语义。比较器 `analysis/vector3_to_matrix_p3_gold_compare.c` 的完整 96 bytes 为 **0 字节差异**，已纳入 Makefile；空构建目录完整严格自检现为 **170 条 PASS**。

> **范围。** 当前金标覆盖一个 `3×3`、stride 4、有限非 NaN backing 的输出矩阵路径。它不外推至 shape 不匹配、零维度、NaN/无穷、别名、无效 descriptor 或任何与函数名暗示不符的“向量输入”接口；该项仅恢复原 ELF 实际观察到的矩阵输出 ABI。


## P3：`quat2matrix` 独立异常终止 ABI 闭环（2026-08-21）

`quat2matrix`（`0x2e30`）的独立有效调用也不返回矩阵。原 ELF 在栈上先构造三组局部矩阵，再将首组地址传入 `quat_psi`；该首组对象将 backing 指针放在 `+0x08`，但 `quat_psi` 固定从 `+0x10` 读取 data。`+0x10` 恰为第二组 descriptor 的元数据 `0x0000000300000004`，因此 `quat_psi+0x0c` 在第一次尝试写入外部 `quat.w` 时向该低地址写入并触发 **SIGSEGV**。这发生在对外部输出矩阵的首次写入之前，故标准 `DpMatrix{3,3,3,data}` 和有效 `DpQuatAbi` 并不存在正常 3×3 返回金标。

已有原 ELF 探针 `analysis/time_orbit/quat2matrix_p3_probe.gdb` 以 `w=0.5, xyz=[-0.5,0.5,0.5]`、正常输出 descriptor 调用并稳定于 `quat_psi` 的同一 SIGSEGV。恢复端以同一个已审计的 `q.w → (volatile double *)0x0000000300000004` 首写表达可观察终止；比较器 `analysis/quat2matrix_p3_death_test.c` 在子进程调用并由父进程验证 `SIGSEGV`。它已纳入 Makefile，空构建 `make clean && make selftest` 的严格门禁为 **171 条 PASS**。

> **范围。** 该结果确认当前原 ELF/Linux x86-64 局部布局下的异常终止，不是正常四元数矩阵变换已通过的断言。它不外推至其它平台、编译器、栈布局、信号处置、无效对象或假设修正局部 descriptor 后的数学函数；`quat_att_mat` 是另一个已验证、可正常返回的独立导出，不能与本符号混同。


## P3：共享 IPC getter 边界与重映射闭环（2026-08-21）

在既有创建态单值／批量读取闭环基础上，新增隔离共享对象的边界金标。原 ELF 初始化后向浮点 index 599（`+0x38+0x95c`）写入位型 `0xff800000`（`-∞f`），向字节 index 599 写入 `0x5a`。调用序列为：float/byte 末元素读取、float/byte `index=-1`、float/byte `out=NULL`、`close_shared`、再次读取 float 599 触发自动重映射。

| 检查项 | 原 ELF 返回或输出 | 恢复端结果 |
|---|---|---|
| `get_float_value(599)` | 0，输出 `0xff800000` | 0/4 bytes 差异 |
| `get_uint8_value(599)` | 0，输出 `0x5a` | 0/1 bytes 差异 |
| `get_float_value(-1)` / `get_uint8_value(-1)` | 均为 `-21` | 返回码一致 |
| 有效 index＋`out=NULL` | 均为 `-22` | 返回码一致 |
| `close_shared` 后 `get_float_value(599)` | 自动重新映射，返回 0，仍输出 `0xff800000` | 0/4 bytes 差异 |

原汇编确认 index 经 `cmp ebx,0x257` 配合无符号 `ja` 判断，因此 `-1` 与 600 同为越界；检查顺序为“若未映射先初始化 → 无符号 index → 空 out → 读锁读取”。边界比较器 `analysis/shared_ipc_getters_boundary_p3_gold_compare.c` 比较完整 32-byte 返回码表、8-byte 双次 float 输出和 1-byte uint8 输出，均为 **0 字节差异**，已纳入 Makefile。空构建目录完整严格自检现为 **172 条 PASS**。

> **范围。** 本轮覆盖单进程创建态的两个末元素、负索引、空指针和关闭后同一共享对象的重映射；仍未覆盖竞争读写、不同进程创建者、权限/大小错误、系统调用失败、锁失败或异常解除映射时序。不得将这些有限结果外推为完整的进程间并发语义。


## P3：`NiceAngle` 四象限角度规范化闭环（2026-08-21）

原 ELF `NiceAngle`（`0xa750`）使用两个标量参数 `x=xmm0`、`y=xmm1`，以 `atan(y/x)` 和精确只读常量将方向角规范化至 `[0,2π)`：第一象限直接 `atan`；第四象限加 `2π`；第二／第三象限加 `π`；正 y 轴返回 `π/2`，负 y 轴返回 `3π/2`，正 x 轴和原点返回正零。原 `.rodata` 常量位型分别为 `π/2=0x3ff921fb54442d18`、`3π/2=0x4012d97c7f3321d2`、`π=0x400921fb54442d18`、`2π=0x401921fb54442d18`，恢复端以 C11 十六进制浮点字面量固定这些值。

原 ELF 探针覆盖输入 `(x,y)`：`(1,1)`、`(1,-1)`、`(-1,1)`、`(-1,-1)`、`(0,1)`、`(0,-1)`、`(1,0)` 与 `(0,0)`。金标 `gold_niceangle_p3_outputs.bin` SHA-256 为 `65b2679278115a9570eb4bcd63ec6a0eaf2ad013dcccde320478237bd3b9d873`。恢复比较器 `analysis/niceangle_p3_gold_compare.c` 的 8 个 double（64 bytes）均为 **0 字节差异**，并已纳入 Makefile；空构建目录完整严格自检为 **173 条 PASS**。

> **范围。** 此证据覆盖四个有限四象限点、两条 y 轴、正 x 轴与原点；原汇编的有序／无序比较分支已按 C 比较结构保留，但尚未以独立原 ELF 金标覆盖 NaN、正负无穷、负零符号、次正规数、极大幅值比值或浮点异常标志。


## P3：`Rotation_X`／`Rotation_Y`／`Rotation_Z` 三轴旋转矩阵闭环（2026-08-21）

三项导出均采用 `void Rotation_*(DpMatrix *out, double angle)` ABI：输出 backing 指针固定取 descriptor `+0x10`，忽略 rows、cols 与 row_stride 后直接连续写入 9 个 double。原 ELF 对每项先调用 glibc `sincos(angle,&sin,&cos)`；由此对零角仍保留 `-sin(0.0)` 的负零位型。矩阵布局分别为：

| 导出 | 连续 3×3 写入布局 |
|---|---|
| `Rotation_X` | `[1,0,0; 0,cos,sin; 0,-sin,cos]` |
| `Rotation_Y` | `[cos,0,-sin; 0,1,0; sin,0,cos]` |
| `Rotation_Z` | `[cos,sin,0; -sin,cos,0; 0,0,1]` |

原 ELF 探针在标准 `DpMatrix{3,3,3,data}` 下按 X、Y、Z 顺序，各调用 `angle=+0.5`、`-0.5`、`+0.0`，采集 9 块矩阵共 648 bytes；金标 SHA-256 为 `8d474573dfabc2288550f9eb3c4c1156199fc9997ce619d7c17010afb4f15e94`。恢复端也直接调用 glibc `sincos`，比较器 `analysis/rotation_xyz_p3_gold_compare.c` 结果为 **0/648 bytes 差异**。它已纳入 Makefile；从空构建目录 `make clean && make selftest` 现为 **174 条 PASS**。

> **范围。** 覆盖三轴、正负有限半弧度和零角，且零角通过完整位比较覆盖负零位置；尚未覆盖 NaN、无穷、大角度周期归约、无效／别名 descriptor、非连续 stride 或浮点异常标志。当前结论限于原 ELF 实际连续写回 ABI，不应扩展为一般矩阵对象安全性保证。


## P3：`s2`／`c2` 半角三角辅助函数闭环（2026-08-21）

原 ELF `s2`（`0x91f0`）与 `c2`（`0x9200`）均为单标量 double ABI：先以精确常量 `0.5` 执行一次 `mulsd`，随后分别尾跳转到 `sin@plt` 和 `cos@plt`。恢复端相应实现 `sin(angle*0.5)` 与 `cos(angle*0.5)`，保持调用相同的系统 libm 路径。

原 ELF 以输入 `0.0`、`+1.0`、`-1.0`、`+6.283185307179586`、`-6.283185307179586` 分别采集五项 `s2` 和五项 `c2` 输出。金标 `gold_c2_s2_p3_outputs.bin` SHA-256 为 `a5180e8bd0fcb029c4f261e9b3981f9233b44e82d59b896ac7d09b1023812d96`；比较器 `analysis/c2_s2_p3_gold_compare.c` 的 10 个 double（80 bytes）为 **0 字节差异**。它已纳入 Makefile；空构建目录完整严格自检为 **175 条 PASS**。

> **范围。** 覆盖零、正负常规角和接近正负一周的输入，且保留浮点周期归约的真实 libm 结果；尚未独立金标覆盖 NaN、无穷、负零、次正规数、极大参数归约和浮点异常标志。


## P3：`Eccentric2True` 五标量轨道角转换闭环（2026-08-21）

反汇编确认 `Eccentric2True` 的五个 double 参数按 `xmm0..xmm4` 进入。其计算顺序为：`first=NiceAngle(value_c,value_d)`；`second=NiceAngle(value_c-value_b,value_d*value_e)`；最后严格按 `(second+value_a)-first` 返回。恢复工程的 `dynamic_orbit.c` 已存在同 ABI、同两次归一化顺序的实现，故本轮未复制函数，而是建立原 ELF 金标并将该现有导出接入函数级回归。

原 ELF 探针覆盖一般正象限、负 x 象限、轴退化与混合符号的四组五标量输入。金标 `gold_eccentric2true_p3_outputs.bin` SHA-256 为 `0c6cf6a4fbbda637636cb4325d94a3ab07251e64915813b0b340d5be89bdd771`；比较器 `analysis/eccentric2true_p3_gold_compare.c` 的 4 个 double（32 bytes）为 **0 字节差异**。因轨道对象包含其它时间坐标导出，比较器最小链接集合为 `dynamic_orbit.o + dynamic_time.o + dynamic_math.o`。已纳入 Makefile；空构建目录完整严格自检现为 **176 条 PASS**。

> **范围。** 覆盖四组有限参数及 `NiceAngle` 组合中的多个象限/轴路径；尚未覆盖 e=0/接近 1 的物理约束网格、特殊浮点、极端相位、参数别名或上层轨道元素输入的端到端一致性。该函数级证据不代表完整轨道传播路径已被穷尽。


## P3：`isSunInFOV` 太阳视场几何判定闭环（2026-08-21）

原 ELF `isSunInFOV`（`0xde00`）的 ABI 为 `int isSunInFOV(const DpVector *sun_direction, double half_fov_angle)`：`rdi` 为太阳方向向量 descriptor，`xmm0` 为半视场角。函数在栈上构造固定 body +Z 参考向量 `[0,0,1]`，调用 `vector2angle(sun_direction,reference)`，然后执行无序比较后的 `setae`，即当且仅当 `half_fov_angle >= measured_angle` 时返回 1。

原 ELF 金标覆盖 +Z 轴内（0.5 rad）、正交轴外（0.5 rad）、反向轴配大阈值（3.2 rad）和约 0.5-rad 边界方向，共得到返回表 `[1,0,1,1]`。金标 `gold_is_sun_in_fov_p3_returns.bin` SHA-256 为 `c942b58bc8713f389c3702ee97885b2f4dcfcc6f6ff08fc4665baca5d7a24971`；比较器 `analysis/is_sun_in_fov_p3_gold_compare.c` 的 16 bytes 为 **0 字节差异**。已纳入 Makefile；空构建目录完整严格自检现为 **177 条 PASS**。

> **范围。** 覆盖固定 body +Z、三种典型几何关系和一个有限边界向量；尚未独立覆盖负阈值、零/非单位/NaN 向量、NaN 阈值、精确阈值舍入翻转、descriptor count 非 3 与并发传感器调度。该验证仅确认导出函数的局部几何判定 ABI。


## P3：`isEarthOut` 地球遮挡几何判定闭环（2026-08-21）

原 ELF `isEarthOut`（`0xd760`）ABI 为 `int isEarthOut(const DpVector *position_gci, const DpVector *view_direction, double margin_angle)`。它无防御性检查地用 `vector_axpby(position,-1,negative_position,0)` 构造 `-position`，计算其与视线的夹角，再计算 `asin(R_Earth/|position|)+margin`，最后仅在夹角**严格大于**该阈值时返回 1。恢复端保留已有防御性 `dp_is_earth_out`，另新增同名无防御 ABI 导出。

原 ELF 在 7000 km x 轴位置下，对地 `-X`、正交 `+Y`、背地 `+X` 三条单位视线的返回分别为 `[0,1,1]`。金标 `gold_is_earth_out_p3_returns.bin` SHA-256 为 `45adb8cb9992e0cbf88fa58318655bf3eff7d9c673c95a075084019c7bd36483`；比较器的 12-byte 返回表为 **0 字节差异**。已纳入 Makefile；空构建目录完整严格自检现为 **178 条 PASS**。

> **范围。** 覆盖单一有限位置、三条轴向视线与零 margin；尚未覆盖阈值等号、不同高度、近地/地内、非单位或零视线、NaN/无穷、异常 descriptor 及上层星敏感器调度。


## P3：`isSunOut` 太阳夹角阈值判定闭环（2026-08-21）

原 ELF `isSunOut`（`0xd730`）ABI 为 `int isSunOut(const DpVector *first, const DpVector *second, double threshold)`。汇编交换前两个 descriptor 后调用 `vector2angle(second,first)`，再用 `ucomisd(angle,threshold); seta` 返回。因此语义为**夹角严格大于阈值**时返回 1，相等时返回 0。

原 ELF 金标覆盖共线夹角 0 配阈值 0、正交夹角配阈值 0.5 和约 0.5-rad 向量配阈值 0.5，返回表为 `[0,1,0]`。比较器 `analysis/is_sun_out_p3_gold_compare.c` 对 12-byte 返回表为 **0 字节差异**；已纳入 Makefile，空构建目录完整严格自检现为 **179 条 PASS**。

> **范围。** 覆盖严格比较的相等边界与两个典型夹角；尚未覆盖 NaN/无穷、零向量、非单位向量、count 不匹配、负阈值及浮点异常标志。


## P3：`ran_gaussian`／`ran_gaussian2` 随机辅助导出闭环（2026-08-21）

原 ELF `ran_gaussian(sigma)` 使用拒绝采样的极坐标/Box–Muller 路径；`ran_gaussian2(mean,sigma)` 先调用前者再按 `random + mean` 返回。恢复端已有相同内部实现，本轮新增同名包装导出和固定种子金标。

以 `srand(1)` 采集两次 `ran_gaussian`（sigma=1、2），并重置种子后采集两次 `ran_gaussian2`（mean/sigma=(3,1)、(-2,0.5)）。32-byte 金标 SHA-256 为 `69b8aa35e121cab356cd489e1de4b830a4da7997def4baf26f3804972dc95af0`；比较器逐字节通过。Makefile 空构建严格自检现为 **180 条 PASS**。

> **范围。** 仅固定 glibc `rand()` 与种子 1 的连续消费序列；未覆盖其他种子、拒绝重抽次数变化、极端 sigma、NaN/无穷及多线程随机状态。


## P3：`Eccentric2Mean` 平近点角转换闭环（2026-08-21）

原 ELF `Eccentric2Mean` 仅执行两条标量指令：`xmm1 *= xmm2; xmm0 -= xmm1`。ABI 为 `double Eccentric2Mean(double eccentric_anomaly, double eccentricity, double sine_eccentric_anomaly)`，语义严格为 `E - e*sin(E)`。

原 ELF 四组输入（一般正值、混合负值、圆轨道 e=0、近小角）产生的 32-byte 金标 SHA-256 为 `75d882388b4f45075df8b0b17001bcb4d0d2f74219b0a51d1e01b15bad02715b`。恢复端已有轨道实现通过比较器逐字节复放。Makefile 已接入该比较器；空构建严格自检现为 **181 条 PASS**。

> **范围。** 覆盖四组有限输入和 e=0；尚未覆盖 NaN、无穷、负零、次正规数及浮点异常标志。


## P3：`wheel_matrix_calc` 飞轮安装矩阵列构造闭环（2026-08-21）

原 ELF `wheel_matrix_calc` 的 ABI 为 `void wheel_matrix_calc(DpMatrix *out, const DpVector *col0, const DpVector *col1, const DpVector *col2, const DpVector *col3)`。它按顺序调用 `matrix_set_col(out,0,col0)` 至 `matrix_set_col(out,3,col3)`，忽略每次返回值，最后一次采用 tail-jump。

原 ELF 金标以四个列向量 `[1,2,3]`、`[4,5,6]`、`[7,8,9]`、`[10,11,12]` 写入连续 3×4、stride=4 输出矩阵，96-byte 数据 SHA-256 为 `dec18b1f462e5aa2890c148e0025e226396ab90ccfc81730728c1e49addfe1a0`。恢复端比较器逐字节通过，Makefile 空构建严格自检现为 **182 条 PASS**。

> **范围。** 覆盖正常 3×4 连续 descriptor；尚未覆盖列越界、任一来源长度不等导致的部分写回、非连续 stride、别名及空指针异常路径。
## P3：`quat_cross` 同名四元数乘法导出闭环（2026-08-21）
原 ELF `quat_cross`（`0x8d20`）ABI 为 `void quat_cross(DpQuatAbi *out, const DpQuatAbi *left, const DpQuatAbi *right)`，寄存器顺序为 `rdi=out,rsi=left,rdx=right`。反汇编显示其依次执行 `vector3_cross(left.xyz,right.xyz,out.xyz)`、`vector_scale(out.xyz,-1)`、以 `left.w` 和 `right.w` 对 `out.xyz` 做两次 AXPY，以及从零逐项累加的 `vector_dot(left.xyz,right.xyz)`；因此可观察输出是 **`right × left`**，而不是以参数名推断的 `left × right`。

非交换金标取 `left=(0.5;1.25,-2.5,3.75)`、`right=(-1.5;4.25,-5.5,6.75)`，原 ELF 写回 `out=(-45.125;-3.5,-6.5,-6.0)`。`gold_quat_cross_p3_output.bin`（24 bytes）SHA-256 为 `1c8f74d32cea438eb8e01e7523da571e0965921af67dd0cafbfc012e4485a09e`，`gold_quat_cross_p3_xyz.bin`（24 bytes）SHA-256 为 `95c440b2c25422b73d87155e8ab40a721a917965f5289d15509ca299f5cecf5d`。回归比较输出 descriptor 的地址无关前 16 bytes、data 指针身份以及三项结果 24 bytes，均为 **0 字节差异**。Makefile 已接入；从空构建目录执行完整严格自检得到 **183 条 PASS**。
> **范围。** 覆盖一组非交换有限正规输入及输出 descriptor 保留字段；尚未覆盖空指针、count 非 3、descriptor/backing 别名、负零、NaN、无穷、极端有限值和浮点异常标志。该闭环仅对指定输入集提供证据，不构成全输入空间等价主张。
## P3：`y_q_unit` 状态四元数归一化导出闭环（2026-08-21）
原 ELF `y_q_unit`（`0x5da0`）ABI 为 `void y_q_unit(double state[33])`。它只读取和写回 `state[0..3]`：按 `q0² + q1² + q2² + q3²` 的顺序逐项累加，计算 `sqrt`，然后严格按 `q0/=norm`、`q1/=norm`、`q2/=norm`、`q3/=norm` 做四次直接除法。范数严格小于 0.01 时，原 ELF 在第一次写回前输出且无换行：`norm = %f, q0 = %f,q1= %f,q2= %f,q3= %f`。

正常范数金标以四元数 `[0.5,-1.5,2.5,-3.5]` 和其余 29 个哨兵状态分量采集，完整 264-byte 状态 SHA-256 为 `abf20426c29ee8816ad9412e320c029c57ceac2391275a5929b1eef799ae5600`。小范数金标以 `[0.001,-0.002,0.003,-0.004]` 采集，状态 SHA-256 为 `7b5e18b57ec9f0da4f48d36c99c6aa0b44dde8182c118cd2c95612b4f686cf64`，71-byte 无换行 stdout SHA-256 为 `de0d3993f119eb8493ebf3fa018f96cd61e039d2a48a167b738d9008b9347b7f`。恢复端比较器分别逐字节验证两组完整状态和小范数诊断文本。

`analysis/y_q_unit_p3_gold_compare.c` 已接入 Makefile。日志 `analysis/elf_c_diff/full_clean_selftest_2026-08-21.log` 记录从空构建目录执行的完整严格自检，共 **184 条 PASS**，包括 `y_q_unit P3 original-ELF compare: PASS (bitwise)`。
> **范围。** 覆盖两组有限非零四元数、普通范数与严格小于阈值的诊断路径，并验证后续 29 个状态 double 未被改写；尚未独立覆盖零范数、阈值恰等于 0.01、负零、NaN、无穷、溢出、次正规数及浮点异常标志。有限金标不构成全部输入等价声明。

