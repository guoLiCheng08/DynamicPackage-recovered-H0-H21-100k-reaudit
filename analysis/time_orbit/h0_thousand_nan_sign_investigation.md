# H0-1000：非有限值符号传播诊断记录

> 状态：持续诊断中。仅记录受控 H0 场景下的原 ELF 金标事实；不外推为全输入空间等价。

## 场景与见证

H0 为高偏心、反太阳、跨日期、全柔性、零设备命令且随机种子固定的 `dyn_main` 场景。每步比较 CoreDynamic 前 `0x108` bytes、主遥测 `0x220` bytes、IPC payload 3000 bytes 与全局 `y[33]`。

| 里程碑 | 结论 |
|---|---|
| 原 ELF 1000 步双采集 | 四类金标逐字节一致，场景确定性已确认。 |
| 初次恢复端比较 | 第 101 步起出现 NaN 符号位分叉。 |
| `quat_att_mat` 全负 NaN probe | 原 ELF矩阵金标表明 `blas_gemm` 的左操作数顺序可观察。 |
| `quat_diff` 全负 NaN probe | 原 ELF导数为 `w=+NaN, xyz=(-NaN,-NaN,+NaN)`；修复 `blas_gemv` 后逐位一致。 |
| RK4 反汇编对照 | 修复阶段 trial、加权和、最终写回的 SSE 左操作数顺序后，H0 第 102 步核心状态与全局 y 已完全一致。 |
| H0 第 102 步 DSS 探针 | 原 ELF与恢复端 DSS measure 均为 NaN；原 `valid_flag=(0,0)`。对非有限 `sun_body` 取消恢复端强制 legacy-NaN 角标记后，H0 第 102 步主帧和 IPC 的两处 DSS flag 差异消失。 |

## 已修复的可观察算术顺序

1. `blas_gemm`：原 ELF 按 `sum = sum + ((A * alpha) * B)`，输出为 `sum + (C * beta)`，且 SSE 目标寄存器对应左操作数。
2. `blas_gemv`：原 ELF按 `dot = dot + (A * x)`，随后 `dot = dot * alpha`，输出为 `dot + (y * beta)`。
3. `quat_att_mat`：显式调用 `quat_psi`，不以手写等价式替代原调用链。
4. `dp_rk4_step_33`：四个阶段均使用原 ELF 的左操作数方向，暂存 `h*k` 缓冲区以零初始化，最终保持 `weighted + y` 的原次序。
5. DSS：H0 所见非有限机体系太阳输入不应强制触发恢复端的 legacy NaN 角度规则。

## 当前最早剩余分叉

修复后，H0 第 102 步 CoreDynamic、主帧、IPC 及全局 `y` 均为零差异。**第 103 步**成为最早不一致处：全局 `y` 出现 17 个 NaN 符号字节差异，CoreDynamic 第一块出现 10 个 NaN 符号字节差异。第 102 步结束时原 ELF `y` 已在所有或多数状态分量进入 NaN 域；第 103 步分叉应从 `differential_equation` 的第一阶段导数与其子调用的向量/矩阵运算进行原 ELF—恢复端阶段快照比较。

下一步应建立：在相同第 102 步前态与相同全局模型/设备状态下，采集原 ELF `differential_equation` 输出的 33 个导数位型，并与恢复端 `dp_differential_equation_33` 的对应导数比较。之后仅对首次不一致分量的调用链展开，以避免盲目修改全局模型。

## 文件

- `analysis/coverage_inventory/h0_thousand_compare_after_dss_nan_fix.log`：当前 1000 步比较日志。
- `analysis/time_orbit/gold_h0_step102_dss.bin`：原 ELF第 102 步 DSS 对象快照。
- `analysis/time_orbit/gold_h0_step102_core.bin`、`gold_h0_step102_main.bin`：原 ELF第 102 步输出快照。
- `analysis/time_orbit/quat_att_mat_h0_step101_nan_probe.gdb`、`quat_diff_h0_step101_nan_probe.gdb`、`y_q_unit_h0_step101_nan_probe.gdb`：已建立的非有限输入函数级取证。

所有操作仅用于离线仿真与二进制行为研究，不得用于飞行、控制、实物执行机构驱动或安全关键用途。

## 第 103 步的导数级定位进展

在 H0 第 102 步结束后，原 ELF 和恢复端的 `y[33]`、CoreDynamic、主帧与 IPC 均一致。对同一 `y` 及同一恢复／原 ELF 全局状态直接调用 `differential_equation` 后，33 项导数仅有一项不一致：`derivative[12]`（GCI z 向轨道加速度）的 qNaN 符号位，原 ELF 为 `0xfff8000000000000`、恢复端为 `0x7ff8000000000000`。

进一步的直接 `orbit_dynamic` 探针确认三轴输出中仅 z 项符号不同：原 ELF `(-NaN,-NaN,-NaN)`，恢复端 `(-NaN,-NaN,+NaN)`。同一位置和日历时间的 `GCI2ECEF` 三项中间位置则已逐位一致，且均为 `(-NaN,-NaN,-NaN)`。因此下一步应在 `orbit_dynamic` 内采集 ECEF 重力加速度（J2/J3/J4 累加结束、调用 `ECEF2GCI` 之前），并对照原 0x9c88..0x9fxx 的 SSE 操作数顺序；无需再扩散审计姿态、RK4 或传感器路径。

这使第 103 步首次分叉从“17 个状态符号差异”缩小为一个已定位的 `orbit_dynamic` z 加速度符号传播差异。

### 轨道直接金标与 RK4 阶段的边界

已建立并通过 `gold_h0_step102_orbit_acc.bin`：在 H0 第 102 步后直接调用 `orbit_dynamic`，恢复端三轴输出现与原 ELF均为 `(-NaN,-NaN,-NaN)`，同时 `differential_equation` 的直接 33 项比较已将先前唯一的 `derivative[12]` 分叉消除。

然而重跑完整 H0-1000 后，`dyn_main` 第 103 步仍开始分叉。这说明责任边界已从**单次微分方程输出**进一步收缩到 **RK4 的第 2–4 阶 trial state 或阶段加权组合**：这些中间状态并非第 102 步结束的原始 `y`，其 NaN 符号组合可触发不同的非有限轨道／刚柔导数路径。下一步必须为原 ELF 与恢复端分别记录第 103 步 RK4 的 `k1..k4` 及四个 trial state，按第一处不一致阶段继续细化；不能仅依赖最终 `orbit_dynamic` 输入的直接函数金标。

暂未把本项计入严格主回归或宣称 H0-1000 通过。当前严格主基线仍需在清理诊断试验后完整重跑。

## 基线回归保护

在引入上述受控非有限值诊断与修复后，执行了 `make clean && make selftest`。既有严格门禁完整通过；末尾的 leap-day LEO 阴影／DSS0-noise／全柔性十步场景继续对 CoreDynamic、主帧、IPC payload、全局 y 报告逐块 `0` 失配。H0-1000 仍为独立诊断场景，当前不计入该已验收门禁数，直至其 1000 步全路径通过并经第二次原 ELF采集复核。

## 原 ELF RK4 阶段缓冲区布局

`RK4_Intergrator` 的 0x7a8-byte 栈帧已经确认：`rsp+0x30` 为 `h*k1`，`+0x140` 为 `h*k2`，`+0x250` 为 `h*k3`，`+0x360` 为 `h*k4`，`+0x470` 为 `differential_equation` 输出缓冲，`+0x580` 为每阶段 trial state，`+0x690` 为最终加权状态。原函数按 `k4 + 2*k3 + 2*k2 + k1`、再乘 `1/6`、再加 `y`，最后调用 `y_q_unit`。H0 第 103 步阶段探针应在四次 `differential_equation` 返回后分别导出 `rsp+0x470` 和对应 trial／h*k 缓冲区，并同步获取恢复端同一阶段快照。

### 阶段探针当前限制

已建立 `dyn_main_h0_step103_rk4_stage_probe.gdb`，并确认四个原 ELF断点地址：`RK4_Intergrator+0x1a7`、`+0x272`、`+0x31e`、`+0x363`。尝试通过 GDB `call` 或手工 RIP 跳转进入嵌套 `dyn_main` 时，调试器会中止函数调用上下文或直接结束目标进程，尚未落盘任何 `hk*`／`trial*` 文件。因此这些文件**不是金标**，不计入任何通过项。替代路径应为：构造专门的原 ELF C 调用跳板（由普通 `main` 调用 `dyn_main`，而非 GDB `call`），或在第 103 次调用前设置进程启动即生效的断点计数器，以避免 GDB 嵌套调用限制。

