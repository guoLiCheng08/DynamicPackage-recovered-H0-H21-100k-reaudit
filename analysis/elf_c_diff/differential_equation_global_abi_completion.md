# `differential_equation` 全局 ABI 恢复完成记录

**阶段状态：** 已完成受控全局 ABI 闭环；尚未宣称覆盖所有输入或全部原始全局生命周期。  
**恢复端工作树：** `/home/ubuntu/dynamicpackage_recovered_work`。  
**原样本：** `input/DynamicPackage.elf`，保持只读、未被修改。  
**严格构建：** `-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off`，以及 `-D_XOPEN_SOURCE=700`。

## 完成的证据链

| 环节 | 产物 | 结果 |
|---|---|---|
| 汇编 ABI 审计 | `analysis/elf_c_diff/differential_equation_global_export_audit.txt` | 固化入口 `rdi=dydt[33]`、`rsi=y[33]`、调用顺序、33 维状态分块、全局 descriptor 和 Sat/SADA 依赖边界。 |
| 原 ELF 采集 | `analysis/golden/differential_equation_global_abi/original_capture_seed.gdb` | 在隔离可执行调试副本上调用 `DynamicInit` 后，以非零刚体角速度、20 个非零柔性状态及非零 C/MA/MD 项采集。 |
| 可复算金标 | `analysis/golden/differential_equation_global_abi/*.bin`、`SHA256SUMS` | 共 20 个 payload 文件：state、derivative、J/H/L、外力、质量、SADA、6 类已验证 Sat 矩阵。 |
| 同名 C11 接口 | `src/dynamic_satellite_globals.{h,c}` | 实现 `void differential_equation(double dydt[33], const double y[33])`；通过全局 descriptor、Sat 和 SADA 组装已验收的 `dp_differential_equation_33` 上下文。 |
| 逐位比较 | `analysis/differential_equation_global_original_gold_compare.c` | 重放原 ELF payload；33/33 个导数 double 位级一致，并验证输入 `y[33]` 未被修改。 |
| 回归门禁 | `Makefile` | 新比较器已纳入 `selftest`。 |

## 新导出与初始化边界

恢复端新增并导出的全局 ABI 对象包括 `J_c_B_mem`、`J_c_B_inv_mem`、`H_w_B_mem`、`L_c_B_mem`、`J_c_B`、`J_c_B_inv`、`H_w_B`、`L_c_B`、`Sat` 与 `SADA`。`SpacecraftMass` 和 `F_I_external` 继续使用已有轨道模块的导出定义，避免产生会偏离 `orbit_dynamic` 读取路径的平行全局副本。

`dp_differential_equation_global_reset()` 是恢复端的**显式测试/初始化辅助入口**，用于装入已验收的 `Sat(flag=0)` 默认矩阵构型、恢复全局 descriptor 形状并清零 H/L、外力和 SADA。它不是原 `differential_equation` 的 ABI 参数，也不改变其原始 `void` 两指针入口。

## 本阶段严格回归

已执行：

```sh
make CC=gcc AR=ar selftest
```

结果为成功，日志 `/tmp/dynamicpackage_selftest_global_differential_run5.log` 中共计 **63 个 PASS 输出行**；其中新增门禁输出为：

```text
differential_equation global ABI original-ELF gold: PASS (33/33 bitwise; input preserved)
```

原交接叙述中的“153 项”是比较器/金标基线数量口径，而当前 Makefile 将若干复合比较器输出为一行，因此本次运行日志的 `PASS` 字符串行数为 63。不要把二者混作失败或覆盖率百分比。

## 顺带修复的既有回归测试初始化缺陷

`DpFlexDynamicsConfig` 后续已扩展了 SADA 可选字段，而三个较早的金标比较器仅设置了旧字段。严格重编译后，未初始化栈指针可能被误判为 SADA 结构，导致编译错误或段错误。为保持原有的**零 SADA 驱动**合同，已作下列最小且不改变数值输入的修复：

| 文件 | 修复 |
|---|---|
| `analysis/time_orbit/flex_modal_real_model_compare.c` | 补齐 `dp_flex_modal_acceleration` 的 3 个 `NULL` SADA 参数；将 `DpFlexDynamicsConfig` 改为 `{0}` 初始化。 |
| `analysis/time_orbit/differential_equation_33_gold_compare.c` | 将 `DpDynamicsContext` 改为 `{0}` 初始化。 |
| `analysis/time_orbit/rk4_fullstate_nonzero_c_k_gold_compare.c` | 将 `DpDynamicsContext` 改为 `{0}` 初始化。 |

以上修复后，这三个既有 bitwise 金标以及整个 `selftest` 均通过。

## 保守结论与后续优先级

本阶段证明的是**所采集的原 ELF 一般非零默认构型快照**与既有回归路径的行为等价，不是所有任意全局状态、无效指针、未初始化调用、替代 `Sat` 配置或完整生命周期的全空间证明。`J_c_B_inv` 已作为可观察全局 descriptor 恢复、采集并在 wrapper 中读取；但当前经过验证的可重入数值核仍从 `J_c_B` 与 C 显式构造有效惯量并求逆。若后续原路径显示 `J_c_B_inv` 对非默认构型输出具有独立影响，应追加多构型金标并把该依赖直接纳入核心算术图。

下一优先级保持为 `RK4_Intergrator` 同名全局接口：审计 `y[33]`、`t`、`step_time`、导数回调与四元数归一化后的全局读写/时间顺序，并建立原 ELF 单步及多步状态金标比较器。
