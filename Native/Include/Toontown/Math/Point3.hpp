#pragma once

#include "Vec3.hpp"

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

    friend constexpr bool operator==( const Point3 &,
                                      const Point3 & ) = default;
  };

  // Panda semantics: subtracting two positions yields a displacement.
  [[nodiscard]] constexpr Vec3 operator-( const Point3 & a, const Point3 & b )
  {
    return { a.m_X - b.m_X, a.m_Y - b.m_Y, a.m_Z - b.m_Z };
  }
}  // namespace Toontown::Math
