#!/usr/bin/env python3
"""为已知小函数注入原型，比较 angr 伪代码的类型显示效果；不执行样本。"""
from __future__ import annotations

from pathlib import Path
import logging

import angr
from angr.sim_type import SimTypeDouble, SimTypeFunction, SimTypePointer

ROOT = Path(__file__).resolve().parent.parent
BINARY = ROOT / "input" / "DynamicPackage.elf"
OUT = ROOT / "analysis" / "angr_trial" / "typed"


def function_by_name(cfg, name):
    return next(f for f in cfg.kb.functions.values() if f.name == name)


def set_prototype(function, project, args):
    prototype = SimTypeFunction(args=args, returnty=None)
    function.prototype = prototype.with_arch(project.arch)


def main() -> None:
    logging.getLogger("angr").setLevel(logging.ERROR)
    OUT.mkdir(parents=True, exist_ok=True)
    project = angr.Project(str(BINARY), auto_load_libs=False)
    cfg = project.analyses.CFGFast(normalize=True, data_references=True)

    set_wheel = function_by_name(cfg, "SetWheelAcc")
    update_wheel = function_by_name(cfg, "UpdateWheel")
    set_prototype(set_wheel, project, [SimTypePointer(SimTypeDouble())])
    set_prototype(update_wheel, project, [SimTypeDouble()])

    for function in (set_wheel, update_wheel):
        decompiler = project.analyses.Decompiler(function, cfg=cfg.model)
        text = decompiler.codegen.text if decompiler.codegen is not None else ""
        (OUT / f"{function.name}.c").write_text(text + "\n", encoding="utf-8")
        print(f"{function.name}: {function.prototype}")
        print(text.splitlines()[0] if text else "<empty>")


if __name__ == "__main__":
    main()
