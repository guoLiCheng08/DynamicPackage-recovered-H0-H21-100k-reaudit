from pathlib import Path
import sys

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import generate_model_specific_docx as base
from docx import Document
from docx.shared import Cm, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

ROOT = Path('/home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work')
OUT = ROOT / 'analysis' / 'documentation' / 'DynamicPackage_单星离线仿真_详细设计说明书_最终版_H0-H21_10万步验证.docx'
CJK = base.CJK
CJK_SERIF = base.CJK_SERIF
BLUE = base.BLUE
TEAL = base.TEAL
DARK = base.DARK


def set_font(run, size=10, bold=False, color=None, name=CJK):
    run.font.name = name
    run._element.rPr.rFonts.set(qn('w:eastAsia'), name)
    run.font.size = Pt(size)
    run.bold = bold
    if color:
        run.font.color.rgb = RGBColor.from_string(color)


def add_text(doc, text, size=10.3, before=0, after=5, bold_prefix=None):
    p = doc.add_paragraph()
    p.paragraph_format.space_before = Pt(before)
    p.paragraph_format.space_after = Pt(after)
    p.paragraph_format.line_spacing = 1.22
    if bold_prefix and text.startswith(bold_prefix):
        r = p.add_run(bold_prefix)
        set_font(r, size=size, bold=True, color=DARK)
        r = p.add_run(text[len(bold_prefix):])
        set_font(r, size=size)
    else:
        r = p.add_run(text)
        set_font(r, size=size)
    return p


def add_code(doc, title, lines):
    table = doc.add_table(rows=1, cols=1)
    table.alignment = WD_ALIGN_PARAGRAPH.CENTER
    cell = table.cell(0, 0)
    base.set_cell_shading(cell, 'F3F6F8')
    base.set_cell_margins(cell, top=110, start=140, bottom=110, end=140)
    p = cell.paragraphs[0]
    p.paragraph_format.space_after = Pt(4)
    r = p.add_run(title)
    set_font(r, size=9.2, bold=True, color=BLUE)
    for line in lines:
        p = cell.add_paragraph()
        p.paragraph_format.space_after = Pt(0)
        p.paragraph_format.line_spacing = 1.05
        r = p.add_run(line)
        set_font(r, size=8.4, name='DejaVu Sans Mono')
    doc.add_paragraph().paragraph_format.space_after = Pt(3)


def add_module_contract(doc, no, title, purpose, inputs, state, outputs, entry, evidence):
    base.add_heading(doc, f'{no}. {title}', 1)
    add_text(doc, purpose)
    base.add_table(doc, ['设计项', '当前模型定义'], [
        ('职责', purpose),
        ('输入', inputs),
        ('持久状态/配置', state),
        ('输出/副作用', outputs),
        ('实现入口', entry),
        ('一手证据', evidence),
    ], widths=[3.0, 14.2], font_size=8.35)


def add_cover(doc):
    p = doc.add_paragraph()
    p.paragraph_format.space_before = Pt(60)
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('DynamicPackage')
    set_font(r, name=CJK_SERIF, size=28, bold=True, color=BLUE)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('单星离线仿真模型详细设计说明书')
    set_font(r, name=CJK_SERIF, size=22, bold=True, color=TEAL)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('H0–H21 行为等价重建基线｜模块输入输出、算法逻辑、状态布局与实现顺序')
    set_font(r, size=10.5, color='666666')
    doc.add_paragraph()
    base.add_callout(doc, '文件定位', '本文件是当前恢复工程的详细设计说明书（DDS）。它按模型内的独立计算单元分章描述：每章都给出职责、输入、持久状态、输出、公式、离散步骤、伪代码、数据布局和证据入口。它的目标是让读者能够在当前平台上据此复现同等的软件仿真结构；位级等价仍必须以原 ELF gold 验证裁决。', fill='E2F0D9')
    base.add_table(doc, ['设计基线', '固定事实'], [
        ('单星状态', '33 个 double：4 四元数 + 3 本体系角速度 + 3 GCI 位置 + 3 GCI 速度 + 10 柔性位移 + 10 柔性速度'),
        ('外部设备命令 ABI', '0x78 B：轮扭矩[4]、MTQ[6]、SADA 标志/双轴命令、推力器使能、惯量更新标志'),
        ('单步输出', 'CoreDynamic caller-state 前 264 B、main telemetry 544 B、IPC payload 3000 B；部分场景比较 global y[33]'),
        ('验证状态', 'H0–H21 共 22 场景；H0–H20 各 100/1000 步，H21 另扩展至 10,000/100,000 步；严格 selftest 为 322 个可执行项、313 条 PASS'),
        ('原 ELF', 'input/DynamicPackage.elf；mode 0400；SHA-256 312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403'),
    ], widths=[3.3, 13.9], font_size=8.8)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(26)
    r = p.add_run('仅限离线研究与仿真。不得用于飞行、控制、实体执行机构或安全关键用途。')
    set_font(r, size=10, bold=True, color='9C0006')
    doc.add_page_break()


def add_reader_contract(doc):
    base.add_heading(doc, '阅读与实现约定', 1)
    add_text(doc, '本设计将“单机”解释为当前单星仿真内的一个独立计算单元或设备单元。每一章按同一模板组织：输入 → 持久状态/参数 → 步骤 → 输出 → 边界 → 验证。文中出现的函数名、结构名、偏移和尺寸均来自当前恢复工程；带 guessed/reserved 的字段名仅是恢复端阅读标签，不应被当作原始源代码标识符。')
    base.add_table(doc, ['记号/约定', '含义'], [
        ('y[k]', '第 k 步开始时的 33 维连续状态；积分结束得到 y[k+1]。'),
        ('xdev[k]', '飞轮、MTQ、推力器、SADA 等离散设备对象状态。'),
        ('u[k]', '调用者提供的 0x78 B 设备命令帧；不是本模型内部产生的控制律。'),
        ('e[k]', '时间、太阳、地磁、坐标变换、外力/力矩等环境量。'),
        ('z[k]', '传感器对象更新得到的测量、有效标志和随机噪声结果。'),
        ('o[k]', 'main telemetry 544 B 与 IPC payload 3000 B 等外部字节输出。'),
        ('Θ', '静态配置：质量、惯量、安装矩阵、耦合矩阵、传感器阈值、饱和限制和常量表。'),
        ('位级顺序', '对当前工程，操作顺序、双精度中间值和 NaN 传播属于可观察行为；不可自由代数化简。'),
    ], widths=[3.1, 14.1], font_size=8.5)
    base.add_callout(doc, '实现边界', '本文件覆盖当前已恢复且已审计的算法逻辑与对象接口。大规模常量表（例如 14×14 地磁系数、默认卫星刚柔矩阵）应从工程内对应的 C 数组逐值复制；不要用近似公式或其他库替换。', fill='FFF2CC')


