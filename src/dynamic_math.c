/* DynamicPackage 基础数学库的高保真 C 重建。 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dynamic_math.h"
#include "dynamic_rng.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(offsetof(DpVector, data) == 0x08, "DpVector ABI");
_Static_assert(offsetof(DpMatrix, data) == 0x10, "DpMatrix ABI");
_Static_assert(offsetof(DpQuatAbi, xyz) == 0x08, "DpQuat vector ABI");
_Static_assert(offsetof(DpQuatAbi, xyz.data) == 0x10, "DpQuat data ABI");

static int matrix_same_shape(const DpMatrix *left, const DpMatrix *right)
{
    return left->rows == right->rows && left->cols == right->cols;
}

/* 显式固定 SSE 目标寄存器的左操作数；声明置于 vector_add 前以避免 C
 * 在 -O0 下先载入 right、再执行 addsd 而改变 qNaN 符号。 */
static double dp_f64_add_left(double left, double right);

int vector_add(DpVector *left_inout, const DpVector *right)
{
    int32_t index;
    if (left_inout->count != right->count) return -1;
    for (index = 0; index < left_inout->count; ++index) {
        left_inout->data[index] = dp_f64_add_left(left_inout->data[index],
                                                   right->data[index]);
    }
    return 0;
}

int vector_sub(DpVector *left_inout, const DpVector *right)
{
    int32_t index;
    if (left_inout->count != right->count) return -1;
    for (index = 0; index < left_inout->count; ++index) {
        left_inout->data[index] -= right->data[index];
    }
    return 0;
}

void vector_set_zero(DpVector *vector_inout)
{
    int32_t index;
    for (index = 0; index < vector_inout->count; ++index) {
        vector_inout->data[index] = 0.0;
    }
}

/* 原 0x97e0：按无符号 count 打印，逐元素独占一行，最后再输出一个换行；恒返回 1。 */
int vector_print(const DpVector *vector)
{
    uint32_t index;

    (void)printf("\nSize: %d\n", vector->count);
    for (index = 0u; index < (uint32_t)vector->count; ++index) {
        (void)printf("%10.5f\n", vector->data[index]);
    }
    (void)putchar('\n');
    return 1;
}

double vector_abs_max(const DpVector *vector)
{
    int32_t index;
    double maximum = 0.0;
    for (index = 0; index < vector->count; ++index) {
        const double magnitude = fabs(vector->data[index]);
        /* 对应 maxsd：NaN 不取代已有的 maximum。 */
        if (magnitude > maximum) maximum = magnitude;
    }
    return maximum;
}

int vector_add_constant(DpVector *vector_inout, double value)
{
    int32_t index;
    for (index = 0; index < vector_inout->count; ++index) {
        vector_inout->data[index] += value;
    }
    return 0;
}

int vector_scale(DpVector *vector_inout, double scalar)
{
    int32_t index;
    for (index = 0; index < vector_inout->count; ++index) {
        vector_inout->data[index] *= scalar;
    }
    return 0;
}

int vector_axpy(const DpVector *source, double alpha, DpVector *destination)
{
    int32_t index;

    /* 原 0x7e40：ucomisd alpha,0；精确为零直接返回 -1，NaN 继续计算。 */
    if (alpha == 0.0) return -1;
    for (index = 0; index < source->count; ++index) {
        destination->data[index] = source->data[index] * alpha + destination->data[index];
    }
    return 0;
}

int vector_axpby(DpVector *source, double alpha, const DpVector *destination,
                 double beta)
{
    int32_t index;

    /* 原 0x7ea0：alpha 精确为零返回 -1；非零（含 NaN）时只读取
     * source->count，读取 source/data 与 destination/data 并写回 destination。
     * 不比较 destination->count。 */
    if (alpha == 0.0) return -1;
    for (index = 0; index < source->count; ++index) {
        destination->data[index] = source->data[index] * alpha +
                                   destination->data[index] * beta;
    }
    return 0;
}

