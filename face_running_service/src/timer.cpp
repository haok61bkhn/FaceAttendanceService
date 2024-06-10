
#include "timer.h"
TimePoint Timer::Now() { return std::chrono::high_resolution_clock::now(); }

// Get time eslapsed from `start` to `end` time points.
double Timer::CalcTimeElapsedMs(TimePoint start, TimePoint end) {
  double elapsed_time_ms =
      std::chrono::duration<double, std::milli>(end - start).count();
  return elapsed_time_ms;
}

// Get time eslapsed from `start` to now.
double Timer::CalcTimeElapsedMs(TimePoint start) {
  auto now = Now();
  return CalcTimeElapsedMs(start, now);
}