### 调度确认

原 ELF `dyn_main` 调用 `CoreDynamic`，且 `CoreDynamic` 在 `0x5926` 直接调用 `RK4_Intergrator`。因此 H0 第 103 步最终状态在直接 `differential_equation` 金标均已一致而整步仍分叉的事实，确实必须由 RK4 的阶段构造／加权 NaN 符号传播解释，而非来自未参与该调度路径的另一积分器。

### 工作副本说明

当前长时域工作目录 `/home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work` 是从用户提供的已核验归档提取的文件快照，目录中不含 `.git` 元数据。后续每项可接受改动应以独立原 ELF金标、恢复端位比对和空构建回归日志为回退／审计依据，而不是依赖 Git 历史。

## 第 103 步阶段金标已采集

采用可执行映射中的真实 `call dyn_main` 跳板后，原 ELF阶段快照已成功生成（最初继承路径写入旧工程，已逐一复制并 SHA-256 核验到当前工作副本）。每个 `hk*`／`trial*` 文件为 264 bytes。恢复端通过仅诊断编译开关导出了 1000 个 RK4 调用的阶段记录；第 103 次调用（记录序号 102）已与原 ELF对比。

最早不一致已确定在 **`h*k1[4]`**：原 ELF `0x7ff8000000000000`、恢复端 `0xfff8000000000000`。四元数导数 `h*k1[0..3]` 在该阶段一致；首项差异是 x 向角速度导数，而非 RK4 的 trial／加权循环本身。故下一步优先审计 `CoreDynamic` 在进入 RK4 前更新的角动量、外力、磁力矩、重力梯度力矩和 `UpdateTorque`，并在原 ELF与恢复端分别快照其输入全局量。 

### 积分前全局量交叉核验

已在原 ELF 的 `RK4_Intergrator+0x1a7` 断点和恢复端 `CoreDynamic` 进入 RK4 前诊断点分别快照：`H_w_B_mem`、`L_c_B_mem`、`F_I_external_mem`、`J_c_B_mem`、`J_c_B_inv_mem`。恢复端的连续二进制布局共 216 bytes，五项与原 ELF金标均为 0 byte 失配。

因此 `h*k1[4]` 的正负 NaN 差异并非这五个持久全局对象的直接差异。原 ELF 保留了 `dynamics_flex`（0xe920，0xb6d bytes）符号；下一轮应在其返回至 `differential_equation` 后截获局部刚体 RHS、有效惯量和角加速度描述符，再逐层比较恢复端 `dp_flex_dynamics_step` 的对应中间量。

### `dynamics_flex` ABI 审计进展

原 ELF `dynamics_flex` 在 `0xe920` 建立 `0x908`-byte 栈帧，入口保留 `rdi` 为刚体角加速度输出 descriptor、`rsi` 为另一输出 descriptor、`rdx`／`rcx`／`r8`／`r9` 及调用栈后续槽位为刚柔状态和矩阵／向量 descriptor。入口立即由 Sat+0x898 耦合矩阵构造有效惯量，后续以 `blas_gemm`、`matrix_sub`、`blas_gemv`、叉乘和多项投影形成刚体 RHS。其完整签名尚不应猜测；优先方案是让第 103 步真实调用触发函数返回断点，并从保存的输入／输出 descriptor 回收角加速度金标。

## `dynamics_flex` 返回边界金标与否证结果

在 `differential_equation+0x42d`（即 `dynamics_flex` 返回后的下一条指令）设置仅触发一次的临时断点，并在第 103 次 `dyn_main` 的首个 RK4 阶段导出输出 descriptor。角加速度 descriptor 指针在 `[$rsp+0x38]`；原 ELF 的 24-byte 金标 `gold_h0_step103_angular_acceleration.bin` 为三项正 quiet-NaN：`7ff8000000000000, 7ff8000000000000, 7ff8000000000000`。这与同一阶段 `h*k1[4]` 的正 NaN 位型一致，证明首分叉确实位于刚柔动力学产生的 x 向角加速度而不是 RK4 后续加权。

进行了一个仅为反证的恢复端试验：对“全 body-rate 为 NaN 且三轴角加速度均为 NaN”的所有情况，在模态投影前强制正 qNaN。H0 完整重跑立即显示该条件过宽：第 101 步原先一致的 CoreDynamic/state[4..6]、主输出、IPC 与 global-y 被错误地由原 ELF所需负 qNaN 改为正 qNaN。该补丁已立即撤回，且不应复用。后续应采集并比较第 101 与第 103 首阶段的 effective inertia、RHS 基项、耦合项和 inverse-times-RHS，确定真正区分两个时刻的原 ELF操作数传播路径。

## 本轮回归与金标完整性

原 ELF H0 第 103 步阶段探针已连续运行两次；`hk1..hk4`、`trial1..trial3`、`H_w_B`、`L_c_B`、`F_I_external`、`J_c_B`、`J_c_B_inv` 以及新增 `angular_acceleration` 共 13 个文件逐文件 `cmp` 全部通过，证明当前探针路径与调试执行副本的采集稳定。新增角加速度金标 SHA-256 为 `38942dc703543c2a5d23412f7713dab1eb9a3fecdd6f11a5ba15240df007de5c`。

撤回过宽试验规则后，以默认严格 C11 配置重新运行 H0-100 步，100 次的 state／main-output／IPC／global-y 逐步 bitwise 比较全部通过。随后执行 `make clean && make selftest`，完整既有门禁通过，日志为 `analysis/coverage_inventory/baseline_after_step103_angacc_probe_full_clean_selftest.log`。该结果只恢复并确认短时域与既有受控门禁；**H0-1000 仍未通过，未加入 Makefile 主门禁。**

## 基础 RHS 操作顺序修复：第 103 步首个刚体分叉已闭合

扩展原 ELF探针确认：第 103 次 `dyn_main` 首个 RK4 阶段中，`dynamics_flex` 调用 `vector3_cross` 前的 RHS 已为 `(+NaN,+NaN,-NaN)`，并且其两个叉乘输入（body-rate、局部 `H_total`）均为 `(-NaN,-NaN,-NaN)`；恢复端对这两个输入以及 effective inertia 和 inverse 已逐字节一致，但旧恢复实现将基础 RHS 代数重排为 `0 - (omega×H) - (-L_c_B)`，得到三项负 NaN。原 ELF则先以 `L_c_B` 初始化 RHS（调用边界输入为 `-L_c_B`），再执行 `RHS - (omega×H)`，因此保留原始 `L_c_B` 的 NaN 符号。

已将 `dp_flex_compose_rigid_rhs_base` 改为按位取反 `minus_term` 以得到 `L_c_B`，随后调用 `vector3_cross` 和 `vector_sub`；不得用乘以 `-1` 或原来的代数重排替代。刚柔块 selftest 与 H0-100 均通过。默认 H0-1000 重跑显示原先第 103 步最早分叉已消失，但仍在后续 **观测步 104** 首次出现 state／telemetry／IPC／global-y 的 NaN 符号差异；H0-1000 依然不得列为通过或纳入 Makefile 主门禁。下一步必须对修复后的第 103 次 RK4 的 k2/k3/k4 和最终加权状态进行逐阶段重取证，不能将“首分叉后移”误报为完整收敛。

## 第 103 步模态耦合缓冲复用取证

在 `dynamics_flex` 的第 103 次首阶段，原 ELF最终阻尼子链的 `Sat+0xd38` 矩阵、10 维 `eta_dot` 输入与其 10 维 `D·eta_dot` 输出均已采集。恢复端对输入矩阵和输入向量逐字节一致，但旧实现复用 `MA·eta` 的 10 项输出缓冲作为 `D·eta_dot` 的 `blas_gemv` 输出；由于原 `blas_gemv` 即使 `beta=0` 仍计算 `dot + old_y*beta`，旧负 qNaN 污染了应为有限值的阻尼输出。将该 10 项缓冲重新清零后，恢复端 `D·eta_dot` 与原 ELF 80-byte 金标逐字节一致。

同一机理还存在于随后的 3 项耦合投影：旧代码复用首个 `C·MA·eta` 的 qNaN 输出作为第二个 `C·(D·eta_dot)` 的 `blas_gemv` 目的地。已在第二次 C 投影前清零 `three_term`，与原 ELF为该调用设置独立零缓冲的反汇编相符。两项缓冲修复均保持 `flex_rigid_selftest` 与 H0-100 步全块 bitwise PASS。

