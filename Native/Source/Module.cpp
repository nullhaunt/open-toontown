#include "Registry.hpp"

NB_MODULE( toontown_cpp, module )
{
  auto minigame = module.def_submodule( "minigame" );
  RegisterMinigameDropScheduler( minigame );
}
