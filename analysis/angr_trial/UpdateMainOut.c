typedef struct struct_0 {
    char padding_0[112];
    unsigned long long field_70;
    unsigned long long field_78;
    unsigned long long field_80;
    unsigned long long field_88;
    unsigned long long field_90;
    unsigned long long field_98;
    unsigned long long field_a0;
    unsigned long long field_a8;
    unsigned long long field_b0;
    unsigned long long field_b8;
    unsigned int field_c0;
    unsigned int field_c4;
    unsigned long long field_c8;
    unsigned long long field_d0;
    unsigned long long field_d8;
    unsigned long long field_e0;
    unsigned long long field_e8;
    unsigned long long field_f0;
    char padding_f8[32];
    unsigned long long field_118;
    unsigned long long field_120;
    unsigned long long field_128;
    unsigned long long field_130;
    unsigned long long field_138;
    unsigned long long field_140;
    unsigned long long field_148;
    unsigned long long field_150;
    unsigned long long field_158;
    unsigned long long field_160;
    char padding_168[24];
    unsigned long long field_180;
    unsigned long long field_188;
    unsigned long long field_190;
    unsigned long long field_198;
    unsigned long long field_1a0;
    unsigned long long field_1a8;
} struct_0;

typedef struct struct_1 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_1;

extern unsigned long long g_616928;
extern unsigned long long g_616930;
extern unsigned long long g_616938;
extern unsigned long long g_616940;
extern unsigned long long g_616948;
extern unsigned long long g_616950;
extern char g_616958;
extern unsigned long long g_616998;
extern unsigned long long g_6169a0;
extern unsigned long long g_6169a8;
extern unsigned long long g_6169b0;
extern struct_1 *g_616b20;
extern struct_1 *g_616e40;
extern struct_1 *g_6173d8;
extern struct_1 *g_617510;
extern void STS;
extern unsigned int g_617aa8;
extern unsigned long long g_617ad8;
extern unsigned long long g_617ae8;
extern unsigned int g_617c00;
extern unsigned long long g_617c30;
extern unsigned long long g_617c40;

void UpdateMainOut(struct_0 *idx, unsigned long long *a1)
{
    void* v13;  // r13
    struct_0 *iter;  // r14
    unsigned long long i;  // rbp
    unsigned long long j;  // rbp
    unsigned long v17;  // xmm0lq
    unsigned long long v18;  // xmm0lq
    unsigned long long k;  // rax
    unsigned long long *index;  // r12
    unsigned long long l;  // rax
    unsigned int v0;  // [bp-0xa8]
    char *v1;  // [bp-0xa0]
    unsigned int v2;  // [bp-0x98]
    char *v3;  // [bp-0x90]
    unsigned int v4;  // [bp-0x88]
    struct_0 *v5;  // [bp-0x80]
    unsigned int v6;  // [bp-0x78]
    struct_0 *v7;  // [bp-0x70]
    char v8;  // [bp-0x68], Other Possible Types: uint128_t
    unsigned long long v9;  // [bp-0x58]
    char v10;  // [bp-0x48], Other Possible Types: uint128_t
    unsigned long long v11;  // [bp-0x38]

    v13 = &STS;
    iter = idx;
    i = 0;
    do
    {
        v13 += 368;
        GetStarTrackerQuat(iter, i & 0xffffffff);
        iter = &iter->padding_0[32];
        *((int *)&idx->padding_0[96 + 4 * i]) = *((int *)((char *)v13 - 56));
        i += 1;
    } while (i != 3);
    j = 0;
    idx->field_70 = g_616b20->field_0;
    idx->field_78 = g_616b20->field_8;
    idx->field_80 = g_616b20->field_10;
    idx->field_88 = g_616e40->field_0;
    idx->field_90 = g_616e40->field_8;
    idx->field_98 = g_616e40->field_10;
    idx->field_a0 = MulV(g_617ad8, 4633260481411531256);
    idx->field_c0 = g_617aa8;
    idx->field_a8 = MulV(g_617ae8, 4633260481411531256);
    v17 = MulV(4633260481411531256, g_617c40);
    idx->field_c4 = g_617c00;
    idx->field_b0 = MulV(g_617c30, 4633260481411531256);
    idx->field_b8 = v17;
    idx->field_c8 = g_6173d8->field_0;
    idx->field_d0 = g_6173d8->field_8;
    idx->field_d8 = g_6173d8->field_10;
    idx->field_e0 = g_617510->field_0;
    idx->field_e8 = g_617510->field_8;
    v18 = g_617510->field_10;
    idx->field_f0 = v18;
    do
    {
        GetWheelSpeed(j & 0xffffffff);
        *((unsigned long long *)&idx->padding_f8[8 * j]) = v18;
        j += 1;
    } while (j != 4);
    k = 0;
    idx->field_118 = g_616998;
    idx->field_128 = g_6169a8;
    idx->field_120 = g_6169a0;
    idx->field_130 = g_6169b0;
    idx->field_138 = g_616928;
    idx->field_150 = g_616940;
    idx->field_140 = g_616930;
    idx->field_158 = g_616948;
    idx->field_148 = g_616938;
    idx->field_160 = g_616950;
    do
    {
        *((int *)&idx->padding_168[k]) = *((int *)&(&g_616958)[k]);
        k += 4;
    } while (k != 24);
    index = a1 + 25;
    idx->field_180 = a1[25];
    v1 = &v8;
    v3 = &v10;
    idx->field_188 = index[1];
    v9 = 0;
    v0 = 3;
    v11 = 0;
    v2 = 3;
    idx->field_190 = index[2];
    v4 = 3;
    v6 = 3;
    v5 = &idx->field_138;
    v7 = &idx->field_150;
    idx->field_198 = index[3];
    idx->field_1a0 = index[4];
    idx->field_1a8 = index[5];
    v8 = 0;
    v10 = 0;
    GCI2ECEF(&v0, &v4, index);
    GCI2ECEF(&v2, &v6, index);
    l = 0;
    do
    {
        *((long long *)&idx[1].padding_0[l]) = *((long long *)&v1[l]);
        *((long long *)&idx[1].padding_0[24 + l]) = *((long long *)&v3[l]);
        l += 8;
    } while (l != 24);
    PosVel2Elements_M(&idx[1].padding_0[48], &idx[1].padding_0[56], &idx[1].padding_0[64], &idx[1].padding_0[72], &idx[1].padding_0[80], &idx[1].padding_0[88], &idx[1].padding_0[96], &idx[1].padding_0[104], v5, v7);
    return;
}

