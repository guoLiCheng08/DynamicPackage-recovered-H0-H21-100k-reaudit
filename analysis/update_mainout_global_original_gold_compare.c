#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_sensors.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
}

static int compare_blob(const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatch = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatch < 10u) {
                printf("UpdateMainOut byte+0x%zx actual=%02x expected=%02x\n",
                       index, (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatch;
        }
    }
    printf("UpdateMainOut mismatched bytes: %u/%zu\n", mismatch, bytes);
    return mismatch == 0u ? 0 : -1;
}

int main(void)
{
    DpMainTelemetryFrame output = {{0}};
    unsigned char expected_output[DP_MAIN_TELEMETRY_BYTES];
    DpState propagated_state;
    unsigned char core_output[0x148] = {0};
    unsigned index;

    dp_device_measure_globals_reset();
    if (read_blob("gold_dyn_main_step1_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0 ||
        read_blob("gold_dyn_main_step1_gyro.bin", DeviceMeasure.gyro,
                  sizeof(DeviceMeasure.gyro)) != 0 ||
        read_blob("gold_dyn_main_step1_dss.bin", DeviceMeasure.dss,
                  sizeof(DeviceMeasure.dss)) != 0 ||
        read_blob("gold_dyn_main_step1_magmeter.bin", DeviceMeasure.magmeter,
                  sizeof(DeviceMeasure.magmeter)) != 0 ||
        read_blob("gold_dyn_main_step1_gps.bin", &DeviceMeasure.gps,
                  sizeof(DeviceMeasure.gps)) != 0 ||
        read_blob("gold_dyn_main_step1_state.bin", &propagated_state,
                  sizeof(propagated_state)) != 0 ||
        read_blob("gold_dyn_main_step1_out.bin", expected_output,
                  sizeof(expected_output)) != 0) {
        return 1;
    }
    dp_device_measure_globals_relocate();

    /* UpdateMainOut 从执行机构全局读取飞轮及 SADA 反馈。以原输出状态播种这些输入。 */
    dp_device_globals_reset();
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        double omega;
        memcpy(&omega, expected_output + DP_TM_RW_OMEGA + index * sizeof(omega),
               sizeof(omega));
        RWheel[index].add_gaussian_noise_flag = 0u;
        RWheel[index].omega = omega;
    }
    memcpy(&SADA.current_angle[0], expected_output + DP_TM_SADA + 0x00u,
           sizeof(SADA.current_angle[0]));
    memcpy(&SADA.current_angle[1], expected_output + DP_TM_SADA + 0x08u,
           sizeof(SADA.current_angle[1]));
    memcpy(&SADA.angular_velocity[0], expected_output + DP_TM_SADA + 0x10u,
           sizeof(SADA.angular_velocity[0]));
    memcpy(&SADA.angular_velocity[1], expected_output + DP_TM_SADA + 0x18u,
           sizeof(SADA.angular_velocity[1]));

    memcpy(core_output + 0x098u, &propagated_state.position_gci,
           sizeof(propagated_state.position_gci));
    memcpy(core_output + 0x0b0u, &propagated_state.velocity_gci,
           sizeof(propagated_state.velocity_gci));
    memcpy(core_output + 0x0c8u, expected_output + DP_TM_PROPAGATED_PREFIX, 48u);

    /* 此快照未保存原进程 libc PRNG 状态；0x000..0x05f 的三台 STS 噪声输出
     * 由独立固定种子比较器验证。其余 448 字节均不依赖该隐式状态。 */
    UpdateMainOut(&output, core_output);
    return compare_blob(output.raw + DP_TM_STS_VALID,
                        expected_output + DP_TM_STS_VALID,
                        DP_MAIN_TELEMETRY_BYTES - DP_TM_STS_VALID) == 0 ? 0 : 1;
}
