/* DynamicPackage Gyro / MagMeter 恢复实现。 */
#include "dynamic_sensors.h"

#include <math.h>
#include <stdlib.h>

/* 原 ELF 全局 STS：3 个连续 0x170 字节对象。 */
DpStsRecovered STS[3];
/* 原 ELF 全局 Gyro：2 个连续 0x320 字节对象。 */
DpGyroRecovered Gyro[2];
/* 原 ELF 全局 MagMeter：2 个连续 0x138 字节对象。 */
DpMagMeterRecovered MagMeter[2];
/* 原 ELF 全局 DSS：2 个连续 0x158 字节对象。 */
DpDssRecovered DSS[2];
int temp;
/* 原 ELF 全局 GPS 对象与初始化标志。 */
DpGpsKalmanRecovered GPS_Kalman;
int init_flag;

/* 原 Gyro_Init：每个 0x320 对象均将配置源、转置输出与测量向量
 * 指向固定对象内 backing，随后执行 matrix_trans(output, source)。 */
void Gyro_Init(void)
{
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        unsigned char *base = (unsigned char *)&Gyro[index];
        Gyro[index].installation_matrix.data = (double *)(base + 0x80u);
        Gyro[index].projection_matrix.data = (double *)(base + 0xc8u);
        Gyro[index].measure.data = (double *)(base + 0x128u);
        (void)matrix_trans(&Gyro[index].projection_matrix,
                           &Gyro[index].installation_matrix);
    }
}

/* 原 MagMeter_Init 与 Gyro_Init 同构，但对象步长为 0x138，三个 backing
 * 偏移分别为 0x78、0xc0、0x120。 */
void MagMeter_Init(void)
{
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        unsigned char *base = (unsigned char *)&MagMeter[index];
        MagMeter[index].installation_matrix.data = (double *)(base + 0x78u);
        MagMeter[index].projection_matrix.data = (double *)(base + 0xc0u);
        MagMeter[index].measure.data = (double *)(base + 0x120u);
        (void)matrix_trans(&MagMeter[index].projection_matrix,
                           &MagMeter[index].installation_matrix);
    }
}

/* 原 DSS_Init：两个 0x158 对象分别以 +0x70、+0xb8、+0x120 作为
 * 配置源矩阵、转置输出矩阵和测量向量 backing。 */
void DSS_Init(void)
{
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        unsigned char *base = (unsigned char *)&DSS[index];
        DSS[index].installation_matrix.data = (double *)(base + 0x70u);
        DSS[index].projection_matrix.data = (double *)(base + 0xb8u);
        DSS[index].measure.data = (double *)(base + 0x120u);
        (void)matrix_trans(&DSS[index].projection_matrix,
                           &DSS[index].installation_matrix);
    }
}

/* 原 STS_Init：每个 0x170 对象先绑定 3×3 安装矩阵和三个四元数 xyz
 * backing，再以 matrix2quat 产生安装姿态，以 quat_conjugate 产生误差姿态。 */
void STS_Init(void)
{
    unsigned index;

    for (index = 0u; index < 3u; ++index) {
        unsigned char *base = (unsigned char *)&STS[index];

        STS[index].installation_matrix.data = (double *)(base + 0x78u);
        STS[index].installation_quat.xyz.count = 3;
        STS[index].installation_quat.xyz.reserved_04 = 0;
        STS[index].installation_quat.xyz.data = (double *)(base + 0xe0u);
        STS[index].error_quat.xyz.count = 3;
        STS[index].error_quat.xyz.reserved_04 = 0;
        STS[index].error_quat.xyz.data = (double *)(base + 0x118u);
        STS[index].measure_quat.w = 1.0;
        STS[index].measure_quat.xyz.count = 3;
        STS[index].measure_quat.xyz.reserved_04 = 0;
        STS[index].measure_quat.xyz.data = (double *)(base + 0x158u);
        matrix2quat(&STS[index].installation_quat, &STS[index].installation_matrix);
        quat_conjugate(&STS[index].error_quat, &STS[index].installation_quat);
    }
}

