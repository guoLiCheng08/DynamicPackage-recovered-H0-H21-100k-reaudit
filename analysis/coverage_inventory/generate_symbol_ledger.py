#!/usr/bin/env python3
"""Generate a reproducible, evidence-oriented symbol coverage ledger.

The ledger deliberately distinguishes direct ABI/name matches from internal semantic
alias candidates. A filename or source-text hit is recorded as evidence discovery,
not treated as proof of full behavioral equivalence.
"""
from __future__ import annotations

import csv
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent
SRC = ROOT / "src"
ANALYSIS = ROOT / "analysis"
MAKEFILE = ROOT / "Makefile"

RUNTIME_EXCLUDED = {
    "_init", "_fini", "_start", "main", "__libc_csu_init", "__libc_csu_fini",
}

# Explicitly reviewed candidate mappings from original ABI names to recovered
# internal helpers. They are labelled CANDIDATE and do not contribute to direct
# ABI coverage unless a future dedicated wrapper and original-ELF comparator is added.
ALIAS_CANDIDATES = {
    "Calc_InertialMagneticVector": "dp_calc_inertial_magnetic_vector",
    "GetStarTrackerQuat": "dp_get_star_tracker_quat",
    "GetWheelSpeed": "dp_get_wheel_speed",
    "GravityGradientTorque": "dp_gravity_gradient_torque",
    "MagTorque": "dp_mag_torque",
    "MagneticVector": "dp_magnetic_vector",
    "RWheelTorque": "dp_rwheel_torque",
    "SetMomentMTQ": "dp_set_mtq_moment",
    "SetSADA": "dp_set_sada",
    "SetThrusterWorkStatus": "dp_set_thruster_work_status",
    "SetWheelAcc": "dp_set_wheel_acc",
    "Sun": "dp_sun_vector",
    "UpdateDSS": "dp_update_dss",
    "UpdateGPS": "dp_update_gps",
    "UpdateGyro": "dp_update_gyro",
    "UpdateMagMeter": "dp_update_magmeter",
    "UpdateMagMoment": "dp_update_mag_moment",
    "UpdateSunSensorValidFlag": "dp_update_dss_valid_flag",
    "UpdateThruster": "dp_update_thruster",
    "UpdateWheel": "dp_update_wheel;dp_update_wheel_group",
    "Update_STS_Quat": "dp_update_sts_quat",
    "Update_sat_inertia_xw": "dp_global_apply_sat_inertia_flag0;dp_global_apply_sat_inertia_flag1",
    "drive_SADA_once": "dp_drive_sada_once",
    "dynamics_flex": "dp_flex_dynamics_step",
    "getSADAangle": "dp_get_sada_angle",
    "isEarthShadow": "dp_is_earth_shadow",
    "isStarTrackerValid": "dp_is_star_tracker_valid",
    "sendDynTele": "dp_send_dyn_tele",
}

TEXT_SUFFIXES = {".c", ".h", ".gdb", ".md", ".txt", ".mk"}


def read_original_symbols() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for line in (OUT / "original_defined_function_symbols.tsv").read_text().splitlines():
        address, size, binding, section, name = line.split("\t", 4)
        if binding not in {"GLOBAL", "WEAK"}:
            continue
        rows.append({
            "original_symbol": name,
            "address": address,
            "size": size,
            "binding": binding,
            "section": section,
        })
    return rows


def read_symbol_file(path: Path) -> set[str]:
    return {line.split("\t", 1)[0] for line in path.read_text().splitlines() if line.strip()}


def refresh_recovered_global_symbols() -> None:
    """Refresh direct-ABI evidence from the exact library used by selftest."""
    library = ROOT / "build" / "libdynamicpackage_recovered.a"
    if not library.is_file():
        raise SystemExit(f"missing recovered library: {library}; run make all first")
    result = subprocess.run(
        ["nm", "-g", "--defined-only", str(library)],
        check=True, text=True, capture_output=True,
    )
    names: set[str] = set()
    for line in result.stdout.splitlines():
        fields = line.split()
        if len(fields) == 3 and fields[1] in {"T", "W"}:
            names.add(fields[2])
    (OUT / "recovered_global_function_names.txt").write_text(
        "".join(f"{name}\n" for name in sorted(names))
    )