int vector_dot(const DpVector *left, const DpVector *right, double *out_dot)
{
    int32_t index;
    double result = 0.0;
    if (left->count != right->count) return -1;
    for (index = 0; index < left->count; ++index) {
        result += left->data[index] * right->data[index];
    }
    *out_dot = result;
    return 0;
}

int vector3_cross(const DpVector *left3, const DpVector *right3, DpVector *out3)
{
    const double *left;
    const double *right;
    double *out;
    if (left3->count != right3->count) return -1;
    left = left3->data;
    right = right3->data;
    out = out3->data;
    out[0] = left[1] * right[2] - left[2] * right[1];
    out[1] = left[2] * right[0] - left[0] * right[2];
    out[2] = left[0] * right[1] - left[1] * right[0];
    return 0;
}

int vector_memcpy(DpVector *dst, const DpVector *src)
{
    uint32_t index;
    const uint32_t source_count = (uint32_t)src->count;

    /* 原 0x9850：只读取 src->count；非零时按该无符号长度复制，
     * 不检查 dst->count，最后始终返回 0。 */
    for (index = 0u; index < source_count; ++index) {
        dst->data[index] = src->data[index];
    }
    return 0;
}

double vector_nrm2(const DpVector *vector)
{
    int32_t index;
    double sum = 0.0;
    for (index = 0; index < vector->count; ++index) {
        sum += vector->data[index] * vector->data[index];
    }
    return sqrt(sum);
}

/* 原 0x7f80：与 vector_nrm2 相同的平方和累加，但不执行 sqrt。 */
double vector_square(const DpVector *vector)
{
    int32_t index;
    double sum = 0.0;
    for (index = 0; index < vector->count; ++index) {
        sum += vector->data[index] * vector->data[index];
    }
    return sum;
}

double vector2angle(const DpVector *left, const DpVector *right)
{
    const double left_norm = vector_nrm2(left);
    const double right_norm = vector_nrm2(right);
    int32_t index;
    double dot = 0.0;

    if (left->count == right->count) {
        for (index = 0; index < left->count; ++index) {
            dot += left->data[index] * right->data[index];
        }
    }
    if (left->count != right->count || left_norm <= 0.0 || right_norm <= 0.0) {
        return 0.0;
    }
    return acos(dot / (left_norm * right_norm));
}

int isSunInFOV(const DpVector *sun_direction, double half_fov_angle)
{
    double reference_data[3] = {0.0, 0.0, 1.0};
    DpVector reference_axis = {3, 0, reference_data};
    const double angle = vector2angle(sun_direction, &reference_axis);

    return half_fov_angle >= angle;
}

void vector_unit(DpVector *destination, const DpVector *source)
{
    int32_t index;
    double norm = vector_nrm2(source);

    /* 原 0x7fd0：仅严格正范数进入写回；零、负值和 NaN 均直接返回。 */
    if (!(norm > 0.0)) return;
    (void)vector_scale(destination, 0.0);
    norm = vector_nrm2(source);
    for (index = 0; index < source->count; ++index) {
        destination->data[index] += source->data[index] * (1.0 / norm);
    }
}

void matrix_print(const DpMatrix *matrix)
{
    uint32_t row;
    uint32_t col;

    (void)printf("\nSize: %d * %d\n", matrix->rows, matrix->cols);
    for (row = 0u; row < (uint32_t)matrix->rows; ++row) {
        for (col = 0u; col < (uint32_t)matrix->cols; ++col) {
            /* 原 0x8a60 使用 row*cols+col，未读取 row_stride。 */
            (void)printf("%10.5f ", matrix->data[row * (uint32_t)matrix->cols + col]);
        }
        (void)putchar('\n');
    }
    (void)putchar('\n');
}

