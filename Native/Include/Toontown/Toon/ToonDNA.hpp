// This stores appearance as plain indices and strings only.  The VBase4
// color tables (allColorsList, ClothesColors, etc.) and the colors getters
// that index them (getArmColor ... getBlackColor) stay in the Python shim,
// behind the anti-corruption boundary; no VBase4 enters this type.
//
// makeNetString/makeFromNetString reproduce the Panda3D datagram wire layout
// directly over a byte buffer, so the native build links no Panda3D.
#pragma once

#include <optional>
#include <string>
#include <tuple>

#include "Toontown/Math/Vec4.hpp"

namespace Toontown::Toon
{
  class ToonDNA
  {
   public:
    ToonDNA() = default;

    // ------------------------------------------------------------------------
    // Wire format
    // ------------------------------------------------------------------------

    [[nodiscard]] std::string MakeNetString() const;
    [[nodiscard]] bool IsValidNetString( const std::string & bytes ) const;
    void               MakeFromNetString( const std::string & bytes );

    // ------------------------------------------------------------------------
    // Generation
    // ------------------------------------------------------------------------

    // dna == (head, torso, legs, gender); color is an index, default via
    // DefaultColor();
    void NewToon(
      const std::tuple<std::string, std::string, std::string, std::string> &
                         dna,
      std::optional<int> color = std::nullopt );

    void NewToonFromProperties( const std::string & head,
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
                                int                 bottomTextureColor );

    // seed == npcId for NPCs (fixed -> reproducible).  isPaid replaces the
    // client base.cr.isPaid() call, supplied at the Python boundary (kept out
    // of core).
    void NewToonRandom( std::optional<int>  seed   = std::nullopt,
                        const std::string & gender = "m",
                        int                 npc    = 0,
                        std::optional<int>  stage  = std::nullopt,
                        bool                isPaid = true );

    // In the original, each field uses Python's `if value:` truthiness guard,
    // so a supplied color/texture index of 0 (or an empty string) is silently
    // skipped.  Reproduced with truthiness, not has_value().
    void UpdateToonProperties(
      std::optional<std::string> head                    = std::nullopt,
      std::optional<std::string> torso                   = std::nullopt,
      std::optional<std::string> legs                    = std::nullopt,
      std::optional<std::string> gender                  = std::nullopt,
      std::optional<int>         armColor                = std::nullopt,
      std::optional<int>         gloveColor              = std::nullopt,
      std::optional<int>         legColor                = std::nullopt,
      std::optional<int>         headColor               = std::nullopt,
      std::optional<int>         topTexture              = std::nullopt,
      std::optional<int>         topTextureColor         = std::nullopt,
      std::optional<int>         sleeveTexture           = std::nullopt,
      std::optional<int>         sleeveTextureColor      = std::nullopt,
      std::optional<int>         bottomTexture           = std::nullopt,
      std::optional<int>         bottomTextureColor      = std::nullopt,
      std::optional<std::tuple<std::string, int>> shirt  = std::nullopt,
      std::optional<std::tuple<std::string, int>> bottom = std::nullopt );

    [[nodiscard]] int GetDefaultColor() const;

    // ------------------------------------------------------------------------
    // Derived classifiers: pure string/index logic
    // ------------------------------------------------------------------------

    [[nodiscard]] std::string GetType() const;
    [[nodiscard]] std::string GetAnimal() const;
    [[nodiscard]] std::string GetHeadSize() const;
    [[nodiscard]] std::string GetMuzzleSize() const;
    [[nodiscard]] std::string GetTorsoSize() const;
    [[nodiscard]] std::string GetLegSize() const;
    [[nodiscard]] std::string GetGender() const;
    [[nodiscard]] std::string GetClothes() const;
    [[nodiscard]] Math::Vec4  GetArmColor() const;
    [[nodiscard]] Math::Vec4  GetLegColor() const;
    [[nodiscard]] Math::Vec4  GetHeadColor() const;
    [[nodiscard]] Math::Vec4  GetGloveColor() const;
    [[nodiscard]] Math::Vec4  GetBlackColor() const;

    [[nodiscard]] ToonDNA     Clone() const;
    [[nodiscard]] std::string GetString() const;

    // The 14-field property tuple (matches NewToonFromProperties args).
    [[nodiscard]] std::tuple<std::string,
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
    AsTuple() const;

    // ------------------------------------------------------------------------
    // Temporary style swap
    // ------------------------------------------------------------------------

    void SetTemporary( const std::string & newHead,
                       int                 newArmColor,
                       int                 newLegColor,
                       int                 newHeadColor );

    void RestoreTemporary( ToonDNA * oldStyle );

    // ------------------------------------------------------------------------
    // Bound to the Python attributes, callers read/write directly.  All plain
    // values; color fields are indices into the shim's color tables.
    // ------------------------------------------------------------------------

    std::string m_Type = "u";
    std::string m_Gender;
    std::string m_Head;
    std::string m_Torso;
    std::string m_Legs;
    int         m_ArmColor           = 0;
    int         m_GloveColor         = 0;
    int         m_LegColor           = 0;
    int         m_HeadColor          = 0;
    int         m_TopTexture         = 0;
    int         m_TopTextureColor    = 0;
    int         m_SleeveTexture      = 0;
    int         m_SleeveTextureColor = 0;
    int         m_BottomTexture      = 0;
    int         m_BottomTextureColor = 0;

    // () in Python when unset; (head, armColor, legColor, headColor) when set.
    std::optional<std::tuple<std::string, int, int, int>> m_Cache;

   private:
    // __defaultColors
    void GetDefaultColors();
  };
}  // namespace Toontown::Toon
