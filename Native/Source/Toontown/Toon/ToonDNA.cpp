#include "Toontown/Toon/ToonDNA.hpp"

#include <format>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <array>
#include <random>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>

#include "Toontown/Math/PandaConvert.hpp"
#include "Toontown/Util/Datagram.hpp"
#include "Toontown/Util/PyRandom.hpp"
#include "Registry.hpp"

namespace Toontown::Toon
{
  namespace
  {
    const std::vector<std::string> TOON_HEAD_TYPES = { "dls",
                                                       "dss",
                                                       "dsl",
                                                       "dll",
                                                       "cls",
                                                       "css",
                                                       "csl",
                                                       "cll",
                                                       "hls",
                                                       "hss",
                                                       "hsl",
                                                       "hll",
                                                       "mls",
                                                       "mss",
                                                       "rls",
                                                       "rss",
                                                       "rsl",
                                                       "rll",
                                                       "fls",
                                                       "fss",
                                                       "fsl",
                                                       "fll",
                                                       "pls",
                                                       "pss",
                                                       "psl",
                                                       "pll",
                                                       "bls",
                                                       "bss",
                                                       "bsl",
                                                       "bll",
                                                       "sls",
                                                       "sss",
                                                       "ssl",
                                                       "sll" };

    const std::vector<std::string> TOON_TORSO_TYPES = {
      "ss", "ms", "ls", "sd", "md", "ld", "s", "m", "l" };

    const std::vector<std::string> TOON_LEG_TYPES = { "s", "m", "l" };

    constexpr int NUM_SHIRTS         = 151;
    constexpr int NUM_SLEEVES        = 138;
    constexpr int NUM_BOY_SHORTS     = 58;
    constexpr int NUM_GIRL_BOTTOMS   = 63;
    constexpr int NUM_CLOTHES_COLORS = 31;
    constexpr int NUM_ALL_COLORS     = 27;

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

    struct ShirtStyle
    {
      int m_TopTexture;
      int m_SleeveTexture;
      // (topColor, sleeveColor)
      std::vector<std::pair<int, int>> m_Colors;
    };

    struct BottomStyle
    {
      int              m_BottomTexture;
      std::vector<int> m_Colors;
    };

#include "Toontown/Toon/ToonDNA.data.inc"

    // try/except allColorsList[i] -> allColorsList[0]
    Math::Vec4 AllColors( int index )
    {
      return index >= 0 && index < static_cast<int>( ALL_COLORS.size() )
               ? ALL_COLORS[ index ]
               : ALL_COLORS[ 0 ];
    }

    // Process-global generator for the unseeded ("global random") path; the
    // seeded NPC path uses a fresh PyRandom(seed) for reproducibility.
    Util::PyRandom & LiveRandom()
    {
      static Util::PyRandom engine{
        static_cast<std::uint64_t>( std::random_device{}() ) << 32 |
        std::random_device{}() };

      return engine;
    }

    std::tuple<int, int, int, int> GetRandomTop( const std::string & gender,
                                                 Util::PyRandom &    random )
    {
      const auto &        collection = TAILOR_COLLECTIONS.at( K_MAKE_A_TOON );
      const auto &        list  = gender == "m" ? collection[ K_BOY_SHIRTS ]
                                                : collection[ K_GIRL_SHIRTS ];
      const std::string & style = list[ random.ChoiceIndex( list.size() ) ];
      const auto & [ m_TopTexture, m_SleeveTexture, m_Colors ] =
        SHIRT_STYLES.at( style );
      const auto & [ color1, color2 ] =
        m_Colors[ random.ChoiceIndex( m_Colors.size() ) ];

      return { m_TopTexture, color1, m_SleeveTexture, color2 };
    }

