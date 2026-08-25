#include "dynamic_core_layout.h"
#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#include <string.h>

void UpdateCoreDynInput(void *core_dynamic_input)
{
    DpCoreDynInput *out = core_dynamic_input;

    if (out == NULL) {
        return;
    }
    /* 3920: WheelGroup +0x08 -> out+0x18；+0x30 -> out+0x00。 */
    memcpy(&out->wheel_group_vector_1, WheelGroup.angular_momentum.data,
           sizeof(out->wheel_group_vector_1));
    memcpy(&out->wheel_group_vector_0, WheelGroup.torque.data,
           sizeof(out->wheel_group_vector_0));
    memcpy(&out->magnetic_torque_command, MTQ_Group.group_moment.data,
           sizeof(out->magnetic_torque_command));
    memcpy(&out->thruster_vector_0, Thruster.force_output.data,
           sizeof(out->thruster_vector_0));
    memcpy(&out->thruster_vector_1, Thruster.torque_output.data,
           sizeof(out->thruster_vector_1));
    /* getSADAangle 只写 +0x78/+0x80，+0x88 由调用方临时对象保留。 */
    out->sada_angle_or_rate.x = SADA.current_angle[0];
    out->sada_angle_or_rate.y = SADA.current_angle[1];
}
