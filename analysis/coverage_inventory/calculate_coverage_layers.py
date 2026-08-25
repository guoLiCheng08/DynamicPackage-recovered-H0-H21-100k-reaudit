#!/usr/bin/env python3
"""Calculate layered coverage metrics from the generated symbol ledger and selftest log."""
from __future__ import annotations

import csv
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent
LEDGER = OUT / "symbol_coverage_ledger.csv"
SELFTEST_LOG = Path("/home/ubuntu/dynamicpackage_audit_tar/reproduced_full_clean_selftest_2026-08-21.log")
MAKEFILE = ROOT / "Makefile"

# The workflow stages are expressly enumerated rather than inferred from function
# names. A stage is "isolated_verified" only when its named comparator is in the
# Makefile and the overall selftest run returned success. UpdateCoreDynInput is
# intentionally left indirect: evidence comes from integrated dyn_main paths only.
PIPELINE = [
    ("DynamicInit", "analysis/dynamic_init_global_original_gold_compare.c"),
    ("UpdateDeviceControl", "analysis/update_device_control_global_original_gold_compare.c"),
    ("UpdateCoreDynInput", "analysis/update_core_dyn_input_p4_gold_compare.c"),
    ("CoreDynamic", "analysis/time_orbit/core_dynamic_default_rk4_bridge_compare.c"),
    ("UpdateDeviceMeasure", "analysis/update_device_measure_global_original_gold_compare.c"),
    ("UpdateMainOut", "analysis/update_mainout_global_original_gold_compare.c"),
    ("sendDynTele", "analysis/time_orbit/send_dyn_tele_compare.c"),
    ("dyn_main", "analysis/dyn_main_global_step1_original_gold_compare.c"),
]

# These are observed, isolated SIGSEGV regression witnesses, not a denominator for
# all possible invalid ABI combinations.
DEATH_WITNESSES = [
    "vector_memcpy NULL-source P3 death test: PASS (SIGSEGV)",
    "vector_axpy NULL-source P3 death test: PASS (SIGSEGV)",
    "vector_axpby NULL-source P3 death test: PASS (SIGSEGV)",
    "quat_to_euler_angle_312 P3 death ABI: PASS (SIGSEGV)",
    "quat2matrix P3 death ABI: PASS (SIGSEGV)",
]


def yes(record: dict[str, str], field: str) -> bool:
    return record[field] == "YES"


