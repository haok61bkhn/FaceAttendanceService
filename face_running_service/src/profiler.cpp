#include "profiler.h"

Profiler::Profiler(double fps_count_duration)
    : fps_count_duration_(fps_count_duration)

{
  first_tp_ = Timer::Now();
}

void Profiler::Reset() {
  first_tp_ = Timer::Now();
  frame_count_ = 0;
  current_fps_ = 0;
}

double Profiler::Tick() {
  // Calculate elapsed time from the last time point that FPS was measured.
  const double uptime_sec = Timer::CalcTimeElapsedMs(first_tp_) / 1000.0;
  ++frame_count_;

  // Recalculate FPS after a time duration
  if (uptime_sec > fps_count_duration_) {
    if (uptime_sec == 0) return 0;
    current_fps_ = frame_count_ / uptime_sec;
    first_tp_ = Timer::Now();
    frame_count_ = 0;
  }
  return current_fps_;
}

double Profiler::CurrentFPS() { return current_fps_; }
double Profiler::GetUptime() {
  const double uptime_sec = Timer::CalcTimeElapsedMs(first_tp_) / 1000.0;
  return uptime_sec;
}