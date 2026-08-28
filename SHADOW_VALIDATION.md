# 纯 C 影子模型验证

## 已实现

- `tools/c_shadow_runtime.c`：独立纯 C 进程，执行 33 维状态积分，发布设备状态和完整 `0x220` 字节主遥测帧。
- `tools/compare_elf_c_state.c`：从正式 ELF 进程的 `y[33]` 读取状态，与纯 C 状态共享帧逐项比较。
- `tools/run_full_shadow_validation.sh`：自动执行 100、1000、10000 步，并生成 TSV 报告。
- `src/dynamic_ipc_telemetry.c`：纯 C 影子使用独立共享内存，不再读取正式 ELF 的 `/sharedvars_example`。

## 执行

只验证纯 C 长跑稳定性：

```sh
make full-shadow-validation
```

同时读取正在运行的正式 ELF 状态：

```sh
ELF_PID=$(pgrep -n -f '/DynamicPackage$') make full-shadow-validation
```

## 输入回放

### 随机数回放

C 模型不加载 ELF、模型 `.so` 或其它模型二进制。随机数由纯 C 的
`src/dynamic_rng.c` 统一提供：未设置回放文件时使用固定种子的本地 `rand()`；
设置 `C_SHADOW_RNG_REPLAY` 时，按顺序读取由测试工具生成的 little-endian
`int32` 随机数文件。文件耗尽或数值超出 `0..RAND_MAX` 会使 C 进程失败，不能
继续生成并判定为通过。

严格的 ELF/C 主遥测等价测试必须让 ELF 侧从启动开始记录每个 `dyn_main` 步的
随机数序列，再将该数据文件交给 C 回放；仅分别调用 `srand()` 不能替代随机数
序列记录。随机数文件属于测试输入，不属于 C 模型运行时依赖。

回放帧位于 `/dev/shm/cfs_test_shadow_input`，包含完整执行机构命令、步长和单调递增序号。纯 C 严格回放模式如下：

```sh
C_SHADOW_REPLAY_REQUIRED=1 C_SHADOW_READY_FILE=/tmp/c_shadow.ready \
  ./build/c_shadow_runtime 0
python3 tools/publish_shadow_input.py 1 --sada-angle 0.1 0.2
```

每发布一个新序号，纯 C 执行且只执行一步，并在状态帧中回写 `input_sequence`。正式 ELF 适配器必须把同一序号和同一命令写入 ELF 的正式输入区，并回传已消费序号；在此之前，运行中的 ELF/C 快照只能用于诊断，不能作为等价通过依据。

正式 ELF 若将 POSIX 共享内存创建后立即解除命名，需要通过其打开的 fd 写入：

```sh
ELF_INPUT_FD=/proc/<ELF_PID>/fd/3 \
  ./build/publish_dual_input 1 0 0 0 0 0 0 0 0 0 0 0 0.1 0.2 0 0
```

`3` 不是固定协议值，必须先查看 `/proc/<ELF_PID>/fd`，确认该 fd 的目标为 ELF 输入共享区。

发布输入后，用测试专用采样器在 `dyn_main` 返回边界抓取 ELF 状态和主遥测：

```sh
./build/trace_elf_dyn_main <ELF_PID> 1
./build/compare_state_frames
```

比较器先检查两帧的 `input_sequence`，再逐字节比较 33 维状态和 `0x220` 字节主遥测。返回非零才是失败。ptrace 会短暂停止正式 ELF，仅适用于隔离测试进程；采样器退出前会恢复断点并 detach。

在双路回放前，先对隔离的正式 ELF 执行完整初始化重置：

```sh
bash tools/reset_elf_runtime.sh <ELF_PID>
```

该操作调用正式 ELF 的 `DynamicDllInit()`，由 ELF 自己继续执行 `dyn_init()`，会同时重建时间、轨道、惯量、设备对象和 `y[33]`。不能只用 ptrace 覆盖 `y[33]`，否则设备内部状态仍可能来自上一轮运行。

报告目录为 `build/validation_reports/`。命令退出码为 0 只表示报告中的项目全部通过；若 ELF 与纯 C 没有同一输入帧、同一积分步或同一初始状态，状态比较必须失败，不能把“都能运行”当作等价。

## 当前边界

纯 C 长跑已经实测 100、1000、10000 步无 NaN。正式 ELF 目前没有向测试接口发布步号、输入帧和 33 维状态快照，因此运行中的一次快照只能用于发现差异，不能证明 100% 替代。完成等价验收还需要：

1. 星务/伪采集输入复制到 ELF 和纯 C 两路，并带同一序号。
2. ELF 和纯 C 每一步发布状态、设备状态、完整遥测及序号。
3. 比较器按序号配对后，连续 100、1000、10000 步逐字段比较。
