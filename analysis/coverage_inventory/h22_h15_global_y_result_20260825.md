# H22：H15 跨年高偏心路径补齐 Global `y` 的结果

## 目标与决策

本轮不立即将 H21 的相同第二 LCG 序列扩展至 1,000,000 步，而是补齐 H15 跨年高偏心路径此前缺失的内部 global `y[33]` 逐步 gold。H15 使用 2031-12-31 23:59:50、`a=26,560,000 m`、`e=0.65`、姿态 `(0.5,0.5,-0.5,0.5)`、固定非零角速度、零设备命令和 `srand(12345)`，此前 100/1000 步仅比较 caller-state、main 和 IPC。

该选择提供新的内部状态观察和不同的跨年/高偏心输入拓扑。相比之下，对 H21 相同输入机械扩展到 1,000,000 步只新增时间长度；四块最终 gold 约 3.79 GiB，两次独立采集约 7.58 GiB，若保留 first/second/final 约 11.38 GiB，当前不作为最高边际价值的优先项。

## 原 ELF 双采集

原 `input/DynamicPackage.elf` 保持 mode `0400`，SHA-256：

```text
312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403
```

动态取证仅运行 `analysis/debug_runner/DynamicPackage.exec_copy`。以独立的 H22 GDB probe 对 100 步和 1000 步各运行两次；每次在 `dyn_main` 返回后追加写出：

- caller-state 前 264 B；
- global `y[33]` 264 B；
- main telemetry 544 B；
- IPC payload 3000 B。

两次独立采集的四个文件均逐文件 `cmp` 一致。为验证 probe 没有改变 H15 输入合同，新增采集中的 state/main/IPC 还与既有 H15 同长度 gold 逐文件 `cmp` 一致。

| 长度 | 新增 global `y` 大小 | 双采集结果 | 与既有 H15 state/main/IPC 交叉核验 | global `y` SHA-256 |
|---:|---:|---|---|---|
| 100 步 | 26,400 B | 四块 `cmp` PASS | 三块 `cmp` PASS | `70b28a78d1d38cf28115173fafb3fa585a97aabafc5655bfb9c15ebc6f70f7d2` |
| 1000 步 | 264,000 B | 四块 `cmp` PASS | 三块 `cmp` PASS | `bf376ad8f7c5a57f9662acf246486058c18519e6d34d0670d4d7babcac65e7cf` |

## 恢复端差分

新增两个 H22 包装器，以共用 H15 比较器的条件编译路径加载独立 state/main/IPC/global-y gold；仅 H22 宏定义启用 `expected_global_y` 分配、读取、每步 `compare_blob` 与释放。既有场景不定义该宏，比较语义不变。

从严格 C11 空构建运行：

```bash
make clean && make check-h15-global-y
make clean && make selftest
```

结果如下：

| 门禁 | 结果 |
|---|---|
| H22 100 步四块比较 | `PASS (bitwise)`，所有块 0-byte mismatch |
| H22 1000 步四块比较 | `PASS (bitwise)`，所有块 0-byte mismatch |
| H22 独立门禁 | 2 条 PASS |
| 完整 selftest | 324 个测试可执行项、315 条 PASS、非零字节差异 0、显式失败 0 |

## 覆盖变化与边界

H15 因此从三块路径升级为四块路径。global `y` 逐步 gold 由 12/22 场景增加到 13/22 场景，新增 1,100 个内部状态步骤和 290,400 B 独有内部状态观察。

本轮新增的 H15 包装器再次比较已有的 state/main/IPC，因而全量 selftest 的总比较运行量增加 1,100 步 × 4 块；这不应被误表述为新的独立输入序列。当前仍未补齐 global `y` 的受控场景是 H3、H4、H5、H7、H8、H9、H10、H16 和 H17。

结论仍然仅适用于当前 Linux x86-64、当前运行时、严格 C11 标志与本页场景合同。它不证明任意输入、命令、随机种子、并发行为、未观察对象、编译器或平台的全域等价。