def add_architecture(doc):
    add_module_contract(doc, '1', '单星模型总装、初始化与单步调度',
                        '单步桥接将一个外部命令帧转为设备状态变化、33 维核心传播、传感器更新、主遥测和可选 IPC。它是全部独立计算单元的固定装配顺序。',
                        'DpState state；DpDeviceControlCommand command；DpDynMainCoreTerms；DpDynMainIpcControl；step_time；可选共享 IPC 帧。',
                        'DpDynMainRecoveredContext：核心模型指针、设备集合、控制上下文、积分时间、设备步长、高斯采样器、上一步本体系地磁缓存。',
                        '更新后的 state、DeviceMeasure、RWheel/MTQ/Thruster/SADA、544 B main telemetry、可选 3000 B IPC。',
                        'dp_dyn_main_recovered_step；DynamicInit；dyn_main/dyn_main_array ABI 适配器。',
                        'src/dynamic_main_bridge.h/.c；dynamic_recovered.h；analysis/elf_c_diff/dyn_main_global_abi_checkpoint.md [S12]')
    diagram = base.create_single_satellite_diagram()
    p = doc.add_paragraph(); p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.add_run().add_picture(str(diagram), width=Cm(16.2))
    base.add_caption(doc, '图 1：当前模型的单星离散数据流。该闭环是软件状态反馈，不是自主控制器。')
    base.add_formula_image(doc, 'dds_single_step', r'x_{dev,k+1}=D(x_{dev,k},u_k,\Delta t,\Theta)\;\to\;y_{k+1}=\Phi_{RK4}(y_k,t_k,\Delta t,x_{dev,k+1},e_k,\Theta)\;\to\;z_{k+1}=H(y_{k+1},x_{dev,k+1},e_{k+1},\xi_k,\Theta)\;\to\;o_{k+1}=G(y_{k+1},x_{dev,k+1},z_{k+1},\Theta)', '式 1：单星离散仿真的统一模型；D、Φ、H、G 分别是设备、传播、传感器、封包。')
    add_code(doc, '算法 1：单步调度伪代码（保持当前顺序）', [
        'require context, state, command, core_terms, out_telemetry',
        'device_dt = (context.device_control_step_time > 0) ? device_control_step_time : step_time',
        'UpdateDeviceControl(command, device_dt)                  // 惯量→轮→MTQ→推力器→SADA',
        'terms.Hwheel  = WheelGroup.angular_momentum',
        'terms.minus_T = -WheelGroup.torque + MTQ_Group.moment × prior_magnetic_body + Thruster.torque',
        'CoreDefaultRK4WithTermsAndEnvironment(state, integration_time, step_time, terms)',
        'UpdateDeviceMeasure(state, environment_after_step, step_time, gaussian_sampler)',
        'BuildTelemetrySource(devices, state, environment_after_step); UpdateMainOut(out_telemetry)',
        'if optional_ipc_frame != NULL: sendDynTele(optional_ipc_frame, out_telemetry, five_drc_bytes)',
    ])
    base.add_heading(doc, '1.1 初始化输入与状态布局', 2)
    base.add_table(doc, ['对象', '布局/字段', '初始化语义'], [
        ('DpInitialConditions', '+0x00 step_time；+0x38 六时间 double；+0x68 六轨道要素；+0x98 9 惯量；+0xe0 质量', 'DynamicInit 写入步长、TimeInit、Elements2PosVel_M、惯量和质量。+0x08 含义尚未完全确定。'),
        ('DpState', 'y[0..3] q；y[4..6] ω；y[7..9] rGCI；y[10..12] vGCI；y[13..32] flexible_state[20]', '所有连续状态以 double 保存；柔性后 20 项按 10 位移 + 10 速度解释。'),
        ('DpDeviceControlCommand', '0x08 轮扭矩[4]；0x28 MTQ[6]；0x58 SADA flag；0x60 双轴；0x70 推力器；0x74 惯量更新', '一次调用对应一帧外部命令；双精度字段按 ABI 对齐。'),
        ('DpCoreDynInput', '0x00/0x18 轮组量；0x30 MTQ；0x48/0x60 推力器；0x78 SADA', '由 UpdateCoreDynInput/设备对象组装；总大小 0x90。'),
    ], widths=[3.4, 6.8, 7.0], font_size=8.2)


def add_math_module(doc):
    add_module_contract(doc, '2', '数学内核：向量、矩阵、旋转、四元数与随机数',
                        '为所有物理和接口模块提供行主序描述符上的基础计算。此模块的逐项累加、原地覆盖与边界返回值是恢复目标的一部分。',
                        'DpVector/DpMatrix 描述符、double 数组、维数、比例 α/β、四元数、随机噪声参数。',
                        '调用方分配的矩阵/向量 backing；没有隐式 BLAS 上下文。',
                        '向量/矩阵结果、四元数/姿态矩阵、3×3 逆、角度、随机高斯样本。',
                        'dynamic_math.c：vector3_cross、blas_gemv、blas_gemm、quat_xi、quat_psi、matrix_inverse_3x3、vector2angle。',
                        'src/dynamic_math.c；analysis/math/*.asm；math ABI selftest [S1]')
    base.add_formula_image(doc, 'dds_vector', r'\mathbf a\times\mathbf b=(a_yb_z-a_zb_y,\;a_zb_x-a_xb_z,\;a_xb_y-a_yb_x)^T,\quad \theta(\mathbf a,\mathbf b)=\arccos\frac{\mathbf a^T\mathbf b}{\|\mathbf a\|\|\mathbf b\|}', '式 2：叉积和 vector2angle 的数学对应；实现对维数/零范数有固定边界行为。')
    base.add_formula_image(doc, 'dds_quat', r'\dot q=\frac{1}{2}\Xi(q)\boldsymbol\omega,\quad \Xi(q)=[(q_0,-q_3,q_2);(q_3,q_0,-q_1);(-q_2,q_1,q_0);(-q_1,-q_2,-q_3)]', '式 3：标量在前的四元数运动学和当前 quat_xi 行列值。')
    add_code(doc, '算法 2：矩阵乘向量与四元数归一化伪代码', [
        'for i in 0 .. rows(A)-1:',
        '    sum = 0; for j in 0 .. cols(A)-1: sum += A[i,j] * x[j]     // 固定 j 递增顺序',
        '    y[i] = alpha * sum + beta * y[i]',
        'norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3)',
        'for i in 0..3: q[i] = q[i] / norm                              // 不加入额外零保护',
    ])
    base.add_table(doc, ['函数族', '输入', '输出', '不可省略的边界'], [
        ('vector_unit / vector2angle', '向量描述符', '单位向量/夹角', '范数不大于 0 或维数不合时保留原实现返回逻辑；不得擅自抛异常。'),
        ('blas_gemv / blas_gemm', '行主序 A、x/B、α、β、旧输出', '线性组合', '即使 β=0，旧输出中 NaN 的传播和浮点操作顺序仍可能可见。'),
        ('quat_*', 'q、ω、姿态矩阵', 'Ψ、Ξ、姿态变换、四元数乘法', 'ABI 适配器参数顺序与内部数学顺序不同，STS 路径必须沿当前调用点。'),
        ('matrix_inverse_3x3', '3×3 double', '伴随/行列式逆', '不添加“防奇异”钳位；0/NaN 按 IEEE-754 行为传播。'),
        ('ran_gaussian*', 'σ 或 μ,σ；C rand()', '高斯样本', '采样拒绝循环消耗 rand() 的次数必须保持一致。'),
    ], widths=[3.3, 3.5, 3.1, 7.3], font_size=8.0)


