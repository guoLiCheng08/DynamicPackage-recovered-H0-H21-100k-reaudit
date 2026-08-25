# CoreDynamic 内部 RK4 阶段追踪金标

探针：`analysis/golden/core_dynamic_global_abi/original_capture_controlled_rk4_stages.gdb`。路径为真实 `dyn_main → CoreDynamic`，受控执行机构命令集与 `controlled_input/` 金标一致。日志：`analysis/golden/core_dynamic_global_abi/controlled_input/rk4_stages.log`。

原 ELF 在一次 CoreDynamic 内部 RK4 调用中触发恰好四次 `differential_equation`。经修正换行后的原始阶段日志 SHA-256 为 `574a6151f6635c59f59e778d05ede3416c161df715e11b986b0eea86851a12e3`，包含四条可逐行解析记录。四个入口阶段均观测到相同的全局 handoff：

```text
H_w_B = 3f27f68a844b343c, 3f4a037226720304, bf3704680435c0a8
L_c_B = bf5deab8ee90a0ec, bf804227580741e2, 3f6cc582054330d2
F_I_external = 0000000000000000, bf2d8b2b41cd29ea, 0000000000000000
```

这与已保存的 post payload 一致，证明剩余状态差异并非 CoreDynamic 在 RK4 阶段之间改写 H/L/F 所致。阶段状态最小锚点如下；完整位模式见日志：

| 阶段 | 四元数 q0 位模式 | 位置 x 位模式 | 柔性 `y[13]` 位模式 |
|---:|---|---|---|
| 0 | `3ff0000000000000` | `4150b363c23f647e` | `0000000000000000` |
| 1 | `3ff0000000000000` | `4150b31ab85f8361` | `bf307022af8a405d` |
| 2 | `3feffffffb4737f5` | `4150b31ab7891647` | `bf2faf0c7cbaade2` |
| 3 | `3feffffff68233b1` | `4150b2d1acd2cf79` | `bf3fa9fc47cc444c` |

下一步应让恢复端诊断候选在完全相同的 pre y、H/L/F、Sat descriptor payload 和 SADA 原始对象下导出对应四个阶段输入；第一个不一致阶段及其子块（四元数、刚体角速度、轨道或柔性状态）将成为继续恢复的定位边界。此金标不代表 CoreDynamic 同名接口已验收。

## 恢复端下一步比较合同

恢复端阶段比较必须以 `controlled_input` 的 `pre_y_33.bin`、`pre_t.bin`、
`step_time.bin`、J/H/L/F、SADA raw、六类 Sat 刚柔矩阵 payload 和已记录的 Core
运行时 descriptor payload 重建同一上下文。对每一阶段，应按原日志顺序比较：

1. `y[0..12]` 的四元数、角速度、位置与速度位模式；
2. 至少 `y[13]` 与 `y[32]` 两个柔性锚点；
3. H/L/F 的全部三元向量位模式；
4. 若第 0 阶段已不同，优先抓取 `differential_equation` 的 33 元导数并逐块比较；若第 0 阶段相同但后续不同，审计 RK4 的加权与临时状态形成顺序。

在第一处差异未归因到具体 `Sat`/SADA/环境对象前，不得重新启用或导出候选
`CoreDynamic`。

## 恢复端首差异定位结果

`analysis/core_dynamic_rk4_stage_candidate.c` 已按相同 pre y、post H/L/F、J、Sat
刚柔矩阵和 SADA raw 重放全局微分方程，并以 `dp_rk4_step_33` 记录四个阶段输入。
结果显示第 0 阶段的全部已跟踪状态字完全一致；第 1 阶段的四元数、位置和速度锚点
仍与原 ELF **逐位一致**，但刚体角速度与柔性状态首次不一致。例如阶段 1：

```text
original w_x = 3f5131b011725686    candidate w_x = 3f512fc51526c0cd
original y[13] = bf307022af8a405d  candidate y[13] = bf30126c29ed1fbd
```

