#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>
#include <ctime>
typedef std::chrono::high_resolution_clock::time_point TimePoint;
class Timer {
 public:
  static TimePoint Now();
  static double CalcTimeElapsedMs(TimePoint start, TimePoint end);
  static double CalcTimeElapsedMs(TimePoint start);
};

#endif