def add_time_coordinate_module(doc):
    add_module_contract(doc, '3', '时间与坐标单元：日历、JD、GMST、岁差、GCI/ECEF',
                        '把日历时间推进为天文时间，再建立惯性、地固和局地坐标间的旋转。它为环境、轨道、GPS 和遥测提供共享时间/坐标基础。',
                        '年/月/日/时/分/秒、step、GCI/ECEF 位置或速度。',
                        'Time 全局/上下文中的小数秒残量与日历分量。',
                        'JD、儒略世纪 T、GMST、岁差矩阵、GCI/ECEF/LocalGeo 结果。',
                        'Calc_JD、Calc_theta_GMST、TimeAdd、GCI2ECEF、ECEF2GCI、GCI2LocalGeo。',
                        'src/dynamic_time.c；H15 跨年、H12/H16 闰日 gold [S2]')
    base.add_formula_image(doc, 'dds_jd_gmst', r'JD=367y-\lfloor1.75(y+\lfloor(m+9)/12\rfloor)\rfloor+\lfloor275m/9\rfloor+d+1721013.5+\frac{h+(min+s/60)/60}{24},\quad T=\frac{JD-2451545}{36525}', '式 4：当前 Calc_JD 与儒略世纪定义。')
    base.add_formula_image(doc, 'dds_gci_ecef', r'\mathbf r_{ECEF}=R_Z(\theta_{GMST})\mathbf P(T)\mathbf r_{GCI},\qquad \mathbf r_{GCI}=[R_Z(\theta_{GMST})\mathbf P(T)]^T\mathbf r_{ECEF}', '式 5：当前 GCI/ECEF 双向变换。')
    add_code(doc, '算法 3：TimeAdd 与坐标转换伪代码', [
        'fractional_seconds += step',
        'while fractional_seconds >= 1.0: calendar.second += 1; fractional_seconds -= 1.0',
        'normalize calendar through mktime only at integral-second boundary',
        'JD = Calc_JD(calendar); T = (JD - 2451545.0) / 36525.0',
        'P = precession_matrix(T); R = Rz(Calc_theta_GMST(calendar)) * P',
        'r_ecef = R * r_gci; r_gci = transpose(R) * r_ecef',
    ])


def add_orbit_environment_modules(doc):
    add_module_contract(doc, '4', '轨道单元：开普勒要素、状态互转、二体与带谐传播',
                        '在当前模型中把六轨道要素初始化为 GCI 位置速度，并在每个导数调用中计算中心引力、J2/J3/J4 和外力质量比加速度。',
                        'a,e,i,Ω,ω、平均/真近点角；rGCI、vGCI；外力 F_I_external；SpacecraftMass。',
                        '轨道常量 μ=398600441500000.0、Re=6378137.0、J2/J3/J4；无额外积分状态。',
                        '位置/速度、轨道要素、加速度 a_orbit。',
                        'M2E、Elements2PosVel_M、PosVel2Elements、orbit_dynamic。',
                        'src/dynamic_orbit.c；高偏心 H11/H14/H15 gold [S3]')
    base.add_formula_image(doc, 'dds_kepler', r'E_{n+1}=E_n+\frac{M-(E_n-e\sin E_n)}{1-e\cos E_n},\qquad |\Delta E|<10^{-10}', '式 6：当前 M2E Newton 迭代；初值 M，最多 10,000 次。')
    base.add_formula_image(doc, 'dds_orbit_acc', r'\mathbf a=\mathbf a_0+\mathbf a_{J2}+\mathbf a_{J3}+\mathbf a_{J4}+\frac{\mathbf F_{external}}{m},\qquad \mathbf a_0=-\frac{\mu}{\|\mathbf r\|^3}\mathbf r', '式 7：当前 orbit_dynamic 的加速度总装。')
    add_code(doc, '算法 4：orbit_dynamic 伪代码', [
        'r_ecef = GCI2ECEF(r_gci, current_time)',
        'rho = Re / norm(r_ecef); s = r_ecef.z / norm(r_ecef)',
        'a_ecef = central_term(r_ecef) + J2_term(rho,s) + J3_term(rho,s) + J4_term(rho,s)',
        'a_gci = ECEF2GCI(a_ecef, current_time)',
        'a_gci += F_I_external / SpacecraftMass',
        'return a_gci',
    ])
    base.add_table(doc, ['退化/边界', '当前实现规则'], [
        ('开普勒迭代', '阈值 1e-10；上限 10,000 次；初值为 M。'),
        ('近圆/近赤道要素', 'PosVel2Elements 使用 e≤1e-14、|sin i|≤1e-14 的明确退化分支。'),
        ('带谐项顺序', '中心项 → J2 → J3 → J4；J3/J4 是原 ELF 展开多项式，不替换为通用 Legendre 梯度。'),
        ('浮点顺序', 'r³ 与中间除法的构造顺序保持；高偏心场景会放大末位差异。'),
    ], widths=[4.0, 13.2], font_size=8.2)

    add_module_contract(doc, '5', '环境单元：太阳、IGRF 地磁、局地坐标与扰动力矩辅助',
                        '由时间和轨道状态计算太阳 GCI 向量、惯性地磁、局地基以及供传感器/力矩使用的姿态投影量。',
                        'JD/T、rGCI/rECEF、姿态矩阵、速度与环境配置。',
                        '14×14 IGRF 系数/年变化率；固定黄赤交角；关联 Legendre 缓冲。',
                        'sun_gci、magnetic_gci、magnetic_body、LocalGeo/LVLH 变换、中间外力矩量。',
                        'dp_sun_vector、dp_magnetic_vector、GCI2LocalGeo、gci_to_lvlh 与力矩辅助。',
                        'src/dynamic_environment.c；H12/H13/H20 环境/传感器 gold [S4]')
    base.add_formula_image(doc, 'dds_sun', r'M=2\pi\,\mathrm{frac}(99.9973583T+0.9931267),\quad r_{sun}=149619000000-2499000000\cos M-21000000\cos2M', '式 8：当前太阳相位与日地距离近似；随后固定黄赤交角旋转进入 GCI。')
    add_text(doc, 'IGRF 实现按十进制年线性更新 g(n,m)、h(n,m)，通过关联 Legendre 递推和阶/次双循环累加局地磁场；路径固定为 GCI 位置 → ECEF → 局地地理坐标 → 球谐 → ECEF → GCI → 原比例缩放。所有系数应从 dynamic_environment.c 中的数组逐值复制。[S4]')