因此 RK4 组装、轨道子系统、H/L/F handoff 和四元数微分在该受控路径上不是首要问题；
首个不一致边界位于 `dynamics_flex` 的刚体/柔性耦合计算。下一轮应针对原 ELF
`dynamics_flex` 调用的实际 Sat descriptor（尤其 SADA、有效惯量、刚柔耦合和模态
矩阵）与恢复端 `DpDynamicsContext` 逐字段比较，而不是调整 RK4 权重或输出包装。

## Sat+0x448 SADA reaction matrix 运行时映射

受控原 ELF 快照新增 `sat_m448_data.bin`，其实际 3×3 payload 为

```text
[[+0.7071067811865476, +0.7071067811865475, 0],
 [-0.7071067811865475, +0.7071067811865476, 0],
 [0, 0, +1]]
```

这与恢复端此前作为 SADA 角加速度反作用项使用的静态常量不同。全局
`differential_equation` 已修正为从 `Sat+0x448` descriptor 读取该矩阵；默认
Sat 同步也保存此 descriptor。受控阶段诊断重放真实 m448 后，阶段 0 仍一致，
阶段 1 的刚体/柔性差异仍存在但数值发生变化，说明 m448 是真实运行时依赖，且不是
剩余首差异的唯一来源。修改后 `make selftest` 的 64 个 PASS 输出行、
`differential_equation` 33/33 全局导数金标及 `RK4_Intergrator` 两步全局金标均继续通过。

## 双 SADA 命令投影修正

原 `dynamics_flex` 的 `0xec41..0xeccf` 反汇编直接证实 SADA 命令向量
`[0,command_angle[0],command_angle[1]]` 先经 `Sat+0x4a8`，再经 `Sat+0x9a0`
投影后才加入陀螺叉乘前的总角动量。恢复端此前错误地将输入直接送入 `Sat+0x9a0`；
现已按该顺序改正 `dp_differential_equation_33`。

在当前受控命令的阶段诊断中，已跟踪的阶段状态未因这项改正进一步收敛，说明该双投影
在此特定 payload 下的最终作用与此前路径相同或被更早的未恢复项主导。该事实不否定
汇编明确的调用顺序，修正保留在生产数值内核中。修改后完整严格自检仍产生 64 个 PASS
输出行，且两项已验收全局 ABI 金标继续通过。

## 四阶段导数级首差异

阶段探针现已在原 `differential_equation` 所有导数写回后记录 `alpha[3]` 与
`eta_ddot[0]`、`eta_ddot[9]`。恢复端以相同 pre y/H/L/F/SADA/Sat payload 调用
全局导数得到的第 0 阶段即不一致：

```text
original alpha = 3f5033abdceec271,bf2d5ca02af4f10d,3eb8d18f671286d8
candidate alpha = 3f500d5027071009,bf2b09f637bfc99e,bebe6f4381786fda
original eta_ddot[0],eta_ddot[9] = bf748c2b5b6cd074,3f11f2687ad58807
candidate eta_ddot[0],eta_ddot[9] = bf741707346867ac,3f118f96f07b14da
```

因此阶段 1 状态差异已进一步收敛到第 0 阶段 `dynamics_flex` RHS：不是 RK4
中间状态形成、轨道微分、H/L/F 写入或四元数微分问题。后续应以该第 0 阶段为唯一
定位点，逐项比较原 ELF 由 `Sat+0x4a8/+0x9a0/+0x688/C/MA/MD` 和 SADA command/
acceleration 形成的刚体 RHS 三元项，直至上述五个导数位模式一致。

## `L_c_B` 符号与总角动量加法顺序校准

原 `dynamics_flex` 第 0 阶段断点证明叉乘后的本地 RHS 初值恰为
`L_c_B`，随后扣除 `omega×H_total`；恢复端通用函数以 `-minus_term` 形式表达该
槽位，因此全局适配器必须传入 `-L_c_B`。修正后第 0 子步的三维 `alpha` 与完整十维
`eta_ddot` 均逐位一致。

