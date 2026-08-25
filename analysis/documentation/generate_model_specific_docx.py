from pathlib import Path
from docx import Document
from docx.shared import Cm, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_BREAK
from docx.enum.section import WD_SECTION
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.enum.style import WD_STYLE_TYPE
from docx.enum.text import WD_LINE_SPACING
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import re

ROOT = Path('/home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work')
OUT_DIR = ROOT / 'analysis' / 'documentation'
OUT = OUT_DIR / 'DynamicPackage_单星模型算法与符号说明_H0-H20.docx'
ASSET_DIR = OUT_DIR / 'docx_formula_assets'
ASSET_DIR.mkdir(parents=True, exist_ok=True)

CJK = 'Noto Sans CJK SC'
CJK_SERIF = 'Noto Serif CJK SC'
BLUE = '1F4E79'
TEAL = '0F6B78'
LIGHT_BLUE = 'D9EAF7'
LIGHT_TEAL = 'DDEBF7'
GRAY = 'F2F2F2'
DARK = '1F1F1F'


def set_run_font(run, name=CJK, size=None, bold=None, color=None, italic=None):
    run.font.name = name
    run._element.rPr.rFonts.set(qn('w:eastAsia'), name)
    if size is not None:
        run.font.size = Pt(size)
    if bold is not None:
        run.bold = bold
    if italic is not None:
        run.italic = italic
    if color is not None:
        run.font.color.rgb = RGBColor.from_string(color)


def set_cell_shading(cell, fill):
    tc_pr = cell._tc.get_or_add_tcPr()
    shd = OxmlElement('w:shd')
    shd.set(qn('w:fill'), fill)
    tc_pr.append(shd)


def set_cell_text(cell, text, bold=False, fill=None, size=8.6, color=None):
    if fill:
        set_cell_shading(cell, fill)
    cell.vertical_alignment = WD_CELL_VERTICAL_ALIGNMENT.CENTER
    cell.text = ''
    p = cell.paragraphs[0]
    p.paragraph_format.space_after = Pt(0)
    p.paragraph_format.space_before = Pt(0)
    p.paragraph_format.line_spacing = 1.05
    r = p.add_run(str(text))
    set_run_font(r, size=size, bold=bold, color=color)


def set_repeat_table_header(row):
    tr_pr = row._tr.get_or_add_trPr()
    tbl_header = OxmlElement('w:tblHeader')
    tbl_header.set(qn('w:val'), 'true')
    tr_pr.append(tbl_header)


def set_cell_margins(cell, top=80, start=90, bottom=80, end=90):
    tc = cell._tc
    tc_pr = tc.get_or_add_tcPr()
    tc_mar = tc_pr.first_child_found_in('w:tcMar')
    if tc_mar is None:
        tc_mar = OxmlElement('w:tcMar')
        tc_pr.append(tc_mar)
    for m, v in [('top', top), ('start', start), ('bottom', bottom), ('end', end)]:
        node = tc_mar.find(qn(f'w:{m}'))
        if node is None:
            node = OxmlElement(f'w:{m}')
            tc_mar.append(node)
        node.set(qn('w:w'), str(v))
        node.set(qn('w:type'), 'dxa')


def add_table(doc, headers, rows, widths=None, font_size=8.5):
    table = doc.add_table(rows=1, cols=len(headers))
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.style = 'Table Grid'
    hdr = table.rows[0]
    set_repeat_table_header(hdr)
    for i, h in enumerate(headers):
        set_cell_text(hdr.cells[i], h, bold=True, fill=BLUE, color='FFFFFF', size=font_size)
        set_cell_margins(hdr.cells[i])
    for row in rows:
        cells = table.add_row().cells
        for i, value in enumerate(row):
            fill = 'FFFFFF' if len(table.rows) % 2 else GRAY
            set_cell_text(cells[i], value, fill=fill, size=font_size)
            set_cell_margins(cells[i])
    if widths:
        for row in table.rows:
            for i, width in enumerate(widths):
                row.cells[i].width = Cm(width)
    doc.add_paragraph().paragraph_format.space_after = Pt(2)
    return table


def add_paragraph(doc, text='', bold_prefix=None, style=None, before=0, after=5, size=10.5):
    p = doc.add_paragraph(style=style)
    p.paragraph_format.space_before = Pt(before)
    p.paragraph_format.space_after = Pt(after)
    p.paragraph_format.line_spacing = 1.24
    if bold_prefix and text.startswith(bold_prefix):
        r = p.add_run(bold_prefix)
        set_run_font(r, size=size, bold=True, color=DARK)
        r = p.add_run(text[len(bold_prefix):])
        set_run_font(r, size=size)
    else:
        r = p.add_run(text)
        set_run_font(r, size=size)
    return p


def add_bullet(doc, text, level=0):
    p = doc.add_paragraph(style='List Bullet' if level == 0 else 'List Bullet 2')
    p.paragraph_format.space_after = Pt(2)
    p.paragraph_format.line_spacing = 1.15
    r = p.add_run(text)
    set_run_font(r, size=10)
    return p


def add_caption(doc, text):
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(1)
    p.paragraph_format.space_after = Pt(7)
    r = p.add_run(text)
    set_run_font(r, size=8.5, italic=True, color='595959')
    return p


def add_formula_image(doc, key, latex, caption, width_cm=14.6):
    # Matplotlib mathtext 要求粗体命令显式带花括号；此处只做排版兼容转换，
    # 不改变公式的模型变量、常量或运算关系。
    latex = re.sub(r'\\boldsymbol(\\[A-Za-z]+|[A-Za-z])', r'\\boldsymbol{\1}', latex)
    latex = re.sub(r'\\mathbf\s*([A-Za-z0-9])', r'\\mathbf{\1}', latex)
    latex = latex.replace(r'\operatorname{sat}', r'\mathrm{sat}')
    latex = latex.replace(r'\mathcal N', r'\mathcal{N}')
    latex = latex.replace(r'\mathbb R', r'\mathrm{R}')
    latex = latex.replace(r'\big', '')
    path = ASSET_DIR / f'{key}.png'
    fig = plt.figure(figsize=(9.6, 1.15), dpi=220)
    fig.patch.set_facecolor('white')
    ax = fig.add_axes([0, 0, 1, 1])
    ax.axis('off')
    ax.text(0.5, 0.5, f'${latex}$', ha='center', va='center', fontsize=17, color='#152C45')
    fig.savefig(path, dpi=220, bbox_inches='tight', pad_inches=0.08, facecolor='white')
    plt.close(fig)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(0)
    p.add_run().add_picture(str(path), width=Cm(width_cm))
    add_caption(doc, caption)


def add_callout(doc, title, body, fill='EAF3F8'):
    table = doc.add_table(rows=1, cols=1)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    cell = table.cell(0, 0)
    set_cell_shading(cell, fill)
    set_cell_margins(cell, top=120, start=160, bottom=120, end=160)
    p = cell.paragraphs[0]
    p.paragraph_format.space_after = Pt(2)
    r = p.add_run(title)
    set_run_font(r, size=10, bold=True, color=BLUE)
    p2 = cell.add_paragraph()
    p2.paragraph_format.space_after = Pt(0)
    p2.paragraph_format.line_spacing = 1.2
    r = p2.add_run(body)
    set_run_font(r, size=9.5)
    doc.add_paragraph().paragraph_format.space_after = Pt(2)