    std::pair<int, int> GetRandomBottom(
      const std::string & gender,
      Util::PyRandom &    random,
      std::optional<int>  girlBottomType = std::nullopt )
    {
      const auto & collection = TAILOR_COLLECTIONS.at( K_MAKE_A_TOON );
      std::string  style;

      if ( gender == "m" )
      {
        const auto & list = collection[ K_BOY_SHORTS ];
        style             = list[ random.ChoiceIndex( list.size() ) ];
      }
      else if ( !girlBottomType )
      {
        const auto & list = collection[ K_GIRL_BOTTOMS ];
        style             = list[ random.ChoiceIndex( list.size() ) ];
      }
      else if ( *girlBottomType == K_SKIRT || *girlBottomType == K_SHORTS )
      {
        // Preserve collection order while filtering by GirlBottoms type.
        std::vector<std::string> filtered;
        for ( const auto & s : collection[ K_GIRL_BOTTOMS ] )
        {
          if ( GIRL_BOTTOM_TYPES[ BOTTOM_STYLES.at( s ).m_BottomTexture ] ==
               *girlBottomType )
          {
            filtered.push_back( s );
          }
        }

        style = filtered[ random.ChoiceIndex( filtered.size() ) ];
      }
      else
      {
        throw std::runtime_error( "GetRandomBottom: bad girlBottomType" );
      }

      const auto & [ m_BottomTexture, m_Colors ] = BOTTOM_STYLES.at( style );
      const int color = m_Colors[ random.ChoiceIndex( m_Colors.size() ) ];

      return { m_BottomTexture, color };
    }
  }  // namespace

  // --------------------------------------------------------------------------
  // Wire
  // --------------------------------------------------------------------------

  std::string ToonDNA::MakeNetString() const
  {
    Util::Datagram datagram;
    datagram.FixedString( m_Type, 1 );

    if ( m_Type == "t" )
    {
      datagram.UInt8(
        static_cast<std::uint8_t>( IndexOf( TOON_HEAD_TYPES, m_Head ) ) );
      datagram.UInt8(
        static_cast<std::uint8_t>( IndexOf( TOON_TORSO_TYPES, m_Torso ) ) );
      datagram.UInt8(
        static_cast<std::uint8_t>( IndexOf( TOON_LEG_TYPES, m_Legs ) ) );
      datagram.UInt8( m_Gender == "m" ? 1 : 0 );
      datagram.UInt8( static_cast<std::uint8_t>( m_TopTexture ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_TopTextureColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_SleeveTexture ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_SleeveTextureColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_BottomTexture ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_BottomTextureColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_ArmColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_GloveColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_LegColor ) );
      datagram.UInt8( static_cast<std::uint8_t>( m_HeadColor ) );
    }
    else
    {
      throw std::runtime_error(
        "ToonDNA:MakeNetString: undefined/unknown avatar type" );
    }

    return datagram.Bytes();
  }

  bool ToonDNA::IsValidNetString( const std::string & bytes ) const
  {
    Util::DatagramIterator it( bytes );
    if ( it.GetRemainingSize() != 15 )
    {
      return false;
    }

    if ( it.GetFixedString( 1 ) != "t" )
    {
      return false;
    }

    if ( const int headIndex = it.GetUInt8();
         headIndex >= static_cast<int>( TOON_HEAD_TYPES.size() ) )
    {
      return false;
    }

    if ( const int torsoIndex = it.GetUInt8();
         torsoIndex >= static_cast<int>( TOON_TORSO_TYPES.size() ) )
    {
      return false;
    }

    if ( const int legsIndex = it.GetUInt8();
         legsIndex >= static_cast<int>( TOON_LEG_TYPES.size() ) )
    {
      return false;
    }

    const int         genderByte = it.GetUInt8();
    const std::string gender     = genderByte == 1 ? "m" : "f";

    if ( const int topTexture = it.GetUInt8(); topTexture >= NUM_SHIRTS )
    {
      return false;
    }

    if ( const int topTextureColor = it.GetUInt8();
         topTextureColor >= NUM_CLOTHES_COLORS )
    {
      return false;
    }

    if ( const int sleeveTexture = it.GetUInt8(); sleeveTexture >= NUM_SLEEVES )
    {
      return false;
    }

    if ( const int sleeveTextureColor = it.GetUInt8();
         sleeveTextureColor >= NUM_CLOTHES_COLORS )
    {
      return false;
    }

    if ( const int bottomTexture = it.GetUInt8();
         bottomTexture >=
         ( gender == "m" ? NUM_BOY_SHORTS : NUM_GIRL_BOTTOMS ) )
    {
      return false;
    }

    if ( const int bottomTextureColor = it.GetUInt8();
         bottomTextureColor >= NUM_CLOTHES_COLORS )
    {
      return false;
    }

    if ( const int armColor = it.GetUInt8(); armColor >= NUM_ALL_COLORS )
    {
      return false;
    }

    // Gloves are always color 0
    if ( const int gloveColor = it.GetUInt8(); gloveColor != 0 )
    {
      return false;
    }

    if ( const int legColor = it.GetUInt8(); legColor >= NUM_ALL_COLORS )
    {
      return false;
    }

    return true;
  }

