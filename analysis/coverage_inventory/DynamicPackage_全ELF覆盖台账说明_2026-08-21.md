# DynamicPackage 全 ELF 行为等价恢复：符号覆盖台账说明

**生成日期：** 2026-08-21  
**工程基线：** `dynamicpackage_recovered_work/`  
**目标定义：** 将原始 Linux x86-64 ELF 的每个非运行时全局函数，映射到恢复端实现、原 ELF 受控金标、严格回归和异常 ABI 证据；以可复算指标管理“全 ELF 可观察行为等价”的长期恢复工作。

> **结论边界。** 本台账衡量的是“符号接口是否有恢复实现及是否已绑定证据”，不是全输入空间的等价证明。任何通过的金标只对其指定 Linux x86-64、浮点库、初态、随机种子、命令和时域有效；不得据此推导安全关键、实体执行机构或飞行用途的正确性。

## 1. 台账分母与证据口径

原样本为未剥离的 x86-64 PIE ELF。其定义函数中共有 205 个符号，其中 200 个为 GLOBAL/WEAK；扣除 `_init`、`_fini`、`_start`、`main`、`__libc_csu_init`、`__libc_csu_fini` 六个 ELF/CRT 运行时入口后，本台账的**领域函数分母为 194**。[1]

| 指标 | 严格定义 | 不应误读为 |
|---|---|---|
| **直接 ABI 可用率** | 原 ELF 函数名以同名全局函数出现在恢复静态库。 | 全部参数、全局副作用和所有输入等价。 |
| **同名严格门禁率** | 直接 ABI 函数至少有一个含该原符号名的比较器且已写入 `Makefile` 的 `selftest`。 | 覆盖了函数的所有分支或特殊浮点。 |
| **三重证据率** | 直接 ABI 同时具备同名 GDB 探针、可按规范化名称关联的金标文件和已接入 `selftest` 的比较器。 | 金标的所有边界均已覆盖。 |
| **内部别名候选** | 经源码审阅，将原公开符号映射至 `dp_*` 内部帮助函数。 | 已有同名公开 ABI 或可以直接计入实现完成。 |
| **顶层管线覆盖** | 八个明确阶段是否各有单独比较器接入 `selftest`。 | 所有时序、外部命令与组合场景均已覆盖。 |
| **异常 ABI 见证数** | 已观察并回归的隔离死亡测试数量。 | 所有非法指针、descriptor、NaN/Inf 等异常组合的百分比。 |

该口径将“代码中存在相近逻辑”与“原公开 ABI 已有独立位级证据”明确分离。这样可以避免因为 `dp_*` 私有帮助函数或注释文本存在，就过早宣称原 ELF 符号已完成恢复。

## 2. 当前量化进度

| 分层维度 | 已完成 | 分母 | 覆盖率/状态 | 含义 |
|---|---:|---:|---:|---|
| 原 ELF 领域函数清单 | 194 | 194 | 100.0% | 分母已固定，可重复提取。 |
| **直接 ABI 名称可用** | 110 | 194 | **56.7%** | 已有同名恢复库全局函数。 |
| **直接 ABI + 同名严格门禁** | 104 | 194 | **53.6%** | 有明确关联的比较器且在 `selftest`。 |
| **直接 ABI + 探针 + 金标 + 门禁** | 53 | 194 | **27.3%** | 当前最强的逐符号证据层。 |
| 内部语义别名候选 | 28 | 194 | 14.4% | 不计入直接 ABI；仍需公开包装和独立金标。 |
| 仅有同名本地符号 | 2 | 194 | 1.0% | 恢复库尚未提供同名全局 ABI。 |
| 直接/别名/本地实现线索 | 140 | 194 | 72.2% | **线索覆盖**，不是行为等价覆盖。 |
| 无恢复端线索 | 42 | 194 | 21.6% | 需从反汇编和原 ELF 金标开始闭环。 |
| 仅源码文字引用 | 12 | 194 | 6.2% | 不可当作实现；需确认实际映射。 |

因此，若只询问“ELF 的公开领域符号有多少已具备同名恢复入口”，当前可回答 **56.7%**；若询问“具有同名实现、探针、金标与门禁这四层证据的符号比例”，当前为 **27.3%**。这两个数字分别反映接口存在性和证据强度，均**不是**“全 ELF 行为等价百分比”。

直接 ABI 层仍有 **84 个**符号缺少同名全局恢复入口；其中 54 个没有可直接计入的实现或别名线索（42 个无证据、12 个仅文本引用），另有 28 个内部别名候选和 2 个本地可见同名符号，需要先闭合公开 ABI。即使对现有 110 个直接入口，仍有 57 个尚未形成探针、金标和门禁齐备的三重证据闭环。

