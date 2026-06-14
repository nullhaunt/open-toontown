#pragma once

#include <nanobind/nanobind.h>

void RegisterMinigameDropScheduler( nanobind::module_ & minigame );
void RegisterSafeZoneCheckerboard( nanobind::module_ & safeZone );
void RegisterMathBoundary( nanobind::module_ & math );
void RegisterClassicCharsCCharPaths( nanobind::module_ & classicChars );
void RegisterSuitDNA( nanobind::module_ & suit );
