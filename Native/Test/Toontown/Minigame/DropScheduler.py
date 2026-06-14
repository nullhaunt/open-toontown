import sys

from toontown_cpp.minigame import DropScheduler as CppDS
from toontown_cpp.minigame import ThreePhaseDropScheduler as CppTP


# --- Reference ----------------------------------------------------------------
class PyDS:
    def __init__(self, gameDuration, firstDropDelay, dropPeriod, maxDropDuration,
                 fasterDropDelay, fasterDropPeriodMult, startTime=None):
        self.gameDuration = gameDuration
        self.firstDropDelay = firstDropDelay
        self._dropPeriod = dropPeriod
        self.maxDropDuration = maxDropDuration
        self.fasterDropDelay = fasterDropDelay
        self.fasterDropPeriodMult = fasterDropPeriodMult
        self._startTime = 0 if startTime is None else startTime
        self.curT = self._startTime + self.firstDropDelay

    def getT(self):
        return self.curT

    def getDuration(self):
        return self.gameDuration

    def getDropPeriod(self):
        delay = self._dropPeriod
        if self.curT - self._startTime >= self.fasterDropDelay:
            delay *= self.fasterDropPeriodMult
        return delay

    def doneDropping(self, continuous=None):
        landTime = self.getT() - self._startTime + self.maxDropDuration
        continuous = continuous is not None
        if continuous:
            maxTime = self.gameDuration + self.maxDropDuration
        else:
            maxTime = self.gameDuration + self.getDropPeriod()
        return landTime >= maxTime

    def skipPercent(self, percent):
        numSkips = 0
        while True:
            prevT = self.curT
            self.stepT()
            if self.curT >= percent * self.gameDuration:
                self.curT = prevT
                break
            numSkips += 1
        return numSkips

    def stepT(self):
        self.curT += self.getDropPeriod()


class PyTP(PyDS):
    def __init__(self, gameDuration, firstDropDelay, dropPeriod, maxDropDuration,
                 slowerDropPeriodMult, normalDropDelay, fasterDropDelay,
                 fasterDropPeriodMult, startTime=None):
        self._slowerDropPeriodMult = slowerDropPeriodMult
        self._normalDropDelay = normalDropDelay
        PyDS.__init__(self, gameDuration, firstDropDelay, dropPeriod,
                      maxDropDuration, fasterDropDelay, fasterDropPeriodMult,
                      startTime)

    def getDropPeriod(self):
        delay = self._dropPeriod
        if self.curT - self._startTime < self._normalDropDelay:
            delay *= self._slowerDropPeriodMult
        elif self.curT - self._startTime >= self.fasterDropDelay:
            delay *= self.fasterDropPeriodMult
        return delay


# --- Harness ------------------------------------------------------------------
fails = 0


def chk(label, a, b):
    global fails
    if a != b:
        fails += 1
        print(f"MISMATCH {label}: py={a!r} cpp={b!r}")


def run_pair(py, cpp, label):
    chk(f"{label}.getDuration", py.getDuration(), cpp.getDuration())
    # Walk the schedule, comparing every step.
    for i in range(2000):
        chk(f"{label}.getT[{i}]", py.getT(), cpp.getT())
        chk(f"{label}.getDropPeriod[{i}]", py.getDropPeriod(), cpp.getDropPeriod())
        chk(f"{label}.done()[{i}]", py.doneDropping(), cpp.doneDropping())
        chk(f"{label}.done(None)[{i}]", py.doneDropping(None), cpp.doneDropping(None))
        chk(f"{label}.done(True)[{i}]", py.doneDropping(True), cpp.doneDropping(True))
        chk(f"{label}.done(False)[{i}]", py.doneDropping(False), cpp.doneDropping(False))
        chk(f"{label}.done(c=True)[{i}]",
            py.doneDropping(continuous=True), cpp.doneDropping(continuous=True))
        if py.doneDropping():
            break
        py.stepT()
        cpp.stepT()


ds_params = [
    (60.0, 1.0, 0.5, 2.0, 30.0, 0.5),
    (45.0, 0.0, 0.3, 1.5, 10.0, 0.25, 5.0),  # with startTime
    (120.0, 2.0, 1.0, 3.0, 60.0, 0.75, None),  # explicit None startTime
]
for p in ds_params:
    run_pair(PyDS(*p), CppDS(*p), f"DS{p}")

tp_params = [
    (60.0, 1.0, 0.5, 2.0, 2.0, 15.0, 40.0, 0.5),
    (90.0, 0.0, 0.4, 2.5, 1.5, 10.0, 50.0, 0.5, 3.0),
]
for p in tp_params:
    run_pair(PyTP(*p), CppTP(*p), f"TP{p}")

# skipPercent on fresh instances (it mutates curT).
for pct in (0.1, 0.25, 0.5, 0.9):
    for p in ds_params:
        chk(f"DS.skipPercent({pct}){p}", PyDS(*p).skipPercent(pct), CppDS(*p).skipPercent(pct))
    for p in tp_params:
        chk(f"TP.skipPercent({pct}){p}", PyTP(*p).skipPercent(pct), CppTP(*p).skipPercent(pct))

if fails:
    print(f"\n{fails} mismatch(es).")
    sys.exit(1)

print("All checks passed: C++ output is identical to the Python.")
