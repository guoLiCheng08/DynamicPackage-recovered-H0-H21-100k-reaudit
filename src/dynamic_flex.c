#include "dynamic_flex.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
#include <inttypes.h>
#include <stdio.h>

/* 每记录 33 个 double／264 bytes：effective[9]、base_rhs[3]、
 * SADA_reaction[3]、post_SADA_rhs[3]、final_rhs[3]、inv(effective)[9]、
 * rigid_angular_acceleration[3]。仅用于离线 H0 差分；默认产物不编译该函数。 */
static void dp_flex_modal_rhs_trace_binary(const DpVec3 *modal_a_term,
                                           const DpVec3 *modal_d_term,
                                           const DpVector *modal_velocity,
                                           const DpMatrix *modal_d_nxn,
                                           const DpVector *modal_d_output)
{
    double record[126];
    FILE *trace;

    if (modal_a_term == NULL || modal_d_term == NULL || modal_velocity == NULL ||
        modal_velocity->data == NULL || modal_velocity->count != 10 ||
        modal_d_nxn == NULL || modal_d_nxn->data == NULL ||
        modal_d_nxn->rows != 10 || modal_d_nxn->cols != 10 ||
        modal_d_nxn->row_stride < 10 || modal_d_output == NULL ||
        modal_d_output->data == NULL || modal_d_output->count != 10) {
        return;
    }
    memcpy(&record[0], modal_a_term, 3u * sizeof(record[0]));
    memcpy(&record[3], modal_d_term, 3u * sizeof(record[0]));
    memcpy(&record[6], modal_velocity->data, 10u * sizeof(record[0]));
    memcpy(&record[16], modal_d_nxn->data, 100u * sizeof(record[0]));
    memcpy(&record[116], modal_d_output->data, 10u * sizeof(record[0]));
    trace = fopen("/tmp/dp_flex_modal_rhs_trace.bin", "ab");
    if (trace == NULL) {
        return;
    }
    (void)fwrite(record, sizeof(record), 1u, trace);
    (void)fclose(trace);
}

static void dp_flex_trace_binary_snapshot(const DpMatrix *effective,
                                          const DpVec3 *rhs_base,
                                          const DpVec3 *sada_reaction,
                                          const DpVec3 *rhs_post_sada,
                                          const DpVec3 *rhs_final,
                                          const DpVec3 *angular_acceleration)
{
    double record[33];
    double inverse[9];
    FILE *trace;

    if (effective == NULL || effective->data == NULL || rhs_base == NULL ||
        sada_reaction == NULL || rhs_post_sada == NULL || rhs_final == NULL ||
        angular_acceleration == NULL) {
        return;
    }
    inv_CAL_M3(effective->data, inverse);
    memcpy(&record[0], effective->data, 9u * sizeof(record[0]));
    memcpy(&record[9], rhs_base, 3u * sizeof(record[0]));
    memcpy(&record[12], sada_reaction, 3u * sizeof(record[0]));
    memcpy(&record[15], rhs_post_sada, 3u * sizeof(record[0]));
    memcpy(&record[18], rhs_final, 3u * sizeof(record[0]));
    memcpy(&record[21], inverse, 9u * sizeof(record[0]));
    memcpy(&record[30], angular_acceleration, 3u * sizeof(record[0]));
    trace = fopen("/tmp/dp_flex_rhs_trace.bin", "ab");
    if (trace == NULL) {
        return;
    }
    (void)fwrite(record, sizeof(record), 1u, trace);
    (void)fclose(trace);
}
#endif

#include "dynamic_sat_inertia_flag0_constants.inc"

static int dp_matrix_is_3x3(const DpMatrix *matrix)
{
    return matrix != NULL && matrix->data != NULL && matrix->rows == 3 &&
           matrix->cols == 3 && matrix->row_stride >= 3;
}

