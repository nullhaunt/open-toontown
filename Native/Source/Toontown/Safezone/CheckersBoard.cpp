#include "Toontown/Safezone/CheckersBoard.hpp"

#include <nanobind/stl/optional.h>
#include <nanobind/stl/vector.h>

#include "Registry.hpp"

namespace Toontown::Safezone
{
  namespace
  {
    constexpr int NONE = -1;

    constexpr int ADJACENT[ 32 ][ 4 ] = {
      { NONE, NONE, 4, NONE },
      { NONE, 4, 5, NONE },

      { NONE, 5, 6, NONE },
      { NONE, 6, 7, NONE },

      { 0, 8, 9, 1 },
      { 1, 9, 10, 2 },

      { 2, 10, 11, 3 },
      { 3, 11, NONE, NONE },

      { NONE, NONE, 12, 4 },
      { 4, 12, 13, 5 },

      { 5, 13, 14, 6 },
      { 6, 14, 15, 7 },

      { 8, 16, 17, 9 },
      { 9, 17, 18, 10 },

      { 10, 18, 19, 11 },
      { 11, 19, NONE, NONE },

      { NONE, NONE, 20, 12 },
      { 12, 20, 21, 13 },

      { 13, 21, 22, 14 },
      { 14, 22, 23, 15 },

      { 16, 24, 25, 17 },
      { 17, 25, 26, 18 },

      { 18, 26, 27, 19 },
      { 19, 27, NONE, NONE },

      { NONE, NONE, 28, 20 },
      { 20, 28, 29, 21 },

      { 21, 29, 30, 22 },
      { 22, 30, 31, 23 },

      { 24, NONE, NONE, 25 },
      { 25, NONE, NONE, 26 },

      { 26, NONE, NONE, 27 },
      { 27, NONE, NONE, NONE },
    };

    constexpr int JUMPS[ 32 ][ 4 ] = {
      { NONE, NONE, 9, NONE },
      { NONE, 8, 10, NONE },

      { NONE, 9, 11, NONE },
      { NONE, 10, NONE, NONE },

      { NONE, NONE, 13, NONE },
      { NONE, 12, 14, NONE },

      { NONE, 13, 15, NONE },
      { NONE, 14, NONE, NONE },

      { NONE, NONE, 17, 1 },
      { 0, 16, 18, 2 },

      { 1, 17, 19, 3 },
      { 2, 18, NONE, NONE },

      { NONE, NONE, 21, 5 },
      { 4, 20, 22, 6 },

      { 5, 21, 23, 7 },
      { 6, 22, NONE, NONE },

      { NONE, NONE, 25, 9 },
      { 8, 24, 26, 10 },

      { 9, 25, 27, 11 },
      { 10, 26, NONE, NONE },

      { NONE, NONE, 29, 13 },
      { 12, 28, 30, 14 },

      { 13, 29, 31, 15 },
      { 14, 30, NONE, NONE },

      { NONE, NONE, NONE, 17 },
      { 16, NONE, NONE, 18 },

      { 17, NONE, NONE, 19 },
      { 18, NONE, NONE, NONE },

      { NONE, NONE, NONE, 21 },
      { 20, NONE, NONE, 22 },

      { 21, NONE, NONE, 23 },
      { 22, NONE, NONE, NONE },
    };

    std::vector<std::optional<int>> ToOptional( const int row[ 4 ] )
    {
      std::vector<std::optional<int>> result;
      result.reserve( 4 );

      for ( int i = 0; i < 4; ++i )
      {
        result.push_back( row[ i ] == NONE ? std::nullopt
                                           : std::optional( row[ i ] ) );
      }

      return result;
    }
  }  // namespace

  CheckersTile::CheckersTile( int tile )
    : m_Tile( tile )
    , m_State( 0 )
  {
  }

  std::vector<std::optional<int>> CheckersTile::GetAdjacent() const
  {
    return m_Adjacent;
  }

  std::vector<std::optional<int>> CheckersTile::GetJumps() const
  {
    return m_Jumps;
  }

  void CheckersTile::SetAdjacent(
    const std::vector<std::optional<int>>& adjList )
  {
    for ( const auto& x : adjList )
    {
      m_Adjacent.push_back( x );
    }
  }

  void CheckersTile::SetJumps( const std::vector<std::optional<int>>& jumpList )
  {
    for ( const auto& x : jumpList )
    {
      m_Jumps.push_back( x );
    }
  }

  int CheckersTile::GetState() const
  {
    return m_State;
  }

  void CheckersTile::SetState( int state )
  {
    m_State = state;
  }

  int CheckersTile::GetTile() const
  {
    return m_Tile;
  }

  void CheckersTile::Delete() {}

