/* 原 ELF 0x1a80..0x1ab2 的算法遥测封装 ABI。 */
#include <stdint.h>

/* 原全局 algorithm_telemetry_size（0x218004）仅由两个 Update wrapper 清零。 */
static int32_t dp_algorithm_telemetry_size;

void Update_Telemetry_Frame_1(void)
{
    dp_algorithm_telemetry_size = 0;
}

void Update_Telemetry_Frame_2(void)
{
    dp_algorithm_telemetry_size = 0;
}

int32_t Set_Algorithm_Telemetry_Frame_1(void)
{
    return 0;
}

int32_t Set_Algorithm_Telemetry_Frame_2(void)
{
    return 0;
}

/* 仅供恢复端比较器观察对应的可见全局计数。 */
int32_t dp_algorithm_telemetry_size_get(void)
{
    return dp_algorithm_telemetry_size;
}

void dp_algorithm_telemetry_size_set_for_test(int32_t value)
{
    dp_algorithm_telemetry_size = value;
}
