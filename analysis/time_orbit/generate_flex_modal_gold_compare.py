#!/usr/bin/env python3
import re
import struct
from pathlib import Path

ROOT = Path('/home/ubuntu/dynamicpackage_restore')
SOURCE = ROOT / 'analysis/time_orbit/sat_flex_real_matrices.log'
OUTPUT = ROOT / 'analysis/time_orbit/flex_modal_real_model_compare.c'
SHAPES = {'C_3x10': (3, 10), 'MA_10x10': (10, 10), 'MD_10x10': (10, 10)}


def decode(word: str) -> float:
    return struct.unpack('<d', int(word, 16).to_bytes(8, 'little'))[0]


def load_blocks() -> dict[str, list[float]]:
    blocks = {name: [] for name in SHAPES}
    active = None
    for line in SOURCE.read_text().splitlines():
        label = line.strip()
        if label in SHAPES:
            active = label
            continue
        if active is None or ':' not in line:
            continue
        expected = SHAPES[active][0] * SHAPES[active][1]
        if len(blocks[active]) >= expected:
            active = None
            continue
        words = re.findall(r'0x([0-9a-f]{16})', line)
        blocks[active].extend(decode(word) for word in words[:expected - len(blocks[active])])
    for name, (rows, cols) in SHAPES.items():
        if len(blocks[name]) != rows * cols:
            raise RuntimeError(f'{name}: expected {rows * cols}, got {len(blocks[name])}')
    return blocks


def c_array(name: str, values: list[float], per_row: int) -> str:
    lines = [f'    double {name}[{len(values)}] = {{']
    for offset in range(0, len(values), per_row):
        lines.append('        ' + ', '.join(value.hex() for value in values[offset:offset + per_row]) + ',')
    lines.append('    };')
    return '\n'.join(lines)


