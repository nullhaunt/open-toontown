#include "Registry.hpp"

NB_MODULE( toontown_cpp, module )
{
  auto minigame = module.def_submodule( "minigame" );
  RegisterMinigameDropScheduler( minigame );

  auto safezone = module.def_submodule( "safezone" );
  RegisterSafeZoneCheckerboard( safezone );

  auto math = module.def_submodule( "math" );
  RegisterMathBoundary( math );

  auto classicchars = module.def_submodule( "classicchars" );
  RegisterClassicCharsCCharPaths( classicchars );

  auto suit = module.def_submodule( "suit" );
  RegisterSuitDNA( suit );
}
