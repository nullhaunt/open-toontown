#pragma once

namespace Toontown::Math
{
  struct Point3
  {
    float m_X = 0.0f;
    float m_Y = 0.0f;
    float m_Z = 0.0f;

    constexpr Point3() = default;
    constexpr Point3( float x, float y, float z )
      : m_X( x )
      , m_Y( y )
      , m_Z( z )
    {
    }

    friend constexpr bool operator==( const Point3&, const Point3& ) = default;
  };
}  // namespace Toontown::Math
