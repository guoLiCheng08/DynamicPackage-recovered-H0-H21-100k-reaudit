#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEP_COUNT 100u
#define DSS_ITEM_BYTES 0x158u
#define DSS_TRACE_BYTES (2u * DSS_ITEM_BYTES)

static double read_f64(const unsigned char *block, size_t offset)
{
    double value;

    memcpy(&value, block + offset, sizeof(value));
    return value;
}

static int32_t read_i32(const unsigned char *block, size_t offset)
{
    int32_t value;

    memcpy(&value, block + offset, sizeof(value));
    return value;
}

int main(int argc, char **argv)
{
    FILE *original;
    FILE *recovered;
    unsigned char original_block[DSS_TRACE_BYTES];
    unsigned char recovered_block[DSS_TRACE_BYTES];
    unsigned step;

    if (argc != 3) {
        return 2;
    }
    original = fopen(argv[1], "rb");
    recovered = fopen(argv[2], "rb");
    if (original == NULL || recovered == NULL) {
        return 2;
    }

    puts("step,orig_flag0,orig_flag1,rec_flag0,rec_flag1,"
         "dss1_measure_x,dss1_measure_y,dss1_measure_z,"
         "dss1_ratio_x,dss1_ratio_y,dss1_angle_x,dss1_angle_y");
    for (step = 0u; step < STEP_COUNT; ++step) {
        const size_t dss1 = DSS_ITEM_BYTES;
        int32_t original_flag0;
        int32_t original_flag1;
        int32_t recovered_flag0;
        int32_t recovered_flag1;

        if (fread(original_block, 1u, sizeof(original_block), original) !=
                sizeof(original_block) ||
            fread(recovered_block, 1u, sizeof(recovered_block), recovered) !=
                sizeof(recovered_block)) {
            (void)fclose(original);
            (void)fclose(recovered);
            return 1;
        }
        original_flag0 = read_i32(original_block, 0x108u);
        original_flag1 = read_i32(original_block, dss1 + 0x108u);
        recovered_flag0 = read_i32(recovered_block, 0x108u);
        recovered_flag1 = read_i32(recovered_block, dss1 + 0x108u);
        if (original_flag0 != recovered_flag0 || original_flag1 != recovered_flag1) {
            printf("%u,%d,%d,%d,%d,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n",
                   step + 1u, original_flag0, original_flag1, recovered_flag0, recovered_flag1,
                   read_f64(original_block, dss1 + 0x120u),
                   read_f64(original_block, dss1 + 0x128u),
                   read_f64(original_block, dss1 + 0x130u),
                   read_f64(original_block, dss1 + 0x140u),
                   read_f64(original_block, dss1 + 0x150u),
                   read_f64(original_block, dss1 + 0x138u),
                   read_f64(original_block, dss1 + 0x148u));
        }
    }

    (void)fclose(original);
    (void)fclose(recovered);
    return 0;
}
