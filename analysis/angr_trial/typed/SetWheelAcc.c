extern void RWheel;

void SetWheelAcc(unsigned long a0)
{
    void* iter;  // rbx
    void* v4;  // rdx
    int v13;  // xmm1
    int v14;  // xmm0
    unsigned long v15;  // xmm3lq
    int v16;  // xmm1
    int v17;  // xmm3
    unsigned long v18;  // xmm0lq
    int v19;  // xmm8
    int v20;  // xmm3
    int v21;  // xmm0
    unsigned long v22;  // xmm1lq
    unsigned long long i;  // rax
    void* v6;  // r12
    unsigned long long index;  // rbp
    unsigned long v8;  // xmm1lq
    unsigned long long v9;  // rax
    unsigned long v10;  // xmm1lq
    unsigned long v11;  // xmm0lq
    int v12;  // xmm8
    unsigned long v0;  // [bp-0x80]
    int v23;  // [bp-0x78]
    char v1;  // [bp-0x58]

    iter = &RWheel;
    v4 = &RWheel;
    i = 0;
    do
    {
        v4 += 120;
        *((unsigned long long *)((char *)&v23 + i)) = DivV(*((long long *)(a0 + i)), *((long long *)((char *)v4 - 80)));
        i += 8;
    } while (i != 32);
    v6 = &RWheel;
    index = 0;
    do
    {
        v8 = *((long long *)((char *)&v23 + 8 * index));
        if ((int)v6[4] != 1 || !((char)((CmpF(v8, 0) & 69) >> 2) & 1) && CmpF(v8, 0) & 64)
        {
            *((unsigned long *)&(&v1)[8 * index]) = v8;
        }
        else
        {
            v0 = v8;
            ran_gaussian();
            *((unsigned long long *)&(&v1)[8 * index]) = AddV(v0, DivV((long long)v6[16], (long long)v6[40]));
        }
    } while ((index += 1, v6 += 120, index != 4));
    v9 = 0;
    do
    {
        v10 = (long long)iter[48];
        v11 = (long long)iter[24];
        if (((CmpF(v11, v10) & 69 | (char)((CmpF(v11, v10) & 69) >> 6)) & 1) != 1 && !(CmpF(v10, (unsigned long long)(v11 ^ 0x8000000000000000)) & 1))
        {
            v12 = (int)*((long long *)&(&v1)[8 * v9]);
            v13 = (int)(long long)iter[40];
            v14 = (int)(long long)iter[32];
            v15 = (unsigned long long)(MulV(v12, v13));
            if (((CmpF(v15, *((unsigned long long *)&v14)) & 69 | (char)((CmpF(v15, *((unsigned long long *)&v14)) & 69) >> 6)) & 1) != 1 || (v14 ^= 0x8000000000000000, (((char)(CmpF(*((unsigned long long *)(void*)&v14), v15)) & 69 | (char)((CmpF(*((unsigned long long *)(void*)&v14), v15) & 69) >> 6)) & 1) != 1))
            {
                *((unsigned long long *)&iter[56]) = *((unsigned long long *)&DivV(v14, v13));
                continue;
            }
LABEL_40e2ef:
            *((unsigned long long *)&iter[56]) = *((unsigned long long *)&v12);
            continue;
        }
        if (((CmpF(v10, v11) & 69 | (char)((CmpF(v10, v11) & 69) >> 6)) & 1) != 1)
        {
            v16 = (int)*((long long *)&(&v1)[8 * v9]);
            v17 = (int)(long long)iter[40];
            v18 = (unsigned long long)(MulV(v16, v17));
            if (((CmpF(v18, 0) & 69 | (char)((CmpF(v18, 0) & 69) >> 6)) & 1) == 1)
            {
                v19 = (int)((long long)iter[32] ^ 0x8000000000000000);
                if (((CmpF(*((unsigned long long *)&v19), v18) & 69 | (char)((CmpF(*((unsigned long long *)&v19), v18) & 69) >> 6)) & 1) != 1)
                    *((unsigned long long *)&iter[56]) = *((unsigned long long *)&DivV(v19, v17));
                else
                    *((unsigned long long *)&iter[56]) = *((unsigned long long *)&v16);
            }
        }
        else
        {
            if (((CmpF((unsigned long long)(v11 ^ 0x8000000000000000), v10) & 69 | (CmpF((unsigned long long)(v11 ^ 0x8000000000000000), v10) & 69) >> 6) & 1) == 1)
                continue;
            v12 = (int)*((long long *)&(&v1)[8 * v9]);
            v20 = (int)(long long)iter[40];
            v21 = (int)(long long)iter[32];
            v22 = (unsigned long long)(MulV(v12, v20));
            if (((CmpF(v22, *((unsigned long long *)&v21)) & 69 | (char)((CmpF(v22, *((unsigned long long *)&v21)) & 69) >> 6)) & 1) == 1)
            {
                if (((CmpF(0, v22) & 69 | (CmpF(0, v22) & 69) >> 6) & 1) == 1)
                    goto LABEL_40e2ef;
                *((unsigned long *)&iter[56]) = 0;
            }
            else
            {
                *((unsigned long long *)&iter[56]) = *((unsigned long long *)&DivV(v21, v20));
            }
        }
    } while ((v9 += 1, iter += 120, v9 != 4));
    return;
}