def add_page_number(paragraph):
    paragraph.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = paragraph.add_run('第 ')
    set_run_font(run, size=8, color='666666')
    fld = OxmlElement('w:fldSimple')
    fld.set(qn('w:instr'), 'PAGE')
    paragraph._p.append(fld)
    run = paragraph.add_run(' 页')
    set_run_font(run, size=8, color='666666')


def create_styles(doc):
    normal = doc.styles['Normal']
    normal.font.name = CJK
    normal._element.rPr.rFonts.set(qn('w:eastAsia'), CJK)
    normal.font.size = Pt(10.5)
    normal.paragraph_format.line_spacing = 1.24
    normal.paragraph_format.space_after = Pt(5)

    for name, size, color in [('Title', 24, BLUE), ('Heading 1', 16, BLUE), ('Heading 2', 13, TEAL), ('Heading 3', 11, DARK)]:
        s = doc.styles[name]
        s.font.name = CJK
        s._element.rPr.rFonts.set(qn('w:eastAsia'), CJK)
        s.font.size = Pt(size)
        s.font.bold = True
        s.font.color.rgb = RGBColor.from_string(color)
        s.paragraph_format.space_before = Pt(15 if name == 'Heading 1' else 10)
        s.paragraph_format.space_after = Pt(6)
        s.paragraph_format.keep_with_next = True

    if 'Formula Caption' not in [x.name for x in doc.styles]:
        style = doc.styles.add_style('Formula Caption', WD_STYLE_TYPE.PARAGRAPH)
        style.font.name = CJK
        style._element.rPr.rFonts.set(qn('w:eastAsia'), CJK)


def add_heading(doc, text, level):
    p = doc.add_heading(text, level=level)
    p.paragraph_format.keep_with_next = True
    return p


def add_cover(doc):
    p = doc.add_paragraph()
    p.paragraph_format.space_before = Pt(74)
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('DynamicPackage')
    set_run_font(r, name=CJK_SERIF, size=28, bold=True, color=BLUE)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('模型贴合算法、公式与验证说明')
    set_run_font(r, name=CJK_SERIF, size=21, bold=True, color=TEAL)
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r = p.add_run('基于当前 C11 行为等价重建工程与 H0–H20 原 ELF 差分证据')
    set_run_font(r, size=11, color='595959')

    doc.add_paragraph()
    add_callout(doc, '文档定位', '本文不是通用卫星教材的公式汇编。每一节均以当前恢复模型的实际状态布局、函数入口、常量、矩阵尺寸、更新顺序和已采集 gold 为边界；正文中的“模型”均指当前 DynamicPackage 恢复端及其对应原 ELF 观察行为。', fill='E2F0D9')
    add_table(doc, ['模型快照', '当前事实'], [
        ('状态维度', '33 个 double：4 姿态四元数 + 3 角速度 + 3 位置 + 3 速度 + 10 柔性模态位移 + 10 模态速度'),
        ('主步入口', 'dp_dyn_main_recovered_step → 设备控制 → 核心 RK4/环境 → 传感器 → 主遥测 → 可选 IPC'),
        ('核心输出', 'CoreDynamic caller-state 前 264 B、main telemetry 544 B、IPC payload 3000 B；部分场景另含 global y[33] 264 B'),
        ('验证基线', 'H0–H20 共 21 个受控场景；100/1000 步双采集并逐步 bitwise 差分；H20 后默认 selftest 为 318 个可执行项、309 条 PASS'),
        ('原 ELF 金标', 'input/DynamicPackage.elf，mode 0400，SHA-256：312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403'),
    ], widths=[3.0, 13.2], font_size=9.2)

    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(35)
    r = p.add_run('工程用途：仅限离线研究与仿真；禁止用于飞行、控制、实体执行机构或安全关键用途。')
    set_run_font(r, size=9.5, bold=True, color='9C0006')
    doc.add_page_break()


def add_contents(doc):
    add_heading(doc, '阅读导航', 1)
    add_paragraph(doc, '阅读时建议先看“模型总装与单步顺序”，再按需要进入轨道、刚柔耦合、传感器或输出协议章节。末尾“验证边界”说明哪些结论有原 ELF 位级证据、哪些仍是未穷尽区域。')
    rows = [
        ('0', '单星闭环模型与符号约定', '从单颗卫星的状态、环境、设备、传感器到输出的统一模型；含符号、单位、坐标系'),
        ('1', '模型总装、状态与数据流', '从输入命令到状态、main 和 IPC 的完整单步路径'),
        ('2', '坐标、时间与环境', 'JD、GMST、GCI/ECEF、太阳、IGRF 地磁'),
        ('3', '轨道动力学与轨道要素', '开普勒方程、二体/J2/J3/J4、要素互转'),
        ('4', '姿态、刚体与柔性耦合', '四元数、外力矩、J_eff、模态状态'),
        ('5', 'RK4 与数值等价约束', '四阶段积分、浮点顺序、qNaN'),
        ('6', '设备与传感器', '飞轮、MTQ、推力器、SADA、STS、DSS、GPS'),
        ('7', '输出协议与验证', '544 B main、3000 B IPC、H0–H20 覆盖与边界'),
        ('附录', '函数—算法—证据索引', '继续审计或扩展场景时的源码定位入口'),
    ]
    add_table(doc, ['章节', '主题', '阅读价值'], rows, widths=[1.2, 5.2, 9.8], font_size=9)
    add_callout(doc, '如何阅读公式', '所有公式均使用当前模型的变量名称或与其一一对应的数学记号。公式旁明确给出实现函数、矩阵尺寸、调用顺序或常量；若某标准教材公式与当前模型不同，以本模型源码和原 ELF 证据为准。')


