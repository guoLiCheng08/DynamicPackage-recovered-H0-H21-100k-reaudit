#!/usr/bin/env python3
"""Build an evidence-first action queue from the symbol coverage ledger."""
from __future__ import annotations

import csv
from pathlib import Path

OUT = Path(__file__).resolve().parent

P0_SURFACE = {
    "DynamicDllInit", "Update_Telemetry_Frame_1", "Update_Telemetry_Frame_2",
    "Set_Algorithm_Telemetry_Frame_1", "Set_Algorithm_Telemetry_Frame_2",
    "Analyze_Command", "Command_Execute", "Algorithm_Command_Execute",
    "DYN_Command_Execute", "getDeskCommand", "dyn_init_array",
}
P1_PHYSICS = {
    "Pnm", "gh_change", "Calc_InertialMagneticVector", "GetInertialMag",
    "UpdateExternalForce", "FrontalArea", "AerodynamicTorque", "isEarthShadow",
    "MagneticVector", "Sun", "GravityGradientTorque",
}
P1_INITIALIZATION = {
    "Gyro_Init", "MagTorque_Init", "MagMeter_Init", "STS_Init", "DSS_Init",
    "Thruster_Init", "Wheel_Init", "SatParaInit", "TorqueInit", "dyn_init",
    "Update_sat_inertia", "SetInertiaTensor", "SetSatInertiaTensor", "SetSpacecraftMass",
}


def group_for(name: str) -> str:
    if name in P0_SURFACE:
        return "公开初始化/命令/遥测封装"
    if name in P1_PHYSICS:
        return "环境、磁场与外力模型"
    if name in P1_INITIALIZATION:
        return "设备/卫星初始化与惯量状态"
    if any(x in name.lower() for x in ("wheel", "torque", "thruster", "sada", "gyro", "mag", "sts", "dss")):
        return "设备、传感器或力矩"
    if any(x in name.lower() for x in ("orbit", "attitude", "inertial", "geo", "dynamics")):
        return "姿态、轨道与动力学"
    return "其他公开行为"


def action_for(status: str, verification: str) -> tuple[str, str]:
    if status == "NO_RECOVERED_EVIDENCE":
        return (
            "反汇编审计 → 重复原 ELF GDB 金标 → 最小 C11 实现 → 位级比较器 → 接入 selftest",
            "尚无恢复端源码/别名/测试证据；先建立 ABI 与受控金标。",
        )
    if status == "SOURCE_REFERENCE_ONLY":
        return (
            "确认注释/调用点所指内部实现 → 独立审计原 ABI → 导出同名包装或独立实现 → 金标与 selftest",
            "只有源码文字引用；不能视为实现或验证。",
        )
    if status == "INTERNAL_ALIAS_CANDIDATE":
        return (
            "验证内部 dp_* 帮助函数与原公开 ABI 是否同参同副作用 → 编写公开 ABI 包装/实现 → 单独原 ELF 金标与 selftest",
            "存在人工审阅的语义候选映射，但尚未计入直接 ABI 覆盖。",
        )
    if status == "LOCAL_ONLY_NAME_MATCH":
        return (
            "审计 local 可见性是否符合原公共 ABI → 必要时导出包装 → 金标与 selftest",
            "存在同名本地符号，但恢复库未提供同名全局 ABI。",
        )
    if status == "DIRECT_ABI_NAME_MATCH" and verification == "DIRECT_IMPLEMENTATION_NO_NAMED_SELFTEST":
        return (
            "补采原 ELF 正常/边界金标 → 新建独立比较器 → 接入 Makefile selftest",
            "已有同名 ABI，但台账未找到按原名称关联且接入 selftest 的比较器。",
        )
    if status == "DIRECT_ABI_NAME_MATCH":
        return (
            "补齐缺失的探针、金标或边界分支证据；维持现有门禁",
            "已有直接实现与至少一项严格回归证据；尚不表示全输入等价。",
        )
    return ("人工审阅", "需要人工分类。")


def main() -> None:
    with (OUT / "symbol_coverage_ledger.csv").open(newline="") as fp:
        rows = [r for r in csv.DictReader(fp) if r["scope"] == "DOMAIN_GLOBAL"]
    queue = []
    for r in rows:
        status = r["implementation_status"]
        verify = r["verification_status"]
        has_threefold = (
            int(r["gdb_probe_count"]) > 0 and
            int(r["gold_file_count"]) > 0 and
            int(r["selftest_named_comparator_count"]) > 0
        )
        if status == "DIRECT_ABI_NAME_MATCH" and verify == "DIRECT_SELFTEST_EVIDENCE" and has_threefold:
            # 已具备同名 ABI、原 ELF 探针、金标和 Makefile 门禁；不再列入待办。
            continue
        action, rationale = action_for(status, verify)
        name = r["original_symbol"]
        if name in P0_SURFACE and status == "NO_RECOVERED_EVIDENCE":
            priority = "P0"
        elif status in {"NO_RECOVERED_EVIDENCE", "SOURCE_REFERENCE_ONLY"}:
            priority = "P1"
        elif status in {"INTERNAL_ALIAS_CANDIDATE", "LOCAL_ONLY_NAME_MATCH"}:
            priority = "P1"
        elif status == "DIRECT_ABI_NAME_MATCH":
            priority = "P3"
        else:
            priority = "P3"
        queue.append({
            "priority": priority,
            "subsystem": group_for(name),
            "original_symbol": name,
            "implementation_status": status,
            "verification_status": verify,
            "reviewed_internal_alias_candidate": r["reviewed_internal_alias_candidate"],
            "recommended_next_action": action,
            "rationale": rationale,
        })
    rank = {"P0": 0, "P1": 1, "P2": 2, "P3": 3}
    queue.sort(key=lambda x: (rank[x["priority"]], x["subsystem"], x["original_symbol"]))
    fields = [
        "priority", "subsystem", "original_symbol", "implementation_status",
        "verification_status", "reviewed_internal_alias_candidate",
        "recommended_next_action", "rationale",
    ]
    with (OUT / "coverage_action_queue.csv").open("w", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=fields)
        writer.writeheader()
        writer.writerows(queue)
    counts = {p: sum(x["priority"] == p for x in queue) for p in rank}
    with (OUT / "coverage_action_queue.md").open("w") as fp:
        fp.write("# DynamicPackage Evidence-First Recovery Action Queue\n\n")
        fp.write("> Queue ordering is evidence-based. It identifies the next proof-producing work, not the physical or safety importance of a function. Every item must preserve the offline-only restriction and close the loop with static audit, repeated original-ELF gold, minimal C11 code, strict comparator, Makefile gate, and documentation.\n\n")
        fp.write(f"Active items: **{len(queue)}** — P0: **{counts['P0']}**, P1: **{counts['P1']}**, P2: **{counts['P2']}**, P3: **{counts['P3']}**.\n\n")
        fp.write("| Priority | Subsystem | Symbol | Current evidence | Next action |\n|---|---|---|---|---|\n")
        for item in queue:
            evidence = item["implementation_status"]
            if item["reviewed_internal_alias_candidate"]:
                evidence += f"; alias: `{item['reviewed_internal_alias_candidate']}`"
            fp.write(
                f"| {item['priority']} | {item['subsystem']} | `{item['original_symbol']}` | {evidence} | {item['recommended_next_action']} |\n"
            )

if __name__ == "__main__":
    main()
