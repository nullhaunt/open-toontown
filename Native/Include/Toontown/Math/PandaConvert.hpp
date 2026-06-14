// This is the only place that knows Panda3D's types exist.  Conversion goes
// through the Python object (getX/getY/getZ on the way in, the panda3d.core
// constructor on the way out), so the native build links nothing from Panda3D.
// When Panda3D is removed, this one header will be deleted and the math core
// will be untouched.
//
// Including this header makes native Vec3/Point3 usable directly as nanobind
// function arguments and return values; callers pass/receive real
// panda3d.core.Vec3/Point3 objects.
#pragma once

#include <nanobind/nanobind.h>

#include "Vec3.hpp"
#include "Point3.hpp"

namespace nanobind::detail
{
  template <>
  struct type_caster<Toontown::Math::Vec3>
  {
    NB_TYPE_CASTER( Toontown::Math::Vec3, const_name( "panda3d.core.Vec3" ) )

    bool from_python( handle src, std::uint8_t, cleanup_list * ) noexcept
    {
      try
      {
        value.m_X = nanobind::cast<float>( src.attr( "getX" )() );
        value.m_Y = nanobind::cast<float>( src.attr( "getY" )() );
        value.m_Z = nanobind::cast<float>( src.attr( "getZ" )() );
        return true;
      }
      catch ( ... )
      {
        return false;
      }
    }

    static handle from_cpp( const Toontown::Math::Vec3 & v,
                            rv_policy,
                            cleanup_list * ) noexcept
    {
      try
      {
        const module_ core = module_::import_( "panda3d.core" );
        return core.attr( "Vec3" )( v.m_X, v.m_Y, v.m_Z ).release();
      }
      catch ( ... )
      {
        return {};
      }
    }
  };

  template <>
  struct type_caster<Toontown::Math::Point3>
  {
    NB_TYPE_CASTER( Toontown::Math::Point3,
                    const_name( "panda3d.core.Point3" ) )

    bool from_python( handle src, std::uint8_t, cleanup_list * ) noexcept
    {
      try
      {
        value.m_X = nanobind::cast<float>( src.attr( "getX" )() );
        value.m_Y = nanobind::cast<float>( src.attr( "getY" )() );
        value.m_Z = nanobind::cast<float>( src.attr( "getZ" )() );
        return true;
      }
      catch ( ... )
      {
        return false;
      }
    }

    static handle from_cpp( const Toontown::Math::Point3 & p,
                            rv_policy,
                            cleanup_list * ) noexcept
    {
      try
      {
        const module_ core = module_::import_( "panda3d.core" );
        return core.attr( "Point3" )( p.m_X, p.m_Y, p.m_Z ).release();
      }
      catch ( ... )
      {
        return {};
      }
    }
  };
};  // namespace nanobind::detail