def create_single_satellite_diagram():
    path = ASSET_DIR / 'single_satellite_closed_loop.png'
    plt.rcParams['font.sans-serif'] = [CJK]
    plt.rcParams['axes.unicode_minus'] = False
    fig, ax = plt.subplots(figsize=(11.0, 5.9), dpi=220)
    fig.patch.set_facecolor('white')
    ax.set_xlim(0, 13)
    ax.set_ylim(0, 8)
    ax.axis('off')

    def block(x, y, w, h, title, body, color):
        patch = FancyBboxPatch((x, y), w, h, boxstyle='round,pad=0.06,rounding_size=0.12',
                               linewidth=1.25, edgecolor='#2B4E6C', facecolor=color)
        ax.add_patch(patch)
        ax.text(x + w / 2, y + h - 0.38, title, ha='center', va='center', fontsize=10.5,
                fontweight='bold', color='#17365D', fontname=CJK)
        ax.text(x + w / 2, y + h / 2 - 0.18, body, ha='center', va='center', fontsize=8.2,
                color='#1F1F1F', linespacing=1.28, fontname=CJK)

    def arrow(x1, y1, x2, y2, label='', curve=0.0):
        a = FancyArrowPatch((x1, y1), (x2, y2), arrowstyle='-|>', mutation_scale=12,
                            connectionstyle=f'arc3,rad={curve}', linewidth=1.15, color='#4F81BD')
        ax.add_patch(a)
        if label:
            ax.text((x1 + x2) / 2, (y1 + y2) / 2 + (0.24 if curve >= 0 else -0.28), label,
                    ha='center', va='center', fontsize=7.5, color='#365F91', fontname=CJK)

    block(0.35, 5.45, 2.30, 1.45, '外部离散输入', '时间 t、步长 Δt\n命令 u、初始状态 y0\n固定种子与配置 Θ', '#EAF2F8')
    block(3.35, 5.45, 2.30, 1.45, '设备状态更新', '飞轮、MTQ、推力器\nSADA、惯量标志\nxdev(k+1)', '#E2F0D9')
    block(6.35, 5.10, 2.65, 2.15, '单星核心动力学', '33 维状态 y\n姿态 + 轨道 + 柔性\nRK4、J2/J3/J4\n刚柔耦合', '#D9EAF7')
    block(10.00, 5.45, 2.45, 1.45, '传感器测量', 'STS、Gyro、DSS\nMagMeter、GPS\nz(k+1)', '#FFF2CC')
    block(10.00, 1.20, 2.45, 1.55, '对外二进制输出', 'main 544 B\nIPC 3000 B\n有效标志与设备状态', '#FCE4D6')
    block(3.50, 1.20, 2.35, 1.55, '环境与坐标', 'JD / GMST / GCI↔ECEF\n太阳、IGRF 地磁\n外力/外力矩', '#EDEDED')
    block(6.40, 1.20, 2.55, 1.55, '模型反馈量', 'Hwheel、τwheel、m×B\nSADA 加速度\nFthruster、Tthruster', '#F4CCCC')

    arrow(2.65, 6.15, 3.35, 6.15, 'u(k)')
    arrow(5.65, 6.15, 6.35, 6.15, 'H、τ、m')
    arrow(9.00, 6.15, 10.00, 6.15, 'y(k+1)')
    arrow(11.23, 5.45, 11.23, 2.75, 'z(k+1)')
    arrow(5.85, 2.00, 6.40, 2.00, '环境项')
    arrow(7.67, 2.75, 7.67, 5.10, '耦合注入')
    arrow(5.85, 2.00, 6.35, 5.10, '时间/环境', curve=0.20)
    arrow(9.00, 5.10, 10.00, 2.75, '真值/设备')
    arrow(6.35, 5.10, 5.30, 2.75, 'GCI 状态', curve=-0.18)
    ax.text(6.45, 7.65, 'DynamicPackage 当前恢复模型：单星离散仿真闭环（不含自主控制律）',
            ha='center', va='center', fontsize=13, fontweight='bold', color='#17365D', fontname=CJK)
    ax.text(6.45, 0.43, '说明：这里的“闭环”指设备状态—动力学—测量—输出的数据反馈；命令 u 由外部测试/调用者给定。',
            ha='center', va='center', fontsize=8.2, color='#666666', fontname=CJK)
    fig.savefig(path, bbox_inches='tight', pad_inches=0.12, facecolor='white')
    plt.close(fig)
    return path


def add_single_satellite_model_section(doc):
    add_heading(doc, '0. 单星闭环模型与符号约定', 1)
    add_paragraph(doc, '本节把当前 DynamicPackage 恢复端抽象成一个可独立阅读的单星离散仿真模型。模型对象是一颗带姿态、轨道、10 阶柔性模态、执行机构和传感器的卫星；所谓“闭环”仅表示软件内部的状态反馈与输出链路，并不包含自主制导、真实飞控或实体执行。外部调用者提供时间、步长、初始状态和设备命令。[S12]')
    diagram = create_single_satellite_diagram()
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(2)
    p.add_run().add_picture(str(diagram), width=Cm(16.3))
    add_caption(doc, '图 0：当前恢复端的单星离散数据流。红色反馈块是进入本步刚体/柔性方程的设备耦合项；它不是控制器。')

    add_heading(doc, '0.1 统一的单步数学模型', 2)
    add_formula_image(doc, 'single_satellite_step', r'x_{dev,k+1}=D(x_{dev,k},u_k,\Delta t,\Theta)\;\to\;y_{k+1}=\Phi_{RK4}(y_k,t_k,\Delta t,x_{dev,k+1},e_k,\Theta)\;\to\;z_{k+1}=H(y_{k+1},x_{dev,k+1},e_{k+1},\xi_k,\Theta)\;\to\;o_{k+1}=G(y_{k+1},x_{dev,k+1},z_{k+1},\Theta)', '式 0：单星离散模型的统一写法。D 为设备更新；ΦRK4 为 33 维核心传播；H 为传感器；G 为 main/IPC 输出。')
    add_paragraph(doc, '为适配公式渲染，式 0 的四个关系在逻辑上按顺序成立：先得到设备状态 xdev,k+1，再在环境 e 与设备耦合项下用 RK4 得到 yk+1，随后生成传感器 z，最后打包对外输出 o。当前源码对应关系为 dp_update_device_control → dp_core_default_rk4_step_with_terms_and_environment → dp_update_device_measure_recovered → dp_update_main_out_pack / dp_send_dyn_tele。[S6] [S12] [S14] [S15]')
    add_callout(doc, '符号阅读规则', '粗体小写表示三维向量，粗体大写表示矩阵，普通小写表示标量；下标 k 表示离散步；上标 T 表示转置；带点变量表示时间导数。若同一对象既有数学记号又有源码名，表格同时给出。')

    add_heading(doc, '0.2 核心状态、输入、环境、测量与输出', 2)
    add_table(doc, ['类别', '统一记号', '在当前模型中的实际含义', '尺寸/单位', '源码线索'], [
        ('连续状态', 'y_k', '33 维积分状态：q、ω、r、v、η、η̇', '33×1 double', 'DP_STATE_DIM；dp_differential_equation_33'),
        ('设备状态', 'xdev,k', '飞轮、MTQ、推力器、SADA、惯量相关对象', '多对象', 'DpDeviceControl / dp_update_device_control'),
        ('外部命令', 'u_k', '轮扭矩、MTQ 磁矩、推力器工作状态/推力、SADA 角度、惯量更新标志', '命令结构', 'dynamic_devices.c'),
        ('环境', 'e_k', 'JD/GMST、太阳向量、地磁、外力/外力矩、坐标变换', '结构/向量', 'dynamic_time.c、dynamic_environment.c'),
        ('随机样本', 'ξ_k', 'C rand() 消耗的高斯噪声样本', '标量序列', 'dp_ran_gaussian_recovered'),
        ('测量', 'z_k', 'STS、Gyro、DSS、MagMeter、GPS 及有效标志', '设备测量对象', 'dp_update_device_measure_recovered'),
        ('对外输出', 'o_k', 'main telemetry 与 IPC payload', '544 B / 3000 B', 'dp_update_main_out_pack / dp_send_dyn_tele'),
        ('参数配置', 'Θ', '质量、惯量、矩阵、设备极限、传感器投影、模型常量', '全局配置', '恢复端设备/环境/柔性配置'),
    ], widths=[2.1, 2.3, 6.8, 2.7, 3.1], font_size=8.0)

    add_heading(doc, '0.3 符号、维度、单位与坐标系总表', 2)
    add_table(doc, ['符号', '定义与当前模型含义', '维度', '单位/取值', '出现位置'], [
        ('t, Δt, k', '仿真时刻、固定/调用给定步长、离散步序号', '标量', 's，s，无量纲', 'TimeAdd、RK4'),
        ('q=[q0,q1,q2,q3]T', '标量在前的姿态四元数；y[0..3]', '4×1', '无量纲；归一化', 'quat_xi / quat_diff'),
        ('A(q)', '由 Ψ(q)、Ξ(q) 构造的方向余弦矩阵', '3×3', '无量纲', 'dynamic_math.c'),
        ('ω, α', '本体系角速度、角加速度；y[4..6] 与其导数', '3×1', 'rad/s，rad/s²', 'dynamics / flex'),
        ('r, v', 'GCI 位置、速度；y[7..12]', '各 3×1', 'm，m/s', 'orbit_dynamic'),
        ('a_orbit', 'GCI 轨道加速度，含中心项、J2/J3/J4 与外力', '3×1', 'm/s²', 'orbit_dynamic'),
        ('η, η̇, η̈', '10 阶柔性模态位移、速度、加速度；y[13..32]', '各 10×1', '模型模态单位', 'dynamic_flex.c'),
        ('J, Jeff', '刚体惯量与有效惯量 J−CCT', '3×3', 'kg·m²', 'flex_effective_inertia_3x3'),
        ('C, MA, MD', '刚柔耦合、模态刚度/阻尼相关矩阵', '3×10；10×10', '配置相关', 'dynamic_flex.c'),
        ('Htotal, Hw', '总角动量与轮组角动量', '各 3×1', 'kg·m²/s', 'dynamics / devices'),
        ('τ, F, m', '力矩、推力、MTQ 磁矩', '各 3×1', 'N·m，N，A·m²/模型单位', 'torque / devices'),
        ('B, sSun', '本体系磁场、太阳方向向量', '各 3×1', '模型磁场单位，m/方向量', 'environment / sensors'),
        ('μ, Re, J2,J3,J4', '地球引力参数、参考半径、带谐常量', '标量', 'm³/s²，m，无量纲', 'dynamic_orbit.c'),
        ('JD, T, θGMST', '儒略日、儒略世纪、格林尼治平恒星时', '标量', 'day，无量纲，rad', 'dynamic_time.c'),
        ('u, xdev, z, o', '外部命令、设备内部状态、测量、二进制输出', '结构/对象', '见上表', 'main bridge / telemetry'),
        ('ξ, μnoise, σnoise', '随机样本、噪声均值、噪声标准差', '标量/向量', '按设备定义', 'dynamic_sensors.c'),
    ], widths=[2.3, 6.1, 2.2, 3.2, 3.2], font_size=7.7)

    add_heading(doc, '0.4 坐标系与下标约定', 2)
    add_table(doc, ['记号', '名称', '轴/参考说明', '当前模型用途'], [
        ('GCI / I', '地心惯性系', '地心、近似惯性参考', '轨道 r,v；太阳；惯性地磁；GPS'),
        ('ECEF / E', '地球固连系', '随地球自转', '重力带谐、局地几何、遥测位置速度'),
        ('B', '卫星本体系', '随卫星姿态转动', 'ω、J、执行机构、传感器、力矩'),
        ('LocalGeo', '局地北东地下系', '由 ECEF 位置构造 N/E/D', '局地地磁、辅助几何'),
        ('LVLH', '局地轨道参考系', '按位置与参考方向构造', '局地姿态/几何辅助'),
        ('p', '轨道平面系', '开普勒椭圆平面', '轨道要素到 GCI 位置速度'),
    ], widths=[2.4, 3.5, 5.0, 6.1], font_size=8.1)
    add_callout(doc, '容易混淆的符号', 'r 既可出现在轨道公式中表示 GCI 位置向量，也常用 r=‖r‖ 表示其模长；B 在 m×B 中是磁场，在下标 B 中表示本体系；T 既可表示转置上标，也可表示儒略世纪或力矩符号的语境变量，具体含义由位置决定。', fill='FFF2CC')


