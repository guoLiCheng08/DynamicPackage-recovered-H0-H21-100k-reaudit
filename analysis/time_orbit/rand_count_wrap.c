#include <stdio.h>

int __real_rand(void);
int e8_rand_stage = 0;
unsigned long e8_rand_count_stage0 = 0u;
unsigned long e8_rand_count_stage1 = 0u;

int __wrap_rand(void)
{
    if (e8_rand_stage == 0) {
        ++e8_rand_count_stage0;
    } else {
        ++e8_rand_count_stage1;
    }
    return __real_rand();
}

__attribute__((destructor)) static void e8_report_rand_counts(void)
{
    (void)fprintf(stderr, "recovered rand calls: warmup=%lu step2=%lu\n",
                  e8_rand_count_stage0, e8_rand_count_stage1);
}
