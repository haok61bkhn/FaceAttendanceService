#ifndef TYPE_OBJECTINFOR_H
#define TYPE_OBJECTINFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>

namespace types {

class ObjectInfor {
 public:
  int class_id;
  std::string class_name;
  cv::Mat object_image;
  cv::Mat object_image_full;
  float confidence;
  cv::Rect rect;
  int tracking_id;
  bool is_lp;
  bool is_face;
  std::vector<cv::Point> key_points;  // of full image
};
}  // namespace types

#endif  // TYPE_OBJECTINFOR_H