def add_system_section(doc):
    add_heading(doc, '1. 模型总装、状态与单步数据流', 1)
    add_paragraph(doc, '当前恢复模型的高层步函数是 dp_dyn_main_recovered_step。它不是把各模块松散拼接，而是以确定的副作用顺序推进：设备对象先变更；其轮组角动量与力矩进入核心动力学；核心积分同时推进时间与 33 维状态；之后才更新传感器、主遥测和 IPC。[S12]')
    add_table(doc, ['阶段', '当前函数/对象', '模型作用', '可观察结果'], [
        ('① 命令与设备', 'dp_update_device_control', '处理惯量标志、4 个飞轮、6 个 MTQ、推力器和双轴 SADA', '设备对象和下一步动力学注入量改变'),
        ('② 动量/力矩注入', 'wheel_group_angular_momentum_3 / wheel_group_torque_3 / mtq_group_moment_3', '组成 H_w、轮反作用与 MTQ×B 项', '刚体/柔性 RHS 改变'),
        ('③ 核心传播', 'dp_core_default_rk4_step_with_terms_and_environment', 'TimeAdd、环境构造、33 维 RK4、四元数归一化', 'state 与 global y 的动态演化'),
        ('④ 传感器', 'dp_update_device_measure_recovered', 'STS、Gyro、DSS、MagMeter、GPS', '测量、有效标志、随机数状态'),
        ('⑤ 输出', 'dp_update_main_out_pack / dp_send_dyn_tele', '生成 544 B 主遥测与 3000 B IPC', '对外 main/IPC 字节序列'),
    ], widths=[1.3, 4.7, 5.2, 5.0], font_size=8.5)
    add_callout(doc, '模型关键点', '设备更新发生在积分之前，传感器更新发生在积分之后；因此同一条命令既可能立即改变执行机构对象，也可能通过飞轮/MTQ/SADA 注入影响当步传播后的状态。改变调用顺序会破坏当前 gold。')

    add_heading(doc, '1.1 33 维状态的模型专属布局', 2)
    add_table(doc, ['状态下标', '数学变量', '当前模型字段/角色', '导数'], [
        ('0–3', 'q0,q1,q2,q3', '标量在前的姿态四元数', 'q̇ = 0.5 Ξ(q)ω'),
        ('4–6', 'ωx,ωy,ωz', '本体系角速度', 'ω̇ = α（刚柔耦合求解）'),
        ('7–9', 'rx,ry,rz', 'GCI 位置', 'ṙ = v'),
        ('10–12', 'vx,vy,vz', 'GCI 速度', 'v̇ = a_orbit'),
        ('13–22', 'η1…η10', '10 阶柔性模态位移', 'η̇ = 当前模态速度'),
        ('23–32', 'η̇1…η̇10', '10 阶柔性模态速度', 'η̈ = 柔性模态方程'),
    ], widths=[2.0, 3.2, 6.3, 4.7], font_size=8.6)
    add_formula_image(doc, 'state_rhs', r'\dot y=(\dot q,\boldsymbol\alpha,\mathbf v,\mathbf a_{orbit},\ddot{\boldsymbol\eta},\dot{\boldsymbol\eta})^T,\quad y\in\mathbb R^{33}', '式 1：当前模型 33 维状态微分方程的分块结构。')


