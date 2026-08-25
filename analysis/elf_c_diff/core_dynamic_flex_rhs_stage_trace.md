# CoreDynamic 受控路径：dynamics_flex RHS 分项阶段金标

探针：`analysis/golden/core_dynamic_global_abi/original_capture_controlled_flex_rhs.gdb`。它沿真实 `dyn_main → CoreDynamic → RK4 → differential_equation → dynamics_flex` 路径运行，在原 `dynamics_flex+2315`（四项三维 RHS 组合循环）捕获每个 RK4 子步的四路输入。日志：`analysis/golden/core_dynamic_global_abi/controlled_input/flex_rhs_terms.log`。

| 子步 | base 位模式 | minus 位模式 | plus 位模式 | tail 位模式 |
|---:|---|---|---|---|
| 0 | `3feeb2cab0085a65,3fdfea2b0e2f5dad,3f955ddd7b8efa7a` | `bf3c3d16ec387571,bf39d8295836c4a3,3f8c672069e7862e` | 全零 | 全零 |
| 1 | `3feee06e930a0ad5,3fe0baabbc14bf00,3f95a4d3a62f22aa` | 同子步 0 | 全零 | `bf106980a98cde21,3f10b573e6ab5bd4,bee13eaa18a2afc0` |
| 2 | `3feedb0079cdc849,3fe0b7d505f82e06,3f95a13c5c01119d` | 同子步 0 | 全零 | `bf0f9253b4ef3f84,3f0fea7ec5f0b0ff,bee090424773386f` |
| 3 | `3fef032708f55105,3fe17a4f867300ff,3f95e398695abdca` | 同子步 0 | 全零 | `bf1f8e75e0c469cf,3f1fe74c6cb8f9cf,bef08e772540399e` |

这里的 `base/minus/plus/tail` 仅是断点寄存器与栈槽的中立命名，尚未强加物理解释。可以确定的是：第 0 子步中只有 base 与 minus 非零；与此前导数级金标对应的第 0 子步角加速度差异，必来自这两路实际对象的恢复端构造或其前置有效惯量，而不是后续模态状态项。下一步须在恢复端 `dp_flex_dynamics_step` 的同一组合点记录对应分项，并按位比较。

第 0 子步分项的原 ELF 十进制值为：

```text
base  = ( 0.95932516461728012,  0.49866749177223363, 0.020865879694378787)
minus = (-0.00043088731419679062, -0.00039435395612048336, 0.013868573409982234)
plus  = (0, 0, 0)
tail  = (0, 0, 0)
```

同子步原叉乘前总动量位模式为
`c01532007163d307,c01b063793fc688a,407eab5ac153a877`，恢复端依相同已重放输入算得
`c01532007163d308,c01b063793fc688a,407eab5ac153a877`：仅 x 分量相差 1 ULP。由此可
排除总角动量的主要数量级错误；待恢复差异集中在原本地 RHS 四项与恢复端对
`L_c_B`、SADA 命令/角加速度三维驱动的拼接顺序，而非 H_total 主体。