## 3. 顶层场景与异常 ABI 指标

顶层路径的局部证据强于普通逐符号统计。`DynamicInit`、`UpdateDeviceControl`、`CoreDynamic`、`UpdateDeviceMeasure`、`UpdateMainOut`、`sendDynTele` 和 `dyn_main` 七个阶段都有独立比较器接入 `selftest`；`UpdateCoreDynInput` 目前只通过 `dyn_main` 集成路径受到间接验证。因此，按“孤立阶段已接入严格门禁”的固定八阶段口径，结果为 **7/8（87.5%）**。[2]

`dyn_main` 在本次重现的日志中有 **51 条**“原 ELF 比较通过”的场景见证，覆盖 2、10、20 和 50 步受控时域，以及设备、姿态、柔性状态、轨道日期、DSS 噪声和 IPC 等组合。这说明顶层主线在已选场景内具有很强证据，但场景空间没有自然的有限总数，故不能转化为“51/N 的全场景覆盖率”。

| 顶层阶段 | 证据状态 | 备注 |
|---|---|---|
| `DynamicInit` | 独立严格门禁 | 全局初始化比较器已接入。 |
| `UpdateDeviceControl` | 独立严格门禁 | 受控设备控制金标已接入。 |
| `UpdateCoreDynInput` | 仅集成间接证据 | 应补原公开 ABI 的隔离原 ELF 金标及比较器。 |
| `CoreDynamic` | 独立严格门禁 | 默认 RK4、两步与环境桥接比较器已接入。 |
| `UpdateDeviceMeasure` | 独立严格门禁 | 全链与调度/GPS 状态机比较器已接入。 |
| `UpdateMainOut` | 独立严格门禁 | 输出布局、设备适配和 STS 路径已有比较器。 |
| `sendDynTele` | 独立严格门禁 | 共享帧映射及 `dyn_main→IPC` 路径已有证据。 |
| `dyn_main` | 独立严格门禁 | 51 个原 ELF 场景见证。 |

异常 ABI 方面，本次 184 条完整回归包含 5 个明确的隔离 `SIGSEGV` 死亡见证：`vector_memcpy`、`vector_axpy`、`vector_axpby` 的空源，以及 `quat_to_euler_angle_312`、`quat2matrix` 的已观察异常描述符路径。该结果应表述为“**5/5 个已枚举死亡见证通过**”，而不应伪造全异常空间的百分比。[2]

## 4. 最重要的缺口与行动队列

### P0：公开生命周期、命令和遥测封装

以下 11 个无恢复证据的公开符号处在调用入口、命令处理或遥测封装层，应优先逐个完成反汇编—金标—最小实现—门禁闭环：`DynamicDllInit`、`Update_Telemetry_Frame_1`、`Update_Telemetry_Frame_2`、`Set_Algorithm_Telemetry_Frame_1`、`Set_Algorithm_Telemetry_Frame_2`、`Analyze_Command`、`Command_Execute`、`Algorithm_Command_Execute`、`DYN_Command_Execute`、`getDeskCommand`、`dyn_init_array`。

这些函数未必是数值核心，却构成原 ELF 对外生命周期与控制入口的一部分。对“全 ELF 行为等价”目标而言，不能仅因已有 `dyn_main` 恢复，就忽略这些导出的系统边界。首项建议为 `DynamicDllInit`：先确认它与当前 `DynamicInit`/初始化全局块的关系，再制作隔离入口金标；随后成组处理命令和遥测帧封装函数，以共享相同的对象布局和 IPC 证据。

### P1：环境、磁场、初始化和内部别名的 ABI 闭合

环境/外力块的缺口包括 `Pnm`、`gh_change`、`GetInertialMag`、`UpdateExternalForce`、`FrontalArea`、`AerodynamicTorque`。其中 `Pnm` 与 `gh_change` 是已有交接文档指定的优先恢复对象；它们应在当前 `Calc_InertialMagneticVector` 内部候选映射之前或同步完成，因为后者的公开 ABI 是否等价取决于球谐系数与时间更新状态。

设备、传感器、惯量与初始化块应分为两类推进。第一类为**无实现证据**，例如 `Gyro_Init`、`MagTorque_Init`、`MagMeter_Init`、`STS_Init`、`DSS_Init`、`Thruster_Init`、`Wheel_Init`、`Update_sat_inertia`；第二类为**仅文本引用或内部别名候选**，例如 `SatParaInit`、`TorqueInit`、`dyn_init`、`SetInertiaTensor`、`SetSatInertiaTensor`、`SetSpacecraftMass`、`GetWheelSpeed`、`MagTorque`、`RWheelTorque`、`SetSADA`、`SetThrusterWorkStatus`、`SetWheelAcc`。后一类不可直接算完成，必须先验证原 ABI 的参数、返回值、全局写回、别名和异常行为，再决定是导出包装还是独立实现。