def add_dynamics_modules(doc):
    add_module_contract(doc, '6', '刚体力矩、33 维微分方程与刚柔耦合',
                        '本单元计算姿态、角速度、轨道和柔性 33 维状态导数。其核心是总角动量、有效惯量、刚体 RHS 和 10 阶模态 RHS 的固定组合。',
                        'state[33]；基础/耦合惯量矩阵；wheel H；minus torque；SADA 驱动；外力/力矩与环境。',
                        'DpCoreDefaultModel：inertia/inverse、coupling、modal_a/modal_d、SADA 动量/反作用/模态映射；默认 flag=0 刚柔配置。',
                        'derivative[33]：q̇、α、v、a_orbit、η̇、η̈。',
                        'dp_differential_equation_33、dp_flex_dynamics_step、torque helpers。',
                        'src/dynamic_dynamics.c、dynamic_flex.c、dynamic_torque.c、dynamic_core_bridge.h [S7][S8][S9]')
    base.add_formula_image(doc, 'dds_total_h', r'\mathbf H_{total}=\mathbf J\boldsymbol\omega+\mathbf H_w+\mathbf H_{SADA,cmd}+\mathbf C\boldsymbol\eta,\qquad \mathbf J_{eff}=\mathbf J-\mathbf C\mathbf C^T', '式 9：当前总角动量和有效惯量。')
    base.add_formula_image(doc, 'dds_flex_rhs', r'\boldsymbol\alpha=\mathbf J_{eff}^{-1}\mathbf b,\qquad \ddot{\boldsymbol\eta}=-\mathbf C^T\boldsymbol\alpha-\mathbf C^T\mathbf M_4\mathbf u_{rigid}-\mathbf C_S^T\mathbf P_S-0.1\mathbf M_A\boldsymbol\eta-\mathbf M_D\dot{\boldsymbol\eta}', '式 10：当前刚柔耦合的角加速度和模态加速度主结构。')
    base.add_table(doc, ['力矩/项', '当前模型公式或构造', '注入位置'], [
        ('飞轮反作用', 'τRW = −WheelGroup.torque', '主桥接形成 minus_total_torque，进入核心 RHS。'),
        ('磁力矩', 'τMTQ = m × Bprior_body', '主桥接使用 prior_magnetic_body；初步后更新该缓存。'),
        ('推力器', 'Fout = scale·Fin；τthr = lever_arm × Fout', '力进入轨道外力；力矩进入刚体项。'),
        ('重力梯度', '3μ/r³ · [rhatB × (J rhatB)]', 'torque 模块在姿态/轨道状态下计算。'),
        ('柔性', 'Cη、CCT、MAη、MDη̇', '角动量、Jeff、刚体 RHS 和模态 RHS。'),
    ], widths=[3.0, 6.3, 6.9], font_size=8.0)
    add_code(doc, '算法 5：33 维导数与柔性求解伪代码', [
        'split state into q, omega, r_gci, v_gci, eta[10], eta_dot[10]',
        'q_dot = 0.5 * Xi(q) * omega',
        'a_orbit = orbit_dynamic(r_gci) + thruster_force / spacecraft_mass',
        'Htotal = J*omega + Hwheel + Hsada_command + C*eta',
        'Jeff = J - C*transpose(C); b = Lc - cross(omega,Htotal) - Rs*u_sada + 0.1*C*MA*eta + C*MD*eta_dot',
        'alpha = inverse_3x3(Jeff) * b',
        'eta_ddot = -transpose(C)*alpha - optional_terms - 0.1*MA*eta - MD*eta_dot',
        'write derivative[0..32] in the fixed state order',
    ])

    add_module_contract(doc, '7', '数值积分单元：四阶段 RK4 与四元数单位化',
                        '以固定四阶段试探状态和固定加权顺序推进全部 33 个 double；积分后对 q[0..3] 逐分量归一化。',
                        'state[33]、time、step、导数回调与上下文。',
                        'h_k1/h_k2/h_k3/h_k4、临时试探状态和时间。',
                        '原地 state 更新，time 增加 step，归一化四元数。',
                        'dp_rk4_step_33、dp_rk4_integrator_33、RK4_Intergrator。',
                        'src/dynamic_rk4.c；H0 长时域 NaN/RK4 取证 [S10][S18]')
    base.add_formula_image(doc, 'dds_rk4', r'k_1=f(t,y),\;k_2=f(t+h/2,y+hk_1/2),\;k_3=f(t+h/2,y+hk_2/2),\;k_4=f(t+h,y+hk_3),\;y^+=y+\frac{h}{6}(k_1+2k_2+2k_3+k_4)', '式 11：当前 RK4 数学形式；实现内部缓存 h·ki。')
    add_code(doc, '算法 6：RK4 严格顺序', [
        'h_k1 = h * f(t, state)',
        'trial = state + 0.5*h_k1; h_k2 = h * f(t + 0.5*h, trial)',
        'trial = state + 0.5*h_k2; h_k3 = h * f(t + 0.5*h, trial)',
        'trial = state + h_k3;     h_k4 = h * f(t + h, trial)',
        'state += (h_k4 + 2*h_k3 + 2*h_k2 + h_k1) / 6      // 保持该求和顺序',
        'q /= sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3); time += h',
    ])


