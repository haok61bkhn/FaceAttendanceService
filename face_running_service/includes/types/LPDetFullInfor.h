#ifndef LP_DET_FULL_INFOR_H
#define LP_DET_FULL_INFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
namespace types {

class LP_DET_FULL {
 public:
  LP_DET_FULL() {}
  LP_DET_FULL(cv::Mat& cropped_image, cv::Mat& object_image,
              std::string& object_id, std::string& object_type, int& time_stamp,
              float& area)
      : cropped_image(cropped_image),
        object_image(object_image),
        object_id(object_id),
        object_type(object_type),
        time_stamp(time_stamp),
        area(area) {}

  float area = 0.0;
  cv::Mat cropped_image;
  cv::Mat object_image;
  int time_stamp;
  std::string object_id;
  std::string object_type;
  std::string camera_id;
};
}  // namespace types
#endif  // LP_DET_FULL_INFOR_H