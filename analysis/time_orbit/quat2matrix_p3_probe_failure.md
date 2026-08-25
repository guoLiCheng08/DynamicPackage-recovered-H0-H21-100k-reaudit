# `quat2matrix` 直接探针：未闭环观察

在 `analysis/time_orbit/quat2matrix_p3_probe.gdb` 中，以恢复端已知的 `DpMatrix`（3×3，data 指针位于 `+0x10`）与 `DpQuatAbi`（scalar `+0x00`、xyz data 指针 `+0x10`）直接调用原 ELF `quat2matrix`，原程序在内部 `quat_psi` 的首个输出 descriptor 解引用处发生 SIGSEGV。

反汇编显示 `quat2matrix` 会在栈上合成三个中间对象，再依次调用 `quat_psi`、`quat_xi`、`matrix_trans`、`blas_gemm`。但当前通过直接调用可观察到的局部对象布局与恢复端 `DpMatrix` 假设并不自洽：`quat_psi` 读取其首参 `+0x10` 作为数据指针，而 `quat2matrix` 在对应局部区放入的值表现为维度打包常量。因此，不能仅以 `quat_att_mat` 的同名算法骨架替代这个导出。

此记录不是行为等价结论，也未纳入自检；后续需要从原 ELF 调用点恢复该导出的真实类型定义或建立隔离死亡边界后，再决定实现策略。


本轮在当前调试副本重复该直接调用，仍由 GDB 报告被调函数内部 `SIGSEGV`，未产生可用于正常返回路径的 3×3 金标。该重复观察只强化已存在的死亡边界证据，不改变其尚缺正常调用点 ABI/命名金标的 P3 状态。


同轮以 `quat_to_euler_312_p3_probe.gdb` 的直接描述符调用原 ELF `quat_to_euler_angle_312` 亦在被调函数内触发 SIGSEGV，未生成正常 12-double 输出金标。该观察与既有 `quat_to_euler_312_p3_death_test.c` 的异常 ABI 见证一致；后续需从真实调用点恢复实际 descriptor 布局，或建立显式信号退出金标，而不能将直接探针失败伪装为正常函数输出。