def main() -> None:
    with LEDGER.open(newline="") as fp:
        records = list(csv.DictReader(fp))
    domain = [r for r in records if r["scope"] == "DOMAIN_GLOBAL"]
    direct = [r for r in domain if r["implementation_status"] == "DIRECT_ABI_NAME_MATCH"]
    aliases = [r for r in domain if r["implementation_status"] == "INTERNAL_ALIAS_CANDIDATE"]
    local = [r for r in domain if r["implementation_status"] == "LOCAL_ONLY_NAME_MATCH"]
    direct_selftests = [r for r in direct if int(r["selftest_named_comparator_count"]) > 0]
    direct_probe = [r for r in direct if int(r["gdb_probe_count"]) > 0]
    direct_gold = [r for r in direct if int(r["gold_file_count"]) > 0]
    direct_threefold = [
        r for r in direct
        if int(r["gdb_probe_count"]) > 0
        and int(r["gold_file_count"]) > 0
        and int(r["selftest_named_comparator_count"]) > 0
    ]
    log = SELFTEST_LOG.read_text(errors="replace")
    makefile = MAKEFILE.read_text(errors="replace")
    pipeline_rows = []
    for stage, comparator in PIPELINE:
        comparator_wired = comparator is not None and comparator in makefile
        status = "ISOLATED_SELFTEST_WIRED" if comparator_wired else "NO_WIRED_EVIDENCE"
        pipeline_rows.append({
            "stage": stage,
            "evidence_comparator": comparator or "dyn_main integrated paths only",
            "status": status,
        })
    isolated = sum(r["status"] == "ISOLATED_SELFTEST_WIRED" for r in pipeline_rows)
    death_pass = [x for x in DEATH_WITNESSES if x in log]
    dyn_main_scenarios = sum(
        line.startswith("dyn_main ") and "original-ELF compare: PASS" in line
        for line in log.splitlines()
    )
    result = {
        "method": {
            "symbol_denominator": "Original ELF non-runtime GLOBAL/WEAK defined FUNC symbols",
            "direct_abi": "Exact original function name exists as a global recovered-library function",
            "threefold_witness": "Direct ABI plus at least one original-symbol-named GDB probe, normalized gold filename, and comparator named in Makefile; this is still finite-input evidence",
            "pipeline": "Eight explicitly enumerated top-level stages; isolated metric requires a named comparator wired into Makefile",
            "abnormal_abi": "Observed death-test witnesses only; no finite denominator exists for all malformed ABI and floating-point combinations",
        },
        "symbol_metrics": {
            "domain_denominator": len(domain),
            "direct_abi_count": len(direct),
            "direct_abi_pct": round(len(direct) / len(domain) * 100, 1),
            "direct_named_selftest_count": len(direct_selftests),
            "direct_named_selftest_pct": round(len(direct_selftests) / len(domain) * 100, 1),
            "direct_probe_count": len(direct_probe),
            "direct_gold_count": len(direct_gold),
            "direct_threefold_witness_count": len(direct_threefold),
            "direct_threefold_witness_pct": round(len(direct_threefold) / len(domain) * 100, 1),
            "internal_alias_candidate_count": len(aliases),
            "local_only_name_count": len(local),
            "direct_or_alias_or_local_count": len(direct) + len(aliases) + len(local),
            "direct_or_alias_or_local_pct": round((len(direct) + len(aliases) + len(local)) / len(domain) * 100, 1),
        },
        "pipeline_metrics": {
            "stages": pipeline_rows,
            "isolated_stage_count": isolated,
            "stage_denominator": len(PIPELINE),
            "isolated_stage_pct": round(isolated / len(PIPELINE) * 100, 1),
            "dyn_main_original_elf_scenario_witnesses": dyn_main_scenarios,
        },
        "abnormal_abi_metrics": {
            "observed_sigsegv_death_witnesses_passed": len(death_pass),
            "witnesses_expected": len(DEATH_WITNESSES),
            "witness_details": death_pass,
            "coverage_percentage": None,
        },
    }
    (OUT / "coverage_layers.json").write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n")
    with (OUT / "coverage_layers.md").open("w") as fp:
        m = result["symbol_metrics"]
        p = result["pipeline_metrics"]
        fp.write("# DynamicPackage Layered Coverage Metrics\n\n")
        fp.write("> **Interpretation.** These metrics are portfolio/accountability measures, not a proof that all inputs behave equivalently. Internal aliases are excluded from direct ABI coverage, and the abnormal ABI space has no finite global denominator.\n\n")
        fp.write("## Symbol Layer\n\n")
        fp.write("| Metric | Value |\n|---|---:|\n")
        fp.write(f"| Domain global function denominator | {m['domain_denominator']} |\n")
        fp.write(f"| Direct ABI name availability | {m['direct_abi_count']} / {m['domain_denominator']} ({m['direct_abi_pct']}%) |\n")
        fp.write(f"| Direct ABI with named selftest | {m['direct_named_selftest_count']} / {m['domain_denominator']} ({m['direct_named_selftest_pct']}%) |\n")
        fp.write(f"| Direct ABI with probe + gold + selftest witness | {m['direct_threefold_witness_count']} / {m['domain_denominator']} ({m['direct_threefold_witness_pct']}%) |\n")
        fp.write(f"| Internal alias candidates, excluded from direct ABI | {m['internal_alias_candidate_count']} / {m['domain_denominator']} |\n")
        fp.write(f"| Direct/alias/local implementation evidence, not equivalence | {m['direct_or_alias_or_local_count']} / {m['domain_denominator']} ({m['direct_or_alias_or_local_pct']}%) |\n\n")
        fp.write("## Top-Level Pipeline Layer\n\n")
        fp.write("| Stage | Evidence | Status |\n|---|---|---|\n")
        for row in pipeline_rows:
            fp.write(f"| `{row['stage']}` | `{row['evidence_comparator']}` | `{row['status']}` |\n")
        fp.write(f"\nIsolated top-level stage evidence: **{p['isolated_stage_count']} / {p['stage_denominator']} ({p['isolated_stage_pct']}%)**. `dyn_main` has **{p['dyn_main_original_elf_scenario_witnesses']}** original-ELF scenario witnesses in the reproduced log. This witness count has no known total scenario denominator.\n\n")
        a = result["abnormal_abi_metrics"]
        fp.write("## Abnormal ABI Layer\n\n")
        fp.write(f"The reproduced run passed **{a['observed_sigsegv_death_witnesses_passed']} / {a['witnesses_expected']}** explicitly enumerated isolated `SIGSEGV` death-test witnesses. This is a count of observed cases, **not an abnormal ABI coverage percentage**.\n")

if __name__ == "__main__":
    main()
