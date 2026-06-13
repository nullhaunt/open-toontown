#include "Toontown/Minigame/DropScheduler.hpp"

#include <nanobind/stl/optional.h>

#include "Registry.hpp"

namespace Toontown::Minigame
{
  DropScheduler::DropScheduler( double                gameDuration,
                                double                firstDropDelay,
                                double                dropPeriod,
                                double                maxDropDuration,
                                double                fasterDropDelay,
                                double                fasterDropPeriodMult,
                                std::optional<double> startTime )
    : m_GameDuration( gameDuration )
    , m_FirstDropDelay( firstDropDelay )
    , m_DropPeriod( dropPeriod )
    , m_MaxDropDuration( maxDropDuration )
    , m_FasterDropDelay( fasterDropDelay )
    , m_FasterDropPeriodMult( fasterDropPeriodMult )
    , m_StartTime( startTime.value_or( 0.0 ) )
    , m_Time( m_StartTime + m_FirstDropDelay )
  {
  }

  double DropScheduler::GetTime() const
  {
    return m_Time;
  }

  double DropScheduler::GetDuration() const
  {
    return m_GameDuration;
  }

  double DropScheduler::GetDropPeriod() const
  {
    double delay = m_DropPeriod;

    if ( m_Time - m_StartTime >= m_FasterDropDelay )
    {
      delay *= m_FasterDropPeriodMult;
    }

    return delay;
  }

  /**
   * The original code branches on `continuous is None`, not the value:
   *  if continuous is None:  continuous = False
   *  else:                   continuous = True
   *
   * Ergo, any supplied argument is treated as continuous, including
   * IsDoneDropping( false ) -- only an omitted/None argument yields false.
   * This contradicts the parameter's name, but no live caller passes false, so
   * it is harmless.
   *
   * has_value() mirrors the `is None` test exactly: preserved deliberately for
   * behavioral parity.
   */
  bool DropScheduler::IsDoneDropping( std::optional<bool> isContinuous ) const
  {
    const double landTime = m_Time - m_StartTime + m_MaxDropDuration;
    const double maxTime  = isContinuous.has_value()
                              ? m_GameDuration + m_MaxDropDuration
                              : m_GameDuration + GetDropPeriod();
    return landTime >= maxTime;
  }

  int DropScheduler::SkipPercent( double percent )
  {
    int skips = 0;

    while ( true )
    {
      const double previousTime = m_Time;
      Step();

      if ( m_Time >= percent * m_GameDuration )
      {
        m_Time = previousTime;
        break;
      }

      ++skips;
    }

    return skips;
  }

  void DropScheduler::Step()
  {
    m_Time += GetDropPeriod();
  }

  ThreePhaseDropScheduler::ThreePhaseDropScheduler(
    double                gameDuration,
    double                firstDropDelay,
    double                dropPeriod,
    double                maxDropDuration,
    double                slowerDropPeriodMult,
    double                normalDropDelay,
    double                fasterDropDelay,
    double                fasterDropPeriodMult,
    std::optional<double> startTime )
    : DropScheduler( gameDuration,
                     firstDropDelay,
                     dropPeriod,
                     maxDropDuration,
                     fasterDropDelay,
                     fasterDropPeriodMult,
                     startTime )
    , m_SlowerDropPeriodMult( slowerDropPeriodMult )
    , m_NormalDropDelay( normalDropDelay )
  {
  }

  double ThreePhaseDropScheduler::GetDropPeriod() const
  {
    double delay = m_DropPeriod;

    if ( m_Time - m_StartTime < m_NormalDropDelay )
    {
      delay *= m_SlowerDropPeriodMult;
    }
    else if ( m_Time - m_StartTime >= m_FasterDropDelay )
    {
      delay *= m_FasterDropPeriodMult;
    }

    return delay;
  }
}  // namespace Toontown::Minigame

void RegisterMinigameDropScheduler( nanobind::module_& minigame )
{
  using namespace Toontown::Minigame;

  nanobind::class_<DropScheduler>( minigame, "DropScheduler" )
    .def( nanobind::init<double,
                         double,
                         double,
                         double,
                         double,
                         double,
                         std::optional<double>>(),
          nanobind::arg( "gameDuration" ),
          nanobind::arg( "firstDropDelay" ),
          nanobind::arg( "dropPeriod" ),
          nanobind::arg( "maxDropDuration" ),
          nanobind::arg( "fasterDropDelay" ),
          nanobind::arg( "fasterDropPeriodMult" ),
          nanobind::arg( "startTime" ) = std::optional<double>() )
    .def( "getT", &DropScheduler::GetTime )
    .def( "getDuration", &DropScheduler::GetDuration )
    .def( "getDropPeriod", &DropScheduler::GetDropPeriod )
    .def( "doneDropping",
          &DropScheduler::IsDoneDropping,
          nanobind::arg( "continuous" ) = std::optional<bool>() )
    .def(
      "skipPercent", &DropScheduler::SkipPercent, nanobind::arg( "percent" ) )
    .def( "stepT", &DropScheduler::Step );

  nanobind::class_<ThreePhaseDropScheduler, DropScheduler>(
    minigame, "ThreePhaseDropScheduler" )
    .def( nanobind::init<double,
                         double,
                         double,
                         double,
                         double,
                         double,
                         double,
                         double,
                         std::optional<double>>(),
          nanobind::arg( "gameDuration" ),
          nanobind::arg( "firstDropDelay" ),
          nanobind::arg( "dropPeriod" ),
          nanobind::arg( "maxDropDuration" ),
          nanobind::arg( "slowerDropPeriodMult" ),
          nanobind::arg( "normalDropDelay" ),
          nanobind::arg( "fasterDropDelay" ),
          nanobind::arg( "fasterDropPeriodMult" ),
          nanobind::arg( "startTime" ) = std::optional<double>() );
}
