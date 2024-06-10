#ifndef FRAME_SAVING_H
#define FRAME_SAVING_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
namespace types {

class FrameSaving {
 public:
  FrameSaving() {}
  FrameSaving(cv::Mat& frame, std::string& image_path)
      : frame(frame), image_path(image_path) {}
  cv::Mat frame;
  std::string image_path;
};
}  // namespace types
#endif  // FRAME_SAVING_H