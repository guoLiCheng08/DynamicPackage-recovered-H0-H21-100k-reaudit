/*
 * 组件级交叉检查：输入来自原 ELF 的受控零命令遥测采样。
 * 只调用已恢复且自包含的 orbit_dynamic()，不链接或调用原 ELF。
 */
#include "../src/dynamic_recovered.h"
#include <stdio.h>

int main(void)
{
    const DpVec3 position_gci = {-525440.25, 6829663.0, 883524.812};
    const DpVec3 velocity_gci = {919.757812, 1037.07983, -7471.61377};
    DpVec3 position_dot = {0.0, 0.0, 0.0};
    DpVec3 velocity_dot = {0.0, 0.0, 0.0};

    orbit_dynamic(&position_dot, &velocity_dot, &position_gci, &velocity_gci);

    printf("{\n");
    printf("  \"input_source\": \"original_elf_zero_command_telemetry\",\n");
    printf("  \"input_gci_position\": [%.9g, %.9g, %.9g],\n",
           position_gci.x, position_gci.y, position_gci.z);
    printf("  \"input_gci_velocity\": [%.9g, %.9g, %.9g],\n",
           velocity_gci.x, velocity_gci.y, velocity_gci.z);
    printf("  \"recovered_position_dot\": [%.9g, %.9g, %.9g],\n",
           position_dot.x, position_dot.y, position_dot.z);
    printf("  \"recovered_central_gravity_acceleration\": [%.9g, %.9g, %.9g]\n",
           velocity_dot.x, velocity_dot.y, velocity_dot.z);
    printf("}\n");
    return 0;
}
