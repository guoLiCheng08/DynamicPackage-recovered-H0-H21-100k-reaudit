# `differential_equation` 全局 ABI 审计

**原 ELF 符号：** `differential_equation`，地址 `0x6a50`，大小 `1626` 字节。  
**证据来源：** `analysis/differential_equation.asm`、`analysis/time_orbit/differential_equation_real_model_snapshot.gdb`、`src/dynamic_dynamics.c` 与 `src/dynamic_core_bridge.h`。  
**范围：** 本文只确认当前样本、当前调用路径下的参数、状态分块、显式全局依赖及写回顺序；不推断异常指针、越界状态或其他宿主环境下的行为。

## 1. 函数参数与状态 ABI

入口处将 `rdi` 保存为输出指针、`rsi` 保存为输入指针。因此真实 ABI 是：

```c
void differential_equation(double derivative[33], const double state[33]);
```

函数没有显式上下文参数。刚柔动力学所需状态经全局 `H_w_B`、`L_c_B`、`J_c_B`、`J_c_B_inv` 及 `Sat` descriptor 图隐式取得。

| 状态或输出范围 | 原汇编证据 | 已确认语义 |
|---|---|---|
| `state[0..3]` | `0x6c12..0x6ca0` | 四元数；先构造局部 `DpQuatAbi` 并调用 `quat_reunit`。 |
| `state[4..6]` | `0x6ca9..0x6cca` | 机体角速度。 |
| `state[7..9]` | `0x7005..0x7027` | GCI 位置；传入 `orbit_dynamic`。 |
| `state[10..12]` | `0x6cd3..0x6cf4`、`0x7040..0x7062` | GCI 速度；复制到位置导数。 |
| `state[13..22]` | `0x6cfd..0x6d49` | 十维柔性模态位移。 |
| `state[23..32]` | `0x6d52..0x6e04` | 十维柔性模态速度。 |
| `derivative[0..3]` | `0x6e7d..0x6eaf` | `quat_diff(reunit(state.q), omega)` 输出。 |
| `derivative[4..6]` | `0x6eb4..0x6ed4` | `dynamics_flex` 给出的刚体角加速度。 |
| `derivative[7..9]` | `0x6ffc..0x7027` | 直接复制 `state[10..12]`。 |
| `derivative[10..12]` | `0x7031..0x7062` | `orbit_dynamic(position)` 输出的 GCI 加速度。 |
| `derivative[13..22]` | `0x6ed9..0x6f51` | `dynamics_flex` 的十维第一模态输出。 |
| `derivative[23..32]` | `0x6f59..0x6fd7` | `dynamics_flex` 的十维第二模态输出。 |

## 2. 调用顺序与显式全局依赖

原汇编在完成局部四元数归一化与 `quat_diff` 后，于 `0x6e31` 调用 `dynamics_flex`，随后于 `0x703b` 调用 `orbit_dynamic`。这两个调用的先后及输出写回顺序必须保持。

`dynamics_flex` 的栈参数按压栈顺序对应下列全局 descriptor：

```text
L_c_B → H_w_B → J_c_B → J_c_B_inv
```

四个对象都是 descriptor，而非裸数组。`H_w_B` 和 `L_c_B` 为三元 `DpVector`；`J_c_B` 和 `J_c_B_inv` 为 3×3 `DpMatrix`。这与 `src/dynamic_runtime_layout.h` 对底层 `*_mem` 与 descriptor 分离的定义一致。

`orbit_dynamic` 不从函数显式参数接收质量或外力；它读取全局 `SpacecraftMass` 与 `F_I_external`。因此，任何同名全局入口必须将这些对象与恢复端上下文一致地绑定，不能使用临时默认值掩盖缺失状态。

## 3. 与已验收上下文化数值核心的映射

`dp_differential_equation_33(derivative, state, context)` 已按上述 33 维分块实现四元数、刚柔和轨道导数组装。它还从显式 `DpDynamicsContext.flex` 获取基础惯量、3×10 耦合矩阵、模态矩阵、角动量、力矩扣减项及 SADA 附加映射。

因此，下一步不是改写该数值核心，而是新建**可审计的全局→上下文适配层**。最小适配层须：

1. 读取真实兼容全局 descriptor 数据，而不是 descriptor 地址本身；
2. 保持 `dynamics_flex` 所需的 `Sat` 3×10 耦合、模态和惯量图与当前构型同步；
3. 将 `H_w_B`、`L_c_B`、`SpacecraftMass`、`F_I_external` 和 SADA 状态纳入同一调用前快照；
4. 仅在一般非退化原 ELF 金标覆盖后，公开同名 `differential_equation`；
5. 在任何全局包装器中保留原 `void` ABI 和无检查写入语义，而不擅自添加输入校验。

## 4. 下一阶段金标合同

应以 `differential_equation_real_model_snapshot.gdb` 为基线，扩展一次原 ELF 一般输入采集，至少记录：

- 33 个输入状态 double 和 33 个输出导数 double；
- `H_w_B`、`L_c_B`、`J_c_B`、`J_c_B_inv` descriptor 的元数据及实际数据；
- `F_I_external`、`SpacecraftMass`；
- 对应 `Sat` 的惯量、3×10 耦合、模态矩阵、SADA 状态；
- 调用前后所有可能可观察的全局对象变化。

只有当恢复端适配器对同一快照输出 33 个 double 逐位一致时，才可将其视为 `differential_equation` 同名导出的受控路径闭合。
