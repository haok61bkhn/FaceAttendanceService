
#ifndef FACE_ATTENDANCE_INFOR_H
#define FACE_ATTENDANCE_INFOR_H

#include <iostream>

namespace types {
class FaceAttendanceInfor {
 public:
  FaceAttendanceInfor() = default;
  FaceAttendanceInfor(std::string& face_id, std::string& object_image_path,
                      std::string& object_image_full_path,
                      std::string& camera_id, std::string& camera_name,
                      int time_stamp)
      : face_id(face_id),
        object_image_path(object_image_path),
        object_image_full_path(object_image_full_path),
        camera_id(camera_id),
        camera_name(camera_name),
        time_stamp(time_stamp) {}
  std::string face_id;
  std::string object_image_path;
  std::string object_image_full_path;
  std::string camera_id;
  std::string camera_name;
  int time_stamp;
};
}  // namespace types
#endif  // FACE_ATTENDANCE_INFOR_H