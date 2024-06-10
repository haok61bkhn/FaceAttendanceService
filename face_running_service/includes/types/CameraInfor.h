#ifndef CAMERA_INFOR_H
#define CAMERA_INFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace types {
class CameraInfor {
 public:
  CameraInfor() {}
  CameraInfor(std::string& camera_id, std::string& camera_name,
              std::string& camera_url, int camera_index)
      : camera_id(camera_id),
        camera_name(camera_name),
        camera_url(camera_url),
        camera_index(camera_index) {}
  std::string camera_id;
  std::string camera_name;
  std::string camera_url;
  std::string main_image_path;
  std::string polygon_str;
  std::vector<cv::Point> polygon;
  int camera_index;
  std::string
      status;  // "Online" or "Offline" or "Reconnecting" or "Connecting"
};
}  // namespace types
#endif  // CAMERA_INFOR_H