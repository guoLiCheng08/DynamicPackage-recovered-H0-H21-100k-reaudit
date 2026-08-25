# H0 高偏心 100 步样例 Gold 与后续 AI 操作指南

> **用途限制：** 本说明只用于离线 ELF 行为研究、软件仿真与逐字节差分验证。禁止用于飞行、控制、实体执行机构或安全关键用途。

本仓库附带一个**可运行的代表性 gold 集**：H0 高偏心、邻接日期、全柔性、零设备命令、`srand(12345)` 的 100 步 legacy `dyn_main` 路径。它的作用是让后续 AI 能直接验证一个真实的端到端逐步 bitwise 场景，同时避免把 10,000/100,000 步的大轨迹提交到普通 Git。

## 1. 本样例证明什么

| 项目 | 固定值 |
|---|---|
| 调用入口 | legacy `dyn_main(main_output, core_dynamic_output, device_command)` |
| 初始化 | `DynamicInit` / 原 ELF `dyn_init` 的等价场景合同 |
| 日期时间 | 2031-12-30 23:59:50 |
| 积分步长 | 0.1 s |
| 轨道根数 | `a=26,560,000 m`、`e=0.65`，其余根数为非零固定值 |
| 初始姿态 | `[0.5, -0.5, 0.5, 0.5]` |
| 初始位置/速度 | `[-12e6, 65e6, 28e6] m` 与 `[0, 2000, 0] m/s` |
| 柔性状态 | 20 个固定、非零且正负交替的分量 |
| 设备命令 | 零命令帧 |
| 随机 | `srand(12345)`，在 `dyn_main` 前调用 |
| 步数 | 100 |
| 每步观察块 | caller-state 264 B、main telemetry 544 B、IPC payload 3000 B、global `y[33]` 264 B |

该场景已对原 ELF 独立采集两次，并对恢复 C 的 100 步 × 4 块观察进行逐字节比较。它是**有限输入合同**的证据，不能外推为任意初态、命令、随机种子、入口、并发行为或平台的全域等价保证。

## 2. 仓库中包含的样例 Gold

| 文件类别 | 文件数 | 总大小 | 作用 |
|---|---:|---:|---|
| H0 100 步主轨迹 | 4 | 407,200 B | 每步 state/main/IPC/global `y`。 |
| 传感器初始化 | 4 | 4,016 B | gyro、magmeter、STS、DSS 的初始全局测量状态。 |
| 设备初始化 | 14 | 1,552 B | 飞轮、MTQ、SADA、推力器和映射/力矩初始对象。 |
| 合计 | 22 | 412,768 B | 可由本仓库的 H0 100 步比较器读取。 |

H0 100 步四个主轨迹的 SHA-256：

| 文件 | SHA-256 |
|---|---|
| `gold_high_ecc_shadow_adjacent_date_hundred_step_state.bin` | `5e639f9abfd128cf818f4a882b1736b78373268695c8a3657f9cc737d15973fa` |
| `gold_high_ecc_shadow_adjacent_date_hundred_step_out.bin` | `4e13819aa227406c24403545b722ef80f47384b759ec0c6f7e828eed90b7e9fb` |
| `gold_high_ecc_shadow_adjacent_date_hundred_step_ipc_payload.bin` | `4e41a1906b21c9b05be62a0542800ed72c120d4e257e5a33bcdc791a35e36e65` |
| `gold_high_ecc_shadow_adjacent_date_hundred_step_global_y.bin` | `ac1f6852ad47f5dc3b6c94c221ecb31171de770cfb77357d1cbbac0cd0ccd948` |

## 3. 立即可执行的样例验证

在仓库根目录运行：

```bash
make clean && make check-h0-hundred
```

该目标按严格 C11 选项编译 `analysis/dyn_main_high_ecc_shadow_adjacent_date_hundred_step_compare.c`，读取本样例的 22 个 `.bin` 文件，并运行 100 步逐字节比较。预期末行：

```text
dyn_main high-eccentric shadow adjacent-date alternate-quaternion full-flex hundred-step original-ELF compare: PASS (bitwise)
```

若失败，比较器会打印至少以下信息：场景名、步号、观察块、字节偏移、actual 字节和 expected 字节。不得通过改写 gold 来“修复”失败。

## 4. 后续 AI：新增场景的标准流程

### 4.1 先写场景合同

新场景开始前，在 Markdown 计划中明确并冻结：入口函数、对象 ABI 布局、初始时间/轨道/姿态/状态、随机种子、命令序列、设备状态、IPC backing、步数和观察块。输入合同不完整时，不得声称差分结果具有可重复性。