def add_device_modules(doc):
    add_module_contract(doc, '8', '反作用飞轮单元',
                        '处理四个飞轮的扭矩命令、可选加速度噪声、扭矩/转速限制、速度/角动量离散更新，并由 3×4 安装矩阵输出轮组角动量与力矩。',
                        '四个 torque_command、step_time、每轮惯量/限制/噪声参数、4 根安装轴。',
                        '4×0x78 B RWheel：noise_flag、σ、omega_limit、torque_limit、inertia、omega、acceleration、H、torque、axis；WheelGroup 映射。',
                        '每轮 acceleration/omega/H/torque；3D WheelGroup.angular_momentum 与 WheelGroup.torque；可选速度遥测噪声。',
                        'SetWheelAcc、UpdateWheel、GetWheelMoment、GetWheelSpeed、Wheel_Init。',
                        'src/dynamic_devices.h/.c；H18–H20 设备终态快照 [S6]')
    base.add_formula_image(doc, 'dds_wheel', r'\alpha_i^{cand}=\tau_i^{cmd}/J_i+\chi_i\mathcal{N}(0,\sigma_{\alpha,i}/J_i),\quad \tau_i=J_i\alpha_i,\quad \omega_i^+=\omega_i+\alpha_i\Delta t,\quad H_i^+=J_i\omega_i^+', '式 12：当前反作用飞轮的候选加速度、限幅后派生量和离散更新。')
    add_code(doc, '算法 7：四轮更新', [
        'for i = 0..3:',
        '    alpha = torque_command[i] / wheel[i].inertia',
        '    if noise_flag && torque_command[i] != 0: alpha += ran_gaussian(sigma_accel) / inertia',
        '    apply original torque/omega limit branches to alpha',
        '    wheel[i].acceleration = alpha; wheel[i].torque = inertia*alpha',
        '    wheel[i].omega += alpha*dt; wheel[i].angular_momentum = inertia*omega',
        'group_torque = Mapping3x4 * wheel_torque[4]; group_H += group_torque*dt',
    ])

    add_module_contract(doc, '9', '磁力矩器（MTQ）单元',
                        '对六个通道磁矩命令逐一饱和，并用 3×6 安装矩阵形成三维群组磁矩；动力学层再与本体系地磁叉乘。',
                        'commanded_moment[6]、6 个 moment_limit、3×6 mapping。',
                        '6×0x38 B MTQ：actual_moment、moment_limit、installation_axis；MTQ_Group。',
                        'actual_moment[6]、channel_moment[6]、group_moment[3]。',
                        'SetMomentMTQ、UpdateMagMoment、MagTorque_Init。',
                        'src/dynamic_devices.h/.c；MTQ ABI 比较 [S6]')
    base.add_formula_image(doc, 'dds_mtq', r'm_i^{act}=\mathrm{sat}(m_i^{cmd},[-m_{i,max},m_{i,max}]),\qquad \mathbf m_{group}=\mathbf M_{3\times6}\mathbf m_{act},\qquad \boldsymbol\tau_{MTQ}=\mathbf m_{group}\times\mathbf B', '式 13：当前六通道 MTQ 饱和、映射和磁力矩。')

    add_module_contract(doc, '10', '推力器单元',
                        '以一个 0xb0 B 推力器对象维护工作状态、输入/输出力和力臂力矩。它既提供轨道外力，也向姿态 RHS 提供力矩。',
                        'work_status、force_scale、force_input[3]、lever_arm[3]。',
                        'Thruster：scale、状态、力臂、输入力、输出力、输出力矩。',
                        'force_output 与 torque_output；未工作时两者清零。',
                        'SetThrusterWorkStatus、UpdateThruster、Thruster_Init。',
                        'src/dynamic_devices.h/.c；H18–H20 终态对象 gold [S6]')
    base.add_formula_image(doc, 'dds_thruster', r'work\_status\ne1\Rightarrow\mathbf F_{out}=\mathbf0,\boldsymbol\tau_{out}=\mathbf0;\qquad work\_status=1\Rightarrow\mathbf F_{out}=k_F\mathbf F_{in},\;\boldsymbol\tau_{out}=\mathbf l\times\mathbf F_{out}', '式 14：当前推力器分支、力缩放和力臂力矩。')

    add_module_contract(doc, '11', 'SADA 双轴驱动单元',
                        '对两个太阳翼驱动轴存储命令角、当前角、角速度、角加速度以及角度/加速度限制；每步按限幅速度增量推进，并向刚柔模型提供耦合驱动。',
                        'command_flag、command_angle[2]、step_time、command_limit[2]、acceleration_limit[2]。',
                        '0x68 B SADA：命令、当前角、角速度、角加速度和双轴限制。',
                        'current_angle[2]、angular_velocity[2]、angular_acceleration[2]；SADA 驱动映射输入。',
                        'SetSADA、drive_SADA_once、getSADAangle。',
                        'src/dynamic_devices.h/.c；dynamic_flex.c 的 SADA 映射 [S6][S9]')
    base.add_formula_image(doc, 'dds_sada', r'\theta_{cmd}\leftarrow\mathrm{sat}(\theta_{cmd},[-\theta_{lim},\theta_{lim}]),\quad \Delta\omega=\mathrm{sat}(\theta_{cmd}-\omega,[-a_{lim}\Delta t,a_{lim}\Delta t]),\quad \omega^+=\omega+\Delta\omega,\;a=\Delta\omega/\Delta t,\;\theta^+=\theta+\omega^+\Delta t', '式 15：当前 SADA 的双重限幅和离散推进。')

    add_module_contract(doc, '12', '设备控制组装单元',
                        '把一个 0x78 B 命令帧按固定次序广播到所有执行机构；这是设备单元之间唯一的统一调度入口。',
                        'DpDeviceControlCommand、step_time、设备对象指针、映射矩阵、可选惯量更新回调。',
                        'DpDeviceControlContext 和全部设备全局对象。',
                        '更新后的轮、MTQ、推力器、SADA、群组量和可选惯量状态。',
                        'dp_update_device_control、UpdateDeviceControl。',
                        'src/dynamic_devices.h/.c；UpdateDeviceControl ABI checkpoint [S6]')
    add_code(doc, '算法 8：设备控制顺序（不得改排）', [
        'if command.inertia_update_flag: inertia_update(flag)',
        'SetWheelAcc(command.wheel_torque_command); UpdateWheel(step_time)',
        'SetMomentMTQ(command.mtq_moment_command); UpdateMagMoment()',
        'SetThrusterWorkStatus(command.thruster_work_status); UpdateThruster()',
        'SetSADA(command.sada_command_flag, command.sada_command_angle); drive_SADA_once(step_time)',
    ])


