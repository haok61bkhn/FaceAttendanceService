#ifndef TYPE_FRAME_SUBMIT_INFOR_H
#define TYPE_FRAME_SUBMIT_INFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>

namespace types {

class FrameSubmitInfor {
 public:
  FrameSubmitInfor() = default;
  FrameSubmitInfor(cv::Mat frame, int source_id)
      : frame(frame), source_id(source_id) {}
  cv::Mat frame;
  int source_id;
};
}  // namespace types

#endif  // TYPE_FRAME_SUBMIT_INFOR_H