int matrix_set_col(DpMatrix *matrix, int32_t column, const DpVector *source)
{
    int32_t row;

    /* 原 0x8b10 先以无符号比较 column < cols，再验证 source.count == rows。 */
    if ((uint32_t)column >= (uint32_t)matrix->cols || source->count != matrix->rows) return -1;
    for (row = 0; row < source->count; ++row) {
        /* 原实现步进 matrix->cols，未读取 row_stride。 */
        matrix->data[row * matrix->cols + column] = source->data[row];
    }
    return 0;
}

void matrix_set_all(DpMatrix *matrix_inout, double value)
{
    int32_t row;
    int32_t col;
    for (row = 0; row < matrix_inout->rows; ++row) {
        for (col = 0; col < matrix_inout->cols; ++col) {
            matrix_inout->data[row * matrix_inout->row_stride + col] = value;
        }
    }
}

int matrix_set_zero(DpMatrix *matrix_inout)
{
    int32_t row;
    int32_t col = 0;

    /* 原 0x88d0 未在尾部清零 EAX；正 rows/cols 时它保留最后一个
     * 内层循环退出索引，即 (rows-1)*row_stride+cols。 */
    for (row = 0; row < matrix_inout->rows; ++row) {
        col = row * matrix_inout->row_stride;
        for (; col < row * matrix_inout->row_stride + matrix_inout->cols; ++col) {
            matrix_inout->data[col] = 0.0;
        }
    }
    return col;
}

int matrix_memcpy(DpMatrix *dst, const DpMatrix *src)
{
    uint32_t row;
    uint32_t col;
    const uint32_t source_rows = (uint32_t)src->rows;
    const uint32_t source_cols = (uint32_t)src->cols;

    /* 原 0x8970：循环边界仅来自 src.rows/src.cols；不读取 dst.rows 或
     * dst.cols，也不作形状验证。源和目标各自使用自己的 row_stride。 */
    for (row = 0u; row < source_rows; ++row) {
        for (col = 0u; col < source_cols; ++col) {
            dst->data[row * (uint32_t)dst->row_stride + col] =
                src->data[row * (uint32_t)src->row_stride + col];
        }
    }
    return 0;
}

int matrix_trans(DpMatrix *dst, const DpMatrix *src)
{
    int32_t row;
    int32_t col;
    for (row = 0; row < src->rows; ++row) {
        for (col = 0; col < src->cols; ++col) {
            dst->data[col * dst->row_stride + row] = src->data[row * src->row_stride + col];
        }
    }
    return 0;
}

int matrix_add_constant(DpMatrix *matrix_inout, double value)
{
    int32_t row;
    int32_t col;
    for (row = 0; row < matrix_inout->rows; ++row) {
        for (col = 0; col < matrix_inout->cols; ++col) {
            matrix_inout->data[row * matrix_inout->row_stride + col] += value;
        }
    }
    return 0;
}

int matrix_scale(DpMatrix *matrix_inout, double value)
{
    int32_t row;
    int32_t col;
    for (row = 0; row < matrix_inout->rows; ++row) {
        for (col = 0; col < matrix_inout->cols; ++col) {
            matrix_inout->data[row * matrix_inout->row_stride + col] *= value;
        }
    }
    return 0;
}

int matrix_add(DpMatrix *left_inout, const DpMatrix *right)
{
    int32_t row;
    int32_t col;
    if (!matrix_same_shape(left_inout, right)) return -1;
    for (row = 0; row < left_inout->rows; ++row) {
        for (col = 0; col < left_inout->cols; ++col) {
            left_inout->data[row * left_inout->row_stride + col] += right->data[row * right->row_stride + col];
        }
    }
    return 0;
}

int matrix_sub(DpMatrix *left_inout, const DpMatrix *right)
{
    int32_t row;
    int32_t col;
    if (!matrix_same_shape(left_inout, right)) return -1;
    for (row = 0; row < left_inout->rows; ++row) {
        for (col = 0; col < left_inout->cols; ++col) {
            left_inout->data[row * left_inout->row_stride + col] -= right->data[row * right->row_stride + col];
        }
    }
    return 0;
}

