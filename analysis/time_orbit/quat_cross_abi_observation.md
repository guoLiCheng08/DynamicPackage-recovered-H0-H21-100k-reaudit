# `quat_cross` ABI 观察记录

> **范围。** 本记录仅证明原 ELF `quat_cross` 在所列受控输入下的可观察行为；不外推至未测试的非法 descriptor、别名、NaN、无穷大或飞行／安全关键用途。

## 1. 静态审计

原 ELF 符号 `quat_cross` 位于 `0x8d20`，长度 148 bytes。x86-64 SysV 参数寄存器显示三项参数依次为 `rdi=out`、`rsi=left`、`rdx=right`。函数将每个四元数的 `+0x08` 嵌入 `DpVector` 交给 `vector3_cross`、`vector_scale`、`vector_axpy` 和 `vector_dot`。

| 调用序列 | 寄存器实参 | 观察结论 |
|---|---|---|
| `vector3_cross` | `left.xyz`、`right.xyz`、`out.xyz` | 先计算 `left.xyz × right.xyz` |
| `vector_scale` | `out.xyz`、`-1.0` | 原位乘以 `-1` |
| 首次 `vector_axpy` | source=`right.xyz`，destination=`out.xyz`，alpha=`left.w` | 在负叉积结果上累加 `right.xyz * left.w` |
| 第二次 `vector_axpy` | source=`left.xyz`，destination=`out.xyz`，alpha=`right.w` | 在同一输出上继续累加 `left.xyz * right.w`；结合实际金标确定最终方向 |
| `vector_dot` | `left.xyz`、`right.xyz`、输出标量槽 | 逐项 `left[i]*right[i]`，从零累加 |

原函数最终输出的可观察四元数方向是 **`right × left`**，而非按参数名称直觉推断的 `left × right`。

## 2. 原 ELF 受控金标

隔离探针构造三个 0x18-byte `DpQuatAbi` descriptor：

| 对象 | 标量 `w` | `xyz` | descriptor 其他字段 |
|---|---:|---:|---|
| `out` | `99.0` | `(101.0, 102.0, 103.0)` | `count=3`，`reserved=0x11223344` |
| `left` | `0.5` | `(1.25, -2.5, 3.75)` | `count=3`，`reserved=0x55667788` |
| `right` | `-1.5` | `(4.25, -5.5, 6.75)` | `count=3`，`reserved=0x99aabbcc` |

原 ELF 返回后，`out` 的标量与 `out.xyz` 分别为 `-45.125` 与 `(-3.5,-6.5,-6.0)`。输出 `count`、`reserved` 和 `data` 指针保持不变。由于 `data` 是跨进程堆地址，回归比较 descriptor 的前 16 bytes，并另行断言恢复端的指针仍指向调用前的 `out_xyz`；向量 backing 的 24 bytes 单独作 `memcmp`。

| 金标文件 | 大小 | SHA-256 |
|---|---:|---|
| `gold_quat_cross_p3_output.bin` | 24 bytes | `1c8f74d32cea438eb8e01e7523da571e0965921af67dd0cafbfc012e4485a09e` |
| `gold_quat_cross_p3_xyz.bin` | 24 bytes | `95c440b2c25422b73d87155e8ab40a721a917965f5289d15509ca299f5cecf5d` |

## 3. 恢复策略和回归

恢复端公开导出位于 `src/dynamic_sensors.c`。既有私有 `dp_quat_cross_left` 为历史上层调用点的方向补偿适配器，因此没有直接包装它；同名导出独立逐项表达原 ELF 的乘、减、负一缩放、两次加法与点积累加顺序，以避免合并表达式改变 IEEE-754 舍入或负零位型。

严格门禁新增 `analysis/quat_cross_p3_gold_compare.c`，并已接入 `Makefile`。该比较器验证输出 descriptor 的地址无关 16 bytes、data 指针身份和三个双精度输出分量的 24 bytes。

## 4. 未覆盖边界

尚未隔离验证空指针、`count != 3`、输入／输出 descriptor 或 backing 的别名、负零组合、NaN/Inf 与超大数值。它们不应被当前单一正常输入金标视为已经证明。