不过，在默认 H0-1000 重跑中，最早公开输出差异仍显示于观测步 103，且 H0-1000 仍失败。这说明仍需以**修复后重新生成的 RK4 stage trace**（特别是第 103 次调用的 hk1..hk4）检查是否有其他刚柔／模态导数分量或 RK4 组合项尚未同步；不得仅依赖当前局部 `D·eta_dot` 金标推断整步已经闭合。当前 build/ 是默认构建之后的后续诊断构建重建前状态之一，下一轮正式验收前必须 `make clean && make all`。

## H0-1000 收敛与正式回归接入（本轮完成）

本轮以原 ELF反汇编、局部二进制快照和逐阶段 gold 为依据，完成了第 103 次 RK4 调用的剩余 NaN 位型收敛。修复点均为受控、金标支持的操作数／缓冲语义恢复：`vector_add` 改为显式保留左 destination 的 SSE `addsd` NaN 符号语义；基础刚体 RHS 恢复为先初始化 `L_c_B` 再扣除 `omega×H`；`MA·eta`、`D·eta_dot` 和两次 C 投影不再错误复用 `beta=0` 仍可传播 qNaN 的旧输出缓冲；全 ECEF 位置 NaN 的轨道输出三轴按原 gold 保持负 qNaN；全 GCI 位置 NaN 的核心地磁输出三轴亦按原 core-output gold 保持负 qNaN。所有这类规则均有具体 H0受控轨迹金标支撑，不应推广为一般非有限输入的理论证明。

修复后，H0第 103 次 RK4 的原 ELF七块阶段金标均逐字节通过：`hk1`、`trial1`、`hk2`、`trial2`、`hk3`、`trial3`、`hk4` 均 PASS。随后在**无诊断宏**的默认严格 C11 构建下，H0-1000 的每一步均比较四类可观察输出：调用者 CoreDynamic 前 0x108（264 B）、主遥测帧（544 B）、IPC payload（3000 B）和全局 `y[33]`（264 B）；1000 × 4 块均为 0-byte mismatch，比较器报告 `PASS (bitwise)`。

原 ELF H0-1000 gold 在通过后重新采集一次，并与此前版本逐文件 `cmp` 一致；SHA-256 保持不变：state `43c8396c1c6b40d6011ac476e9a1a2aaf683434500da3e66c8fdd1eae5344009`，global-y `a28cc71a925db48df22acde2334327c52444bed817e4dda8ba64d24222a8d065`，main output `fdae7c3a2c4eca07427fb1f76ffb1847a0c63308d3133789fb39e567058df7a6`，IPC `45326a9db90401b32a3050d08092f51957c8611840281e5bcf839517ae6e8b47`。

H0-1000 比较器现已加入 `Makefile` 的 `selftest`。一次 `make clean && make selftest` 在默认严格 C11 标志下通过，包含 H0-100 与 H0-1000 的正式门禁；`make -n selftest` 统计为 **278 个**测试可执行项。原样本核验保持 `mode=0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> 结论边界：H0-1000 PASS 是对这个明确给定的离线、高偏心／阴影／日期边界／全柔性／零设备命令／固定随机种子的单一受控场景的逐步 bitwise 证据。它不等价于任意输入、异常、并发、平台或编译器上的数学全域证明。该恢复工程仅限离线研究与仿真，不得用于飞行、控制、实体执行机构或任何安全关键用途。

下一步应在不宣称全空间证明的前提下建立不同日期、初始姿态、环境和设备命令的 100／1000 步矩阵；每增加一个场景，均遵循“原 ELF双采集 gold → 最小恢复实现 → 分层 bitwise 比较 → 默认完整回归”的流程。

## 新场景 H1：mixed-command 100 步基线

在 H0-1000 收敛后，新增 H1 作为与 H0零设备命令不同的受控设备命令场景。初始条件为 2025-01-02 03:04:05、近圆 LEO（a=7,000,000 m，e=0.01）、单位四元数、初始体角速度 `(0.001,-0.002,0.003)`、初始 GCI 位置 `(7,000,000,0,0)`、初始 GCI 速度 `(0,7546,0)`，固定 `srand(1)`。第 2 至第 9 次调用依次施加轮扭矩、MTQ 矩、SADA flag/角指令、thruster 和惯量更新标志、反向轮扭矩、第二组 SADA 指令、第二轴 MTQ 与第二次 thruster；之后至第 100 步均为零命令。

原 ELF H1 gold 已双采集，四文件逐字节一致：caller CoreDynamic 26400 B (`d4f916f193c0a816f709ec6b9e1f4a9052a2114a34d19b431414b3ca8fea082a`)，global-y 26400 B (`980dc9a4f5851ca41515ca98eb96cc068200d2127e297b9d929c686a1df61023`)，main 54400 B (`7c7e6f7ca0862c4b1d2e2ae6723aae138305ec11991da32e9b344dafe880993a`)，IPC 300000 B (`07976336021846eca34246208a31ef5e82c0f19e7ad0a7ccbc8a8eab3a363531`)。

新建恢复端逐步比较器以相同初始条件／命令日程对每步的四类对象作字节比较；首次运行即 100 × 4 块全部 0-byte mismatch，报告 `dyn_main mixed-command hundred-step original-ELF compare: PASS (bitwise)`。该 H1-100 比较器目前尚未加入 Makefile；下一步是将**同一输入日程**延伸到 1000 步并重复原 ELF双采集及差分收敛，之后再决定是否接入正式门禁。

## H1 mixed-command 1000 步收敛与正式门禁接入

H1 的同一初始条件和前 9 步混合设备命令时间表已从 100 步延伸至 1000 步。原 ELF四类 gold 双采集逐文件一致：caller CoreDynamic 264000 B (`d3b6fccd5eccb5d8be5c58ceec04cf30e685012171f74b32c767c757d7aff6c6`)，global-y 264000 B (`0ec6311b4214cdac6d95c3ccb6b54241128f3f7a0051a2066f7d3c4a531defe1`)，main 544000 B (`b871b630c32da0457d55a121c66304e2e202b9d150ce90e040da08d58d3675af`)，IPC 3000000 B (`35dc27a0c5959b9e3b44385b1ca2035dbfdd5a62ba12f5c38c7ba4a2dae6f60a`)。

默认严格 C11 恢复端首次运行 H1-1000 即逐步 bitwise PASS：1000 × 4 个观测块（caller CoreDynamic 264 B、global-y 264 B、main 544 B、IPC 3000 B）全为 0-byte mismatch。H1-100 与 H1-1000 比较器均已加入 Makefile `selftest`。随后一次 `make clean && make selftest` 在默认严格标志下通过，H0/H1 的 100 与 1000 步四个长时域门禁均报告 PASS；`make -n selftest` 统计为 **280 个**测试可执行项。

> 覆盖边界：目前 H1 增加的是单一、固定种子、固定初始日期和近圆 LEO 参数下的有限混合设备命令日程；它增强了命令路径的受控证据，但仍不覆盖任意命令序列、持久实时输入、并发／IPC竞争、不同 ABI 平台或所有异常值组合。不得将 H0/H1 的有限场景 PASS 宣称为全输入空间证明；工程仍仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H2：闰日、替代四元数、全柔性状态的 100／1000 步覆盖

H2 使用 2024-02-29 12:34:56、`a=7,078,137 m`、`e=0.05`、替代四元数 `(0.5,-0.5,0.5,0.5)`、体角速度 `(-0.006,0.011,-0.009)`、指定 LEO GCI 位置／速度以及完整的 20 维非零柔性状态，保持零设备命令和 `srand(12345)`。该场景与 H0 的高偏心日期边界及 H1 的混合设备命令均不同，专门扩展闰日历法、姿态和全柔性初始状态的受控证据。

H2-100 与 H2-1000 均完成原 ELF双采集并逐文件一致。H2-1000 哈希为：state `346a83e713dee0040755c65a4b71ad9ddec7882f58d673d742daba7407477e1f`，global-y `3aca30edeb9e91f72fd8cee29b3e5db2923e2caf8293b0b6c55ec28465b3d55b`，main `a90dbcdcec63d1edfc5f13900b13c8f221600f6f1c6cd53a80574f0e134f9b28`，IPC `004c58987cceeea3ab2f0cb7dfc14cb1c7befe27de146c7cf24102c04267d2f6`。

默认严格 C11 恢复端对 H2 的 100 与 1000 步均在每一步比较 caller CoreDynamic、global-y、main 和 IPC 四类对象，全部 0-byte mismatch。两比较器已接入 Makefile；之后的 `make clean && make selftest` 通过 H0/H1/H2 六个 100／1000 步门禁，`make -n selftest` 统计为 **282 个**测试可执行项。

> H2 提供另一个有限受控点而非全空间结论。所有使用限制仍然有效：只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H3：固定种子自定义 DSS 噪声的 100／1000 步覆盖

H3 在 2025-01-02 03:04:05 近圆 LEO、单位四元数和零设备命令的基础上，为两个 DSS 通道开启 Gaussian noise，并设定非零、异号的两组均值与 sigma；随机数种子固定为 `srand(12345)`。H3 与 H0/H1/H2 的关键区别是可观察的传感器噪声路径，而不是新的轨道积分初值或设备命令日程。

H3-100 与 H3-1000 的原 ELF state、main 和 IPC gold 均双采集逐文件一致。H3-1000 哈希：state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`，main `2920f9c473f98cdb99e80592fffe36d894977b21ba774bbaa9269c19d897d822`，IPC `6b388b65446e9afda48e52ec7a6a00862f79577cd6228d9ea6c01f3ac9717330`。