static DpVector dp_vec3_view(DpVec3 *value)
{
    DpVector view;
    view.count = 3;
    view.reserved_04 = 0;
    view.data = value == NULL ? NULL : &value->x;
    return view;
}

static DpVector dp_const_vec3_view(const DpVec3 *value)
{
    DpVector view;
    view.count = 3;
    view.reserved_04 = 0;
    view.data = value == NULL ? NULL : (double *)&value->x;
    return view;
}

/* 按 IEEE-754 符号位精确取反；不得以乘以 -1 代替，因为 SSE mulsd
 * 对 qNaN 保留左操作数符号。 */
static double dp_flex_negate_f64_bits(double value)
{
    uint64_t bits;

    memcpy(&bits, &value, sizeof(bits));
    bits ^= UINT64_C(0x8000000000000000);
    memcpy(&value, &bits, sizeof(value));
    return value;
}

int dp_flex_effective_inertia_3x3(DpMatrix *effective_inertia_3x3,
                                  const DpFlexRigidMatrices *matrices)
{
    double transpose_data[3 * DP_FLEX_MAX_MODE_DIM] = {0.0};
    double product_data[9] = {0.0};
    DpMatrix transpose;
    DpMatrix product;

    if (effective_inertia_3x3 == NULL || matrices == NULL ||
        !dp_matrix_is_3x3(effective_inertia_3x3) ||
        !dp_matrix_is_3x3(&matrices->base_inertia_3x3) ||
        matrices->coupling_3xn.data == NULL || matrices->coupling_3xn.rows != 3 ||
        matrices->coupling_3xn.cols <= 0 || matrices->coupling_3xn.cols > DP_FLEX_MAX_MODE_DIM ||
        matrices->coupling_3xn.row_stride < matrices->coupling_3xn.cols) {
        return -1;
    }

    transpose.rows = matrices->coupling_3xn.cols;
    transpose.cols = matrices->coupling_3xn.rows;
    transpose.row_stride = matrices->coupling_3xn.rows;
    transpose.reserved_0c = 0;
    transpose.data = transpose_data;

    product.rows = 3;
    product.cols = 3;
    product.row_stride = 3;
    product.reserved_0c = 0;
    product.data = product_data;

    if (matrix_trans(&transpose, &matrices->coupling_3xn) != 0 ||
        blas_gemm(&matrices->coupling_3xn, &transpose, &product, 1.0, 0.0) != 0 ||
        matrix_memcpy(effective_inertia_3x3, &matrices->base_inertia_3x3) != 0 ||
        matrix_sub(effective_inertia_3x3, &product) != 0) {
        return -1;
    }
    return 0;
}

int dp_flex_compose_rigid_rhs(DpVec3 *rhs_out,
                              const DpVec3 *term_0,
                              const DpVec3 *term_1,
                              const DpVec3 *term_2,
                              const DpVec3 *term_3,
                              const DpVec3 *body_rate,
                              const DpVec3 *angular_momentum)
{
    DpVector rhs;
    DpVector term;
    DpVector omega;
    DpVector momentum;
    DpVec3 gyro_term;

    if (rhs_out == NULL || term_0 == NULL || term_1 == NULL || term_2 == NULL ||
        term_3 == NULL || body_rate == NULL || angular_momentum == NULL) {
        return -1;
    }

    *rhs_out = *term_0;
    rhs = dp_vec3_view(rhs_out);
    term = dp_const_vec3_view(term_1);
    if (vector_add(&rhs, &term) != 0) return -1;
    term = dp_const_vec3_view(term_2);
    if (vector_add(&rhs, &term) != 0) return -1;
    term = dp_const_vec3_view(term_3);
    if (vector_add(&rhs, &term) != 0) return -1;

    omega = dp_const_vec3_view(body_rate);
    momentum = dp_const_vec3_view(angular_momentum);
    gyro_term.x = gyro_term.y = gyro_term.z = 0.0;
    term = dp_vec3_view(&gyro_term);
    if (vector3_cross(&omega, &momentum, &term) != 0) return -1;
    if (vector_sub(&rhs, &term) != 0) return -1;
    return 0;
}

