from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import re
import sys

if len(sys.argv) != 3:
    raise SystemExit('usage: normalize_docx_wps_fonts.py INPUT.docx OUTPUT.docx')

src = Path(sys.argv[1]).resolve()
dst = Path(sys.argv[2]).resolve()
if not src.is_file():
    raise FileNotFoundError(src)
if dst.exists():
    dst.unlink()


def transform(name: str, data: bytes) -> bytes:
    if not name.endswith('.xml'):
        return data
    text = data.decode('utf-8')
    if name == 'word/settings.xml':
        text = re.sub(r'<m:mathPr>.*?</m:mathPr>', '', text, flags=re.DOTALL)
        text = text.replace('w:eastAsia="ja-JP"', 'w:eastAsia="zh-CN"')
    for old in ('Noto Serif CJK SC', 'Noto Sans CJK SC', 'DejaVu Sans Mono', 'Courier', 'Cambria Math'):
        text = text.replace(old, 'Microsoft YaHei')
    return text.encode('utf-8')

with ZipFile(src, 'r') as zin, ZipFile(dst, 'w', ZIP_DEFLATED) as zout:
    for info in zin.infolist():
        zout.writestr(info, transform(info.filename, zin.read(info.filename)))

print(dst)
