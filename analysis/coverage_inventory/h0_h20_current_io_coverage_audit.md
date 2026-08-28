# H0–H35 当前平台输入输出覆盖审计

**审计日期：** 2026-08-26（主 `dyn_main` 已扩展至 H0–H21、H26、H27、H30、H32；H28 已补齐 `dyn_main_array`，H29/H31 已补齐 `getDeskCommand` 连续与生命周期入口差分，H33 已补齐共享 IPC getter 的无效索引／空输出／自动重开合同，H34 已补齐圆赤道根数启动链主传播，H35 已补齐 `e=0.99` 近抛物高偏心根数启动链主传播）

> 本文量化的是当前 Linux x86-64、当前运行时库、严格 C11 构建和固定受控输入下，恢复 C 与只读原 ELF 已建立 gold 的逐步字节比较范围。它不将有限序列外推为任意输入、任意长度命令、并发或数学全域的一致性证明。

## 1. 基线完整性

| 项目 | 当前值 |
|---|---|
| 原 ELF 文件 | `input/DynamicPackage.elf` |
| 文件权限 | `0400` |
| SHA-256 | `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403` |
| H32–H35 取证状态 | H32：`a=42,000,000 m`、`e=0.75` 安全高偏心根数组合已单步预筛并完成 100／1,000 步四块双采集 PASS；H33：IPC getter 无效索引／空输出／自动重开合同已双采集 PASS；H34：`a=7,000,000 m`、`e=0`、`i=0` 圆赤道根数启动链已完成 100／1,000 步四块双采集 PASS；H35：`a=700,000,000 m`、`e=0.99` 近抛物高偏心根数启动链已完成字段对齐预筛及 100／1,000 步四块双采集 PASS |
| 默认严格构建 | `-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off` |
| 当前重跑门禁 | `make clean && make selftest` 通过；333 条 PASS、0 个错误标记 |
| 默认测试可执行项 | 342 |

## 2. 受控输入序列数量

| 项目 | 数量 | 说明 |
|---|---:|---|
| 已纳入长时域场景 | 22 | H0–H21；每个场景有明确初态、时间、种子、设备/传感器配置及命令时间表 |
| H0–H20 每场景长度 | 2 | 100 步与 1,000 步各一条独立 gold 序列 |
| H21 场景长度 | 4 | 100、1,000、10,000、100,000 步；均使用第二 LCG 命令种子 |
| 长时域 gold 序列 | 46 | H0–H20 的 42 条，加 H21 的 4 条；每条原 ELF gold 均至少双采集并逐文件 `cmp` |
| 每个实现的 `dyn_main` 调用 | 134,200 | H0–H20：21×(100+1,000)=23,100；H21：100+1,000+10,000+100,000=111,100 |
| 输入状态槽位 | 33 个 `double` | 4 四元数 + 3 本体系角速度 + 3 GCI 位置 + 3 GCI 速度 + 20 柔性状态 |
| 设备命令 ABI | 0x78 B | 轮扭矩[4]、MTQ[6]、SADA flag/双轴命令、推力器使能、惯量更新等字段 |

## 3. 已覆盖输入类别

| 输入维度 | 已有有限覆盖 | 代表场景 |
|---|---|---|
| 时间/历法 | 2024 闰日、2025 常规日、2031 跨日/跨年边界 | H0、H2、H6、H11–H16 |
| 轨道 | 近圆 LEO、普通 LEO、高偏心轨道、阴影侧位置 | H0、H1、H2、H6、H11–H16 |
| 姿态/角速度 | 单位四元数、三组替代四元数、非零三轴初始角速度 | H0、H1、H11、H15–H21 |
| 柔性初态 | 零、稀疏非零、完整非零、完整反相、完整交错振幅 | H0、H2、H7、H11–H14、H20–H21 |
| DSS 随机/开关 | 双关闭、仅 DSS0、仅 DSS1、双路自定义、阴影联合路径 | H3、H8–H10、H12、H20–H21 |
| 设备初态 | 默认、预置飞轮角速度/角动量 | H4、H18–H21 |
| 命令时间表 | 零命令、固定脉冲、持续 SADA、极端双轴 SADA、两条 LCG 固定种子有效离散命令 | H1、H4、H5、H7、H13、H18–H21 |
| 固定种子随机 | `srand(1)`、`srand(12345)`；H19/H20 为 LCG `0x13579bdf`，H21 为第二 LCG `0x2468ace1` | H0–H21 |