double dp_ran_gaussian_recovered(double sigma)
{
    double x;
    double y;
    double radius_squared;

    do {
        do {
            x = (double)rand() / 2147483647.0;
        } while (x == 0.0 || x == 1.0);
        x = x + x;
        x = x - 1.0;
        do {
            y = (double)rand() / 2147483647.0;
        } while (y == 0.0 || y == 1.0);
        y = y + y;
        y = y - 1.0;
        radius_squared = x * x;
        radius_squared += y * y;
    } while (radius_squared > 1.0 || radius_squared == 0.0);
    y *= sigma;
    return y * sqrt((-2.0 * log(radius_squared)) / radius_squared);
}

double dp_ran_gaussian2(double mean, double sigma)
{
    return dp_ran_gaussian_recovered(sigma) + mean;
}

double ran_gaussian(double sigma)
{
    return dp_ran_gaussian_recovered(sigma);
}

double ran_gaussian2(double mean, double sigma)
{
    return ran_gaussian(sigma) + mean;
}

static void update_sensor_vector(DpMatrix *projection_matrix, DpVector *measure,
                                 const DpVec3 *input, uint32_t noise_flag,
                                 const double mean[3], const double sigma[3],
                                 DpGaussian2Sampler gaussian2, void *opaque)
{
    double input_data[3] = {input->x, input->y, input->z};
    DpVector source = {3, 0, input_data};
    unsigned index;

    /* UpdateGyro/UpdateMagMeter：alpha=1，beta=0 的 GEMV 先完成无噪声投影。 */
    (void)blas_gemv(projection_matrix, &source, measure, 1.0, 0.0);
    if (noise_flag == 1u) {
        for (index = 0; index < 3u; ++index) {
            const double noise = gaussian2 != NULL ? gaussian2(mean[index], sigma[index], opaque) :
                                                    dp_ran_gaussian2(mean[index], sigma[index]);
            measure->data[index] += noise;
        }
    }
}

/* 原 ELF ABI：rdi=三元素 DpVector，更新全局 Gyro[2]。 */
void UpdateGyro(const DpVector *body_rate)
{
    DpVec3 input;

    input.x = body_rate->data[0];
    input.y = body_rate->data[1];
    input.z = body_rate->data[2];
    dp_update_gyro(Gyro, &input, NULL, NULL);
}

void dp_update_gyro(DpGyroRecovered gyros[2], const DpVec3 *body_rate,
                    DpGaussian2Sampler gaussian2, void *opaque)
{
    unsigned index;
    for (index = 0; index < 2u; ++index) {
        update_sensor_vector(&gyros[index].projection_matrix, &gyros[index].measure,
                             body_rate, gyros[index].gaussian_noise_flag,
                             gyros[index].gaussian_mean, gyros[index].gaussian_sigma,
                             gaussian2, opaque);
    }
}

/* 原 ELF ABI：rdi=三元素 DpVector，更新全局 MagMeter[2]。 */
void UpdateMagMeter(const DpVector *magnetic_body)
{
    DpVec3 input;

    input.x = magnetic_body->data[0];
    input.y = magnetic_body->data[1];
    input.z = magnetic_body->data[2];
    dp_update_magmeter(MagMeter, &input, NULL, NULL);
}

void dp_update_magmeter(DpMagMeterRecovered meters[2], const DpVec3 *magnetic_body,
                        DpGaussian2Sampler gaussian2, void *opaque)
{
    unsigned index;
    for (index = 0; index < 2u; ++index) {
        update_sensor_vector(&meters[index].projection_matrix, &meters[index].measure,
                             magnetic_body, meters[index].gaussian_noise_flag,
                             meters[index].gaussian_mean, meters[index].gaussian_sigma,
                             gaussian2, opaque);
    }
}

