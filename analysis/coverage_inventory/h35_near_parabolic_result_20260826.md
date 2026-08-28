# H35：近抛物高偏心根数启动链主传播

**日期：** 2026-08-26

**范围：** 当前 Linux x86-64、当前运行时、默认严格 C11 构建；仅离线研究、受控仿真与回归。

H35 补充 H32 `e=0.75` 高偏心路径，验证一个仍在椭圆域内、但更接近抛物边界的启动链根数合同：`a=700,000,000 m`、`e=0.99`、`i=0.85 rad`、RAAN `2.30 rad`、近地点幅角 `1.10 rad`、平近点角 `0.37 rad`。其近地点为 `7,000,000 m`、远地点为 `1,393,000,000 m`、`sqrt(1-e²)=0.14106735979665884425`。正式 `input/DynamicPackage.elf` 始终保持 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`；动态调试只针对执行副本。

## 预筛与数据类型校正

启动顺序固定为 `DynamicDllInit → 覆写根数 → dyn_init → srand(1) → dyn_main`；caller `core` 置零、设备命令为零、共享帧为新的零初始化映射。首轮粗略扫描曾把 main telemetry offset `0x1e0` 的 double 轨道根数数组拆为 float 半字，产生伪 `NaN`。经 `DP_TM_ORBIT_ELEMENTS=0x1e0` 布局确认后，H35 改为仅扫描对齐的 double 状态与已知 main double 字段。校正检查显示初始化 `y`、一步 caller state、global `y` 和 main 已知 double 字段均无 `NaN/Inf`；main 的首四个轨道根数为有限值 `a=699999999.9992301`、`e=0.9900000000017579`、`i=0.849999999922629`、RAAN `2.2999999998647374`。因此该候选获准进入长期双采集。

## 双采集与恢复端逐步差分

100 步与 1000 步各使用 first/second 独立原 ELF probe。每一步导出 caller `CoreDynamic` 前 264 B、global `y[33]` 264 B、main telemetry 544 B 和 IPC payload 3000 B。两个长度的八条 gold 流均逐文件 `cmp` PASS。恢复端以同一启动链和根数合同逐步比较：100 步为 400 条比较、1000 步为 4000 条比较，全部 0-byte mismatch。

| 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 | 恢复端 |
|---:|---|---|---|---|---|
| 100 | `0c07c099320b234409a4d75cda5ff388f0c9a9e23a0bdee462722c9e02894031` | `e86e101b23050c4f075cffcd85c0dc975a0b5f03e2bda3ac025d2ac75bc416b0` | `a18c69872d190a77904a69752af68bc740134e91581920d65711311574729b71` | `4502751ebb629d903f9b296e20c0da824d0cabe52a97828d8983f4373aac925e` | 400 条 PASS |
| 1000 | `f0ffe3cde232aed45d8aae14ba00d907f678ae094bfc0d0db56f01cd8b6be0a7` | `f5275542a1219837f9c9ac31cc3c3a01e64dbf392974d57b1127129c48c4df41` | `5e9ba0e34731780c4c8cbb7deef1415f195430d2ed217cbdc662fce84daf535e` | `c556309e04234746be5680eccbadb02d5f2b452197080a9f6f8a44d1523b47d9` | 4000 条 PASS |

原 ELF 预筛 probe 为 `dyn_main_h35_near_parabolic_prescreen_probe.gdb`；100/1000 步 first/second probes 与恢复端 `analysis/dyn_main_h35_near_parabolic_{hundred,thousand}_step_compare.c` 已一并保存。`make check-h35-near-parabolic` 已通过并加入默认 `selftest`。

从空构建运行 `make clean && make selftest` 后，默认门禁为 342 个测试可执行项、369 条编译调用、333 条 PASS、非零 mismatch 计数 0、显式失败计数 0。可提交摘要日志为 `analysis/coverage_inventory/h35_full_selftest_summary_20260826.log`；不提交重复的完整逐步输出，以控制版本库体积。

## 结论与边界

H35 证明：仅在列明的 `e=0.99` 根数、零命令、固定随机种子、当前 Linux x86-64、当前运行时和严格 C11 构建下，恢复 C 与原 ELF 在 100/1000 步的 state、global `y`、main 和 IPC 四块逐字节一致。它不证明 `e=1`、双曲根数、其他近抛物相位、任意半长轴、任意时长、所有异常输入、并发、其他平台或其他编译器的行为。

工程只限离线研究、教学、受控仿真与回归。不得用于真实卫星飞行、姿态控制、实体执行机构、任务决策或任何安全关键用途。