int dp_flex_solve_rigid_acceleration(DpVec3 *angular_acceleration_out,
                                     const DpMatrix *effective_inertia_3x3,
                                     const DpVec3 *rhs)
{
    double inverse_data[9] = {0.0};
    DpMatrix inverse;
    DpVector rhs_view;
    DpVector acceleration_view;

    if (angular_acceleration_out == NULL || rhs == NULL ||
        !dp_matrix_is_3x3(effective_inertia_3x3)) {
        return -1;
    }

    inverse.rows = 3;
    inverse.cols = 3;
    inverse.row_stride = 3;
    inverse.reserved_0c = 0;
    inverse.data = inverse_data;
    inv_CAL_M3(effective_inertia_3x3->data, inverse.data);

    rhs_view = dp_const_vec3_view(rhs);
    acceleration_view = dp_vec3_view(angular_acceleration_out);
    return blas_gemv(&inverse, &rhs_view, &acceleration_view, 1.0, 0.0);
}

int dp_flex_modal_acceleration(DpVector *modal_acceleration,
                               const DpMatrix *coupling_3xn,
                               const DpMatrix *modal_matrix_a_nxn,
                               const DpMatrix *modal_matrix_d_nxn,
                               const DpVector *modal_position,
                               const DpVector *modal_velocity,
                               const DpVec3 *angular_acceleration,
                               const DpMatrix *rigid_map_3x3,
                               const DpVec3 *rigid_aux,
                               const DpMatrix *sada_modal_pre_map_3x3,
                               const DpMatrix *sada_modal_acceleration_map_3xn,
                               const DpFlexSadaDrive *sada_drive)
{
    double transpose_data[DP_FLEX_MAX_MODE_DIM * 3] = {0.0};
    double sada_transpose_data[DP_FLEX_MAX_MODE_DIM * 3] = {0.0};
    double sada_input_data[3] = {0.0, 0.0, 0.0};
    double matrix_term_data[DP_FLEX_MAX_MODE_DIM] = {0.0};
    DpMatrix transpose;
    DpMatrix sada_transpose;
    DpVector angular_acceleration_view;
    DpVector sada_input;
    DpVec3 sada_projected = {0.0, 0.0, 0.0};
    DpVector sada_projected_view;
    DpVector matrix_term;
    const int n = coupling_3xn == NULL ? 0 : coupling_3xn->cols;

    if (modal_acceleration == NULL || coupling_3xn == NULL ||
        modal_matrix_a_nxn == NULL || modal_matrix_d_nxn == NULL ||
        modal_position == NULL || modal_velocity == NULL || angular_acceleration == NULL ||
        modal_acceleration->data == NULL || modal_position->data == NULL ||
        modal_velocity->data == NULL || coupling_3xn->data == NULL ||
        modal_matrix_a_nxn->data == NULL || modal_matrix_d_nxn->data == NULL ||
        coupling_3xn->rows != 3 || n <= 0 || n > DP_FLEX_MAX_MODE_DIM ||
        coupling_3xn->row_stride < n || modal_acceleration->count != n ||
        modal_position->count != n || modal_velocity->count != n ||
        modal_matrix_a_nxn->rows != n || modal_matrix_a_nxn->cols != n ||
        modal_matrix_a_nxn->row_stride < n || modal_matrix_d_nxn->rows != n ||
        modal_matrix_d_nxn->cols != n || modal_matrix_d_nxn->row_stride < n) {
        return -1;
    }

    transpose.rows = n;
    transpose.cols = 3;
    transpose.row_stride = 3;
    transpose.reserved_0c = 0;
    transpose.data = transpose_data;
    angular_acceleration_view = dp_const_vec3_view(angular_acceleration);
    matrix_term.count = n;
    matrix_term.reserved_04 = 0;
    matrix_term.data = matrix_term_data;

    /* 0xf2d1: C^T*alpha，alpha=-1、beta=0。 */
    if (matrix_trans(&transpose, coupling_3xn) != 0 ||
        blas_gemv(&transpose, &angular_acceleration_view, modal_acceleration, -1.0, 0.0) != 0) {
        return -1;
    }

    /* 0xf33f..0xf365：可选的 M4 辅助刚体项，再以 C^T 负投影并累加。 */
    if (rigid_map_3x3 != NULL || rigid_aux != NULL) {
        DpVec3 rigid_projection;
        DpVector rigid_projection_view;
        DpVector rigid_aux_view;

        if (!dp_matrix_is_3x3(rigid_map_3x3) || rigid_aux == NULL) {
            return -1;
        }
        rigid_projection.x = rigid_projection.y = rigid_projection.z = 0.0;
        rigid_projection_view = dp_vec3_view(&rigid_projection);
        rigid_aux_view = dp_const_vec3_view(rigid_aux);
        if (blas_gemv(rigid_map_3x3, &rigid_aux_view, &rigid_projection_view, 1.0, 0.0) != 0 ||
            blas_gemv(&transpose, &rigid_projection_view, modal_acceleration, -1.0, 1.0) != 0) {
            return -1;
        }
    }

    /* 0xf2ff..0xf365：太阳翼 [0,a0,a1] 的 3×10 负转置投影。
     * 它必须位于刚体 alpha 投影之后、刚度与阻尼项之前。 */
    if (sada_modal_pre_map_3x3 != NULL || sada_modal_acceleration_map_3xn != NULL ||
        sada_drive != NULL) {
        if (!dp_matrix_is_3x3(sada_modal_pre_map_3x3) ||
            sada_modal_acceleration_map_3xn == NULL || sada_drive == NULL ||
            sada_modal_acceleration_map_3xn->data == NULL ||
            sada_modal_acceleration_map_3xn->rows != 3 ||
            sada_modal_acceleration_map_3xn->cols != n ||
            sada_modal_acceleration_map_3xn->row_stride < n) {
            return -1;
        }
        sada_transpose.rows = n;
        sada_transpose.cols = 3;
        sada_transpose.row_stride = 3;
        sada_transpose.reserved_0c = 0;
        sada_transpose.data = sada_transpose_data;
        sada_input.count = 3;
        sada_input.reserved_04 = 0;
        sada_input.data = sada_input_data;
        sada_input_data[1] = sada_drive->angular_acceleration[0];
        sada_input_data[2] = sada_drive->angular_acceleration[1];
        sada_projected_view = dp_vec3_view(&sada_projected);
        if (blas_gemv(sada_modal_pre_map_3x3, &sada_input,
                      &sada_projected_view, 1.0, 0.0) != 0 ||
            matrix_trans(&sada_transpose, sada_modal_acceleration_map_3xn) != 0 ||
            blas_gemv(&sada_transpose, &sada_projected_view,
                      modal_acceleration, -1.0, 1.0) != 0) {
            return -1;
        }
    }

    /* 0xf3cb、0xf3d0：-0.1 * MA * eta，随后 vector_add。 */
    if (blas_gemv(modal_matrix_a_nxn, modal_position, &matrix_term, -0.1, 0.0) != 0 ||
        vector_add(modal_acceleration, &matrix_term) != 0) {
        return -1;
    }

    /* 0xf446、0xf451：-MD * eta_dot，随后第二次 vector_add。 */
    if (blas_gemv(modal_matrix_d_nxn, modal_velocity, &matrix_term, -1.0, 0.0) != 0 ||
        vector_add(modal_acceleration, &matrix_term) != 0) {
        return -1;
    }
    return 0;
}