void dp_update_dss(DpDssRecovered sensors[2], const DpVec3 *sun_body,
                   DpGaussian2Sampler gaussian2, void *opaque)
{
    unsigned sensor_index;

    if (sensors == NULL || sun_body == NULL) {
        return;
    }
    for (sensor_index = 0u; sensor_index < 2u; ++sensor_index) {
        DpDssRecovered *sensor = &sensors[sensor_index];
        double input_data[3] = {sun_body->x, sun_body->y, sun_body->z};
        DpVector input = {3, 0, input_data};
        double z_value;

        if (sensor->projection_matrix.data == NULL || sensor->measure.data == NULL ||
            sensor->projection_matrix.rows != 3 || sensor->projection_matrix.cols != 3 ||
            sensor->projection_matrix.row_stride < 3 || sensor->measure.count != 3) {
            continue;
        }
        if (blas_gemv(&sensor->projection_matrix, &input, &sensor->measure, 1.0, 0.0) != 0) {
            continue;
        }
        z_value = sensor->measure.data[2];
        if (z_value > 0.0) {
            double angle_x;
            double angle_y;
            const double scale = 1.0 / z_value;

            sensor->measure.data[0] *= scale;
            sensor->measure.data[1] *= scale;
            sensor->measure.data[2] *= scale;
            angle_x = atan2(sensor->measure.data[0], sensor->measure.data[2]);
            if (sensor->gaussian_noise_flag != 0u) {
                angle_x += gaussian2 != NULL ? gaussian2(sensor->gaussian_mean[0], sensor->gaussian_sigma[0], opaque) :
                                               dp_ran_gaussian2(sensor->gaussian_mean[0], sensor->gaussian_sigma[0]);
            }
            angle_y = atan2(sensor->measure.data[1], sensor->measure.data[2]);
            if (sensor->gaussian_noise_flag != 0u) {
                angle_y += gaussian2 != NULL ? gaussian2(sensor->gaussian_mean[1], sensor->gaussian_sigma[1], opaque) :
                                               dp_ran_gaussian2(sensor->gaussian_mean[1], sensor->gaussian_sigma[1]);
            }
            sensor->angle_x = angle_x;
            sensor->angle_y = angle_y;
            if (sensor->gaussian_noise_flag != 0u) {
                sensor->ratio_x = tan(sensor->angle_x);
                sensor->ratio_y = tan(sensor->angle_y);
            } else {
                sensor->ratio_x = sensor->measure.data[0];
                sensor->ratio_y = sensor->measure.data[1];
            }
        }
    }
}

/* 原 ELF ABI：rdi 为 3 元素 DpVector；内部两次迭代均推进全局 temp。 */
void UpdateDSS(const DpVector *sun_body)
{
    DpVec3 input;

    input.x = sun_body->data[0];
    input.y = sun_body->data[1];
    input.z = sun_body->data[2];
    dp_update_dss(DSS, &input, NULL, NULL);
    temp += 2;
}

static int dp_quat_components_valid(const DpQuatAbi *quat)
{
    return quat != NULL && quat->xyz.count == 3 && quat->xyz.data != NULL;
}

static void dp_quat_cross_left(DpQuatAbi *out, const DpQuatAbi *left,
                               const DpQuatAbi *right)
{
    /* 原 quat_cross 的实际算术路径（left × right）：每个分量先计算
     * right×left 的叉积、乘 -1，再依次执行两次 AXPY。显式保留该顺序以匹配
     * 原 ELF 在非恒等四元数输入下的 IEEE-754 舍入。 */
    const double *left_xyz = left->xyz.data;
    const double *right_xyz = right->xyz.data;
    double *out_xyz = out->xyz.data;
    double first;
    double second;
    double dot = 0.0;

    first = right_xyz[1] * left_xyz[2];
    second = right_xyz[2] * left_xyz[1];
    out_xyz[0] = first - second;
    out_xyz[0] = out_xyz[0] * -1.0;
    first = left_xyz[0] * right->w;
    out_xyz[0] = first + out_xyz[0];
    first = right_xyz[0] * left->w;
    out_xyz[0] = first + out_xyz[0];

    first = right_xyz[2] * left_xyz[0];
    second = right_xyz[0] * left_xyz[2];
    out_xyz[1] = first - second;
    out_xyz[1] = out_xyz[1] * -1.0;
    first = left_xyz[1] * right->w;
    out_xyz[1] = first + out_xyz[1];
    first = right_xyz[1] * left->w;
    out_xyz[1] = first + out_xyz[1];

    first = right_xyz[0] * left_xyz[1];
    second = right_xyz[1] * left_xyz[0];
    out_xyz[2] = first - second;
    out_xyz[2] = out_xyz[2] * -1.0;
    first = left_xyz[2] * right->w;
    out_xyz[2] = first + out_xyz[2];
    first = right_xyz[2] * left->w;
    out_xyz[2] = first + out_xyz[2];

    dot += right_xyz[0] * left_xyz[0];
    dot += right_xyz[1] * left_xyz[1];
    dot += right_xyz[2] * left_xyz[2];
    out->w = right->w * left->w - dot;
}

