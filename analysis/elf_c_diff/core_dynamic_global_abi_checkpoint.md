# CoreDynamic 全局 ABI 检查点

## 当前结论

`CoreDynamic(void *output_0x148, const void *input_0x90)` 已由诊断候选提升为恢复库的无条件同名导出。该结论仅适用于已记录的三组原 ELF 隔离快照；它不是对未采样输入空间或原系统全部功能的等价性声明。

| 金标路径 | 覆盖场景 | 已逐字节比较的观测对象 | 结果 |
|---|---|---|---|
| `analysis/golden/core_dynamic_global_abi/` | 零命令真实路径 | `y[33]`、`H_w_B[3]`、`L_c_B[3]`、`F_I_external[3]`、0x148 输出对象 | 通过 |
| `analysis/golden/core_dynamic_global_abi/controlled_input/` | 受控执行机构命令 | 同上 | 通过 |
| `analysis/golden/core_dynamic_global_abi/controlled_attitude/` | 非共线姿态下的受控命令 | 同上 | 通过 |

## ABI 与路径证据

| 项目 | 已审计结论 |
|---|---|
| 调用 ABI | `rdi` 指向至少 0x148 字节输出对象，`rsi` 指向 0x90 字节 Core 输入帧。 |
| 主调用顺序 | `TimeAdd(step_time)` → `TimeArrayGet` → 力矩/外力装配 → `H_w_B` 与 `L_c_B` 写入 → `RK4_Intergrator(step_time)` → 环境、状态和时间输出字段。 |
| 输入 `+0x18` | 经 wheel-group handoff 逐位复制至 `H_w_B`。 |
| 输入 `+0x48` | 由 body-to-inertial 矩阵转换后写入 `F_I_external`；非共线姿态快照验证了姿态依赖。 |
| 输入 `+0x78` | 写入输出对象 `+0x130..+0x140`，并与上游已更新的 SADA 全局状态协同使用。 |
| 重力梯度 | 当前三组已采样构型的 `Sat+0x1a8` 对应第三矩阵为零；恢复端没有以 `J_c_B` 代替该运行时 descriptor。 |

## RK4 末位差异闭环

此前，受控命令和非共线姿态快照均仅残留 `y[18]` 的 1 ULP 差异：恢复端为 `be522f159035fef3`，原 ELF 为 `be522f159035fef2`。原 ELF 第四个 RK4 子步的最终刚体 RHS 也仅在 z 分量不同：原值 `3f7eaf925a76aa92`，恢复端旧值 `3f7eaf925a76aa94`。

对 `dynamics_flex` 的断点与反汇编审计确认，原 ELF 在 `inv_CAL_M3` 前按每个标量执行如下写回顺序：

> 基础刚体 RHS → 扣除 SADA 角加速度反作用 → 加入 `0.1·C·M_A·eta` → 加入 `C·M_D·eta_dot`。

旧恢复端将 SADA 反作用放在两项模态尾项之后，因此计算了 `(base + MA + MD) - SADA`；原 ELF 实际计算的是 `((base - SADA) + MA) + MD`。两种表达式在前三个子步恰好舍入相同，但在第四子步 z 分量相差 2 ULP，进而使 `eta_ddot[5]`、`h*k4[18]` 和最终 `y[18]` 发生末位偏差。

恢复端现将 `dp_flex_compose_rigid_rhs_coupled` 分解为基础刚体项与模态尾项辅助例程，并在 `dp_flex_dynamics_step` 中按原 ELF 写回顺序插入 SADA 扣除。受控命令金标中的四个最终 RHS 均逐位一致；受控 RK4 的四个阶段状态记录和四个导数记录中，候选探针覆盖的所有字段均逐位一致。最终 `y[18]` 为 `be522f159035fef2`。

## 生产导出与回归门禁

`src/dynamic_satellite_globals.c` 不再以 `DP_DIAGNOSTIC_CORE_CANDIDATE` 隔离 `CoreDynamic`；`src/dynamic_satellite_globals.h` 已声明该原型。`analysis/core_dynamic_global_diagnostic_compare.c` 已在任一字节差异时返回失败状态，且 Makefile 将同一比较器分别编译为以下三个固定金标目标：

| 目标 | 金标 |
|---|---|
| `core_dynamic_global_zero_gold_compare` | 零命令真实路径 |
| `core_dynamic_global_controlled_gold_compare` | 受控执行机构命令 |
| `core_dynamic_global_attitude_gold_compare` | 非共线姿态受控命令 |

每个目标均比较 264 字节状态、24 字节 `H_w_B`、24 字节 `L_c_B`、24 字节 `F_I_external` 和 328 字节输出对象。生产静态库的全局符号表已包含 `CoreDynamic`、`RK4_Intergrator` 与 `differential_equation`。

## 验证边界与下一步

三组快照通过证明了这些精确输入及其可观察边界上的位级一致性，但不能外推为任意 Sat descriptor、任意控制命令、任意多步持续运行或未恢复顶层入口的完全等价。后续工作应按既定证据链为 `DynamicInit`、`UpdateDeviceControl`、`UpdateDeviceMeasure` 和 `UpdateMainOut` 的同名接口分别执行反汇编审计、隔离原 ELF 金标、最小 C11 实现、逐位比较和回归纳入。

原 ELF 样本继续保持为只读输入；所有工作限于离线仿真与行为研究，不得用于飞行、控制、实物执行机构驱动或其他安全关键用途。