static int dp_flex_compose_rigid_rhs_base(DpVec3 *rhs_out,
                                          const DpVec3 *body_rate,
                                          const DpVec3 *angular_momentum,
                                          const DpVec3 *minus_term)
{
    DpVec3 three_term;
    DpVector rhs;
    DpVector rate;
    DpVector momentum;
    DpVector three_term_view;

    if (rhs_out == NULL || body_rate == NULL || angular_momentum == NULL) {
        return -1;
    }
    /* 原 0xed60..0xee78：RHS 先以 L_c_B（调用者传入的是其按位取反
     * 的 -L_c_B）初始化，随后扣除 omega×H。不能重排为
     * 0 - (omega×H) - (-L_c_B)，因为 qNaN 的符号来自每次 SSE
     * subsd 的左操作数。 */
    if (minus_term != NULL) {
        rhs_out->x = dp_flex_negate_f64_bits(minus_term->x);
        rhs_out->y = dp_flex_negate_f64_bits(minus_term->y);
        rhs_out->z = dp_flex_negate_f64_bits(minus_term->z);
    } else {
        rhs_out->x = rhs_out->y = rhs_out->z = 0.0;
    }
    rhs = dp_vec3_view(rhs_out);
    rate = dp_const_vec3_view(body_rate);
    momentum = dp_const_vec3_view(angular_momentum);
    three_term.x = three_term.y = three_term.z = 0.0;
    three_term_view = dp_vec3_view(&three_term);
    if (vector3_cross(&rate, &momentum, &three_term_view) != 0 ||
        vector_sub(&rhs, &three_term_view) != 0) {
        return -1;
    }
    return 0;
}