def add_sensor_modules(doc):
    add_module_contract(doc, '13', '随机数与高斯噪声单元',
                        '通过 C rand() 的拒绝采样极坐标算法为轮、陀螺、磁强计、DSS 和 STS 产生噪声。随机数消耗位置属于可观察状态机。',
                        'rand() 产生的两个均匀样本；sigma 或 mean/sigma。',
                        'C 运行时 PRNG 全局状态；不在模型对象中显式保存。',
                        '单个高斯样本。',
                        'ran_gaussian、ran_gaussian2、dp_ran_gaussian_recovered。',
                        'src/dynamic_sensors.c；H12/H19/H20 固定种子差分 [S5][S16]')
    base.add_formula_image(doc, 'dds_gaussian', r'x,y\sim(-1,1),\;s=x^2+y^2\in(0,1],\qquad n=y\sigma\sqrt{\frac{-2\ln s}{s}},\qquad z=\mu+n', '式 16：当前拒绝采样极坐标高斯算法。')
    add_text(doc, '实现要求：每次调用都必须使用与当前 C 运行时相同的 rand() 行为；任何额外或缺少的调用都会使后续所有噪声序列错位。测试因此固定 srand 种子、开关、设备更新顺序和传感器更新顺序。')

    add_module_contract(doc, '14', '陀螺仪单元（2 路）',
                        '将本体系三轴角速度投影至两个陀螺安装坐标，并按每个分量的开关/均值/标准差添加可选高斯噪声。',
                        'body_rate[3]、2 个 installation_matrix/projection_matrix、噪声 flag/mean/sigma。',
                        '每个 0x320 B Gyro：矩阵、噪声参数、measure 描述符和 backing。',
                        'Gyro[0..1].measure[3]。',
                        'Gyro_Init、UpdateGyro、dp_update_gyro。',
                        'src/dynamic_sensors.h/.c [S5]')
    base.add_formula_image(doc, 'dds_gyro', r'\mathbf z=\mathbf P\boldsymbol\omega,\qquad z_i\leftarrow z_i+\chi_{noise}\mathcal N(\mu_i,\sigma_i)', '式 17：当前两路陀螺投影和逐分量可选噪声。')

    add_module_contract(doc, '15', '磁强计单元（2 路）',
                        '将本体系地磁向量投影至两个磁强计安装坐标，并按相同的可选高斯规则形成测量。',
                        'magnetic_body[3]、两个投影矩阵、噪声参数。',
                        '每个 0x138 B MagMeter：噪声参数、矩阵和 measure。',
                        'MagMeter[0..1].measure[3]。',
                        'MagMeter_Init、UpdateMagMeter、dp_update_magmeter。',
                        'src/dynamic_sensors.h/.c；prior_magnetic_body 在主桥接中缓存 [S5][S12]')
    base.add_formula_image(doc, 'dds_mag', r'\mathbf z_{mag,j}=\mathbf P_{mag,j}\mathbf B_{body}+\chi_{j}\mathcal N(\boldsymbol\mu_j,\boldsymbol\sigma_j),\qquad j\in\{0,1\}', '式 18：当前两路磁强计模型。')

    add_module_contract(doc, '16', '数字太阳敏感器 DSS 单元（2 路）',
                        '将本体系太阳向量投影到各 DSS，要求前向 z 分量为正后再归一化为比值/角度；然后以太阳/位置 GCI 几何更新有效标志。',
                        'sun_body、sun_gci、position_gci、两个安装投影矩阵、阈值、噪声参数。',
                        '每个 0x158 B DSS：threshold、噪声、矩阵、valid_flag、measure、angle_x/y、ratio_x/y。',
                        '两组角/比值、valid_flag。',
                        'DSS_Init、UpdateDSS、UpdateSunSensorValidFlag、isEarthShadow。',
                        'src/dynamic_sensors.h/.c；H11 调用点角度取证 [S5][S18]')
    base.add_formula_image(doc, 'dds_dss', r'\mathbf u=\mathbf P_{DSS}\mathbf s_B,\;u_z>0:\quad \tilde u_x=u_x/u_z,\;\tilde u_y=u_y/u_z,\;\alpha_x=\mathrm{atan2}(\tilde u_x,1),\;\alpha_y=\mathrm{atan2}(\tilde u_y,1)', '式 19：当前 DSS 前向半空间、比值归一化和双角输出。')
    base.add_formula_image(doc, 'dds_shadow', r'\alpha_{limb}=\arccos\frac{R_e}{\|\mathbf r\|},\qquad shadow=[\angle(\mathbf s_{GCI},\mathbf r_{GCI})>\alpha_{limb}+\pi/2],\qquad \theta_{DSS}=\angle(\mathbf z_{DSS},(0,0,1)^T)', '式 20：当前地影和 DSS 有效性几何；参考向量固定为 (0,0,1)。')
    add_code(doc, '算法 9：DSS 与有效标志', [
        'u = projection_matrix * sun_body',
        'if u.z > 0: ratio_x=u.x/u.z; ratio_y=u.y/u.z; angle_x=atan2(ratio_x,1); angle_y=atan2(ratio_y,1)',
        'if gaussian_noise_flag: angle_x/y += ran_gaussian2(mean,sigma); ratio_x/y = tan(angle_x/y)',
        'theta = vector2angle(measure, reference=(0,0,1))',
        'if valid_angle_threshold < theta: valid_flag = 1 else valid_flag = isEarthShadow(sun_gci, position_gci)',
    ])

    add_module_contract(doc, '17', '星敏感器 STS 单元（3 路）',
                        '按各自更新周期更新测量四元数；以安装四元数/误差四元数与真值姿态组合，利用太阳排除和地球视场几何确定有效标志；读取时可注入 312 欧拉噪声。',
                        'truth_quat、position_gci、sun_gci、step_time、安装矩阵、周期、阈值、欧拉噪声参数。',
                        '每个 0x170 B STS：period、噪声、阈值、installation_quat、error_quat、accumulated_time、valid、measure_quat。',
                        '3 路测量四元数与 valid_flag。',
                        'STS_Init、Update_STS_Quat、Update_STS_ValidFlag、GetStarTrackerQuat、isStarTrackerValid。',
                        'src/dynamic_sensors.h/.c；STS 四元数 ABI 取证 [S5]')
    base.add_formula_image(doc, 'dds_sts', r'\alpha_E=\arcsin\frac{R_e}{\|\mathbf r\|},\qquad q_{meas}=q_{error}\otimes q_{truth}\quad(\mathrm{ABI\ order})', '式 21：当前 STS 地球半角与测量四元数关系；具体乘法实参方向以源代码 ABI 为准。')

    add_module_contract(doc, '18', 'GPS/Kalman 接口单元',
                        '当前恢复路径并不实现完整导航滤波；它把传播后的 GCI 位置、速度和六个时间分量复制到 0x70 B GPS_Kalman 对象，并处理初始化标志。',
                        'time_values[6]、position_gci、velocity_gci、init_flag。',
                        'GPS_Kalman：+0x28 位置、+0x40 速度、+0x58 六个 int32 时间；全局 init_flag。',
                        'GPS 对象字段与主遥测对应 GPS 区域。',
                        'UpdateGPS、dp_update_gps。',
                        'src/dynamic_sensors.h/.c [S5]')

    add_module_contract(doc, '19', '设备测量总装单元',
                        '把核心传播后的状态和环境尾区按固定顺序送入 STS、Gyro、DSS、MagMeter、GPS，并形成可供 main telemetry 打包的 DeviceMeasure。',
                        'DpState；DpDeviceMeasureEnvironment（sun_body/sun_gci/magnetic_body/time[6]）；step；高斯采样器。',
                        'DpDeviceMeasureRecovered：3 STS、2 Gyro、2 DSS、2 MagMeter、GPS、GPS init flag。',
                        '更新后的所有传感器对象和 GPS。',
                        'dp_update_device_measure_recovered、UpdateDeviceMeasure。',
                        'src/dynamic_sensors.h/.c；设备测量 ABI checkpoint [S5]')
    add_code(doc, '算法 10：传感器总装顺序', [
        'Update_STS_Quat(truth_quat, step_time)',
        'UpdateGyro(body_rate)',
        'UpdateDSS(sun_body); UpdateSunSensorValidFlag(sun_gci, position_gci)',
        'UpdateMagMeter(magnetic_body)',
        'UpdateGPS(time_values, position_gci, velocity_gci)',
        'Update_STS_ValidFlag(attitude, position_gci, sun_gci)',
    ])


