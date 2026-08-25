#!/usr/bin/env python3
"""Summarize the evidence backlog into work-estimation buckets.

Estimates are expressed as engineering-day ranges per closed evidence loop, not
calendar commitments. They include static ABI audit, repeated gold acquisition,
minimal C11 recovery or wrapper work, comparator, Makefile wiring, documentation,
and one full clean selftest. They do not imply full input-space equivalence.
"""
from __future__ import annotations

import csv
from collections import Counter, defaultdict
from pathlib import Path

OUT = Path(__file__).resolve().parent

# Conservative per-symbol evidence-loop ranges. Complex scientific/environment
# functions are manually elevated; the script records the assumptions explicitly.
RANGES = {
    "DIRECT_IMPLEMENTATION_NO_NAMED_SELFTEST": (0.25, 0.75),
    "ALIAS_CANDIDATE_NO_NAMED_SELFTEST": (0.5, 1.5),
    "EVIDENCE_EXISTS_MAPPING_UNRESOLVED": (1.0, 3.0),
    "NO_SYMBOL_LEVEL_EVIDENCE": (1.0, 3.0),
}
COMPLEX_NO_EVIDENCE = {
    "Pnm", "gh_change", "UpdateExternalForce", "AerodynamicTorque", "FrontalArea",
    "GetInertialMag", "Get_Orbit_RV", "LocalGeo2LVLH", "R_ECEF2LocalGeo",
    "get_attitude", "get_Inertial2Body", "get_Body2Inertial", "Update_sat_inertia",
}
P0_PUBLIC_SURFACE = {
    "DynamicDllInit", "Update_Telemetry_Frame_1", "Update_Telemetry_Frame_2",
    "Set_Algorithm_Telemetry_Frame_1", "Set_Algorithm_Telemetry_Frame_2",
    "Analyze_Command", "Command_Execute", "Algorithm_Command_Execute",
    "DYN_Command_Execute", "getDeskCommand", "dyn_init_array",
}


def fmt_range(lo: float, hi: float) -> str:
    return f"{lo:.1f}–{hi:.1f}"


