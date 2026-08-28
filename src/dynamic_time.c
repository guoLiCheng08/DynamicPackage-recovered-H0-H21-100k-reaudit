#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dynamic_time.h"

#include <math.h>
#include <time.h>

/*
 * 原 Calc_JD 的常量与顺序：
 * 9, 12, 1.75, 275, 367, 1721013.5, 60, 1440。
 * floor() 在负值路径等效于原汇编的截断后修正序列。
 */
double Calc_JD(double year, double month, double day,
               double hour, double minute, double second)
{
    const double month_term = floor((month + 9.0) / 12.0);
    const double correction = floor(1.75 * (year + month_term));
    const double month_days = floor(275.0 * month / 9.0);
    const double minutes_total = minute + second / 60.0;
    const double hours_total = hour + minutes_total / 60.0;

    return 367.0 * year - correction + month_days + day + 1721013.5 +
           hours_total / 24.0;
}

/* 原 JulianCenturies：直接调用 Calc_JD，再以 J2000 日历儒略日和
 * 一个儒略世纪天数进行减法、除法。 */
double JulianCenturies(double year, double month, double day,
                       double hour, double minute, double second)
{
    double julian_date = Calc_JD(year, month, day, hour, minute, second);

    julian_date -= 2451545.0;
    julian_date /= 36525.0;
    return julian_date;
}

/*
 * 原 Calc_theta_GMST 先以当日 00:00:00 调用 Calc_JD，随后以世纪数
 * 计算恒星秒多项式并加入 UTC 时刻的 1.0027379093507951 倍，再按
 * 86400 秒取模，最后除 240 并乘 π/180 转为弧度。
 */
double Calc_theta_GMST(double year, double month, double day,
                       double hour, double minute, double second)
{
    double centuries = Calc_JD(year, month, day, 0.0, 0.0, 0.0);
    double polynomial_linear;
    double polynomial_quadratic;
    double polynomial_cubic;
    double clock_seconds;
    double theta_seconds;

    centuries = (centuries - 2451545.0) / 36525.0;

    clock_seconds = hour * 3600.0;
    clock_seconds += minute * 60.0;
    clock_seconds += second;
    clock_seconds *= 1.0027379093507951;

    polynomial_linear = 8640184.8128660005 * centuries;
    polynomial_quadratic = 0.093104000000000006 * centuries;
    polynomial_linear += 24110.548409999999;
    polynomial_quadratic *= centuries;
    polynomial_linear += polynomial_quadratic;
    polynomial_cubic = 0.0000061999999999999999 * centuries;
    polynomial_cubic *= centuries;
    polynomial_cubic *= centuries;
    polynomial_linear -= polynomial_cubic;

    theta_seconds = fmod(polynomial_linear + clock_seconds, 86400.0);
    return theta_seconds / 240.0 * 0.017453292519943295;
}

/* 原 Rotation_Z 使用的行主序被动 Z 旋转矩阵。 */
static void dp_time_rotation_z(double angle, double out[9])
{
    double sine;
    double cosine;
    sincos(angle, &sine, &cosine);
    out[0] = cosine;
    out[1] = sine;
    out[2] = 0.0;
    out[3] = -sine;
    out[4] = cosine;
    out[5] = 0.0;
    out[6] = 0.0;
    out[7] = 0.0;
    out[8] = 1.0;
}

/* 原 Rotation_Y 使用的行主序被动 Y 旋转矩阵。 */
static void dp_time_rotation_y(double angle, double out[9])
{
    double sine;
    double cosine;
    sincos(angle, &sine, &cosine);
    out[0] = cosine;
    out[1] = 0.0;
    out[2] = -sine;
    out[3] = 0.0;
    out[4] = 1.0;
    out[5] = 0.0;
    out[6] = sine;
    out[7] = 0.0;
    out[8] = cosine;
}

/*
 * 原 Calc_Precession：
 * Rz(-z) · Ry(theta) · Rz(-zeta)，角度系数均已由 ELF .rodata 解码。
 * 中间矩阵乘法委托给与其它恢复块共享的 blas_gemm，以固定求和顺序。
 */
