#pragma once

#include <optional>

namespace Toontown::Minigame
{
  class DropScheduler
  {
   public:
    DropScheduler( double                gameDuration,
                   double                firstDropDelay,
                   double                dropPeriod,
                   double                maxDropDuration,
                   double                fasterDropDelay,
                   double                fasterDropPeriodMult,
                   std::optional<double> startTime = std::nullopt );
    virtual ~DropScheduler() = default;

    [[nodiscard]] double         GetTime() const;
    [[nodiscard]] double         GetDuration() const;
    [[nodiscard]] virtual double GetDropPeriod() const;
    [[nodiscard]] bool           IsDoneDropping(
                std::optional<bool> isContinuous = std::nullopt ) const;
    int  SkipPercent( double percent );
    void Step();

   protected:
    double m_GameDuration;
    double m_FirstDropDelay;
    double m_DropPeriod;
    double m_MaxDropDuration;
    double m_FasterDropDelay;
    double m_FasterDropPeriodMult;
    double m_StartTime;
    double m_Time;
  };

  class ThreePhaseDropScheduler : public DropScheduler
  {
   public:
    ThreePhaseDropScheduler( double                gameDuration,
                             double                firstDropDelay,
                             double                dropPeriod,
                             double                maxDropDuration,
                             double                slowerDropPeriodMult,
                             double                normalDropDelay,
                             double                fasterDropDelay,
                             double                fasterDropPeriodMult,
                             std::optional<double> startTime = std::nullopt );

    [[nodiscard]] double GetDropPeriod() const override;

   private:
    double m_SlowerDropPeriodMult;
    double m_NormalDropDelay;
  };
}  // namespace Toontown::Minigame
