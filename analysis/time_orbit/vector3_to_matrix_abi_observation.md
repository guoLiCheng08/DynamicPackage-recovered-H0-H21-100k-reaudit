# `vector3_to_matrix` 初步 ABI 观察

原 ELF `vector3_to_matrix`（0x8370，177 bytes）仅在入口保存 RDI，随后在栈上构造两个临时矩阵 descriptor，并以保存的 RDI 作为 `blas_gemm` 第三参数（输出矩阵）。反汇编中未见从外部输入向量读取的指令；两个临时 data 区在调用前被清零。因此，当前可确认的只有“RDI 为输出矩阵 descriptor、内部调用 `blas_gemm`”这一点，尚不能安全断言其公开函数签名或用途。

此观察未建立原 ELF 可复现金标、未写入恢复端、未纳入严格自检。后续应先从原 ELF 调用点恢复真实调用约定或对单参数合法矩阵调用建立快照，再决定实现。
