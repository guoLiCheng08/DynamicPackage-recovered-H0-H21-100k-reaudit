# DynamicPackage 全 ELF 行为等价恢复：第 1 批推进检查点

**日期：** 2026-08-21  
**本批目标：** 闭合 `DynamicDllInit` 的默认启动分支，以及为 `UpdateCoreDynInput` 增加独立原 ELF ABI 金标与严格门禁。  
**原样本：** `input/DynamicPackage.elf`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

> **结论。** 本批新增两个原 ELF 符号的公开恢复 ABI 和独立位级回归。`UpdateCoreDynInput` 在一个覆盖六个三维输入槽和尾部哨兵字节的非零受控场景中，完整 **144/144 字节**匹配。`DynamicDllInit` 在“新进程、共享输入不可读”的默认启动分支中，初始化条件块、SADA 对象及五组地址无关设备 backing 均为**零字节差异**。完整空构建严格门禁现有 **186 条 PASS**。

## 1. 新增闭环

| 原 ELF 符号 | 已恢复 ABI | 原 ELF 金标 | 位级比较范围 | 门禁状态 |
|---|---|---|---|---|
| `UpdateCoreDynInput` | `void UpdateCoreDynInput(void *core_dynamic_input)` | 重复采集的 P4 受控设备快照；两次结果 `cmp` 一致。 | `0x90` 字节输出帧；五组三维设备向量、两项 SADA 角以及 `+0x88` 尾部哨兵。 | 已接入 `selftest`，PASS。 |
| `DynamicDllInit` | `void DynamicDllInit(void)` | 重复采集的新进程默认启动快照；以 PIE 基址计算本地 `init` 全局地址，避免与 libc 同名符号混淆。 | `0xe8` 初始条件块、`0x68` SADA 对象及 Wheel/MTQ/Thruster 五组 3-double backing。 | 已接入 `selftest`，PASS。 |

`UpdateCoreDynInput` 的原 ELF 反汇编表明其唯一显式参数位于 `rdi`。函数将 WheelGroup、MTQ_Group 和 Thruster descriptor backing 拷入连续六个 `DpVec3` 槽位，并跳转到 `getSADAangle`。新增探针以互异非零值驱动所有源槽位，并将输出预填为 `0xa5`；金标确认 `getSADAangle` 只写 `+0x78` 与 `+0x80` 两个 double，`+0x88` 保留哨兵。本恢复实现已有相同写入范围，新增的独立比较器验证其在该受控场景中无任何字节差异。[1]

`DynamicDllInit` 在本批覆盖的分支中无显式参数。恢复端在独立的 `dynamic_dll_init.c` 中构造经原 ELF 金标确认的默认 `DpInitialConditions`，包括 `0.01` 步长、2024-10-28 04:16:16 时间参数、轨道根数、9 个惯量项和 600 质量值。它还重建零设备群组 backing 与 SADA 的初始角、命令限位和加速度限位。为避免为旧的局部测试引入链接依赖，该实现从 `dynamic_satellite_globals.c` 独立成单独目标文件。[2]

## 2. 可复现性结果

| 核验项 | 结果 | 证据 |
|---|---|---|
| `UpdateCoreDynInput` 原 ELF 金标重复采集 | PASS | 两份 `0x90` 输出金标 SHA-256 相同：`8ddb7c81068a572cfd1f72200c3882c609379c71e916b207a17ad30ec53f14c4`。 |
| `DynamicDllInit` 默认分支金标重复采集 | PASS | 全部 11 个状态/ backing 文件逐字节 `cmp` 一致；`init` 金标 SHA-256 为 `16049c14696ba27f8245c354ebaa67000cbf22d437568cbdead4306d13738cbc`。 |
| 新增比较器单独运行 | PASS | `UpdateCoreDynInput`: `0/144` 不同；`DynamicDllInit`: 所有七个比较块均 `0` 不同。 |
| 空构建严格回归 | PASS | `make clean && make selftest` 输出 **186** 条 PASS，无编译警告、链接错误或 FAIL 标记。日志 SHA-256：`b442f9f78e0f6b777d104b2efcfbf438768457b9bae8f21073ddfef0659050d4`。 |
| 原 ELF 保护 | PASS | 本批结束后原样本仍为既定 SHA-256，且为只读模式。 |