int matrix_mul_elements(DpMatrix *left_inout, const DpMatrix *right)
{
    int32_t row;
    int32_t col;
    if (!matrix_same_shape(left_inout, right)) return -1;
    for (row = 0; row < left_inout->rows; ++row) {
        for (col = 0; col < left_inout->cols; ++col) {
            left_inout->data[row * left_inout->row_stride + col] *= right->data[row * right->row_stride + col];
        }
    }
    return 0;
}

int matrix_div_elements(DpMatrix *left_inout, const DpMatrix *right)
{
    int32_t row;
    int32_t col;
    if (!matrix_same_shape(left_inout, right)) return -1;
    for (row = 0; row < left_inout->rows; ++row) {
        for (col = 0; col < left_inout->cols; ++col) {
            left_inout->data[row * left_inout->row_stride + col] /= right->data[row * right->row_stride + col];
        }
    }
    return 0;
}

static double dp_f64_mul_left(double left, double right);

int blas_gemv(const DpMatrix *matrix, const DpVector *x,
              DpVector *y_inout, double alpha, double beta)
{
    int32_t row;
    int32_t col;
    if (matrix->rows != y_inout->count) return -1;
    /* 原 ELF 对 alpha == 0 的路径只缩放 y，且返回 -1。 */
    if (alpha == 0.0) {
        (void)vector_scale(y_inout, beta);
        return -1;
    }
    for (row = 0; row < matrix->rows; ++row) {
        double dot = 0.0;
        for (col = 0; col < matrix->cols; ++col) {
            double product = matrix->data[row * matrix->row_stride + col];

            /* 原 0x81a1..0x81af：A 为乘法左操作数，dot 为加法左操作数。 */
            product = dp_f64_mul_left(product, x->data[col]);
            dot = dp_f64_add_left(dot, product);
        }
        /* 原 0x81bf..0x81c7：dot×alpha 后，以 dot 为左操作数加上旧 y×beta。 */
        dot = dp_f64_mul_left(dot, alpha);
        y_inout->data[row] = dp_f64_add_left(dot,
                                              dp_f64_mul_left(y_inout->data[row], beta));
    }
    return 0;
}

/* 以左操作数作为 ABI 浮点目标寄存器；对 qNaN 的符号传播可观察。 */
static double dp_f64_mul_left(double left, double right)
{
    return left * right;
}

static double dp_f64_add_left(double left, double right)
{
    return left + right;
}

int blas_gemm(const DpMatrix *left, const DpMatrix *right,
               DpMatrix *out_inout, double alpha, double beta)
{
    int32_t row;
    int32_t col;
    int32_t k;

    /* 原 0x8210 在读取任一 shape 前以 ucomisd 判定 alpha；精确零立即失败。 */
    if (alpha == 0.0) return -1;
    if (left->cols != right->rows || left->rows != out_inout->rows ||
        right->cols != out_inout->cols) return -1;
    for (row = 0; row < out_inout->rows; ++row) {
        for (col = 0; col < out_inout->cols; ++col) {
            double sum = 0.0;
            for (k = 0; k < left->cols; ++k) {
                double product = left->data[row * left->row_stride + k];

                /* 原 0x82f0：先 A×alpha，再乘 B，随后累加；每步保留左操作数。 */
                product = dp_f64_mul_left(product, alpha);
                product = dp_f64_mul_left(product,
                                          right->data[k * right->row_stride + col]);
                sum = dp_f64_add_left(sum, product);
            }
            /* 原 0x830c：sum + (旧 C×beta)，不作代数重排且 sum 保持左操作数。 */
            out_inout->data[row * out_inout->row_stride + col] =
                dp_f64_add_left(sum,
                                dp_f64_mul_left(out_inout->data[row * out_inout->row_stride + col],
                                                beta));
        }
    }
    return 0;
}

