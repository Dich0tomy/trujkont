#include "trujkont/delta_time/delta_time.hpp"

auto DeltaTime::get() noexcept -> TickType
{
  auto const now = Clock::now();
  auto const delta_time = std::chrono::duration_cast<Duration>(now - last_frame_time).count();

  last_frame_time = now;

  return delta_time;
}
