# dynamics_flex 公开 ABI 审计（进行中）

- 原 ELF 入口：`0xe920`，void 返回，大型 `0x908` 字节栈帧。
- 入口保存：`rdi`、`rsi`、`rdx`、`rcx`、`r8`、`r9`。单次原 ELF 入口探针确认六者均为带 count/data 的描述符对象；反汇编读取的后续栈槽在该受控调用中均为零，故仅标为可选/待验证而非必需参数。
- 已观察的可变对象：原 `rdx` 作为两次 10 维矩阵投影累加的结果；原 `r8` 作为结尾 `vector_memcpy` 的输出。
- 隐式依赖：`SADA+0x08/+0x10/+0x38/+0x40`，以及 `Sat+0x898/+0xa00/+0xd38` 等矩阵描述符。
- 现有 `dp_flex_dynamics_step` 只有 7 参数重构接口，不可在未完成参数布局、别名和副作用金标前直接导出为同名 ABI。

## 单次入口描述符快照

| 寄存器 | 观测维度 | 当前角色结论 |
|---|---:|---|
| rdi | 3 | 输入/输出待确认 |
| rsi | 3 | 输入/输出待确认 |
| rdx | 10 | 已观察到两次累加写入 |
| rcx | 10 | 输入/输出待确认 |
| r8 | 10 | 已观察到结尾 memcpy 写入 |
| r9 | 10 | 输入/输出待确认 |

栈槽 7–9 在当前受控正常调用中均为零；不视为必需实参。

## 零状态正常金标

单次原 ELF 正常启动链在 `dynamics_flex` 末尾的六个描述符数据区均为全零。这是可复放的零状态正常见证，但输入激励与输出均缺乏区分度；不得据此声称非零模态、非零角速度、别名或异常描述符情形的行为等价。下一阶段必须采集非零受控输入的重复原 ELF 金标。

## 非零受控金标（重复一致）

入口注入：`rdi=(0.1,-0.2,0.3)`、`rsi=(1,2,-1)`；`rcx[0..1]=(0.25,-0.5)`；`r9[0..1]=(0.75,0.125)`。原 ELF 于 `dynamics_flex+0xb43` 的快照显示：`rdi` 的 3 个 double 被改写；`rdx` 的 10 个 double 均成为非零结果；`r8` 复制 `rcx`；在此场景中 `rsi` 与 `r9` 保持注入值。该结果来自真实正常初始化链且两次执行位型一致，但尚未覆盖所有元素非零、描述符别名、空指针和维度异常情形。

## 公开参数结构 ABI

六个寄存器实参均为 `DpVector*`（16 字节：`int32 count`、4 字节保留、`double *data`）。单次入口快照中 `rdi/rsi.count=3`，`rdx/rcx/r8/r9.count=10`。公开包装必须保持该描述符 ABI，并仅在内部调用边界把两个 3 维 data 区建立为裸 `DpVec3` 视图。

## 当前最小包装对照（未通过）

已实现六 `DpVector*` 同名包装并以默认模型配置调用内部步骤。非零受控对照中，仅 `r8 <- rcx` 的 10 维速度复制输出位型匹配；角加速度和模态加速度与原 ELF 失配（例如原 `rdi[0]=0x3fe145bcd43bcf9e`，当前为 `0x3f56fa0ca9b8fdec`）。因此该包装不得接入 selftest 或覆盖台账。下一步需以更多原 ELF 检查点区分 `rsi` 的精确角色及隐式 Sat/SADA 配置。
rsi 角色：原 dynamics_flex 在 0xeb97、0xedc5..0xee20 使原 rsi 进入 3 维组合与 vector3_cross 链；不能仅按总角动量映射。

## 原调用链

单次入口 GDB 调用栈确认：`CoreDynamic -> RK4_Intergrator -> differential_equation -> dynamics_flex`。六个 DpVector 参数来自 33 维动力学导数路径；后续参数角色与隐式项校正应以 `differential_equation` 状态切片和 Sat 配置为基准。

## 十参数 ABI 复测

按调用点补齐 `L_c_B/H_w_B/J_c_B/J_c_B_inv` 四个栈上传参后，非零包装对照的角加速度与模态加速度位型未改变，只有速度复制继续匹配。主失配因此转移为原 `differential_equation` 时刻的实际 Sat/SADA 全局状态装配，下一轮需采集调用前全局描述符与 SADA 快照。

