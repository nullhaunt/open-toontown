from toontown_cpp.classicchars import (
    CharPaths,
    getNodePos,
    getAdjacentNodes,
    getWayPoints,
    getRaycastFlag,
    getPointsFromTo,
    getWalkDuration,
    getWalkDistance,
    _getCharPaths,
)

from toontown.toonbase import TTLocalizer

DaleOrbitDistanceOverride = {('b', 'c'): 2.5,
                             ('e', 'f'): 2.5}
startNode = 'a'


def getPaths(charName, location=0):
    if charName == TTLocalizer.Mickey:
        return _getCharPaths('mickey')
    elif charName == TTLocalizer.VampireMickey:
        return _getCharPaths('mickey')
    elif charName == TTLocalizer.Minnie:
        return _getCharPaths('minnie')
    elif charName == TTLocalizer.WitchMinnie:
        return _getCharPaths('minnie')
    elif charName == TTLocalizer.Daisy or charName == TTLocalizer.SockHopDaisy:
        return _getCharPaths('daisy')
    elif charName == TTLocalizer.Goofy:
        return _getCharPaths('goofy' if location == 0 else 'goofySpeedway')
    elif charName == TTLocalizer.SuperGoofy:
        return _getCharPaths('goofySpeedway')
    elif charName == TTLocalizer.Donald or charName == TTLocalizer.FrankenDonald:
        return _getCharPaths('donald')
    elif charName == TTLocalizer.Pluto:
        return _getCharPaths('pluto')
    elif charName == TTLocalizer.WesternPluto:
        return _getCharPaths('pluto')
    elif charName == TTLocalizer.Chip or charName == TTLocalizer.PoliceChip:
        return _getCharPaths('chip')
    elif charName == TTLocalizer.Dale or charName == TTLocalizer.JailbirdDale:
        return _getCharPaths('chip')
    elif charName == TTLocalizer.DonaldDock:
        return _getCharPaths('donaldDock')
    return None


__all__ = ['CharPaths', 'getPaths', 'getNodePos', 'getAdjacentNodes',
           'getWayPoints', 'getRaycastFlag', 'getPointsFromTo',
           'getWalkDuration', 'getWalkDistance', 'startNode',
           'DaleOrbitDistanceOverride']