默认严格 C11 恢复端对 H3-100 和 H3-1000 的每步 caller CoreDynamic、main 和 IPC 输出全部 0-byte mismatch。H3 两条比较器已加入 Makefile；`make clean && make selftest` 通过 H0/H1/H2/H3 八个 100／1000 步长时域门禁，`make -n selftest` 统计为 **284 个**测试可执行项。

> 限制：H3 只验证这个固定噪声参数和固定 `rand()` 序列。它不证明任意噪声分布、不同 libc PRNG、并发传感器更新或所有随机种子下的等价性；工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## 后续候选 H4（尚未计入通过）

已审阅 `dyn_main_noncollinear_multi_actuator_ten_step_probe.gdb`：其以固定初始轮速和替代四元数为基础，在前九次调用中覆盖多轴轮扭矩、多个 MTQ 通道、推力器工作标志及惯量更新标志。该候选可用于补充 H0（高偏心边界）、H1（混合命令）、H2（闰日与全柔性）和 H3（固定 DSS 噪声）未覆盖的非共线多执行机构耦合。

当前 H4 仅有既有 10 步短时探针；**尚未**建立 100／1000 步原 ELF双采集 gold，尚未运行恢复端长时域比较，也未加入 Makefile。不得将其标记为 PASS。后续应先复制并审计该探针，分别采集 100 步和 1000 步 state/main/IPC（必要时 global-y）gold，重复采集比较后才开始恢复端差分。

## H4：非共线多执行机构 100／1000 步覆盖

H4 以固定初始轮速／角动量、替代四元数和固定 `srand(1)` 为基线，在前九次调用覆盖多轴反作用轮扭矩、多个 MTQ 通道、推力器工作标志与惯量更新标志；后续零命令演化。H4 的 100／1000 步原 ELF state、main 和 IPC gold 均双采集逐文件一致。H4-1000 哈希：state `f5dad3b6b21f7d430585671ef63317d301c4d5218c209841f28e1d2729c1e5fd`，main `01fe5a86b301f072446ff5bc278654adb5a25b5ebfb93df14ad37ae48c7f0b1e`，IPC `a06616bb72dc89d4e9575fa42659bccf73d022d59a437636bc06fd4dab08db5f`。

首次 H4-100 比较的首步不一致被证实为测试比较器新宏未触发既有多执行机构命令／初始轮速装配，并非恢复库算法差异。共享比较器已增加独立的 `DP_NONCOL_MULTI_ACTUATOR_HUNDRED` 与 `DP_NONCOL_MULTI_ACTUATOR_THOUSAND` gold 分支，同时令既有多执行机构初始化分支识别这两个宏；10 步既有宏保持不变。修正后 H4-100 与 H4-1000 全部逐步 state、main、IPC 块为 0-byte mismatch。

H4 两条长时域比较器已接入 Makefile。`make clean && make selftest` 已通过 H0–H4 共十条 100／1000 步门禁，`make -n selftest` 为 **286 个**测试可执行项。

> H4 仍只是特定九步命令表、初始轮速和固定种子的有限受控证据，不代表任意执行机构组合、故障、饱和、并发命令或全输入空间行为。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H5：固定饱和 SADA 指令的 100／1000 步覆盖

H5 在 2025-01-02 03:04:05 近圆 LEO、单位四元数、零柔性初态与 `srand(1)` 的基础上，保持 `sada_command_flag=1`，并持续请求两轴极端角度 `(1.0,-1.0)`，用于覆盖 SADA 位置饱和／状态转换的有限受控路径。

H5-100 与 H5-1000 原 ELF state、main 与 IPC gold 均双采集逐文件一致。H5-1000 哈希：state `15566885a8a123302f53a8a439cc915d2b793152d54b172ee7202d84f2692ea2`，main `d380041dcd6592337b28fca490b19c886f25f10b78cafbf1ff511ec9a8d0ea84`，IPC `b9c813ef31a0d404a65da150c80eb763642ed7cfc3de7982dd204128e6b1c25a`。

默认严格 C11 恢复端在 H5 100／1000 步中对 caller CoreDynamic、main 与 IPC 逐步比较均为 0-byte mismatch。两条 H5 比较器已接入 Makefile；`make clean && make selftest` 已通过 H0–H5 共十二条 100／1000 步长时域门禁，`make -n selftest` 统计为 **288 个**测试可执行项。

> H5 只覆盖此固定 SADA 饱和角组合和固定初始条件；不证明任意位置、速度、时序、故障模式、并发命令或全输入空间行为。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H6：闰日阴影、替代四元数与全柔性状态的 100／1000 步覆盖

H6 结合 2024-02-29、阴影侧 GCI 位置 `(-6.6e6,2.2e6,1.0e6)`、替代四元数 `(0.5,-0.5,0.5,0.5)`、完整非零 20 维柔性状态、零设备命令和 `srand(12345)`。它与 H2 的闰日全柔性场景不同，额外覆盖了受控阴影环境下的 DSS／姿态／柔性耦合。

H6-100 与 H6-1000 的 state、global-y、main 与 IPC 原 ELF gold 均双采集逐文件一致。H6-1000 哈希：state `f2cab04e4229844dfdd538a3f48e9101f05cedc006a44a53fa4fa130d8ff0615`，global-y `0eeecf135559df4b56010535d5c59daf3a1ed14613032e61e5f757f30c881d87`，main `59be09a989a4328d6619f1e020b1e505eeab4f0e90e5eece89aae1bda8ba73a6`，IPC `10cde4dd3d61e34def6d4b3c87fad3ecdb6bad56db047b268300f5798d662488`。

默认严格 C11 恢复端在 H6 的 100／1000 步中，对 caller CoreDynamic、global-y、main 和 IPC 四类逐步比较全部 0-byte mismatch。两条 H6 比较器已接入 Makefile；`make clean && make selftest` 通过 H0–H6 共十四条 100／1000 步门禁，`make -n selftest` 为 **290 个**测试可执行项。

> H6 是阴影几何与姿态／柔性组合的有限受控证据，不证明任意日照状态、轨道几何、噪声、命令时序、并发或全输入空间行为。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H7：非共线、非零柔性、SADA flag1 的 100／1000 步覆盖

H7 使用 2025-01-02 03:04:05 近圆 LEO、替代四元数、六个分散非零柔性状态、SADA flag1 与固定非共线角请求 `(0.01,-0.01)`，并启用惯量更新标志、`srand(1)`。该场景补充 H4 的多执行机构序列和 H5 的饱和指令，聚焦非共线几何、柔性耦合与持久 SADA flag1 的组合。

H7-100 与 H7-1000 原 ELF state、main 与 IPC gold 均双采集逐文件一致。H7-1000 哈希：state `2af310a51d93524ab57ada3ec321a8761b0cda7d2725a04d77e8740df744f2b0`，main `949fee298b60dfa457024388743e98ad2d6473cdb7acc802150799d17012f949`，IPC `8fd160f129143a78ed454c3dc52731339e57de219c15ea1bc4c723691a1e3a96`。

默认严格 C11 恢复端在 H7 的 100／1000 步中对 caller CoreDynamic、main 与 IPC 逐步比较全部 0-byte mismatch。独立 H7 100／1000 宏分支已添加至共享比较器，保持既有 10／20 步分支不变；两条 H7 比较器已接入 Makefile。`make clean && make selftest` 通过 H0–H7 共十六条 100／1000 步门禁，`make -n selftest` 为 **292 个**测试可执行项。

