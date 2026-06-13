#pragma once

namespace Toontown::Math
{
  struct Vec3
  {
    float m_X = 0.0f;
    float m_Y = 0.0f;
    float m_Z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3( float x, float y, float z )
      : m_X( x )
      , m_Y( y )
      , m_Z( z )
    {
    }

    friend constexpr bool operator==( const Vec3&, const Vec3& ) = default;
  };
}  // namespace Toontown::Math
