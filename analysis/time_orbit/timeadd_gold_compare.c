#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int check_array(const double actual[6], const double expected[6],
                       const char *label)
{
    unsigned i;
    for (i = 0u; i < 6u; ++i) {
        if (bits(actual[i]) != bits(expected[i])) {
            printf("FAIL %s field=%u actual=%a expected=%a "
                   "actual_bits=%016llx expected_bits=%016llx\n",
                   label, i, actual[i], expected[i],
                   (unsigned long long)bits(actual[i]),
                   (unsigned long long)bits(expected[i]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    double actual[6];
    const double expected0[6] = {2024.0, 2.0, 28.0, 23.0, 59.0, 59.5};
    const double expected1[6] = {2000.0, 1.0, 1.0, 0.0, 0.0, 2.0};
    double total;

    /* 原 ELF 由 timeadd_function_gold.gdb 直接调用获得的连续状态序列。 */
    TimeInit(2024.0, 2.0, 28.0, 23.0, 59.0, 59.75);
    TimeAdd(0.5);
    TimeArrayGet(actual);
    if (check_array(actual, expected0, "case0 calendar") != 0) return 1;
    total = TimeTotalGet();
    if (bits(total) != bits(0.5)) {
        printf("FAIL case0 total actual=%a expected=%a\n", total, 0.5);
        return 1;
    }

    /* TimeInit 之后不重置小数秒残量和累计秒，是原 ELF 的关键状态语义。 */
    TimeInit(1999.0, 12.0, 31.0, 23.0, 59.0, 59.25);
    TimeAdd(2.5);
    TimeArrayGet(actual);
    if (check_array(actual, expected1, "case1 calendar") != 0) return 1;
    total = TimeTotalGet();
    if (bits(total) != bits(3.0)) {
        printf("FAIL case1 total actual=%a expected=%a\n", total, 3.0);
        return 1;
    }

    puts("TimeInit/TimeAdd original-ELF gold compare: PASS (bitwise)");
    return 0;
}
