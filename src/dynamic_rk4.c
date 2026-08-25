#include "dynamic_recovered.h"

#include <stddef.h>
#include <math.h>
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
#include <stdio.h>
#endif

/* 保持原 SSE 指令的左操作数：qNaN 符号传播属于可观察 ABI。 */
static double dp_rk4_mul_left(double left, double right)
{
    return left * right;
}

static double dp_rk4_add_left(double left, double right)
{
    return left + right;
}

/*
 * 由 RK4_Intergrator 0x5e90..0x6321 逐循环恢复。原函数保存的并非 raw k，
 * 而是 h*k1、h*k2、h*k3、h*k4；最终按 k4 + 2*k3 + 2*k2 + k1 累加，
 * 乘 1/6 后才加回 y。不要改写为代数等价的融合表达式。
 */
void dp_rk4_step_33(double state[DP_STATE_DIM], double *time, double step,
                    DpDerivative33 derivative, void *context)
{
    double derivative_buffer[DP_STATE_DIM] = {0.0};
    double h_k1[DP_STATE_DIM] = {0.0};
    double h_k2[DP_STATE_DIM] = {0.0};
    double h_k3[DP_STATE_DIM] = {0.0};
    double h_k4[DP_STATE_DIM] = {0.0};
    double trial[DP_STATE_DIM];
    double weighted[DP_STATE_DIM];
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    double trace_trial1[DP_STATE_DIM];
    double trace_trial2[DP_STATE_DIM];
    double trace_trial3[DP_STATE_DIM];
#endif
    double half_step;
    size_t index;

    derivative(*time, state, derivative_buffer, context);
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        h_k1[index] = dp_rk4_add_left(dp_rk4_mul_left(derivative_buffer[index], step),
                                      h_k1[index]);
    }

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = state[index];
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = dp_rk4_add_left(dp_rk4_mul_left(h_k1[index], 0.5),
                                       trial[index]);
    }
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    for (index = 0u; index < DP_STATE_DIM; ++index) trace_trial1[index] = trial[index];
#endif
    half_step = step * 0.5;
    derivative(*time + half_step, trial, derivative_buffer, context);
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        h_k2[index] = dp_rk4_add_left(dp_rk4_mul_left(derivative_buffer[index], step),
                                      h_k2[index]);
    }

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = state[index];
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = dp_rk4_add_left(dp_rk4_mul_left(h_k2[index], 0.5),
                                       trial[index]);
    }
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    for (index = 0u; index < DP_STATE_DIM; ++index) trace_trial2[index] = trial[index];
#endif
    derivative(*time + half_step, trial, derivative_buffer, context);
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        h_k3[index] = dp_rk4_add_left(dp_rk4_mul_left(derivative_buffer[index], step),
                                      h_k3[index]);
    }

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = state[index];
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        trial[index] = dp_rk4_add_left(trial[index], h_k3[index]);
    }
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    for (index = 0u; index < DP_STATE_DIM; ++index) trace_trial3[index] = trial[index];
#endif
    derivative(*time + step, trial, derivative_buffer, context);
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        h_k4[index] = dp_rk4_add_left(dp_rk4_mul_left(derivative_buffer[index], step),
                                      h_k4[index]);
    }
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    {
        FILE *trace = fopen("/tmp/dp_rk4_stage_trace.bin", "ab");
        if (trace != NULL) {
            (void)fwrite(h_k1, sizeof(h_k1[0]), DP_STATE_DIM, trace);
            (void)fwrite(trace_trial1, sizeof(trace_trial1[0]), DP_STATE_DIM, trace);
            (void)fwrite(h_k2, sizeof(h_k2[0]), DP_STATE_DIM, trace);
            (void)fwrite(trace_trial2, sizeof(trace_trial2[0]), DP_STATE_DIM, trace);
            (void)fwrite(h_k3, sizeof(h_k3[0]), DP_STATE_DIM, trace);
            (void)fwrite(trace_trial3, sizeof(trace_trial3[0]), DP_STATE_DIM, trace);
            (void)fwrite(h_k4, sizeof(h_k4[0]), DP_STATE_DIM, trace);
            (void)fclose(trace);
        }
    }
#endif

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        weighted[index] = h_k4[index];
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        const double twice_k3 = dp_rk4_add_left(h_k3[index], h_k3[index]);

        weighted[index] = dp_rk4_add_left(twice_k3, weighted[index]);
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        const double twice_k2 = dp_rk4_add_left(h_k2[index], h_k2[index]);

        weighted[index] = dp_rk4_add_left(twice_k2, weighted[index]);
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        weighted[index] = dp_rk4_add_left(weighted[index], h_k1[index]);
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        weighted[index] = dp_rk4_add_left(dp_rk4_mul_left(weighted[index],
                                                            0.16666666666666666),
                                          0.0);
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        state[index] = dp_rk4_add_left(weighted[index], state[index]);
    }
    *time += step;
}

/* 原 y_q_unit（0x5da0）：和 quat_reunit 不同，逐分量直接除以范数，
 * 不能替换为先求倒数再向量缩放，否则多步轨迹会出现末位差异。 */
static void dp_y_q_unit(double state[DP_STATE_DIM])
{
    double norm_squared;
    double norm;

    norm_squared = state[0] * state[0];
    norm_squared += state[1] * state[1];
    norm_squared += state[2] * state[2];
    norm_squared += state[3] * state[3];
    norm = sqrt(norm_squared);
    state[0] /= norm;
    state[1] /= norm;
    state[2] /= norm;
    state[3] /= norm;
}

/* 原 RK4_Intergrator 的可复用高层语义：阶段内核结束后调用 y_q_unit。 */
void dp_rk4_integrator_33(double state[DP_STATE_DIM], double *time, double step,
                          DpDerivative33 derivative, void *context)
{
    dp_rk4_step_33(state, time, step, derivative, context);
    dp_y_q_unit(state);
}
