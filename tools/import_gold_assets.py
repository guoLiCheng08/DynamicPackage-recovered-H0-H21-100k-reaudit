#!/usr/bin/env python3
"""从 DynamicPackage 交接 ZIP 导入原 ELF 金标数据。

默认导入 analysis 下的金标二进制（`.bin`）和名字以 `gold_` 开头的文件，
并保留相对目录；不会覆盖已有文件，适合在测试前准备离线回归数据。
"""

import argparse
import io
import zipfile
from pathlib import Path


def find_handoff_archive(outer: zipfile.ZipFile) -> bytes:
    names = [name for name in outer.namelist()
             if name.lower().endswith(".zip") and "h0-h21" in name.lower()]
    if not names:
        raise RuntimeError("外层 ZIP 中未找到 H0-H21 交接包")
    return outer.read(sorted(names)[-1])


def main() -> int:
    parser = argparse.ArgumentParser(description="导入 DynamicPackage ELF 金标")
    parser.add_argument("archive", type=Path, help="外层分析 ZIP")
    parser.add_argument("--target", type=Path, default=Path("."),
                        help="c_modle 根目录，默认当前目录")
    args = parser.parse_args()

    imported = 0
    skipped = 0
    with zipfile.ZipFile(args.archive) as outer:
        with zipfile.ZipFile(io.BytesIO(find_handoff_archive(outer))) as inner:
            for info in inner.infolist():
                path = Path(info.filename)
                if (path.is_absolute() or ".." in path.parts or
                        "analysis" not in path.parts or info.is_dir() or
                        not (path.name.startswith("gold_") or
                             path.suffix.lower() == ".bin")):
                    continue
                relative = Path(*path.parts[path.parts.index("analysis"):])
                destination = args.target / relative
                if destination.exists():
                    skipped += 1
                    continue
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(inner.read(info))
                imported += 1

    print(f"result=pass")
    print(f"gold_imported={imported}")
    print(f"gold_skipped_existing={skipped}")
    print(f"target={args.target.resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