  void ToonDNA::MakeFromNetString( const std::string & bytes )
  {
    Util::DatagramIterator it( bytes );

    m_Type = it.GetFixedString( 1 );
    if ( m_Type == "t" )
    {
      const int headIndex  = it.GetUInt8();
      const int torsoIndex = it.GetUInt8();
      const int legsIndex  = it.GetUInt8();
      m_Head               = TOON_HEAD_TYPES.at( headIndex );
      m_Torso              = TOON_TORSO_TYPES.at( torsoIndex );
      m_Legs               = TOON_LEG_TYPES.at( legsIndex );
      const int gender     = it.GetUInt8();
      m_Gender             = gender == 1 ? "m" : "f";
      m_TopTexture         = it.GetUInt8();
      m_TopTextureColor    = it.GetUInt8();
      m_SleeveTexture      = it.GetUInt8();
      m_SleeveTextureColor = it.GetUInt8();
      m_BottomTexture      = it.GetUInt8();
      m_BottomTextureColor = it.GetUInt8();
      m_ArmColor           = it.GetUInt8();
      m_GloveColor         = it.GetUInt8();
      m_LegColor           = it.GetUInt8();
      m_HeadColor          = it.GetUInt8();
    }
    else
    {
      throw std::runtime_error( std::format(
        "ToonDNA::MakeFromNetString: unknown avatar type: {}", m_Type ) );
    }
  }

  // --------------------------------------------------------------------------
  // Generation
  // --------------------------------------------------------------------------

  void ToonDNA::NewToon(
    const std::tuple<std::string, std::string, std::string, std::string> & dna,
    std::optional<int> color )
  {
    m_Type               = "t";
    m_Head               = std::get<0>( dna );
    m_Torso              = std::get<1>( dna );
    m_Legs               = std::get<2>( dna );
    m_Gender             = std::get<3>( dna );
    m_TopTexture         = 0;
    m_TopTextureColor    = 0;
    m_SleeveTexture      = 0;
    m_SleeveTextureColor = 0;
    m_BottomTexture      = 0;
    m_BottomTextureColor = 0;

    const int c  = color.value_or( GetDefaultColor() );
    m_ArmColor   = c;
    m_LegColor   = c;
    m_HeadColor  = c;
    m_GloveColor = 0;
  }

  void ToonDNA::NewToonFromProperties( const std::string & head,
                                       const std::string & torso,
                                       const std::string & legs,
                                       const std::string & gender,
                                       int                 armColor,
                                       int                 gloveColor,
                                       int                 legColor,
                                       int                 headColor,
                                       int                 topTexture,
                                       int                 topTextureColor,
                                       int                 sleeveTexture,
                                       int                 sleeveTextureColor,
                                       int                 bottomTexture,
                                       int                 bottomTextureColor )
  {
    m_Type               = "t";
    m_Head               = head;
    m_Torso              = torso;
    m_Legs               = legs;
    m_Gender             = gender;
    m_ArmColor           = armColor;
    m_GloveColor         = gloveColor;
    m_LegColor           = legColor;
    m_HeadColor          = headColor;
    m_TopTexture         = topTexture;
    m_TopTextureColor    = topTextureColor;
    m_SleeveTexture      = sleeveTexture;
    m_SleeveTextureColor = sleeveTextureColor;
    m_BottomTexture      = bottomTexture;
    m_BottomTextureColor = bottomTextureColor;
  }

