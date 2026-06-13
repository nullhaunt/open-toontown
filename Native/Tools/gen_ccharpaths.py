#!/usr/bin/env python3

import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_SRC = os.path.normpath(
    os.path.join(HERE, "..", "..", "toontown", "classicchars", "CCharPaths.py"))
DEFAULT_OUT = os.path.normpath(
    os.path.join(HERE, "..", "Source", "Classicchars", "CCharPaths.data.inc"))

src_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_SRC
out_path = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_OUT

lines = open(src_path).read().splitlines()
try:
    start = next(i for i, l in enumerate(lines) if l.startswith("__mickeyPaths"))
    end = next(i for i, l in enumerate(lines) if l.startswith("DaleOrbitDistanceOverride"))
except StopIteration:
    sys.exit(f"{src_path} has no path data -- is it the shim rather than the original?")
block = "\n".join(lines[start:end])


class P3:
    def __init__(self, *a):
        if len(a) == 1 and isinstance(a[0], P3):
            self.t = a[0].t
        else:
            self.t = tuple(float(x) for x in a)


def Point3(*a):
    return P3(*a)


ns = {"Point3": Point3}
exec(block, ns)

chars = [
    ("mickey", "__mickeyPaths", "__mickeyWaypoints"),
    ("minnie", "__minniePaths", "__minnieWaypoints"),
    ("goofy", "__goofyPaths", "__goofyWaypoints"),
    ("goofySpeedway", "__goofySpeedwayPaths", "__goofySpeedwayWaypoints"),
    ("donald", "__donaldPaths", "__donaldWaypoints"),
    ("pluto", "__plutoPaths", "__plutoWaypoints"),
    ("daisy", "__daisyPaths", "__daisyWaypoints"),
    ("chip", "__chipPaths", "__chipWaypoints"),
]


def fl(v):
    return repr(float(v)) + "F"


def p3(t):
    return "{ " + fl(t[0]) + ", " + fl(t[1]) + ", " + fl(t[2]) + " }"


def neighbors(n):
    if isinstance(n, str):
        n = (n,)  # Normalize bare-string neighbor to a one-element list
    return "{ " + ", ".join(f'"{c}"' for c in n) + " }"


out = []
for key, pv, wv in chars:
    paths, wps = ns[pv], ns[wv]
    out.append(f"  static const CharPaths {key} = {{")
    out.append("    // nodes: name -> { position, neighbours }")
    out.append("    {")
    for node, (pos, nb) in paths.items():
        out.append(f'      {{ "{node}", {{ {p3(pos.t)}, {neighbors(nb)} }} }},')
    out.append("    },")
    out.append("    // waypoints: from, to, raycast, points")
    out.append("    {")
    for frm, to, ray, pts in wps:
        ptsC = ("{ " + ", ".join(p3(p.t) for p in pts) + " }") if pts else "{}"
        out.append(f'      {{ "{frm}", "{to}", {ray}, {ptsC} }},')
    out.append("    },")
    out.append("  };")
    out.append("")
out.append("  static const CharPaths donaldDock = {")
out.append('    { { "a", { { 0.0F, 0.0F, 0.0F }, { "a" } } } },')
out.append("    {},")
out.append("  };")

os.makedirs(os.path.dirname(out_path), exist_ok=True)
open(out_path, "w").write("\n".join(out) + "\n")
print(f"wrote {out_path}")
print("nodes per char:", {k: len(ns[pv]) for k, pv, _ in chars})
