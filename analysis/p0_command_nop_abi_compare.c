/* 两个原 ELF repz-ret 命令入口：无参数、无返回契约的 ABI 烟雾门禁。 */
#include "dynamic_recovered.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    volatile uint64_t caller_sentinel = UINT64_C(0x4d414e55535f434d);
    const uint64_t before = caller_sentinel;

    Algorithm_Command_Execute();
    DYN_Command_Execute();

    (void)printf("P0 command repz-ret ABI compare: %s\n",
                 caller_sentinel == before ? "PASS" : "FAIL");
    return caller_sentinel == before ? 0 : 1;
}