def add_math_attitude_section(doc):
    add_heading(doc, '2. 数学基础、坐标与姿态', 1)
    add_paragraph(doc, '恢复模型的数学层没有替换为第三方 BLAS 或统一“优化”表达式，而是保留向量/矩阵循环、行主序和累加顺序。原因是本工程验收的是逐字节输出，而不是仅比较物理量近似误差。[S1]')
    add_heading(doc, '2.1 向量、矩阵与方向约定', 2)
    add_formula_image(doc, 'vector_ops', r'\mathbf a\times\mathbf b=(a_yb_z-a_zb_y,\;a_zb_x-a_xb_z,\;a_xb_y-a_yb_x)^T,\quad \theta=\arccos\frac{\mathbf a^T\mathbf b}{\|\mathbf a\|_2\|\mathbf b\|_2}', '式 2：当前 vector3_cross 与 vector2angle 的数学对应。')
    add_paragraph(doc, '模型行为补充：vector2angle 在维度不同、或任一范数不大于零时返回 0.0；vector_unit 仅在范数严格大于零时写入。这个看似细小的规则影响 DSS、STS 和地影/视场边界。[S1]')
    add_formula_image(doc, 'gemv', r'\mathbf y\leftarrow\alpha\mathbf A\mathbf x+\beta\mathbf y,\qquad \mathbf C\leftarrow\alpha\mathbf A\mathbf B+\beta\mathbf C', '式 3：恢复端 blas_gemv / blas_gemm 的基础语义；实现保留逐元素循环与累加顺序。')

    add_heading(doc, '2.2 四元数与本体姿态', 2)
    add_formula_image(doc, 'quat_kin', r'\Xi(q)=[(q_0,-q_3,q_2);(q_3,q_0,-q_1);(-q_2,q_1,q_0);(-q_1,-q_2,-q_3)],\qquad \dot q=\frac{1}{2}\Xi(q)\boldsymbol\omega', '式 4：当前 quat_xi / quat_diff 的四元数运动学；状态中 q0 为标量部。')
    add_formula_image(doc, 'quat_norm', r'\|q\|=\sqrt{q_0^2+q_1^2+q_2^2+q_3^2},\qquad q\leftarrow q/\|q\|', '式 5：当前模型的四元数范数与积分后归一化。')
    add_paragraph(doc, '当前实现用 Ψ(q)、Ξ(q) 与矩阵乘法构造姿态矩阵，而不是用简化的手写多项式；此外 quat_cross 的导出 ABI 参数方向与内部适配器不同，星敏感器组合必须遵循现有调用点。[S1] [S5]')


def add_time_environment_section(doc):
    add_heading(doc, '3. 时间、坐标、太阳与地磁环境', 1)
    add_heading(doc, '3.1 JD、GMST 与 GCI/ECEF', 2)
    add_formula_image(doc, 'jd', r'JD=367y-\lfloor1.75(y+\lfloor(m+9)/12\rfloor)\rfloor+\lfloor275m/9\rfloor+d+1721013.5+\frac{h+(min+s/60)/60}{24}', '式 6：Calc_JD 的当前实现公式与时间分量顺序。')
    add_formula_image(doc, 'gmst', r'\theta_{GMST}=\operatorname{fmod}(24110.54841+8640184.812866T+0.093104T^2-0.0000062T^3+1.0027379093507951\,t_{UTC},86400)\frac{\pi}{43200}', '式 7：Calc_theta_GMST 的恒星秒多项式与弧度转换。')
    add_formula_image(doc, 'gci_ecef', r'\mathbf r_{ECEF}=R_Z(\theta_{GMST})\,\mathbf P(T)\,\mathbf r_{GCI},\qquad \mathbf r_{GCI}=[R_Z(\theta_{GMST})\mathbf P(T)]^T\mathbf r_{ECEF}', '式 8：当前 GCI/ECEF 双向变换；逆向实现显式构造后再转置。')
    add_paragraph(doc, '岁差采用 P(T)=Rz(−z)Ry(θ)Rz(−ζ)，其中 ζ、θ、z 是 T 的三次多项式。TimeAdd 保留小数秒残量，并仅在整秒边界以 mktime 归一化日历；这也是跨年、闰日 gold 场景的重要行为。[S2]')

    add_heading(doc, '3.2 太阳与 IGRF 地磁', 2)
    add_formula_image(doc, 'sun', r'M=2\pi\operatorname{frac}(99.9973583T+0.9931267),\quad L=2\pi\operatorname{frac}\left(\frac{M}{2\pi}+0.7859444+\frac{6892\sin M+72\sin2M}{1296000}\right)', '式 9：当前太阳模型的平均近点角与黄经相位。')
    add_formula_image(doc, 'sun_radius', r'r_\odot=149619000000-2499000000\cos M-21000000\cos2M', '式 10：当前太阳向量的日地距离近似；随后经固定黄赤交角 X 旋转进入 GCI。')
    add_paragraph(doc, '地磁路径使用当前工程内提取的 14×14 系数数组和 13 阶球谐循环。系数按十进制年线性外推 g(t)=g(t0)+ġ(t−t0)、h(t)=h(t0)+ḣ(t−t0)，通过关联 Legendre 递推形成局地磁场，再回转到 GCI 并按原比例常量缩放。[S4]')
    add_callout(doc, '模型贴合说明', '本模型不是“通用地磁函数接口”：系数表、epoch、比例常量、GCI→ECEF→局地→ECEF→GCI 的顺序均来自当前恢复代码。替换为其他 IGRF 库或不同阶数虽可能更现代，但会破坏本模型等价性。')


def add_orbit_section(doc):
    add_heading(doc, '4. 轨道力学：要素、传播与摄动', 1)
    add_heading(doc, '4.1 开普勒方程与轨道要素', 2)
    add_formula_image(doc, 'kepler', r'E_{k+1}=E_k+\frac{M-(E_k-e\sin E_k)}{1-e\cos E_k},\qquad |\Delta E|<10^{-10}', '式 11：M2E 的 Newton 迭代；初值 E0=M，最多 10,000 次。')
    add_formula_image(doc, 'elements_pv', r'n=\sqrt{\frac{\mu}{a^3}},\quad x_p=a(\cos E-e),\quad y_p=a\sqrt{1-e^2}\sin E,\quad \mathbf r_{GCI}=R_3(\Omega)R_1(i)R_3(\omega)\mathbf r_p', '式 12：Elements2PosVel_M 的核心轨道平面量与 3-1-3 旋转。')
    add_paragraph(doc, '反向要素求解以 r、v、r·v 和 h=r×v 构造 a、e、i、RAAN、近地点幅角、真近点角和平均近点角。当前实现对近圆 e≤1e−14、近赤道 |sin i|≤1e−14 有明确退化分支，而非套用一般椭圆公式。[S3]')

    add_heading(doc, '4.2 二体、J2/J3/J4 与外力', 2)
    add_formula_image(doc, 'central_j2', r'\mathbf a_0=-\frac{\mu}{r^3}\mathbf r,\qquad \mathbf a_{J2}=-\frac{3}{2}J_2\frac{\mu}{R_e^2}\left(\frac{R_e}{r}\right)^2\big((x/r)(1-5s^2),(y/r)(1-5s^2),(z/r)(3-5s^2)\big)^T,\;s=z/r', '式 13：中心引力和当前 J2 展开项。')
    add_formula_image(doc, 'j3j4', r'\mathbf a=\mathbf a_0+\mathbf a_{J2}+\mathbf a_{J3}+\mathbf a_{J4}+\frac{\mathbf F_{external}}{m}', '式 14：当前轨道加速度的总和；J3/J4 采用原 ELF 的显式展开多项式和累加顺序。')
    add_paragraph(doc, '模型常量为 μ=398600441500000.0 m³/s²、Re=6378137.0 m，并包含 J2、J3、J4 常量。真实 orbit_dynamic 先将 GCI 位置转 ECEF、在 ECEF 计算中心项和带谐项、再转回 GCI，最后添加 F_I_external/SpacecraftMass。[S3]')
    add_callout(doc, '高保真细节', '在当前实现中 r³ 是按 r×r×r 的中间变量顺序构造；不能把它替换成 r·(x²+y²+z²)。高偏心场景中二者可出现 1 ULP 差异。')


