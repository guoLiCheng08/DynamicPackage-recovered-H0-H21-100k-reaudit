# STS 有效标志恢复证据（进行中）

## 已确认函数边界 ABI

`Update_STS_ValidFlag` 的实际参数顺序为：

```c
Update_STS_ValidFlag(const DpQuatAbi *attitude,
                     const DpVector *position_gci,
                     const DpVector *sun_vector);
```

最初将位置向量作为首参会在原 ELF 的 `quat_conjugate` 内崩溃；改为上述顺序后，受控动态探针正常返回。

| 项目 | 原 ELF 观察 |
|---|---|
| STS 条目数量 | 3 |
| 条目步长 | `0x170` 字节 |
| 有效标志字段 | 每条目 `+0x138`，32 位整数 |
| 默认构型输入 | 单位四元数、`position=(7000000,0,0)`、`sun=(1,0,0)` |
| 默认构型输出 | 三个有效标志均为 `1` |

## 已验证的完整判定链

`Update_STS_ValidFlag` 对每个 STS 对象将其 `+0x50`、`+0x58` 双精度参数和 `+0x60` 的安装矩阵传递给 `isStarTrackerValid`，并将返回的 32 位整数写回条目 `+0x138`。原 ELF 断点快照已确认其调用和数据流如下：

1. 固定局部视轴 `(0,0,1)` 经安装矩阵 `M_install` 投影为机体系视轴；
2. 对输入姿态四元数取共轭，并经 `quat_att_mat` 构造机体系到 GCI 的姿态矩阵；
3. 该姿态矩阵将机体系视轴变换为 GCI 视轴；
4. 以 `vector2angle(sun_gci, gci_boresight)` 与对象的太阳排除角 `+0x50` 比较；
5. 若夹角不大于阈值，直接返回 `1`；若超过阈值，则返回 `isEarthOut(position_gci, gci_boresight, +0x58) != 1`。

三对象的太阳排除角和地球边界角分别具有相同位模式 `3fe38c35418a5bf6` 与 `3fdbecde5da115a9`。三路安装矩阵的完整九元素位模式已固化在 `sts_valid_flag_gold_compare.c`；在固定位置 `(7000000,0,0)` 和单位姿态下，太阳分别指向 `+X/+Y/+Z/-X` 时，原 ELF 与恢复实现的三路有效标志均为 `(1,1,1)`、`(1,1,0)`、`(1,1,0)`、`(1,1,0)`。

`vector2angle`、`isEarthOut` 和 `quat_att_mat` 均有独立原 ELF 金标，`Update_STS_ValidFlag` 的对象级比较器亦已纳入 `make selftest`。特别是 `quat_att_mat` 必须按原 ELF 所示的 `Xiᵀ·Psi` 矩阵链实现；直接使用代数等价的四元数公式会在一个非对角元素产生一 ULP 差异。
