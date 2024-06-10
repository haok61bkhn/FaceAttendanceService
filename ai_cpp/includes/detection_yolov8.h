#ifndef DETECTION_YOLOV8_H_
#define DETECTION_YOLOV8_H_

#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "engine_model.h"
#include "image_model.h"
#include "types.h"
using namespace types;
namespace models {
class DETECTION_YOLOV8 : public EngineModel, public ImageModel {
 public:
  DETECTION_YOLOV8(std::string& onnx_file, std::string& engine_file,
                   std::string& classes_file, int batch_size, int input_width,
                   int input_height, int input_chanel);
  ~DETECTION_YOLOV8();
  std::vector<std::string> ReadClasses(const std::string& filename);
  std::vector<DET> Detect(const cv::Mat& img);
  cv::Mat DrawDet(cv::Mat& img, std::vector<DET>& detections);

 private:
  bool Preprocess(const cv::Mat& img, std::vector<float>& result, float& ratio,
                  int& left, int& top);
  float Clamp(float val, float min, float max);

 private:
  int batch_size_;
  std::vector<std::string> classes_;
};
}  // namespace models
#endif