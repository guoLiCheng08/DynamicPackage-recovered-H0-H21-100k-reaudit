# H28：dyn_main_array 旧式数组 ABI 的长时域行为等价验证

H28 覆盖独立于主 `dyn_main` 结构化调用约定的旧式数组 ABI。原 ELF 的 `dyn_main_array` 使用 30 个 `double` 初始化数组、16 个 `double` 命令数组、0x1e8 B legacy output 和 0x200 B legacy state。反汇编确认的命令桥接为：`[0..3]` 轮扭矩、`[4..9]` MTQ、`[10]` 经 `cvttsd2si` 转换的 SADA flag、`[11..12]` SADA 双轴命令、`[14]` 推力器状态与 `[15]` 惯量更新标志。

H28 使用有效多执行机构命令：轮扭矩 `(0.005,0,-0.003,0)`、MTQ 通道 0/3 命令 `(0.008,-0.006)`、SADA flag `1` 与双轴 `(0.01,-0.005)`、推力器/惯量 flag 均为 `1`。100/1000 步 legacy output/state 均完成 first/second 原 ELF 双采集，每种长度两块文件逐文件 `cmp` 一致。原 ELF 一直保持 mode `0400` 与 SHA-256 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`。

| 长度 | legacy output SHA-256 | legacy state SHA-256 | 原 ELF 双采集 |
|---:|---|---|---|
| 100 | `94eed78188a46a359e560a4758325eacc3d06b5c87724f61cf0e8b66f370feb5` | `54b0af090a09a37e8a072badf22e0899b5e21067243b1dd8bd7c4f2c43832a5e` | PASS |
| 1000 | `ed7a94197228f313dfe7c131926073c5e52effeac8e7590c32131e6c09ae2776` | `a2eb4eb72813988f6511f47a7d2efcc79ec89b1572c5874c88256f040ab47694` | PASS |

首次差分表现为第 1 步即分叉。H28 额外对 `dyn_init_array` 后及第一帧后的 RWheel、MTQ、Thruster、SADA 建立原 ELF快照；比较时屏蔽进程地址重定位，仅比较有效数值字段、描述符计数和对象内 backing。证据确认恢复端的 `dyn_init` 路径遗漏如下静态设备初态：SADA 的静态角度/限幅；前三个 MTQ 通道的 `400.0` 限幅与安装轴 `(1,0,0)`、`(1,0,0)`、`(0,1,0)`；以及 Thruster force scale `0x3f2d8b2b41cd29ea`、lever `(0,0,0.01)`、force input `(0,-1,0)`。

在每个设备重装阶段按原 ELF顺序补入这些静态值并重建 MTQ 3×6 群组映射后，第一帧后 RWheel、MTQ、Thruster、SADA 的有效字段均为 0-byte mismatch；H28 legacy output/state 随后也在 100 与 1000 步全部逐步通过。

| 恢复端验证 | 输出/状态记录 | 结论 |
|---|---:|---|
| H28 100 步 | 200 条，每条 488 B/512 B | 全部 0-byte mismatch |
| H28 1000 步 | 2000 条，每条 488 B/512 B | 全部 0-byte mismatch |
| `make check-h28-dyn-main-array` | 2 条 PASS | 0 个非零字节块 |
| `make clean && make selftest` | 321 条 PASS | 0 个非零字节块、0 个显式失败 |

H28 现已进入默认严格 `selftest` 和独立 `make check-h28-dyn-main-array` 门禁。它增加了主 `dyn_main` 之外的一条长期公开 ABI 证据链；该结果仍仅限当前 Linux x86-64、固定运行时、严格 C11 与本文件列明的受控数组合同。工程仅限离线研究与仿真。
