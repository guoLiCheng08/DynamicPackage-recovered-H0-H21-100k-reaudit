#!/usr/bin/env python3
"""静态 angr 反编译试验：不执行 DynamicPackage ELF。"""
from __future__ import annotations

import json
import logging
from pathlib import Path

import angr

ROOT = Path(__file__).resolve().parent.parent
BINARY = ROOT / "input" / "DynamicPackage.elf"
OUT = ROOT / "analysis" / "angr_trial"
TARGETS = [
    "CoreDynamic",
    "dynamics_flex",
    "orbit_dynamic",
    "SetWheelAcc",
    "UpdateWheel",
    "UpdateGyro",
    "UpdateMagMeter",
    "UpdateMainOut",
    "sendDynTele",
]


def get_function(cfg: angr.analyses.CFGFast, name: str):
    funcs = cfg.kb.functions
    matches = [f for f in funcs.values() if f.name == name]
    if matches:
        return matches[0]
    symbol = cfg.project.loader.main_object.get_symbol(name)
    if symbol is not None:
        return funcs.get(symbol.rebased_addr)
    return None


def main() -> None:
    logging.getLogger("angr").setLevel(logging.ERROR)
    OUT.mkdir(parents=True, exist_ok=True)
    project = angr.Project(str(BINARY), auto_load_libs=False)
    cfg = project.analyses.CFGFast(
        normalize=True,
        data_references=True,
        cross_references=True,
        force_complete_scan=False,
    )
    results = {
        "binary": str(BINARY),
        "base_address": hex(project.loader.main_object.mapped_base),
        "targets": {},
    }
    for name in TARGETS:
        function = get_function(cfg, name)
        entry: dict[str, object] = {"found": function is not None}
        if function is None:
            results["targets"][name] = entry
            continue
        entry.update(
            {
                "address": hex(function.addr),
                "size": function.size,
                "block_count": len(list(function.blocks)),
                "edge_count": function.graph.number_of_edges(),
                "calling_convention": str(function.calling_convention),
                "prototype": str(function.prototype),
            }
        )
        try:
            decompiler = project.analyses.Decompiler(function, cfg=cfg.model)
            code = decompiler.codegen.text if decompiler.codegen is not None else ""
            (OUT / f"{name}.c").write_text(code + "\n", encoding="utf-8")
            entry["decompiled"] = bool(code.strip())
            entry["pseudo_c_lines"] = len(code.splitlines())
        except Exception as exc:  # analysis failures are captured, not hidden
            entry["decompiled"] = False
            entry["decompiler_error"] = f"{type(exc).__name__}: {exc}"
        results["targets"][name] = entry
    (OUT / "summary.json").write_text(
        json.dumps(results, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    print(json.dumps(results, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
