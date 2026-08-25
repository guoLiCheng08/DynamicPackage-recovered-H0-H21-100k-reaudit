from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import re

ROOT = Path('/home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work')
SRC = ROOT / 'analysis' / 'documentation' / 'DynamicPackage_单星仿真_基础原理_逐式符号注释_Word自动更新目录_H0-H20.docx'
DST = ROOT / 'analysis' / 'documentation' / 'DynamicPackage_单星仿真_基础原理_逐式符号注释_WPS兼容_图片公式_H0-H20.docx'


def transform_xml(name: str, data: bytes) -> bytes:
    if not name.endswith('.xml'):
        return data
    text = data.decode('utf-8')

    # 公式已全部作为 PNG 嵌入。删除 python-docx 模板自动写入、但未使用的 OMML 默认 mathPr，
    # 避免 WPS 因尝试加载 Cambria Math 而进入数学对象兼容分支。
    if name == 'word/settings.xml':
        text = re.sub(r'<m:mathPr>.*?</m:mathPr>', '', text, flags=re.DOTALL)
        text = text.replace('w:eastAsia="ja-JP"', 'w:eastAsia="zh-CN"')

    # 避免依赖生成环境才存在的 Noto 字体；使用大多数 Windows/WPS 安装均包含的微软雅黑。
    # 公式本体为 PNG，因此其中的希腊字母、上下标、矩阵不会依赖客户端字体。
    text = text.replace('Noto Serif CJK SC', 'Microsoft YaHei')
    text = text.replace('Noto Sans CJK SC', 'Microsoft YaHei')
    text = text.replace('Cambria Math', 'Microsoft YaHei')
    return text.encode('utf-8')


def main():
    if not SRC.is_file():
        raise FileNotFoundError(SRC)
    if DST.exists():
        DST.unlink()

    with ZipFile(SRC, 'r') as zin, ZipFile(DST, 'w', compression=ZIP_DEFLATED) as zout:
        for info in zin.infolist():
            data = zin.read(info.filename)
            zout.writestr(info, transform_xml(info.filename, data))

    print(f'input={SRC}')
    print(f'output={DST}')


if __name__ == '__main__':
    main()