def main() -> None:
    with (OUT / "coverage_action_queue.csv").open(newline="") as fp:
        queue = list(csv.DictReader(fp))

    # Direct ABI symbols with some selftest but lacking all three evidence layers
    # are tracked separately from active queue items.
    with (OUT / "symbol_coverage_ledger.csv").open(newline="") as fp:
        ledger = list(csv.DictReader(fp))
    direct_thickening = [
        r for r in ledger
        if r["scope"] == "DOMAIN_GLOBAL"
        and r["implementation_status"] == "DIRECT_ABI_NAME_MATCH"
        and not (
            int(r["gdb_probe_count"]) > 0
            and int(r["gold_file_count"]) > 0
            and int(r["selftest_named_comparator_count"]) > 0
        )
    ]

    packages: dict[str, list[dict[str, str]]] = defaultdict(list)
    for r in queue:
        packages[r["verification_status"]].append(r)
    packages["DIRECT_EVIDENCE_THICKENING"] = direct_thickening

    rows = []
    for status, items in sorted(packages.items()):
        if status == "DIRECT_EVIDENCE_THICKENING":
            low, high = 0.25, 1.0
            rationale = "同名直接 ABI 已存在；补缺失探针、金标或独立门禁。"
        else:
            if status.startswith("ALIAS_CANDIDATE"):
                low, high = RANGES["ALIAS_CANDIDATE_NO_NAMED_SELFTEST"]
            else:
                low, high = RANGES.get(status, (1.0, 3.0))
            rationale = "按当前实现/证据状态估算单符号闭环。"
        subtotal_lo = subtotal_hi = 0.0
        elevated = []
        for item in items:
            lo, hi = low, high
            if item.get("original_symbol") in COMPLEX_NO_EVIDENCE and status in {"NO_SYMBOL_LEVEL_EVIDENCE", "EVIDENCE_EXISTS_MAPPING_UNRESOLVED"}:
                lo, hi = 3.0, 8.0
                elevated.append(item["original_symbol"])
            subtotal_lo += lo
            subtotal_hi += hi
        rows.append({
            "work_package": status,
            "items": len(items),
            "single_symbol_engineering_days": fmt_range(low, high),
            "subtotal_engineering_days": fmt_range(subtotal_lo, subtotal_hi),
            "complex_items_elevated": ";".join(elevated),
            "assumption": rationale,
        })

    # Practical delivery increments, selected from the queue and ledger.
    p0 = [r for r in queue if r["priority"] == "P0"]
    p1 = [r for r in queue if r["priority"] == "P1"]
    p2 = [r for r in queue if r["priority"] == "P2"]
    no_evidence = [r for r in queue if r["implementation_status"] == "NO_RECOVERED_EVIDENCE"]
    aliases = [r for r in queue if r["implementation_status"] == "INTERNAL_ALIAS_CANDIDATE"]
    source_refs = [r for r in queue if r["implementation_status"] == "SOURCE_REFERENCE_ONLY"]
    direct_no_named = [r for r in queue if r["verification_status"] == "DIRECT_IMPLEMENTATION_NO_NAMED_SELFTEST"]
    quick_lo = len(p0) * 0.5 + len(direct_no_named) * 0.25
    quick_hi = len(p0) * 1.5 + len(direct_no_named) * 0.75
    abi_lo = quick_lo + len(aliases) * 0.5 + len(source_refs) * 1.0
    abi_hi = quick_hi + len(aliases) * 1.5 + len(source_refs) * 3.0
    strong_lo = abi_lo + len(direct_thickening) * 0.25 + sum(
        3.0 if r["original_symbol"] in COMPLEX_NO_EVIDENCE else 1.0 for r in no_evidence
    )
    strong_hi = abi_hi + len(direct_thickening) * 1.0 + sum(
        8.0 if r["original_symbol"] in COMPLEX_NO_EVIDENCE else 3.0 for r in no_evidence
    )
    scenarios = [
        {
            "target": "快速补齐公开封装和现有直接接口门禁",
            "scope": f"P0 {len(p0)} 项 + 直接实现但缺同名门禁 {len(direct_no_named)} 项；不含复杂科学函数",
            "engineering_days": fmt_range(quick_lo, quick_hi),
            "expected_effect": "提升公开 ABI/门禁覆盖，不能显著提高全分支或环境模型等价证据。",
        },
        {
            "target": "公开 ABI 闭合到别名/引用层",
            "scope": f"在快速批次基础上闭合 {len(aliases)} 个内部别名候选及 {len(source_refs)} 个仅引用项",
            "engineering_days": fmt_range(abi_lo, abi_hi),
            "expected_effect": "消除大量“有内部逻辑、无公开 ABI 证据”的缺口。",
        },
        {
            "target": "扩大强证据层并处理无证据/高复杂函数",
            "scope": f"前两批 + {len(direct_thickening)} 个直接接口证据加厚 + {len(no_evidence)} 个无证据项（含复杂环境/动力学项）",
            "engineering_days": fmt_range(strong_lo, strong_hi),
            "expected_effect": "显著增加探针、金标和门禁闭环；仍不是全输入空间证明。",
        },
    ]

    with (OUT / "effort_backlog_summary.csv").open("w", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    with (OUT / "effort_scenarios.csv").open("w", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=list(scenarios[0]))
        writer.writeheader()
        writer.writerows(scenarios)
    with (OUT / "effort_estimate.md").open("w") as fp:
        fp.write("# DynamicPackage Recovery Effort Estimate\n\n")
        fp.write("> **Estimate boundary.** An engineering day is a planning unit for one focused implementation stream; it is not a calendar commitment. Every range includes the full proof loop: static ABI audit, repeated original-ELF gold, minimal C11 implementation/wrapper, strict comparator, Makefile gate, documentation, and clean selftest. No range proves unrestricted input-space equivalence.\n\n")
        fp.write("## Backlog by Evidence State\n\n")
        fp.write("| Work package | Items | Baseline per-symbol effort (engineering days) | Subtotal (engineering days) |\n|---|---:|---:|---:|\n")
        for row in rows:
            fp.write(f"| `{row['work_package']}` | {row['items']} | {row['single_symbol_engineering_days']} | {row['subtotal_engineering_days']} |\n")
        fp.write("\nComplex no-evidence items are elevated to 3.0–8.0 engineering days each: `" + "`, `".join(sorted(COMPLEX_NO_EVIDENCE)) + "`.\n\n")
        fp.write("## Delivery Scenarios\n\n")
        fp.write("| Target | Scope | Estimated focused engineering effort | Outcome boundary |\n|---|---|---:|---|\n")
        for s in scenarios:
            fp.write(f"| {s['target']} | {s['scope']} | {s['engineering_days']} | {s['expected_effect']} |\n")
        fp.write("\n## Queue Mix\n\n")
        fp.write(f"- Active action-queue items: **{len(queue)}**.\n")
        fp.write(f"- P0: **{len(p0)}**; P1: **{len(p1)}**; P2: **{len(p2)}**.\n")
        fp.write(f"- No recovered evidence: **{len(no_evidence)}**; internal alias candidates: **{len(aliases)}**; source-reference-only: **{len(source_refs)}**; direct ABI with no named selftest: **{len(direct_no_named)}**.\n")
        fp.write(f"- Direct ABI evidence-thickening backlog: **{len(direct_thickening)}**.\n")

if __name__ == "__main__":
    main()
