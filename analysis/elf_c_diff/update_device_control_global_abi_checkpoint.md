# UpdateDeviceControl 全局 ABI 检查点

> **范围限制。** 本记录只证明下述两个受控原 ELF 快照下的逐字节行为一致；不代表任意设备状态、随机噪声路径或完整输入空间的等价性。所有工作仅用于离线仿真研究。

## ABI 与调用序列审计

原 ELF `UpdateDeviceControl` 位于 `0x3840`。入口 ABI 为 `rdi` 指向 0x78 字节命令帧、`xmm0` 传入 SADA 步进缩放值。反汇编确认的调用顺序为：`Update_sat_inertia_xw(command+0x74)`、`SetWheelAcc(command+0x08)`、`UpdateWheel(step_time)`、`SetMomentMTQ(command+0x28)`、`UpdateMagMoment`、`SetThrusterWorkStatus(command+0x70)`、`UpdateThruster`、`SetSADA(command+0x58/+0x60)`、`drive_SADA_once(xmm0)`。

恢复端将同名全局适配器放在 `src/dynamic_device_globals.c`，保留 `src/dynamic_devices.c` 中已经金标验证的可重入执行机构内核。这样不会将新的全局符号依赖引入旧的函数级回归链接目标。

| 对象 | 已审计布局/行为 | 当前适配器输出 |
|---|---|---|
| `RWheel[4]` | 4 × 0x78，命令扭矩、角速度、动量、力矩 | 完整 480 字节比较 |
| `MTQ[6]` | 6 × 0x38，限幅后的通道磁矩 | 完整 336 字节比较 |
| `WheelGroup` | H、力矩和 3×4 映射 descriptor | H 与力矩 backing 比较 |
| `MTQ_Group` | 3D 输出、6D 通道和 3×6 映射 descriptor | 输出与通道 backing 比较 |
| `Thruster` | 输入缩放、力臂叉乘 | 输出力与输出力矩 backing 比较 |
| `SADA` | 0x68 命令、角、角速度、角加速度状态 | 完整 104 字节比较 |
| `J_c_B/J_c_B_inv` | 惯量旗标副作用 | 两个 9-double 数组比较 |

## 原 ELF 金标

金标通过 GDB 在只读样本的可执行调试副本上采集。探针先以相同的预置设备状态、单位轴映射、推进器力臂/输入、SADA 限幅和 0x78 命令帧建立输入，再调用原同名接口并转储二进制 post-state。

| 场景 | 命令惯量旗标 | 验证结果 |
|---|---:|---|
| `controlled_input` | 1 | 11 类可观察 payload 全部 0 字节差异 |
| `flag0_input` | 0 | 11 类可观察 payload 全部 0 字节差异 |

`flag=1` 的原 ELF 分支不会沿用 DynamicInit 写入的当前惯量，而是恢复到静态 3×3 基线。恢复端使用原 ELF 金标提取的 9 个 IEEE-754 位模式并沿用 `inv_CAL_M3` 等价实现生成逆矩阵。`flag=0` 通过既有 `dp_load_sat_inertia_flag0_static` 的完整静态矩阵写入路径更新全局 Sat/J/Jinv。

## 持续回归

以下比较器已进入 `make CC=gcc AR=ar selftest`，任一字节不一致都会使目标失败：

| 比较器 | 金标目录 |
|---|---|
| `update_device_control_global_flag1_gold_compare` | `analysis/golden/update_device_control_global_abi/controlled_input/` |
| `update_device_control_global_flag0_gold_compare` | `analysis/golden/update_device_control_global_abi/flag0_input/` |

发布后严格全量自检通过：历史 **64 条 PASS** 标签保持通过，且两组新增全局比较器中每个列出的 payload 都报告 `mismatched bytes: 0/...`。

## 尚未覆盖的边界

仍需通过新增原 ELF 金标覆盖设备默认初始化对象、非单位安装映射、噪声标志与随机采样、飞轮饱和边界、推进器关闭路径、非零群组累积状态、不同 SADA 步长和未审计惯量旗标值。`dyn_init` 包装层及设备测量/主输出仍是后续阶段任务。
