# 高偏心率 50 步主遥测争议：交付包与原 ELF重新审计

**审计日期：** 2026-08-25  
**范围：** `H0` 高偏心、邻接日期、全柔性、零设备命令、`srand(12345)` 的 50 步 legacy `dyn_main` 路径。  
**限制：** 本记录仅限离线软件仿真与原 ELF 行为差分，不得用于飞行、控制、实体执行机构或安全关键用途。

## 1. 争议与审核问题

曾出现“高偏心率 50 步场景第 24、37 步各有两个 main telemetry 字节差异，CoreDynamic 和 IPC 一致”的报告。为判断其是否意味着恢复 C 未完全重建，审计按以下问题进行：

1. 已交付 ZIP 是否实际包含相同的比较器、gold 和模型源码；
2. 在 ZIP 的全新解压副本中，从空构建执行相同比较器是否复现；
3. 当前 50 步 gold 是否仍可由原 ELF 调试执行副本双采集；
4. 原 ELF新采集、旧 gold、恢复 C 三者在第 24、37 步及全部 50 步上是否一致；
5. 交付流程是否缺少一个用户可直接调用的场景目标。

## 2. 交付 ZIP 审计

交付 ZIP：`DynamicPackage_recovered_final_H0-H21_100k_git_handoff_20260825T013114Z.zip`。

| 项目 | 结果 |
|---|---|
| ZIP SHA-256 / CRC | 均通过 |
| ZIP 内 Git 提交 | `7c2845856171854542ba69c622cfcb21521370b8` |
| 50 步比较器、50/100 步 main gold、`dynamic_main_bridge.c`、`Makefile` | 与工作区逐字节一致 |
| ZIP 是否包含 `build/` | 否；该目录按设计可由 `make` 重建 |

从 ZIP 全新解压到独立目录后，执行 `make clean && make all`，再以 Makefile 的严格 C11 标志编译并运行 `dyn_main_high_ecc_shadow_adjacent_date_fifty_step_compare.c`。结果为：第 24 步 main `0/544`，第 37 步 main `0/544`，50 步 × 四观察块合计 200 个比较块均为零字节差异。

同一干净 ZIP 解压副本执行完整 `make selftest`：322 个测试可执行项、313 条 PASS、非零字节差异记录 0、显式 FAIL/error 记录 0。

## 3. 原 ELF重新双采集

原始 `input/DynamicPackage.elf` 未被执行或修改；动态调试仅使用 `analysis/debug_runner/DynamicPackage.exec_copy`。重新复制原有 50 步探针，改为独立输出前缀后连续运行两次：

- `dyn_main_high_ecc_shadow_adjacent_date_fifty_step_reaudit_first_probe.gdb`
- `dyn_main_high_ecc_shadow_adjacent_date_fifty_step_reaudit_second_probe.gdb`

两次原 ELF 采集以及既有 legacy gold 的四个逐步主块完全相同：

| 块 | 每步大小 | 50 步大小 | 第一次 vs 第二次 | 第一次 vs 既有 gold | SHA-256（第一次） |
|---|---:|---:|---|---|---|
| CoreDynamic caller-state | 264 B | 13,200 B | PASS | PASS | `fa7f2386c1080fbc603b7570e0356d3efb18d745b154c6325d630bbac2da2b81` |
| main telemetry | 544 B | 27,200 B | PASS | PASS | `0d08ec9a8390aea645de8fe4e3b99abfeb85537b50883331a426d1002d3c90c0` |
| IPC payload | 3,000 B | 150,000 B | PASS | PASS | `de757375e226415fbfce9da073cf9a95161f7a55f6b827bf89d24483335daa8f` |
| global `y[33]` | 264 B | 13,200 B | PASS | PASS | `69db925af7d812665c9d8c16e91d954ee26c697cac84e34201825855dd027668` |

第 24 步和第 37 步的 main 544 B 块也逐步与既有 gold 相同。其 SHA-256 分别为：

| 步骤 | main 544 B SHA-256 |
|---:|---|
| 24 | `8adbe86d3b55c88b7dacc4116ae04eb38f60e78dcd576944e3d2ee3cab820153` |
| 37 | `d64679b01f291929d268f0df22c440ea036b1e898a1a123618188bc92895751d` |

原 ELF 仍为 mode `0400`，SHA-256 为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

## 4. 入口等价检查与根因结论

原 ELF probe 采用 `dyn_init` → 手工状态装配 → `srand(12345)` → legacy `dyn_main(out,state,cmd)` → legacy `sendDynTele(0,out)`。恢复端比较器采用相同 `DynamicInit`、状态、随机种子和 legacy `dyn_main`，并在 main 比较后以恢复的 `dp_send_dyn_tele` 比较 IPC payload。

恢复的 legacy `dyn_main` 入口定义在 `src/dynamic_dyn_main_globals.c`，其顺序是 `UpdateDeviceControl` → `UpdateCoreDynInput` → `CoreDynamic` → `UpdateDeviceMeasure` → `UpdateMainOut`。它是该场景比较器实际调用的路径，不是未链接的 `dynamic_recovered.c` 早期草稿。

**结论：** 在当前证据下，没有发现模型源码行为分叉，也没有发现 ZIP 打包内容损坏或缺失导致的差异。争议报告无法由同一 ZIP、同一源码、同一 gold、同一编译标志、同一原 ELF调试副本或同一入口复现。唯一已确认的交付改进点是：原 Makefile 将该比较器仅嵌入长 `selftest` 配方，用户不易单独复核，容易手工复制错误命令或使用错误二进制。

## 5. 交付流程更正

Makefile 新增明确目标：

```bash
make clean && make check-h0-fifty
```

该目标按默认严格 C11 选项重新编译 `dyn_main_high_ecc_shadow_adjacent_date_fifty_step_compare.c` 并运行 50 步四块逐步比较。修改后，从空构建执行该目标结果为 PASS：第 24、37 步 main 都是 `0/544`，末步四块均为零字节差异。

随后完整 `make clean && make selftest` 仍通过：322 个测试可执行项、313 条 PASS、非零字节差异 0、显式 FAIL/error 0。

## 6. 限定结论

上述结论确认的是这个精确定义的 H0 50 步 legacy 入口场景。它不构成任意连续初态、命令、随机种子、并发时序、未观察对象、其他入口或其他平台的全域等价证明。若未来得到不同的完整初始化、命令、调用入口或输出缓冲布局，应按原 ELF双采集和逐步 bitwise 差分建立新的场景，而不是覆盖本证据。
