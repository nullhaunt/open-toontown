import os
import sys

from panda3d.core import Point3, Vec3
from toontown_cpp.classicchars import (
    getNodePos, getAdjacentNodes, getWayPoints, getRaycastFlag,
    getPointsFromTo, getWalkDuration, getWalkDistance, _getCharPaths,
)


# --- Locate and load the original path data -----------------------------------
def find_original():
    d = os.path.dirname(os.path.abspath(__file__))
    while True:
        cand = os.path.join(d, "toontown", "classicchars", "CCharPaths.py")
        if os.path.isfile(cand):
            return cand
        parent = os.path.dirname(d)
        if parent == d:
            sys.exit("could not find toontown/classicchars/CCharPaths.py above this test")
        d = parent


lines = open(find_original()).read().splitlines()
try:
    start = next(i for i, l in enumerate(lines) if l.startswith("__mickeyPaths"))
    end = next(i for i, l in enumerate(lines) if l.startswith("DaleOrbitDistanceOverride"))
except StopIteration:
    sys.exit("original CCharPaths.py has no data -- already shimmed? run the test first.")
ns = {"Point3": Point3}
exec("\n".join(lines[start:end]), ns)

dd_paths = {"a": (Point3(0, 0, 0), "a")}
dd_wpts = ()

# (cpp key, paths dict, waypoint tuple) -- mirrors the data generator + DonaldDock
charmap = [
    ("mickey", ns["__mickeyPaths"], ns["__mickeyWaypoints"]),
    ("minnie", ns["__minniePaths"], ns["__minnieWaypoints"]),
    ("goofy", ns["__goofyPaths"], ns["__goofyWaypoints"]),
    ("goofySpeedway", ns["__goofySpeedwayPaths"], ns["__goofySpeedwayWaypoints"]),
    ("donald", ns["__donaldPaths"], ns["__donaldWaypoints"]),
    ("pluto", ns["__plutoPaths"], ns["__plutoWaypoints"]),
    ("daisy", ns["__daisyPaths"], ns["__daisyWaypoints"]),
    ("chip", ns["__chipPaths"], ns["__chipWaypoints"]),
    ("donaldDock", dd_paths, dd_wpts),
]


# --- Reference query logic, verbatim from the original module -----------------
def r_nodePos(node, paths):
    return paths[node][0]


def r_adjacent(node, paths):
    a = paths[node][1]
    return [a] if isinstance(a, str) else list(a)


def r_wayPoints(frm, to, paths, wpts):
    lst = []
    if frm != to:
        for path in wpts:
            if path[0] == frm and path[1] == to:
                for point in path[3]:
                    lst.append(Point3(point))
                break
            elif path[0] == to and path[1] == frm:
                for point in path[3]:
                    lst = [Point3(point)] + lst
                break
    return lst


def r_raycast(frm, to, wpts):
    result = 0
    if frm != to:
        for path in wpts:
            if path[0] == frm and path[1] == to:
                if path[2]:
                    result = 1
                    break
            elif path[0] == to and path[1] == frm:
                if path[2]:
                    result = 1
                    break
    return result


def r_pointsFromTo(frm, to, paths, wpts):
    return ([Point3(r_nodePos(frm, paths))] + r_wayPoints(frm, to, paths, wpts)
            + [Point3(r_nodePos(to, paths))])


def r_walkDistance(frm, to, paths, wpts):
    pts = r_pointsFromTo(frm, to, paths, wpts)
    return sum(Vec3(pts[i + 1] - pts[i]).length() for i in range(len(pts) - 1))


def r_walkDuration(frm, to, velocity, paths, wpts):
    pts = r_pointsFromTo(frm, to, paths, wpts)
    return sum(Vec3(pts[i + 1] - pts[i]).length() / velocity
               for i in range(len(pts) - 1))


# --- Harness ------------------------------------------------------------------
VELOCITY = 13.0
fails = 0


def chk(label, cond):
    global fails
    if not cond:
        fails += 1
        print(f"MISMATCH {label}")


def p3eq(a, b):
    return (a.getX(), a.getY(), a.getZ()) == (b.getX(), b.getY(), b.getZ())


def listp3eq(a, b):
    return len(a) == len(b) and all(p3eq(x, y) for x, y in zip(a, b))


for key, paths, wpts in charmap:
    cp = _getCharPaths(key)
    nodes = list(paths.keys())
    for n in nodes:
        chk(f"{key}.getNodePos[{n}]", p3eq(r_nodePos(n, paths), getNodePos(n, cp)))
        chk(f"{key}.getAdjacentNodes[{n}]",
            r_adjacent(n, paths) == list(getAdjacentNodes(n, cp)))
    for a in nodes:
        for b in nodes:
            chk(f"{key}.getWayPoints[{a},{b}]",
                listp3eq(r_wayPoints(a, b, paths, wpts), getWayPoints(a, b, cp)))
            chk(f"{key}.getRaycastFlag[{a},{b}]",
                r_raycast(a, b, wpts) == getRaycastFlag(a, b, cp))
            chk(f"{key}.getPointsFromTo[{a},{b}]",
                listp3eq(r_pointsFromTo(a, b, paths, wpts),
                         getPointsFromTo(a, b, cp)))
            chk(f"{key}.getWalkDistance[{a},{b}]",
                r_walkDistance(a, b, paths, wpts)
                == getWalkDistance(a, b, VELOCITY, cp))
            chk(f"{key}.getWalkDuration[{a},{b}]",
                r_walkDuration(a, b, VELOCITY, paths, wpts)
                == getWalkDuration(a, b, VELOCITY, cp))

if fails:
    print(f"\n{fails} mismatch(es).")
    sys.exit(1)

print("All checks passed: C++ output is identical to the Python.")
