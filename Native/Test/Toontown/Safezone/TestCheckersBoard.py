import sys

from toontown_cpp.safezone import CheckersBoard as CppBoard


# --- Reference ----------------------------------------------------------------
class PyBoard:
    def __init__(self):
        self.squareList = []
        for x in range(32):
            self.squareList.append(PyTile(x))

        adj = [
            [None, None, 4, None], [None, 4, 5, None], [None, 5, 6, None],
            [None, 6, 7, None], [0, 8, 9, 1], [1, 9, 10, 2], [2, 10, 11, 3],
            [3, 11, None, None], [None, None, 12, 4], [4, 12, 13, 5],
            [5, 13, 14, 6], [6, 14, 15, 7], [8, 16, 17, 9], [9, 17, 18, 10],
            [10, 18, 19, 11], [11, 19, None, None], [None, None, 20, 12],
            [12, 20, 21, 13], [13, 21, 22, 14], [14, 22, 23, 15],
            [16, 24, 25, 17], [17, 25, 26, 18], [18, 26, 27, 19],
            [19, 27, None, None], [None, None, 28, 20], [20, 28, 29, 21],
            [21, 29, 30, 22], [22, 30, 31, 23], [24, None, None, 25],
            [25, None, None, 26], [26, None, None, 27], [27, None, None, None],
        ]
        jmp = [
            [None, None, 9, None], [None, 8, 10, None], [None, 9, 11, None],
            [None, 10, None, None], [None, None, 13, None], [None, 12, 14, None],
            [None, 13, 15, None], [None, 14, None, None], [None, None, 17, 1],
            [0, 16, 18, 2], [1, 17, 19, 3], [2, 18, None, None],
            [None, None, 21, 5], [4, 20, 22, 6], [5, 21, 23, 7],
            [6, 22, None, None], [None, None, 25, 9], [8, 24, 26, 10],
            [9, 25, 27, 11], [10, 26, None, None], [None, None, 29, 13],
            [12, 28, 30, 14], [13, 29, 31, 15], [14, 30, None, None],
            [None, None, None, 17], [16, None, None, 18], [17, None, None, 19],
            [18, None, None, None], [None, None, None, 21], [20, None, None, 22],
            [21, None, None, 23], [22, None, None, None],
        ]
        for x in range(32):
            self.squareList[x].setAdjacent(adj[x])
            self.squareList[x].setJumps(jmp[x])

    def getSquare(self, arrayLoc):
        return self.squareList[arrayLoc]

    def getState(self, squareNum):
        return self.squareList[squareNum].getState()

    def setState(self, squareNum, newState):
        self.squareList[squareNum].setState(newState)

    def getAdjacent(self, squareNum):
        return self.squareList[squareNum].adjacent

    def getStates(self):
        retList = []
        for x in range(32):
            retList.append(self.squareList[x].getState())
        return retList

    def setStates(self, squares):
        for x in range(32):
            self.squareList[x].setState(squares[x])

    def getJumps(self, squareNum):
        return self.squareList[squareNum].jumps


class PyTile:
    def __init__(self, tileNum):
        self.tileNum = tileNum
        self.state = 0
        self.adjacent = []
        self.jumps = []

    def setJumps(self, jumpList):
        for x in jumpList:
            self.jumps.append(x)

    def getJumps(self):
        return self.jumps

    def setAdjacent(self, adjList):
        for x in adjList:
            self.adjacent.append(x)

    def getAdjacent(self):
        return self.adjacent

    def setState(self, newState):
        self.state = newState

    def getState(self):
        return self.state

    def getNum(self):
        return self.tileNum


# --- Harness ------------------------------------------------------------------
fails = 0


def chk(label, a, b):
    global fails
    if a != b:
        fails += 1
        print(f"MISMATCH {label}: py={a!r} cpp={b!r}")


def compare_static(py, cpp):
    chk("getStates", py.getStates(), list(cpp.getStates()))
    for sq in range(32):
        chk(f"board.getState[{sq}]", py.getState(sq), cpp.getState(sq))
        chk(f"board.getAdjacent[{sq}]",
            list(py.getAdjacent(sq)), list(cpp.getAdjacent(sq)))
        chk(f"board.getJumps[{sq}]",
            list(py.getJumps(sq)), list(cpp.getJumps(sq)))
        pt, ct = py.squareList[sq], cpp.squareList[sq]
        chk(f"tile.getNum[{sq}]", pt.getNum(), ct.getNum())
        chk(f"tile.getAdjacent[{sq}]",
            list(pt.getAdjacent()), list(ct.getAdjacent()))
        chk(f"tile.getJumps[{sq}]", list(pt.getJumps()), list(ct.getJumps()))
        chk(f"tile.adjacent attr[{sq}]", list(pt.adjacent), list(ct.adjacent))
        chk(f"tile.jumps attr[{sq}]", list(pt.jumps), list(ct.jumps))
        chk(f"getSquare.getNum[{sq}]",
            py.getSquare(sq).getNum(), cpp.getSquare(sq).getNum())


def compare_list_ops(py, cpp):
    # The exact operations the callers run on adjacency/jump lists.
    for sq in range(32):
        pa, ca = py.squareList[sq].getAdjacent(), cpp.squareList[sq].getAdjacent()
        for v in range(32):
            chk(f"adj in[{sq},{v}]", v in pa, v in ca)
            if v in pa:
                chk(f"adj index[{sq},{v}]", pa.index(v), ca.index(v))
        pj, cj = py.squareList[sq].getJumps(), cpp.squareList[sq].getJumps()
        for v in range(32):
            chk(f"jmp in[{sq},{v}]", v in pj, v in cj)
            if v in pj:
                chk(f"jmp index[{sq},{v}]", pj.index(v), cj.index(v))


# Static structure + read operations.
compare_static(PyBoard(), CppBoard())
compare_list_ops(PyBoard(), CppBoard())

# Reference semantics: every mutation path must show up in getStates().
py, cpp = PyBoard(), CppBoard()
py.squareList[5].setState(3);
cpp.squareList[5].setState(3)
py.getSquare(7).setState(2);
cpp.getSquare(7).setState(2)
py.setState(9, 4);
cpp.setState(9, 4)
py.squareList[31].setState(1);
cpp.squareList[31].setState(1)
chk("mutated getStates", py.getStates(), list(cpp.getStates()))
chk("mutated getState(5) via squareList", py.squareList[5].getState(), cpp.squareList[5].getState())
chk("mutated getSquare(7)", py.getSquare(7).getState(), cpp.getSquare(7).getState())

# setStates round-trip with a non-trivial pattern.
pattern = [(i * 7) % 5 for i in range(32)]
py, cpp = PyBoard(), CppBoard()
py.setStates(pattern)
cpp.setStates(pattern)
chk("setStates getStates", py.getStates(), list(cpp.getStates()))

if fails:
    print(f"\n{fails} mismatch(es).")
    sys.exit(1)

print("All checks passed: C++ output is identical to the Python.")
