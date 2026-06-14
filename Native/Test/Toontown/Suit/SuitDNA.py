import sys

from direct.distributed.PyDatagram import PyDatagram
from toontown_cpp.suit import (
    suitHeadTypes, suitDepts, getSuitBodyType, getSuitDept, getSuitType,
    getRandomSuitType, getRandomSuitByDept, SuitDNA,
)

# Reference tables (verbatim from the original)
A = ['ym', 'hh', 'tbc', 'dt', 'bs', 'le', 'bw', 'pp', 'nc', 'rb', 'nd', 'tf', 'm', 'mh']
B = ['p', 'ds', 'b', 'ac', 'sd', 'bc', 'ls', 'tm', 'ms']
C = ['f', 'mm', 'cr', 'bf', 'sc', 'tw', 'mb', 'cc', 'gh']
DEPTS = ['c', 'l', 'm', 's']
SPD = 8

fails = 0


def chk(label, cond):
    global fails
    if not cond:
        fails += 1
        print(f"FAIL {label}")


def ref_body(n):
    if n in A:
        return 'a'
    if n in B:
        return 'b'
    if n in C:
        return 'c'
    return None


def ref_dept(n):
    i = list(suitHeadTypes).index(n)
    return DEPTS[i // SPD]


def ref_type(n):
    return list(suitHeadTypes).index(n) % SPD + 1


# --- Deterministic: classifiers, bit-exact over every cog type ----------------
for n in list(suitHeadTypes):
    chk(f"getSuitBodyType[{n}]", getSuitBodyType(n) == ref_body(n))
    chk(f"getSuitDept[{n}]", getSuitDept(n) == ref_dept(n))
    chk(f"getSuitType[{n}]", getSuitType(n) == ref_type(n))


# --- Deterministic: net-string wire format vs real PyDatagram -----------------
def ref_net_suit(name, dept):
    dg = PyDatagram()
    dg.addFixedString('s', 1)
    dg.addFixedString(name, 3)
    dg.addFixedString(dept, 1)
    return dg.getMessage()


def ref_net_boss(dept):
    dg = PyDatagram()
    dg.addFixedString('b', 1)
    dg.addFixedString(dept, 1)
    return dg.getMessage()


for name in ['ds', 'tbc', 'm', 'f', 'mh']:  # 2, 3, 1-char names
    dept = getSuitDept(name)
    dna = SuitDNA()
    dna.newSuit(name)
    chk(f"makeNetString suit[{name}]", dna.makeNetString() == ref_net_suit(name, dept))
    # round-trip recovers fields (name null-stripped)
    rt = SuitDNA(dna.makeNetString())
    chk(f"roundtrip suit[{name}]",
        (rt.type, rt.name, rt.dept, rt.body) == ('s', name, dept, getSuitBodyType(name)))

for dept in DEPTS:
    dna = SuitDNA()
    dna.newBossCog(dept)
    chk(f"makeNetString boss[{dept}]", dna.makeNetString() == ref_net_boss(dept))
    rt = SuitDNA(dna.makeNetString())
    chk(f"roundtrip boss[{dept}]", (rt.type, rt.dept) == ('b', dept))

# --- RNG: invariants and coverage, not sequence -------------------------------
for level in range(1, 13):
    lo, hi = max(level - 4, 1), min(level, 8)
    seen = set()
    for _ in range(5000):
        v = getRandomSuitType(level)
        chk(f"getRandomSuitType range[{level}]", lo <= v <= hi)
        seen.add(v)
    chk(f"getRandomSuitType coverage[{level}]", seen == set(range(lo, hi + 1)))

for dept in DEPTS:
    di = DEPTS.index(dept)
    deptset = set(list(suitHeadTypes)[SPD * di:SPD * di + 8])
    seen = set()
    for _ in range(5000):
        x = getRandomSuitByDept(dept)
        chk(f"getRandomSuitByDept membership[{dept}]", x in deptset)
        seen.add(x)
    chk(f"getRandomSuitByDept coverage[{dept}]", seen == deptset)

for _ in range(20000):
    dna = SuitDNA()
    dna.newSuitRandom()  # defaults
    chk("newSuitRandom default valid",
        dna.type == 's' and dna.name in list(suitHeadTypes) and dna.dept in DEPTS
        and dna.body == getSuitBodyType(dna.name))

for dept in DEPTS:
    for level in range(1, 8):
        dna = SuitDNA()
        dna.newSuitRandom(level, dept)
        chk(f"newSuitRandom[{level},{dept}]",
            dna.dept == dept and getSuitDept(dna.name) == dept
            and dna.body == getSuitBodyType(dna.name))

if fails:
    print(f"\n{fails} failure(s).")
    sys.exit(1)

print("All checks passed: SuitDNA deterministic logic is bit-identical; RNG holds invariants.")
