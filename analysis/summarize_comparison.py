#!/usr/bin/env python3
"""生成接口级比较摘要；不加载或执行原 ELF。"""
from __future__ import annotations

import json
import math
from pathlib import Path

root = Path(__file__).resolve().parent
original = json.loads((root / "original_zero_command_telemetry.json").read_text())
recovered = json.loads((root / "recovered_orbit_probe.json").read_text())

velocity = original["gci_velocity"]
position_dot = recovered["recovered_position_dot"]
position_dot_abs_error = [abs(a - b) for a, b in zip(velocity, position_dot)]
position_dot_max_abs_error = max(position_dot_abs_error)
acceleration = recovered["recovered_central_gravity_acceleration"]
acceleration_norm = math.sqrt(sum(value * value for value in acceleration))

# 评分是基于已收集的函数数、代码量和逐模块证据的工程评估，不是机器生成的“相似度”。
summary = {
    "sample_sha256": "312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403",
    "test_scope": "private_shm_no_network_zero_command",
    "original_elf_executed": True,
    "original_test_exit_status": 0,
    "comparison_level": "interface_contract_and_component_invariant",
    "full_end_to_end_numeric_equivalence_test": False,
    "comparison_result": {
        "input_snapshot_source": "original ELF shared-memory telemetry",
        "contract_checked": "orbit kinematics: position derivative equals inertial velocity",
        "original_velocity_gci": velocity,
        "recovered_position_dot": position_dot,
        "max_absolute_error": position_dot_max_abs_error,
        "contract_pass": position_dot_max_abs_error == 0.0,
        "recovered_central_gravity_acceleration": acceleration,
        "recovered_central_gravity_acceleration_norm": acceleration_norm,
    },
    "recovery_score_percent": {
        "input_ipc_layout": 95,
        "state_vector_layout": 85,
        "output_telemetry_mapping": 70,
        "initialization_and_control_flow": 75,
        "rk4_integrator": 90,
        "orbit_model_numeric_detail": 25,
        "flexible_body_and_device_model": 10,
        "complete_buildable_runtime": 15,
        "overall_engineering_recovery": 35,
    },
    "measurement_basis": {
        "original_global_defined_functions": 200,
        "original_global_defined_function_code_bytes": 56100,
        "directly_reimplemented_top_level_functions": 6,
        "important_missing_runtime_blocks": [
            "CoreDynamic",
            "dynamics_flex",
            "UpdateDeviceControl",
            "UpdateCoreDynInput",
            "UpdateDeviceMeasure",
            "UpdateMainOut",
            "sensor and actuator model implementations",
        ],
    },
}
(root / "comparison_result.json").write_text(
    json.dumps(summary, ensure_ascii=False, indent=2) + "\n"
)
print(json.dumps(summary, ensure_ascii=False, indent=2))
