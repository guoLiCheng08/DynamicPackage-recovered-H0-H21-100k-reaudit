/* DynamicPackage 时间模块：依据 Calc_JD / GMST 反汇编逐步恢复。 */
#ifndef DYNAMIC_TIME_H
#define DYNAMIC_TIME_H

#include "dynamic_math.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 原坐标转换函数读取的连续 6×double 日历时间对象。 */
typedef struct {
    double year;
    double month;
    double day;
    double hour;
    double minute;
    double second;
} DpCalendarTime;

/* 原全局日历状态 API：TimeInit 不重置累计秒或小数秒残量。 */
void TimeInit(double year, double month, double day,
              double hour, double minute, double second);
void TimeAdd(double seconds);
void TimeArrayGet(double out_calendar[6]);
double TimeTotalGet(void);

/* 原符号 Calc_JD：year, month, day, hour, minute, second → Julian Date。 */
double Calc_JD(double year, double month, double day,
               double hour, double minute, double second);
/* 原 ELF ABI：6 个 double 分别位于 xmm0..xmm5，返回从 J2000 起的儒略世纪。 */
double JulianCenturies(double year, double month, double day,
                       double hour, double minute, double second);

/* 原符号 Calc_theta_GMST：年、月、日、时、分、秒 → GMST 弧度。 */
double Calc_theta_GMST(double year, double month, double day,
                       double hour, double minute, double second);

/* 原符号 Calc_Precession：以儒略世纪为输入，写入 3×3 岁差矩阵。 */
void Calc_Precession(double julian_centuries, DpMatrix *out_3x3);

/* 原符号 GCI2ECEF：按指定日历时间将 3D GCI 向量转换至 ECEF。 */
void GCI2ECEF(DpVector *out_ecef, const DpVector *in_gci,
              const DpCalendarTime *calendar_time);

/* 原符号 ECEF2GCI：按指定日历时间将 3D ECEF 向量转换至 GCI。 */
void ECEF2GCI(DpVector *out_gci, const DpVector *in_ecef,
              const DpCalendarTime *calendar_time);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_TIME_H */