def main() -> None:
    blocks = load_blocks()
    source = f'''#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_flex.h"

static uint64_t bits(double value)
{{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}}

static double from_bits(uint64_t value)
{{
    double result;
    memcpy(&result, &value, sizeof(result));
    return result;
}}

int main(void)
{{
{c_array('c_data', blocks['C_3x10'], 10)}
{c_array('ma_data', blocks['MA_10x10'], 10)}
{c_array('md_data', blocks['MD_10x10'], 10)}
    double j_data[9] = {{
        0x1.24a0000000000p+10, 0x1.8700000000000p+9, 0x1.7ae147ae147aep+0,
        0x1.86fffffffffffp+9, 0x1.4b9ffffffffffp+10, -0x1.1b33333333333p+4,
        0x1.7ae147ae147aep+0, -0x1.1b33333333333p+4, 0x1.02e0000000000p+11
    }};
    double modal_position_data[10] = {{
        0x1.0624dd2f1a9fcp-10, -0x1.0624dd2f1a9fcp-9, 0x1.89374bc6a7efap-9,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0
    }};
    double modal_velocity_data[10] = {{
        0x1.6872b020c49bap-7, -0x1.89374bc6a7efap-7, 0x1.a9fbe76c8b439p-7,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0
    }};
    double output_data[10] = {{0.0}};
    double modal_velocity_output_data[10] = {{0.0}};
    const uint64_t alpha_bits[3] = {{
        UINT64_C(0x3f36e0882c67b83c), UINT64_C(0x3f36dcb2526b234e),
        UINT64_C(0x3f23567451d0a876)
    }};
    const uint64_t expected[10] = {{
        UINT64_C(0xbf5f6b5e4cb791b9), UINT64_C(0x3f78e3d0bd482e6e),
        UINT64_C(0xbf84d2c06cb74ba9), UINT64_C(0xbf13d737cf235946),
        UINT64_C(0x3f3d1c028f2d140e), UINT64_C(0xbf02193bb4a383d2),
        UINT64_C(0xbf20149e75bcf8ee), UINT64_C(0x3ea253bdcfcda598),
        UINT64_C(0x3ed6d5111c87b889), UINT64_C(0x3eb4f5831c2f9526)
    }};
    const uint64_t rate_bits[3] = {{
        UINT64_C(0x3f847ae147ae147b), UINT64_C(0xbf947ae147ae147b),
        UINT64_C(0x3f9eb851eb851eb8)
    }};
    const uint64_t momentum_bits[3] = {{
        UINT64_C(0xc00f17009913099e), UINT64_C(0xc0333eda470aa2eb),
        UINT64_C(0x404f411a56c06a96)
    }};
    const uint64_t expected_rhs_bits[3] = {{
        UINT64_C(0x3fe5ce3fcd7b4568), UINT64_C(0x3fe7722fe7e80661),
        UINT64_C(0x3fd2f6b61d53fb4f)
    }};
    DpMatrix c = {{3, 10, 10, 0, c_data}};
    DpMatrix ma = {{10, 10, 10, 0, ma_data}};
    DpMatrix md = {{10, 10, 10, 0, md_data}};
    DpMatrix j = {{3, 3, 3, 0, j_data}};
    DpVector modal_position = {{10, 0, modal_position_data}};
    DpVector modal_velocity = {{10, 0, modal_velocity_data}};
    DpVector output = {{10, 0, output_data}};
    DpVector modal_velocity_output = {{10, 0, modal_velocity_output_data}};
    DpVec3 alpha;
    DpVec3 body_rate;
    DpVec3 angular_momentum;
    DpVec3 rhs;
    DpVec3 step_alpha;
    DpFlexDynamicsConfig config;
    unsigned index;

    alpha.x = from_bits(alpha_bits[0]);
    alpha.y = from_bits(alpha_bits[1]);
    alpha.z = from_bits(alpha_bits[2]);
    if (dp_flex_modal_acceleration(&output, &c, &ma, &md, &modal_position,
                                   &modal_velocity, &alpha, NULL, NULL) != 0) {{
        puts("FAIL modal acceleration returned error");
        return 1;
    }}
    for (index = 0u; index < 10u; ++index) {{
        if (bits(output_data[index]) != expected[index]) {{
            printf("FAIL modal acceleration[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\\n",
                   index, output_data[index], (unsigned long long)expected[index],
                   (unsigned long long)bits(output_data[index]));
            return 1;
        }}
    }}
    body_rate.x = from_bits(rate_bits[0]);
    body_rate.y = from_bits(rate_bits[1]);
    body_rate.z = from_bits(rate_bits[2]);
    angular_momentum.x = from_bits(momentum_bits[0]);
    angular_momentum.y = from_bits(momentum_bits[1]);
    angular_momentum.z = from_bits(momentum_bits[2]);
    if (dp_flex_compose_rigid_rhs_coupled(&rhs, &body_rate, &angular_momentum, NULL,
                                          &c, &ma, &md, &modal_position,
                                          &modal_velocity) != 0) {{
        puts("FAIL coupled rigid rhs returned error");
        return 1;
    }}
    for (index = 0u; index < 3u; ++index) {{
        const double actual = index == 0u ? rhs.x : (index == 1u ? rhs.y : rhs.z);
        if (bits(actual) != expected_rhs_bits[index]) {{
            printf("FAIL coupled rigid rhs[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\\n",
                   index, actual, (unsigned long long)expected_rhs_bits[index],
                   (unsigned long long)bits(actual));
            return 1;
        }}
    }}
    config.rigid_matrices.coupling_3xn = c;
    config.rigid_matrices.base_inertia_3x3 = j;
    config.modal_matrix_a_nxn = &ma;
    config.modal_matrix_d_nxn = &md;
    config.angular_momentum = &angular_momentum;
    config.minus_term = NULL;
    config.rigid_map_3x3 = NULL;
    config.rigid_aux = NULL;
    memset(output_data, 0, sizeof(output_data));
    if (dp_flex_dynamics_step(&step_alpha, &output, &modal_velocity_output,
                              &body_rate, &modal_position, &modal_velocity,
                              &config) != 0) {{
        puts("FAIL flex dynamics step returned error");
        return 1;
    }}
    for (index = 0u; index < 3u; ++index) {{
        const double actual = index == 0u ? step_alpha.x : (index == 1u ? step_alpha.y : step_alpha.z);
        if (bits(actual) != alpha_bits[index]) {{
            printf("FAIL flex dynamics alpha[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\\n",
                   index, actual, (unsigned long long)alpha_bits[index],
                   (unsigned long long)bits(actual));
            return 1;
        }}
    }}
    for (index = 0u; index < 10u; ++index) {{
        if (bits(output_data[index]) != expected[index] ||
            bits(modal_velocity_output_data[index]) != bits(modal_velocity_data[index])) {{
            printf("FAIL flex dynamics modal[%u] accel_bits=%016llx expected=%016llx velocity_bits=%016llx expected_velocity=%016llx\\n",
                   index, (unsigned long long)bits(output_data[index]),
                   (unsigned long long)expected[index],
                   (unsigned long long)bits(modal_velocity_output_data[index]),
                   (unsigned long long)bits(modal_velocity_data[index]));
            return 1;
        }}
    }}
    puts("flex modal acceleration, coupled RHS and dynamics step real-model original-ELF compare: PASS (bitwise)");
    return 0;
}}
'''
    OUTPUT.write_text(source)


if __name__ == '__main__':
    main()
