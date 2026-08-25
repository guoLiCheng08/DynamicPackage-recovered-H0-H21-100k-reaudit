# CoreDynamic 输入帧到全局动力学项：动态手off解码

本记录仅基于原 ELF 的两组真实 `dyn_main → CoreDynamic` 运行时快照。所有地址、状态和 payload 都来自工作副本中的只读调试样本；恢复端尚未据此发布 `CoreDynamic` 同名实现。

## 金标集

| 组别 | 目录 | 输入条件 | 用途 |
|---|---|---|---|
| 基线 | `analysis/golden/core_dynamic_global_abi/` | 零设备命令 | 验证零输入下的全局项、状态与输出。 |
| 受控执行机构 | `analysis/golden/core_dynamic_global_abi/controlled_input/` | 非零飞轮、MTQ、SADA、推进器命令 | 观察 0x90 输入帧、SatTorque backing store、H/L/F 及 RK4 输出变化。 |

两组各包含 28 个二进制 payload 文件和 `SHA256SUMS`。受控组与基线的 `pre_input_90.bin`、`post_y_33.bin`、`post_output_148.bin` 与 `post_sat_torque_168.bin` 均具有不同 SHA-256。

## 可验证的输入帧分组

`DpCoreDynInput` 由六个连续的 3-double 向量组成。受控快照的实测值如下：

| 输入偏移 | 受控帧实测值 | 已观测的 CoreDynamic 结果 | 当前结论 |
|---:|---|---|---|
| `+0x00` | `(0.001828235710007747, 0.007938678144371051, -0.0035121478523248784)` | `SatTorque` backing `+0x080` 为其逐分量相反数。 | 传入 `RWheelTorque` 的群组轮力矩；该项以负号加入总力矩。 |
| `+0x18` | `(0.0001828235710007747, 0.0007938678144371052, -0.00035121478523248784)` | Core 返回 `H_w_B` 逐位等于该值。 | 飞轮群组角动量，直接写入 `H_w_B`。 |
| `+0x30` | `(-0.001, 0.003, 0)` | 进入 `MagTorque` 调用；受控总力矩受其影响。 | MTQ 磁矩命令；精确磁场输入来自步骤前环境对象。 |
| `+0x48` | `(0, -0.0002254, 0)` | 返回 `F_I_external` 与该值逐位一致。 | 推进器/外力向量；该金标起始姿态为单位四元数，尚需非单位姿态金标验证完整坐标变换。 |
| `+0x60` | `(0.000002254, 0, -0)` | 对应可见的非零附加 SatTorque backing 子项。 | 推进器力矩向量，复制进入 SatTorque 子向量后参与 `UpdateTorque`。 |
| `+0x78` | `(π+偏移, π+偏移, 0)` | 直接写入输出对象 `+0x130/+0x138/+0x140`。 | SADA 角/速率输出尾字段；前两项还参与已审计 SADA 柔性驱动。 |

## 全局导数入口的明确手off

在相同受控快照中，CoreDynamic 返回后这三个 `differential_equation` 依赖全局向量为：

```text
H_w_B        = ( 0.0001828235710007747,
                 0.0007938678144371052,
                -0.00035121478523248784 )
L_c_B        = (-0.001825981710007747,
                -0.007938678144371051,
                 0.0035121478523248784 )
F_I_external = ( 0.0, -0.0002254, 0.0 )
```

`L_c_B` 与同一 post snapshot 中 `SatTorque` 总向量 backing store `+0x128` **逐位相同**。`H_w_B` 与输入帧 `+0x18` **逐位相同**。因此同名适配器的顺序边界应为：先由输入帧构建/更新 SatTorque backing vectors，调用 `UpdateTorque`，将总向量写入 `L_c_B`，将 `+0x18` 写入 `H_w_B`，更新 `F_I_external`，最后调用全局 `RK4_Intergrator(step_time)`。

## 已知不完整点

受控金标已经反驳了任何零输入 CoreDynamic wrapper 的充分性，但尚未覆盖所有外力方向、非单位初始姿态下 `+0x48` 的坐标变换、任意 MTQ 环境场、所有推进器开关，或完整 SADA 加速度组合。实现阶段必须调用已验收的力矩/坐标/外力内核并在每个未测输入族增加原 ELF 金标；不得以捕获常量或单一输入组的查表替代这些计算。

## 非共线姿态外力变换补充金标

