import math

radius = 70_000_000.0
for delta in (0.080, 0.090, 0.093, 0.100):
    x = -radius * math.cos(delta)
    y = radius * math.sin(delta)
    print(f"{delta:.17g} {x:.17g} {y:.17g}")
