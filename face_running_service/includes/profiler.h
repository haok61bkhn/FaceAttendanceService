#ifndef PROFILER_H_
#define PROFILER_H_

#include "timer.h"

#include <chrono>
#include <iostream>

/// Profiler to measure system performance.
/// Currently support FPS only
class Profiler {
 public:
  /// Constructor.
  /// FPS will be measure in fps_count_duration (seconds) continuously
  Profiler(double fps_count_duration = 10.0);

  void Reset();

  /// Run this method once when a processing task is done.
  /// Return the current FPS.
  double Tick();
  double GetUptime();

  /// Get current FPS.
  double CurrentFPS();
  unsigned long frame_count_ = 0;  /// FPS count from the last FPS measurement

 private:
  TimePoint first_tp_;                /// Time point on the last FPS measurement
  double current_fps_ = 0;            /// Current FPS
  double fps_count_duration_ = 10.0;  /// Duration to measure FPS. Should be
  /// high enough to get a stable FPS
  long long last_time = 0;
};

#endif