/* 原 ELF `quat_cross`：rdi=输出四元数，rsi=left，rdx=right。
 * 其 ABI 可观察结果是 right × left。内部既有 dp_quat_cross_left 是为
 * 历史调用点补偿参数顺序的适配器，不能直接用作该导出包装。这里逐项保留
 * vector3_cross(left,right)、乘 -1 和两次 AXPY 的实际算术次序。 */
void quat_cross(DpQuatAbi *out, const DpQuatAbi *left, const DpQuatAbi *right)
{
    const double *left_xyz = left->xyz.data;
    const double *right_xyz = right->xyz.data;
    double *out_xyz = out->xyz.data;
    double product;
    double dot = 0.0;

    product = left_xyz[1] * right_xyz[2];
    out_xyz[0] = product - left_xyz[2] * right_xyz[1];
    out_xyz[0] = out_xyz[0] * -1.0;
    product = right_xyz[0] * left->w;
    out_xyz[0] = product + out_xyz[0];
    product = left_xyz[0] * right->w;
    out_xyz[0] = product + out_xyz[0];

    product = left_xyz[2] * right_xyz[0];
    out_xyz[1] = product - left_xyz[0] * right_xyz[2];
    out_xyz[1] = out_xyz[1] * -1.0;
    product = right_xyz[1] * left->w;
    out_xyz[1] = product + out_xyz[1];
    product = left_xyz[1] * right->w;
    out_xyz[1] = product + out_xyz[1];

    product = left_xyz[0] * right_xyz[1];
    out_xyz[2] = product - left_xyz[1] * right_xyz[0];
    out_xyz[2] = out_xyz[2] * -1.0;
    product = right_xyz[2] * left->w;
    out_xyz[2] = product + out_xyz[2];
    product = left_xyz[2] * right->w;
    out_xyz[2] = product + out_xyz[2];

    dot += left_xyz[0] * right_xyz[0];
    dot += left_xyz[1] * right_xyz[1];
    dot += left_xyz[2] * right_xyz[2];
    out->w = right->w * left->w - dot;
}

/* 原 ELF ABI：rdi=真值四元数，xmm0=时间步，操作全局 STS[3]。 */
void Update_STS_Quat(const DpQuatAbi *truth_quat, double step_time)
{
    dp_update_sts_quat(STS, truth_quat, step_time);
}

void dp_update_sts_quat(DpStsRecovered sensors[3], const DpQuatAbi *truth_quat,
                        double step_time)
{
    unsigned index;

    if (sensors == NULL || !dp_quat_components_valid(truth_quat)) {
        return;
    }
    for (index = 0u; index < 3u; ++index) {
        DpStsRecovered *sensor = &sensors[index];
        const double remainder = fabs(fmod(sensor->accumulated_time, sensor->update_period));

        if (step_time > remainder && dp_quat_components_valid(&sensor->error_quat) &&
            dp_quat_components_valid(&sensor->measure_quat)) {
            /* Update_STS_Quat: quat_cross(out=measure, rsi=error, rdx=truth) 的实际
             * 代数结果为 truth × error，而非 error × truth。 */
            dp_quat_cross_left(&sensor->measure_quat, truth_quat, &sensor->error_quat);
        }
        sensor->accumulated_time += step_time;
    }
}

int dp_is_earth_out(const DpVector *position_gci, const DpVector *view_direction,
                    double margin_angle)
{
    double negative_position_data[3] = {0.0, 0.0, 0.0};
    DpVector negative_position = {3, 0, negative_position_data};
    double angle;
    double position_norm;
    double earth_half_angle;

    if (position_gci == NULL || view_direction == NULL || position_gci->count != 3 ||
        view_direction->count != 3 || position_gci->data == NULL || view_direction->data == NULL) {
        return 0;
    }
    (void)vector_axpby((DpVector *)position_gci, -1.0, &negative_position, 0.0);
    angle = vector2angle(&negative_position, view_direction);
    position_norm = vector_nrm2(position_gci);
    earth_half_angle = asin(6378137.0 / position_norm);
    return angle > earth_half_angle + margin_angle;
}

int isEarthOut(const DpVector *position_gci, const DpVector *view_direction,
               double margin_angle)
{
    double negative_position_data[3] = {0.0, 0.0, 0.0};
    DpVector negative_position = {3, 0, negative_position_data};
    double angle;
    double position_norm;
    double earth_half_angle;

    (void)vector_axpby((DpVector *)position_gci, -1.0, &negative_position, 0.0);
    angle = vector2angle(&negative_position, view_direction);
    position_norm = vector_nrm2(position_gci);
    earth_half_angle = asin(6378137.0 / position_norm);
    return angle > earth_half_angle + margin_angle;
}