  void ToonDNA::NewToonRandom( std::optional<int>  seed,
                               const std::string & gender,
                               int                 npc,
                               std::optional<int>  stage,
                               bool                isPaid )
  {
    // In Python, `if seed:` uses a fresh seeded Random (reproducible);
    // otherwise, the global unseeded RNG.  seed == 0 is falsy, matching the
    // global path.
    const bool isUsingSeed = seed.has_value() && *seed != 0;

    auto Generate = [ & ]( Util::PyRandom & random )
    {
      m_Type = "t";

      // choice(toonLegTypes + ['m', 'l', 'l', 'l'])
      static const std::vector<std::string> legChoices = {
        "s", "m", "l", "m", "l", "l", "l" };

      m_Legs   = legChoices[ random.ChoiceIndex( legChoices.size() ) ];
      m_Gender = gender;

      if ( !npc )
      {
        if ( stage && *stage == K_MAKE_A_TOON )
        {
          const auto & indices =
            isPaid ? ALL_HEAD_ANIMAL_INDICES : ALL_HEAD_ANIMAL_INDICES_TRIAL;
          const int animal = indices[ random.ChoiceIndex( indices.size() ) ];
          m_Head           = TOON_HEAD_TYPES.at( animal );
        }
        else
        {
          m_Head =
            TOON_HEAD_TYPES[ random.ChoiceIndex( TOON_HEAD_TYPES.size() ) ];
        }
      }
      else
      {
        // toonHeadTypes[:22]
        m_Head = TOON_HEAD_TYPES[ random.ChoiceIndex( 22 ) ];
      }

      auto [ top, topColor, sleeve, sleeveColor ] =
        GetRandomTop( gender, random );
      auto [ bottom, bottomColor ] = GetRandomBottom( gender, random );

      m_TopTexture         = top;
      m_TopTextureColor    = topColor;
      m_SleeveTexture      = sleeve;
      m_SleeveTextureColor = sleeveColor;

      if ( gender == "m" )
      {
        // [:3]
        m_Torso              = TOON_TORSO_TYPES[ random.ChoiceIndex( 3 ) ];
        m_BottomTexture      = bottom;
        m_BottomTextureColor = bottomColor;

        const int color =
          DEFAULT_BOY_COLORS[ random.ChoiceIndex( DEFAULT_BOY_COLORS.size() ) ];
        m_ArmColor  = color;
        m_LegColor  = color;
        m_HeadColor = color;
      }
      else
      {
        // [:6]
        m_Torso = TOON_TORSO_TYPES[ random.ChoiceIndex( 6 ) ];
        if ( m_Torso[ 1 ] == 'd' )
        {
          auto [ b, bc ]       = GetRandomBottom( gender, random, K_SKIRT );
          m_BottomTexture      = b;
          m_BottomTextureColor = bc;
        }
        else
        {
          auto [ b, bc ]       = GetRandomBottom( gender, random, K_SHORTS );
          m_BottomTexture      = b;
          m_BottomTextureColor = bc;
        }

        const int color = DEFAULT_GIRL_COLORS[ random.ChoiceIndex(
          DEFAULT_GIRL_COLORS.size() ) ];
        m_ArmColor      = color;
        m_LegColor      = color;
        m_HeadColor     = color;
      }

      m_GloveColor = 0;
    };

    if ( isUsingSeed )
    {
      Util::PyRandom random( static_cast<std::uint64_t>( *seed ) );
      Generate( random );
    }
    else
    {
      Generate( LiveRandom() );
    }
  }

