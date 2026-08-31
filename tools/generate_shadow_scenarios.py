#!/usr/bin/env python3
"""Generate deterministic 15-column dual-replay command scenarios."""

from pathlib import Path
import sys


def frame(wheel=(0.0, 0.0, 0.0, 0.0), mtq=(0.0,) * 6, sada_flag=0,
          sada=(0.0, 0.0), thruster=0, inertia=0):
    return [*wheel, *mtq, float(sada_flag), *sada, float(thruster), float(inertia)]


def write_case(directory, name, frames):
    path = directory / f"{name}.csv"
    with path.open("w", encoding="ascii") as output:
        output.write("# wheel[4],mtq[6],sada_flag,sada_angle[2],thruster,inertia\n")
        for values in frames:
            output.write(",".join(format(value, ".9g") for value in values) + "\n")


def repeated(count, value):
    return [value[:] for _ in range(count)]


def main():
    directory = Path(sys.argv[1] if len(sys.argv) == 2 else "build/shadow_scenarios")
    directory.mkdir(parents=True, exist_ok=True)

    write_case(directory, "01_baseline_100", repeated(100, frame()))
    write_case(directory, "02_wheel_step_200",
               repeated(20, frame()) +
               repeated(120, frame(wheel=(0.001, -0.002, 0.003, -0.004))) +
               repeated(60, frame()))
    write_case(directory, "03_mtq_step_200",
               repeated(20, frame()) +
               repeated(120, frame(mtq=(0.01, -0.02, 0.03, -0.04, 0.05, -0.06))) +
               repeated(60, frame()))
    write_case(directory, "04_sada_position_200",
               repeated(20, frame()) +
               repeated(90, frame(sada_flag=1, sada=(0.01, -0.005))) +
               repeated(90, frame(sada_flag=1, sada=(-0.01, 0.005))))
    write_case(directory, "05_thruster_enable_200",
               repeated(20, frame()) + repeated(90, frame(thruster=1)) +
               repeated(90, frame()))
    first_mixed = frame(wheel=(-0.002, 0.001, -0.002, 0.0),
                        mtq=(-0.02, 0.01, -0.01, 0.0, 0.005, -0.005),
                        sada=( -0.01, 0.005))
    second_mixed = frame(wheel=(-0.001, 0.0, 0.0, -0.001),
                         mtq=(-0.01, 0.0, 0.0, -0.01, 0.005, -0.005),
                         sada_flag=1, sada=(0.01, -0.005))
    write_case(directory, "06_mixed_first_step_1", [first_mixed])
    write_case(directory, "06_mixed_first_two_2", [first_mixed, second_mixed])
    write_case(directory, "06_mixed_after_idle_2", [frame(), first_mixed])
    write_case(directory, "07_combined_transition_500",
               repeated(50, frame()) +
               repeated(150, frame(wheel=(0.001, -0.002, 0.003, -0.004),
                                   mtq=(0.01, -0.02, 0.03, -0.04, 0.05, -0.06),
                                   sada_flag=1, sada=(0.01, -0.005), thruster=1)) +
               repeated(150, frame(wheel=(-0.004, 0.003, -0.002, 0.001),
                                   mtq=(-0.06, 0.05, -0.04, 0.03, -0.02, 0.01),
                                   sada_flag=1, sada=(-0.01, 0.005), thruster=1)) +
               repeated(150, frame()))
    mixed_frames = [
        frame(wheel=(0.001 * ((step % 5) - 2), -0.001 * ((step % 4) - 1),
                     0.002 * ((step % 3) - 1), -0.001 * (step % 2)),
              mtq=(0.01 * ((step % 5) - 2), -0.01 * ((step % 4) - 1),
                   0.01 * ((step % 3) - 1), -0.01 * (step % 2), 0.005, -0.005),
              sada_flag=step % 2, sada=(0.01 if step % 2 else -0.01, -0.005 if step % 3 else 0.005),
              thruster=(step // 50) % 2)
        for step in range(10000)
    ]
    write_case(directory, "08_mixed_1000", mixed_frames[:1000])
    write_case(directory, "08b_mixed_2000", mixed_frames[:2000])
    write_case(directory, "08c_mixed_4000", mixed_frames[:4000])
    write_case(directory, "09_mixed_10000", mixed_frames)
    print(directory)


if __name__ == "__main__":
    main()