## 3. 覆盖台账变化

覆盖生成器已改为在每次运行时从 `build/libdynamicpackage_recovered.a` 刷新全局函数清单，避免新模块因静态清单滞后而漏记。[3]

| 指标 | 本批后数值 | 解释 |
|---|---:|---|
| 具有同名公开恢复 ABI | 111/194，57.2% | `DynamicDllInit` 已加入直接 ABI 分子。 |
| 具有同名严格门禁 | 106/194，54.6% | `DynamicDllInit` 与此前本批的 `UpdateCoreDynInput` 均已登记。 |
| 探针＋金标＋门禁三重见证 | 55/194，28.4% | 这是较严格的符号级证据口径，不等于全输入空间证明。 |
| 独立顶层阶段门禁 | 8/8，100.0% | `UpdateCoreDynInput` 从仅 `dyn_main` 间接见证升级为独立门禁。 |
| 未闭环行动队列 | 88 项 | 余下 P0 10、P1 73、P2 5。 |

## 4. 严格边界与未完成项

本批的 `DynamicDllInit` 只能计为**默认启动分支已闭环**，不能泛化为其全行为等价。原 ELF 会尝试读取共享输入 64、65、66；这些读取成功时的值覆盖、失败诊断 stdout、传感器内部对象以及非新进程重入语义尚无独立金标。后续应使用可控共享输入伪环境，分别采集三种读取成功/失败组合，再分离比较初始条件、stdout、设备对象和 IPC 副作用。

`UpdateCoreDynInput` 的普通有效输出 ABI 现已闭环，但空指针、未初始化 descriptor、别名输出缓冲区和内存不可写等异常 ABI 尚未与原 ELF 比较。不得把恢复端的 `NULL` 提前返回扩展为原 ELF 的已证明行为；当前金标仅支持有效 `rdi` 和已初始化全局 descriptor 的受控场景。

> **计数限制。** 186 PASS 是严格回归断言/比较器通过次数，并非 186 个函数，更不是 194 个符号的完整行为等价证明。

## 5. 下一批建议

下一批应优先批量闭合余下 10 个 P0 外部入口：`dyn_init_array`、`Update_Telemetry_Frame_1`、`Update_Telemetry_Frame_2`、`Set_Algorithm_Telemetry_Frame_1`、`Set_Algorithm_Telemetry_Frame_2`、`Analyze_Command`、`Command_Execute`、`Algorithm_Command_Execute`、`DYN_Command_Execute` 与 `getDeskCommand`。这些入口共享命令帧、遥测帧和 IPC 对象，可复用本批的“原 ELF probe → 重复金标 → 最小 wrapper → 单元比较器 → 全量门禁 → 台账刷新”模板。[4]

## References

[1] `analysis/coverage_inventory/target_closure/update_core_dyn_input_p4_probe.gdb`、`gold_update_core_dyn_input_p4_output.bin`、`analysis/update_core_dyn_input_p4_gold_compare.c`。  
[2] `analysis/coverage_inventory/target_closure/dynamic_dll_init_p0_probe.gdb`、`gold_dynamic_dll_init_p0_*.bin`、`src/dynamic_dll_init.c`、`analysis/dynamic_dll_init_p0_gold_compare.c`。  
[3] `analysis/coverage_inventory/generate_symbol_ledger.py`、`symbol_coverage_ledger.csv`、`coverage_layers.md`。  
[4] `analysis/coverage_inventory/coverage_action_queue.csv`、`analysis/coverage_inventory/effort_estimate.md`。

**用途限制：** 仅限离线 ELF 行为研究与仿真验证；不得接入飞行、控制、实体执行机构或其他安全关键系统。