void inv_CAL_M3(const double input[9], double output[9])
{
    const double a00 = input[0];
    const double a01 = input[1];
    const double a02 = input[2];
    const double a10 = input[3];
    const double a11 = input[4];
    const double a12 = input[5];
    const double a20 = input[6];
    const double a21 = input[7];
    const double a22 = input[8];
    double determinant_positive_0;
    double determinant_positive_1;
    double determinant_negative;
    double determinant;

    /* 对应 0x8b91..0x8c0a：不能收缩为余子式的代数等价表达。 */
    determinant_positive_0 = a02 * a10;
    determinant_positive_0 *= a21;
    determinant_positive_1 = a12 * a01;
    determinant_positive_1 *= a20;
    determinant_positive_0 += determinant_positive_1;
    determinant_positive_1 = a00 * a11;
    determinant_positive_1 *= a22;
    determinant_positive_0 += determinant_positive_1;

    determinant_negative = a01 * a22;
    determinant_negative *= a10;
    determinant_positive_1 = a00 * a21;
    determinant_positive_1 *= a12;
    determinant_negative += determinant_positive_1;
    determinant_positive_1 = a02 * a11;
    determinant_positive_1 *= a20;
    determinant_negative += determinant_positive_1;
    determinant = determinant_positive_0 - determinant_negative;

    /* 原函数无奇异性保护，且每项均以独立 divsd 完成。 */
    output[0] = (a11 * a22 - a12 * a21) / determinant;
    output[1] = (a21 * a02 - a01 * a22) / determinant;
    output[2] = (a01 * a12 - a02 * a11) / determinant;
    output[3] = (a12 * a20 - a22 * a10) / determinant;
    output[4] = (a22 * a00 - a02 * a20) / determinant;
    output[5] = (a02 * a10 - a00 * a12) / determinant;
    output[6] = (a10 * a21 - a20 * a11) / determinant;
    output[7] = (a20 * a01 - a00 * a21) / determinant;
    output[8] = (a00 * a11 - a10 * a01) / determinant;
}

/* 原 0x66b0/0x66d0：固定状态维度 33，前向逐元素读写。 */
void array_copy(double destination[33], const double source[33])
{
    unsigned index;
    for (index = 0u; index < 33u; ++index) {
        destination[index] = source[index];
    }
}

void array_sum(const double source[33], double destination[33], double alpha)
{
    unsigned index;
    for (index = 0u; index < 33u; ++index) {
        destination[index] = source[index] * alpha + destination[index];
    }
}

double Frac(double value)
{
    return value - floor(value);
}

double rng_uniform_pos(void)
{
    double value;
    int32_t random_value;
    do {
        if (dp_rng_next(&random_value) != 0) return 0.5;
        value = (double)random_value / (double)RAND_MAX;
    } while (value == 0.0 || value == 1.0);
    return value;
}

double NiceAngle(double x, double y)
{
    const double half_pi = 0x1.921fb54442d18p+0;
    const double three_half_pi = 0x1.2d97c7f3321d2p+2;
    const double pi = 0x1.921fb54442d18p+1;
    const double two_pi = 0x1.921fb54442d18p+2;

    if (x > 0.0) {
        if (y > 0.0) {
            return atan(y / x);
        }
        if (y < 0.0) {
            return atan(y / x) + two_pi;
        }
        return 0.0;
    }
    if (x < 0.0) {
        return atan(y / x) + pi;
    }
    if (x == 0.0) {
        if (y > 0.0) {
            return half_pi;
        }
        if (y < 0.0) {
            return three_half_pi;
        }
    }
    return 0.0;
}

double s2(double angle)
{
    return sin(angle * 0.5);
}

double c2(double angle)
{
    return cos(angle * 0.5);
}

void Rotation_X(DpMatrix *out_3x3, double angle)
{
    double sine;
    double cosine;
    double *out = out_3x3->data;

    sincos(angle, &sine, &cosine);
    out[0] = 1.0;    out[1] = 0.0;    out[2] = 0.0;
    out[3] = 0.0;    out[4] = cosine; out[5] = sine;
    out[6] = 0.0;    out[7] = -sine;  out[8] = cosine;
}