## 调用前 Sat/SADA 运行态快照

原 ELF `dynamics_flex` 入口快照重复一致：SADA 含非零运行态（包括 π 量级角度及非零动态字段），并非恢复端临时配置中的零状态；Sat+0x448/+0x4a8/+0x688/+0x898/+0x9a0/+0xa00/+0xd38 均为已初始化描述符。十参数 ABI 已补齐而结果不变，故后续校正重点为把这些真实运行态字段精确映射到恢复端配置。

## SADA 角加速度映射复测

恢复端配置现已从全局 SADA 复制 `angular_acceleration[2]`；当前非零临时对照输出未变化。该结果说明恢复端测试初始化尚未重放原探针中的 SADA 运行态，或主失配仍在 Sat backing/三维项组合顺序。该单项修正不构成验收，公开包装继续不接入门禁。

## SADA 当前角/角速度复测

将原入口快照的 `SADA.current_angle=(0,pi)`、`angular_velocity=(pi,0)` 与零角加速度重放到恢复端非零对照后，角加速度和模态加速度位型仍完全不变。该组 SADA 标量状态可暂时排除为主失配直接来源；下一轮聚焦 Sat matrix backing 与两个 3 维项的精确组合语义。

## rsi 真实构造来源（已更正）

`differential_equation` 在 `0x6e23..0x6e31` 调用 `quat_diff(rdi=局部导数, rsi=r13, rdx=r12)`；r12 是传入 quat_diff 的 3 维角速度输入，并于 `0x6e4c` 原样作为 dynamics_flex 的 rsi 传入。因此 rsi 应映射为 3 维 body-rate/角速度输入，不能视为 quat_diff 输出。此前简化包装对 rsi 的 body-rate 映射是正确的；主失配继续位于 Sat backing 与三维组合细节。

## flag1 惯量构型复测

将临时对照由 `Update_sat_inertia_xw(0)` 改为 flag1 后，恢复端角加速度和模态加速度均保持零，而速度复制仍正确，表明当前 flag1 默认模型装配与公开包装之间仍存在配置/描述符初始化缺口。该结果不构成任何金标匹配；flag1 运行态需单独校正后才能比较。

## flag1 内部返回码诊断

直接调用 flag1 配置下的 `dp_flex_dynamics_step` 返回 0，故公开包装的零输出并非错误返回或提前退出，而是当前临时对照的状态/矩阵组合产生的计算结果。后续失配校正需比较原 ELF 与恢复端调用时的具体输入状态和 Sat backing 值。

## 原 J_c_B backing 重放复测

将原 ELF 调用前捕获的 9 个 J_c_B 位型重放到恢复端 flag0 模型后，非零临时对照输出未变化。单独替换惯量矩阵不能解释当前失配；后续重点转向 Sat 柔性矩阵 backing（耦合、模态与反作用映射）及公开入口与内部步骤间的数据流差异。

## Sat 柔性矩阵 backing 采样

扩展原 ELF 探针已采集 `Sat+0x898` 耦合矩阵、`+0x448/+0x4a8/+0x688` 映射块和 `+0x9a0` 相关 3×3 backing 的实际位型；它们均为非平凡运行态数据，且与恢复端临时默认模型存在差异。单独替换 J 或 SADA 标量未改变输出后，后续必须以这些采样值建立逐元素受控矩阵重放比较。

## backing 位型重放输入

已将采样矩阵块的 87 个连续 64 位字保存为 `analysis/time_orbit/dynamics_flex_sat_backing_bits.txt`；按采集顺序包含 3×10 coupling、两组 3×3 映射、一组 3×10 映射和一组 3×3 映射。下一轮受控重放将以该清单而非静态默认模型作为输入。

## 完整模态 A/D runtime backing

原 ELF 探针现已采集 Sat+0xa00 与 Sat+0xd38 的完整 10×10 模态 A/D backing；连同前述耦合与映射块，已规范化为 `analysis/time_orbit/dynamics_flex_full_runtime_bits.txt` 中 356 个 64 位元素。下一阶段以此清单建立受控逐元素重放，不再以静态默认模型替代真实运行态。