int isSunOut(const DpVector *first, const DpVector *second, double threshold)
{
    const double angle = vector2angle(second, first);
    return angle > threshold;
}

/* 原 ELF ABI：rdi=时间数组，rsi=位置，rdx=速度；写入全局 GPS_Kalman。 */
void UpdateGPS(const double time_values[6], const DpVec3 *position_gci,
               const DpVec3 *velocity_gci)
{
    dp_update_gps(&GPS_Kalman, time_values, position_gci, velocity_gci, &init_flag);
}

void dp_update_gps(DpGpsKalmanRecovered *gps, const double time_values[6],
                   const DpVec3 *position_gci, const DpVec3 *velocity_gci,
                   int *init_flag_inout)
{
    unsigned index;
    const double *position = &position_gci->x;
    const double *velocity = &velocity_gci->x;

    if (gps == NULL || time_values == NULL || position_gci == NULL || velocity_gci == NULL) {
        return;
    }
    for (index = 0u; index < 3u; ++index) {
        gps->position_gci[index] = position[index];
        gps->velocity_gci[index] = velocity[index];
    }
    for (index = 0u; index < 6u; ++index) {
        gps->time_components[index] = (int32_t)time_values[index];
    }
    if (init_flag_inout != NULL && *init_flag_inout == 1) {
        *init_flag_inout = 0;
    }
}

