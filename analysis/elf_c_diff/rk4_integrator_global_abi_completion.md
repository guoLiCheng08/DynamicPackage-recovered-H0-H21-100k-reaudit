# `RK4_Intergrator` 全局 ABI 恢复完成记录

**阶段状态：** 已完成受控同名全局 ABI 闭环，已通过两步原 ELF 全状态逐位比较；不宣称覆盖所有输入、所有初始化顺序或全部上层生命周期。  
**恢复端工作树：** `/home/ubuntu/dynamicpackage_recovered_work`。  
**原 ELF：** `input/DynamicPackage.elf`，始终只读且未修改。

## 导出 ABI 与全局对象结论

原 ELF `RK4_Intergrator` 位于 `0x5e90`，大小 1195 字节。唯一显式参数是 System V AMD64 `xmm0` 中的 `double step`，因此公共声明已从过时的无参假设修正为：

```c
void RK4_Intergrator(double step);
```

| 原对象 | 原地址 | 恢复端对象 | 已验证用途 |
|---|---:|---|---|
| `y[33]` | `0x2184e0` | `double y[33]` | 全局 RK4 状态读写。 |
| `t` | `0x2184c0` | `double t` | 单步完成后更新一次的积分时间。 |
| `step_time` | `0x215598` | `double step_time` | 可观察的上层步长配置；RK4 本体不读取它，仍使用显式 `step`。 |
| `differential_equation` | `0x6a50` | 已闭环同名接口 | 四个阶段导数调用。 |
| `y_q_unit` | `0x5da0` | wrapper 内的直接除法实现 | 加权状态写回后、`t` 写回前的四元数归一化。 |

## 产物和证据链

| 环节 | 产物 | 结果 |
|---|---|---|
| 导出 ABI 审计 | `analysis/elf_c_diff/rk4_integrator_global_export_audit.txt` | 固化地址、参数、y/t/step_time、33 元循环、四阶段和最终存储顺序。 |
| 原 ELF 采集 | `analysis/golden/rk4_integrator_global_abi/original_capture_seed.gdb` | 隔离调试副本内执行 `DynamicInit`，设置非零刚柔状态与 C/MA/MD 项后，直接调用原全局 RK4 两次。 |
| 可复算金标 | `analysis/golden/rk4_integrator_global_abi/*.bin`、`SHA256SUMS` | 共 21 个二进制文件，包括 pre/step1/step2 的 33 元状态、t、显式 step、step_time、J/H/L、外力、质量、SADA 和 6 类 Sat 矩阵 payload。 |
| 同名实现 | `src/dynamic_satellite_globals.{h,c}`、`src/dynamic_recovered.h` | 导出 `y`、`t`、`step_time` 与 `RK4_Intergrator(double)`；复用已验收的 `dp_rk4_step_33`。 |
| 逐位比较器 | `analysis/rk4_integrator_global_original_gold_compare.c` | 重放独立全局 snapshot；验证两步 33 维 `y`、`t` 位级一致，并验证积分器未改写输入刚柔/外力/SADA/质量/step_time payload。 |
| 持续回归 | `Makefile` | 比较器已加入 `selftest`。 |

## 保持的算术与存储顺序

实现没有将原 RK4 代数重排。它保留 `h*k1`、`h*k2`、`h*k3`、`h*k4` 存储方式与以下逐项顺序：

```text
weighted = h*k4
weighted += h*k3 + h*k3
weighted += h*k2 + h*k2
weighted += h*k1
weighted *= 1/6
y += weighted
normalize y[0..3] by four direct divisions
t = old_t + step
```

为匹配原 `0x62f8..0x6321` 的顺序，wrapper 将阶段内核的时间推进保存在局部 `next_time`；全局 `t` 直到 `y` 完成直接除法归一化后才写回。当前 `differential_equation` 不消费传入阶段时间，但 wrapper 通过已有 RK4 内核保持了原阶段时间 `t`、`t+h/2`、`t+h/2`、`t+h` 的生成顺序。

## 回归结果

已执行：

```sh
make CC=gcc AR=ar selftest
```

日志：`/tmp/dynamicpackage_selftest_global_rk4.log`。完成成功，共出现 **64 个 `PASS` 输出行**。本阶段新增门禁输出为：

```text
RK4_Intergrator global ABI original-ELF gold: PASS (2 steps, y/t bitwise; globals preserved)
```

此前闭环的 differential-equation 全局 ABI 门禁也同时通过。日志行数不是金标数量；一个复合比较器可在一行中报告多个逐位字段，因此不能将 64 当作总功能覆盖率。

## 保守边界与后续工作

此阶段仅证明：在采集到的默认 `Sat(flag=0)`、非零 33 维状态、非零选择性 C/MA/MD、零 H/L/外力/SADA 的受控路径中，两个连续显式 `h=0.1` 全局 RK4 调用的 y/t 输出与原 ELF 按位相同。它不证明异常 NaN/零范数四元数、任意步长、任何替代 Sat 构型、任意全局指针状态或完整 `DynamicInit/CoreDynamic` 生命周期在全输入空间等价。

下一优先模块是 **`CoreDynamic` 同名全局接口**：需要先审计其 0x90 字节输入帧、DynSat/SatTorque/环境对象读写，以及它在调用 RK4 前后的 `TimeAdd(step_time)`、执行机构动量/力矩组装和全局状态同步。以原 ELF `core_dynamic_pre_rk4_probe.gdb` 的 pre/post 边界快照为起点，建立同名入口的最小状态适配器与比较器。