> H7 仅验证固定非共线几何、固定柔性状态与持久 flag1/惯量更新组合。它不代表任意柔性模态、几何、控制标志序列、故障、并发或全输入空间行为。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H8：双 DSS 高斯噪声关闭的 100／1000 步覆盖
H8 使用 2025-01-02 03:04:05、近圆 LEO（`a=7,000,000 m`、`e=0.01`）、单位四元数、初始本体系角速度 `(0.001,-0.002,0.003)`、零设备命令和 `srand(1)`。它在 `DynamicInit` 后将两路 DSS 的 `gaussian_noise_flag` 均置零，因而是对 H3“两路自定义 DSS Gaussian noise”路径的独立、有限的无高斯噪声配置验证；这里的“both-off”仅指两路 DSS 高斯噪声开关关闭，并非声称 DSS 设备不可用。

H8-100 与 H8-1000 的原 ELF state、main 与 IPC gold 均按同一受控过程连续独立采集两次，逐文件 `cmp` 一致。H8-1000 的哈希分别为：state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`，main `e343f7469589ebcb72b59d36046b7c323502fe91951e3c0543a3be46b77d9586`，IPC `b495c13cf661cd9d7402fbb2e74b4f0d1bb7fc6dacef634bb77381aa9dbfdfe8`。恢复端使用默认严格 C11 标志，对每一步 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B 和 IPC payload／3000 B 比较均为 0-byte mismatch；H8 没有额外采集或比较 global `y[33]`，因此不将该块列入本场景的验证声明。

H8 100／1000 步比较器已接入 Makefile。新增后从空构建运行 `make clean && make selftest` 已通过；按 `make -n selftest` 的完整命令展开当前共有 588 行构建／执行命令，即 294 个测试可执行项。回归前后 `input/DynamicPackage.elf` 的权限均为 `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H8 仅是固定种子、固定时间/轨道/初态、零命令和两路 DSS Gaussian-noise flag 均关闭的有限受控证据。它不能证明任意传感器参数、随机数库、异常值、并发、平台、编译器、命令序列或完整输入空间的行为等价。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H9：单 DSS0 高斯噪声开启的 100／1000 步覆盖
H9 使用 2025-01-02 03:04:05、近圆 LEO（`a=7,000,000 m`、`e=0.01`）、单位四元数、初始本体系角速度 `(0.001,-0.002,0.003)`、零设备命令和 `srand(1)`。在 `DynamicInit` 后设置 `DSS0.gaussian_noise_flag=1`、`DSS1.gaussian_noise_flag=0`。它将 H8 的双关闭状态与 H3 的双路自定义噪声状态分离，覆盖仅 DSS0 消耗既有 Gaussian-noise 随机序列的有限路径；这描述的是噪声开关而非设备可用性判断。

H9-100 与 H9-1000 的原 ELF state、main 与 IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H9-1000 哈希为：state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`，main `c6f62f261eaad6183fa923da64366bfcf585af9cfc3ab8cefee125b2777a0244`，IPC `51b29a6be9305269dcc1461bdf0622de46513e36cc0162fbebf555a93174305c`。默认严格 C11 恢复端对每一步 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B 和 IPC payload／3000 B 均为 0-byte mismatch。H9 未另行采集或比较 global `y[33]`，故不将该块列入 H9 验证声明。

H9 100／1000 步比较器已加入 Makefile。之后从空构建执行 `make clean && make selftest` 已通过；`make -n selftest` 当前为 296 个测试可执行项、592 行编译／执行命令。回归前后 `input/DynamicPackage.elf` 的权限均为 `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H9 仅证明固定时间、轨道、初态、种子、零命令和固定 `DSS0=1`／`DSS1=0` Gaussian-noise flag 组合下的有限受控轨迹。它不证明其他 RNG 实现、噪声参数、传感器标志组合、异常输入、并发、平台、编译器、指令集或任意输入空间的行为等价。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H10：单 DSS1 高斯噪声开启的 100／1000 步覆盖
H10 使用 2025-01-02 03:04:05、近圆 LEO（`a=7,000,000 m`、`e=0.01`）、单位四元数、初始本体系角速度 `(0.001,-0.002,0.003)`、零设备命令和 `srand(1)`。在 `DynamicInit` 后设置 `DSS0.gaussian_noise_flag=0`、`DSS1.gaussian_noise_flag=1`。它是 H9 的互补状态，覆盖仅 DSS1 消耗既有 Gaussian-noise 随机序列的有限路径；与 H8 的双关闭及 H3 的双路自定义噪声共同构成两路高斯噪声开关的四种有限组合，且该描述不代表设备可用性判断。

H10-100 与 H10-1000 原 ELF state、main 与 IPC gold 都连续独立采集两次并逐文件 `cmp` 一致。H10-1000 哈希为：state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`，main `a80b4f83f3534d2b1a111bea7d4cc08b50d50cf3cc934c224d2089beb067fec1`，IPC `9f7e8a4520c38daa151faa7b711a87f4a417c0a2f0aaaf6fcea436afb0c0dc3f`。默认严格 C11 恢复端对每一步 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B 和 IPC payload／3000 B 均为 0-byte mismatch。H10 未另行采集或比较 global `y[33]`，故不将该块列入 H10 验证声明。

H10 100／1000 步比较器已加入 Makefile。之后从空构建执行 `make clean && make selftest` 已通过；`make -n selftest` 当前为 298 个测试可执行项、596 行编译／执行命令。回归前后 `input/DynamicPackage.elf` 的权限均为 `0400`，SHA-256 均为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H10 仅证明固定时间、轨道、初态、种子、零命令和固定 `DSS0=0`／`DSS1=1` Gaussian-noise flag 组合下的有限受控轨迹。它不证明其他 RNG 实现、噪声参数、传感器标志组合、异常输入、并发、平台、编译器、指令集或任意输入空间的行为等价。工程仅限离线研究与仿真，禁止用于飞行、控制、实体执行机构或安全关键用途。

## H11 候选：第三四元数／反相柔性高偏心路径的 DSS 有效标志取证（尚未验收）

为增加与 H0-H10 不同的姿态和柔性初态覆盖，建立了 H11 候选：2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、初始四元数 `(0.5,0.5,0.5,-0.5)`、初始角速度 `(0.0125,-0.00875,0.00425)`、20 维反相柔性初态、零设备命令、`srand(12345)`。原 ELF 的 100 步四块 gold 已独立连续采集两次；其 SHA-256 为 state `9bfe8f2b2cd4145512f79e013d94e0a61140c9ce94c7351da18d075af7b0e53d`、global `y` `244d8be7ed7fed5ab2432c1278233ce75374271e2c6c53bfc7d5891ba2054477`、main `9379892ee7b7bb6f8fc2b9bc6a91e622eb8238586b1cd220b2821e3702af41af`、IPC `9a93cb9530f07426824baf18fd1fc870aa0561851785330a69cf5bddaf27fce6`。该金标仅用于诊断，**H11 尚未通过，未接入 Makefile，也没有建立 H11-1000 验收声明**。

首次严格 C11 的 H11-100 逐步比较表明，caller `CoreDynamic` 前 `0x108`／264 B 与 global `y[33]`／264 B 在全部 100 步均为 0-byte mismatch；main／544 B 和 IPC payload／3000 B 的唯一分叉分别发生于步骤 68、73、85，均为 `DSS_VALID` 的两个 `int32` 字段：main 偏移 `0xc0`、`0xc4` 与 IPC 偏移 `0x968`、`0x969`。恢复端为 `1,1`，原 ELF 为 `0,0`，其余 main／IPC 字节与四个核心状态块一致。

使用仅操作 `analysis/debug_runner/DynamicPackage.exec_copy` 的原 ELF 探针，连续独立双采集了每步 `DSS[2]` 全对象（100×`0x2b0`=68,800 B）；两次完全一致，SHA-256 为 `c5ef7528a8e76d4adeac660b8b7f401d2b7b8052a1b6b679b81179b359db5dc4`。忽略四个内部 backing-pointer 槽的地址重定位后，原／恢复 DSS trace 只有上述三步、两通道的 six-byte `valid_flag` 差异；两通道的阈值、量测、比率和角度均一致。这证实问题限于 `UpdateDSS`→`UpdateSunSensorValidFlag` 的寄存器/调用链可观察语义，而非轨道积分、姿态、柔性、DSS 量测、遥测封包或 IPC 数据载荷的广泛数值偏移。

已反汇编原 `vector2angle`、`UpdateDSS`、`UpdateSunSensorValidFlag` 和 `isEarthShadow`。一个把零向量夹角全局简化为 `0.0` 的假设会使 H11 绝大多数步骤错误地从原 ELF 的 `valid=1` 变为 `valid=0`；一个“第二 DSS 通道 `z>0` 即进入正常地影分支”的放宽假设也新增多步错误。两种没有充分条件证据的泛化均已撤回。工作树保留先前经 H0-H10 受控证据支持的窄规则；H11 仍是待进一步隔离的候选，而非 PASS。

撤回未证实的 H11 泛化后，从空构建运行默认严格 C11 `make clean && make selftest` 已通过；`make -n selftest` 为 298 个测试可执行项（596 行编译／执行命令）。原 `input/DynamicPackage.elf` 仍为权限 `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H11 的可复现差异是后续反汇编和隔离实验的工作项，不是恢复端已验证通过的行为。当前有效的长时域 PASS 集仍为 H0-H10，且各场景的观察块范围以覆盖矩阵中的明确记录为准。所有结论仅适用于固定输入、种子、平台与严格编译旗标下的离线研究／仿真；不得外推为任意输入、并发、平台、数学全域或安全关键用途的 100% 证明，也不得用于飞行、控制或实体执行机构。

