extern long long WheelGroup;
extern long long g_617d48;
extern long long g_617d70;
extern void RWheel;

void UpdateWheel(void)
{
    void* iter;  // rax
    unsigned long long idx;  // rdx
    int v8;  // xmm1
    int v9;  // xmm2
    unsigned long v10;  // xmm1lq
    int v11;  // xmm0
    int v12;  // xmm0
    unsigned long v0;  // [bp-0x50]
    unsigned int v1;  // [bp-0x48]
    char *v2;  // [bp-0x40]
    int v3;  // [bp-0x38], Other Possible Types: char
    uint128_t v4;  // [bp-0x28]

    iter = &RWheel;
    v1 = 4;
    v2 = &v3;
    v3 = 0;
    idx = 0;
    v4 = 0;
    do
    {
        iter += 120;
        v8 = (int)*((long long *)((char *)iter - 64));
        v9 = (int)*((long long *)((char *)iter - 80));
        v10 = (unsigned long long)(MulV(v8, v9));
        *((unsigned long *)((char *)&v3 + 8 * idx)) = v10;
        idx += 1;
        v12 = AddV(MulV(v8, v11), *((long long *)((char *)iter - 72)));
        *((unsigned long *)((char *)iter - 48)) = v10;
        *((unsigned long long *)((char *)iter - 72)) = *((unsigned long long *)&v12);
        *((unsigned long long *)((char *)iter - 56)) = *((unsigned long long *)&MulV(v12, v9));
    } while (idx != 4);
    v0 = *((unsigned long long *)&v11);
    blas_gemv(&g_617d70, &v1, &g_617d48);
    vector_axpy(&g_617d48, &WheelGroup);
    return;
}