def add_output_modules(doc):
    add_module_contract(doc, '20', '主遥测封包单元（UpdateMainOut，544 B）',
                        '将设备测量、设备状态、传播状态、ECEF 状态和轨道要素写入一个固定 0x220 B 二进制帧。该单元只处理确定性字段偏移/类型转换/写入顺序。',
                        'DpTelemetrySourceSnapshot：STS、Gyro、DSS、Mag、轮速、SADA、GPS、传播前缀、ECEF 位置速度、8 要素。',
                        '无持久状态；写入 DpMainTelemetryFrame.raw[544]。',
                        '544 B 原始帧；用于 main gold 和后续 IPC。',
                        'dp_telemetry_source_from_devices、dp_update_main_out_pack、UpdateMainOut。',
                        'src/dynamic_telemetry_layout.h/.c；主输出 ABI 对照 [S14]')
    base.add_table(doc, ['偏移', '字段', '类型/数量', '来源'], [
        ('0x000/0x020/0x040', 'STS 四元数 0/1/2', '3×4 double', 'STS measure_quat'),
        ('0x060', 'STS valid', '3×int32', 'STS valid_flag'),
        ('0x070/0x088', 'Gyro 0/1', '2×3 double', 'Gyro measure'),
        ('0x0a0/0x0c0', 'DSS 值/valid', '4 double；2 int32', 'DSS angle/value、flag'),
        ('0x0c8/0x0e0', 'MagMeter 0/1', '2×3 double', 'MagMeter measure'),
        ('0x0f8/0x118', '轮速/SADA', '4 double；4 double', 'RWheel omega；SADA raw'),
        ('0x138/0x168', 'GPS value/flag', '6 double；6 int32', 'GPS_Kalman'),
        ('0x180/0x1b0/0x1c8/0x1e0', '传播前缀/ECEF位置/ECEF速度/要素', '6 double；3；3；8', '核心状态与坐标/要素转换'),
    ], widths=[3.1, 4.2, 4.0, 5.1], font_size=7.9)
    add_code(doc, '算法 11：main telemetry 封包', [
        'zero or use destination frame according to current caller contract',
        'put STS quaternions and int32 validity at fixed offsets',
        'put Gyro, DSS values/flags, MagMeter, wheel omega and SADA in fixed field order',
        'put GPS double values and int32 flags',
        'put propagated prefix; compute/put ECEF position/velocity; put orbit elements',
        'preserve exact memcpy/put_f64/put_i32 order; DSS angles are converted to degrees here',
    ])

    add_module_contract(doc, '21', 'IPC/共享内存与 sendDynTele 协议单元（3000 B）',
                        '将 544 B main 帧和五个 DRC 字节按原 sendDynTele 的 float/u8 setter 调用序列映射到 0xBF0 B 共享帧。其重点是二进制协议，而不是物理模型。',
                        'DpMainTelemetryFrame、五个 DRC 字节、可选 DpIpcSharedFrame/映射共享内存。',
                        '0xBF0 B 共享对象：前 0x38 B pthread_rwlock，随后 float 区 +0x38，u8 区 +0x998；名字 /sharedvars_example。',
                        '共享帧 payload、setter/getter 返回码、离线比较器可读取的 3000 B 字节。',
                        'dp_send_dyn_tele、sendDynTele、set/get_float_value、set/get_uint8_value、Put_*_In_Buff。',
                        'src/dynamic_ipc_telemetry.h/.c；IPC gold 和 P1 ABI 对照 [S15]')
    base.add_table(doc, ['协议项', '固定定义', '设计要求'], [
        ('帧大小', 'DP_IPC_SHM_BYTES=0xBF0=3056 B', '前 0x38 B 是锁；比较器的 payload 观察为 3000 B。'),
        ('float 区', '+0x038', '使用 float setter/getter 的索引/字节序语义。'),
        ('u8 区', '+0x998', '写入有效标志和五个 DRC 控制字节等。'),
        ('端序', 'endian_flag=0 高字节优先；1 原样复制', 'Put/var2buff/read 系列必须按当前标志路径实现。'),
        ('并发', 'pthread_rwlock_t 固定 0x38 B（Linux x86-64）', '离线核心比较只验证确定性路径，不声明所有竞争时序。'),
    ], widths=[3.0, 5.3, 8.1], font_size=8.0)

    add_module_contract(doc, '22', '命令解析、调度包装与全局 ABI 兼容层',
                        '为原 ELF 保留的公共入口提供输入映射、全局对象桥接、桌面/共享输入访问、算法遥测包装和 legacy 调用兼容。它们把物理仿真核心暴露给原始 ABI。',
                        '共享输入索引、桌面命令状态、legacy command/state/output 指针。',
                        'tc_cmd、desk_command、共享帧、模型全局对象、算法遥测 size。',
                        '转换后的命令帧、全局状态副作用、legacy 输出与 IPC。',
                        'getDynInput、Analyze_Command、Command_Execute、getDeskCommand、dyn_main、dyn_main_array、Algorithm_Command_Execute。',
                        'src/dynamic_core_layout.h、dynamic_scheduler.c、dynamic_recovered.c、dynamic_ipc_telemetry.c。')
    add_text(doc, '该章中的若干公共入口在原 ELF 中是 ABI/协议胶水，部分字段语义仍只具有偏移级证据。详细设计要求实现时严格保持已确认的对象大小、读写偏移、返回值和副作用；对尚未确定字段保持 reserved/guessed 命名，不可把猜测扩展为事实。')