  CheckersBoard::CheckersBoard()
  {
    m_SquareList.reserve( 32 );  // Keep element references stable

    for ( int x = 0; x < 32; ++x )
    {
      m_SquareList.emplace_back( x );
    }

    for ( int x = 0; x < 32; ++x )
    {
      m_SquareList[ x ].SetAdjacent( ToOptional( ADJACENT[ x ] ) );
      m_SquareList[ x ].SetJumps( ToOptional( JUMPS[ x ] ) );
    }
  }

  CheckersTile& CheckersBoard::GetSquare( int arrayLoc )
  {
    return m_SquareList[ arrayLoc ];
  }

  int CheckersBoard::GetState( int square ) const
  {
    return m_SquareList[ square ].GetState();
  }

  void CheckersBoard::SetState( int square, int state )
  {
    m_SquareList[ square ].SetState( state );
  }

  std::vector<std::optional<int>> CheckersBoard::GetAdjacent( int square ) const
  {
    return m_SquareList[ square ].GetAdjacent();
  }

  std::vector<std::optional<int>> CheckersBoard::GetJumps( int square ) const
  {
    return m_SquareList[ square ].GetJumps();
  }

  std::vector<int> CheckersBoard::GetStates() const
  {
    std::vector<int> result;
    result.reserve( 32 );

    for ( int x = 0; x < 32; ++x )
    {
      result.push_back( m_SquareList[ x ].GetState() );
    }

    return result;
  }

  void CheckersBoard::SetStates( const std::vector<int>& squares )
  {
    for ( int x = 0; x < 32; ++x )
    {
      m_SquareList[ x ].SetState( squares.at( static_cast<std::size_t>( x ) ) );
    }
  }

  void CheckersBoard::Delete() {}

  std::vector<CheckersTile>& CheckersBoard::SquareList()
  {
    return m_SquareList;
  }
}  // namespace Toontown::Safezone

void RegisterSafezoneCheckersBoard( nanobind::module_& safezone )
{
  using namespace Toontown::Safezone;

  nanobind::class_<CheckersTile>( safezone, "CheckersTile" )
    .def( nanobind::init<int>(), nanobind::arg( "tileNum" ) )
    .def( "getAdjacent", &CheckersTile::GetAdjacent )
    .def( "getJumps", &CheckersTile::GetJumps )
    .def(
      "setAdjacent", &CheckersTile::SetAdjacent, nanobind::arg( "adjList" ) )
    .def( "setJumps", &CheckersTile::SetJumps, nanobind::arg( "jumpList" ) )
    .def( "getState", &CheckersTile::GetState )
    .def( "setState", &CheckersTile::SetState, nanobind::arg( "newState" ) )
    .def( "getNum", &CheckersTile::GetTile )
    .def( "delete", &CheckersTile::Delete )
    .def_prop_ro( "adjacent", &CheckersTile::GetAdjacent )
    .def_prop_ro( "jumps", &CheckersTile::GetJumps )
    .def_prop_ro( "tileNum", &CheckersTile::GetTile )
    .def_prop_ro( "state", &CheckersTile::GetState );

  nanobind::class_<CheckersBoard>( safezone, "CheckersBoard" )
    .def( nanobind::init() )
    .def( "getSquare",
          &CheckersBoard::GetSquare,
          nanobind::arg( "arrayLoc" ),
          nanobind::rv_policy::reference_internal )
    .def( "getState", &CheckersBoard::GetState, nanobind::arg( "squareNum" ) )
    .def( "setState",
          &CheckersBoard::SetState,
          nanobind::arg( "squareNum" ),
          nanobind::arg( "newState" ) )
    .def(
      "getAdjacent", &CheckersBoard::GetAdjacent, nanobind::arg( "squareNum" ) )
    .def( "getJumps", &CheckersBoard::GetJumps, nanobind::arg( "squareNum" ) )
    .def( "getStates", &CheckersBoard::GetStates )
    .def( "setStates", &CheckersBoard::SetStates, nanobind::arg( "squares" ) )
    .def( "delete", &CheckersBoard::Delete )
    /**
     * squareList is a real Python list whose elements are live references into
     * the board (reference_internal + self as keep-alive parent), so mutations
     * like board.squareList[ i ].setState( 0 ) reach the actual tiles.
     *
     * A real list is required because callers index it; the reference is
     * required because they mutate through it.  bind_vector would give both,
     * but conflicts with the std::vector caster the adjacency/jump lists need
     * to stay real lists.
     */
    .def_prop_ro(
      "squareList",
      []( CheckersBoard& board )
      {
        const nanobind::handle self = nanobind::find( &board );
        nanobind::list         squares;

        for ( auto& tile : board.SquareList() )
        {
          squares.append( nanobind::cast(
            &tile, nanobind::rv_policy::reference_internal, self ) );
        }

        return squares;
      } );
}