  void ToonDNA::UpdateToonProperties(
    std::optional<std::string>                  head,
    std::optional<std::string>                  torso,
    std::optional<std::string>                  legs,
    std::optional<std::string>                  gender,
    std::optional<int>                          armColor,
    std::optional<int>                          gloveColor,
    std::optional<int>                          legColor,
    std::optional<int>                          headColor,
    std::optional<int>                          topTexture,
    std::optional<int>                          topTextureColor,
    std::optional<int>                          sleeveTexture,
    std::optional<int>                          sleeveTextureColor,
    std::optional<int>                          bottomTexture,
    std::optional<int>                          bottomTextureColor,
    std::optional<std::tuple<std::string, int>> shirt,
    std::optional<std::tuple<std::string, int>> bottom )
  {
    // In the original, Python guards each field with `if value:` truthiness,
    // so an empty string or a 0 index is silently skipped.

    if ( head && !head->empty() )
    {
      m_Head = *head;
    }

    if ( torso && !torso->empty() )
    {
      m_Torso = *torso;
    }

    if ( legs && !legs->empty() )
    {
      m_Legs = *legs;
    }

    if ( gender && !gender->empty() )
    {
      m_Gender = *gender;
    }

    if ( armColor && *armColor )
    {
      m_ArmColor = *armColor;
    }

    if ( gloveColor && *gloveColor )
    {
      m_GloveColor = *gloveColor;
    }

    if ( legColor && *legColor )
    {
      m_LegColor = *legColor;
    }

    if ( headColor && *headColor )
    {
      m_HeadColor = *headColor;
    }

    if ( topTexture && *topTexture )
    {
      m_TopTexture = *topTexture;
    }

    if ( topTextureColor && *topTextureColor )
    {
      m_TopTextureColor = *topTextureColor;
    }

    if ( sleeveTexture && *sleeveTexture )
    {
      m_SleeveTexture = *sleeveTexture;
    }

    if ( bottomTexture && *bottomTexture )
    {
      m_BottomTexture = *bottomTexture;
    }

    if ( bottomTextureColor && *bottomTextureColor )
    {
      m_BottomTextureColor = *bottomTextureColor;
    }

    if ( shirt )
    {
      const auto & [ name, colorIndex ] = *shirt;
      const ShirtStyle & style          = SHIRT_STYLES.at( name );
      m_TopTexture                      = style.m_TopTexture;
      m_TopTextureColor                 = style.m_Colors[ colorIndex ].first;
      m_SleeveTexture                   = style.m_SleeveTexture;
      m_SleeveTextureColor              = style.m_Colors[ colorIndex ].second;
    }

    if ( bottom )
    {
      const auto & [ name, colorIndex ] = *bottom;
      const BottomStyle & style         = BOTTOM_STYLES.at( name );
      m_BottomTexture                   = style.m_BottomTexture;
      m_BottomTextureColor              = style.m_Colors[ colorIndex ];
    }
  }

  int ToonDNA::GetDefaultColor() const
  {
    return 25;
  }

  void ToonDNA::GetDefaultColors()
  {
    const int color = GetDefaultColor();
    m_ArmColor      = color;
    m_GloveColor    = 0;
    m_LegColor      = color;
    m_HeadColor     = color;
  }

  // --------------------------------------------------------------------------
  // Derived classifiers
  // --------------------------------------------------------------------------

  std::string ToonDNA::GetType() const
  {
    if ( m_Type == "t" )
    {
      return GetAnimal();
    }

    throw std::runtime_error(
      std::format( "ToonDNA::GetType: invalid DNA type: {}", m_Type ) );
  }

  std::string ToonDNA::GetAnimal() const
  {
    switch ( m_Head.empty() ? '\0' : m_Head[ 0 ] )
    {
      case 'd':
        return "dog";

      case 'c':
        return "cat";

      case 'm':
        return "mouse";

      case 'h':
        return "horse";

      case 'r':
        return "rabbit";

      case 'f':
        return "duck";

      case 'p':
        return "monkey";

      case 'b':
        return "bear";

      case 's':
        return "pig";

      default:
        throw std::runtime_error( "ToonDNA::GetAnimal: unknown head" );
    }
  }

  std::string ToonDNA::GetHeadSize() const
  {
    const char c = m_Head.size() > 1 ? m_Head[ 1 ] : '\0';
    if ( c == 'l' )
    {
      return "long";
    }

    if ( c == 's' )
    {
      return "short";
    }

    throw std::runtime_error( "ToonDNA::GetHeadSize: unknown head size" );
  }

  std::string ToonDNA::GetMuzzleSize() const
  {
    const char c = m_Head.size() > 2 ? m_Head[ 2 ] : '\0';
    if ( c == 'l' )
    {
      return "long";
    }

    if ( c == 's' )
    {
      return "short";
    }

    throw std::runtime_error( "ToonDNA::GetMuzzleSize: unknown muzzle size" );
  }

  std::string ToonDNA::GetTorsoSize() const
  {
    const char c = m_Torso.empty() ? '\0' : m_Torso[ 0 ];
    if ( c == 'l' )
    {
      return "long";
    }

    if ( c == 'm' )
    {
      return "medium";
    }

    if ( c == 's' )
    {
      return "short";
    }

    throw std::runtime_error( "ToonDNA::GetTorsoSize: unknown torso size" );
  }

  std::string ToonDNA::GetLegSize() const
  {
    if ( m_Legs == "l" )
    {
      return "long";
    }

    if ( m_Legs == "m" )
    {
      return "medium";
    }

    if ( m_Legs == "s" )
    {
      return "short";
    }

    throw std::runtime_error( "ToonDNA::GetLegSize: unknown leg size" );
  }

