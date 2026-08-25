# `UpdateSatellite` ABI 观察（P3，2026-08-21）

原 ELF `UpdateSatellite` 位于 `0xc5a0`，完整函数体仅两个字节：`f3 c3`，即 `repz ret`。它不读取参数、不访问全局对象，也不写回任何内存；System V ABI 下等价于无参 `void` 空操作。

恢复端在 `dynamic_satellite_globals.c` 中加入：

```c
void UpdateSatellite(void) { }
```

并已在严格 C11 构建下通过。

## 状态快照采集边界

已建立使用原 ELF 运行时 PIE 基址推导 `y`、`t` 和 `step_time` 的 GDB 探针。当前 GDB 对本样本局部/无调试类型全局的 generic-pointer 解引用仍报错，尚未获得独立的调用前后内存二进制快照。因此，当前**仅机器码级空操作语义已确认**；尚未将该导出计入严格函数级 PASS 计数，也未声称动态全局状态快照已经通过。

> 后续可采用寄存器注入式跳转或由一个链接原 ELF 的专用 C 驱动程序读取全局地址，以补全状态快照门禁。

补充复核：原 ELF `objdump --disassemble=UpdateSatellite` 再次确认唯一指令序列为 `f3 c3`；恢复静态库 `nm -g build/libdynamicpackage_recovered.a` 已显示全局 `T UpdateSatellite` 符号。该确认只覆盖机器码与链接可见性，不替代待补的动态前后状态快照。