## 4. 输出观察范围与字节量

| 观察块 | 已覆盖场景 | 每步大小 | 逐步块数 | 逐步字节数 | 结论 |
|---|---:|---:|---:|---:|---|
| caller `CoreDynamic` 前 0x108 | 28/28 | 264 B | 140,800 | 37,171,200 B | 全部主路径逐步 `memcmp` |
| main telemetry | 28/28 | 544 B | 140,800 | 76,595,200 B | 全部主路径逐步 `memcmp` |
| IPC payload | 28/28 | 3,000 B | 140,800 | 422,400,000 B | 全部主路径逐步 `memcmp` |
| global `y[33]` | 28/28 | 264 B | 140,800 | 37,171,200 B | 全部主路径均已建立并逐步比较此块 gold |
| 终态执行机构快照 | H18–H21、H26/H27 | 1,088 B/场景长度 | 98 份 | 15,232 B | RWheel、MTQ、Thruster 与四个三元向量 |
| **合计** | — | — | **568,788 次显式比较操作** | **574,594,185 B** | 含 H28 legacy 两块、H29 IPC 三块、H30 四块、H31 生命周期六块、H32 四块、H33 IPC getter 四块、H34 四块与 H35 四块 gold |

全部结构化主 `dyn_main` 路径现均对 `CoreDynamic + global y + main + IPC = 4,072 B/步` 建立逐步原 ELF gold。28 个主路径的 140,800 个步骤四块比较共计 573,337,600 B，另有 H18–H21 与 H26/H27 的 15,232 B 终态执行机构快照；H28 另以 legacy output/state 两块在 1,100 步比较 1,100,000 B；H29 另以 flags/init-tail/DRC 三块在 1,100 步比较 140,800 B；H31 另以两阶段 flags/init-tail/DRC 六块比较 512 B；H33 另以返回码、无效 float 哨兵、自动重开 float 输出与无效 uint8 哨兵四块比较 41 B。

## 5. H21 新增路径与超长时域结果

H21 固定保持 H20 的时间、近圆轨道、完整 20 维柔性模板、DSS0 噪声开启/DSS1 关闭、`srand(12345)` 和 RWheel0 预置，但将设备命令 LCG 种子从 `0x13579bdf` 换为 `0x2468ace1`。递推式和有限有效命令映射不变，因此它提供了独立的设备命令时间序列，而不会把无界或非法命令输入混入结论。

H21 的 100、1,000、10,000 与 100,000 步均双采集原 ELF；每种长度均产生四个逐步主块及七个终态快照，共 11 个 gold 文件，且两次采集逐文件 `cmp` 一致。恢复端在每一步比较 CoreDynamic 前 264 B、global `y[33]` 264 B、main 544 B 与 IPC payload 3000 B，并在终态比较 7 个设备/向量快照。

| H21 长度 | 原 ELF 双采集 | 恢复端零字节比较记录 | 恢复端结论 | 四块逐步字节 |
|---:|---|---:|---|---:|
| 100 步 | 11 文件逐文件 `cmp` PASS | 411 | bitwise PASS | 407,200 B |
| 1,000 步 | 11 文件逐文件 `cmp` PASS | 4,011 | bitwise PASS | 4,072,000 B |
| 10,000 步 | 11 文件逐文件 `cmp` PASS | 40,011 | bitwise PASS | 40,720,000 B |
| 100,000 步 | 11 文件逐文件 `cmp` PASS | 400,011 | bitwise PASS | 407,200,000 B |

