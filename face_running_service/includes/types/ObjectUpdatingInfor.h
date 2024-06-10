#ifndef TYPE_OBJECTUPDATINGINFOR_H
#define TYPE_OBJECTUPDATINGINFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>

namespace types {

class ObjectUpdatingInfor {
 public:
  ObjectUpdatingInfor() = default;
  ObjectUpdatingInfor(std::string& object_id, cv::Mat& object_image,
                      cv::Mat& object_image_full, float& area, int& time_stamp,
                      std::string& camera_id, std::string& camera_name)
      : object_id(object_id),
        object_image(object_image),
        object_image_full(object_image_full),
        area(area),
        time_stamp(time_stamp),
        camera_id(camera_id),
        camera_name(camera_name) {}
  float area;
  std::string object_id;
  std::string camera_name;
  cv::Mat object_image;
  cv::Mat object_image_full;
  int time_stamp;
  std::string camera_id;
  std::vector<float> feature;
};
}  // namespace types

#endif  // TYPE_OBJECTUPDATINGINFOR_H