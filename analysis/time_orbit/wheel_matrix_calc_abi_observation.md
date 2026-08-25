# `wheel_matrix_calc` ABI 观察（P3，2026-08-21）

原 ELF `wheel_matrix_calc` 位于 `0xe3f0`，其真实 ABI 为：

```c
void wheel_matrix_calc(DpMatrix *out, const DpVector *col0,
                       const DpVector *col1, const DpVector *col2,
                       const DpVector *col3);
```

函数不进行三角函数或飞轮参数计算；它依次尾调用/调用已审计的 `matrix_set_col`：

| 列索引 | 来源参数 |
|---:|---|
| 0 | `rsi` / `col0` |
| 1 | `rdx` / `col1` |
| 2 | `rcx` / `col2` |
| 3 | `r8` / `col3` |

每次 `matrix_set_col` 的实际返回值均被忽略，且最后一次采用 tail-jump。因此后续金标必须覆盖正常 3×4 输出和任一列 descriptor 失败时的部分写回行为。