void Calc_Precession(double julian_centuries, DpMatrix *out_3x3)
{
    double zeta;
    double theta;
    double z;
    double left_data[9] = {0.0};
    double middle_data[9] = {0.0};
    double right_data[9] = {0.0};
    double product_data[9] = {0.0};
    const DpMatrix left = {3, 3, 3, 0, left_data};
    const DpMatrix middle = {3, 3, 3, 0, middle_data};
    const DpMatrix right = {3, 3, 3, 0, right_data};
    DpMatrix product = {3, 3, 3, 0, product_data};

    zeta = 1.4635555405334672e-06 * julian_centuries * julian_centuries;
    zeta += 0.011180861349838081 * julian_centuries;
    zeta += 8.7256766326094288e-08 * julian_centuries * julian_centuries *
            julian_centuries;

    theta = -2.068457570453835e-06 * julian_centuries * julian_centuries;
    theta += 0.0097171734551696701 * julian_centuries;
    theta += -2.028121072185522e-07 * julian_centuries * julian_centuries *
             julian_centuries;

    z = 3.8436028638364008e-06 * julian_centuries * julian_centuries;
    z += zeta;
    z += 9.9386804627454871e-10 * julian_centuries * julian_centuries *
         julian_centuries;

    dp_time_rotation_z(-z, left_data);
    dp_time_rotation_y(theta, middle_data);
    dp_time_rotation_z(-zeta, right_data);
    (void)blas_gemm(&middle, &right, &product, 1.0, 0.0);
    (void)blas_gemm(&left, &product, out_3x3, 1.0, 0.0);
}

/*
 * 原 GCI2ECEF 构造 Rz(GMST)、当日儒略世纪岁差矩阵，先相乘再乘输入向量。
 * 所有局部矩阵均为连续行主序 3×3，调用顺序与反汇编一致。
 */
void GCI2ECEF(DpVector *out_ecef, const DpVector *in_gci,
              const DpCalendarTime *calendar_time)
{
    double earth_rotation_data[9] = {0.0};
    double precession_data[9] = {0.0};
    double combined_data[9] = {0.0};
    DpMatrix earth_rotation = {3, 3, 3, 0, earth_rotation_data};
    DpMatrix precession = {3, 3, 3, 0, precession_data};
    DpMatrix combined = {3, 3, 3, 0, combined_data};
    double theta;
    double centuries;

    theta = Calc_theta_GMST(calendar_time->year, calendar_time->month,
                            calendar_time->day, calendar_time->hour,
                            calendar_time->minute, calendar_time->second);
    dp_time_rotation_z(theta, earth_rotation_data);

    centuries = Calc_JD(calendar_time->year, calendar_time->month,
                        calendar_time->day, calendar_time->hour,
                        calendar_time->minute, calendar_time->second);
    centuries = (centuries - 2451545.0) / 36525.0;
    Calc_Precession(centuries, &precession);

    (void)blas_gemm(&earth_rotation, &precession, &combined, 1.0, 0.0);
    (void)blas_gemv(&combined, in_gci, out_ecef, 1.0, 0.0);
}

/*
 * 原 ECEF2GCI 先构造与 GCI2ECEF 相同的组合矩阵，再显式转置后乘 ECEF
 * 输入向量；不以代数简化替代该顺序，以保留浮点运算轨迹。
 */
void ECEF2GCI(DpVector *out_gci, const DpVector *in_ecef,
              const DpCalendarTime *calendar_time)
{
    double earth_rotation_data[9] = {0.0};
    double precession_data[9] = {0.0};
    double combined_data[9] = {0.0};
    double transposed_data[9] = {0.0};
    DpMatrix earth_rotation = {3, 3, 3, 0, earth_rotation_data};
    DpMatrix precession = {3, 3, 3, 0, precession_data};
    DpMatrix combined = {3, 3, 3, 0, combined_data};
    DpMatrix transposed = {3, 3, 3, 0, transposed_data};
    double theta;
    double centuries;

    theta = Calc_theta_GMST(calendar_time->year, calendar_time->month,
                            calendar_time->day, calendar_time->hour,
                            calendar_time->minute, calendar_time->second);
    dp_time_rotation_z(theta, earth_rotation_data);

    centuries = Calc_JD(calendar_time->year, calendar_time->month,
                        calendar_time->day, calendar_time->hour,
                        calendar_time->minute, calendar_time->second);
    centuries = (centuries - 2451545.0) / 36525.0;
    Calc_Precession(centuries, &precession);

    (void)blas_gemm(&earth_rotation, &precession, &combined, 1.0, 0.0);
    (void)matrix_trans(&transposed, &combined);
    (void)blas_gemv(&transposed, in_ecef, out_gci, 1.0, 0.0);
}