def add_dynamics_flex_section(doc):
    add_heading(doc, '5. 姿态力矩、刚体与柔性耦合动力学', 1)
    add_heading(doc, '5.1 力矩模型', 2)
    add_formula_image(doc, 'torques', r'\boldsymbol\tau_{RW}=-\boldsymbol\tau_{wheel},\qquad \boldsymbol\tau_{MTQ}=\mathbf m\times\mathbf B,\qquad \boldsymbol\tau_{gg}=\frac{3\mu}{r^3}\,[\hat{\mathbf r}_B\times(\mathbf J\hat{\mathbf r}_B)]', '式 15：当前飞轮反作用、磁力矩和重力梯度力矩。')
    add_paragraph(doc, '当前主桥接会把轮组角动量直接注入核心项，把轮组力矩作为反号核心项；在已有先验本体系磁场时，MTQ 的 m×B 也注入同一力矩链。该注入发生在核心 RK4 之前。[S7] [S12]')
    add_heading(doc, '5.2 总角动量与有效惯量', 2)
    add_formula_image(doc, 'h_total', r'\mathbf H_{total}=\mathbf J\boldsymbol\omega+\mathbf H_w+\mathbf H_{SADA,cmd}+\mathbf C\boldsymbol\eta', '式 16：dp_differential_equation_33 在柔性求解前按固定顺序装配的总角动量。')
    add_formula_image(doc, 'j_eff', r'\mathbf J_{eff}=\mathbf J-\mathbf C\mathbf C^T,\qquad \boldsymbol\alpha=\mathbf J_{eff}^{-1}\mathbf b', '式 17：dp_flex_effective_inertia_3x3 与刚体角加速度求解。')
    add_formula_image(doc, 'rigid_rhs', r'\mathbf u_S=(0,a_{S,0},a_{S,1})^T,\qquad \mathbf b=\mathbf L_c-\boldsymbol\omega\times\mathbf H_{total}-\mathbf R_S\mathbf u_S+0.1\mathbf C\mathbf M_A\boldsymbol\eta+\mathbf C\mathbf M_D\dot{\boldsymbol\eta}', '式 18：当前刚体耦合 RHS 的模型结构与 SADA 反作用项。')
    add_paragraph(doc, '本模型的“柔性”不是泛化有限元求解器，而是固定的 10 模态耦合模型。J、C、MA、MD 及 SADA 映射矩阵由当前卫星全局配置装配；其中有效惯量显式求 3×3 逆矩阵。[S8] [S9]')
    add_heading(doc, '5.3 模态方程', 2)
    add_formula_image(doc, 'modal_rhs', r'\ddot{\boldsymbol\eta}=-\mathbf C^T\boldsymbol\alpha-\mathbf C^T\mathbf M_4\mathbf u_{rigid}-\mathbf C_S^T\mathbf P_S-0.1\mathbf M_A\boldsymbol\eta-\mathbf M_D\dot{\boldsymbol\eta}', '式 19：当前模态加速度的主项；M4 与 SADA 项按配置可选。')
    add_paragraph(doc, '实现顺序是：基础 RHS → SADA 反作用扣除 → MA 柔性项 → MD 阻尼项 → 有效惯量求解 α → 模态方程。为保留 qNaN 传播，MA 与 MD 的中间缓冲不复用，即使某些 BLAS 语义的 β=0 在实数意义上看似可覆盖旧值。[S9]')


def add_rk4_section(doc):
    add_heading(doc, '6. RK4 积分与浮点执行轨迹', 1)
    add_formula_image(doc, 'rk4', r'k_1=f(t,y),\;k_2=f(t+h/2,y+hk_1/2),\;k_3=f(t+h/2,y+hk_2/2),\;k_4=f(t+h,y+hk_3),\;y^+=y+\frac{h}{6}(k_1+2k_2+2k_3+k_4)', '式 20：当前 33 维 RK4 的数学形式。')
    add_paragraph(doc, '当前 dp_rk4_step_33 实际缓存的是 h·k1、h·k2、h·k3、h·k4；末尾按“k4 → 2k3 → 2k2 → k1”的顺序加权，再乘 1/6 后加回状态。随后 dp_rk4_integrator_33 对四元数逐分量除以范数。这个细节是 H0 长时域取证后明确固定的实现约束。[S10]')
    add_table(doc, ['阶段', '当前试探状态', '时间点', '实现行为'], [
        ('k1', 'y', 't', '导数后乘 h 写入 h_k1'),
        ('k2', 'y + 0.5 h_k1', 't + h/2', '导数后乘 h 写入 h_k2'),
        ('k3', 'y + 0.5 h_k2', 't + h/2', '导数后乘 h 写入 h_k3'),
        ('k4', 'y + h_k3', 't + h', '导数后乘 h 写入 h_k4'),
        ('结束', 'y + (h_k1+2h_k2+2h_k3+h_k4)/6', 't+h', '逐分量写回并归一化 q'),
    ], widths=[1.6, 6.2, 2.6, 6.8], font_size=8.8)
    add_callout(doc, '位级约束', '不能把 RK4 表达式融合成 FMA、改变加法结合次序、先求 1/‖q‖ 再乘向量，或启用 fast-math。当前工程固定 -O0、-fno-fast-math、-ffp-contract=off，并用原 ELF 每步 gold 验证。', fill='FCE4D6')


