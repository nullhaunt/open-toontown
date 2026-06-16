#pragma once

namespace Toontown::Math
{
  struct Vec4
  {
    float m_X = 0.0f;
    float m_Y = 0.0f;
    float m_Z = 0.0f;
    float m_W = 0.0f;

    constexpr Vec4() = default;
    constexpr Vec4( float x, float y, float z, float w )
      : m_X( x )
      , m_Y( y )
      , m_Z( z )
      , m_W( w )
    {
    }

    friend constexpr bool operator==( const Vec4 &, const Vec4 & ) = default;
  };
}  // namespace Toontown::Math
