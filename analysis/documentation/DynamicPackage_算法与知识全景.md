# DynamicPackage 算法、公式与知识全景

**文档性质：**离线行为等价重建工程的技术交接文档。  
**覆盖基线：**H0–H20 受控场景、当前 Linux x86-64、当前运行时库、严格 C11 构建。  
**编写目的：**将工程内已经反汇编审计、C11 重建并以原 ELF 差分验证的算法，按“数学模型—源码实现—输入输出—验证边界”的方式统一说明，便于继续研究、审计和维护。

> **重要定义。** 本工程的目标是对给定 ELF 进行**行为等价重建**，而不是声称已经恢复原始作者的全部 C 源代码或获得任意输入上的数学全等证明。文中的“与 ELF 一致”均表示：在明确的输入、随机种子、当前平台、当前编译选项和指定观察块下，通过原 ELF gold 的逐步逐字节比较得到的一致结果。

---

## 目录

1. [系统总览与数据流](#1-系统总览与数据流)
2. [符号、坐标系与 33 维状态](#2-符号坐标系与-33-维状态)
3. [线性代数、旋转与四元数](#3-线性代数旋转与四元数)
4. [时间、儒略日、恒星时与坐标变换](#4-时间儒略日恒星时与坐标变换)
5. [轨道力学与轨道要素](#5-轨道力学与轨道要素)
6. [环境模型：太阳、地磁与局地坐标](#6-环境模型太阳地磁与局地坐标)
7. [力矩、刚体动力学与 33 维微分方程](#7-力矩刚体动力学与-33-维微分方程)
8. [柔性刚体耦合动力学](#8-柔性刚体耦合动力学)
9. [RK4 数值积分与浮点轨迹](#9-rk4-数值积分与浮点轨迹)
10. [执行机构与设备命令](#10-执行机构与设备命令)
11. [传感器、随机噪声与有效性判定](#11-传感器随机噪声与有效性判定)
12. [主调度、遥测与 IPC 输出](#12-主调度遥测与-ipc-输出)
13. [输入输出覆盖、验证证据与当前完成度](#13-输入输出覆盖验证证据与当前完成度)
14. [位级一致性的工程要点](#14-位级一致性的工程要点)
15. [已知边界、非目标与继续扩展方法](#15-已知边界非目标与继续扩展方法)
16. [源码索引与参考](#16-源码索引与参考)

---

## 1. 系统总览与数据流

DynamicPackage 是一个以离散步长推进的**姿态—轨道—柔性—执行机构—传感器—遥测**联合仿真包。单步不是单一公式，而是以固定顺序连接多个模块；该顺序本身会改变浮点舍入、随机数消耗和输出，因此属于可观察行为的一部分。[S12] [S13]

```text
输入初态/时间/设备命令
        │
        ▼
UpdateDeviceControl ──► 飞轮/MTQ/推力器/SADA 状态
        │                         │
        ▼                         ▼
CoreDynamic / RK4 ◄──── 执行机构角动量与力矩注入
        │
        ├──► 时间推进、太阳向量、地磁场、GCI/ECEF
        ├──► 33 维姿态—轨道—柔性状态传播
        └──► 环境结果
                  │
                  ▼
           UpdateDeviceMeasure
                  │
                  ▼
       主遥测帧（544 B）──► IPC payload（3000 B）
```

| 层级 | 主要任务 | 代表实现 |
|---|---|---|
| 数学基础层 | 向量、矩阵、四元数、3×3 求逆、角度与随机数 | `dynamic_math.c` |
| 时间与坐标层 | JD、GMST、岁差、GCI/ECEF 旋转 | `dynamic_time.c` |
| 轨道与环境层 | 二体/J2/J3/J4、太阳、地磁、局地坐标 | `dynamic_orbit.c`、`dynamic_environment.c` |
| 动力学层 | 33 维 ODE、刚体/柔性耦合、RK4 | `dynamic_dynamics.c`、`dynamic_flex.c`、`dynamic_rk4.c` |
| 设备与传感器层 | 飞轮、MTQ、推力器、SADA、STS、陀螺、DSS、GPS | `dynamic_devices.c`、`dynamic_sensors.c` |
| 输出接口层 | 主遥测、IPC 字节序列化、共享内存 | `dynamic_telemetry.c`、`dynamic_ipc_telemetry.c` |

这里的“控制”是软件仿真中的命令输入与状态更新。**本工程只限离线研究/仿真，不得用于飞行、实体执行机构控制或任何安全关键用途。**

---

## 2. 符号、坐标系与 33 维状态

### 2.1 记号

| 符号 | 含义 | 典型单位 |
|---|---|---|
| \(\mathbf r,\mathbf v\) | GCI 惯性系位置、速度 | m、m/s |
| \(\boldsymbol\omega\) | 本体系角速度 | rad/s |
| \(q=[q_0,q_1,q_2,q_3]^T\) | 标量在前的姿态四元数 | 无量纲 |
| \(\mathbf J\) | 刚体惯量矩阵 | kg·m² |
| \(\boldsymbol\eta,\dot{\boldsymbol\eta}\) | 柔性模态位移、速度 | 模态坐标、模态坐标/s |
| \(\mathbf C\) | 3×\(n\) 刚柔耦合矩阵 | 由卫星配置决定 |
| \(\mathbf M_A,\mathbf M_D\) | 柔性模态矩阵（恢复命名） | 由卫星配置决定 |
| \(\mu\) | 地球引力参数 \(3.986004415\times10^{14}\) | m³/s² |
| \(R_e\) | 地球参考半径 \(6\,378\,137\) | m |
| \(\mathbf B\) | 本体系磁场 | 模型输出单位 |
| \(\mathbf m\) | 磁力矩器磁矩 | A·m² 或模型单位 |

### 2.2 坐标系

| 坐标系 | 含义 | 在工程中的主要用途 |
|---|---|---|
| **GCI** | 地心惯性坐标系 | 轨道状态、太阳向量、惯性地磁、GPS 位置速度 |
| **ECEF** | 地球固连坐标系 | 地球重力带谐项、局地地磁、遥测位置/速度 |
| **Body** | 卫星本体坐标系 | 角速度、惯量、执行机构、传感器、力矩 |
| **LocalGeo / LVLH** | 局地北东地下/局地轨道参考 | 局地几何与视向处理 |

### 2.3 33 维状态布局

恢复端按照固定 33 个 `double` 组织状态。前 13 项为姿态、角速度和轨道状态；后 20 项对应 10 阶柔性模态的位置和速度。该布局直接驱动微分方程、RK4 缓冲区、gold 采集和 internal global `y[33]` 比较。[S8] [S10]

| 下标 | 数量 | 变量 | 微分关系 |
|---:|---:|---|---|
| 0–3 | 4 | \(q_0,q_1,q_2,q_3\) | \(\dot q\) 由四元数运动学给出 |
| 4–6 | 3 | \(\omega_x,\omega_y,\omega_z\) | \(\dot{\boldsymbol\omega}=\boldsymbol\alpha\) |
| 7–9 | 3 | \(r_x,r_y,r_z\) | \(\dot{\mathbf r}=\mathbf v\) |
| 10–12 | 3 | \(v_x,v_y,v_z\) | \(\dot{\mathbf v}=\mathbf a_{orbit}\) |
| 13–22 | 10 | \(\boldsymbol\eta\) | \(\dot{\boldsymbol\eta}\) |
| 23–32 | 10 | \(\dot{\boldsymbol\eta}\) | \(\ddot{\boldsymbol\eta}\) |

> **知识点。** “状态”是积分器保存的最小动态记忆；“导数”描述状态的瞬时变化率。每一个仿真步都先根据当前状态计算 \(\dot y\)，再由积分器计算下一时刻的 \(y\)。

---

## 3. 线性代数、旋转与四元数

### 3.1 向量与矩阵基础

三维叉乘、点积、欧氏范数和夹角是后续几乎所有模块的基础：

$$
\mathbf a\times\mathbf b=
\begin{bmatrix}
a_yb_z-a_zb_y\\
a_zb_x-a_xb_z\\
a_xb_y-a_yb_x
\end{bmatrix},\qquad
\mathbf a\cdot\mathbf b=\sum_i a_i b_i,
$$

$$
\lVert\mathbf a\rVert_2=\sqrt{\mathbf a^T\mathbf a},\qquad
\theta(\mathbf a,\mathbf b)=
\arccos\!\left(\frac{\mathbf a^T\mathbf b}{\lVert\mathbf a\rVert_2\lVert\mathbf b\rVert_2}\right).
$$

源码中的 `vector2angle` 在维数不一致、或任一范数不大于零时返回 `0.0`，而不是抛出异常；`vector_unit` 仅在范数严格大于零时写入归一化结果。这个边界语义会影响 DSS/STS 有效性和非有限值传播。[S1]

矩阵乘向量和矩阵乘矩阵分别为

$$
\mathbf y\leftarrow\alpha\mathbf A\mathbf x+\beta\mathbf y,
\qquad
\mathbf C\leftarrow\alpha\mathbf A\mathbf B+\beta\mathbf C.
$$

恢复实现没有把这些式子交给外部 BLAS；它按固定循环、固定逐项累加顺序实现，以对齐原 ELF 的浮点舍入与 qNaN 符号传播。[S1]

### 3.2 基本旋转

工程使用行主序的**被动**旋转矩阵。例如 Z 轴矩阵为

$$
R_Z(\phi)=
\begin{bmatrix}
\cos\phi&\sin\phi&0\\
-\sin\phi&\cos\phi&0\\
0&0&1
\end{bmatrix}.
$$

同一约定下，\(R_X\) 与 \(R_Y\) 的非零正负号不能任意替换为其他教材约定；它们直接决定 GCI/ECEF、局地坐标和姿态矩阵的方向。[S1] [S2]

### 3.3 四元数

四元数写为 \(q=[q_0,\mathbf q_v]^T\)，其中 \(q_0\) 是标量部。范数为

$$
\lVert q\rVert=\sqrt{q_0^2+q_1^2+q_2^2+q_3^2},
\qquad q\leftarrow q/\lVert q\rVert.
$$

恢复端的四元数运动学通过矩阵 \(\Xi(q)\) 计算：

$$
\Xi(q)=
\begin{bmatrix}
q_0&-q_3&q_2\\
q_3&q_0&-q_1\\
-q_2&q_1&q_0\\
-q_1&-q_2&-q_3
\end{bmatrix},
\qquad
\dot q=\frac12\Xi(q)\boldsymbol\omega.
$$

姿态矩阵按实现的 \(\Psi(q)\)、\(\Xi(q)\) 组合构造：

$$
\mathbf A(q)=\Xi(q)^T\Psi(q).
$$

该方式等价于常见的四元数方向余弦矩阵构造，但在本工程中不能为“更简洁”的显式多项式展开式而替换，因为乘加顺序可改变末位或 NaN 符号。[S1]

312 欧拉角到四元数的实现为，设 \(c_a=\cos(a/2),s_a=\sin(a/2)\) 等：

$$
\begin{aligned}
q_0&=c_ac_bc_c-s_as_bs_c,\\
q_1&=c_as_bc_c-s_ac_bs_c,\\
q_2&=c_ac_bs_c+s_as_bc_c,\\
q_3&=s_ac_bc_c+c_as_bs_c.
\end{aligned}
$$

> **知识点。** 四元数避免欧拉角在某些姿态下的万向节锁，但必须保持单位范数。积分过程中本工程先按 RK4 更新，再做逐分量的单位化；这个时序是已验证行为的一部分。[S1] [S10]

### 3.4 3×3 逆矩阵

对矩阵 \(\mathbf A=[a_{ij}]\)，实现按伴随矩阵计算

$$
\mathbf A^{-1}=\frac{\operatorname{adj}(\mathbf A)}{\det(\mathbf A)}.
$$

其中行列式和每个余子式的具体乘减/除法顺序保留为原 ELF 顺序，且没有额外奇异性保护。因此当 \(\det(\mathbf A)=0\) 或非有限时，结果按 IEEE-754 自然产生无穷大或 NaN；不能擅自加入钳位而声称等价。[S1]

---

## 4. 时间、儒略日、恒星时与坐标变换

### 4.1 儒略日与儒略世纪

工程采用以下日历输入（年、月、日、时、分、秒）计算 JD：

$$
\begin{aligned}
A&=\left\lfloor\frac{m+9}{12}\right\rfloor,\\
JD&=367y-\lfloor1.75(y+A)\rfloor+
\left\lfloor\frac{275m}{9}\right\rfloor+d+1721013.5+\frac{h+(min+s/60)/60}{24}.
\end{aligned}
$$

儒略世纪为

$$T=\frac{JD-2451545.0}{36525}.$$

这个公式为太阳模型、岁差和 GMST 提供时间自变量。[S2]

### 4.2 格林尼治平恒星时

先按当天零点计算 \(T\)，再把恒星秒归一化为弧度：

$$
\begin{aligned}
\theta_s={}&24110.54841+8640184.812866T+0.093104T^2-0.0000062T^3\\
&+1.0027379093507951\,(3600h+60m+s),\\
\theta_{GMST}={}&\operatorname{fmod}(\theta_s,86400)\frac{\pi}{43200}.
\end{aligned}
$$

> **知识点。** 恒星时不是普通太阳日时间；它描述地球相对惯性空间的转角，因此用于 GCI 与 ECEF 的相互旋转。

### 4.3 岁差与 GCI/ECEF

岁差矩阵为

$$\mathbf P(T)=R_Z(-z)R_Y(\theta)R_Z(-\zeta),$$

其中 \(\zeta,\theta,z\) 均是 \(T\) 的三次多项式，系数固化在实现中。坐标变换按照

$$
\mathbf r_{ECEF}=R_Z(\theta_{GMST})\mathbf P(T)\mathbf r_{GCI},
$$

$$
\mathbf r_{GCI}=\left[R_Z(\theta_{GMST})\mathbf P(T)\right]^T\mathbf r_{ECEF}
$$

执行。逆变换明确构造同一组合矩阵后再转置，不以“直接写逆矩阵”替换。[S2]

### 4.4 仿真时间推进

`TimeAdd(Δt)` 保存小数秒残量；当残量达到一整秒时，才通过 `mktime` 归一化日历秒、分、时、日、月、年。这个机制已在跨年和闰日场景中进入 gold 对照。[S2]

---

## 5. 轨道力学与轨道要素

### 5.1 开普勒方程

对于椭圆轨道，平均近点角 \(M\)、偏近点角 \(E\) 满足

$$M=E-e\sin E.$$

实现用 Newton 迭代：

$$
E_{k+1}=E_k+
\frac{M-(E_k-e\sin E_k)}{1-e\cos E_k},
$$

初值 \(E_0=M\)，阈值 \(|\Delta E|<10^{-10}\)，最多 10,000 次。这样的迭代式是轨道要素到位置速度转换的核心。[S3]

### 5.2 轨道要素到位置/速度

令

$$n=\sqrt{\frac{\mu}{a^3}},\quad
\beta=\sqrt{1-e^2},\quad
D=a(1-e\cos E),$$

轨道平面内状态为

$$
\begin{aligned}
x_p&=a(\cos E-e),& y_p&=a\beta\sin E,\\
v_{x,p}&=-\frac{a^2n\sin E}{D},&v_{y,p}&=\frac{a^2n\beta\cos E}{D}.
\end{aligned}
$$

之后按

$$\mathbf r_{GCI}=R_3(\Omega)R_1(i)R_3(\omega)\mathbf r_p,$$

$$\mathbf v_{GCI}=R_3(\Omega)R_1(i)R_3(\omega)\mathbf v_p$$

旋转到 GCI。这里 \(a,e,i,\Omega,\omega\) 分别是半长轴、偏心率、倾角、升交点赤经、近地点幅角。[S3]

真近点角路径还使用

$$p=a(1-e^2),\qquad r=\frac{p}{1+e\cos\nu},$$

并采用单独的三角函数求值路径，以对齐原实现的浮点轨迹。[S3]

### 5.3 位置/速度到轨道要素

恢复端首先计算

$$
\mathbf h=\mathbf r\times\mathbf v,\qquad
r=\lVert\mathbf r\rVert,\qquad
v^2=\mathbf v^T\mathbf v,
$$

$$
a=\frac{\mu r}{2\mu-v^2r},\qquad
 e\cos E=1-\frac{r}{a},\qquad
 e\sin E=\frac{\mathbf r\cdot\mathbf v}{\sqrt{\mu a}}.
$$

由此得到 \(e\)、倾角、RAAN、近地点幅角、真近点角、平均近点角和纬度幅角。对近圆 \((e\le10^{-14})\) 与近赤道 \((|\sin i|\le10^{-14})\) 输入，源码中存在明确退化分支。[S3]

### 5.4 二体引力、带谐项与外力

令 \(r=\lVert\mathbf r\rVert\)，\(s=z/r\)，\(\rho=R_e/r\)，\(k=\mu/R_e^2\)。中心引力为

$$
\mathbf a_0=-\frac{\mu}{r^3}\mathbf r.
$$

源码按“中心项 → J2 → J3 → J4”的顺序累加。J2 项可写为

$$
\mathbf a_{J2}=-\frac32J_2k\rho^2
\begin{bmatrix}
(x/r)(1-5s^2)\\
(y/r)(1-5s^2)\\
(z/r)(3-5s^2)
\end{bmatrix}.
$$

本实现的 J3/J4 采用反汇编得到的展开多项式，而不是运行时通用 Legendre 梯度循环：

$$
\mathbf a_{J3}=-\frac12J_3k\rho^3
\begin{bmatrix}
(x/r)\,5(7s^3-3s)\\
(y/r)\,5(7s^3-3s)\\
3\left(10s^2-\frac{35}{3}s^4-1\right)
\end{bmatrix},
$$

$$
\mathbf a_{J4}=-\frac58J_4k\rho^4
\begin{bmatrix}
(x/r)(3-42s^2+63s^4)\\
(y/r)(3-42s^2+63s^4)\\
-(z/r)(15-70s^2+63s^4)
\end{bmatrix}.
$$

最后再加入外力加速度：

$$\mathbf a=\mathbf a_0+\mathbf a_{J2}+\mathbf a_{J3}+\mathbf a_{J4}+\frac{\mathbf F_{external}}{m}.$$

真实调用先将位置从 GCI 旋到 ECEF，在 ECEF 中计算带谐重力，再旋回 GCI 后添加外力；顺序不可省略。[S3]

> **知识点。** J2 描述地球赤道隆起带来的主导非球形摄动；J3/J4 是更高阶的轴对称重力带谐修正。高偏心轨道会使这些细小项的浮点累积更敏感。

---

## 6. 环境模型：太阳、地磁与局地坐标

### 6.1 太阳向量

令 \(T=(JD-2451545)/36525\)，实现先求以周为单位的小数相位：

$$
M=2\pi\,\operatorname{frac}(99.9973583T+0.9931267),
$$

$$
L=2\pi\,\operatorname{frac}\left(\frac{M}{2\pi}+0.7859444+
\frac{6892\sin M+72\sin2M}{1296000}\right),
$$

$$r_\odot=149619000000-2499000000\cos M-21000000\cos2M.$$

黄道平面向量 \([r_\odot\cos L,r_\odot\sin L,0]^T\) 经过固定黄赤交角 \(-0.40909280420293637\) 的 X 轴旋转，得到 GCI 太阳向量。[S4]

### 6.2 地磁模型

地磁模型使用至 13 阶的球谐系数（数组为 14×14），系数随十进制年线性变化：

$$g_n^m(t)=g_n^m(t_0)+\dot g_n^m(t-t_0),\qquad
h_n^m(t)=h_n^m(t_0)+\dot h_n^m(t-t_0).$$

关联 Legendre 函数 \(P_n^m\) 与导数以递推方式构造；局地磁场三分量累加含 \((a/r)^{n+2}\)、\(\cos(m\lambda)\)、\(\sin(m\lambda)\) 和 \(P_n^m\) 的每阶每次项。然后执行

```text
GCI 位置 → ECEF → 局地地理坐标 → 地磁球谐 → ECEF → GCI → 固定比例缩放
```

得到惯性地磁向量。[S4]

> **知识点。** 这里是“地磁球谐场”而不是简单偶极子。球谐级数用许多 \((n,m)\) 项逼近地球磁场的经纬度和高度变化；当前实现使用原 ELF 提取的固定系数表。

### 6.3 局地北东地下与 LVLH

对 ECEF 位置 \(\mathbf r\)，构造

$$
\hat{\mathbf d}=-\frac{\mathbf r}{\lVert\mathbf r\rVert},\qquad
\hat{\mathbf e}=\frac{\hat{\mathbf z}\times\mathbf r}{\lVert\hat{\mathbf z}\times\mathbf r\rVert},\qquad
\hat{\mathbf n}=\frac{\hat{\mathbf e}\times\hat{\mathbf d}}{\lVert\hat{\mathbf e}\times\hat{\mathbf d}\rVert}.
$$

把 \(\hat{\mathbf n},\hat{\mathbf e},\hat{\mathbf d}\) 作为矩阵行，得到 ECEF→LocalGeo 旋转。LVLH 辅助函数先在该局地系中投影参考向量，再以其平面角进行 Z 轴转动。[S4]

---

## 7. 力矩、刚体动力学与 33 维微分方程

### 7.1 外部和执行机构力矩

| 力矩 | 实现公式 | 直观解释 |
|---|---|---|
| 飞轮反作用 | \(\boldsymbol\tau_{RW}=-\boldsymbol\tau_{wheel-group}\) | 轮加速对本体产生反向作用力矩 |
| 磁力矩器 | \(\boldsymbol\tau_{MTQ}=\mathbf m\times\mathbf B\) | 磁偶极与地磁场耦合产生力矩 |
| 重力梯度 | \(\boldsymbol\tau_{gg}=3\mu r^{-3}\,[\hat{\mathbf r}_B\times(\mathbf J\hat{\mathbf r}_B)]\) | 非球形惯量在地球梯度场中趋向特定姿态 |
| 气动力矩 | 先沿速度修正阻力项，再经投影矩阵和固定力臂叉乘 | 稀薄大气相对速度引起的扰动力矩 |

重力梯度中的 \(\hat{\mathbf r}_B\) 是先经姿态矩阵投影到本体系的位置单位向量。[S11] [S4]

### 7.2 刚体欧拉方程

独立刚体例程按

$$
\mathbf H=\mathbf J\boldsymbol\omega+\mathbf H_{add},
$$

$$
\boldsymbol\alpha=\mathbf J^{-1}
\left(\mathbf T_{ext}-\boldsymbol\omega\times\mathbf H\right)
$$

计算角加速度。这里 \(\mathbf H_{add}\) 可以包含飞轮角动量或其他耦合动量；在软件中外部项和叉乘项按固定顺序累加。[S8]

### 7.3 33 维方程的总装

主方程将状态拆分为 \(q,\boldsymbol\omega,\mathbf r,\mathbf v,\boldsymbol\eta,\dot{\boldsymbol\eta}\)。总体结构为

$$
\dot y=
\begin{bmatrix}
\dot q\\
\boldsymbol\alpha\\
\mathbf v\\
\mathbf a_{orbit}\\
\ddot{\boldsymbol\eta}\\
\dot{\boldsymbol\eta}
\end{bmatrix}.
$$

进入柔性求解前，系统按如下顺序形成总角动量：

$$
\mathbf H_{total}=
\mathbf J\boldsymbol\omega+
\mathbf H_w+
\mathbf H_{SADA,cmd}+
\mathbf C\boldsymbol\eta.
$$

顺序是实现的一部分：先基底惯量乘角速度、再加飞轮、再加 SADA 命令映射、最后加柔性耦合动量。[S8]

---

## 8. 柔性刚体耦合动力学

柔性模型是系统中公式最密集的部分。令 \(n=10\) 为当前模态数，\(\mathbf C\in\mathbb R^{3\times n}\) 为耦合矩阵，\(\mathbf M_A,\mathbf M_D\in\mathbb R^{n\times n}\) 为模态矩阵。

### 8.1 有效惯量

恢复实现先构造

$$\mathbf J_{eff}=\mathbf J-\mathbf C\mathbf C^T.$$

然后显式求逆 \(\mathbf J_{eff}^{-1}\)。[S9]

### 8.2 刚体右端项与角加速度

令 \(\mathbf L_c\) 为恢复配置中的外加/偏置力矩项，\(\mathbf a_S\) 为 SADA 角加速度输入，\(\mathbf R_S\) 为其刚体反作用映射。刚体 RHS 的结构可概括为

$$
\begin{aligned}
\mathbf b={}&\mathbf L_c-
\boldsymbol\omega\times\mathbf H_{total}
-\mathbf R_S\begin{bmatrix}0\\a_{S,0}\\a_{S,1}\end{bmatrix}\\
&+0.1\,\mathbf C\mathbf M_A\boldsymbol\eta
+\mathbf C\mathbf M_D\dot{\boldsymbol\eta},\\
\boldsymbol\alpha={}&\mathbf J_{eff}^{-1}\mathbf b.
\end{aligned}
$$

源码中 \(\mathbf L_c\) 以按位取反配合减法的方式建立，并且 SADA、刚度项、阻尼项按固定时序进入 RHS；这对非有限值和逐位重建尤为重要。[S9]

### 8.3 模态加速度

模态方程可写成

$$
\begin{aligned}
\ddot{\boldsymbol\eta}={}&-
\mathbf C^T\boldsymbol\alpha
-\mathbf C^T\mathbf M_4\mathbf u_{rigid}
-\mathbf C_S^T\mathbf P_S
-0.1\mathbf M_A\boldsymbol\eta
-\mathbf M_D\dot{\boldsymbol\eta},
\end{aligned}
$$

其中中间的 \(\mathbf M_4\mathbf u_{rigid}\) 和 SADA 项为配置可选项；\(\mathbf P_S\) 来自 \([0,a_{S,0},a_{S,1}]^T\) 经 SADA 预映射。模态速度子状态直接满足

$$\frac{d\boldsymbol\eta}{dt}=\dot{\boldsymbol\eta}.$$

> **知识点。** 柔性模型将“结构形变”简化为有限个模态坐标。刚体角加速度会激发模态，模态形变又通过 \(\mathbf C\) 反馈到本体的有效惯量与角动量，形成双向耦合。[S9]

---

## 9. RK4 数值积分与浮点轨迹

对一般 ODE \(\dot y=f(t,y)\)，经典四阶 Runge–Kutta 为

$$
\begin{aligned}
k_1&=f(t_n,y_n),\\
k_2&=f(t_n+h/2,y_n+hk_1/2),\\
k_3&=f(t_n+h/2,y_n+hk_2/2),\\
k_4&=f(t_n+h,y_n+hk_3),\\
y_{n+1}&=y_n+\frac h6(k_1+2k_2+2k_3+k_4).
\end{aligned}
$$

恢复端内部保存的是 \(hk_i\)，最终以 `k4 + 2*k3 + 2*k2 + k1` 的顺序累加后乘 \(1/6\)，再加回状态。每步积分完成后，四元数逐分量除以其范数而归一化。[S10]

> **为什么不能随意改写公式？** 在实数代数中，\(a+b+c\) 与 \((a+c)+b\) 相等；在 IEEE-754 浮点中，舍入、溢出和 qNaN 负号传播可能不同。本工程追求的是原 ELF 的执行轨迹，而不是仅追求近似误差小。

---

## 10. 执行机构与设备命令

### 10.1 飞轮

给定第 \(i\) 个轮的扭矩命令 \(\tau_i^{cmd}\) 与惯量 \(J_i\)：

$$
\alpha_i^{cand}=\frac{\tau_i^{cmd}}{J_i}+
\begin{cases}
\mathcal N(0,\sigma_{\alpha,i}/J_i),&\text{噪声开关开启且命令非零},\\
0,&\text{否则}.
\end{cases}
$$

再执行扭矩上限和转速方向限制。离散更新为

$$
\tau_i=J_i\alpha_i,\qquad
\omega_i^{+}=\omega_i+\alpha_i\Delta t,\qquad
H_i^{+}=J_i\omega_i^{+}.
$$

四个轮的扭矩由 3×4 配置矩阵映射为轮组力矩，轮组角动量按 \(\mathbf H_w^+=\mathbf H_w+\boldsymbol\tau_w\Delta t\) 更新。[S6]

### 10.2 磁力矩器（MTQ）

六个通道分别饱和：

$$m_i^{act}=\operatorname{sat}(m_i^{cmd},[-m_{i,max},m_{i,max}]).$$

随后通过 3×6 映射矩阵得到组合磁矩 \(\mathbf m\)，动力学层再计算 \(\mathbf m\times\mathbf B\)。[S6] [S11]

### 10.3 推力器

若 `work_status != 1`，输出力和力矩清零；若开启：

$$
\mathbf F_{out}=k_F\mathbf F_{in},\qquad
\boldsymbol\tau_{out}=\mathbf l\times\mathbf F_{out}.
$$

其中 \(\mathbf l\) 是固定力臂。[S6]

### 10.4 SADA（太阳翼驱动）

SADA 对两个轴分别将命令角钳制到对称限位，然后限制一次速度增量：

$$
\Delta\omega=
\operatorname{sat}(\theta_{target}-\omega,
[-a_{max}\Delta t,a_{max}\Delta t]),
$$

$$
\omega^+=\omega+\Delta\omega,\qquad
a=\frac{\Delta\omega}{\Delta t},\qquad
\theta^+=\theta+\omega^+\Delta t.
$$

SADA 的命令角与角加速度还会分别通过配置矩阵注入总角动量、刚体 RHS 和模态 RHS。[S6] [S8] [S9]

### 10.5 固定单步设备顺序

设备控制严格依次为：

```text
惯量更新 → 飞轮加速度设定 → 飞轮更新/群组映射 → MTQ 饱和/映射
         → 推力器开关/更新 → SADA 设定/推进
```

H18–H20 的多执行机构场景对这一序列以及最终 RWheel、MTQ、Thruster 对象快照进行了原 ELF 位级对照。[S6]

---

## 11. 传感器、随机噪声与有效性判定

### 11.1 高斯随机数

实现使用拒绝采样的极坐标形式。先采样 \(x,y\in(-1,1)\)，令 \(s=x^2+y^2\)，仅接受 \(0<s\le1\)，返回

$$n=y\sigma\sqrt{\frac{-2\ln s}{s}},\qquad x_{noise}=\mu+n.$$

该过程依赖 C `rand()` 的消耗顺序；因此固定 `srand()` 种子、设备更新顺序和噪声开关都是可比较行为的一部分。[S5]

### 11.2 陀螺与磁强计

对输入本体系向量 \(\mathbf u\)，先经安装投影矩阵：

$$\mathbf z=\mathbf P\mathbf u.$$

若噪声开关开启，则每个分量加独立高斯样本：

$$z_i\leftarrow z_i+\mathcal N(\mu_i,\sigma_i).$$

陀螺输入是本体系角速度，磁强计输入是本体系地磁场。[S5]

### 11.3 数字太阳敏感器（DSS）

DSS 先投影太阳本体系向量：

$$\mathbf u=\mathbf P_{DSS}\mathbf s_B.$$

仅当 \(u_z>0\) 时，将其标准化到 \(u_z=1\)：

$$
\tilde u_x=\frac{u_x}{u_z},\quad
\tilde u_y=\frac{u_y}{u_z},\quad
\tilde u_z=1,
$$

$$
\alpha_x=\operatorname{atan2}(\tilde u_x,1),\qquad
\alpha_y=\operatorname{atan2}(\tilde u_y,1).
$$

噪声开启时，角度相加高斯噪声，再用 \(\tan\alpha_x,\tan\alpha_y\) 形成比值；关闭时比值保留 \(\tilde u_x,\tilde u_y\)。遥测阶段将角度乘 \(180/\pi\) 输出为度。[S5] [S14]

### 11.4 星敏感器（STS）

STS 在更新周期满足时，以真值姿态与安装误差四元数相乘，得到测量四元数。输出阶段若噪声开启，会先对 312 欧拉误差采样、转为四元数，再与测量四元数按原 ABI 方向组合。[S5]

有效性包含太阳排除角与地球遮挡几何。对视轴 \(\mathbf b\)，位置 \(\mathbf r\)，地球半角为

$$\alpha_E=\arcsin\frac{R_e}{\lVert\mathbf r\rVert}.$$

程序用 \(-\mathbf r\) 与视轴夹角检查地球是否离开视场边界。[S5]

### 11.5 地影与 DSS 有效性

地影判定为

$$
\alpha_{limb}=\arccos\frac{R_e}{\lVert\mathbf r\rVert},\qquad
\alpha_{sun,r}=\angle(\mathbf s_{GCI},\mathbf r_{GCI}),
$$

$$\text{shadow}=\left[\alpha_{sun,r}>\alpha_{limb}+\frac\pi2\right].$$

DSS 有效性不是直接以太阳 GCI 向量判断，而是以**测量向量与本地参考轴** \((0,0,1)\) 的夹角：

$$
\theta_{DSS}=\angle(\mathbf z_{DSS},[0,0,1]^T).
$$

源码逻辑为：若 `threshold < θ_DSS`，有效标志为 1；否则由地影判定决定。H11 的原 ELF 调用点跟踪确认参考向量确为 \((0,0,1)\)，这一点曾用于修正早期错误泛化。[S5]

### 11.6 GPS

GPS 路径在当前恢复中主要复制 GCI 位置、速度和六个时间分量；初始化标志从 1 转为 0。它是确定性接口更新，而不是完整导航滤波器预测/校正循环。[S5]

---

## 12. 主调度、遥测与 IPC 输出

### 12.1 单步总调度

高层桥接的固定顺序为：

1. 更新设备控制；
2. 把轮组角动量和反向力矩注入核心项；
3. 使用时间、环境和 RK4 推进 33 维状态；
4. 更新传感器测量；
5. 构造遥测源；
6. 填充主遥测帧；
7. 可选地序列化到 IPC payload。

该顺序把执行机构的本步更新与动力学、传感器和输出绑定在同一状态机中。[S13]

### 12.2 主遥测帧

主遥测帧大小为 **544 B**。主要字段来自：

| 输出类别 | 主要内容 |
|---|---|
| STS | 3 路四元数与 3 个有效标志 |
| Gyro / MagMeter | 各 2 路三分量测量 |
| DSS | 两个角对与两个有效标志 |
| RWheel / SADA | 4 个轮速；两轴角度与角速度 |
| GPS | GCI 位置、速度、时间整型标志 |
| 传播信息 | 时间前缀、ECEF 位置速度、8 个轨道要素 |

封装采用 `memcpy`/定长偏移而非结构体直接强转；SADA 四个字段还具有特定写入次序。[S14]

### 12.3 IPC

IPC payload 比较块为 **3000 B**。IPC 模块提供整数/浮点的端序读写、共享内存打开/关闭、字段索引写入和主遥测到 IPC 的映射。其本质是**二进制协议序列化**：不仅数值，还包括 float 降精度、字节序、字段位置和某些 `u8` 标志都会影响输出。[S15]

> **知识点。** “主遥测一致”不自动意味着“IPC 一致”。主遥测与 IPC 之间仍有类型转换、偏移映射和字节序等额外行为，所以两者被作为独立观察块进行差分。

---

## 13. 输入输出覆盖、验证证据与当前完成度

### 13.1 已纳入默认门禁的场景

当前工程包含 H0–H20 共 **21 个受控场景**。每个场景分别执行 100 步与 1000 步，因此有 42 条长时域序列，累计每侧 23,100 次 `dyn_main` 步进。场景覆盖日期边界、闰日、近圆/高偏心轨道、多个四元数、零/非零完整柔性、DSS 噪声开关、零/持续/脉冲/固定种子 LCG 设备命令及执行机构初态等。[S16]

| 输出观察块 | 覆盖场景数 | 每步比较大小 | 说明 |
|---|---:|---:|---|
| CoreDynamic caller-state 前缀 | 21 / 21 | 264 B | 所有长时域场景 |
| main telemetry | 21 / 21 | 544 B | 所有长时域场景 |
| IPC payload | 21 / 21 | 3000 B | 所有长时域场景 |
| internal global `y[33]` | 11 / 21 | 264 B | 仅有建立该块原 ELF gold 的场景 |
| 最终执行机构对象 | H18–H20 | 多块 | RWheel、MTQ、Thruster 与四个推力器向量 |

截至 H20 的严格空构建回归为：`make clean && make selftest`，**318 个测试可执行项**、**636 条编译/执行命令**、**309 条 PASS**、错误模式计数 0。原 ELF 权限保持 `0400`，SHA-256 固定为：

```text
312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403
```

上述统计与场景矩阵、双采集 hash、比较器和全量日志均保存在工程分析目录中。[S16] [S17]

### 13.2 双采集与差分流程

每一个进入默认门禁的新场景遵循同一取证闭环：

```text
反汇编/源码审计
  → 仅对调试执行副本运行原 ELF 探针
  → 首次采集 gold
  → 备份并第二次采集
  → 每个 gold 文件 cmp/sha256 一致
  → 恢复端同初始化与同命令逐步比较
  → 100 步 PASS + 1000 步 PASS
  → make clean && make selftest
  → 覆盖文档与 Git 提交
```

原始 `input/DynamicPackage.elf` 被保持为只读金标；动态调试仅在副本上进行。[S16] [S17]

### 13.3 如何理解“覆盖”

已覆盖的是**离散、明确、可重放的输入模板**，不是浮点连续空间的所有组合。正确的结论是：

> 在列明的场景、当前 Linux x86-64、当前运行时、当前严格编译选项、固定随机种子和指定观察块中，恢复端与原 ELF 的输出逐步逐字节一致。

不正确的结论是：“已经数学证明任意输入、任意命令长度、任意平台都 100% 相同。”

---

## 14. 位级一致性的工程要点

### 14.1 计算顺序不是实现细节

对一般数值软件，常见重构会把表达式改写为更短或更快的形式；对于位级对齐，这可能破坏行为。例如：

```text
(a + b) + c    与    a + (b + c)
a * (b * c)    与    (a * b) * c
A(Bx)          与    (AB)x
```

在有限精度下可能产生不同舍入位。恢复端因此固定 `blas_gemv`、`blas_gemm`、J3/J4、多项式、RK4 权重和坐标变换的运算顺序。[S1] [S3] [S10]

### 14.2 qNaN 符号和 SSE 左操作数

IEEE-754 qNaN 的有效载荷和符号位可能随指令、寄存器源操作数和表达式改写而变化。工程中部分辅助函数显式保证“左操作数”进入相应乘加，以对齐原 ELF 的 SSE 行为；已观察到的非有限轨道/环境路径也仅在有原 ELF 证据时实施最小的负 qNaN 位型修正。[S1] [S3] [S4]

### 14.3 随机数与设备更新顺序

传感器和轮噪声使用 `rand()`。一次多余的随机调用、不同的设备遍历顺序、不同的噪声开关路径，都可能使之后所有随机样本错位。因此固定种子测试必须同时固定：

1. 初始 `srand()`；
2. 每步命令发生器；
3. 设备更新顺序；
4. 传感器更新顺序；
5. 运行平台与 C 运行时。

H12、H19、H20 正是这类约束的代表性验证。[S5] [S16]

---

## 15. 已知边界、非目标与继续扩展方法

### 15.1 已知边界

| 项目 | 当前边界 |
|---|---|
| 输入空间 | 仅 21 个受控模板，不穷尽所有 33 维连续状态、传感器参数和命令序列 |
| internal global `y` | 仅 11 个场景建立并比较该 gold；其他 10 个不能声称已验证 |
| 平台 | 当前 Linux x86-64、当前运行时库、当前 GCC/浮点约束；未证明跨平台一致 |
| 并发/共享内存竞争 | 重点验证确定性离线调用路径，不等同于所有并发时序 |
| 未观察对象 | 未纳入 gold 的内部对象不应被声称逐字节验证 |
| 原始源码 | 目标是行为重建；符号名称、结构字段名和早期骨架中仍可能含推断性命名 |

### 15.2 明确不应做的事

1. 不应修改只读原 ELF 金标；
2. 不应把有限 PASS 宣称为任意输入 100% 证明；
3. 不应为了“数值更稳定”擅自改变运算顺序、加入钳位或替换数学库路径；
4. 不应把离线仿真代码接入飞行、控制、实体执行机构或安全关键系统；
5. 不应把动态调试副本中的实验改动误写入正式恢复实现。

### 15.3 推荐的继续覆盖顺序

后续场景应优先选择与 H0–H20 实质独立的输入交叉，并始终使用原 ELF gold 裁决：

| 优先级 | 候选维度 | 原因 |
|---:|---|---|
| 1 | 多个新的固定 LCG 种子 × H20 柔性/传感器模板 | 扩大长命令序列与随机消耗覆盖 |
| 2 | 现有仅三块场景补采 global `y` | 缩小内部状态观察缺口 |
| 3 | 轮噪声、陀螺/磁强计噪声的交叉开关 | 覆盖不同随机调用拓扑 |
| 4 | 边界轨道要素：近圆/近赤道/高偏心 | 覆盖轨道根数退化分支与重力敏感区 |
| 5 | SADA 限位、速度饱和与推力器开关序列 | 覆盖执行机构离散边界 |
| 6 | 明确的 NaN/Inf 诊断输入 | 仅在隔离探针中研究浮点 ABI，不能无证据泛化 |

---

## 16. 源码索引与参考

下面的引用是本工程内的一手实现/证据文件，而非把一般教材公式强行当作行为证据。公式的具体常量、分支和运算顺序以这些文件为准。

[S1]: ../../src/dynamic_math.c "线性代数、旋转、四元数、3×3 求逆、随机数与浮点顺序"
[S2]: ../../src/dynamic_time.c "JD、GMST、岁差、GCI/ECEF 与时间推进"
[S3]: ../../src/dynamic_orbit.c "开普勒方程、轨道要素、二体/J2/J3/J4 与外力"
[S4]: ../../src/dynamic_environment.c "太阳、IGRF 球谐地磁、局地坐标、气动力矩辅助"
[S5]: ../../src/dynamic_sensors.c "STS、Gyro、DSS、MagMeter、GPS、地影与噪声"
[S6]: ../../src/dynamic_devices.c "飞轮、MTQ、推力器、SADA 与设备控制顺序"
[S7]: ../../src/dynamic_torque.c "飞轮反作用、磁力矩、重力梯度力矩"
[S8]: ../../src/dynamic_dynamics.c "33 维微分方程与总角动量装配"
[S9]: ../../src/dynamic_flex.c "有效惯量、刚柔耦合 RHS、模态动力学"
[S10]: ../../src/dynamic_rk4.c "四阶段 RK4 与四元数单位化"
[S11]: ../../src/dynamic_torque.c "外力矩实现"
[S12]: ../../src/dynamic_scheduler.c "基础调度顺序"
[S13]: ../../src/dynamic_main_bridge.c "端到端单步桥接"
[S14]: ../../src/dynamic_telemetry.c "544 B 主遥测封包和字段来源"
[S15]: ../../src/dynamic_ipc_telemetry.c "IPC 端序、共享内存与协议打包"
[S16]: ../coverage_inventory/long_horizon_matrix_h0_h1.md "H0–H20 覆盖矩阵、gold 双采集与有限验证边界"
[S17]: ../coverage_inventory/full_clean_selftest_with_h0_to_h20_long_horizon_gates.log "H20 后严格空构建回归日志"
[S18]: ../time_orbit/h0_thousand_nan_sign_investigation.md "H0 非有限值与 H11–H20 连续取证日志"

---

**最终声明：**本文件描述的是一个经反汇编、受控动态探针和有限逐步 bitwise 差分支撑的离线行为重建工程。其价值在于可审计、可复现、可继续扩展；其边界同样必须保留：不等同于原始源码公开、不等同于全输入数学证明、更不适合任何实际飞行或安全关键用途。