H11 进一步的顶层最小隔离实验以步骤 68、73、85 的第二 DSS 归一化量测为输入，分别在原 ELF 调用 `atan2(y,z)` 后调用原 `vector2angle(measure, zero)`；三项输出均为 `+0.0`（24 B SHA-256 `9d908ecfb6b256def8b49a7c504e6c889c4b0e41fe6ce3e01863dd7b61a20aa0`）。这与原 `vector2angle` 的零右范数 `ucomisd`／`jbe` 返回零反汇编路径一致，但顶层调试器的两个独立 `call` 表达式可能改写 caller-saved XMM 寄存器，不能替代 `UpdateDeviceMeasure` 中 `UpdateDSS` 紧邻 `UpdateSunSensorValidFlag` 的真实调用点。因此该结果仅排除了普通零向量公式本身的解释，**不构成扩大或替换现有窄规则的充分依据**。

## H11 闭合：第三四元数／反相柔性高偏心路径的 DSS 有效性调用链

H11 使用 2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、初始四元数 `(0.5,0.5,0.5,-0.5)`、初始角速度 `(0.0125,-0.00875,0.00425)`、20 维反相柔性初态、零设备命令及 `srand(12345)`。该场景的百步四块 gold 已独立连续双采集。初始恢复端只有步骤 68、73、85 的两路 `DSS_VALID` 分叉：core／global `y` 全程一致，main／IPC 的相应有效标志原 ELF 为 `0,0`、恢复端为 `1,1`。

原 `UpdateSunSensorValidFlag` 反汇编显示，它在局部栈槽构造的并非零向量，而是 `(0,0,1)`；该向量随后传给两处 `vector2angle` 调用。为避免 GDB 嵌套 `call dyn_main` 断点限制，只在 `DynamicPackage.exec_copy` 的运行期内将两条 call 临时跳转到短跳板，短跳板先调用原 `vector2angle`，再保存原返回 `xmm0` 并原样返回调用点。该诊断没有修改 `input/DynamicPackage.elf`，并且每次诊断后四块正式 gold 均逐文件未变。

H11 步骤 68、73、85 的两路原 `vector2angle` 返回分别为 `0.9491289345377069`、`1.021994114358845`、`1.0309815219893705`，均低于 `pi/3`。以同样仅驻留调试副本的运行期跳板记录 `isEarthShadow`，三步均返回 `0`。因此两路原有效标志均为 `0`。恢复端已以最小方式将 `dp_update_dss_valid_flag` 改为使用真实基准向量 `(0,0,1)`，删除没有充分原 ELF依据的 legacy-NaN／特殊第二通道规则；H11-100 随即对 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B 全部逐步 bitwise PASS。

H11-1000 的四块原 ELF gold 再次独立连续双采集且逐文件 `cmp` 一致。其 SHA-256 为 state `d7743612eae0fa8e15127fdd94896249927c85c7eb47042cd409bcbd605f306c`、global-y `a1fabb17aca8130de168e05964d03525b0bd3134ba8b70d68f843163be55cc8b`、main `ab484fb2fb6f24fe8d578a20d1f20b943f01dd8cb8c928bc66731559c4b49e6b`、IPC `10dba40b7f4c3ab2dc8e25d8aa3a1e1ecfee07798d5ced9ff64dba43ae362271`。默认严格 C11 恢复端对四块的全部 1000 个步骤均为 0-byte mismatch。

H11 100／1000 比较器已加入 Makefile。从空构建执行 `make clean && make selftest` 后 H0-H11 的门禁均通过；`make -n selftest` 展开为 300 个测试可执行项、600 行编译／执行命令。原 ELF 仍为 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H11 的 PASS 仅说明上述固定输入、种子、平台、严格编译标志及四个已列观察块上的受控有限轨迹逐步 bitwise 一致。它不证明任意日期、传感器参数、异常值、并发、平台、编译器或完整数学输入域的等价性；工程仅限离线研究／仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H12：闰日阴影／全柔性／单 DSS0 高斯噪声联合路径的 100／1000 步覆盖

H12 使用 2024-02-29 12:34:56、`a=7,078,137 m`、`e=0.05`、替代四元数 `(0.5,-0.5,0.5,0.5)`、阴影侧初始 GCI 位置 `(-6,600,000,2,200,000,1,000,000)`、完整非零 20 维柔性状态、零设备命令和 `srand(12345)`。在 `DynamicInit` 后设置 `DSS0.gaussian_noise_flag=1`、`DSS1.gaussian_noise_flag=0`。该场景将 H6 的闰日阴影／全柔性传播与 H9 的单 DSS0 噪声随机序列合并为一个独立的有限受控路径。

H12-100 与 H12-1000 的原 ELF state、global `y`、main、IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H12-1000 哈希为 state `f2cab04e4229844dfdd538a3f48e9101f05cedc006a44a53fa4fa130d8ff0615`、global-y `0eeecf135559df4b56010535d5c59daf3a1ed14613032e61e5f757f30c881d87`、main `27b161b70c55c0d5755c376a5fdeaa6f1b265e286ffd7931b8c79fa27605dd07`、IPC `8cf11eb7705df131b225dae299ba94d148ada7eb2a0971fe7a515ad830211103`。

默认严格 C11 恢复端对 H12 的每一步 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B 均为 0-byte mismatch；100／1000 比较器已经接入 Makefile。从空构建运行 `make clean && make selftest` 后 H0-H12 长时域门禁均通过；`make -n selftest` 展开为 302 个测试可执行项、604 行编译／执行命令。原 ELF保持 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H12 仅证明该固定联合输入、种子、平台、严格编译标志和明确四块观察范围下的有限轨迹逐步 bitwise 一致。它不能外推为任意阴影几何、噪声参数、异常输入、并发、平台、编译器或数学全域的 100% 行为证明；工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H13：替代姿态／完整柔性振幅／持续 SADA 与惯量更新的 100／1000 步覆盖

H13 使用 2025-01-02 03:04:05、`a=7,000,000 m`、`e=0.01`、替代四元数 `(0.5,-0.5,0.5,0.5)`、初始角速度 `(0.001,-0.002,0.003)`、完整交错非零 20 维柔性状态、`srand(1)`，并持续施加 SADA 双轴命令 `(0.01,-0.01)` 与惯量更新标志。该场景在姿态、柔性振幅和持续命令三方面补充了 H7 的稀疏柔性／非共线 SADA 覆盖。

H13-100 与 H13-1000 的原 ELF state、global `y`、main、IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H13-1000 哈希为 state `7f21d30353c78c8a104d8e438f0e6f87013467420a7bea136d5d55c33d562b61`、global-y `11a87a3910ae7a247f05aaa0818ccbe00e510d058d61a100856eeba76f86859b`、main `875d112c51ed017e069ee78547353bf78639f22d94953825975ed431cf6454d2`、IPC `0290d1dc273f3930ae4d110b358b13b318823f531cef41e8f58e54e302417111`。

默认严格 C11 恢复端对 H13 的每一步 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B 均为 0-byte mismatch；100／1000 比较器已经接入 Makefile。从空构建运行 `make clean && make selftest` 后 H0-H13 长时域门禁均通过；`make -n selftest` 展开为 304 个测试可执行项、608 行编译／执行命令。原 ELF保持 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H13 仅证明该固定时间、轨道、姿态、柔性初态、持续命令、种子、平台、严格编译标志和明确四块观察范围下的有限轨迹逐步 bitwise 一致。它不能外推为任意柔性参数、命令序列、异常输入、并发、平台、编译器或数学全域的 100% 行为证明；工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H14：高偏心／替代姿态／完整柔性路径的 100／1000 步覆盖