  std::string ToonDNA::GetGender() const
  {
    return m_Gender;
  }

  std::string ToonDNA::GetClothes() const
  {
    if ( m_Torso.size() == 1 )
    {
      return "naked";
    }

    if ( m_Torso[ 1 ] == 's' )
    {
      return "shorts";
    }

    if ( m_Torso[ 1 ] == 'd' )
    {
      return "dress";
    }

    throw std::runtime_error( "ToonDNA::GetClothes: unknown clothing type" );
  }

  Math::Vec4 ToonDNA::GetArmColor() const
  {
    return AllColors( m_ArmColor );
  }

  Math::Vec4 ToonDNA::GetLegColor() const
  {
    return AllColors( m_LegColor );
  }

  Math::Vec4 ToonDNA::GetHeadColor() const
  {
    return AllColors( m_HeadColor );
  }

  Math::Vec4 ToonDNA::GetGloveColor() const
  {
    return AllColors( m_GloveColor );
  }

  Math::Vec4 ToonDNA::GetBlackColor() const
  {
    return AllColors( 26 );
  }

  ToonDNA ToonDNA::Clone() const
  {
    ToonDNA dna;
    dna.MakeFromNetString( MakeNetString() );
    return dna;
  }

  std::string ToonDNA::GetString() const
  {
    return std::format( "type = toon\n"
                        "gender = {}\n"
                        "head = {}, torso = {}, legs = {}\n"
                        "arm color = {}\n"
                        "glove color = {}\n"
                        "leg color = {}\n"
                        "head color = {}\n"
                        "top texture = {}\n"
                        "top texture color = {}\n"
                        "sleeve texture = {}\n"
                        "sleeve texture color = {}\n"
                        "bottom texture = {}\n"
                        "bottom texture color = {}\n",
                        m_Gender,
                        m_Head,
                        m_Torso,
                        m_Legs,
                        m_ArmColor,
                        m_GloveColor,
                        m_LegColor,
                        m_HeadColor,
                        m_TopTexture,
                        m_TopTextureColor,
                        m_SleeveTexture,
                        m_SleeveTextureColor,
                        m_BottomTexture,
                        m_BottomTextureColor );
  }

  std::tuple<std::string,
             std::string,
             std::string,
             std::string,
             int,
             int,
             int,
             int,
             int,
             int,
             int,
             int,
             int,
             int>
  ToonDNA::AsTuple() const
  {
    return { m_Head,
             m_Torso,
             m_Legs,
             m_Gender,
             m_ArmColor,
             m_GloveColor,
             m_LegColor,
             m_HeadColor,
             m_TopTexture,
             m_TopTextureColor,
             m_SleeveTexture,
             m_SleeveTextureColor,
             m_BottomTexture,
             m_BottomTextureColor };
  }

  void ToonDNA::SetTemporary( const std::string & newHead,
                              int                 newArmColor,
                              int                 newLegColor,
                              int                 newHeadColor )
  {
    // The original's Python guard, `not self.cache and self.getArmColor !=
    // newArmColor` compares a bound method (never called) to an int -> always
    // True, so the guard is effectively `not cache`.  Reproduced as such, the
    // missing () is preserved.
    if ( !m_Cache )
    {
      m_Cache = std::make_tuple( m_Head, m_ArmColor, m_LegColor, m_HeadColor );
      UpdateToonProperties( newHead,
                            std::nullopt,
                            std::nullopt,
                            std::nullopt,
                            newArmColor,
                            std::nullopt,
                            newLegColor,
                            newHeadColor );
    }
  }

  void ToonDNA::RestoreTemporary( ToonDNA * oldStyle )
  {
    // () in Python
    std::optional<std::tuple<std::string, int, int, int>> cache;

    if ( oldStyle )
    {
      cache = oldStyle->m_Cache;
    }

    if ( cache )
    {
      const auto & [ head, armColor, legColor, headColor ] = *cache;
      UpdateToonProperties( head,
                            std::nullopt,
                            std::nullopt,
                            std::nullopt,
                            armColor,
                            std::nullopt,
                            legColor,
                            headColor );

      if ( oldStyle )
      {
        oldStyle->m_Cache.reset();
      }
    }
  }
}  // namespace Toontown::Toon

