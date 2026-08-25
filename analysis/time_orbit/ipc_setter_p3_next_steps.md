# IPC Setter P3 下一步

已确认原 ELF 地址：`set_float_value` 为 `0x32f0`（119 bytes），`set_uint8_value` 为 `0x3370`（146 bytes）。既有 `getdyninput_controlled_gold.gdb` 已在 `init_shared()` 后用公开 ABI 写入 3 个 uint8 和 12 个 float，说明当前调试副本路径与 setter 标量调用 ABI 可正常工作。

下一步应创建独立受控脚本：先 `close_shared()` 与 `init_shared()`，再对索引 `0x257` 分别写入 `-12.75f` 和 `0xa7`，使用对应 getter 读取写回，记录两次成功返回值、两项回读位模式和索引 `0x258` 的 `-11` 边界拒绝返回。应将 float 与 uint8 结果分别导出为 `set_float_value_gold.bin`、`set_uint8_value_gold.bin`，各重复运行两次，再将既有 `p1_ipc_setters_abi_compare.c` 改为直接加载命名金标并运行全量 selftest。
