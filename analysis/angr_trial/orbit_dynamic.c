extern long long F_I_external;

void orbit_dynamic(long long a0, long long a1)
{
    char *v44;  // rax
    int v45;  // xmm8
    int v54;  // xmm2
    int v55;  // xmm7
    int v56;  // xmm5
    int v57;  // xmm0
    int v46;  // xmm4
    int v47;  // xmm7
    int v48;  // xmm3
    int v49;  // xmm1
    int v50;  // xmm2
    int v51;  // xmm5
    int v52;  // xmm3
    int v53;  // xmm0
    unsigned long v0;  // [bp-0x228]
    unsigned long long v1;  // [bp-0x220]
    unsigned long long v2;  // [bp-0x218]
    unsigned long v3;  // [bp-0x210]
    unsigned long long v4;  // [bp-0x208]
    unsigned long v5;  // [bp-0x200], Other Possible Types: unsigned long long
    unsigned long v6;  // [bp-0x1f8]
    char *v7;  // [bp-0x1f0]
    uint128_t v8;  // [bp-0x1e8]
    unsigned long v9;  // [bp-0x1d8]
    unsigned long long v10;  // [bp-0x1d0]
    unsigned int v11;  // [bp-0x1c8]
    char *v12;  // [bp-0x1c0]
    unsigned int v13;  // [bp-0x1b8]
    char *idx2;  // [bp-0x1b0]
    unsigned int v15;  // [bp-0x1a8]
    char *index;  // [bp-0x1a0]
    unsigned int v17;  // [bp-0x198]
    char *idx;  // [bp-0x190]
    unsigned int v19;  // [bp-0x188]
    char *idx1;  // [bp-0x180]
    unsigned int v21;  // [bp-0x178]
    char *v22;  // [bp-0x170]
    unsigned int v23;  // [bp-0x168]
    char *v24;  // [bp-0x160]
    int v25;  // [bp-0x158]
    int v26;  // [bp-0x138], Other Possible Types: unsigned long
    unsigned long v27;  // [bp-0x130]
    unsigned long v28;  // [bp-0x128], Other Possible Types: unsigned long long
    int v29;  // [bp-0x118], Other Possible Types: unsigned long
    unsigned long v30;  // [bp-0x110]
    unsigned long v31;  // [bp-0x108], Other Possible Types: unsigned long long
    int v32;  // [bp-0xf8], Other Possible Types: unsigned long
    unsigned long v33;  // [bp-0xf0]
    unsigned long v34;  // [bp-0xe8], Other Possible Types: unsigned long long
    int v35;  // [bp-0xd8], Other Possible Types: unsigned long
    unsigned long v36;  // [bp-0xd0]
    unsigned long v37;  // [bp-0xc8], Other Possible Types: unsigned long long
    int v38;  // [bp-0xb8]
    unsigned long long v39;  // [bp-0xa8]
    int v40;  // [bp-0x98]
    unsigned long long v41;  // [bp-0x88]
    char v42;  // [bp-0x78]

    *((uint128_t *)&v25) = 0;
    v12 = &v25;
    v26 = 0;
    v29 = 0;
    *((uint128_t *)&v32) = 0;
    v35 = 0;
    v38 = 0;
    idx2 = &v26;
    *((unsigned long long *)&(&v25)[16]) = 0;
    v11 = 3;
    v28 = 0;
    index = &v29;
    v13 = 3;
    v31 = 0;
    v15 = 3;
    idx = &v32;
    v34 = 0;
    v17 = 3;
    v37 = 0;
    idx1 = &v35;
    v19 = 3;
    v39 = 0;
    v21 = 3;
    v22 = &v38;
    v40 = 0;
    v24 = &v40;
    v41 = 0;
    v23 = 3;
    TimeArrayGet(&v42);
    GCI2ECEF(&v11, a1, &v42);
    vector_nrm2(&v11);
    v44 = v12;
    v45 = (int)(long long)v25;
    v7 = &v13;
    v46 = (int)(long long)(&v25)[16];
    v47 = (int)(long long)(&v25)[8];
    v27 = *((unsigned long long *)&v47);
    v28 = *((unsigned long long *)&v46);
    v26 = *((unsigned long long *)&v45);
    v0 = DivV(0x42f6a8665b911600, MulV(0x415854a640000000, 0x415854a640000000));
    v48 = DivV(v46, 0);
    v10 = DivV(0x415854a640000000, 0);
    v5 = *((unsigned long long *)&v48);
    v1 = *((unsigned long long *)&DivV(v45, 0));
    v49 = MulV(v48, v48);
    v2 = *((unsigned long long *)&DivV(v47, 0));
    v8 = 0x8000000000000000;
    v3 = *((unsigned long long *)&v49);
    v50 = MulV(v49, v48);
    v9 = *((unsigned long long *)&v50);
    v4 = *((unsigned long long *)&MulV(v48, v50));
    vector_scale(&v13, &v13);
    v51 = (int)(MulV(0x4014000000000000, v3));
    v52 = (int)v5;
    v6 = *((unsigned long long *)&v52);
    v5 = v10;
    v53 = SubV(0x3ff0000000000000, v51);
    v29 = *((unsigned long long *)&MulV(v1, v53));
    v30 = (unsigned long long)(MulV(v53, v2));
    v31 = *((unsigned long long *)&MulV(SubV(0x4008000000000000, v51), v52));
    vector_scale(&v15);
    v54 = (int)(MulV(SubV(MulV(v9, 0x401c000000000000), MulV(0x4008000000000000, v6)), 0x4014000000000000));
    v34 = MulV(SubV(SubV(MulV(v3, 0x4024000000000000), MulV(v4, 4622757367511340373)), 0x3ff0000000000000), 0x4008000000000000);
    v32 = *((unsigned long long *)&MulV(v1, v54));
    v33 = (unsigned long long)(MulV(v54, v2));
    vector_scale(&v17);
    v55 = (int)v3;
    v56 = (int)(MulV(v4, 0x404f800000000000));
    v57 = AddV(SubV(0x4008000000000000, MulV(0x4045000000000000, v55)), v56);
    v35 = *((unsigned long long *)&MulV(v1, v57));
    v36 = (unsigned long long)(MulV(v57, v2));
    v37 = *((unsigned long long *)&MulV(AddV(SubV(0x402e000000000000, MulV(v55, 0x4051800000000000)), v56) ^ v8, v6));
    vector_scale(&v19);
    vector_add(&v21, v7);
    vector_add(&v21, &v15);
    vector_add(&v21, &v17);
    vector_add(&v21, &v19);
    ECEF2GCI(a0, &v21, &v42);
    vector_axpy(&F_I_external, &v23);
    vector_add(a0, &v23);
    return;
}