void RegisterToonDNA( nanobind::module_ & toon )
{
  using namespace Toontown::Toon;

  toon.attr( "toonHeadTypes" )  = nanobind::cast( TOON_HEAD_TYPES );
  toon.attr( "toonTorsoTypes" ) = nanobind::cast( TOON_TORSO_TYPES );
  toon.attr( "toonLegTypes" )   = nanobind::cast( TOON_LEG_TYPES );

  nanobind::class_<ToonDNA>( toon, "ToonDNA" )
    .def(
      "__init__",
      []( ToonDNA *        self,
          nanobind::object str,
          nanobind::object type,
          nanobind::object dna,
          nanobind::object r,
          nanobind::object b,
          nanobind::object g )
      {
        new ( self ) ToonDNA();

        if ( !str.is_none() )
        {
          const auto bytes = nanobind::cast<nanobind::bytes>( str );
          self->MakeFromNetString( std::string( bytes.c_str(), bytes.size() ) );
        }
        else if ( !type.is_none() )
        {
          if ( nanobind::cast<std::string>( type ) == "t" )
          {
            if ( dna.is_none() )
            {
              self->NewToonRandom(
                r.is_none() ? std::nullopt
                            : std::optional( nanobind::cast<int>( r ) ),
                g.is_none() ? std::string( "m" )
                            : nanobind::cast<std::string>( g ),
                b.is_none() ? 0 : nanobind::cast<int>( b ) );
            }
            else
            {
              auto t = nanobind::cast<ToonDNA>( dna ).AsTuple();
              std::apply(
                [ & ]( auto &&... a )
                {
                  self->NewToonFromProperties( a... );
                },
                t );
            }
          }
        }
        else
        {
          self->m_Type = "u";
        }
      },
      nanobind::arg( "str" )  = nanobind::none(),
      nanobind::arg( "type" ) = nanobind::none(),
      nanobind::arg( "dna" )  = nanobind::none(),
      nanobind::arg( "r" )    = nanobind::none(),
      nanobind::arg( "b" )    = nanobind::none(),
      nanobind::arg( "g" )    = nanobind::none() )

    .def_rw( "type", &ToonDNA::m_Type )
    .def_rw( "gender", &ToonDNA::m_Gender )
    .def_rw( "head", &ToonDNA::m_Head )
    .def_rw( "torso", &ToonDNA::m_Torso )
    .def_rw( "legs", &ToonDNA::m_Legs )
    .def_rw( "armColor", &ToonDNA::m_ArmColor )
    .def_rw( "gloveColor", &ToonDNA::m_GloveColor )
    .def_rw( "legColor", &ToonDNA::m_LegColor )
    .def_rw( "headColor", &ToonDNA::m_HeadColor )
    .def_rw( "topTex", &ToonDNA::m_TopTexture )
    .def_rw( "topTexColor", &ToonDNA::m_TopTextureColor )
    .def_rw( "sleeveTex", &ToonDNA::m_SleeveTexture )
    .def_rw( "sleeveTexColor", &ToonDNA::m_SleeveTextureColor )
    .def_rw( "botTex", &ToonDNA::m_BottomTexture )
    .def_rw( "botTexColor", &ToonDNA::m_BottomTextureColor )

    // cache: () when unset, a 4-tuple when set (Python empty tuple)
    .def_prop_rw(
      "cache",
      []( const ToonDNA & dna ) -> nanobind::object
      {
        if ( !dna.m_Cache )
        {
          return nanobind::tuple();
        }

        const auto & c = *dna.m_Cache;
        return nanobind::make_tuple( std::get<0>( c ),
                                     std::get<1>( c ),
                                     std::get<2>( c ),
                                     std::get<3>( c ) );
      },
      []( ToonDNA & dna, nanobind::object value )
      {
        if ( nanobind::len( value ) == 0 )
        {
          dna.m_Cache.reset();
        }
        else
        {
          auto t =
            nanobind::cast<std::tuple<std::string, int, int, int>>( value );
          dna.m_Cache = t;
        }
      } )

    .def( "makeNetString",
          []( const ToonDNA & dna )
          {
            const std::string net = dna.MakeNetString();
            return nanobind::bytes( net.data(), net.size() );
          } )

    .def(
      "isValidNetString",
      []( const ToonDNA & dna, nanobind::bytes bytes )
      {
        return dna.IsValidNetString(
          std::string( bytes.c_str(), bytes.size() ) );
      },
      nanobind::arg( "string" ) )

    .def(
      "makeFromNetString",
      []( ToonDNA & dna, nanobind::bytes bytes )
      {
        dna.MakeFromNetString( std::string( bytes.c_str(), bytes.size() ) );
      },
      nanobind::arg( "string" ) )

    .def( "defaultColor", &ToonDNA::GetDefaultColor )

    .def( "newToon",
          &ToonDNA::NewToon,
          nanobind::arg( "dna" ),
          nanobind::arg( "color" ) = nanobind::none() )

    .def( "newToonFromProperties",
          &ToonDNA::NewToonFromProperties,
          nanobind::arg( "head" ),
          nanobind::arg( "torso" ),
          nanobind::arg( "legs" ),
          nanobind::arg( "gender" ),
          nanobind::arg( "armColor" ),
          nanobind::arg( "gloveColor" ),
          nanobind::arg( "legColor" ),
          nanobind::arg( "headColor" ),
          nanobind::arg( "topTexture" ),
          nanobind::arg( "topTextureColor" ),
          nanobind::arg( "sleeveTexture" ),
          nanobind::arg( "sleeveTextureColor" ),
          nanobind::arg( "bottomTexture" ),
          nanobind::arg( "bottomTextureColor" ) )

    .def( "updateToonProperties",
          &ToonDNA::UpdateToonProperties,
          nanobind::arg( "head" )               = nanobind::none(),
          nanobind::arg( "torso" )              = nanobind::none(),
          nanobind::arg( "legs" )               = nanobind::none(),
          nanobind::arg( "gender" )             = nanobind::none(),
          nanobind::arg( "armColor" )           = nanobind::none(),
          nanobind::arg( "gloveColor" )         = nanobind::none(),
          nanobind::arg( "legColor" )           = nanobind::none(),
          nanobind::arg( "headColor" )          = nanobind::none(),
          nanobind::arg( "topTexture" )         = nanobind::none(),
          nanobind::arg( "topTextureColor" )    = nanobind::none(),
          nanobind::arg( "sleeveTexture" )      = nanobind::none(),
          nanobind::arg( "sleeveTextureColor" ) = nanobind::none(),
          nanobind::arg( "bottomTexture" )      = nanobind::none(),
          nanobind::arg( "bottomTextureColor" ) = nanobind::none() )

    .def(
      "newToonRandom",
      []( ToonDNA &           dna,
          std::optional<int>  seed,
          const std::string & gender,
          int                 npc,
          std::optional<int>  stage )
      {
        // isPaid handled internally
        dna.NewToonRandom( seed, gender, npc, stage );
      },
      nanobind::arg( "seed" )   = nanobind::none(),
      nanobind::arg( "gender" ) = "m",
      nanobind::arg( "npc" )    = 0,
      nanobind::arg( "stage" )  = nanobind::none() )

    .def( "asTuple", &ToonDNA::AsTuple )

    .def( "getType", &ToonDNA::GetType )

    .def( "getAnimal", &ToonDNA::GetAnimal )

    .def( "getHeadSize", &ToonDNA::GetHeadSize )

    .def( "getMuzzleSize", &ToonDNA::GetMuzzleSize )

    .def( "getTorsoSize", &ToonDNA::GetTorsoSize )

    .def( "getLegSize", &ToonDNA::GetLegSize )

    .def( "getGender", &ToonDNA::GetGender )

    .def( "getClothes", &ToonDNA::GetClothes )

    .def( "getArmColor", &ToonDNA::GetArmColor )

    .def( "getLegColor", &ToonDNA::GetLegColor )

    .def( "getHeadColor", &ToonDNA::GetHeadColor )

    .def( "getGloveColor", &ToonDNA::GetGloveColor )

    .def( "getBlackColor", &ToonDNA::GetBlackColor )

    .def( "clone", &ToonDNA::Clone )

    .def( "__str__", &ToonDNA::GetString )

    .def( "setTemporary",
          &ToonDNA::SetTemporary,
          nanobind::arg( "newHead" ),
          nanobind::arg( "newArmColor" ),
          nanobind::arg( "newLegColor" ),
          nanobind::arg( "newHeadColor" ) )

    .def( "restoreTemporary",
          &ToonDNA::RestoreTemporary,
          nanobind::arg( "oldStyle" ) );
}
