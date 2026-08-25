#!/usr/bin/env python3
"""静态导出 CoreDynamic / dynamics_flex 的 CFG 调用和数据对象依赖；不执行样本。"""
from __future__ import annotations

import json
import logging
from pathlib import Path

import angr

ROOT = Path(__file__).resolve().parent.parent
BINARY = ROOT / "input" / "DynamicPackage.elf"
OUT = ROOT / "analysis" / "core_slices"
TARGETS = ("CoreDynamic", "dynamics_flex")
KNOWN_GLOBALS = {
    "Sat": (0x6156E0, 4216),
    "SADA": (0x616980, 104),
    "H_w_B": (0x615650, 16),
    "L_c_B": (0x615660, 16),
    "J_c_B": (0x615690, 24),
    "J_c_B_inv": (0x615670, 24),
    "SatTorque": (0x616760, 360),
}


def find_function(cfg, name):
    return next((f for f in cfg.kb.functions.values() if f.name == name), None)


def classify_global(address):
    for name, (base, size) in KNOWN_GLOBALS.items():
        if base <= address < base + size:
            return {"name": name, "offset": hex(address - base)}
    return None


def main() -> None:
    logging.getLogger("angr").setLevel(logging.ERROR)
    OUT.mkdir(parents=True, exist_ok=True)
    project = angr.Project(str(BINARY), auto_load_libs=False)
    cfg = project.analyses.CFGFast(normalize=True, data_references=True, cross_references=True)
    report = {}

    for target in TARGETS:
        function = find_function(cfg, target)
        if function is None:
            report[target] = {"found": False}
            continue
        calls = []
        for site in sorted(function.get_call_sites()):
            destination = function.get_call_target(site)
            target_name = None
            if destination is not None:
                callee = cfg.kb.functions.get(destination)
                target_name = callee.name if callee is not None else None
            calls.append(
                {
                    "site": hex(site),
                    "target": hex(destination) if destination is not None else None,
                    "target_name": target_name,
                }
            )

        blocks = []
        globals_used = []
        for block in function.blocks:
            instructions = []
            for insn in block.capstone.insns:
                instruction = {"address": hex(insn.address), "mnemonic": insn.mnemonic, "op_str": insn.op_str}
                instructions.append(instruction)
                # Pull all hexadecimal immediates as a conservative address candidate list.
                for token in insn.op_str.replace("+", " ").replace("]", " ").replace("[", " ").split():
                    if token.startswith("0x"):
                        try:
                            candidate = int(token.rstrip(","), 16)
                        except ValueError:
                            continue
                        reference = classify_global(candidate)
                        if reference is not None:
                            globals_used.append({"instruction": hex(insn.address), **reference})
            blocks.append({"address": hex(block.addr), "size": block.size, "instructions": instructions})

        report[target] = {
            "found": True,
            "address": hex(function.addr),
            "size": function.size,
            "call_sites": calls,
            "global_references": globals_used,
            "blocks": blocks,
        }

    (OUT / "core_dependency_slices.json").write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    for name, content in report.items():
        lines = [f"# {name} 依赖切片", "", f"函数地址：`{content.get('address', 'N/A')}`，大小：`{content.get('size', 0)}` B。", "", "## 直接调用"]
        for call in content.get("call_sites", []):
            lines.append(f"- `{call['site']}` → `{call['target_name'] or call['target'] or '未解析'}`")
        lines.extend(["", "## 已识别全局对象引用"])
        seen = set()
        for item in content.get("global_references", []):
            key = (item["name"], item["offset"])
            if key not in seen:
                seen.add(key)
                lines.append(f"- `{item['name']}+{item['offset']}`（首次指令 `{item['instruction']}`）")
        (OUT / f"{name}_依赖切片.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