int dp_is_star_tracker_valid(const DpQuatAbi *attitude_gci_to_body,
                             const DpVector *position_gci, const DpVector *sun_gci,
                             const DpMatrix *installation_matrix,
                             double sun_exclusion_angle, double earth_margin_angle)
{
    double fixed_boresight_data[3] = {0.0, 0.0, 1.0};
    double installed_boresight_data[3] = {0.0, 0.0, 0.0};
    double conjugate_xyz_data[3] = {0.0, 0.0, 0.0};
    double attitude_matrix_data[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double gci_boresight_data[3] = {0.0, 0.0, 0.0};
    DpVector fixed_boresight = {3, 0, fixed_boresight_data};
    DpVector installed_boresight = {3, 0, installed_boresight_data};
    DpQuatAbi conjugate = {0.0, {3, 0, conjugate_xyz_data}};
    DpMatrix attitude_matrix = {3, 3, 3, 0, attitude_matrix_data};
    DpVector gci_boresight = {3, 0, gci_boresight_data};
    double angle;

    if (!dp_quat_components_valid(attitude_gci_to_body) || position_gci == NULL ||
        sun_gci == NULL || installation_matrix == NULL) {
        return 0;
    }
    (void)blas_gemv(installation_matrix, &fixed_boresight, &installed_boresight, 1.0, 0.0);
    quat_conjugate(&conjugate, attitude_gci_to_body);
    quat_att_mat(&attitude_matrix, &conjugate);
    (void)blas_gemv(&attitude_matrix, &installed_boresight, &gci_boresight, 1.0, 0.0);
    angle = vector2angle(sun_gci, &gci_boresight);
    if (angle > sun_exclusion_angle) {
        return dp_is_earth_out(position_gci, &gci_boresight, earth_margin_angle) != 1;
    }
    return 1;
}

/* 原 ELF ABI：四个指针参数，xmm0/xmm1 为太阳排除与地球边界阈值。 */
int isStarTrackerValid(const DpQuatAbi *attitude_gci_to_body,
                       const DpVector *position_gci, const DpVector *sun_gci,
                       const DpMatrix *installation_matrix,
                       double sun_exclusion_angle, double earth_margin_angle)
{
    return dp_is_star_tracker_valid(attitude_gci_to_body, position_gci, sun_gci,
                                    installation_matrix, sun_exclusion_angle, earth_margin_angle);
}

/* 原 ELF ABI：rdi=姿态，rsi=位置，rdx=太阳，更新全局 STS[3].valid_flag。 */
void Update_STS_ValidFlag(const DpQuatAbi *attitude_gci_to_body,
                          const DpVector *position_gci, const DpVector *sun_gci)
{
    dp_update_sts_valid_flag(STS, attitude_gci_to_body, position_gci, sun_gci);
}

void dp_update_sts_valid_flag(DpStsRecovered sensors[3], const DpQuatAbi *attitude_gci_to_body,
                              const DpVector *position_gci, const DpVector *sun_gci)
{
    unsigned index;

    if (sensors == NULL) {
        return;
    }
    for (index = 0u; index < 3u; ++index) {
        sensors[index].valid_flag = dp_is_star_tracker_valid(
            attitude_gci_to_body, position_gci, sun_gci, &sensors[index].installation_matrix,
            sensors[index].sun_exclusion_angle, sensors[index].earth_margin_angle);
    }
}

int dp_is_earth_shadow(const DpVector *sun_gci, const DpVector *position_gci)
{
    const double position_norm = vector_nrm2(position_gci);
    const double earth_limb_angle = acos(6378137.0 / position_norm);
    const double separation_angle = vector2angle(sun_gci, position_gci);

    return separation_angle > earth_limb_angle + 1.5707963267948966;
}

/* 原 ELF ABI：rdi=sun 向量，rsi=位置向量，eax 返回地影判定。 */
int isEarthShadow(const DpVector *sun_gci, const DpVector *position_gci)
{
    return dp_is_earth_shadow(sun_gci, position_gci);
}

/* 原 ELF ABI：rdi=sun GCI，rsi=位置 GCI，更新全局 DSS[2].valid_flag。 */
void UpdateSunSensorValidFlag(const DpVector *sun_gci, const DpVector *position_gci)
{
    dp_update_dss_valid_flag(DSS, sun_gci, position_gci);
}

void dp_update_dss_valid_flag(DpDssRecovered sensors[2], const DpVector *sun_gci,
                              const DpVector *position_gci)
{
    double reference_data[3] = {0.0, 0.0, 1.0};
    DpVector reference_vector = {3, 0, reference_data};
    unsigned index;
    if (sensors == NULL || sun_gci == NULL || position_gci == NULL) {
        return;
    }
    for (index = 0u; index < 2u; ++index) {
        /* 原 UpdateSunSensorValidFlag 在其 +0x20 栈槽写入
         * `(0,0,1)`，并把该三维向量传给两次 vector2angle。原
         * 调用点跳板 trace 已记录 H11 步 68/73/85 的返回分别为
         * 0.9491289345377069、1.021994114358845、1.0309815219893705；
         * 均低于 pi/3，随后 isEarthShadow=0，故两个 valid_flag 为 0。 */
        const double angle = vector2angle(&sensors[index].measure, &reference_vector);

        if (sensors[index].valid_angle_threshold < angle) {
            sensors[index].valid_flag = 1;
        } else {
            sensors[index].valid_flag = dp_is_earth_shadow(sun_gci, position_gci) != 0;
        }
    }
}

void dp_update_device_measure_recovered(DpDeviceMeasureRecovered *devices,
                                        const DpState *state,
                                        const DpDeviceMeasureEnvironment *environment,
                                        double step_time,
                                        DpGaussian2Sampler gaussian2, void *opaque)
{
    double truth_xyz[3];
    double position_data[3];
    double sun_gci_data[3];
    DpQuatAbi truth_quat;
    DpVector position_gci;
    DpVector sun_gci;

    if (devices == NULL || state == NULL || environment == NULL) {
        return;
    }
    truth_xyz[0] = state->attitude_q.q1;
    truth_xyz[1] = state->attitude_q.q2;
    truth_xyz[2] = state->attitude_q.q3;
    truth_quat.w = state->attitude_q.q0;
    truth_quat.xyz.count = 3;
    truth_quat.xyz.reserved_04 = 0;
    truth_quat.xyz.data = truth_xyz;
    position_data[0] = state->position_gci.x;
    position_data[1] = state->position_gci.y;
    position_data[2] = state->position_gci.z;
    position_gci.count = 3;
    position_gci.reserved_04 = 0;
    position_gci.data = position_data;
    sun_gci_data[0] = environment->sun_gci.x;
    sun_gci_data[1] = environment->sun_gci.y;
    sun_gci_data[2] = environment->sun_gci.z;
    sun_gci.count = 3;
    sun_gci.reserved_04 = 0;
    sun_gci.data = sun_gci_data;

    dp_update_sts_quat(devices->sts, &truth_quat, step_time);
    dp_update_sts_valid_flag(devices->sts, &truth_quat, &position_gci, &sun_gci);
    dp_update_gyro(devices->gyro, &state->body_rate, gaussian2, opaque);
    dp_update_dss(devices->dss, &environment->sun_body, gaussian2, opaque);
    dp_update_dss_valid_flag(devices->dss, &sun_gci, &position_gci);
    dp_update_magmeter(devices->magmeter, &environment->magnetic_body, gaussian2, opaque);
    dp_update_gps(&devices->gps, environment->time_values, &state->position_gci,
                  &state->velocity_gci, &devices->gps_init_flag);
}

static void dp_euler_angle_312_to_quat(double out[4], double a, double b, double c)
{
    double sa;
    double ca;
    double sb;
    double cb;
    double sc;
    double cc;

    sa = sin(0.5 * a); ca = cos(0.5 * a);
    sb = sin(0.5 * b); cb = cos(0.5 * b);
    sc = sin(0.5 * c); cc = cos(0.5 * c);
    out[0] = ca * cb * cc - sa * sb * sc;
    out[1] = ca * sb * cc - sa * cb * sc;
    out[2] = ca * cb * sc + sa * sb * cc;
    out[3] = sa * cb * cc + ca * sb * sc;
}

/* 原 ELF ABI：rdi=输出 4-double 缓冲区，esi=全局 STS 索引。 */
void GetStarTrackerQuat(double out_quat[4], int32_t index)
{
    (void)dp_get_star_tracker_quat(out_quat, &STS[index], NULL, NULL);
}

int dp_get_star_tracker_quat(double out_quat[4], const DpStsRecovered *sensor,
                             DpGaussian2Sampler gaussian2, void *opaque)
{
    const DpQuatAbi *measure;

    if (out_quat == NULL || sensor == NULL || sensor->measure_quat.xyz.data == NULL) {
        return -1;
    }
    measure = &sensor->measure_quat;
    if (sensor->gaussian_noise_flag == 1u) {
        double euler_quat[4];
        double output_xyz[3];
        DpQuatAbi euler_error;
        DpQuatAbi output_quat;
        double a;
        double b;
        double c;
        const double half = 0.5;

        /* GetStarTrackerQuat：原 ELF 先完成均值/标准差求和，再逐项乘 0.5。 */
        a = gaussian2 != NULL ? gaussian2((sensor->euler_mean_deg[1] + sensor->euler_mean_deg[2]) * half,
                                           (sensor->euler_sigma_deg[1] + sensor->euler_sigma_deg[2]) * half,
                                           opaque) :
                                dp_ran_gaussian2((sensor->euler_mean_deg[1] + sensor->euler_mean_deg[2]) * half,
                                                 (sensor->euler_sigma_deg[1] + sensor->euler_sigma_deg[2]) * half);
        b = gaussian2 != NULL ? gaussian2(sensor->euler_mean_deg[0] * half,
                                           sensor->euler_sigma_deg[0] * half,
                                           opaque) :
                                dp_ran_gaussian2(sensor->euler_mean_deg[0] * half,
                                                 sensor->euler_sigma_deg[0] * half);
        c = gaussian2 != NULL ? gaussian2(sensor->euler_mean_deg[0] * half,
                                           sensor->euler_sigma_deg[0] * half,
                                           opaque) :
                                dp_ran_gaussian2(sensor->euler_mean_deg[0] * half,
                                                 sensor->euler_sigma_deg[0] * half);
        dp_euler_angle_312_to_quat(euler_quat, a, b, c);
        euler_error.w = euler_quat[0];
        euler_error.xyz = (DpVector){3, 0, &euler_quat[1]};
        output_quat.w = 0.0;
        output_quat.xyz = (DpVector){3, 0, output_xyz};
        /* 原调用：quat_cross(out, rsi=euler_error, rdx=measure)，该 ABI 的结果为
         * measure × euler_error。 */
        dp_quat_cross_left(&output_quat, measure, &euler_error);
        out_quat[0] = output_quat.w;
        out_quat[1] = output_xyz[0];
        out_quat[2] = output_xyz[1];
        out_quat[3] = output_xyz[2];
    } else {
        out_quat[0] = measure->w;
        out_quat[1] = measure->xyz.data[0];
        out_quat[2] = measure->xyz.data[1];
        out_quat[3] = measure->xyz.data[2];
    }
    return 0;
}
