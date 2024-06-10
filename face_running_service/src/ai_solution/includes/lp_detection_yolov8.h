#ifndef LP_DETECTION_YOLOV8_H_
#define LP_DETECTION_YOLOV8_H_

#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "engine_model.h"
#include "image_model.h"
#include "types.h"
using namespace types;
namespace models {
class LP_DETECTION_YOLOV8 : public EngineModel, public ImageModel {
 public:
  LP_DETECTION_YOLOV8(std::string& onnx_file, std::string& engine_file,
                      int batch_size, int input_width, int input_height,
                      int input_chanel, int topk, float score_thres,
                      float iou_thres);
  ~LP_DETECTION_YOLOV8();
  std::vector<LP_DET> Detect(const cv::Mat& img);
  cv::Mat Align_LP(const cv::Mat& img, std::vector<float>& kps);

  cv::Mat DrawLP(cv::Mat& draw_img, std::vector<LP_DET>& detections);

 private:
  bool Preprocess(const cv::Mat& img, std::vector<float>& result, float& ratio,
                  int& left, int& top);
  float Clamp(float val, float min, float max);

 private:
  int batch_size_;
  float score_thres_;
  float iou_thres_;
  int topk_;

  int num_anchors_ = 2100;
  int num_points_ = 4;
  std::vector<std::string> classes_ = {"white_lp", "blue_lp", "red_lp",
                                       "yellow_lp"};
  // std::vector<std::string> classes_ = {"lp"};
  int num_channels_ = 16 + classes_.size();
};
}  // namespace models
#endif