#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H
#define MIN_FACE_SIZE 50
#include <iostream>
#include <string>
#include "AppConfig.h"
#include "face_alignment.h"
#include "face_detection.h"
#include "face_extraction.h"
#include "yaml-cpp/yaml.h"

namespace models {
class FaceRecognition {
 public:
  FaceRecognition();
  ~FaceRecognition();

 private:
  void Init();
  void InitFaceDetection();
  void InitFaceAlignment();
  void InitFaceExtraction();

 public:
  bool RecognizeFace(const cv::Mat& frame, std::vector<float>& face_feature,
                     cv::Mat& face_crop);
  bool RecognizeFaces(cv::Mat& frame,
                      std::vector<std::vector<float>>& face_features,
                      std::vector<cv::Mat>& face_crops);
  FaceDetection* face_detector;
  FaceAligner* face_aligner;
  FaceExtraction* face_extractor;
};

}  // namespace models
#endif  // FACE_RECOGNITION_H