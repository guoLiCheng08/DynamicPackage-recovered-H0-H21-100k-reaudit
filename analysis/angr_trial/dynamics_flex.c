typedef struct struct_0 {
    char padding_0[8];
    struct struct_1 *field_8;
} struct_0;

typedef struct struct_1 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_1;

extern long long g_615b88;
extern long long g_615d68;
extern long long g_615f78;
extern long long g_616080;
extern unsigned long long g_616988;
extern unsigned long long g_616990;
extern unsigned long long g_6169b8;
extern unsigned long long g_6169c0;

void dynamics_flex(long long a0, long long a1, long long a2, long long a3, long long a4, long long a5, struct_0 *a6, struct_0 *a7, long long a8)
{
    int v134;  // xmm2
    int v135;  // xmm2
    int v144;  // xmm2
    char *v145;  // rax
    int v146;  // xmm2
    unsigned long long v147;  // xmm3lq
    unsigned long long v148;  // rax
    char *v149;  // rdx
    int v150;  // xmm3
    int v151;  // xmm0
    uint128_t v152;  // xmm2
    uint128_t v153;  // xmm2
    unsigned long long *v136;  // rax
    char *v137;  // rdi
    char *v138;  // rdx
    unsigned long long *v139;  // rsi
    char *v140;  // rax
    char *idx;  // rax
    int v142;  // xmm2
    int v143;  // xmm2
    long long v0;  // [bp-0x938], Other Possible Types: uint128_t
    char *v1;  // [bp-0x918], Other Possible Types: int
    int v2;  // [bp-0x908], Other Possible Types: char *, unsigned long long
    char *v3;  // [bp-0x8f8]
    unsigned long v4;  // [bp-0x8f0]
    unsigned int v5;  // [bp-0x8d8]
    char *v6;  // [bp-0x8d0]
    unsigned int v7;  // [bp-0x8c8]
    char *v8;  // [bp-0x8c0]
    unsigned int v9;  // [bp-0x8b8]
    char *v10;  // [bp-0x8b0]
    unsigned int v11;  // [bp-0x8a8]
    char *v12;  // [bp-0x8a0]
    unsigned int v13;  // [bp-0x898]
    char *v14;  // [bp-0x890]
    unsigned int v15;  // [bp-0x888]
    char *v16;  // [bp-0x880]
    unsigned int v17;  // [bp-0x878]
    char *v18;  // [bp-0x870]
    unsigned int v19;  // [bp-0x868]
    char *v20;  // [bp-0x860]
    unsigned int v21;  // [bp-0x858]
    char *v22;  // [bp-0x850]
    char v23;  // [bp-0x848], Other Possible Types: unsigned int
    char *v24;  // [bp-0x840]
    unsigned int v25;  // [bp-0x838]
    char *v26;  // [bp-0x830]
    unsigned int v27;  // [bp-0x828]
    char *v28;  // [bp-0x820]
    unsigned int v29;  // [bp-0x818]
    char *v30;  // [bp-0x810]
    unsigned int v31;  // [bp-0x808]
    char *v32;  // [bp-0x800]
    unsigned int v33;  // [bp-0x7f8]
    char *v34;  // [bp-0x7f0]
    unsigned int v35;  // [bp-0x7e8]
    char *v36;  // [bp-0x7e0]
    unsigned int v37;  // [bp-0x7d8]
    char *v38;  // [bp-0x7d0]
    unsigned int v39;  // [bp-0x7c8]
    char *v40;  // [bp-0x7c0]
    unsigned long v41;  // [bp-0x7b8]
    unsigned int v42;  // [bp-0x7b0]
    char *v43;  // [bp-0x7a8]
    unsigned long v44;  // [bp-0x798]
    unsigned int v45;  // [bp-0x790]
    char *v46;  // [bp-0x788]
    unsigned long v47;  // [bp-0x778]
    unsigned int v48;  // [bp-0x770]
    char *v49;  // [bp-0x768]
    unsigned long v50;  // [bp-0x758]
    unsigned int v51;  // [bp-0x750]
    char *v52;  // [bp-0x748]
    unsigned long v53;  // [bp-0x738]
    unsigned int v54;  // [bp-0x730]
    char *v55;  // [bp-0x728]
    unsigned long v56;  // [bp-0x718]
    unsigned int v57;  // [bp-0x710]
    char *v58;  // [bp-0x708]
    unsigned long v59;  // [bp-0x6f8]
    unsigned int v60;  // [bp-0x6f0]
    char *v61;  // [bp-0x6e8]
    char v62;  // [bp-0x6d8], Other Possible Types: unsigned long long
    unsigned long v63;  // [bp-0x6d0]
    unsigned long v64;  // [bp-0x6c8]
    char v65;  // [bp-0x6b8], Other Possible Types: unsigned long long
    unsigned long v66;  // [bp-0x6b0]
    unsigned long v67;  // [bp-0x6a8]
    int v68;  // [bp-0x698], Other Possible Types: char, unsigned long
    unsigned long v69;  // [bp-0x690]
    unsigned long v70;  // [bp-0x688], Other Possible Types: unsigned long long
    int v71;  // [bp-0x678]
    int v72;  // [bp-0x658], Other Possible Types: char
    unsigned long long v73;  // [bp-0x648]
    int v74;  // [bp-0x638]
    int v75;  // [bp-0x618]
    int v76;  // [bp-0x5f8]
    unsigned long v77;  // [bp-0x5f8]
    unsigned long v78;  // [bp-0x5f0]
    unsigned long v79;  // [bp-0x5e8], Other Possible Types: unsigned long long
    int v80;  // [bp-0x5d8]
    int v81;  // [bp-0x5b8]
    unsigned long long v82;  // [bp-0x5a8]
    int v83;  // [bp-0x598], Other Possible Types: char
    unsigned long long v84;  // [bp-0x588]
    int v85;  // [bp-0x578], Other Possible Types: char
    int v86;  // [bp-0x558]
    int v87;  // [bp-0x538], Other Possible Types: char
    unsigned long long v88;  // [bp-0x528]
    int v89;  // [bp-0x518]
    unsigned long long v90;  // [bp-0x508]
    int v91;  // [bp-0x4f8]
    uint128_t v92;  // [bp-0x4e8]
    uint128_t v93;  // [bp-0x4d8]
    uint128_t v94;  // [bp-0x4c8]
    unsigned long long v95;  // [bp-0x4b8]
    int v96;  // [bp-0x4a8]
    int v97;  // [bp-0x498]
    int v98;  // [bp-0x488]
    int v99;  // [bp-0x478]
    unsigned long long v100;  // [bp-0x468]
    int v101;  // [bp-0x458]
    int v102;  // [bp-0x448]
    int v103;  // [bp-0x438]
    int v104;  // [bp-0x428]
    unsigned long long v105;  // [bp-0x418]
    int v106;  // [bp-0x408], Other Possible Types: char
    int v107;  // [bp-0x3f8]
    int v108;  // [bp-0x3e8]
    int v109;  // [bp-0x3d8]
    unsigned long long v110;  // [bp-0x3c8]
    char v111;  // [bp-0x3b8]
    int v112;  // [bp-0x368], Other Possible Types: char
    int v113;  // [bp-0x358]
    int v114;  // [bp-0x348]
    int v115;  // [bp-0x338]
    int v116;  // [bp-0x328]
    int v117;  // [bp-0x318]
    int v118;  // [bp-0x308]
    int v119;  // [bp-0x2f8]
    int v120;  // [bp-0x2e8]
    int v121;  // [bp-0x2d8]
    char v122;  // [bp-0x2c8], Other Possible Types: uint128_t
    uint128_t v123;  // [bp-0x2b8]
    uint128_t v124;  // [bp-0x2a8]
    uint128_t v125;  // [bp-0x298]
    uint128_t v126;  // [bp-0x288]
    char v127;  // [bp-0x278], Other Possible Types: uint128_t
    uint128_t v128;  // [bp-0x268]
    uint128_t v129;  // [bp-0x258]
    uint128_t v130;  // [bp-0x248]
    uint128_t v131;  // [bp-0x238]
    char v132;  // [bp-0x228]
    char v133;  // [bp-0x138]

    v63 = g_616988;
    v6 = &v62;
    v64 = g_616990;
    v8 = &v65;
    v66 = g_6169b8;
    v0 = a0;
    v62 = 0;
    v91 = 0;
    v5 = 3;
    v65 = 0;
    v92 = 0;
    v67 = g_6169c0;
    v7 = 3;
    v93 = 0;
    v43 = &v91;
    v44 = 12884901898;
    v94 = 0;
    v1 = 0;
    v4 = 12884901898;
    v41 = 12884901891;
    v95 = 0;
    v42 = 3;
    v45 = 3;
    v46 = &v132;
    matrix_trans(&v44, &g_615f78);
    v3 = &v41;
    blas_gemm(&g_615f78, &v44, &v41);
    v134 = v1;
    v47 = 12884901891;
    v1 = &v47;
    v100 = 0;
    v96 = v134;
    v48 = 3;
    v49 = &v96;
    v97 = v134;
    v98 = v134;
    v99 = v134;
    v2 = v134;
    matrix_memcpy(&v47, a8, &v47);
    matrix_sub(v1, v3, v1);
    v70 = 0;
    v135 = v2;
    v10 = &v68;
    v136 = &a6->field_8->field_0;
    v68 = v135;
    v9 = 3;
    v1 = v135;
    v11 = 3;
    v68 = *(v136);
    v68 = (int)_INSERT(v68 CONCAT 0, 8, v136[1]);
    *((unsigned long long *)&(&v71)[16]) = 0;
    v71 = v135;
    v12 = &v71;
    v68 = (int)_INSERT(v68 CONCAT 0, 16, v136[2]);
    blas_gemv(a8, a1, &v11);
    v14 = &v72;
    v72 = v1;
    v73 = 0;
    v13 = 3;
    v74 = v72;
    *((unsigned long long *)&(&v74)[16]) = 0;
    v15 = 3;
    v16 = &v74;
    blas_gemv(&g_615b88, &v5, &v13);
    blas_gemv(&g_616080, &v13, &v15);
    *((unsigned long long *)&(&v75)[16]) = 0;
    v17 = 3;
    v75 = v1;
    v18 = &v75;
    blas_gemv(&g_615f78, a3, &v17);
    v137 = v12;
    v76 = v1;
    v79 = 0;
    v138 = v16;
    v139 = &a7->field_8->field_0;
    v20 = &v76;
    v140 = v18;
    v19 = 3;
    v21 = 3;
    v77 = AddV(AddV(AddV((long long)v71, *(v139)), (long long)v74), (long long)v75);
    v78 = AddV(AddV(AddV((long long)(&v71)[8], v139[1]), (long long)(&v74)[8]), (long long)(&v75)[8]);
    *((unsigned long long *)&(&v80)[16]) = 0;
    v80 = v76;
    v22 = &v80;
    v79 = AddV(AddV(AddV((long long)(&v71)[16], v139[2]), (long long)(&v74)[16]), (long long)(&v75)[16]);
    vector3_cross(a1, &v19, &v21);
    idx = v10;
    v68 = SubV((long long)v68, (long long)v80);
    v50 = v4;
    v69 = SubV((long long)(&v68)[8], (long long)(&v80)[8]);
    v51 = 3;
    v70 = SubV((long long)(&v68)[16], (long long)(&v80)[16]);
    v52 = &v133;
    matrix_trans(&v50, &g_615d68, v22, v4);
    v142 = v1;
    v53 = 12884901891;
    v105 = 0;
    v101 = v142;
    v54 = 3;
    v55 = &v101;
    v102 = v142;
    v103 = v142;
    v104 = v142;
    blas_gemm(&g_615f78, &v50, &v53);
    v56 = 12884901891;
    v110 = 0;
    v143 = v1;
    v57 = 3;
    v58 = &v106;
    v106 = v143;
    v107 = v143;
    v108 = v143;
    v109 = v143;
    matrix_memcpy(&v56, &g_616080);
    matrix_sub(&v56, &v53);
    v2 = &v23;
    v81 = v1;
    v82 = 0;
    v23 = 3;
    v24 = &v81;
    blas_gemv(&g_615b88, &v7, &v23, &v23);
    v84 = 0;
    v25 = 3;
    v26 = &v83;
    v83 = v1;
    blas_gemv(&v56, v2, &v25, v2);
    v144 = v1;
    v27 = 10;
    v28 = &v112;
    v112 = v144;
    v113 = v144;
    v114 = v144;
    v115 = v144;
    v116 = v144;
    blas_gemv("\n", a3, &v27);
    v85 = (int)_INSERT(v85 CONCAT 0, 16, 0);
    v29 = 3;
    v30 = &v85;
    v85 = v1;
    blas_gemv(&g_615f78, &v27, &v29);
    v145 = v30;
    v146 = v1;
    v31 = 10;
    v147 = MulV((long long)v85, 4591870180066957722);
    v117 = v146;
    v32 = &v117;
    *((unsigned long long *)&(&v86)[8]) = MulV((long long)(&v85)[8], 4591870180066957722);
    *((unsigned long long *)&v86) = v147;
    v2 = v147;
    *((unsigned long long *)&(&v86)[16]) = MulV(4591870180066957722, (long long)(&v85)[16]);
    v118 = v146;
    v119 = v146;
    v120 = v146;
    v121 = v146;
    blas_gemv("\n", a5, &v31);
    v88 = 0;
    v33 = 3;
    v34 = &v87;
    v87 = v1;
    blas_gemv(&g_615f78, &v31, &v33);
    v148 = 0;
    v149 = v10;
    v150 = (int)v2;
    while (1)
    {
        v151 = AddV(SubV(*((long long *)&v149[v148]), *((long long *)&v26[v148])), v150);
        *((unsigned long long *)&v149[v148]) = *((unsigned long long *)&v151);
        *((unsigned long long *)&v149[v148]) = *((unsigned long long *)&AddV(v151, *((long long *)&v34[v148])));
        v148 += 8;
        if (v148 == 24)
            break;
        v150 = (int)*((long long *)((char *)&v86 + v148));
    }
    inv_CAL_M3(v49, &v111);
    v61 = &v111;
    v59 = 12884901891;
    v60 = 3;
    blas_gemv(&v59, &v9, v0);
    blas_gemv(&v44, v0, a2);
    v90 = 0;
    v35 = 3;
    v89 = 0;
    v36 = &v89;
    v0 = 0;
    blas_gemv(&g_615b88, &v7, &v35);
    blas_gemv(&v50, &v35, a2);
    v152 = v0;
    v37 = 10;
    v38 = &v122;
    v122 = v152;
    v123 = v152;
    v124 = v152;
    v125 = v152;
    v126 = v152;
    blas_gemv("\n", a3, &v37);
    vector_add(a2, &v37);
    v153 = v0;
    v39 = 10;
    v40 = &v127;
    v127 = v153;
    v128 = v153;
    v129 = v153;
    v130 = v153;
    v131 = v153;
    blas_gemv("\n", a5, &v39);
    vector_add(a2, &v39);
    vector_memcpy(a4, a3);
    return;
}

