# DynamicPackage Symbol Coverage Metrics

> **Method boundary.** The denominator is the original ELF's non-runtime GLOBAL/WEAK defined FUNC symbols. Exact recovered symbol-name matches are counted as direct ABI availability. Internal `dp_*` mappings are deliberately reported as candidates and are not counted as direct ABI coverage. A named comparator in `Makefile` is source-level evidence, not a claim that all inputs are equivalent.

- Domain global function denominator: **194**
- Direct original-name functions present in recovered global library: **194 / 194 (100.0%)**
- Direct original-name functions with a named comparator wired into `selftest`: **194 / 194 (100.0%)**
- Reviewed internal alias candidates (excluded from direct ABI metric): **0 / 194 (0.0%)**

| Implementation status | Count | Share of domain denominator |
|---|---:|---:|
| `DIRECT_ABI_NAME_MATCH` | 194 | 100.0% |

The full per-symbol ledger is `symbol_coverage_ledger.csv`.
