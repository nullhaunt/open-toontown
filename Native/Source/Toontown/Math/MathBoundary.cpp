#include "Toontown/Math/PandaConvert.hpp"
#include "Toontown/Math/Point3.hpp"
#include "Toontown/Math/Vec3.hpp"
#include "Registry.hpp"

namespace
{
  /**
   * Round-trips a Vec3 through the native type, negating it so the test can
   * confirm the value actually crossed into C++ and back (not passed through).
   */
  Toontown::Math::Vec3 NegateVec3( Toontown::Math::Vec3 v )
  {
    return { -v.m_X, -v.m_Y, -v.m_Z };
  }

  Toontown::Math::Point3 NegatePoint3( Toontown::Math::Point3 p )
  {
    return { -p.m_X, -p.m_Y, -p.m_Z };
  }
}  // namespace

void RegisterMathBoundary( nanobind::module_& math )
{
  math.def( "_acltestNegateVec3", &NegateVec3, nanobind::arg( "v" ) );
  math.def( "_acltestNegatePoint3", &NegatePoint3, nanobind::arg( "p" ) );
}
