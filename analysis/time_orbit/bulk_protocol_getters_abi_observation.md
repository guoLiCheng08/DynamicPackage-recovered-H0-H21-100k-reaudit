# `get_all_floats`／`get_all_uint8s` 初步 ABI 观察

原 ELF `get_all_floats(void *out)` 与 `get_all_uint8s(void *out)` 均依赖全局 `g_data` 共享内存对象和其中的 `pthread_rwlock_t`。当 `g_data` 为空时，函数调用内部 `init_shared.part.0`；初始化失败返回 `-20`。非空 out 为必需条件，空 out 返回 `-22`。

在共享对象可用时，两个函数均持读锁后复制原帧的 payload 片段，并解锁返回 0。`get_all_floats` 复制自 `g_data+0x38` 的 `0x960` bytes；`get_all_uint8s` 复制自 `g_data+0x998` 的 `0x258` bytes。它们并非纯数组辅助函数；下一步需要先建立不触碰真实共享内存的原 ELF 测试隔离策略，或恢复并验证 `init_shared`／`g_data` 生命周期，之后再创建可重复金标。

补充审计：`init_shared` 是幂等包装，若 `g_data!=NULL` 返回 0，否则进入 `init_shared.part.0`。`close_shared` 先对非空 `g_data` 执行 `munmap(...,0xBF0)` 并清零，再关闭非负 `g_shm_fd` 并置 -1。内部初始化例程使用 POSIX shared-memory、映射长度 `0xBF0`，并在成功创建时初始化共享读写锁和清零相关帧范围；错误路径可返回 `-1` 至 `-4`。由于这涉及持久共享对象与进程间锁，此阶段仍不对原 ELF 运行态写入共享区域，避免污染或把环境状态当作函数行为金标。

单值读取补充：`get_float_value(int32_t index,float *out)` 与 `get_uint8_value(int32_t index,uint8_t *out)` 的 index 均经无符号比较限制为 `0..0x257`；超界返回 `-21`，空 out 返回 `-22`，初始化失败返回 `-20`。成功时持读锁后分别从 `g_data+0x38+4*index` 与 `g_data+0x998+index` 拷贝，解锁返回 0。它们可与批量 getter 共用同一 `g_data` 生命周期金标，但暂不在未隔离共享对象的情况下实现。
