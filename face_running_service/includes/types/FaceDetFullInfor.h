#ifndef FACE_DET_FULL_INFOR_H
#define FACE_DET_FULL_INFOR_H
#include <iostream>
#include <opencv2/opencv.hpp>

namespace types {
class FaceDetFullInfor {
 public:
  FaceDetFullInfor() = default;
  FaceDetFullInfor(cv::Mat& aligned_face, cv::Mat& object_image,
                   cv::Mat& object_image_full, std::string& object_id,
                   int& time_stamp, float& area, std::string& camera_id,
                   std::string& camera_name)
      : aligned_face(aligned_face),
        object_image(object_image),
        object_image_full(object_image_full),
        object_id(object_id),
        time_stamp(time_stamp),
        area(area),
        camera_id(camera_id),camera_name(camera_name) {}
  cv::Mat aligned_face;
  cv::Mat object_image;
  cv::Mat object_image_full;
  int time_stamp;
  std::string object_id;
  std::string camera_id;
  std::string camera_name;
  float area;
};
}  // namespace types
#endif  // FACE_DET_FULL_INFOR_H