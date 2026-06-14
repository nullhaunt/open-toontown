#pragma once

#include <optional>
#include <string>

namespace Toontown::Suit
{
  // --------------------------------------------------------------------------
  // Deterministic classifiers (bit-parity).
  // --------------------------------------------------------------------------
  std::string GetSuitBodyType( const std::string & name );  // "a"/"b"/"c"
  std::string GetSuitDept( const std::string & name );      // dept char
  int         GetSuitType( const std::string & name );      // 1..8 within dept

  // --------------------------------------------------------------------------
  // Native-RNG generators (invariant tested, no bit-parity).
  // --------------------------------------------------------------------------
  int         GetRandomSuitType( int level );
  std::string GetRandomSuitByDept( const std::string & dept );

  class SuitDNA
  {
   public:
    SuitDNA() = default;

    [[nodiscard]] std::string MakeNetString() const;  // Raw wire bytes
    void                      MakeFromNetString( const std::string & bytes );

    void NewSuit( std::optional<std::string> name = std::nullopt );
    void NewBossCog( const std::string & dept );
    void NewSuitRandom( std::optional<int>         level = std::nullopt,
                        std::optional<std::string> dept  = std::nullopt );
    void NewGoon( std::optional<std::string> name = std::nullopt );

    [[nodiscard]] std::string GetType() const;    // "suit"/"boss"
    [[nodiscard]] std::string GetString() const;  // __str__

    // Public so the bindings expose them as the Python attributes
    // type/name/dept/body, which callers read and write directly.
    std::string m_Type = "u";
    std::string m_Name;
    std::string m_Dept;
    std::string m_Body;
  };
}  // namespace Toontown::Suit
