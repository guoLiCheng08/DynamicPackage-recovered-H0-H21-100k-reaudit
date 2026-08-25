# DynamicPackage Layered Coverage Metrics

> **Interpretation.** These metrics are portfolio/accountability measures, not a proof that all inputs behave equivalently. Internal aliases are excluded from direct ABI coverage, and the abnormal ABI space has no finite global denominator.

## Symbol Layer

| Metric | Value |
|---|---:|
| Domain global function denominator | 194 |
| Direct ABI name availability | 194 / 194 (100.0%) |
| Direct ABI with named selftest | 194 / 194 (100.0%) |
| Direct ABI with probe + gold + selftest witness | 194 / 194 (100.0%) |
| Internal alias candidates, excluded from direct ABI | 0 / 194 |
| Direct/alias/local implementation evidence, not equivalence | 194 / 194 (100.0%) |

## Top-Level Pipeline Layer

| Stage | Evidence | Status |
|---|---|---|
| `DynamicInit` | `analysis/dynamic_init_global_original_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `UpdateDeviceControl` | `analysis/update_device_control_global_original_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `UpdateCoreDynInput` | `analysis/update_core_dyn_input_p4_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `CoreDynamic` | `analysis/time_orbit/core_dynamic_default_rk4_bridge_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `UpdateDeviceMeasure` | `analysis/update_device_measure_global_original_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `UpdateMainOut` | `analysis/update_mainout_global_original_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `sendDynTele` | `analysis/time_orbit/send_dyn_tele_compare.c` | `ISOLATED_SELFTEST_WIRED` |
| `dyn_main` | `analysis/dyn_main_global_step1_original_gold_compare.c` | `ISOLATED_SELFTEST_WIRED` |

Isolated top-level stage evidence: **8 / 8 (100.0%)**. `dyn_main` has **51** original-ELF scenario witnesses in the reproduced log. This witness count has no known total scenario denominator.

## Abnormal ABI Layer

The reproduced run passed **5 / 5** explicitly enumerated isolated `SIGSEGV` death-test witnesses. This is a count of observed cases, **not an abnormal ABI coverage percentage**.