void Rotation_Y(DpMatrix *out_3x3, double angle)
{
    double sine;
    double cosine;
    double *out = out_3x3->data;

    sincos(angle, &sine, &cosine);
    out[0] = cosine; out[1] = 0.0;    out[2] = -sine;
    out[3] = 0.0;    out[4] = 1.0;    out[5] = 0.0;
    out[6] = sine;   out[7] = 0.0;    out[8] = cosine;
}

void Rotation_Z(DpMatrix *out_3x3, double angle)
{
    double sine;
    double cosine;
    double *out = out_3x3->data;

    sincos(angle, &sine, &cosine);
    out[0] = cosine;  out[1] = sine;   out[2] = 0.0;
    out[3] = -sine;   out[4] = cosine; out[5] = 0.0;
    out[6] = 0.0;     out[7] = 0.0;    out[8] = 1.0;
}

/* 原 0x2e00：四个连续 double 的前向复制；无返回值和无形状检查。 */
void quat_cpy(double destination[4], const double source[4])
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = source[3];
}

/* 原 0x9170：打印 scalar 与 embedded xyz；返回最后一次 printf 的字符数。 */
int quat_print(const DpQuatAbi *quat)
{
    (void)printf("\nqs: %f\n", quat->w);
    (void)printf("qv.x: %f\n", quat->xyz.data[0]);
    (void)printf("qv.y: %f\n", quat->xyz.data[1]);
    return printf("qv.z: %f\n\n", quat->xyz.data[2]);
}

void quat_xi(DpMatrix *out_4x3, const DpQuatAbi *quat)
{
    const double w = quat->w;
    const double x = quat->xyz.data[0];
    const double y = quat->xyz.data[1];
    const double z = quat->xyz.data[2];
    double *m = out_4x3->data;

    /* quat_xi 的连续行主序 4×3 写入，未见额外维度检查。 */
    m[0 * out_4x3->row_stride + 0] = w;
    m[0 * out_4x3->row_stride + 1] = -z;
    m[0 * out_4x3->row_stride + 2] = y;
    m[1 * out_4x3->row_stride + 0] = z;
    m[1 * out_4x3->row_stride + 1] = w;
    m[1 * out_4x3->row_stride + 2] = -x;
    m[2 * out_4x3->row_stride + 0] = -y;
    m[2 * out_4x3->row_stride + 1] = x;
    m[2 * out_4x3->row_stride + 2] = w;
    m[3 * out_4x3->row_stride + 0] = -x;
    m[3 * out_4x3->row_stride + 1] = -y;
    m[3 * out_4x3->row_stride + 2] = -z;
}

void AxisVector2Matrix(DpMatrix *out_3x3, const DpVector *axis_x,
                       const DpVector *axis_y, const DpVector *axis_z)
{
    double *out = out_3x3->data;
    out[0] = axis_x->data[0];
    out[1] = axis_x->data[1];
    out[2] = axis_x->data[2];
    out[3] = axis_y->data[0];
    out[4] = axis_y->data[1];
    out[5] = axis_y->data[2];
    out[6] = axis_z->data[0];
    out[7] = axis_z->data[1];
    out[8] = axis_z->data[2];
}

void vector3_to_matrix(DpMatrix *out_3x3)
{
    double left_data[3] = {0.0, 0.0, 0.0};
    double right_data[3] = {0.0, 0.0, 0.0};
    DpMatrix left = {3, 1, 1, 0, left_data};
    DpMatrix right = {1, 3, 3, 0, right_data};

    /* 原函数本身仅把两个全零局部矩阵送入 blas_gemm。 */
    (void)blas_gemm(&left, &right, out_3x3, 1.0, 0.0);
}

