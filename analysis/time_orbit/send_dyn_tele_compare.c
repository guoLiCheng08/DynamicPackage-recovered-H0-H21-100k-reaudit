#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

static void put_f64(uint8_t *raw, unsigned offset, double value)
{
    memcpy(raw + offset, &value, sizeof(value));
}

static void put_i32(uint8_t *raw, unsigned offset, int32_t value)
{
    memcpy(raw + offset, &value, sizeof(value));
}

static uint32_t f32_bits_at(const DpIpcSharedFrame *frame, unsigned index)
{
    uint32_t result;
    memcpy(&result, frame->raw + DP_IPC_FLOAT_BASE + index * 4u, sizeof(result));
    return result;
}

static uint32_t f32_bits(double value)
{
    float converted = (float)value;
    uint32_t result;
    memcpy(&result, &converted, sizeof(result));
    return result;
}

static int require_float(const DpIpcSharedFrame *frame, unsigned index, double expected)
{
    if (f32_bits_at(frame, index) != f32_bits(expected)) {
        printf("FAIL float[%u] actual=%08x expected=%08x\n", index,
               f32_bits_at(frame, index), f32_bits(expected));
        return 1;
    }
    return 0;
}

static int require_byte(const DpIpcSharedFrame *frame, unsigned index, uint8_t expected)
{
    const uint8_t actual = frame->raw[DP_IPC_U8_BASE + index];
    if (actual != expected) {
        printf("FAIL byte[%u] actual=%u expected=%u\n", index, actual, expected);
        return 1;
    }
    return 0;
}

int main(void)
{
    DpMainTelemetryFrame telemetry;
    DpIpcSharedFrame frame;
    unsigned group;
    unsigned element;

    memset(&telemetry, 0, sizeof(telemetry));
    memset(&frame, 0xa5, sizeof(frame));
    for (group = 0u; group < 3u; ++group) {
        put_i32(telemetry.raw, DP_TM_STS_VALID + group * 4u, (int32_t)(10u + group));
        for (element = 0u; element < 4u; ++element)
            put_f64(telemetry.raw, DP_TM_STS_QUAT_0 + group * 0x20u + element * 8u,
                    100.0 + (double)(group * 4u + element));
    }
    for (element = 0u; element < 3u; ++element) {
        put_f64(telemetry.raw, DP_TM_GYRO_0 + element * 8u, 200.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_GYRO_1 + element * 8u, 210.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_MAGMETER_0 + element * 8u, 300.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_MAGMETER_1 + element * 8u, 310.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_GPS_VALUE + element * 8u, 400.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_GPS_VALUE + (element + 3u) * 8u, 410.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_POSITION_ECEF + element * 8u, 500.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_VELOCITY_ECEF + element * 8u, 510.0 + (double)element);
    }
    for (element = 0u; element < 4u; ++element) {
        put_f64(telemetry.raw, DP_TM_DSS_VALUE + element * 8u, 220.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_RW_OMEGA + element * 8u, 320.0 + (double)element);
        put_f64(telemetry.raw, DP_TM_SADA + element * 8u, 330.0 + (double)element);
    }
    put_i32(telemetry.raw, DP_TM_DSS_VALID, 20);
    put_i32(telemetry.raw, DP_TM_DSS_VALID + 4u, 21);
    for (element = 0u; element < 6u; ++element) {
        put_i32(telemetry.raw, DP_TM_GPS_FLAG + element * 4u, (int32_t)(30u + element));
        put_f64(telemetry.raw, DP_TM_ORBIT_ELEMENTS + element * 8u, 600.0 + (double)element);
    }
    put_f64(telemetry.raw, DP_TM_ORBIT_ELEMENTS + 0x30u, 605.0);
    dp_send_dyn_tele(&frame, &telemetry, 0x41u, 0x42u, 0x43u, 0x44u, 0x45u);

    for (group = 0u; group < 3u; ++group) {
        if (require_byte(&frame, group + 3u, (uint8_t)(10u + group)) != 0) return 1;
        for (element = 0u; element < 4u; ++element)
            if (require_float(&frame, 12u + group * 4u + element,
                              100.0 + (double)(group * 4u + element)) != 0) return 1;
    }
    if (require_byte(&frame, 6u, 0x41u) || require_byte(&frame, 7u, 0x42u) ||
        require_byte(&frame, 8u, 20u) || require_byte(&frame, 9u, 21u) ||
        require_byte(&frame, 10u, 0x43u) || require_byte(&frame, 11u, 0x44u) ||
        require_byte(&frame, 12u, 78u) || require_byte(&frame, 13u, 31u) ||
        require_byte(&frame, 18u, 0x45u) || require_byte(&frame, 19u, 0x45u)) return 1;
    for (element = 0u; element < 3u; ++element) {
        if (require_float(&frame, 24u + element, 200.0 + (double)element) ||
            require_float(&frame, 27u + element, 210.0 + (double)element) ||
            require_float(&frame, 34u + element, 300.0 + (double)element) ||
            require_float(&frame, 37u + element, 310.0 + (double)element) ||
            require_float(&frame, 44u + element, 400.0 + (double)element) ||
            require_float(&frame, 47u + element, 410.0 + (double)element) ||
            require_float(&frame, 50u + element, 500.0 + (double)element) ||
            require_float(&frame, 53u + element, 510.0 + (double)element)) return 1;
    }
    for (element = 0u; element < 4u; ++element) {
        if (require_float(&frame, 30u + element, 220.0 + (double)element) ||
            require_float(&frame, 40u + element, 320.0 + (double)element)) return 1;
    }
    if (require_float(&frame, 62u, 330.0) || require_float(&frame, 63u, 331.0) ||
        require_float(&frame, 219u, 332.0) || require_float(&frame, 220u, 333.0)) return 1;
    for (element = 0u; element < 6u; ++element)
        if (require_float(&frame, 56u + element, 600.0 + (double)element) ||
            require_byte(&frame, 12u + element, (uint8_t)(element == 0u ? 78u : 30u + element))) return 1;
    puts("sendDynTele shared-frame mapping compare: PASS");
    return 0;
}
