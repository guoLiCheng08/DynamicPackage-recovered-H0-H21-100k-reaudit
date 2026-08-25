typedef struct struct_0 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
    char padding_18[8];
    unsigned long long field_20;
    unsigned long long field_28;
    char padding_30[16];
    unsigned long long field_40;
} struct_0;

extern long long g_6156e8;
extern struct_0 *g_6156f8;
extern long long g_6157a8;
extern long long g_6157d0;
extern long long g_6157f8;
extern long long g_615800;
extern long long g_615828;
extern long long g_615888;
extern long long SatTorque;
extern long long g_616770;
extern long long g_616780;
extern long long g_616790;
extern long long g_6167a0;

void CoreDynamic(unsigned long long *idx, unsigned long long *index)
{
    unsigned long idx1;  // fs
    long long v70;  // rdi
    long long v71;  // rsi
    long long v72;  // rdx
    long long v73;  // rcx
    long long v74;  // r8
    long long v75;  // r9
    unsigned long long v76;  // xmm0lq
    unsigned long v77;  // 4117
    unsigned int v0;  // [bp-0x398]
    char *v1;  // [bp-0x390]
    unsigned int v2;  // [bp-0x388]
    char *idx2;  // [bp-0x380]
    unsigned int v4;  // [bp-0x378]
    char *v5;  // [bp-0x370]
    unsigned int v6;  // [bp-0x368]
    char *v7;  // [bp-0x360]
    unsigned int v8;  // [bp-0x358]
    char *v9;  // [bp-0x350]
    unsigned int v10;  // [bp-0x348]
    char *v11;  // [bp-0x340]
    unsigned int v12;  // [bp-0x338]
    char *v13;  // [bp-0x330]
    unsigned int v14;  // [bp-0x328]
    char *v15;  // [bp-0x320]
    unsigned int v16;  // [bp-0x318]
    char *v17;  // [bp-0x310]
    unsigned int v18;  // [bp-0x308]
    char *v19;  // [bp-0x300]
    unsigned int v20;  // [bp-0x2f8]
    char *v21;  // [bp-0x2f0]
    unsigned long v22;  // [bp-0x2e8]
    unsigned int v23;  // [bp-0x2e0]
    char *v24;  // [bp-0x2d8]
    unsigned long long v25;  // [bp-0x2c8]
    unsigned int v26;  // [bp-0x2c0]
    char *v27;  // [bp-0x2b8]
    unsigned long v28;  // [bp-0x2a8]
    unsigned int v29;  // [bp-0x2a0]
    char *v30;  // [bp-0x298]
    char v31;  // [bp-0x288], Other Possible Types: uint128_t, unsigned long
    unsigned long v32;  // [bp-0x280]
    unsigned long v33;  // [bp-0x278], Other Possible Types: unsigned long long
    int v34;  // [bp-0x268], Other Possible Types: unsigned long
    unsigned long v35;  // [bp-0x260]
    unsigned long v36;  // [bp-0x258], Other Possible Types: unsigned long long
    int v37;  // [bp-0x248], Other Possible Types: unsigned long
    unsigned long v38;  // [bp-0x240]
    unsigned long v39;  // [bp-0x238], Other Possible Types: unsigned long long
    int v40;  // [bp-0x228], Other Possible Types: unsigned long
    unsigned long v41;  // [bp-0x220]
    unsigned long v42;  // [bp-0x218], Other Possible Types: unsigned long long
    char v43;  // [bp-0x208], Other Possible Types: uint128_t
    unsigned long long v44;  // [bp-0x1f8]
    char v45;  // [bp-0x1e8], Other Possible Types: uint128_t, unsigned long
    unsigned long v46;  // [bp-0x1e0]
    unsigned long v47;  // [bp-0x1d8], Other Possible Types: unsigned long long
    int v48;  // [bp-0x1c8], Other Possible Types: char
    int v49;  // [bp-0x1a8]
    int v50;  // [bp-0x188]
    int v51;  // [bp-0x168]
    int v52;  // [bp-0x148]
    int v53;  // [bp-0x128]
    int v54;  // [bp-0x108]
    uint128_t v55;  // [bp-0xf8]
    uint128_t v56;  // [bp-0xe8]
    int v57;  // [bp-0xd8]
    uint128_t v58;  // [bp-0xc8]
    uint128_t v59;  // [bp-0xb8]
    uint128_t v60;  // [bp-0xa8]
    unsigned long long v61;  // [bp-0x98]
    char v62;  // [bp-0x88], Other Possible Types: uint128_t
    uint128_t v63;  // [bp-0x78]
    uint128_t v64;  // [bp-0x68]
    uint128_t v65;  // [bp-0x58]
    unsigned long long v66;  // [bp-0x48]
    unsigned long v67;  // [bp-0x40]

    v67 = *((long long *)(40 + idx1));
    v33 = 0;
    v54 = 0;
    v1 = &v31;
    v0 = 3;
    v55 = 0;
    v56 = 0;
    v31 = 0;
    v34 = 0;
    v37 = 0;
    v40 = 0;
    v57 = 0;
    v58 = 0;
    v59 = 0;
    v60 = 0;
    idx2 = &v34;
    v36 = 0;
    v2 = 3;
    v39 = 0;
    v5 = &v37;
    v4 = 3;
    v42 = 0;
    v6 = 3;
    v7 = &v40;
    v61 = 0;
    v22 = 12884901891;
    v24 = &v57;
    v28 = 12884901891;
    v9 = &v43;
    v23 = 3;
    v30 = &v62;
    v44 = 0;
    v11 = &v45;
    v25 = 0x3ff0000000000000;
    v43 = 0;
    v27 = &v48;
    v8 = 3;
    v45 = 0;
    v48 = (int)_INSERT(v48 CONCAT 0, 0, 0);
    *((uint128_t *)&v49) = 0;
    *((uint128_t *)&v50) = 0;
    v62 = 0;
    v63 = 0;
    v64 = 0;
    v65 = 0;
    v13 = &v49;
    v47 = 0;
    v10 = 3;
    *((unsigned long long *)&(&v48)[16]) = 0;
    v15 = &v50;
    v26 = 3;
    *((unsigned long long *)&(&v49)[16]) = 0;
    v12 = 3;
    *((unsigned long long *)&(&v50)[16]) = 0;
    v14 = 3;
    v66 = 0;
    v29 = 3;
    *((uint128_t *)&v51) = 0;
    v17 = &v51;
    *((unsigned long long *)&(&v51)[16]) = 0;
    *((uint128_t *)&v52) = 0;
    *((uint128_t *)&v53) = 0;
    v19 = &v52;
    v16 = 3;
    *((unsigned long long *)&(&v52)[16]) = 0;
    v18 = 3;
    *((unsigned long long *)&(&v53)[16]) = 0;
    v21 = &v53;
    v20 = 3;
    TimeAdd();
    TimeArrayGet(&v54);
    MagUpdate(&v54);
    v31 = index[6];
    v32 = index[7];
    v33 = index[8];
    MagTorque(&g_616770, &v0, &g_6157d0);
    v34 = *(index);
    v35 = index[1];
    v36 = index[2];
    RWheelTorque(&SatTorque, &v2);
    v37 = index[12];
    v38 = index[13];
    v39 = index[14];
    vector_memcpy(&g_616780, &v4);
    GravityGradientTorque(&g_616790, &g_6157f8, &g_6156e8, &g_615888);
    MagTorque(&g_6167a0, &g_6157a8, &g_6157d0);
    UpdateTorque(v70, v71, v72, v73, v74, v75);
    v40 = index[9];
    v41 = index[10];
    v42 = index[11];
    get_Body2Inertial(&v22);
    blas_gemv(&v22, &v6, &v8);
    UpdateExternalForce(&v8);
    v45 = index[3];
    v46 = index[4];
    v47 = index[5];
    SetWheelAngularMoment(&v10);
    RK4_Intergrator(v70, v71, v72, v73, v74, v75);
    get_attitude(&v25, &v12);
    *(idx) = v25;
    idx[1] = (long long)v48;
    idx[2] = (long long)(&v48)[8];
    idx[3] = (long long)(&v48)[16];
    idx[4] = (long long)v49;
    idx[5] = (long long)(&v49)[8];
    v76 = (long long)(&v49)[16];
    idx[6] = v76;
    quat_att_mat(&g_615888, &v25);
    TimeArrayGet(idx + 25);
    TimeTotalGet(v70, v71, v72, v73, v74, v75);
    idx[31] = v76;
    Get_Orbit_RV(idx + 19, idx + 22);
    Get_Orbit_RV(g_615800, g_615828);
    GetInertialMag(&v14);
    idx[16] = (long long)v50;
    idx[0x11] = (long long)(&v50)[8];
    idx[18] = (long long)(&v50)[16];
    quat_att_mat(&v28, &v25);
    blas_gemv(&v28, &v14, &v16);
    idx[13] = (long long)v51;
    idx[14] = (long long)(&v51)[8];
    idx[15] = (long long)(&v51)[16];
    vector_memcpy(&g_6157d0, &v16);
    GetSunVector(&v18, &v54);
    idx[10] = (long long)v52;
    idx[11] = (long long)(&v52)[8];
    idx[12] = (long long)(&v52)[16];
    blas_gemv(&v28, &v18, &v20);
    idx[7] = (long long)v53;
    idx[8] = (long long)(&v53)[8];
    idx[9] = (long long)(&v53)[16];
    idx[32] = g_6156f8->field_0;
    idx[33] = g_6156f8->field_20;
    idx[0x22] = g_6156f8->field_40;
    idx[35] = g_6156f8->field_8;
    idx[36] = g_6156f8->field_10;
    v77 = *((long long *)(40 + idx1));
    idx[37] = g_6156f8->field_28;
    idx[38] = index[15];
    idx[39] = index[16];
    idx[40] = index[0x11];
    if (v67 ^ v77)
        __stack_chk_fail(); /* do not return */
    return;
}

