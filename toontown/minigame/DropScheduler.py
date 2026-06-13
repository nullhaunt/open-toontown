"""
Strangler shim: the implementation now lives in toontown_cpp.minigame.

This preserves the `toontown.minigame.DropScheduler` import path so the call
sites (DistributedCatchGame, DropPlacer, DistributedPartyCatchActivityBase)
need no edits.  This will be removed once the callers import toontown_cpp
directly.
"""

from toontown_cpp.minigame import DropScheduler, ThreePhaseDropScheduler

__all__ = ['DropScheduler', 'ThreePhaseDropScheduler']
