/*
 * DynamicPackage 数学对象 ABI 还原。
 *
 * 证据：向量、矩阵、BLAS、四元数转换相关函数的 x86-64 反汇编。
 * 的 x86-64 反汇编。该头文件只定义已由固定偏移确认的容器布局。
 */
#ifndef DYNAMIC_MATH_H
#define DYNAMIC_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* vector: [0x00] int32 count, [0x08] double *data */
typedef struct {
    int32_t count;
    int32_t reserved_04;
    double *data;
} DpVector;

/* matrix: [0x00] rows, [0x04] cols, [0x08] row_stride, [0x10] double *data */
typedef struct {
    int32_t rows;
    int32_t cols;
    int32_t row_stride;
    int32_t reserved_0c;
    double *data;
} DpMatrix;

/* quaternion: [0x00] scalar w, [0x08] embedded DpVector xyz (data pointer at +0x10) */
typedef struct {
    double w;
    DpVector xyz;
} DpQuatAbi;

int vector_add(DpVector *left_inout, const DpVector *right);
int vector_sub(DpVector *left_inout, const DpVector *right);
void vector_set_zero(DpVector *vector_inout);
int vector_print(const DpVector *vector);
double vector_abs_max(const DpVector *vector);
int vector_add_constant(DpVector *vector_inout, double value);
int vector_scale(DpVector *vector_inout, double scalar);
/* 原 ABI：source（rdi）先乘 alpha，再累加写入 destination（rsi）；alpha 为零时返回 -1。 */
int vector_axpy(const DpVector *source, double alpha, DpVector *destination);
int vector_axpby(DpVector *y_inout, double alpha, const DpVector *x, double beta);
int vector_dot(const DpVector *left, const DpVector *right, double *out_dot);
int vector3_cross(const DpVector *left3, const DpVector *right3, DpVector *out3);
int vector_memcpy(DpVector *dst, const DpVector *src);
double vector_nrm2(const DpVector *vector);
double vector_square(const DpVector *vector);
/* 原 vector2angle：长度不等或任一范数非正返回 0；否则 acos(dot/(norm1*norm2))。 */
double vector2angle(const DpVector *left, const DpVector *right);
/* 原 isSunInFOV ABI：太阳方向向量与半视场角，参考轴固定为 body +Z。 */
int isSunInFOV(const DpVector *sun_direction, double half_fov_angle);
void vector_unit(DpVector *destination, const DpVector *source);

void matrix_print(const DpMatrix *matrix);
int matrix_set_col(DpMatrix *matrix, int32_t column, const DpVector *source);
void matrix_set_all(DpMatrix *matrix_inout, double value);
int matrix_set_zero(DpMatrix *matrix_inout);
int matrix_memcpy(DpMatrix *dst, const DpMatrix *src);
int matrix_trans(DpMatrix *dst, const DpMatrix *src);
int matrix_add_constant(DpMatrix *matrix_inout, double value);
int matrix_scale(DpMatrix *matrix_inout, double value);
int matrix_add(DpMatrix *left_inout, const DpMatrix *right);
int matrix_sub(DpMatrix *left_inout, const DpMatrix *right);
int matrix_mul_elements(DpMatrix *left_inout, const DpMatrix *right);
int matrix_div_elements(DpMatrix *left_inout, const DpMatrix *right);
int blas_gemv(const DpMatrix *matrix, const DpVector *x,
              DpVector *y_inout, double alpha, double beta);
int blas_gemm(const DpMatrix *left, const DpMatrix *right,
              DpMatrix *out_inout, double alpha, double beta);

/* 原符号 inv_CAL_M3：输入／输出均为连续 9 个 double 的裸 3×3 矩阵。 */
void inv_CAL_M3(const double input[9], double output[9]);

/* 原 array_* ABI：固定 33 个连续 double（状态向量长度）。 */
void array_copy(double destination[33], const double source[33]);
void array_sum(const double source[33], double destination[33], double alpha);

double Frac(double value);
double rng_uniform_pos(void);
/* 原 NiceAngle ABI：x 位于 xmm0，y 位于 xmm1；返回标准化方向角。 */
double NiceAngle(double x, double y);
double s2(double angle);
double c2(double angle);
double Eccentric2True(double value_a, double value_b, double value_c,
                      double value_d, double value_e);
void Rotation_X(DpMatrix *out_3x3, double angle);
void Rotation_Y(DpMatrix *out_3x3, double angle);
void Rotation_Z(DpMatrix *out_3x3, double angle);

/* 原 quat_cpy ABI：两个连续 4×double 裸四元数数组。 */
void quat_cpy(double destination[4], const double source[4]);
int quat_print(const DpQuatAbi *quat);
/* 原 quat_cross ABI：out（rdi）、left（rsi）、right（rdx）；可观察结果为 right × left。 */
void quat_cross(DpQuatAbi *out, const DpQuatAbi *left, const DpQuatAbi *right);
void quat_xi(DpMatrix *out_4x3, const DpQuatAbi *quat);
/* 原 ELF ABI：rdi=4×3 输出矩阵 descriptor，rsi=四元数 descriptor。 */
void quat_psi(DpMatrix *out_4x3, const DpQuatAbi *quat);
void quat_att_mat(DpMatrix *out_3x3, const DpQuatAbi *quat);
void AxisVector2Matrix(DpMatrix *out_3x3, const DpVector *axis_x,
                       const DpVector *axis_y, const DpVector *axis_z);
/* 原 `vector3_to_matrix` 的唯一参数实际是输出矩阵 descriptor。 */
void vector3_to_matrix(DpMatrix *out_3x3);
void euler_angle_312_to_quat(DpQuatAbi *out, double angle_a,
                             double angle_b, double angle_c);
/* 原 ELF 在当前 x86-64 局部 matrix descriptor 布局下于内部 quat_psi 触发 SIGSEGV。 */
void quat_to_euler_angle_312(double *angle_a, double *angle_b, double *angle_c,
                             const DpQuatAbi *quat);
/* 原 ELF 当前 x86-64 局部 matrix descriptor 布局下于内部 quat_psi 触发 SIGSEGV。 */
void quat2matrix(DpMatrix *out_3x3, const DpQuatAbi *quat);
void Cartesian2Polar(double *elevation, double *azimuth, double *radius,
                     const DpVector *cartesian);
void matrix2quat(DpQuatAbi *out, const DpMatrix *matrix);
void quat_diff(DpQuatAbi *out, const DpQuatAbi *quat, const DpVector *angular_rate);
void quat_conjugate(DpQuatAbi *out, const DpQuatAbi *in);
double quat_norm(const DpQuatAbi *quat);
void quat_reunit(DpQuatAbi *quat_inout);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_MATH_H */