H14 使用 2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、替代四元数 `(0.5,-0.5,0.5,0.5)`、初始角速度 `(0.0125,-0.00875,0.00425)`、完整非零 20 维柔性初态、零设备命令和 `srand(12345)`。该场景把高偏心跨日动力学环境与替代姿态、全柔性耦合为独立有限轨迹。

H14-100 与 H14-1000 的原 ELF state、global `y`、main、IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H14-1000 哈希为 state `1fa8ec2d30b4fb0ab1ab3041746bfd3491b72936ee36006a6d499a63f4697ead`、global-y `52e096393d9be5bced3d894a82228007dcea21e7dc6e51e091cdc3edf4a47a36`、main `c417b08f0c58fc8f1f8961ab0238a04b26fe09eac5151d02814b333368c9f826`、IPC `387f325a3b458381ab506133e121c9a859c26937769c07347d68f9ad6e630dac`。

默认严格 C11 恢复端对 H14 的每一步 caller `CoreDynamic` 前 `0x108`／264 B、global `y[33]`／264 B、main／544 B、IPC payload／3000 B 均为 0-byte mismatch；100／1000 比较器已经接入 Makefile。从空构建运行 `make clean && make selftest` 后 H0-H14 长时域门禁均通过；`make -n selftest` 展开为 306 个测试可执行项、612 行编译／执行命令。原 ELF保持 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H14 仅证明该固定日期、轨道、姿态、柔性初态、命令、种子、平台、严格编译标志和明确四块观察范围下的有限轨迹逐步 bitwise 一致。它不能外推为任意高偏心参数、姿态、柔性参数、异常输入、并发、平台、编译器或数学全域的 100% 行为证明；工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H15：跨年轨道时间边界路径的 100／1000 步覆盖

H15 使用 2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、第三姿态四元数 `(0.5,0.5,-0.5,0.5)`、初始角速度 `(0.0125,-0.00875,0.00425)`、零柔性状态、零设备命令和 `srand(12345)`。该场景以跨年时间边界为主要独立覆盖维度，并保留高偏心轨道和有限姿态变化。

H15-100 与 H15-1000 的原 ELF state、main、IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H15-1000 哈希为 state `dd04631d8912590010eed11aa5415ee663aa7f73ca438f7b920f5ac5246fa028`、main `828ada1d4872a81636313b903903a29569cf004d0c536266245db89356861d67`、IPC `456ff515ada45c073a33a71fdb8b66d6a672c38165fa0650ba3dbd45ad1b0e23`。

H15 的共享比较器以 `DP_ORBIT_TIME_BOUNDARY_HUNDRED_STEP`／`DP_ORBIT_TIME_BOUNDARY_THOUSAND_STEP` 选择独立长时域 gold 和步数；两个包装器同时定义 `DP_ORBIT_TIME_BOUNDARY`，使初始时间、轨道、姿态和种子仍由原有输入装配分支初始化。默认严格 C11 恢复端对每一步 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B 均为 0-byte mismatch。H15 100／1000 比较器已经接入 Makefile；从空构建运行 `make clean && make selftest` 后 H0-H15 长时域门禁均通过，`make -n selftest` 展开为 308 个测试可执行项、616 行编译／执行命令。原 ELF保持 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H15 的 PASS 仅证明该固定跨年时刻、轨道、姿态、零柔性、命令、种子、平台、严格编译标志及三个已列观察块下的有限轨迹逐步 bitwise 一致。H15 **未建立或核验 global `y` gold**；它不能外推为任意时间／历法边界、轨道／姿态参数、异常输入、并发、平台、编译器或数学全域的 100% 行为证明。工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H16：普通闰日近地轨道路径的 100／1000 步覆盖

H16 使用 2024-02-29 12:34:56、`a=7,078,137 m`、`e=0.05`、第三姿态四元数 `(0.5,0.5,-0.5,0.5)`、初始角速度 `(-0.006,0.011,-0.009)`、初始 GCI 位置 `(6,500,000,-2,200,000,3,500,000)`、速度 `(2,000,6,200,3,000)`、零柔性状态、零设备命令和 `srand(12345)`。它补充了闰日普通近地轨道与第三姿态的有限传播覆盖。

H16-100 与 H16-1000 的原 ELF state、main、IPC gold 均连续独立采集两次并逐文件 `cmp` 一致。H16-1000 哈希为 state `905b8e793ac0016b83e7cd5da0c98361755b897a8231c480ed84133dc863082b`、main `4c781c13dfcbc104792dcc4130fd350b5b108b0c468679d9cfcc5f9ce126050d`、IPC `935c7074e9039d00c84c74197ccf3e02174209bb99fe0576e896cecbb973e0e2`。

共享比较器以 `DP_LEAP_DAY_LEO_HUNDRED_STEP`／`DP_LEAP_DAY_LEO_THOUSAND_STEP` 选择独立长时域 gold 和步数；包装器同时定义 `DP_LEAP_DAY_LEO`，令既有闰日初始时间、轨道、姿态和种子初始化与新门禁同步。默认严格 C11 恢复端对每一步 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B 均为 0-byte mismatch。H16 100／1000 比较器已经接入 Makefile；从空构建运行 `make clean && make selftest` 后 H0-H16 长时域门禁均通过，`make -n selftest` 展开为 310 个测试可执行项、620 行编译／执行命令。原 ELF保持 mode `0400`，SHA-256 仍为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H16 的 PASS 仅证明该固定闰日时刻、近地轨道、姿态、零柔性、命令、种子、平台、严格编译标志及三个已列观察块下的有限轨迹逐步 bitwise 一致。H16 **未建立或核验 global `y` gold**；它不能外推为任意日期、轨道、姿态、异常输入、并发、平台、编译器或数学全域的 100% 行为证明。工程只限离线研究与仿真，禁止用于飞行、控制、实体执行机构或任何安全关键用途。

## H17：单位四元数、零柔性与全程默认设备命令的 100／1000 步覆盖

H17 从已有 `dyn_main_zero_ten_step_probe.gdb` 审计并扩展而来，选择的是 2025-01-02 03:04:05、`a=7,000,000 m`、`e=0.01`、单位四元数、三轴初始角速度 `(0.001,-0.002,0.003)`、初始位置 `x=7,000,000 m`、速度 `vy=7,546 m/s`、零 20 维柔性状态、零值 `DpDeviceControlCommand`、默认传感器参数和 `srand(1)`。与 H1 不同，H17 在完整 100／1000 步中都不施加轮、MTQ、SADA、推力器或惯量更新命令；与 H8-H10 不同，它保持默认传感器配置而不切换 DSS 高斯噪声开关。因此这是默认设备控制路径的有限长时域补充，而不是对任一既有命令时间表的替代。

原 ELF 调试严格只针对 `analysis/debug_runner/DynamicPackage.exec_copy`。H17-100 和 H17-1000 各自连续独立运行两次；每次均采集 caller `CoreDynamic` 前 `0x108`／264 B、main／544 B、IPC payload／3000 B，并逐文件 `cmp` 一致。H17-1000 最终哈希为 state `f34389335c9a98fa581a43974e10d6ee2cd16bd51bcfaf5e956cc87d9415f3aa`、main `32c5602300c873aeffea4ff2b6335d384133ef2e07a5dc4d67f065b0fd4e9e64`、IPC `39ca467a6704401633a7944129e5fa95c7a5d8f4195d188f095ebbfd69969892`。

