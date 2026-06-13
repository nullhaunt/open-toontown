import sys

from panda3d.core import Vec3, Point3
from toontown_cpp.math import _acltestNegateVec3, _acltestNegatePoint3

fails = 0


def chk(label, cond):
    global fails
    if not cond:
        fails += 1
        print(f"FAIL {label}")


cases = [(0.0, 0.0, 0.0), (1.0, -2.0, 3.5), (-7.25, 100.0, 0.001),
         (12345.0, -54321.0, 0.5)]

for x, y, z in cases:
    """
     Expected values are built as panda3d.core types so both sides are stored
     at float precision -- comparing against Python double literals like -0.001
     would spuriously fail, since 0.001 is not exactly representable in float.
     """
    out = _acltestNegateVec3(Vec3(x, y, z))
    exp = Vec3(-x, -y, -z)
    chk(f"Vec3 type {x, y, z}", isinstance(out, Vec3))
    chk(f"Vec3 value {x, y, z}",
        (out.getX(), out.getY(), out.getZ())
        == (exp.getX(), exp.getY(), exp.getZ()))

    out = _acltestNegatePoint3(Point3(x, y, z))
    exp = Point3(-x, -y, -z)
    chk(f"Point3 type {x, y, z}", isinstance(out, Point3))
    chk(f"Point3 value {x, y, z}",
        (out.getX(), out.getY(), out.getZ())
        == (exp.getX(), exp.getY(), exp.getZ()))

if fails:
    print(f"\n{fails} failure(s).")
    sys.exit(1)

print("All checks passed: native math crosses the boundary as panda3d.core types.")