def add_verification(doc):
    add_module_contract(doc, '23', '行为等价验证、gold 采集与严格回归',
                        '以原 ELF 为只读金标，对当前恢复模型的限定场景逐步逐字节差分。该单元定义工程验收，而不是物理模型本身。',
                        '固定初态、时间、设备命令、随机种子、原 ELF 调试副本、观察块配置。',
                        'gold 文件、场景比较器、Makefile selftest 规则、回归日志。',
                        'cmp/sha256 双采集结论、逐步 PASS/首分叉诊断、默认自测结果。',
                        'analysis/time_orbit/*.gdb；analysis/*compare.c；Makefile selftest。',
                        'analysis/coverage_inventory/long_horizon_matrix_h0_h1.md；H0–H21（含 H21 10,000/100,000 步）[S16][S17][S18]')
    base.add_table(doc, ['观察块', '覆盖情况', '比较语义'], [
        ('CoreDynamic caller-state 前 264 B', '22/22 场景、每步', '动力学调用边界状态写入。'),
        ('main telemetry 544 B', '22/22 场景、每步', '主遥测封包字节序列。'),
        ('IPC payload 3000 B', '22/22 场景、每步', '共享协议 payload 字节序列。'),
        ('internal global y[33] 264 B', '12/22 场景、每步', '只对已有原 ELF gold 的场景宣称覆盖。'),
        ('执行机构终态对象', 'H18–H21', 'RWheel、MTQ、Thruster 和四个推力器向量的终态快照。'),
    ], widths=[5.0, 4.5, 6.9], font_size=8.2)
    add_code(doc, '算法 12：新场景接入门禁流程', [
        '1. 审计：从已有场景选择独立输入维度；审查初始化/命令/观察块。',
        '2. 探针：只运行 DynamicPackage.exec_copy；不得修改 input/DynamicPackage.elf。',
        '3. Gold：基础场景 100/1000 步各采集两次；H21 另完成 10,000/100,000 步双采集；逐文件 cmp 与 SHA-256 一致。',
        '4. 差分：恢复端用同一初始化/随机种子/命令，每一步按块 memcmp。',
        '5. 分叉：定位首步首块→反汇编→隔离诊断→最小修复→重跑全链路。',
        '6. 门禁：全部计划时域 PASS 后才修改 Makefile；make clean && make selftest。',
    ])
    base.add_callout(doc, '验收边界', 'H0–H21 的 PASS 证明的是当前 Linux x86-64、当前运行时、严格 C11 编译、列明输入和列明观察块下的逐步 bitwise 一致；其中 H21 第二 LCG 交叉序列已验证至 100,000 步。它不证明任意连续输入、任意长度命令序列、所有内部对象、并发时序或其他平台的全域等价。', fill='FCE4D6')


def add_appendices(doc):
    base.add_heading(doc, '附录 A：核心常量、对象尺寸与必须逐值复制的数据', 1)
    base.add_table(doc, ['类别', '当前事实', '实现要求'], [
        ('轨道常量', 'μ=398600441500000.0；Re=6378137.0；J2/J3/J4 见 dynamic_orbit.c', '保持 double 常量位模式与累加顺序。'),
        ('状态维度', 'DP_STATE_DIM=33', '不得压缩、重排或拆成不同积分器状态。'),
        ('设备对象', 'RWheel 4×0x78；MTQ 6×0x38；Thruster 0xb0；SADA 0x68', '保持字段偏移、对齐和对象更新顺序。'),
        ('传感器对象', 'Gyro 2×0x320；MagMeter 2×0x138；DSS 2×0x158；STS 3×0x170；GPS 0x70', '保持噪声/矩阵/测量/标志字段的偏移语义。'),
        ('输出对象', 'main 0x220；Core input 0x90；IPC SHM 0xBF0', '封包/协议按确认偏移写入；不要直接依赖编译器结构体布局。'),
        ('环境大表', 'IGRF 14×14；默认刚柔模型 inertia/coupling/MA/MD 与 SADA maps', '从 src/dynamic_environment.c 与 dp_core_default_model_init 的数据逐值复制。'),
    ], widths=[3.1, 7.0, 6.3], font_size=8.1)

    base.add_heading(doc, '附录 B：源码—详细设计章节追踪矩阵', 1)
    base.add_table(doc, ['源码模块', 'DDS 章节', '覆盖内容'], [
        ('dynamic_math.c/.h', '第 2 章', '向量、矩阵、旋转、四元数、逆矩阵、随机数与浮点顺序'),
        ('dynamic_time.c/.h', '第 3 章', '日历、JD、GMST、岁差、坐标变换'),
        ('dynamic_orbit.c/.h', '第 4 章', '开普勒、要素互转、二体/J2/J3/J4、外力'),
        ('dynamic_environment.c/.h', '第 5 章', '太阳、IGRF、局地坐标、环境辅助'),
        ('dynamic_dynamics.c/.h / dynamic_flex.c/.h / dynamic_torque.c/.h', '第 6 章', '33 维 ODE、力矩、刚柔耦合'),
        ('dynamic_rk4.c/.h', '第 7 章', '四阶段积分、时间/四元数更新'),
        ('dynamic_devices.c/.h', '第 8–12 章', '轮、MTQ、推力器、SADA、设备调度'),
        ('dynamic_sensors.c/.h / sensor_schedule.c', '第 13–19 章', '噪声、五类传感器、有效性和测量总装'),
        ('dynamic_main_bridge.c/.h / core_bridge.c/.h / core_environment.c/.h', '第 1、6 章', '顶层单步、核心默认模型、环境尾区'),
        ('dynamic_telemetry.c/.h / telemetry_layout.h', '第 20 章', '544 B main telemetry'),
        ('dynamic_ipc_telemetry.c/.h', '第 21 章', '3000 B payload、共享内存、端序'),
        ('scheduler/recovered/wrappers/globals', '第 22 章', 'ABI 兼容、命令、全局对象与包装'),
        ('analysis + Makefile', '第 23 章', 'gold、差分、回归与覆盖边界'),
    ], widths=[5.1, 3.0, 8.3], font_size=8.0)

    base.add_heading(doc, '附录 C：设计使用限制', 1)
    add_text(doc, '本设计书已尽量覆盖当前恢复模型的全部已审计算法逻辑与接口，使实现者能够按同一单星软件仿真结构重建模块。但是，原 ELF 的未观测字段、被标为 reserved/guessed 的命名、未穷尽输入空间和平台相关浮点/随机数行为仍必须由原 ELF 差分验证确认。不可把文档或恢复代码用于任何实际飞行、控制、实体执行机构或安全关键用途。')


def build_doc():
    doc = Document()
    section = doc.sections[0]
    section.top_margin = Cm(1.5); section.bottom_margin = Cm(1.5)
    section.left_margin = Cm(1.55); section.right_margin = Cm(1.55)
    base.create_styles(doc)
    footer = section.footer.paragraphs[0]; footer.text = ''
    base.add_page_number(footer)
    header = section.header.paragraphs[0]; header.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = header.add_run('DynamicPackage 单星离线仿真详细设计说明书  |  H0–H21（含 100,000 步）')
    set_font(r, size=8, color='7F7F7F')

    add_cover(doc)
    add_reader_contract(doc)
    add_architecture(doc)
    add_math_module(doc)
    add_time_coordinate_module(doc)
    add_orbit_environment_modules(doc)
    add_dynamics_modules(doc)
    add_device_modules(doc)
    add_sensor_modules(doc)
    add_output_modules(doc)
    add_verification(doc)
    add_appendices(doc)

    doc.core_properties.title = 'DynamicPackage 单星离线仿真详细设计说明书（H0–H21，含 100,000 步验证）'
    doc.core_properties.subject = '当前 C11 行为等价重建模型的模块化详细设计'
    doc.core_properties.author = 'Manus AI'
    doc.core_properties.comments = '仅限离线研究与仿真。'
    doc.save(OUT)
    print(OUT)


if __name__ == '__main__':
    build_doc()
