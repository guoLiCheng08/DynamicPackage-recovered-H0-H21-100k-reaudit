# DynamicPackage Recovery Effort Estimate

> **Estimate boundary.** An engineering day is a planning unit for one focused implementation stream; it is not a calendar commitment. Every range includes the full proof loop: static ABI audit, repeated original-ELF gold, minimal C11 implementation/wrapper, strict comparator, Makefile gate, documentation, and clean selftest. No range proves unrestricted input-space equivalence.

## Backlog by Evidence State

| Work package | Items | Baseline per-symbol effort (engineering days) | Subtotal (engineering days) |
|---|---:|---:|---:|
| `ALIAS_CANDIDATE_NO_NAMED_SELFTEST` | 10 | 0.5–1.5 | 5.0–15.0 |
| `ALIAS_CANDIDATE_WITH_NAMED_SELFTEST` | 15 | 0.5–1.5 | 7.5–22.5 |
| `DIRECT_EVIDENCE_THICKENING` | 64 | 0.2–1.0 | 16.0–64.0 |
| `DIRECT_IMPLEMENTATION_NO_NAMED_SELFTEST` | 5 | 0.2–0.8 | 1.2–3.8 |
| `EVIDENCE_EXISTS_MAPPING_UNRESOLVED` | 21 | 1.0–3.0 | 31.0–88.0 |
| `NO_SYMBOL_LEVEL_EVIDENCE` | 22 | 1.0–3.0 | 38.0–106.0 |

Complex no-evidence items are elevated to 3.0–8.0 engineering days each: `AerodynamicTorque`, `FrontalArea`, `GetInertialMag`, `Get_Orbit_RV`, `LocalGeo2LVLH`, `Pnm`, `R_ECEF2LocalGeo`, `UpdateExternalForce`, `Update_sat_inertia`, `get_Body2Inertial`, `get_Inertial2Body`, `get_attitude`, `gh_change`.

## Delivery Scenarios

| Target | Scope | Estimated focused engineering effort | Outcome boundary |
|---|---|---:|---|
| 快速补齐公开封装和现有直接接口门禁 | P0 0 项 + 直接实现但缺同名门禁 5 项；不含复杂科学函数 | 1.2–3.8 | 提升公开 ABI/门禁覆盖，不能显著提高全分支或环境模型等价证据。 |
| 公开 ABI 闭合到别名/引用层 | 在快速批次基础上闭合 25 个内部别名候选及 12 个仅引用项 | 25.8–77.2 | 消除大量“有内部逻辑、无公开 ABI 证据”的缺口。 |
| 扩大强证据层并处理无证据/高复杂函数 | 前两批 + 64 个直接接口证据加厚 + 31 个无证据项（含复杂环境/动力学项） | 98.8–299.2 | 显著增加探针、金标和门禁闭环；仍不是全输入空间证明。 |

## Queue Mix

- Active action-queue items: **73**.
- P0: **0**; P1: **68**; P2: **5**.
- No recovered evidence: **31**; internal alias candidates: **25**; source-reference-only: **12**; direct ABI with no named selftest: **5**.
- Direct ABI evidence-thickening backlog: **64**.