优先选择与 H0–H21 有实质独立性的路径，例如：不同固定命令种子、日期/地影边界、传感器噪声组合、设备状态转换、轨道退化分支或不同 ABI 包装入口。不要随机修改多个维度后再猜测差异来源。

### 4.2 从原 ELF 建立 gold

1. **保持 `input/DynamicPackage.elf` 只读。** 先核验 mode `0400` 与 SHA-256：

   ```bash
   stat -c 'mode=%a size=%s' input/DynamicPackage.elf
   sha256sum input/DynamicPackage.elf
   ```

2. **只使用隔离调试执行副本。** 原 ELF 动态取证只能运行 `analysis/debug_runner/DynamicPackage.exec_copy`。

3. **复制 probe，不覆盖旧文件。** 从最接近的 `analysis/time_orbit/*.gdb` 模板复制为新 probe；为第一次和第二次采集分别使用 `*_first_probe.gdb`、`*_second_probe.gdb`，所有输出采用独立的场景前缀。

4. **固定采集顺序。** probe 必须固定初值和随机种子，调用原 ELF 的初始化/目标入口/IPC 入口，并按每步顺序写出 state、main、IPC 和适用的 global `y`。不要改变写出顺序、缓冲区尺寸或 `srand` 调用位置。

5. **独立运行两次。** 以两个全新原 ELF 进程运行 probe：

   ```bash
   gdb -q -nx -batch -x <first_probe.gdb>  > <first_capture.log>  2>&1
   gdb -q -nx -batch -x <second_probe.gdb> > <second_capture.log> 2>&1
   ```

6. **逐文件验证确定性。** 对每个观察块执行 `cmp`，并记录大小与 SHA-256：

   ```bash
   cmp <first_state.bin> <second_state.bin>
   cmp <first_main.bin> <second_main.bin>
   cmp <first_ipc.bin> <second_ipc.bin>
   cmp <first_global_y.bin> <second_global_y.bin>
   sha256sum <first_*.bin>
   ```

   只要任一文件不一致，gold 不可接受。先分析原 ELF 的运行条件、未初始化对象、随机源、环境依赖或 probe 写出顺序；不要继续修改恢复 C。

### 4.3 建立恢复端比较器

1. 从最接近的 `analysis/*_compare.c` 复制为新比较器；保留 `compare_blob` 的逐字节语义。
2. 只替换场景合同、独立 gold 文件名和步数；不能把 bitwise 比较改成 epsilon 比较，不能用代数重写掩盖 qNaN/累加顺序差异。
3. 比较器必须在每一步比较所有已定义观察块，并在失败时保留第一分叉信息。
4. 将比较器加到 `Makefile` 的明确目标和 `selftest`。先从空构建运行新目标，再运行完整回归：

   ```bash
   make clean && make <new-scenario-target>
   make clean && make selftest
   ```

### 4.4 定位真实差异

若恢复 C 与 gold 首次不一致：

1. 记录最早步号、观察块、字节偏移、actual/expected 字节；
2. 缩短重放到首次分叉附近，不修改 gold；
3. 从调用顺序、对象布局、初始化、时间/环境、RK4 中间值、传感器/设备状态、遥测封包中逐层定位；
4. 只实施有原 ELF 静态或动态证据支持的最小修复；
5. 修复后重新跑原场景与全量回归，更新矩阵和证据说明。

## 5. Git 与归档规则

| 内容 | 普通 GitHub 仓库 | 完整交接包 |
|---|---|---|
| 源码、ELF、probe、比较器、场景说明 | 必须保留 | 保留 |
| H0 100 步样例 gold（约 403 KiB） | 可以保留 | 保留 |
| 新场景小型样例 gold | 可酌情保留 | 保留 |
| 10,000/100,000 步大轨迹 | 不提交普通 Git | 必须保留，附 SHA-256 |
| 每次运行日志 | 不逐次提交 | 仅关键最终证据保留 |
| `build/` | 不提交 | 不提交 |

大 gold 的目的不是每次构建都重新生成，而是作为原 ELF 行为的固定参考。完整集应放在带 SHA-256 的 ZIP/TAR 交接包中；轻量仓库只保留方法、脚本、比较器、清单和少量可运行样例。

## 6. 交接时必须说明的边界

后续 AI 或维护者应始终说明：当前结论是当前 Linux x86-64、当前运行时、当前严格构建、精确定义输入合同下的有限逐步 bitwise 一致性证据。不能承诺任意输入、任意平台或未采集路径的 100% 全域等价。