进一步，原函数在 `0xebbd..0xedf7` 中以 `Jω → H_w → SADA command projection → Cη`
的顺序进行浮点累加。恢复端按相同顺序重排后，受控四阶段中阶段 0、1、2 的已记录
刚体与十维柔性导数均逐位一致，所有已记录的阶段状态锚点一致。独立启用候选
`CoreDynamic` 的完整 0x148 输出对象、H/L/F 已达到逐位一致；33 维最终状态只剩
`y[18]` 一项相差 1 ULP：

```text
candidate y[18] = be522f159035fef3
original  y[18] = be522f159035fef2
```

这很可能来自阶段 3 SADA 柔性模态投影或其临时向量加法的舍入顺序。候选仍只允许
通过 `DP_DIAGNOSTIC_CORE_CANDIDATE` 条件编译，不得提前导出到生产静态库。

## SADA 模态支路寄存器观察（未定稿）

对 `dynamics_flex+0x9df/+0xa24` 的初步断点观察在四个子步中记录到前一对象等于
当步 `alpha`，后一对象稳定为 `[0, SADA.angular_acceleration[0],
SADA.angular_acceleration[1]]`。这与静态反汇编中的局部 descriptor 复用相符，但
尚不足以将对应寄存器直接命名为最终 `Sat+0x4a8` 投影的输入或输出。因此该观察仅
作为下一轮更精细栈/descriptor 跟踪的起点；不据此修改已接近收敛的生产刚柔计算。

## RK4 最终加权缓冲区观察（待寄存器映射）

在 `RK4_Intergrator+0x3ef`（最终 1/6 缩放前）对 `y[18]` 对应槽位捕获到：

```text
r13=3e00f4b92ef2f99a  r14=be525881900c376f  r15=be521e5862244b39
rdx=be625a39a32d4612  rbx=be7b46a05850fe6b  rbp=0000000000000000
r12=0000000000000000
```

这些值已证明最终写回前存在多组独立 RK4 工作缓冲区；其确切寄存器到 `h*k1..h*k4`
及 trial/state-copy 的映射仍需在后续按函数全流程建立。此证据不得被直接用于末位补偿，
而应只用于复原原始权重缓冲算术顺序。

## y[18] 末位差异的 h*k4 归因

原 RK4 最终缓冲断点与恢复端按同一权重顺序重建的 `y[18]` 四个 h*k 项比较如下：

```text
h*k1  original/candidate = 3e00f4b92ef2f99a  (一致)
h*k2  original/candidate = be525881900c376f  (一致)
h*k3  original/candidate = be521e5862244b39  (一致)
h*k4  original = be625a39a32d4612
h*k4  candidate = be625a39a32d4618
```

因此最终 weighted buffer 的差异（原 `be7b46a05850fe6b`，恢复端
`be7b46a05850fe6d`）不是 RK4 权重结合顺序的根因，而是第 4 子步自身的
`dydt[18]`。该分量对应阶段 3 的第六个 `eta_ddot`，原为
`be96f0c80bf89796`，恢复端为 `be96f0c80bf8979e`；该子步还观测到
`alpha_z` 的 2 ULP 差异。后续应在阶段 3、状态已严格一致的条件下审计
`dynamics_flex` 的刚体求解与 SADA/模态投影中间量，不应修改通用 RK4 加权实现。

恢复端诊断钩子已在 `dp_flex_dynamics_step` 前记录四阶段 `H_total`，与原 ELF
`dynamics_flex` 的 `vector3_cross` 输入日志逐项逐位一致：

```text
c01532007163d307,c01b063793fc688a,407eab5ac153a877
c01500dcc2905e3b,c01aeca9ea72e0dd,407eab5c24741e68
c0150093bc585b88,c01aeb6dc6eae591,407eab5c32a55a22
c014cf38679da4dd,c01ad0ad0f535799,407eab5da2875488
```

因此当前第 4 子步 `alpha_z`/`dydt[18]` 的末位差异不来自 `Jω/H_w/SADA/Cη`
的总动量组装；定位范围进一步收敛到 `dp_flex_dynamics_step` 内部的 SADA 角加速度
反作用、有效惯量解算或模态投影临时项的舍入顺序。
