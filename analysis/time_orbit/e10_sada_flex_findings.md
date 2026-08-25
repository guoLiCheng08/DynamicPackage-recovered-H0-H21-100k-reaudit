# E10：SADA 位置命令刚柔耦合诊断记录

## 已验证的排除项

在 `CoreDynamic+0x4a6`（RK4 调用指令处）截取并比较零命令和 SADA 位置命令第一轮，可确认以下对象逐字节一致：

| 对象 | 验证结果 | 说明 |
|---|---|---|
| 33 维 RK4 前状态 `y` | 一致 | 两个用例从同一初态积分。 |
| `Sat` 完整 4216 字节对象 | 一致 | `Update_sat_inertia_xw(flag=0)` 对两用例产生相同 Sat 描述符状态。 |
| Sat 指向的惯量、逆惯量、5 个 3×3 矩阵、3×10 矩阵、耦合矩阵、两 10×10 模态矩阵 | 一致 | `gold_sada_position_step{1,2}_model.bin` 与恢复版默认模型逐字节一致，且两轮间相同。 |
| `SatTorque` 原始快照 | 一致 | 不是首轮差异来源。 |
| `J_c_B_mem`、`J_c_B_inv`、`H_w_B`、`L_c_B`、`J_c_B` 描述符及其指向的数据 | 一致 | 不是 SADA 命令首轮差异来源。 |

因此，`Update_sat_inertia_xw` 的 flag=0 路径对该位置命令首轮并不需要角度相关重构；此前把差异归因于 Sat 矩阵的假设已被排除。

## 新的根因证据

原 ELF `dynamics_flex`（`0xe920`）在函数入口直接读取：

- `SADA+0x08`、`SADA+0x10`：两个 SADA 命令输入；
- `SADA+0x38`、`SADA+0x40`：两个 SADA 角加速度输入。

这些量被构造成两个嵌入三元向量，之后参与 `blas_gemv`、`blas_gemm`、`matrix_sub`、`inv_CAL_M3`、`vector3_cross` 与模态投影链。恢复版 `dp_flex_dynamics_step` 当前未持有 SADA 状态，也未包含此分支，是 SADA CoreDynamic 首轮状态差异的根因。

SADA 位置命令金标第一轮设备状态为：

| 字段 | 值 |
|---|---:|
| `command_angle[0]` | `+0.01` |
| `command_angle[1]` | `-0.01` |
| `current_angle[0]` | `3.1415926553351223` |
| `current_angle[1]` | `3.1415926527171285` |
| `angular_velocity[0]` | `1.7453292519943297e-07` |
| `angular_velocity[1]` | `-8.726646259971648e-08` |
| `angular_acceleration[0]` | `1.7453292519943296e-05` |
| `angular_acceleration[1]` | `-8.726646259971648e-06` |

## 原 `dynamics_flex` ABI 观察

在 `differential_equation+0x428` 的首次 RK4 子步调用处，原函数寄存器参数布局确认包含：刚体角加速度输出三元向量、机体角速度三元向量、三个 10 维模态向量，以及 4 个栈传参的 `L_c_B`、`H_w_B`、`J_c_B`、`J_c_B_inv` 对象。首三个输出/输入的存储位置已由子步动态快照复核。

四个原 ELF RK4 子步已保存至：

- `gold_sada_position_step1_flex_stage1.bin`
- `gold_sada_position_step1_flex_stage2.bin`
- `gold_sada_position_step1_flex_stage3.bin`
- `gold_sada_position_step1_flex_stage4.bin`

每个文件布局为 `alpha[3] || eta_ddot[10] || eta_dot_out[10]`。第一子步原刚体角加速度为：

```text
[ 3.576164948087543e-05,
 -3.0741041477828937e-06,
 -5.131724416686019e-06 ]
```

恢复版（尚不含 SADA 分支）同一初态基线为：

```text
[ 3.2778217049780996e-06,
  3.6865021721320597e-06,
  1.3494114480426792e-06 ]
```

## 后续实现方向

应扩展 `DpFlexDynamicsConfig` 或 Core 模型上下文，使其显式持有 `DpSadaRecovered` 的命令角与角加速度。随后按 `dynamics_flex` 的矩阵调用顺序恢复两组三元 SADA 输入对刚体 RHS、有效惯量求解、模态加速度投影的附加项；以四个子步输出金标逐项校准，最后再接入 `dp_dyn_main_recovered_step`。

不可将 SADA 路径通过预录状态或静态输出替代；验收仍应是基于实时设备状态的原算术路径等价实现。

## 新增中间量快照

为继续逐指令还原，已在原 ELF `dynamics_flex+0x409`（地址 `0xed29`，首个 RK4 子步）捕获完整局部栈：

- `gold_sada_position_step1_flex_mid_ed29_stack.bin`（2304 字节）

该截点在以下链完成之后：`Sat+0x4a8` 对某个 SADA 三元输入的 `blas_gemv`、`Sat+0x9a0` 的第二次 `blas_gemv`、`Sat+0x898` 的耦合投影；在四个三元项求和并进行 `vector3_cross` 前。栈中已确认的显式 SADA 局部向量为：

| 栈偏移 | 内容 |
|---|---|
| `0x260..0x278` | `[0, command_angle[0], command_angle[1]] = [0, +0.01, -0.01]` |
| `0x280..0x298` | `[0, angular_acceleration[0], angular_acceleration[1]] = [0, +1.7453292519943296e-05, -8.726646259971648e-06]` |
| `0x2a0..0x2b8` | 三元投影局部输出（截点前已生成） |
| `0x2c0..0x2d8` | 三元投影局部输出（截点前已生成） |
| `0x2e0..0x2f8` | 命令角嵌入三元向量拷贝 `[0,+0.01,-0.01]` |
| `0x300..0x318` | `Sat+0x4a8` 作用后的三元结果 `[-0.24549805508730987,-0.24163603153116672,16.14867083934111]` |
| `0x320..0x338` | 第二个三元投影局部输出 |
| `0x380..0x398` | 由后续刚体/耦合矩阵链得到的三元结果 |

这说明 SADA 两个标量在原函数中均先被嵌入为三元向量 `[0,s0,s1]`，而非直接作为 `[s0,s1,0]`；后续恢复必须严格保留这一布局。

## 当前代码状态

已将 SADA 数据通路接入恢复版的可重入模型，但尚未将上述矩阵方程写入：

- `DpFlexSadaDrive` 保存 `command_angle[2]` 与 `angular_acceleration[2]`；
- `DpCoreDefaultModel` 持有该快照；
- 主循环在 `UpdateDeviceControl` 之后、RK4 之前复制当前 `DpSadaRecovered` 的 `+0x08/+0x10/+0x38/+0x40`；
- 现有零命令、RW 正向、MTQ 正 X 三个端到端比较器仍全部通过。

因此，后续唯一尚未完成的功能点是以原 `dynamics_flex` 调用顺序实现该快照对应的刚体 RHS 与模态加速度附加项，再运行 SADA 端到端金标。