H21-10,000 四个主块 SHA-256：state `b259860daabc172973ecfa9c35a26a02ae3b8455b69d1762ae2b3ce6439df4d9`，global `y` `f8d7363c852b5e765ce25a19c0f9ddc66b96a4d1b18228ae5d9a752b10e8fdb9`，main `5e7019d0f2ceaa9ab3404c29a9ed0999cfa7d9899cfd1bb0566dd4f14c619d5e`，IPC `6f85c20c67b8d429efb3ec4dec2a21bc592f0539678e418239a895e87cc33cd8`。

H21-100,000 四个主块 SHA-256：state `8bc705db223fda6627bef87e4873c001eb4b7627cb6c26f5928292478a1708f8`，global `y` `a2ca5a4a914ba34406a21d67e4fe1d52d91f5448677c530f02f65ce463e8a2ad`，main `b022873d1b3411d72dc643e51154e345665389837705f4b0482297c9c0af4c1b`，IPC `8b69c88e0e15896793808ddf4682a78c34ded3e14bfe364b12037e0b23eeac8b`。

H21 的 10,000 与 100,000 步比较器均已加入默认 Makefile 门禁。从空构建运行 `make clean && make selftest` 后，`make -n selftest` 展开 322 个测试可执行项／644 条编译执行命令；日志中 PASS 行计数 313、错误模式计数 0。

## 6. 当前可作出的结论与不能作出的结论

在 H0–H21、H26、H27、H30、H32、H34、H35 所列结构化 `dyn_main` 输入，以及 H28 旧式数组 ABI、H29 连续共享 IPC、H31 共享 IPC 生命周期与 H33 getter 无效索引／空输出／自动重开输入、种子、平台、严格编译选项及观察块内，恢复 C 与原 ELF 的已建 gold 轨迹均为逐字节一致；其中 H21 的独立第二 LCG 命令序列已连续验证至 100,000 步，H28–H30、H32、H34 与 H35 已验证至 1,000 步，H31 已验证固定无竞争打开/关闭/重开序列，H33 已验证固定 getter 错误返回与输出保持合同。最新空构建默认回归也通过。这是目前有直接双采集原 ELF gold 支持的结论。

不能据此声称所有 33 个连续双精度状态、每一个命令帧数值、所有随机种子、无限长度命令序列、所有未观测对象、并发时序或所有数学边界均已穷尽。H28 已证明主 `dyn_main` 的通过不能自动外推至另一 ABI；后续新场景与入口必须继续遵循“反汇编/接口审计 → 原 ELF 双采集 → 逐步 bitwise 差分 → 默认严格回归”的流程。

## 7. 证据入口

- 覆盖矩阵：`analysis/coverage_inventory/long_horizon_matrix_h0_h1.md`
- 当前严格门禁摘要：`analysis/coverage_inventory/h35_full_selftest_summary_20260826.log`
- H21 输入选择：`analysis/coverage_inventory/h21_alt_lcg_seed_cross_path_plan.md`
- H21 10,000 步原 ELF 采集日志：`analysis/time_orbit/h21_ten_thousand_{first,second}_capture.log`
- H21 100,000 步原 ELF 采集日志：`analysis/time_orbit/h21_hundred_thousand_{first,second}_capture.log`
- H21 恢复端长时域差分：`analysis/coverage_inventory/h21_{ten,hundred}_thousand_recovered_compare.log`
- H33 IPC getter 错误合同：`analysis/coverage_inventory/h33_ipc_getter_invalid_result_20260825.md`
- H34 圆赤道根数主传播：`analysis/coverage_inventory/h34_circular_equatorial_result_20260825.md`
- H35 近抛物高偏心根数主传播：`analysis/coverage_inventory/h35_near_parabolic_result_20260826.md`
- 连续取证：`analysis/time_orbit/h0_thousand_nan_sign_investigation.md`
- 当前源：`src/`；默认门禁：`Makefile`
