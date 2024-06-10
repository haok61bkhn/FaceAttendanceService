#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H
#include <opencv2/opencv.hpp>
#include "engine_model.h"
#include "image_model.h"
#include "types.h"
using namespace types;
namespace models {
class FaceDetection : public EngineModel, public ImageModel {
 public:
  FaceDetection(std::string& onnx_file, std::string& engine_file,
                int batch_size, int input_width, int input_height,
                int input_chanel, float obj_threshold, float nms_threshold);
  ~FaceDetection();
  std::vector<FaceDetectRes> Detect(const cv::Mat& img);
  void DrawFace(cv::Mat img, std::vector<FaceDetectRes>& boxes, bool landmark);
  cv::Mat Demo(cv::Mat& img);

 private:
  void NMS(std::vector<FaceDetectRes>& input_boxes, float NMS_THRESH);
  void FilterBoxes(std::vector<FaceDetectRes>& predicted_boxes);
  bool Preprocess(const cv::Mat& img, std::vector<float>& result, float& ratio);
  std::vector<std::vector<float>> GenerateAnchorCenters(int stride);
  void GenerateProposals(const std::vector<std::vector<float>>& anchors,
                         int feat_stride, const std::vector<float>& score_blob,
                         const std::vector<float>& bbox_blob,
                         const std::vector<float>& kps_blob,
                         std::vector<FaceDetectRes>& faceobjects);

 private:
  std::vector<int> list_stride_{8, 16, 32};
  int batch_size_;
  float nms_threshold_;
  float obj_threshold_;
  std::vector<std::vector<float>> anchors_;
  std::vector<std::vector<std::vector<float>>> all_anchor_centers_;
};
}  // namespace models
#endif