static int dp_flex_add_modal_rhs_terms(DpVec3 *rhs_out,
                                       const DpMatrix *coupling_3xn,
                                       const DpMatrix *modal_matrix_a_nxn,
                                       const DpMatrix *modal_matrix_d_nxn,
                                       const DpVector *modal_position,
                                       const DpVector *modal_velocity)
{
    const int n = coupling_3xn == NULL ? 0 : coupling_3xn->cols;
    double mode_term_data[DP_FLEX_MAX_MODE_DIM] = {0.0};
    DpVec3 three_term;
    DpVector rhs;
    DpVector three_term_view;
    DpVector mode_term;
    unsigned index;
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    DpVec3 modal_a_term_trace;
#endif

    if (rhs_out == NULL || coupling_3xn == NULL || modal_matrix_a_nxn == NULL ||
        modal_matrix_d_nxn == NULL || modal_position == NULL || modal_velocity == NULL ||
        coupling_3xn->data == NULL || modal_matrix_a_nxn->data == NULL ||
        modal_matrix_d_nxn->data == NULL || coupling_3xn->rows != 3 || n <= 0 ||
        n > DP_FLEX_MAX_MODE_DIM || coupling_3xn->row_stride < n ||
        modal_position->data == NULL || modal_velocity->data == NULL ||
        modal_position->count != n || modal_velocity->count != n ||
        modal_matrix_a_nxn->rows != n || modal_matrix_a_nxn->cols != n ||
        modal_matrix_a_nxn->row_stride < n || modal_matrix_d_nxn->rows != n ||
        modal_matrix_d_nxn->cols != n || modal_matrix_d_nxn->row_stride < n) {
        return -1;
    }
    rhs = dp_vec3_view(rhs_out);
    three_term.x = three_term.y = three_term.z = 0.0;
    three_term_view = dp_vec3_view(&three_term);
    mode_term.count = n;
    mode_term.reserved_04 = 0;
    mode_term.data = mode_term_data;

    /* 0xf0a4..0xf17d：MA*eta，经 C 投影后逐分量乘 0.1 并写回 RHS。 */
    if (blas_gemv(modal_matrix_a_nxn, modal_position, &mode_term, 1.0, 0.0) != 0 ||
        blas_gemv(coupling_3xn, &mode_term, &three_term_view, 1.0, 0.0) != 0) {
        return -1;
    }
    for (index = 0u; index < 3u; ++index) {
        three_term_view.data[index] *= 0.1;
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    modal_a_term_trace = three_term;
#endif
    if (vector_add(&rhs, &three_term_view) != 0) {
        return -1;
    }

    /* 原 0xf1b4 在独立、零初始化的 10 项输出缓冲中计算 MD*eta_dot。
     * 不可复用先前 MA*eta 的 buffer：即使 beta=0，原 blas_gemv 的
     * `dot + (old_y*beta)` 仍会让旧 qNaN 污染新输出。 */
    memset(mode_term_data, 0, (size_t)n * sizeof(mode_term_data[0]));
    /* 0xf1b4..0xf204：MD*eta_dot，经 C 投影后第二次写回 RHS。
     * 原函数为这次 C 投影分配独立零缓冲；不得复用首个 C·MA·eta 的
     * three_term，否则 beta=0 仍会保留旧 qNaN。 */
    three_term.x = three_term.y = three_term.z = 0.0;
    if (blas_gemv(modal_matrix_d_nxn, modal_velocity, &mode_term, 1.0, 0.0) != 0 ||
        blas_gemv(coupling_3xn, &mode_term, &three_term_view, 1.0, 0.0) != 0) {
        return -1;
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    dp_flex_modal_rhs_trace_binary(&modal_a_term_trace, &three_term, modal_velocity,
                                   modal_matrix_d_nxn, &mode_term);
#endif
    if (vector_add(&rhs, &three_term_view) != 0) {
        return -1;
    }
    return 0;
}

int dp_flex_compose_rigid_rhs_coupled(DpVec3 *rhs_out,
                                      const DpVec3 *body_rate,
                                      const DpVec3 *angular_momentum,
                                      const DpVec3 *minus_term,
                                      const DpMatrix *coupling_3xn,
                                      const DpMatrix *modal_matrix_a_nxn,
                                      const DpMatrix *modal_matrix_d_nxn,
                                      const DpVector *modal_position,
                                      const DpVector *modal_velocity)
{
    if (dp_flex_compose_rigid_rhs_base(rhs_out, body_rate, angular_momentum, minus_term) != 0 ||
        dp_flex_add_modal_rhs_terms(rhs_out, coupling_3xn, modal_matrix_a_nxn,
                                    modal_matrix_d_nxn, modal_position, modal_velocity) != 0) {
        return -1;
    }
    return 0;
}

int dp_flex_dynamics_step(DpVec3 *angular_acceleration,
                          DpVector *modal_acceleration,
                          DpVector *modal_velocity_output,
                          const DpVec3 *body_rate,
                          const DpVector *modal_position,
                          const DpVector *modal_velocity,
                          const DpFlexDynamicsConfig *config)
{
    double effective_data[9] = {0.0};
    DpMatrix effective;
    DpVec3 rhs;
    const DpMatrix *coupling;
    double sada_acceleration_input_data[3] = {0.0, 0.0, 0.0};
    double sada_reaction_data[3] = {0.0, 0.0, 0.0};
    DpVector sada_acceleration_input = {3, 0, sada_acceleration_input_data};
    DpVector sada_reaction = {3, 0, sada_reaction_data};
    DpVector rhs_view;
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    DpVec3 rhs_base_trace;
    DpVec3 sada_reaction_trace;
    DpVec3 rhs_post_sada_trace;
#endif

    if (angular_acceleration == NULL || modal_acceleration == NULL ||
        modal_velocity_output == NULL || body_rate == NULL || modal_position == NULL ||
        modal_velocity == NULL || config == NULL || config->modal_matrix_a_nxn == NULL ||
        config->modal_matrix_d_nxn == NULL || config->angular_momentum == NULL) {
        return -1;
    }
    coupling = &config->rigid_matrices.coupling_3xn;
    if (modal_acceleration->data == NULL || modal_velocity_output->data == NULL ||
        modal_velocity_output->count != modal_velocity->count ||
        modal_acceleration->count != modal_velocity->count) {
        return -1;
    }

    effective.rows = 3;
    effective.cols = 3;
    effective.row_stride = 3;
    effective.reserved_0c = 0;
    effective.data = effective_data;

    if (dp_flex_effective_inertia_3x3(&effective, &config->rigid_matrices) != 0 ||
        dp_flex_compose_rigid_rhs_base(&rhs, body_rate, config->angular_momentum,
                                       config->minus_term) != 0) {
        return -1;
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    rhs_base_trace = rhs;
#endif
    if (config->sada_drive != NULL && config->sada_acceleration_reaction_map_3x3 != NULL) {
        /* 原 dynamics_flex 以 [0,a0,a1] 经独立反作用矩阵计算太阳翼角加速度
         * 对母体的刚体 RHS 项；该项在求逆有效惯量之前相减。 */
        sada_acceleration_input_data[1] = config->sada_drive->angular_acceleration[0];
        sada_acceleration_input_data[2] = config->sada_drive->angular_acceleration[1];
        rhs_view = dp_vec3_view(&rhs);
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
        {
            uint64_t pre_reaction_bits[3];
            memcpy(&pre_reaction_bits[0], &rhs.x, sizeof(pre_reaction_bits[0]));
            memcpy(&pre_reaction_bits[1], &rhs.y, sizeof(pre_reaction_bits[1]));
            memcpy(&pre_reaction_bits[2], &rhs.z, sizeof(pre_reaction_bits[2]));
            (void)fprintf(stderr, "dp_flex_rhs_pre_reaction=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
                          pre_reaction_bits[0], pre_reaction_bits[1], pre_reaction_bits[2]);
        }
#endif
        if (blas_gemv(config->sada_acceleration_reaction_map_3x3,
                      &sada_acceleration_input, &sada_reaction, 1.0, 0.0) != 0) {
            return -1;
        }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
        {
            uint64_t reaction_bits[3];
            memcpy(&reaction_bits[0], &sada_reaction.data[0], sizeof(reaction_bits[0]));
            memcpy(&reaction_bits[1], &sada_reaction.data[1], sizeof(reaction_bits[1]));
            memcpy(&reaction_bits[2], &sada_reaction.data[2], sizeof(reaction_bits[2]));
            (void)fprintf(stderr, "dp_sada_reaction=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
                          reaction_bits[0], reaction_bits[1], reaction_bits[2]);
        }
#endif
        if (vector_sub(&rhs_view, &sada_reaction) != 0) {
            return -1;
        }
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    {
        uint64_t rhs_bits[3];
        memcpy(&rhs_bits[0], &rhs.x, sizeof(rhs_bits[0]));
        memcpy(&rhs_bits[1], &rhs.y, sizeof(rhs_bits[1]));
        memcpy(&rhs_bits[2], &rhs.z, sizeof(rhs_bits[2]));
        (void)fprintf(stderr, "dp_flex_rhs=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
                      rhs_bits[0], rhs_bits[1], rhs_bits[2]);
    }
#endif
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    sada_reaction_trace.x = sada_reaction_data[0];
    sada_reaction_trace.y = sada_reaction_data[1];
    sada_reaction_trace.z = sada_reaction_data[2];
    rhs_post_sada_trace = rhs;
#endif
    /* 原 ELF 在 f1e9..f268：基础 RHS -> SADA 反作用扣除 -> MA 尾项 -> MD 尾项。 */
    if (dp_flex_add_modal_rhs_terms(&rhs, coupling, config->modal_matrix_a_nxn,
                                    config->modal_matrix_d_nxn, modal_position,
                                    modal_velocity) != 0) {
        return -1;
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    {
        uint64_t rhs_bits[3];
        memcpy(&rhs_bits[0], &rhs.x, sizeof(rhs_bits[0]));
        memcpy(&rhs_bits[1], &rhs.y, sizeof(rhs_bits[1]));
        memcpy(&rhs_bits[2], &rhs.z, sizeof(rhs_bits[2]));
        (void)fprintf(stderr, "dp_flex_rhs_post_modal=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
                      rhs_bits[0], rhs_bits[1], rhs_bits[2]);
    }
#endif
    if (dp_flex_solve_rigid_acceleration(angular_acceleration, &effective, &rhs) != 0) {
        return -1;
    }
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    dp_flex_trace_binary_snapshot(&effective, &rhs_base_trace,
                                  &sada_reaction_trace, &rhs_post_sada_trace,
                                  &rhs, angular_acceleration);
#endif
    if (dp_flex_modal_acceleration(modal_acceleration, coupling,
                                   config->modal_matrix_a_nxn,
                                   config->modal_matrix_d_nxn,
                                   modal_position, modal_velocity,
                                   angular_acceleration, config->rigid_map_3x3,
                                   config->rigid_aux,
                                   config->sada_modal_pre_map_3x3,
                                   config->sada_modal_acceleration_map_3xn,
                                   config->sada_drive) != 0) {
        return -1;
    }
    if (vector_memcpy(modal_velocity_output, modal_velocity) != 0) {
        return -1;
    }
    return 0;
}

void dp_reset_inertia_from_baseline_3x3(double inertia_out[9],
                                        double inverse_out[9],
                                        const double baseline[9])
{
    unsigned index;

    for (index = 0u; index < 9u; ++index) {
        inertia_out[index] = baseline[index];
    }
    inv_CAL_M3(inertia_out, inverse_out);
}

static void dp_copy_double_bits(double *destination, const uint64_t *source, size_t count)
{
    size_t index;

    for (index = 0u; index < count; ++index) {
        memcpy(&destination[index], &source[index], sizeof(destination[index]));
    }
}

int dp_load_sat_inertia_flag0_static(DpSatInertiaFlag0Outputs *outputs)
{
    if (outputs == NULL || outputs->inertia_3x3 == NULL ||
        outputs->inverse_inertia_3x3 == NULL || outputs->matrix_3x3_e8 == NULL ||
        outputs->matrix_3x3_448 == NULL || outputs->matrix_3x3_4a8 == NULL ||
        outputs->matrix_3x3_508 == NULL || outputs->matrix_3x3_568 == NULL ||
        outputs->matrix_3x10_688 == NULL || outputs->coupling_3x10_898 == NULL ||
        outputs->matrix_3x3_9a0 == NULL || outputs->modal_matrix_a_10x10 == NULL ||
        outputs->modal_matrix_d_10x10 == NULL) {
        return -1;
    }

    dp_copy_double_bits(outputs->inertia_3x3, dp_flag0_inertia_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x3_e8, dp_flag0_m3_e8_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x3_448, dp_flag0_m3_448_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x3_4a8, dp_flag0_m3_4a8_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x3_508, dp_flag0_m3_508_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x3_568, dp_flag0_m3_568_bits, 9u);
    dp_copy_double_bits(outputs->matrix_3x10_688, dp_flag0_m6_bits, 30u);
    dp_copy_double_bits(outputs->coupling_3x10_898, dp_flag0_c_bits, 30u);
    dp_copy_double_bits(outputs->matrix_3x3_9a0, dp_flag0_m9_bits, 9u);
    dp_copy_double_bits(outputs->modal_matrix_a_10x10, dp_flag0_ma_bits, 100u);
    dp_copy_double_bits(outputs->modal_matrix_d_10x10, dp_flag0_md_bits, 100u);
    inv_CAL_M3(outputs->inertia_3x3, outputs->inverse_inertia_3x3);
    return 0;
}
