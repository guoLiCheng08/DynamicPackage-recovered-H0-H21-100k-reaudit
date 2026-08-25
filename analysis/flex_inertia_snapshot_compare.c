#include <math.h>
#include <stdio.h>

#include "dynamic_math.h"

int main(void)
{
    /* 首次 dynamics_flex 快照：原 ELF 的 J_c_B_mem。 */
    const double original_j[9] = {
        1170.5, 782.0, 1.48,
        782.0, 1326.5, -17.7,
        1.48, -17.7, 2071.0,
    };
    /* 同一快照中原 ELF 的 J_c_B_inv_mem。 */
    const double original_inverse[9] = {
        0.0014095906986634926, -0.00083109195585282175, -8.1103437241028068e-06,
        -0.00083109195585282165, 0.0012439597449094761, 1.1225544944258765e-05,
        -8.1103437241028068e-06, 1.1225544944258765e-05, 0.00048296025854863595,
    };
    double recovered_inverse[9] = {0.0};
    double max_abs_error = 0.0;
    int max_index = 0;
    int i;

    inv_CAL_M3(original_j, recovered_inverse);
    for (i = 0; i < 9; ++i) {
        const double error = fabs(recovered_inverse[i] - original_inverse[i]);
        if (error > max_abs_error) {
            max_abs_error = error;
            max_index = i;
        }
    }
    printf("max_abs_error=%.17g index=%d recovered=%.17g original=%.17g\n",
           max_abs_error, max_index, recovered_inverse[max_index], original_inverse[max_index]);
    return max_abs_error <= 1e-15 ? 0 : 1;
}