def list_files() -> list[Path]:
    paths: list[Path] = [MAKEFILE]
    for base in (SRC, ANALYSIS):
        for p in base.rglob("*"):
            if p.is_file() and p.suffix.lower() in TEXT_SUFFIXES:
                paths.append(p)
    return paths


def text_hits(files: list[Path], symbol: str, suffix: str | None = None) -> list[str]:
    # C identifier boundaries avoid substrings such as Sun inside SunSensor.
    pattern = re.compile(r"(?<![A-Za-z0-9_])" + re.escape(symbol) + r"(?![A-Za-z0-9_])")
    hits: list[str] = []
    for path in files:
        if suffix and path.suffix != suffix:
            continue
        try:
            text = path.read_text(errors="ignore")
        except OSError:
            continue
        if pattern.search(text):
            hits.append(str(path.relative_to(ROOT)))
    return sorted(hits)


def normalize_name(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", value.lower())


def path_hits(needle: str, require_prefix: bool = False) -> list[str]:
    hits: list[str] = []
    for p in ANALYSIS.rglob("*gold*"):
        if not p.is_file():
            continue
        normalized_name = normalize_name(p.name)
        matches = normalized_name.startswith(needle + "gold") if require_prefix else needle in normalized_name
        if matches:
            hits.append(str(p.relative_to(ROOT)))
    return sorted(hits)


def compact(paths: list[str]) -> str:
    return ";".join(paths)


def main() -> None:
    refresh_recovered_global_symbols()
    global_functions = read_symbol_file(OUT / "recovered_global_function_names.txt")
    all_functions = read_symbol_file(OUT / "recovered_all_function_symbols.tsv")
    source_ref_map = {}
    for line in (OUT / "unmatched_original_source_references.tsv").read_text().splitlines():
        state, name = line.split("\t", 1)
        source_ref_map[name] = state

    makefile_text = MAKEFILE.read_text(errors="ignore")
    files = list_files()
    records: list[dict[str, str]] = []
    for row in read_original_symbols():
        name = row["original_symbol"]
        scope = "ELF_RUNTIME_EXCLUDED" if name in RUNTIME_EXCLUDED else "DOMAIN_GLOBAL"
        in_global = name in global_functions
        in_all = name in all_functions
        alias = ALIAS_CANDIDATES.get(name, "")
        alias_targets = [x for x in alias.split(";") if x]
        alias_present = bool(alias_targets) and all(x in global_functions for x in alias_targets)
        source_ref = source_ref_map.get(name, "DIRECT_OR_NOT_CHECKED")

        probes = text_hits(files, name, ".gdb")
        comparators = [p for p in text_hits(files, name, ".c") if p.startswith("analysis/")]
        comparator_in_selftest = [p for p in comparators if p in makefile_text]
        docs = [p for p in text_hits(files, name, ".md") if p.startswith("analysis/")]
        normalized = normalize_name(name)
        # Very short ABI names are normally excluded from filename matching to avoid
        # incidental collisions. c2/s2/Frac/Pnm/Sun/M2E are reviewed exceptions: their dedicated
        # original-ELF probes export unambiguous symbol-prefixed gold files.
        if name in {"c2", "s2", "Frac", "Pnm", "Sun", "M2E"}:
            golds = path_hits(normalized, require_prefix=True)
        else:
            golds = path_hits(normalized) if len(normalized) >= 5 else []

        if scope == "ELF_RUNTIME_EXCLUDED":
            status = "EXCLUDED_RUNTIME"
        elif in_global:
            status = "DIRECT_ABI_NAME_MATCH"
        elif in_all:
            status = "LOCAL_ONLY_NAME_MATCH"
        elif alias_present:
            status = "INTERNAL_ALIAS_CANDIDATE"
        elif source_ref == "SOURCE_REF":
            status = "SOURCE_REFERENCE_ONLY"
        else:
            status = "NO_RECOVERED_EVIDENCE"

        # Strong evidence requires a direct ABI-name function and an explicit
        # selftest comparator mentioning the original symbol. GDB/gold presence
        # is recorded separately because many historical tests use non-symbolic names.
        if status == "DIRECT_ABI_NAME_MATCH" and comparator_in_selftest:
            verification = "DIRECT_SELFTEST_EVIDENCE"
        elif status == "DIRECT_ABI_NAME_MATCH":
            verification = "DIRECT_IMPLEMENTATION_NO_NAMED_SELFTEST"
        elif status == "INTERNAL_ALIAS_CANDIDATE" and comparator_in_selftest:
            verification = "ALIAS_CANDIDATE_WITH_NAMED_SELFTEST"
        elif status == "INTERNAL_ALIAS_CANDIDATE":
            verification = "ALIAS_CANDIDATE_NO_NAMED_SELFTEST"
        elif probes or golds or comparators:
            verification = "EVIDENCE_EXISTS_MAPPING_UNRESOLVED"
        else:
            verification = "NO_SYMBOL_LEVEL_EVIDENCE"

        records.append({
            **row,
            "scope": scope,
            "recovered_global_name_match": "YES" if in_global else "NO",
            "recovered_any_visibility_name_match": "YES" if in_all else "NO",
            "reviewed_internal_alias_candidate": alias,
            "alias_target_present": "YES" if alias_present else "NO",
            "source_text_reference": source_ref,
            "implementation_status": status,
            "verification_status": verification,
            "gdb_probe_count": str(len(probes)),
            "gdb_probe_paths": compact(probes),
            "gold_file_count": str(len(golds)),
            "gold_file_paths": compact(golds),
            "comparator_count": str(len(comparators)),
            "comparator_paths": compact(comparators),
            "selftest_named_comparator_count": str(len(comparator_in_selftest)),
            "selftest_named_comparator_paths": compact(comparator_in_selftest),
            "documentation_count": str(len(docs)),
            "documentation_paths": compact(docs),
            "coverage_note": (
                "Exact recovered symbol available" if in_global else
                "Recovered local-only symbol; public ABI wrapper absent" if in_all else
                "Reviewed internal helper mapping; requires original-name ABI wrapper and dedicated comparator" if alias_present else
                "Original name occurs in recovery source/comments but no verified ABI mapping" if source_ref == "SOURCE_REF" else
                "No source-level recovery evidence found by this inventory"
            ),
        })

    fieldnames = list(records[0])
    with (OUT / "symbol_coverage_ledger.csv").open("w", newline="") as fp:
        writer = csv.DictWriter(fp, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(records)

    counts: dict[str, int] = {}
    domain = [r for r in records if r["scope"] == "DOMAIN_GLOBAL"]
    for r in domain:
        counts[r["implementation_status"]] = counts.get(r["implementation_status"], 0) + 1
    direct = sum(r["implementation_status"] == "DIRECT_ABI_NAME_MATCH" for r in domain)
    direct_selftest = sum(r["verification_status"] == "DIRECT_SELFTEST_EVIDENCE" for r in domain)
    aliases = sum(r["implementation_status"] == "INTERNAL_ALIAS_CANDIDATE" for r in domain)
    with (OUT / "symbol_coverage_metrics.md").open("w") as fp:
        fp.write("# DynamicPackage Symbol Coverage Metrics\n\n")
        fp.write("> **Method boundary.** The denominator is the original ELF's non-runtime GLOBAL/WEAK defined FUNC symbols. "
                 "Exact recovered symbol-name matches are counted as direct ABI availability. Internal `dp_*` mappings are deliberately reported as candidates and are not counted as direct ABI coverage. "
                 "A named comparator in `Makefile` is source-level evidence, not a claim that all inputs are equivalent.\n\n")
        fp.write(f"- Domain global function denominator: **{len(domain)}**\n")
        fp.write(f"- Direct original-name functions present in recovered global library: **{direct} / {len(domain)} ({direct / len(domain) * 100:.1f}%)**\n")
        fp.write(f"- Direct original-name functions with a named comparator wired into `selftest`: **{direct_selftest} / {len(domain)} ({direct_selftest / len(domain) * 100:.1f}%)**\n")
        fp.write(f"- Reviewed internal alias candidates (excluded from direct ABI metric): **{aliases} / {len(domain)} ({aliases / len(domain) * 100:.1f}%)**\n\n")
        fp.write("| Implementation status | Count | Share of domain denominator |\n|---|---:|---:|\n")
        for key in sorted(counts):
            fp.write(f"| `{key}` | {counts[key]} | {counts[key] / len(domain) * 100:.1f}% |\n")
        fp.write("\nThe full per-symbol ledger is `symbol_coverage_ledger.csv`.\n")

if __name__ == "__main__":
    main()
