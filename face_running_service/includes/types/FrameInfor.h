#ifndef TYPE_FRAMEINFOR_H
#define TYPE_FRAMEINFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ObjectInfor.h"

namespace types {
class FrameInfor {
 public:
  cv::Mat frame;
  std::vector<types::ObjectInfor> objects;
  float fps = 0;
  int frame_number;
  int last_time;
  int time_stamp;
  int source_id;
};
}  // namespace types
#endif  // TYPE_FRAMEINFOR_H