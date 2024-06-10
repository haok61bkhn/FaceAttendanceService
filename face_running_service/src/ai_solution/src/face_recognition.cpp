#include "face_recognition.h"

using namespace models;

FaceRecognition::FaceRecognition() { Init(); }

FaceRecognition::~FaceRecognition() {
  if (face_detector != nullptr) delete face_detector;
  if (face_aligner != nullptr) delete face_aligner;
  if (face_extractor != nullptr) delete face_extractor;
}

void FaceRecognition::Init() {
  InitFaceDetection();
  InitFaceAlignment();
  InitFaceExtraction();
}

void FaceRecognition::InitFaceDetection() {
  std::string config_file =
      AppConfig::Instance()->ai_directory + "config/face_det.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["FACE_DET"];

  std::string onnx_file = AppConfig::Instance()->ai_directory +
                          config["onnx_file"].as<std::string>();
  std::string engine_file = AppConfig::Instance()->ai_directory +
                            config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  float obj_threshold = config["obj_threshold"].as<float>();
  float nms_threshold = config["nms_threshold"].as<float>();
  face_detector = new FaceDetection(onnx_file, engine_file, BATCH_SIZE,
                                    IMAGE_WIDTH, IMAGE_HEIGHT, INPUT_CHANNEL,
                                    obj_threshold, nms_threshold);
}
void FaceRecognition::InitFaceAlignment() { face_aligner = new FaceAligner(); }

void FaceRecognition::InitFaceExtraction() {
  std::string config_file =
      AppConfig::Instance()->ai_directory + "config/face_extract.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["FACE_EXTRACT"];
  std::string onnx_file = AppConfig::Instance()->ai_directory +
                          config["onnx_file"].as<std::string>();
  std::string engine_file = AppConfig::Instance()->ai_directory +
                            config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  face_extractor = new FaceExtraction(onnx_file, engine_file, BATCH_SIZE,
                                      IMAGE_WIDTH, IMAGE_HEIGHT, INPUT_CHANNEL);
}

bool FaceRecognition::RecognizeFace(const cv::Mat& frame,
                                    std::vector<float>& face_feature,
                                    cv::Mat& face_crop) {
  std::vector<FaceDetectRes> face_dets = face_detector->Detect(frame);
  if (face_dets.size() == 0) {
    return false;
  }
  int max_index = -1;
  float max_area = 0.0f;
  for (int i = 0; i < face_dets.size(); i++) {
    if ((face_dets[i].x2 - face_dets[i].x1) < MIN_FACE_SIZE ||
        (face_dets[i].y2 - face_dets[i].y1) < MIN_FACE_SIZE) {
      continue;
    }

    float area = (face_dets[i].x2 - face_dets[i].x1) *
                 (face_dets[i].y2 - face_dets[i].y1);
    if (area > max_area) {
      max_area = area;
      max_index = i;
    }
  }
  if (max_index == -1) {
    return false;
  }
  int padding = 5;
  int x1 = std::max(0, (int)face_dets[max_index].x1 - padding);
  int y1 = std::max(0, (int)face_dets[max_index].y1 - padding);
  int x2 = std::min(frame.cols, (int)face_dets[max_index].x2 + padding);
  int y2 = std::min(frame.rows, (int)face_dets[max_index].y2 + padding);
  face_crop = frame(cv::Rect(x1, y1, x2 - x1, y2 - y1)).clone();
  face_dets[max_index].aligned_face =
      face_aligner->AlignFace(frame, face_dets[max_index].landmark);
  face_extractor->ExtractFace(face_dets[max_index].aligned_face, face_feature);
  return true;
}

bool FaceRecognition::RecognizeFaces(
    cv::Mat& frame, std::vector<std::vector<float>>& face_features,
    std::vector<cv::Mat>& face_crops) {
  std::vector<FaceDetectRes> face_dets = face_detector->Detect(frame);
  if (face_dets.size() == 0) {
    return false;
  }
  for (int i = 0; i < face_dets.size(); i++) {
    if ((face_dets[i].x2 - face_dets[i].x1) < MIN_FACE_SIZE ||
        (face_dets[i].y2 - face_dets[i].y1) < MIN_FACE_SIZE) {
      continue;
    }
    int padding = 5;
    int x1 = std::max(0, (int)face_dets[i].x1 - padding);
    int y1 = std::max(0, (int)face_dets[i].y1 - padding);
    int x2 = std::min(frame.cols, (int)face_dets[i].x2 + padding);
    int y2 = std::min(frame.rows, (int)face_dets[i].y2 + padding);
    cv::Mat face_crop = frame(cv::Rect(x1, y1, x2 - x1, y2 - y1)).clone();
    face_crops.push_back(face_crop);
    face_dets[i].aligned_face =
        face_aligner->AlignFace(frame, face_dets[i].landmark);
    std::vector<float> face_feature;
    face_extractor->ExtractFace(face_dets[i].aligned_face, face_feature);
    face_features.push_back(face_feature);
  }
  return true;
}
