typedef struct struct_0 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_0;

extern void g_616a68;
extern void g_6170a8;

struct_0 * UpdateGyro(long long a0)
{
    void* iter;  // rbx
    struct_0 *v2;  // rax
    long long v3;  // rdi
    long long v4;  // rsi
    long long v5;  // rdx
    long long v6;  // rcx
    long long v7;  // r8
    long long v8;  // r9

    iter = &g_616a68;
    do
    {
        v2 = blas_gemv(iter, a0, iter + 176);
        if (*((int *)((char *)iter - 80)) == 1)
        {
            ran_gaussian2();
            *((unsigned long long *)(long long)iter[184]) = AddV(*((long long *)((char *)iter - 72)), *((long long *)(long long)iter[184]));
            ran_gaussian2(v3, v4, v5, v6, v7, v8);
            *((unsigned long long *)((long long)iter[184] + 8)) = AddV(*((long long *)((char *)iter - 64)), *((long long *)((long long)iter[184] + 8)));
            ran_gaussian2(v3, v4, v5, v6, v7, v8);
            v2 = (long long)iter[184];
            v2->field_10 = AddV(*((long long *)((char *)iter - 56)), v2->field_10);
        }
    } while ((iter += 800, iter != &g_6170a8));
    return v2;
}

