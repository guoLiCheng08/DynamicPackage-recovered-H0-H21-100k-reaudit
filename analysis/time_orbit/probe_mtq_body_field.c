#include <stdio.h>
#include <string.h>

#include "dynamic_core_environment.h"
#include "dynamic_time.h"
#include "dynamic_torque.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

int main(void)
{
    double state[33];
    double torque[45];
    DpCalendarTime calendar;
    DpCoreEnvironmentOutputs environment;
    DpVec3 moment = {0.001, 0.0, 0.0};
    DpVec3 result;
    double q_xyz[3];
    double attitude_data[9];
    double attitude_t_data[9];
    double original_b_data[3];
    double forward_data[3];
    double transpose_data[3];
    DpQuatAbi q;
    DpMatrix attitude = {3, 3, 3, 0, attitude_data};
    DpMatrix attitude_t = {3, 3, 3, 0, attitude_t_data};
    DpVector original_b = {3, 0, original_b_data};
    DpVector forward = {3, 0, forward_data};
    DpVector transpose = {3, 0, transpose_data};

    if (read_blob("gold_mtq_x_pos_step1_post_y.bin", state, sizeof(state)) != 0 ||
        read_blob("gold_mtq_x_pos_step2_torque.bin", torque, sizeof(torque)) != 0) return 1;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.1);
    TimeArrayGet((double *)&calendar);
    if (dp_core_environment_from_states(&environment, state, state, &calendar) != 0 ||
        dp_mag_torque(&result, &moment, &environment.device_environment.magnetic_body) != 0) return 1;
    if (read_blob("gold_mtq_x_pos_step2_bi_vector.bin", original_b_data,
                  sizeof(original_b_data)) != 0) return 1;
    q_xyz[0] = state[1]; q_xyz[1] = state[2]; q_xyz[2] = state[3];
    q.w = state[0]; q.xyz = (DpVector){3, 0, q_xyz};
    quat_att_mat(&attitude, &q);
    matrix_trans(&attitude_t, &attitude);
    printf("y quaternion attitude matrix %.17g %.17g %.17g / %.17g %.17g %.17g / %.17g %.17g %.17g\n",
           attitude_data[0], attitude_data[1], attitude_data[2],
           attitude_data[3], attitude_data[4], attitude_data[5],
           attitude_data[6], attitude_data[7], attitude_data[8]);
    blas_gemv(&attitude, &original_b, &forward, 1.0, 0.0);
    blas_gemv(&attitude_t, &original_b, &transpose, 1.0, 0.0);
    printf("original B by attitude %.17g %.17g %.17g; transpose %.17g %.17g %.17g\n",
           forward_data[0], forward_data[1], forward_data[2],
           transpose_data[0], transpose_data[1], transpose_data[2]);
    printf("gci B %.17g %.17g %.17g\n", environment.magnetic_gci.x,
           environment.magnetic_gci.y, environment.magnetic_gci.z);
    printf("body B %.17g %.17g %.17g\n", environment.device_environment.magnetic_body.x,
           environment.device_environment.magnetic_body.y, environment.device_environment.magnetic_body.z);
    printf("mXB %.17g %.17g %.17g\n", result.x, result.y, result.z);
    printf("SatTorque %.17g %.17g %.17g\n", torque[0x128 / 8],
           torque[0x128 / 8 + 1], torque[0x128 / 8 + 2]);
    return 0;
}
