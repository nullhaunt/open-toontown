#include "Toontown/ClassicChars/CCharPaths.hpp"

#include <stdexcept>

#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "Toontown/Math/PandaConvert.hpp"
#include "Registry.hpp"

namespace Toontown::ClassicChars
{
  namespace
  {
#include "Toontown/ClassicChars/CCharPaths.data.inc"
  }

  const CharPaths& GetCharPaths( const std::string& key )
  {
    if ( key == "mickey" )
    {
      return mickey;
    }

    if ( key == "minnie" )
    {
      return minnie;
    }

    if ( key == "goofy" )
    {
      return goofy;
    }

    if ( key == "goofySpeedway" )
    {
      return goofySpeedway;
    }

    if ( key == "donald" )
    {
      return donald;
    }

    if ( key == "pluto" )
    {
      return pluto;
    }

    if ( key == "daisy" )
    {
      return daisy;
    }

    if ( key == "chip" )
    {
      return chip;
    }

    if ( key == "donaldDock" )
    {
      return donaldDock;
    }

    throw std::out_of_range( "CCharPaths: unknown key " + key );
  }

  Math::Point3 GetNodePos( const std::string& node, const CharPaths& paths )
  {
    return paths.m_Nodes.at( node ).m_Pos;
  }
  std::vector<std::string> GetAdjacentNodes( const std::string& node,
                                             const CharPaths&   paths )
  {
    // The original code returned either a tuple or (for some nodes) a bare
    // string; callers only consume this via random.choice, where a one-element
    // list is equivalent, so neighbors are normalized to a list (see data
    // generator).
    return paths.m_Nodes.at( node ).m_Neighbors;
  }

  std::vector<Math::Point3> GetWaypoints( const std::string& from,
                                          const std::string& to,
                                          const CharPaths&   paths )
  {
    std::vector<Math::Point3> result;

    if ( from != to )
    {
      for ( const auto& waypoint : paths.m_Waypoints )
      {
        if ( waypoint.m_From == from && waypoint.m_To == to )
        {
          result = waypoint.m_Points;
          break;
        }

        if ( waypoint.m_From == to && waypoint.m_To == from )
        {
          // Reverse case: the original prepends each point (i.e., reversed
          // order).
          result.assign( waypoint.m_Points.rbegin(), waypoint.m_Points.rend() );
          break;
        }
      }
    }

    return result;
  }

  bool GetRayCastFlag( const std::string& from,
                       const std::string& to,
                       const CharPaths&   paths )
  {
    bool result = false;

    if ( from != to )
    {
      for ( const auto& waypoint : paths.m_Waypoints )
      {
        // Mirrors Python's if/elif: a forward match with a falsy flag does
        // not fallthrough to the reverse check, and does not break.
        if ( waypoint.m_From == from && waypoint.m_To == to )
        {
          if ( waypoint.m_RayCast )
          {
            result = true;
            break;
          }
        }
        else if ( waypoint.m_From == to && waypoint.m_To == from )
        {
          if ( waypoint.m_RayCast )
          {
            result = true;
            break;
          }
        }
      }
    }

    return result;
  }

  std::vector<Math::Point3> GetPointsFromTo( const std::string& from,
                                             const std::string& to,
                                             const CharPaths&   paths )
  {
    std::vector<Math::Point3> result;
    result.push_back( GetNodePos( from, paths ) );

    const auto mid = GetWaypoints( from, to, paths );
    result.insert( result.end(), mid.begin(), mid.end() );

    result.push_back( GetNodePos( to, paths ) );
    return result;
  }

  double GetWalkDuration( const std::string& from,
                          const std::string& to,
                          double             velocity,
                          const CharPaths&   paths )
  {
    const auto points   = GetPointsFromTo( from, to, paths );
    double     duration = 0.0;

    for ( std::size_t i = 0; i + 1 < points.size(); ++i )
    {
      // Segment length at float precision (Panda), accumulated in double
      // (Python).
      const float distance  = ( points[ i + 1 ] - points[ i ] ).Length();
      duration             += static_cast<double>( distance ) / velocity;
    }

    return duration;
  }

  double GetWalkDistance( const std::string&      from,
                          const std::string&      to,
                          [[maybe_unused]] double velocity,
                          const CharPaths&        paths )
  {
    // velocity is accepted to match the original signature, but unused there
    // too.
    const auto points = GetPointsFromTo( from, to, paths );
    double     total  = 0.0;

    for ( std::size_t i = 0; i + 1 < points.size(); ++i )
    {
      const float distance  = ( points[ i + 1 ] - points[ i ] ).Length();
      total                += static_cast<double>( distance );
    }

    return total;
  }
}  // namespace Toontown::ClassicChars

void RegisterClassicCharsCCharPaths( nanobind::module_& classicchars )
{
  using namespace Toontown::ClassicChars;

  // Opaque token returned by the shim's getPaths and passed back to the
  // queries.
  nanobind::class_<CharPaths>( classicchars, "CharPaths" );

  classicchars.def( "_getCharPaths",
                    &GetCharPaths,
                    nanobind::arg( "key" ),
                    nanobind::rv_policy::reference );
  classicchars.def( "getNodePos",
                    &GetNodePos,
                    nanobind::arg( "node" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getAdjacentNodes",
                    &GetAdjacentNodes,
                    nanobind::arg( "node" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getWayPoints",
                    &GetWaypoints,
                    nanobind::arg( "fromNode" ),
                    nanobind::arg( "toNode" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getRaycastFlag",
                    &GetRayCastFlag,
                    nanobind::arg( "fromNode" ),
                    nanobind::arg( "toNode" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getPointsFromTo",
                    &GetPointsFromTo,
                    nanobind::arg( "fromNode" ),
                    nanobind::arg( "toNode" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getWalkDuration",
                    &GetWalkDuration,
                    nanobind::arg( "fromNode" ),
                    nanobind::arg( "toNode" ),
                    nanobind::arg( "velocity" ),
                    nanobind::arg( "paths" ) );
  classicchars.def( "getWalkDistance",
                    &GetWalkDistance,
                    nanobind::arg( "fromNode" ),
                    nanobind::arg( "toNode" ),
                    nanobind::arg( "velocity" ),
                    nanobind::arg( "paths" ) );
}
