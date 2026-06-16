#include "Toontown/Suit/SuitDNA.hpp"

#include <random>
#include <stdexcept>
#include <vector>
#include <format>

#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "Toontown/Util/PyRandom.hpp"
#include "Toontown/Util/Datagram.hpp"
#include "Registry.hpp"

namespace Toontown::Suit
{
  namespace
  {
    const std::vector<std::string> SUIT_HEAD_TYPES = { "f",
                                                       "p",
                                                       "ym",
                                                       "mm",
                                                       "ds",
                                                       "hh",
                                                       "cr",
                                                       "tbc",
                                                       "bf",
                                                       "b",
                                                       "dt",
                                                       "ac",
                                                       "bs",
                                                       "sd",
                                                       "le",
                                                       "bw",
                                                       "sc",
                                                       "pp",
                                                       "tw",
                                                       "bc",
                                                       "nc",
                                                       "mb",
                                                       "ls",
                                                       "rb",
                                                       "cc",
                                                       "tm",
                                                       "nd",
                                                       "gh",
                                                       "ms",
                                                       "tf",
                                                       "m",
                                                       "mh" };

    const std::vector<std::string> SUIT_A_TYPES = { "ym",
                                                    "hh",
                                                    "tbc",
                                                    "dt",
                                                    "bs",
                                                    "le",
                                                    "bw",
                                                    "pp",
                                                    "nc",
                                                    "rb",
                                                    "nd",
                                                    "tf",
                                                    "m",
                                                    "mh" };

    const std::vector<std::string> SUIT_B_TYPES = {
      "p", "ds", "b", "ac", "sd", "bc", "ls", "tm", "ms" };

    const std::vector<std::string> SUIT_C_TYPES = {
      "f", "mm", "cr", "bf", "sc", "tw", "mb", "cc", "gh" };

    const std::vector<std::string> SUIT_DEPTS = { "c", "l", "m", "s" };

    const std::vector SUITS_PER_LEVEL = { 1, 1, 1, 1, 1, 1, 1, 1 };

    const std::vector<std::string> GOON_TYPES = { "pg", "sg" };

    constexpr int SUITS_PER_DEPT = 8;

    Util::PyRandom & Random()
    {
      static Util::PyRandom engine{
        static_cast<std::uint64_t>( std::random_device{}() ) << 32 |
        std::random_device{}() };

      return engine;
    }

    // Inclusive, matching Python randint: a + _randBelow(b - a + 1).
    int RandomInteger( int min, int max )
    {
      return static_cast<int>( Random().Int( min, max ) );
    }

    int IndexOf( const std::vector<std::string> & v, const std::string & s )
    {
      for ( std::size_t i = 0; i < v.size(); ++i )
      {
        if ( v[ i ] == s )
        {
          return static_cast<int>( i );
        }
      }

      return -1;
    }

    bool Contains( const std::vector<std::string> & v, const std::string & s )
    {
      return IndexOf( v, s ) >= 0;
    }
  }  // namespace

  std::string GetSuitBodyType( const std::string & name )
  {
    if ( Contains( SUIT_A_TYPES, name ) )
    {
      return "a";
    }

    if ( Contains( SUIT_B_TYPES, name ) )
    {
      return "b";
    }

    if ( Contains( SUIT_C_TYPES, name ) )
    {
      return "c";
    }

    // Original prints + returns None; unreachable for valid names.
    return "";
  }

  std::string GetSuitDept( const std::string & name )
  {
    const int index = IndexOf( SUIT_HEAD_TYPES, name );

    if ( index < 0 )
    {
      return "";
    }

    if ( index < SUITS_PER_DEPT )
    {
      return SUIT_DEPTS[ 0 ];
    }

    if ( index < SUITS_PER_DEPT * 2 )
    {
      return SUIT_DEPTS[ 1 ];
    }

    if ( index < SUITS_PER_DEPT * 3 )
    {
      return SUIT_DEPTS[ 2 ];
    }

    if ( index < SUITS_PER_DEPT * 4 )
    {
      return SUIT_DEPTS[ 3 ];
    }

    return "";
  }

  int GetSuitType( const std::string & name )
  {
    return IndexOf( SUIT_HEAD_TYPES, name ) % SUITS_PER_DEPT + 1;
  }

