# `quat_to_euler_angle_312` ABI 与异常终止观察

> **结论。** 在本 Linux x86-64 原 ELF 中，`quat_to_euler_angle_312(double *a, double *b, double *c, const DpQuatAbi *q)` 对正常构造的四元数 descriptor 不会返回欧拉角；它在内部 `quat2matrix` 调用链进入 `quat_psi` 时触发 `SIGSEGV`。因此本导出当前可验证的原 ELF 行为是**异常终止 ABI**，不是正常数值转换 ABI。

## 已确认入口与局部对象路径

`quat_to_euler_angle_312` 位于 `0x2f40`。前三个参数按 `rdi`、`rsi`、`rdx` 保存为三个独立 `double *` 输出；第四个 `rcx` 是 `DpQuatAbi`，其 `w` 位于 `+0x00`，嵌入 xyz 向量的 data 指针位于 `+0x10`。函数先把 descriptor 字段复制到局部对象，调用 `quat_reunit`，构造 3×3 输出矩阵，再调用 `quat2matrix`。理论上的后续提取为：

| 写回 | 原汇编中的矩阵读取 |
|---|---|
| `*a` | `atan2(-m[3], m[4])` |
| `*b` | `asin(m[5])` |
| `*c` | `atan2(-m[2], m[8])` |

这些 `atan2`／`asin` 指令在所测合法输入中不可达。

## 失败机制的调用现场证据

以 `q={w=1.0, xyz={count=3, data=[0,0,0]}}` 从 `main` 直接按 System V 寄存器 ABI 跳转至 `quat_to_euler_angle_312`。在 `quat_psi`（`0x8dc0`）入口截取到：

| 对象 | 预期数据指针字段 | 实际局部槽位 |
|---|---:|---|
| `quat_psi` 的输出矩阵 `rdi` | 函数读取 `rdi+0x10` | `rdi+0x08` 才是有效 backing 指针；`rdi+0x10` 已是下一局部 descriptor 的 `0x0000000300000004` |
| `quat_psi` 的四元数 `rsi` | `rsi+0x10` | 有效 xyz backing 指针 |

`quat_psi+0x0c` 执行 `movsd QWORD PTR [rax],xmm0`；此时 `rax` 来源于错误的 `rdi+0x10`，值为 `0x0000000300000004`，导致未映射低地址写入并以 `SIGSEGV` 终止。该布局也解释了此前直接调用 `quat2matrix` 的同类崩溃。

## 采集状态与恢复约束

原 `gdb call` 方式会因嵌套断点而中止表达式评估，故采用 `main` 处寄存器注入／直接跳转，在 `quat_psi` 入口保存现场。正常输出金标采集两次均在同一指令前异常终止；没有产生外部 `a/b/c` 写回。后续恢复应以独立子进程验证合法 descriptor 输入触发 `SIGSEGV`，不应虚构可返回的正常欧拉角数值，也不能将这种有限异常语义外推至所有无效指针、信号处理器或平台内存布局。
