#pragma once

#include <chrono>

class DeltaTime
{
private:
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::milliseconds;

public:
  using TickType = Duration::rep;

  DeltaTime() = default;

  auto get() noexcept -> TickType;

private:
  Clock::time_point last_frame_time;
};