def add_devices_sensors_section(doc):
    add_heading(doc, '7. 执行机构、传感器与有效性逻辑', 1)
    add_heading(doc, '7.1 飞轮、MTQ、推力器和 SADA', 2)
    add_formula_image(doc, 'wheel', r'\alpha_i^{cand}=\tau_i^{cmd}/J_i+\mathcal N(0,\sigma_{\alpha,i}/J_i),\qquad \tau_i=J_i\alpha_i,\quad \omega_i^+=\omega_i+\alpha_i\Delta t,\quad H_i^+=J_i\omega_i^+', '式 21：当前飞轮命令、可选噪声、饱和后加速度和离散更新。')
    add_formula_image(doc, 'thruster_sada', r'\mathbf F_{out}=k_F\mathbf F_{in},\quad \boldsymbol\tau_{thr}=\mathbf l\times\mathbf F_{out},\qquad \Delta\omega=\operatorname{sat}(\theta_{target}-\omega,[-a_{max}\Delta t,a_{max}\Delta t])', '式 22：当前推力器输出及 SADA 速度增量限幅。')
    add_paragraph(doc, 'MTQ 的 6 个命令分别按 moment_limit 饱和后，由 3×6 映射组成群组磁矩。SADA 的双轴命令先按 command_limit 对称钳制，随后限制本步角速度变化，并输出角加速度。设备固定更新顺序为：惯量更新 → 飞轮 → MTQ → 推力器 → SADA。[S6]')

    add_heading(doc, '7.2 高斯噪声与测量投影', 2)
    add_formula_image(doc, 'gaussian', r'x,y\sim(-1,1),\;s=x^2+y^2\in(0,1],\qquad n=y\sigma\sqrt{\frac{-2\ln s}{s}},\qquad z=\mu+n', '式 23：当前 dp_ran_gaussian_recovered 的拒绝采样/极坐标形式。')
    add_formula_image(doc, 'sensor_proj', r'\mathbf z=\mathbf P\mathbf u,\qquad z_i\leftarrow z_i+\chi_{noise}\,\mathcal N(\mu_i,\sigma_i),\quad \chi_{noise}\in\{0,1\}', '式 24：Gyro 与 MagMeter 的安装矩阵投影及可选分量噪声；χnoise=1 表示噪声开关开启。')
    add_paragraph(doc, '当前传感器噪声使用 C rand()；因此随机种子、模块调用顺序和开关路径都影响后续输出。H12、H19、H20 已覆盖 DSS 噪声开关与固定种子命令交叉，但不等于穷尽所有种子或噪声参数。[S5] [S16]')

    add_heading(doc, '7.3 DSS、STS、地影与 GPS', 2)
    add_formula_image(doc, 'dss', r'\mathbf u=\mathbf P_{DSS}\mathbf s_B,\;u_z>0:\quad \tilde u_x=u_x/u_z,\;\tilde u_y=u_y/u_z,\;\alpha_x=\operatorname{atan2}(\tilde u_x,1),\;\alpha_y=\operatorname{atan2}(\tilde u_y,1)', '式 25：当前 DSS 的投影、前向半空间判定、归一化和角测量。')
    add_formula_image(doc, 'shadow', r'\alpha_{limb}=\arccos\frac{R_e}{\|\mathbf r\|},\qquad \mathrm{shadow}=\left[\angle(\mathbf s_{GCI},\mathbf r_{GCI})>\alpha_{limb}+\frac{\pi}{2}\right]', '式 26：当前 isEarthShadow 的几何判定。')
    add_paragraph(doc, 'DSS 有效性在当前模型中使用 measure 与固定参考向量 (0,0,1) 的夹角；若 valid_angle_threshold < angle，则 flag=1，否则由地影结果决定。H11 的调用点证据确认参考向量不是零向量，也不是太阳 GCI 向量。[S5] [S18]')
    add_paragraph(doc, 'STS 按更新周期合成测量四元数，之后以太阳排除与地球视场几何设置有效标志；GPS 则复制当前 GCI 位置、速度和六个时间分量，并改变其初始化标志。[S5]')


def add_output_validation_section(doc):
    add_heading(doc, '8. 输出协议、原 ELF 证据与当前验证范围', 1)
    add_heading(doc, '8.1 main telemetry 与 IPC', 2)
    add_table(doc, ['输出对象', '当前大小/观察方式', '主要内容', '与模型的关系'], [
        ('CoreDynamic caller-state', '264 B / 每步', '调用者可见核心状态前缀', '反映动力学边界处的状态写入'),
        ('global y[33]', '264 B / 部分场景每步', '内部 33 个 double 状态', '直接观察积分状态；仅 11/21 场景有 gold'),
        ('main telemetry', '0x220 = 544 B / 每步', 'STS、Gyro、DSS、Mag、轮速、SADA、GPS、ECEF、轨道要素', 'UpdateMainOut 的二进制主输出'),
        ('IPC payload', '3000 B / 每步', '主遥测映射、float/u8 字段、端序/偏移结果', 'sendDynTele 的协议输出'),
        ('设备终态', 'H18–H20', 'RWheel、MTQ、Thruster、4 个向量', '多执行机构路径的对象状态验证'),
    ], widths=[3.2, 3.4, 6.2, 4.4], font_size=8.5)
    add_paragraph(doc, 'main telemetry 由固定偏移 memcpy/put_f64/put_i32 写入；DSS 角度在这里换为度。IPC 不是 main 的简单内存别名，还包括字段重排、浮点精度/类型与端序操作，所以它被独立采集和比较。[S13] [S14] [S15]')

    add_heading(doc, '8.2 H0–H20 的有限验证事实', 2)
    add_table(doc, ['项目', '当前数量/状态', '严谨解释'], [
        ('受控输入场景', '21 个（H0–H20）', '日期、轨道、姿态、柔性、传感器开关、种子和命令模板的有限组合'),
        ('长时域序列', '每场景 100 步 + 1000 步；共 42 条', '每个新场景先原 ELF 双采集，再运行恢复端逐步差分'),
        ('累计步进', '21 × (100 + 1000) = 23,100 步/每侧', '不是连续状态空间的全枚举'),
        ('所有场景共有观察', 'state 264 B + main 544 B + IPC 3000 B / 每步', '每一步按字节比较'),
        ('内部 y 覆盖', '11 / 21 场景', '其余 10 个场景未建 global y gold，不能宣称已比较'),
        ('最新默认门禁', '318 个可执行项、309 条 PASS、错误模式计数 0', 'H20 后 make clean && make selftest 的当前日志统计'),
    ], widths=[3.3, 5.0, 8.9], font_size=8.5)
    add_callout(doc, '正确的结论', '在列明的 H0–H20 输入模板、当前 Linux x86-64、当前运行时、当前严格 C11 编译选项、固定随机种子和已采集观察块中，恢复端输出与原 ELF 逐步逐字节一致。', fill='E2F0D9')
    add_callout(doc, '不能得出的结论', '有限场景 PASS 不等于任意 33 维连续状态、任意长度命令序列、任意随机种子、未采集内部对象、并发时序或其他平台都已经证明一致。', fill='FCE4D6')


