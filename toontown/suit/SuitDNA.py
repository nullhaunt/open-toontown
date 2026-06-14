"""
Identity logic and tables live in toontown_cpp.suit.  Localization and engine
types remain here.
"""
from panda3d.core import VBase4
from toontown_cpp.suit import (
    suitHeadTypes,
    suitATypes,
    suitBTypes,
    suitCTypes,
    suitDepts,
    suitsPerLevel,
    suitsPerDept,
    goonTypes,
    getSuitBodyType,
    getSuitDept,
    getSuitType,
    getRandomSuitType,
    getRandomSuitByDept,
    SuitDNA,
)

from toontown.toonbase import TTLocalizer

suitDeptFullnames = {'c': TTLocalizer.Bossbot,
                     'l': TTLocalizer.Lawbot,
                     'm': TTLocalizer.Cashbot,
                     's': TTLocalizer.Sellbot}

suitDeptFullnamesP = {'c': TTLocalizer.BossbotP,
                      'l': TTLocalizer.LawbotP,
                      'm': TTLocalizer.CashbotP,
                      's': TTLocalizer.SellbotP}

corpPolyColor = VBase4(0.95, 0.75, 0.75, 1.0)
legalPolyColor = VBase4(0.75, 0.75, 0.95, 1.0)
moneyPolyColor = VBase4(0.65, 0.95, 0.85, 1.0)
salesPolyColor = VBase4(0.95, 0.75, 0.95, 1.0)


def getDeptFullname(dept):
    return suitDeptFullnames[dept]


def getDeptFullnameP(dept):
    return suitDeptFullnamesP[dept]


def getSuitDeptFullname(name):
    return suitDeptFullnames[getSuitDept(name)]
