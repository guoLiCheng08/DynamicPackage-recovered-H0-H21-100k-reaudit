#!/usr/bin/env python3
"""Build a conservative acceptance matrix for full observable-ELF equivalence.

The existing ledger establishes ABI/name, probe, gold and selftest discovery. This
script deliberately does not infer edge, alias, state-lifecycle or IPC coverage
from a normal-case pass; those fields begin as UNKNOWN until a dedicated witness
is recorded and reviewed.
"""
from __future__ import annotations

import csv
from pathlib import Path

OUT = Path(__file__).resolve().parent
LEDGER = OUT / "symbol_coverage_ledger.csv"
MATRIX = OUT / "full_equivalence_acceptance_matrix.csv"
SUMMARY = OUT / "full_equivalence_acceptance_summary.md"

FIELDS = [
    "original_symbol", "scope", "implementation_status", "verification_status",
    "public_abi_closed", "normal_gold_and_gate", "repeatability_witness",
    "edge_or_error_abi", "state_lifecycle_or_alias", "external_io_or_ipc",
    "cross_scenario_evidence", "acceptance_status", "next_required_evidence",
]


def main() -> None:
    rows: list[dict[str, str]] = []
    with LEDGER.open(newline="") as fp:
        for source in csv.DictReader(fp):
            if source["scope"] != "DOMAIN_GLOBAL":
                continue
            direct = source["implementation_status"] == "DIRECT_ABI_NAME_MATCH"
            normal = direct and source["verification_status"] == "DIRECT_SELFTEST_EVIDENCE"
            repeat = normal and int(source["gdb_probe_count"]) > 0 and int(source["gold_file_count"]) > 1
            # No automatic inference is permitted for exceptional, stateful or
            # external-I/O behavior. A normal gold pass is evidence only for the
            # specifically probed scenario.
            edge = "UNKNOWN"
            state = "UNKNOWN"
            external = "NOT_APPLICABLE_OR_UNREVIEWED"
            cross = "UNKNOWN"
            if all((direct, normal, repeat)) and edge == "YES" and state == "YES" and cross == "YES":
                status = "ACCEPTED_FULL_EVIDENCE"
                next_step = "none"
            elif not direct:
                status = "ABI_OPEN"
                next_step = "recover exact public ABI, then capture a minimal normal-case gold"
            elif not normal:
                status = "NORMAL_CASE_OPEN"
                next_step = "capture repeatable original-ELF gold and wire a named strict comparator"
            else:
                status = "ROBUSTNESS_EVIDENCE_OPEN"
                next_step = "add explicit edge/error, alias/state and cross-scenario witnesses; review IPC/stdout if observable"
            rows.append({
                "original_symbol": source["original_symbol"],
                "scope": source["scope"],
                "implementation_status": source["implementation_status"],
                "verification_status": source["verification_status"],
                "public_abi_closed": "YES" if direct else "NO",
                "normal_gold_and_gate": "YES" if normal else "NO",
                "repeatability_witness": "YES" if repeat else "NO",
                "edge_or_error_abi": edge,
                "state_lifecycle_or_alias": state,
                "external_io_or_ipc": external,
                "cross_scenario_evidence": cross,
                "acceptance_status": status,
                "next_required_evidence": next_step,
            })

    with MATRIX.open("w", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=FIELDS)
        writer.writeheader()
        writer.writerows(rows)

    counts: dict[str, int] = {}
    for row in rows:
        counts[row["acceptance_status"]] = counts.get(row["acceptance_status"], 0) + 1
    abi = sum(row["public_abi_closed"] == "YES" for row in rows)
    normal = sum(row["normal_gold_and_gate"] == "YES" for row in rows)
    repeat = sum(row["repeatability_witness"] == "YES" for row in rows)
    full = sum(row["acceptance_status"] == "ACCEPTED_FULL_EVIDENCE" for row in rows)
    with SUMMARY.open("w") as fp:
        fp.write("# DynamicPackage 全等价验收矩阵摘要\n\n")
        fp.write("> **严格口径。** `ACCEPTED_FULL_EVIDENCE` 只有在公开 ABI、重复正常金标、异常 ABI、状态/别名、外部 I/O 或 IPC（适用时）及跨场景证据均有明确见证时才可填入。脚本绝不从普通 PASS 自动推断这些字段。\n\n")
        fp.write(f"- 领域函数分母：**{len(rows)}**\n")
        fp.write(f"- 公开 ABI 已闭合：**{abi}/{len(rows)} ({abi / len(rows) * 100:.1f}%)**\n")
        fp.write(f"- 已有同名正常金标和严格门禁：**{normal}/{len(rows)} ({normal / len(rows) * 100:.1f}%)**\n")
        fp.write(f"- 自动可识别的重复金标见证：**{repeat}/{len(rows)} ({repeat / len(rows) * 100:.1f}%)**\n")
        fp.write(f"- 满足全证据验收：**{full}/{len(rows)} ({full / len(rows) * 100:.1f}%)**\n\n")
        fp.write("| 当前验收状态 | 数量 |\n|---|---:|\n")
        for key in sorted(counts):
            fp.write(f"| `{key}` | {counts[key]} |\n")
        fp.write("\n完整逐符号要求见 `full_equivalence_acceptance_matrix.csv`。\n")


if __name__ == "__main__":
    main()