  int GetRandomSuitType( int level )
  {
    return RandomInteger( std::max( level - 4, 1 ), std::min( level, 8 ) );
  }

  std::string GetRandomSuitByDept( const std::string & dept )
  {
    const int deptNumber = IndexOf( SUIT_DEPTS, dept );
    return SUIT_HEAD_TYPES[ SUITS_PER_DEPT * deptNumber +
                            RandomInteger( 0, 7 ) ];
  }

  std::string SuitDNA::MakeNetString() const
  {
    Util::Datagram datagram;
    datagram.FixedString( m_Type, 1 );

    if ( m_Type == "s" )
    {
      datagram.FixedString( m_Name, 3 );
      datagram.FixedString( m_Dept, 1 );
    }
    else if ( m_Type == "b" )
    {
      datagram.FixedString( m_Dept, 1 );
    }
    else
    {
      throw std::runtime_error(
        "SuitDNA::MakeNetString: undefined/unknown type" );
    }

    return datagram.Bytes();
  }

  void SuitDNA::MakeFromNetString( const std::string & bytes )
  {
    Util::DatagramIterator it( bytes );

    m_Type = it.GetFixedString( 1 );
    if ( m_Type == "s" )
    {
      m_Name = it.GetFixedString( 3 );
      m_Dept = it.GetFixedString( 1 );
      m_Body = GetSuitBodyType( m_Name );
    }
    else if ( m_Type == "b" )
    {
      m_Dept = it.GetFixedString( 1 );
    }
    else
    {
      throw std::runtime_error( "SutDNA::MakeFromNetString: unknown type" );
    }
  }

  void SuitDNA::NewSuit( std::optional<std::string> name )
  {
    m_Type = "s";
    m_Name = name.value_or( "ds" );  // __defaultSuit uses 'ds'
    m_Dept = GetSuitDept( m_Name );
    m_Body = GetSuitBodyType( m_Name );
  }

  void SuitDNA::NewBossCog( const std::string & dept )
  {
    m_Type = "b";
    m_Dept = dept;
  }

  void SuitDNA::NewSuitRandom( std::optional<int>         level,
                               std::optional<std::string> dept )
  {
    m_Type = "s";

    int lvl = 0;
    if ( !level )
    {
      // random.choice(range(1, len(suitsPerLevel))) -> 1 ... size - 1
      lvl = RandomInteger( 1, static_cast<int>( SUITS_PER_LEVEL.size() ) - 1 );
    }
    else
    {
      if ( *level < 0 || *level > static_cast<int>( SUITS_PER_LEVEL.size() ) )
      {
        throw std::runtime_error( "SuitDNA::NewSuitRandom: invalid level" );
      }

      lvl = *level;
    }

    const std::string chosenDept =
      dept ? *dept
           : SUIT_DEPTS[ RandomInteger(
               0, static_cast<int>( SUIT_DEPTS.size() ) - 1 ) ];
    m_Dept = chosenDept;

    const int index  = IndexOf( SUIT_DEPTS, chosenDept );
    const int base   = index * SUITS_PER_DEPT;
    int       offset = 0;

    for ( int i = 1; i < lvl; ++i )
    {
      offset += SUITS_PER_LEVEL[ i - 1 ];
    }

    const int bottom = base + offset;
    const int top    = bottom + SUITS_PER_LEVEL[ lvl - 1 ];
    // random.choice(range(bottom, top))
    m_Name = SUIT_HEAD_TYPES[ RandomInteger( bottom, top - 1 ) ];
    m_Body = GetSuitBodyType( m_Name );
  }

  void SuitDNA::NewGoon( std::optional<std::string> name )
  {
    // The original's `if type == None` tests the builtin `type`, never None,
    // so the default-Goon branch is dead -- a supplied, valid Goon name is
    // required.
    m_Type = "g";

    if ( name && Contains( GOON_TYPES, *name ) )
    {
      m_Name = *name;
    }
    else
    {
      throw std::runtime_error( "SuitDNA::NewGoon: unknown Goon type" );
    }
  }