恢复端比较器 `dyn_main_global_zero_{hundred,thousand}_step_compare.c` 在严格 C11 选项下分别运行，100 步和 1000 步的每一块均为 0-byte mismatch、bitwise PASS；未出现首个分叉或源码修复需求。两个比较器已接入 Makefile。以 `make clean && make selftest` 从空构建执行的完整日志是 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h17_long_horizon_gates.log`：`make -n selftest` 为 312 个测试可执行项（624 行编译／执行命令），`PASS` 行计数 303，错误模式计数 0。原 ELF 仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H17 只比较 state、main 与 IPC 三块，**没有采集、建立或比较 global `y` gold**。它仅在所列固定输入、种子、平台和严格编译条件下为三个观察块提供有限步数逐步 bitwise 证据；不可外推为任意输入、所有传感器或命令组合、并发、平台、编译器、浮点库、数学域或安全关键部署上的 100% 等价。工程仅限离线研究与仿真，禁止飞行、控制、实体执行机构或安全关键用途。

## H18：多执行机构脉冲命令与 global y 的 100／1000 步四块覆盖

H18 先审计了现有 `dyn_main_global_y_multi_actuator_ten_step_probe.gdb` 和恢复端比较器。虽然其名称包含 `global_y`，原十步探针实际上仅转储 state、main、IPC 和终态设备对象，并没有 `&y` gold；因此不能将旧短时结果表述为 global `y` 已验证。本轮在复制的 H18 原 ELF探针中，于每次 `dyn_main` 返回后以与既有四块场景一致的 264 B 范围转储 `&y`，并在恢复端新比较器中逐步读取、比较同一 global `y[33]`。

H18 固定使用 2025-01-02 03:04:05、`a=7,000,000 m`、`e=0.01`、四元数 `(0.5,0.5,-0.5,0.5)`、零柔性、`srand(1)`，且预置 RWheel0 `omega=628.4185307179587`、角动量 `7.981915339118876`。前九次调用依次脉冲轮扭矩、MTQ、推力器开关和惯量更新；之后均为零命令。该时间表与 H1 的组合命令及 H17 的全程零命令不同，补充了多执行机构状态转移后的有限长时域路径。

H18-100 和 H18-1000 的四个逐步流（CoreDynamic caller-state、global `y`、main、IPC）以及七个最终执行机构快照均独立连续采集两次、逐文件 `cmp` 一致。H18-1000 的逐步流 SHA-256 为 state `df72bb03261aff0c9c8fcc90aca2193d4ec386e315b29c473fa840a0ed670059`、global-y `be8a0ffb03f6515e17c53b9b3101522d1bd835defc666fd5926be193977fba47`、main `1c82c5c6eb78c5f0ba0fb7678ff729a047c824da80ac89bec4873286e52e1092`、IPC `38b6fa8c5dbf1d9be92b7ec8e67f775f898f2af87c67d3067138c03f64ec38ab`。

恢复端 `dyn_main_multi_actuator_global_y_{hundred,thousand}_step_compare.c` 在严格 C11 选项下运行；100／1000 的每一步四块均为 0-byte mismatch，RWheel／MTQ／Thruster 与四个推力器向量终态快照同样 bitwise PASS，未出现需要修复的首个分叉。两比较器已进入 Makefile。`make clean && make selftest` 的空构建日志为 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h18_long_horizon_gates.log`；`make -n selftest` 为 314 个测试可执行项（628 行编译／执行命令），`PASS` 行计数 305、错误模式计数 0。原 ELF 仍保持 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H18 是固定输入、固定命令时间表、固定种子、固定平台和严格编译条件下四个列明逐步观察块与终态设备快照的有限 bitwise 验证；不可外推为任意控制参数、命令序列、随机数实现、平台、编译器、并发、异常输入、数学域或安全关键系统中的 100% 等价。工程仅限离线研究与仿真，禁止飞行、控制、实体执行机构或安全关键用途。

## H19：固定种子有效命令时间表的 100／1000 步四块覆盖

为响应组合／随机覆盖需求，本轮先建立 `analysis/coverage_inventory/combinatorial_random_differential_test_plan.md`，明确当前 Linux x86-64、当前运行时库、严格 C11 编译和当前随机数实现是本轮目标平台。H19 是该计划的第一个可审计批次：不使用无界随机浮点输入，而是用固定 32 位 LCG，种子 `0x13579bdf`，递推式 `state = state * 1664525 + 1013904223 (mod 2^32)`，在每一步生成有限、有效的设备命令组合。

LCG 的状态位映射被同时写入原 ELF GDB probe 和恢复端 C 比较器：低位选择一个轮扭矩通道和正负 `0.005`，条件位可选第二轮通道和正负 `0.003`；另一个条件位选择一个 MTQ 通道和正负 `0.008`；SADA 启用时双轴目标来自 `±0.01` 与 `±0.005` 的固定集合；独立状态位控制推力器工作与惯量更新。这保证同一当前平台上原 ELF和恢复端从相同命令流开始。H19 维持 H18 的 2025 近圆 LEO、第三姿态、零柔性、RWheel0 预置与传感器 `srand(1)`。

H19-100 与 H19-1000 的 global `y`、caller-state、main、IPC 四个逐步流，及 RWheel／MTQ／Thruster 和四个推力器三元向量的终态快照，均独立连续采集两次并逐文件 `cmp` 一致。H19-1000 的四个逐步流 SHA-256 是 state `36eda1837c9265dd81d20869b0f444ff9dd5c3fac621b7a282ab117ad4861cc1`、global-y `448fe43cf847b017913ff5c50c843d15c0bf38bd242632faa3ad6d1eb427bb68`、main `f6bf82cd88cd7305b3204d6c7fae7e6136bbdcde8269e89a1b334d2f4897317f`、IPC `57c8d506f38613beb59f20674cb8d7eb4b7d1a232f20401a3f0f231613043739`。

恢复端 H19 百步和千步比较器均报告所有逐步块为 0-byte mismatch，终态设备快照也全部 bitwise PASS，没有出现首个分叉。两个比较器已经接入 Makefile。当前平台上从空构建的完整日志是 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h19_long_horizon_gates.log`；`make -n selftest` 为 316 个测试可执行项（632 行编译／执行命令），`PASS` 行计数 307、错误模式计数 0。原 ELF 仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H19 扩大的是当前平台上一个明确、可重放、有限的固定种子命令序列覆盖。它不是对所有 32 位 seed、所有命令序列、连续浮点空间、异常输入或并发执行的穷尽证明；后续 L1／L2／L3 批次须分别采用独立双采集 gold 和同一四块逐步差分流程。工程仅限离线研究与仿真，禁止飞行、控制、实体执行机构或安全关键用途。

## H20：非零柔性、DSS 开关与固定种子多执行机构命令的交叉覆盖

H20 的候选审计确认三个维度此前均有独立四块取证但未在同一长时域场景交叉：H13 的完整正负 20 维柔性模板，H12 的 DSS0 `gaussian_noise_flag=1`／DSS1=0 和传感器 `srand(12345)`，以及 H19 的 32 位 LCG 有效多执行机构命令时间表。为避免推测连续随机输入，H20 仅组合这些既有、有效、可重放的离散输入：2025 近圆 LEO、第三四元数 `(0.5,0.5,-0.5,0.5)`、H18/H19 的 RWheel0 预置、H13 柔性模板、H12 DSS 配置以及 H19 的 LCG 种子 `0x13579bdf`／递推式与字段映射。

原 ELF探针在 `dyn_init` 后按 H13/H12 已验证偏移写入 y[13..32] 柔性值和 DSS 开关，再使用 H19 命令发生器，并于每次 `dyn_main` 后采集 global `y`、caller-state、main 和 IPC，最后采集 RWheel／MTQ／Thruster 与四个推力器向量。H20-100 与 H20-1000 的四个逐步流和七个终态快照均连续独立采集两次、逐文件 `cmp` 一致。H20-1000 的逐步流 SHA-256 为 state `577f6f398d0fc182513b9c3fbf42e98e87b2c1c93129afa18bdd5314fcbeeb23`、global-y `d8b3b8dcdc389bf9b59e1c76fa4b75f5d95a03863b876d941bcc104b3302d849`、main `24509dd5f88086b705e33a03cfc976383c062f90d317e8366b17911126849417`、IPC `54f31020cbcb8514e0404c3853195eeb8c5ec44146237829fbe2823eceda6803`。

恢复端 H20 百步和千步比较器均报告每一步四块为 0-byte mismatch；RWheel、MTQ、Thruster 和四个推力器向量终态快照亦全部 bitwise PASS。无首个分叉，未改动恢复源码。两个比较器已接入 Makefile。当前平台空构建日志是 `analysis/coverage_inventory/full_clean_selftest_with_h0_to_h20_long_horizon_gates.log`；`make -n selftest` 为 318 个测试可执行项（636 行编译／执行命令），`PASS` 行计数 309、错误模式计数 0。原 ELF 仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> H20 是当前平台上列明离散柔性、传感器、随机种子与命令序列交叉下的有限 bitwise 验证，而不是对所有柔性值、传感器参数、随机数种子、连续命令、异常输入、并发或当前平台全部输入空间的穷尽证明。工程仅限离线研究与仿真，禁止飞行、控制、实体执行机构或安全关键用途。