def add_engineering_appendix(doc):
    add_heading(doc, '9. 位级行为等价的实现约束', 1)
    add_table(doc, ['约束', '当前工程做法', '为什么与模型贴合'], [
        ('编译', '-std=c11 -O0 -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off', '避免重排、融合乘加和未审计优化改变浮点轨迹'),
        ('qNaN', '对已观察路径保留符号位与 SSE 左操作数顺序', 'NaN 的符号/传播是 ABI 可见结果，不可用普通代数化简覆盖'),
        ('原 ELF', 'input/DynamicPackage.elf 只读 0400；调试仅针对 exec_copy', '金标保持不变，诊断不污染原二进制'),
        ('gold', '原 ELF 两次独立采集、逐文件 cmp/sha256', '先确认被比较对象本身可重复，再解释恢复端差异'),
        ('回归', '新场景 100/1000 PASS 后才进入 Makefile selftest', '阻止局部修复破坏既有场景'),
    ], widths=[2.6, 6.4, 8.2], font_size=8.4)
    add_paragraph(doc, '最重要的工程原则是：只有原 ELF 证据支持的特殊值、NaN 修正或传感器规则才能进入恢复端；不存在“为了让更多测试通过而猜测泛化规则”的允许路径。[S1] [S16] [S18]')

    add_heading(doc, '10. 函数—算法—证据索引', 1)
    add_table(doc, ['算法主题', '主实现函数', '源码/证据入口'], [
        ('状态总装', 'dp_dyn_main_recovered_step', 'src/dynamic_main_bridge.c [S12]'),
        ('33 维 ODE', 'dp_differential_equation_33', 'src/dynamic_dynamics.c [S8]'),
        ('刚柔耦合', 'dp_flex_dynamics_step', 'src/dynamic_flex.c [S9]'),
        ('RK4', 'dp_rk4_step_33 / dp_rk4_integrator_33', 'src/dynamic_rk4.c [S10]'),
        ('轨道', 'M2E / Elements2PosVel_M / orbit_dynamic', 'src/dynamic_orbit.c [S3]'),
        ('时间/坐标', 'Calc_JD / Calc_theta_GMST / GCI2ECEF', 'src/dynamic_time.c [S2]'),
        ('环境', 'dp_sun_vector / dp_magnetic_vector', 'src/dynamic_environment.c [S4]'),
        ('设备', 'dp_update_device_control', 'src/dynamic_devices.c [S6]'),
        ('传感器', 'dp_update_device_measure_recovered', 'src/dynamic_sensors.c [S5]'),
        ('遥测/IPC', 'dp_update_main_out_pack / dp_send_dyn_tele', 'src/dynamic_telemetry.c、dynamic_ipc_telemetry.c [S14][S15]'),
        ('验证矩阵', 'H0–H20 / full selftest', 'analysis/coverage_inventory [S16][S17]'),
    ], widths=[3.0, 6.0, 8.2], font_size=8.3)

    add_heading(doc, '11. 继续扩展时的审计模板', 1)
    add_paragraph(doc, '任何新的输入组合都应先被定义为有限、有效、可重放的场景，而不是把无界浮点 fuzz 直接接入默认门禁。推荐步骤如下：')
    for text in [
        '从现有矩阵选择一个尚未与既有场景重复的维度组合，例如新 LCG 命令种子 × 非零柔性 × 传感器开关。',
        '复制已验证的原 ELF 探针模板，使用独立 gold 文件名；只在调试执行副本运行。',
        '分别进行 100 步和 1000 步的原 ELF 双采集，逐文件 cmp 与 sha256 一致后才接受 gold。',
        '恢复端比较器必须同步驱动初始化、命令发生器、步数和 gold 文件名；比较块不能超过原 ELF 实际采集范围。',
        '若出现分叉，定位第一步/第一块后再进行反汇编和隔离运行时诊断；禁止无证据的全局修正。',
        '仅在两个时域都 PASS 后接入 Makefile，执行 make clean && make selftest，更新覆盖矩阵、连续日志和 Git。',
    ]:
        add_bullet(doc, text)

    add_heading(doc, '参考与工程证据', 1)
    refs = [
        ('[S1]', 'src/dynamic_math.c — 向量、矩阵、四元数、3×3 逆、旋转和浮点顺序。'),
        ('[S2]', 'src/dynamic_time.c — JD、GMST、岁差、GCI/ECEF 和时间推进。'),
        ('[S3]', 'src/dynamic_orbit.c — 开普勒方程、轨道要素、二体/J2/J3/J4 和外力。'),
        ('[S4]', 'src/dynamic_environment.c — 太阳、IGRF 球谐地磁、局地坐标与气动力矩辅助。'),
        ('[S5]', 'src/dynamic_sensors.c — STS、Gyro、DSS、MagMeter、GPS、地影和高斯噪声。'),
        ('[S6]', 'src/dynamic_devices.c — 飞轮、MTQ、推力器、SADA 和设备顺序。'),
        ('[S7]', 'src/dynamic_torque.c — 飞轮反作用、磁力矩、重力梯度力矩。'),
        ('[S8]', 'src/dynamic_dynamics.c — 33 维方程与总角动量装配。'),
        ('[S9]', 'src/dynamic_flex.c — 有效惯量、刚柔 RHS 与模态方程。'),
        ('[S10]', 'src/dynamic_rk4.c — RK4 阶段和四元数单位化。'),
        ('[S12]', 'src/dynamic_main_bridge.c — 主步端到端数据流。'),
        ('[S14]', 'src/dynamic_telemetry.c — 544 B 主遥测封包。'),
        ('[S15]', 'src/dynamic_ipc_telemetry.c — IPC 序列化、端序与共享内存。'),
        ('[S16]', 'analysis/coverage_inventory/long_horizon_matrix_h0_h1.md — H0–H20 覆盖矩阵。'),
        ('[S17]', 'analysis/coverage_inventory/full_clean_selftest_with_h0_to_h20_long_horizon_gates.log — H20 后严格回归。'),
        ('[S18]', 'analysis/time_orbit/h0_thousand_nan_sign_investigation.md — H0/H11–H20 连续取证。'),
    ]
    add_table(doc, ['编号', '工程内一手来源'], refs, widths=[1.3, 15.9], font_size=8.5)
    add_callout(doc, '最终边界声明', '本文档说明的是可审计的离线行为重建。它不公开或推定原始源代码，也不构成任意输入、所有平台或安全关键用途的 100% 等价保证。', fill='FCE4D6')


def build_doc():
    doc = Document()
    section = doc.sections[0]
    section.top_margin = Cm(1.6)
    section.bottom_margin = Cm(1.55)
    section.left_margin = Cm(1.65)
    section.right_margin = Cm(1.65)
    create_styles(doc)

    footer = section.footer.paragraphs[0]
    footer.text = ''
    add_page_number(footer)
    header = section.header.paragraphs[0]
    header.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    run = header.add_run('DynamicPackage 模型贴合算法说明  |  离线行为等价重建')
    set_run_font(run, size=8, color='7F7F7F')

    add_cover(doc)
    add_contents(doc)
    add_single_satellite_model_section(doc)
    add_system_section(doc)
    add_math_attitude_section(doc)
    add_time_environment_section(doc)
    add_orbit_section(doc)
    add_dynamics_flex_section(doc)
    add_rk4_section(doc)
    add_devices_sensors_section(doc)
    add_output_validation_section(doc)
    add_engineering_appendix(doc)

    doc.core_properties.title = 'DynamicPackage 单星模型、算法、符号与验证说明（H0–H20）'
    doc.core_properties.subject = 'Linux x86-64 PIE ELF 行为等价重建的算法与验证文档'
    doc.core_properties.author = 'Manus AI'
    doc.core_properties.comments = '仅限离线研究与仿真。'
    doc.save(OUT)
    print(OUT)


if __name__ == '__main__':
    build_doc()