### P2：加深已有直接接口的证据厚度

目前有 6 个同名直接恢复函数尚未被台账按“原符号名比较器”关联到 `selftest`，另有 57 个直接入口缺少探针、金标或门禁三者之一。对这些项目，优先补证据而非重写实现：重复采集原 ELF 金标，保存哈希，编写独立比较器，并将比较器加入 `Makefile`。尤其应补 `UpdateCoreDynInput` 的隔离 ABI 比较器，从而将顶层管线的孤立证据由 7/8 提升至 8/8。

## 5. 固定的单元闭环模板

后续每个符号都应遵循相同的闭环，以保证覆盖率变化具有可审计性：

1. 从原 ELF 的符号表、反汇编和调用点确认真实 SysV ABI、descriptor 偏移、写回范围与存储顺序。
2. 在可执行调试副本中建立最小 GDB 探针，固定输入与 backing；原只读样本不得改写。
3. 至少重复一次原 ELF 金标采集并比较 SHA-256，以确认受控场景确定性。
4. 仅据反汇编与金标写最小 C11 实现，保留浮点、分支和内存写回顺序。
5. 编写地址无关的逐字节比较器；若原 ELF 正常路径不可达，则使用隔离死亡测试复制已观察信号语义。
6. 将比较器加入 `Makefile` 的 `selftest`，从空构建运行全量门禁。
7. 更新 `symbol_coverage_ledger.csv`、覆盖矩阵、行动队列与回归日志哈希。

## 6. 可复算产物

| 文件 | 用途 |
|---|---|
| `original_defined_function_symbols.tsv` | 原 ELF 全定义函数符号与地址/大小/绑定。 |
| `original_global_function_names.txt` | 194 个领域函数分母的来源（运行时项另行排除）。 |
| `recovered_global_function_names.txt` | 恢复库公开函数符号集合。 |
| `symbol_coverage_ledger.csv` | 每个原领域符号的实现状态、探针、金标、比较器和文档关联。 |
| `symbol_coverage_metrics.md` | 直接 ABI 与别名候选的基础统计。 |
| `coverage_layers.json` / `.md` | 符号、顶层管线与异常 ABI 的分层指标。 |
| `coverage_action_queue.csv` / `.md` | 未闭环条目的优先级、当前证据和下一步动作。 |
| `generate_symbol_ledger.py` | 台账再生成脚本。 |
| `calculate_coverage_layers.py` | 分层指标计算脚本。 |
| `build_action_queue.py` | 行动队列计算脚本。 |

## 结论

覆盖台账已经将“还差多少”的问题拆解为可复算的四个维度。当前工程具有 **56.7% 的直接公开 ABI 名称可用率**、**53.6% 的直接 ABI 同名严格门禁率**、**27.3% 的三重证据率**；顶层八阶段中有 **7/8** 的独立严格门禁，且 `dyn_main` 已有 51 条原 ELF 场景见证。与此同时，仍有 84 个领域函数未形成同名公开恢复入口，54 个没有可直接计入的实现/别名线索，故“全 ELF 行为等价”距离完成仍然明显。

下一轮应先闭合 `DynamicDllInit` 与命令/遥测 P0 封装组，并补 `UpdateCoreDynInput` 的隔离 ABI 回归；随后推进 `Pnm`/`gh_change` 及初始化/设备组。每完成一个符号闭环，即可由脚本重新计算上述比例，避免以主观感觉估计进度。

## References

[1] `analysis/coverage_inventory/original_defined_function_symbols.tsv`；由原只读 `input/DynamicPackage.elf` 的符号表提取。  
[2] `/home/ubuntu/dynamicpackage_audit_tar/reproduced_full_clean_selftest_2026-08-21.log`；隔离目录中 `make clean && make selftest` 的 184 条 PASS 重现日志。  
[3] `analysis/coverage_inventory/symbol_coverage_ledger.csv`、`coverage_layers.json`、`coverage_action_queue.csv`；本台账的可复算数据产物。  
[4] `Makefile`、`analysis/elf_c_diff/systematic_coverage_matrix.md`、`AI_HANDOFF_zh-CN.md`；严格构建门禁、既有覆盖边界与工程约束。

**用途限制：** 本工程仅可用于离线 ELF 行为研究和仿真验证；不得接入飞行、控制、实体执行机构或其他安全关键系统。