为避免单位姿态或绕 y 轴旋转掩盖 `+0x48` 外力变换，新增
`analysis/golden/core_dynamic_global_abi/controlled_attitude/`。该探针仅在
CoreDynamic 实际入口将全局四元数设为绕 x 轴 60° 的单位旋转：

```text
q = (0.86602540378443864676, 0.5, 0, 0)
```

在相同受控执行机构输入下，原 ELF 返回：

```text
F_I_external = (0, -0.00011269999999999997, -0.00019520212601301248)
```

相对于单位姿态下 `(0, -0.0002254, 0)`，这证明 `+0x48` 机体系外力会在
CoreDynamic 内经姿态相关的 body-to-inertial 路径转换，而不是总能直接复制。
该输出符合已知 60° x 轴旋转对 y 分量的分解方向；后续实现必须复用或恢复原
`get_Body2Inertial` 语义，并在比较器中按位验证这组非共线金标。

## 候选实现诊断与 MagTorque 补充

一个未导出的 CoreDynamic 候选适配器曾以当前已恢复原语重建 H/L/F、全局 RK4 和
部分输出；`analysis/core_dynamic_global_diagnostic_compare.c` 在受控输入金标中测得
`y` 有 **125/264** 字节差异、`L_c_B` 有 **18/24** 字节差异、输出有 **79/328**
字节差异。因此该候选已由 `#if 0` 从生产静态库撤回，既不导出 `CoreDynamic`，也
不被视为验收实现。

追加的原 ELF入口 snapshot 显示：`Sat+0xf0` 的第一次 MagTorque 磁场 backing
向量为 `(0,0,0)`，而 `Sat+0xc8` 的第二次 MagTorque 磁矩 backing 向量为
`(0.005,0.005,0.005)`。在这组受控起始条件下，它们不形成总力矩的非零贡献。
同时，预期 `L_c_B` 恰为 `-input[+0x00] + input[+0x60]`；因此候选中的重力梯度
附加项导致差异，说明原 CoreDynamic 的 `GravityGradientTorque` 使用的姿态/惯量对象
仍须从全局 Sat 描述符精确恢复，不能直接以 `J_c_B` 和当前临时姿态矩阵替代。

## 重力梯度对象实测修正

`GravityGradientTorque` 反汇编及新增入口 snapshot 表明，CoreDynamic 调用点的三个
对象并不等同于恢复端临时使用的 `y` 位置、姿态矩阵和 `J_c_B`。在本受控构型中，
从 `Sat` descriptor backing 数据得到：

```text
Sat+0x118 vector = (4377999.035119174, 4925604.47715956, 2162071.964895931)
Sat+0x008 matrix = [[1170.5, 782, 1.48],
                    [782, 1326.5, -17.7],
                    [1.48, -17.7, 2071]]
Sat+0x1a8 matrix = zero 3x3
```

CoreDynamic 的 `GravityGradientTorque` 调用参数依序来自这三项。第三矩阵全零使该
受控路径的重力梯度输出为零，从而 `L_c_B` 恰等于轮反号子项与推进器力矩子项之和。
这解释了候选适配器错误地以 `J_c_B` 作为第三对象时产生的 18/24 字节 `L_c_B` 差异。
后续同名实现必须从恢复端 `Sat` 的相应 descriptor 读取这三项，而不是以物理名称猜测
参数顺序。

## H/L 写入函数级闭环

新增的原 ELF反汇编确认 `SetWheelAngularMoment`（`0x69f0`）从其 `rdi` 向量
backing store按三个 `movsd` 直接复制到 `H_w_B.data[0..2]`；`SetTorque`
（`0x6990`）按同一方式复制到 `L_c_B.data[0..2]`。`UpdateTorque`
（`0xcd00`）先清零 `SatTorque+0x70` 总向量，再依次累加 `SatTorque+0x00`、
`+0x10`、`+0x20`、`+0x30`、`+0x40`、`+0x50`、`+0x60` 七个子向量，最后将
`+0x70` 总向量传给 `SetTorque`。

因此 CoreDynamic 的 H/L 全局写入不含隐藏缩放、拷贝方向或符号翻转；所有符号均
由各子力矩生产函数在 UpdateTorque 之前决定。该结论与受控快照中
`H_w_B == input[+0x18]`、`L_c_B == SatTorque total backing +0x128` 一致。