void euler_angle_312_to_quat(DpQuatAbi *out, double angle_a,
                             double angle_b, double angle_c)
{
    double sine_a;
    double cosine_a;
    double sine_b;
    double cosine_b;
    double sine_c;
    double cosine_c;

    sine_a = sin(angle_a * 0.5);
    cosine_a = cos(angle_a * 0.5);
    sine_b = sin(angle_b * 0.5);
    cosine_b = cos(angle_b * 0.5);
    sine_c = sin(angle_c * 0.5);
    cosine_c = cos(angle_c * 0.5);
    out->w = cosine_a * cosine_b * cosine_c - sine_a * sine_b * sine_c;
    out->xyz.data[0] = cosine_a * sine_b * cosine_c - sine_a * cosine_b * sine_c;
    out->xyz.data[1] = cosine_a * cosine_b * sine_c + sine_a * sine_b * cosine_c;
    out->xyz.data[2] = sine_a * cosine_b * cosine_c + cosine_a * sine_b * sine_c;
}

static void dp_quat_matrix_legacy_fault(double w)
{
    /*
     * 原 quat2matrix 的局部 matrix descriptor 在 quat_psi 中将 +0x10
     * 的下一 descriptor 元数据 0x0000000300000004 误作 data 指针。
     */
    volatile uintptr_t legacy_matrix_data = UINT64_C(0x0000000300000004);
    volatile double *legacy_output = (volatile double *)legacy_matrix_data;
    *legacy_output = w;
}

void quat_to_euler_angle_312(double *angle_a, double *angle_b, double *angle_c,
                             const DpQuatAbi *quat)
{
    (void)angle_a;
    (void)angle_b;
    (void)angle_c;
    dp_quat_matrix_legacy_fault(quat->w);
}

void quat2matrix(DpMatrix *out_3x3, const DpQuatAbi *quat)
{
    /* 原函数在第一次使用 out_3x3 前即于 quat_psi 终止。 */
    (void)out_3x3;
    dp_quat_matrix_legacy_fault(quat->w);
}

void Cartesian2Polar(double *elevation, double *azimuth, double *radius,
                     const DpVector *cartesian)
{
    const double x = cartesian->data[0];
    const double y = cartesian->data[1];
    const double z = cartesian->data[2];
    const double planar_squared = x * x + y * y;
    const double planar_norm = sqrt(planar_squared);

    *elevation = atan2(z, planar_norm);
    *azimuth = atan2(y, x);
    *radius = sqrt(z * z + planar_squared);
}

void matrix2quat(DpQuatAbi *out, const DpMatrix *matrix)
{
    const double *m = matrix->data;
    const double m00 = m[0];
    const double m01 = m[1];
    const double m02 = m[2];
    const double m10 = m[3];
    const double m11 = m[4];
    const double m12 = m[5];
    const double m20 = m[6];
    const double m21 = m[7];
    const double m22 = m[8];
    const double trace = m00 + m11 + m22;
    double root;

    if (trace >= m00 && trace >= m11 && trace >= m22) {
        root = sqrt(trace + 1.0);
        out->w = root * 0.5;
        out->xyz.data[0] = (m12 - m21) * 0.5 / root;
        out->xyz.data[1] = (m20 - m02) * 0.5 / root;
        out->xyz.data[2] = (m01 - m10) * 0.5 / root;
    } else if (m00 >= m11 && m00 >= m22) {
        root = sqrt(1.0 + m00 - m11 - m22);
        out->w = (m12 - m21) * 0.5 / root;
        out->xyz.data[0] = root * 0.5;
        out->xyz.data[1] = (m01 + m10) * 0.5 / root;
        out->xyz.data[2] = (m20 + m02) * 0.5 / root;
    } else if (m11 >= m22) {
        root = sqrt(1.0 - m00 + m11 - m22);
        out->w = (m20 - m02) * 0.5 / root;
        out->xyz.data[0] = (m01 + m10) * 0.5 / root;
        out->xyz.data[1] = root * 0.5;
        out->xyz.data[2] = (m12 + m21) * 0.5 / root;
    } else {
        root = sqrt(1.0 - m00 - m11 + m22);
        out->w = (m01 - m10) * 0.5 / root;
        out->xyz.data[0] = (m20 + m02) * 0.5 / root;
        out->xyz.data[1] = (m12 + m21) * 0.5 / root;
        out->xyz.data[2] = root * 0.5;
    }
}