/*
 * TimeInit/TimeAdd 原本通过 BSS 中的 tm_canlender、second_decimal、
 * second_total 与 CalenderTimeArray 协作。这里保留其跨 TimeInit 的小数秒
 * 残量和累计秒语义；只有小数秒达到 1 时才调用 mktime 归一化整秒进位。
 */
static struct tm dp_time_calendar_tm;
static double dp_time_second_decimal;
static double dp_time_second_total;
static double dp_time_calendar_array[6];

void TimeInit(double year, double month, double day,
              double hour, double minute, double second)
{
    dp_time_calendar_array[0] = year;
    dp_time_calendar_array[1] = month;
    dp_time_calendar_array[2] = day;
    dp_time_calendar_array[3] = hour;
    dp_time_calendar_array[4] = minute;
    dp_time_calendar_array[5] = second;

    dp_time_calendar_tm.tm_year = (int)(year - 1900.0);
    dp_time_calendar_tm.tm_mon = (int)month - 1;
    dp_time_calendar_tm.tm_mday = (int)day;
    dp_time_calendar_tm.tm_hour = (int)hour;
    dp_time_calendar_tm.tm_min = (int)minute;
    dp_time_calendar_tm.tm_sec = (int)second;
}

void dp_time_seed(const double calendar[6], double second_decimal,
                  double second_total)
{
    if (calendar == NULL) return;
    TimeInit(calendar[0], calendar[1], calendar[2], calendar[3],
             calendar[4], calendar[5] - floor(calendar[5]));
    dp_time_calendar_array[0] = calendar[0];
    dp_time_calendar_array[1] = calendar[1];
    dp_time_calendar_array[2] = calendar[2];
    dp_time_calendar_array[3] = calendar[3];
    dp_time_calendar_array[4] = calendar[4];
    dp_time_calendar_array[5] = calendar[5];
    dp_time_second_decimal = second_decimal;
    dp_time_second_total = second_total;
}

void TimeAdd(double seconds)
{
    dp_time_second_decimal += seconds;
    if (dp_time_second_decimal >= 1.0) {
        const int whole_seconds = (int)floor(dp_time_second_decimal);
        dp_time_second_decimal -= (double)whole_seconds;
        dp_time_calendar_tm.tm_sec += whole_seconds;
        (void)mktime(&dp_time_calendar_tm);
    }

    dp_time_second_total += seconds;
    dp_time_calendar_array[0] = (double)(dp_time_calendar_tm.tm_year + 1900);
    dp_time_calendar_array[1] = (double)(dp_time_calendar_tm.tm_mon + 1);
    dp_time_calendar_array[2] = (double)dp_time_calendar_tm.tm_mday;
    dp_time_calendar_array[3] = (double)dp_time_calendar_tm.tm_hour;
    dp_time_calendar_array[4] = (double)dp_time_calendar_tm.tm_min;
    dp_time_calendar_array[5] = (double)dp_time_calendar_tm.tm_sec +
                                 dp_time_second_decimal;
}

void TimeArrayGet(double out_calendar[6])
{
    out_calendar[0] = dp_time_calendar_array[0];
    out_calendar[1] = dp_time_calendar_array[1];
    out_calendar[2] = dp_time_calendar_array[2];
    out_calendar[3] = dp_time_calendar_array[3];
    out_calendar[4] = dp_time_calendar_array[4];
    out_calendar[5] = dp_time_calendar_array[5];
}

double TimeTotalGet(void)
{
    return dp_time_second_total;
}
