#!/usr/bin/env python3
"""对 CoreDynamic 和 dynamics_flex 运行 angr 变量恢复和类型约束求解；不执行样本。"""
from __future__ import annotations

import json
import logging
from pathlib import Path

import angr

ROOT = Path(__file__).resolve().parent.parent
BINARY = ROOT / "input" / "DynamicPackage.elf"
OUT = ROOT / "analysis" / "angr_trial"
TARGETS = ["CoreDynamic", "dynamics_flex", "SetWheelAcc", "UpdateWheel"]


def find_function(cfg, name):
    for function in cfg.kb.functions.values():
        if function.name == name:
            return function
    return None


def main() -> None:
    logging.getLogger("angr").setLevel(logging.ERROR)
    project = angr.Project(str(BINARY), auto_load_libs=False)
    cfg = project.analyses.CFGFast(normalize=True, data_references=True)
    report = {}
    for name in TARGETS:
        function = find_function(cfg, name)
        entry = {"found": function is not None}
        if function is None:
            report[name] = entry
            continue
        try:
            recovery = project.analyses.VariableRecoveryFast(function)
            variables = list(project.kb.variables[function.addr].get_variables())
            entry.update(
                {
                    "variable_count": len(variables),
                    "stack_variables": sum(v.__class__.__name__ == "SimStackVariable" for v in variables),
                    "register_variables": sum(v.__class__.__name__ == "SimRegisterVariable" for v in variables),
                    "memory_variables": sum(v.__class__.__name__ == "SimMemoryVariable" for v in variables),
                    "constraint_groups": len(recovery.type_constraints),
                    "constraint_count": sum(len(v) for v in recovery.type_constraints.values()),
                }
            )
            attribute_names = [a for a in dir(recovery) if "type" in a.lower() or "var" in a.lower()]
            entry["recovery_type_variable_attributes"] = attribute_names
            # angr 9.3 的公开变量到类型变量映射为 var_to_typevars；
            # typevars 是内部容器，不符合 Typehoon 的字典接口。
            mapping = getattr(recovery, "var_to_typevars", None)
            typehoon = project.analyses.Typehoon(
                recovery.type_constraints,
                recovery.func_typevar,
                var_mapping=mapping,
            )
            typed = 0
            structs = []
            if mapping:
                for variable, typevars in mapping.items():
                    for typevar in typevars:
                        type_ = typehoon.simtypes_solution.get(typevar)
                        if type_ is not None:
                            typed += 1
                            type_name = type(type_).__name__
                            if type_name == "SimStruct":
                                structs.append(str(type_))
            entry.update(
                {
                    "type_solution_count": len(typehoon.simtypes_solution or {}),
                    "typed_variable_relations": typed,
                    "inferred_structs": sorted(set(structs))[:20],
                }
            )
        except Exception as exc:
            entry["error"] = f"{type(exc).__name__}: {exc}"
        report[name] = entry
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "type_recovery_summary.json").write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    print(json.dumps(report, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