  std::string SuitDNA::GetType() const
  {
    if ( m_Type == "s" )
    {
      return "suit";
    }

    if ( m_Type == "b" )
    {
      return "boss";
    }

    throw std::runtime_error( "SuitDNA::GetType: invalid DNA type" );
  }

  std::string SuitDNA::GetString() const
  {
    if ( m_Type == "s" )
    {
      return std::format( "type = suit\n"
                          "body = {}, dept = {}, name = {}",
                          m_Body,
                          m_Dept,
                          m_Name );
    }

    if ( m_Type == "b" )
    {
      return std::format( "type = boss cog\n"
                          "dept = {}",
                          m_Dept );
    }

    return "type undefined";
  }
}  // namespace Toontown::Suit

void RegisterSuitDNA( nanobind::module_ & suit )
{
  using namespace Toontown::Suit;

  // Canonical tables exposed once; the shim re-exports them so callers keep a
  // single source.
  suit.attr( "suitHeadTypes" ) = nanobind::cast( SUIT_HEAD_TYPES );
  suit.attr( "suitATypes" )    = nanobind::cast( SUIT_A_TYPES );
  suit.attr( "suitBTypes" )    = nanobind::cast( SUIT_B_TYPES );
  suit.attr( "suitCTypes" )    = nanobind::cast( SUIT_C_TYPES );
  suit.attr( "suitDepts" )     = nanobind::cast( SUIT_DEPTS );
  suit.attr( "suitsPerLevel" ) = nanobind::cast( SUITS_PER_LEVEL );
  suit.attr( "goonTypes" )     = nanobind::cast( GOON_TYPES );
  suit.attr( "suitsPerDept" )  = SUITS_PER_DEPT;

  suit.def( "getSuitBodyType", &GetSuitBodyType, nanobind::arg( "name" ) );

  suit.def( "getSuitDept", &GetSuitDept, nanobind::arg( "name" ) );

  suit.def( "getSuitType", &GetSuitType, nanobind::arg( "name" ) );

  suit.def( "getRandomSuitType", &GetRandomSuitType, nanobind::arg( "level" ) );

  suit.def(
    "getRandomSuitByDept", &GetRandomSuitByDept, nanobind::arg( "dept" ) );

  nanobind::class_<SuitDNA>( suit, "SuitDNA" )
    .def(
      "__init__",
      []( SuitDNA * self, nanobind::object str, nanobind::object type )
      {
        new ( self ) SuitDNA();

        if ( !str.is_none() )
        {
          const auto bytes = nanobind::cast<nanobind::bytes>( str );
          self->MakeFromNetString( std::string( bytes.c_str(), bytes.size() ) );
        }
        else if ( !type.is_none() )
        {
          if ( nanobind::cast<std::string>( type ) == "s" )
          {
            self->NewSuit();
          }
        }
        else
        {
          self->m_Type = "u";
        }
      },
      nanobind::arg( "str" )  = nanobind::none(),
      nanobind::arg( "type" ) = nanobind::none() )

    .def_rw( "type", &SuitDNA::m_Type )
    .def_rw( "name", &SuitDNA::m_Name )
    .def_rw( "dept", &SuitDNA::m_Dept )
    .def_rw( "body", &SuitDNA::m_Body )

    .def( "makeNetString",
          []( const SuitDNA & dna )
          {
            const std::string net = dna.MakeNetString();
            return nanobind::bytes( net.data(), net.size() );
          } )

    .def(
      "makeFromNetString",
      []( SuitDNA & dna, nanobind::bytes bytes )
      {
        dna.MakeFromNetString( std::string( bytes.c_str(), bytes.size() ) );
      },
      nanobind::arg( "string" ) )

    .def(
      "newSuit", &SuitDNA::NewSuit, nanobind::arg( "name" ) = nanobind::none() )

    .def( "newBossCog", &SuitDNA::NewBossCog, nanobind::arg( "dept" ) )

    .def( "newSuitRandom",
          &SuitDNA::NewSuitRandom,
          nanobind::arg( "level" ) = nanobind::none(),
          nanobind::arg( "dept" )  = nanobind::none() )

    .def(
      "newGoon", &SuitDNA::NewGoon, nanobind::arg( "name" ) = nanobind::none() )

    .def( "getType", &SuitDNA::GetType )

    .def( "__str__", &SuitDNA::GetString );
}
