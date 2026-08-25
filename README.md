# DynamicPackage：离线行为等价重建（轻量 GitHub 镜像）

> **用途限制：** 本仓库仅用于离线二进制研究、软件仿真、代码审阅与可审计差分验证。不得用于飞行、控制、实体执行机构或任何安全关键用途。

本仓库是 `DynamicPackage.elf` 的 C11 行为等价重建的**轻量协作镜像**。它包含恢复源码、原 ELF、GDB 采集 probe、恢复端比较器、设计文档、场景说明和 gold 管理方法；它故意不包含大体积的原 ELF gold 轨迹、重复运行日志和 `build/`。

## 内容范围

| 内容 | 是否包含 | 说明 |
|---|---|---|
| `src/`、头文件、`Makefile` | 是 | 严格 C11 恢复实现与构建规则。 |
| `input/DynamicPackage.elf` | 是 | 只读原始 Linux x86-64 PIE ELF 基线。 |
| `analysis/**/*.c`、`*.gdb`、`*.md`、`*.py` | 是 | probe、比较器、分析取证、覆盖矩阵和生成器。 |
| 最终 Microsoft Word/PDF 文档 | 是 | 单星模型、公式、详细设计和验证边界。 |
| 代表性 H0 100 步 `.bin` gold | 是 | 22 个文件、约 403 KiB；可运行 H0 100 步逐字节样例。 |
| 大体积 `.bin` gold（10k/100k）及重复 `.log` 与 `build/` | 否 | 完整交接包保存 gold；日志/build 可重新生成，不作为协作源码内容提交。 |

完整 gold 的采集、双采集确定性、日常比较和 GitHub 归档边界，请阅读：

- [`analysis/coverage_inventory/GOLD_GENERATION_AND_GITHUB_ARCHIVING.md`](analysis/coverage_inventory/GOLD_GENERATION_AND_GITHUB_ARCHIVING.md)
- [`analysis/coverage_inventory/high_ecc_fifty_step_delivery_reaudit_20260825.md`](analysis/coverage_inventory/high_ecc_fifty_step_delivery_reaudit_20260825.md)
- [`analysis/coverage_inventory/H0_HUNDRED_SAMPLE_GOLD_AND_AI_HANDOFF.md`](analysis/coverage_inventory/H0_HUNDRED_SAMPLE_GOLD_AND_AI_HANDOFF.md)

## 构建

当前工程使用严格 C11 构建选项：

```bash
make clean
make all
```

默认编译选项包括：

```text
-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off
```

## Gold 与差分测试

gold 不是每次测试生成的文件。它是在新增场景或争议复核时，从隔离的原 ELF 调试执行副本以固定输入独立采集两次，并逐文件 `cmp` 后接受的参考轨迹。

本轻量镜像附带 H0 高偏心 100 步代表性 gold，因此可运行该场景的独立逐字节比较；但未附带全量 gold，不能单独声称可运行完整 `make selftest`。要进行全量逐步 bitwise 验证，请使用经过 SHA-256 校验的完整交接 ZIP，解压后执行：

```bash
make clean && make selftest
```

高偏心 H0 100 步代表性样例的独立复核入口为：

```bash
make clean && make check-h0-hundred
```

该命令读取本仓库已附带的 22 个小型 gold 文件，比较每步 CoreDynamic caller-state、main telemetry、IPC payload 和 global `y[33]`；不重新生成 gold。H0 50 步与全量场景仍需要完整交接包中的对应 gold。

## 当前验证边界

当前证据覆盖 H0–H21 的受控场景，以及 H21 的 10,000/100,000 步交叉序列。其含义是：在当前 Linux x86-64、当前运行时、严格构建和精确定义输入合同下，已建立 gold 的路径实现逐步、逐字节比较通过。

这不是对任意初态、任意连续命令、任意随机种子、并发行为、未观察对象或其他平台的全域等价保证。任何新路径都应遵循：原 ELF 双采集 → `cmp` 确定性 → 恢复端逐步 bitwise 比较 → 严格回归 → 记录场景合同与哈希。

## 原 ELF 完整性

`input/DynamicPackage.elf` 必须保持只读。当前基线 SHA-256：

```text
312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403
```

动态取证仅使用 `analysis/debug_runner/DynamicPackage.exec_copy`，不执行或修改原始输入 ELF。

## 完整交接资产

完整交接包包含本镜像省略的 gold 二进制、历史回归日志和完整本地 `.git` 历史。使用完整包时，请优先核验其 SHA-256、成员清单、原 ELF 哈希和 Git 提交，然后再运行全量差分测试。

## 许可证与披露

仓库仅保留行为等价重建所需的恢复源码和研究证据，不声称拥有原 ELF 的原始源码版权。使用者应自行确认其对输入二进制、相关数据和输出的合法使用权。
