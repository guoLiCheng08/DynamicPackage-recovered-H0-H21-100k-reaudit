/* DynamicPackage CoreDynamic 输入布局：由 UpdateCoreDynInput/CoreDynamic 交叉确认。 */
#ifndef DYNAMIC_CORE_LAYOUT_H
#define DYNAMIC_CORE_LAYOUT_H

#include <stddef.h>
#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 原 ELF ABI：rdi/rsi 为未读取保留指针，rdx 为至少 0x74 字节的动态命令帧；
 * 从共享输入 float 索引 0..11 和 u8 索引 0..2 解码字段。 */
void getDynInput(void *reserved_0, void *reserved_1, void *dynamic_input);

/*
 * 字段名称为物理语义候选；偏移、长度和顺序来自直接内存读写。
 * UpdateCoreDynInput: +0x00/+0x18 来自 WheelGroup，+0x30 来自 MTQ_Group，
 * +0x48/+0x60 来自 Thruster，+0x78 由 getSADAangle 填充。
 */
typedef struct {
    DpVec3 wheel_group_vector_0;      /* +0x00 */
    DpVec3 wheel_group_vector_1;      /* +0x18 */
    DpVec3 magnetic_torque_command;   /* +0x30 */
    DpVec3 thruster_vector_0;         /* +0x48 */
    DpVec3 thruster_vector_1;         /* +0x60 */
    DpVec3 sada_angle_or_rate;        /* +0x78 */
} DpCoreDynInput;

_Static_assert(offsetof(DpCoreDynInput, wheel_group_vector_0) == 0x00, "core input +0x00");
_Static_assert(offsetof(DpCoreDynInput, wheel_group_vector_1) == 0x18, "core input +0x18");
_Static_assert(offsetof(DpCoreDynInput, magnetic_torque_command) == 0x30, "core input +0x30");
_Static_assert(offsetof(DpCoreDynInput, thruster_vector_0) == 0x48, "core input +0x48");
_Static_assert(offsetof(DpCoreDynInput, thruster_vector_1) == 0x60, "core input +0x60");
_Static_assert(offsetof(DpCoreDynInput, sada_angle_or_rate) == 0x78, "core input +0x78");
_Static_assert(sizeof(DpCoreDynInput) == 0x90, "core input byte size");

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_CORE_LAYOUT_H */
