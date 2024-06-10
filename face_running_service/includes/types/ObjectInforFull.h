#ifndef TYPE_OBJECTINFORFULL_H
#define TYPE_OBJECTINFORFULL_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ObjectInfor.h"
#include "types.h"
namespace types {

class ObjectInforFull {
 public:
  ObjectInfor object;
  std::vector<types::Point> landmark;
  int time_stamp;
  std::string object_image_path;
  std::string camera_id;
  std::string camera_name;
  std::string object_id;
};

}  // namespace types
#endif  // TYPE_OBJECTINFORFULL_H