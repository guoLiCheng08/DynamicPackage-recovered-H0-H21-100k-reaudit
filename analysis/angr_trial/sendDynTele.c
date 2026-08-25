typedef struct FILE {
} FILE;

typedef struct struct_0 {
    char padding_0[360];
    char field_168;
} struct_0;

extern char g_40fbb8;
extern char g_40fbf8;
extern char g_40fc38;
extern char g_40fc70;
extern char g_615520;
extern char g_615524;
extern char g_615540;
extern char g_615544;
extern char g_61558c;
extern FILE *stderr;

int sendDynTele(unsigned long a0, struct_0 *a1)
{
    unsigned long long v3;  // rbp
    unsigned int v4;  // ebp
    unsigned long long v13;  // rbp
    unsigned int v14;  // r12d
    unsigned long long v15;  // rbp
    unsigned int v16;  // ebp
    unsigned long long v17;  // rbp
    unsigned long long v18;  // rbp
    unsigned long long v19;  // rbp
    unsigned long long v20;  // rbp
    unsigned long long v21;  // rbp
    unsigned long long v22;  // rbp
    int i;  // r13d
    unsigned int v23;  // r12d
    unsigned long v24;  // rdi
    int v25;  // eax
    unsigned long long v6;  // r15
    unsigned int v7;  // r14d
    unsigned long long v8;  // rbp
    unsigned int v9;  // ebp
    unsigned long long v10;  // rbp
    unsigned int v11;  // r15d
    int v12;  // r13d
    unsigned long v0;  // [bp-0x48]
    struct_0 *v1;  // [bp-0x40]

    v3 = 0;
    do
    {
        if (set_uint8_value((unsigned int)v3 + 3, a1->padding_0[96 + 4 * v3]))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->STS_valid[%d]\n");
    } while ((v3 += 1, v3 != 3));
    v1 = a1;
    v4 = 12;
    i = 0;
    do
    {
        v6 = 0;
        do
        {
            v7 = v4 + (unsigned int)v6;
            if (set_float_value(v7))
                __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fbb8, i, (unsigned int)v6, v7);
        } while ((v6 += 1, v6 != 4));
        i += 1;
        v4 += 4;
    } while (i != 3);
    if (set_uint8_value(6, g_615520))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->GYRO_valid[%d]\n", 0);
    if (set_uint8_value(7, g_615524))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->GYRO_valid[%d]\n", 1);
    v8 = 0;
    do
    {
        if (set_float_value((unsigned int)v8 + 24))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fbf8);
    } while ((v8 += 1, v8 != 3));
    v9 = 0;
    do
    {
        if (set_float_value(v9 + 27))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fbf8);
    } while ((v9 += 1, v9 != 3));
    v10 = 0;
    while (1)
    {
        __printf_chk(1, "output ->DSS_valid [%d]=%d\r\n", (unsigned int)v10, a1->padding_0[192 + 4 * v10]);
        if (set_uint8_value((unsigned int)v10 + 8, a1->padding_0[192 + 4 * v10]))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->DSS_valid[%d]\n", (unsigned int)v10);
        if (v10 == 1)
            break;
        v10 = 1;
    }
    v11 = 30;
    v12 = 0;
    while (1)
    {
        v13 = 0;
        while (1)
        {
            v14 = v11 + (unsigned int)v13;
            v0 = *((long long *)&v1->padding_0[160 + 8 * v13]);
            __printf_chk(1, "output ->DSS[%d][%d] = %f, \r\n", v12, (unsigned int)v13, *((long long *)&v1->padding_0[160 + 8 * v13]));
            if (set_float_value(v14))
                __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fc38, v12, (unsigned int)v13, v14);
            if (v13 == 1)
                break;
            v13 = 1;
        }
        v11 += 2;
        v1 = &v1->padding_0[16];
        if (v12 == 1)
            break;
        v12 = 1;
    }
    if (set_uint8_value(10, g_615540))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->MagMeter_valid[%d]\n", 0);
    if (set_uint8_value(11, g_615544))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->MagMeter_valid[%d]\n", 1);
    v15 = 0;
    do
    {
        if (set_float_value((unsigned int)v15 + 0x22))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fc70);
    } while ((v15 += 1, v15 != 3));
    v16 = 0;
    do
    {
        if (set_float_value(v16 + 37))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, &g_40fc70);
    } while ((v16 += 1, v16 != 3));
    v17 = 0;
    do
    {
        if (set_float_value((unsigned int)v17 + 40))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->RW_omega[%d]\n");
    } while ((v17 += 1, v17 != 4));
    if (set_float_value(62))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->SADA_Feedback[%d]\n", 0);
    if (set_float_value(63))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->SADA_Feedback[%d]\n", 1);
    if (set_float_value(219))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->SADA_Feedback[%d]\n", 0);
    if (set_float_value(220))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "output->SADA_Feedback[%d]\n", 1);
    v18 = 0;
    do
    {
        if (set_float_value((unsigned int)v18 + 44))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_R_I[%d]\n");
    } while ((v18 += 1, v18 != 3));
    v19 = 0;
    do
    {
        if (set_float_value((unsigned int)v19 + 47))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_V_I[%d]\n");
    } while ((v19 += 1, v19 != 3));
    v20 = 0;
    do
    {
        if (set_float_value((unsigned int)v20 + 50))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_R_E[%d]\n");
    } while ((v20 += 1, v20 != 3));
    v21 = 0;
    do
    {
        if (set_float_value((unsigned int)v21 + 53))
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_V_E[%d]\n");
    } while ((v21 += 1, v21 != 3));
    if (set_float_value(56))
        fwrite("recv_gnss_data->GPS_a\n", 1, 22, stderr);
    if (set_float_value(57))
        fwrite("recv_gnss_data->GPS_e\n", 1, 22, stderr);
    if (set_float_value(58))
        fwrite("recv_gnss_data->GPS_inc\n", 1, 24, stderr);
    if (set_float_value(59))
        fwrite("recv_gnss_data->GPS_OMEGA\n", 1, 26, stderr);
    if (set_float_value(60))
        fwrite("recv_gnss_data->GPS_omega\n", 1, 26, stderr);
    if (set_float_value(61))
        fwrite("recv_gnss_data->GPS_M\n", 1, 22, stderr);
    v22 = 0;
    while (1)
    {
        v23 = v22;
        v24 = (unsigned int)v22 + 12;
        if (v22)
        {
            if (set_uint8_value(v24, (&a1->field_168)[4 * v22]))
                goto LABEL_4027ed;
            if (v23 == 5)
                break;
        }
        else if (set_uint8_value(v24, a1->field_168 + 48))
        {
LABEL_4027ed:
            __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_time[%d]\n");
            if (v23 == 5)
                break;
        }
        v22 += 1;
    }
    if (set_uint8_value(18, g_61558c))
        __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_valid[%d]\n", 0);
    v25 = set_uint8_value(19, g_61558c);
    if (v25)
        return __fprintf_chk(*((unsigned int *)&stderr), 0x1, "recv_gnss_data->GPS_valid[%d]\n", 1);
    return v25;
}