/* 原 quat_psi：不读取或修改输出矩阵 descriptor 字段，只按固定 4×3
 * 行主序将 w/x/y/z 及其符号翻转写入 data backing。 */
void quat_psi(DpMatrix *out_4x3, const DpQuatAbi *quat)
{
    const double w = quat->w;
    const double x = quat->xyz.data[0];
    const double y = quat->xyz.data[1];
    const double z = quat->xyz.data[2];
    double *out = out_4x3->data;

    out[0] = w;
    out[1] = z;
    out[2] = -y;
    out[3] = -z;
    out[4] = w;
    out[5] = x;
    out[6] = y;
    out[7] = -x;
    out[8] = w;
    out[9] = -x;
    out[10] = -y;
    out[11] = -z;
}

void quat_att_mat(DpMatrix *out_3x3, const DpQuatAbi *quat)
{
    const double w = quat->w;
    const double x = quat->xyz.data[0];
    const double y = quat->xyz.data[1];
    const double z = quat->xyz.data[2];
    double psi_data[12] = {0.0};
    double xi_data[12] = {0.0};
    double xi_trans_data[12] = {0.0};
    DpMatrix psi = {4, 3, 3, 0, psi_data};
    DpMatrix xi = {4, 3, 3, 0, xi_data};
    DpMatrix xi_trans = {3, 4, 4, 0, xi_trans_data};

    (void)w;
    (void)x;
    (void)y;
    (void)z;
    /* 原 0x8fb7：通过同名辅助导出构造 psi，保留非有限值的符号传播。 */
    quat_psi(&psi, quat);
    quat_xi(&xi, quat);
    (void)matrix_trans(&xi_trans, &xi);
    (void)blas_gemm(&xi_trans, &psi, out_3x3, 1.0, 0.0);
}

void quat_diff(DpQuatAbi *out, const DpQuatAbi *quat, const DpVector *angular_rate)
{
    double xi_data[12] = {0.0};
    double derivative_data[4] = {0.0};
    DpMatrix xi = {4, 3, 3, 0, xi_data};
    DpVector derivative = {4, 0, derivative_data};

    quat_xi(&xi, quat);
    (void)blas_gemv(&xi, angular_rate, &derivative, 0.5, 0.0);
    /* 原函数将前三项写至嵌入 xyz，第四项写至标量 w。 */
    out->xyz.data[0] = derivative.data[0];
    out->xyz.data[1] = derivative.data[1];
    out->xyz.data[2] = derivative.data[2];
    out->w = derivative.data[3];
}

void quat_conjugate(DpQuatAbi *out, const DpQuatAbi *in)
{
    out->w = in->w;
    out->xyz.data[0] = -in->xyz.data[0];
    out->xyz.data[1] = -in->xyz.data[1];
    out->xyz.data[2] = -in->xyz.data[2];
}

double quat_norm(const DpQuatAbi *quat)
{
    return sqrt(quat->w * quat->w + quat->xyz.data[0] * quat->xyz.data[0] +
                quat->xyz.data[1] * quat->xyz.data[1] +
                quat->xyz.data[2] * quat->xyz.data[2]);
}

void quat_reunit(DpQuatAbi *quat_inout)
{
    /* 原 ELF 先计算范数，再无条件对标量及嵌入向量调用除法／缩放。 */
    const double norm = quat_norm(quat_inout);
    quat_inout->w /= norm;
    (void)vector_scale(&quat_inout->xyz, 1.0 / norm);
}
