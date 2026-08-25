# `isSunOut` ABI 观察（P3，2026-08-21）

原 ELF `isSunOut` 位于 `0xd730`，函数体为 40 bytes。入口参数按 System V ABI 为：

```c
int isSunOut(const DpVector *first, const DpVector *second, double threshold);
```

其仅交换前两个向量参数后调用 `vector2angle(second, first)`，随后执行 `ucomisd(angle, threshold); seta`。因此可观察返回语义是：

```c
return vector2angle(second, first) > threshold;
```

比较为严格大于；当值相等时返回 0。尚未采集独立原 ELF 金标，亦尚未在恢复端实现或纳入严格回归。
