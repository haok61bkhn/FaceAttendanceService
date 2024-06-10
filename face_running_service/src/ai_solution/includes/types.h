#ifndef TYPES_H_
#define TYPES_H_
#include <iostream>
#include <opencv2/opencv.hpp>

namespace types {
class Result {
 public:
  std::string label;
  float confidence;
  cv::Mat cropped_image;
  std::vector<float> feature;
  std::vector<int> box = {0, 0, 0, 0};
};
struct Point {
  float x;
  float y;
  float prob;
};

struct FaceDetectRes {
  float x1;
  float y1;
  float x2;
  float y2;
  std::vector<Point> landmark = std::vector<Point>(5);
  cv::Mat aligned_face;
  float score;
};

struct Rect {
  float x;
  float y;
  float width;
  float height;
};

struct LP_DET {
  Rect rect;
  std::string label = "";
  float prob = 0.0;
  std::vector<float> kps;
  float area = 0.0;
  cv::Mat cropped_image;
};

struct Char_DET {
  Rect rect;
  int label = 0;
  float prob = 0.0;
};

struct DET {
  Rect rect;
  int label = 0;
  float prob = 0.0;
};

struct FaceSearchResult {
  std::string pid;
  float min_distance;
  float score;
};

}  // namespace types

#endif