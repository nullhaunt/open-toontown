#pragma once

#include <nanobind/nanobind.h>

void RegisterMinigameDropScheduler( nanobind::module_& minigame );
void RegisterSafezoneCheckersBoard( nanobind::module_& safezone );
void RegisterMathBoundary( nanobind::module_& math );
void RegisterClassicCharsCCharPaths( nanobind::module_& classicchars );
