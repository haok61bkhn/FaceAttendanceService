#ifndef FACE_EXTRACTION_H
#define FACE_EXTRACTION_H
#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "engine_model.h"
#include "image_model.h"
#include "types.h"
using namespace types;
namespace models {
class FaceExtraction : public EngineModel, public ImageModel {
 public:
  FaceExtraction(std::string& onnx_file, std::string& engine_file,
                 int batch_size, int input_width, int input_height,
                 int input_chanel);
  ~FaceExtraction();
  std::vector<std::vector<float>> ExtractFaces(
      const std::vector<cv::Mat>& imgs);
  void ExtractFace(cv::Mat& img, std::vector<float>& feature);

 private:
  float Norm(std::vector<float> const& u);
  void L2Norm(std::vector<float>& v);
  void Preprocess(std::vector<cv::Mat>& imgs, std::vector<float>& result);
  void Preprocess(cv::Mat& img, std::vector<float>& result);
  int batch_size_;
};
}  // namespace models
#endif