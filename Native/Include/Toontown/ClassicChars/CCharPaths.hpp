// CharPaths is an opaque token: callers receive one from the Python shim's
// getPaths() and pass it back to these queries.  They never inspect it, so the
// data never needs to cross a boundary as a Python structure.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "Toontown/Math/Point3.hpp"

namespace Toontown::ClassicChars
{
  struct PathNode
  {
    Math::Point3             m_Pos;
    std::vector<std::string> m_Neighbors;
  };

  struct Waypoint
  {
    std::string               m_From;
    std::string               m_To;
    int                       m_RayCast;
    std::vector<Math::Point3> m_Points;
  };

  struct CharPaths
  {
    std::map<std::string, PathNode> m_Nodes;
    std::vector<Waypoint>           m_Waypoints;
  };

  // key is a stable identifier ("mickey" ... "donaldDock"); the Python shim
  // maps TTLocalizer character names onto these.
  const CharPaths & GetCharPaths( const std::string & key );

  Math::Point3 GetNodePos( const std::string & node, const CharPaths & paths );

  std::vector<std::string> GetAdjacentNodes( const std::string & node,
                                             const CharPaths &   paths );

  std::vector<Math::Point3> GetWaypoints( const std::string & from,
                                          const std::string & to,
                                          const CharPaths &   paths );

  bool GetRayCastFlag( const std::string & from,
                       const std::string & to,
                       const CharPaths &   paths );

  std::vector<Math::Point3> GetPointsFromTo( const std::string & from,
                                             const std::string & to,
                                             const CharPaths &   paths );

  double GetWalkDuration( const std::string & from,
                          const std::string & to,
                          double              velocity,
                          const CharPaths &   paths );

  double GetWalkDistance( const std::string & from,
                          const std::string & to,
                          double              velocity,
                          const CharPaths &   paths );
}  // namespace Toontown::ClassicChars
