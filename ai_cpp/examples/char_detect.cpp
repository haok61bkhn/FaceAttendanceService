#include <iostream>
#include <opencv2/opencv.hpp>
#include "char_detection_yolov8.h"
#include "yaml-cpp/yaml.h"

using namespace models;

int main(int argc, char** argv) {
  std::string config_file = "../config/char_det.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["CHAR_DET"];

  std::string onnx_file = config["onnx_file"].as<std::string>();
  std::string engine_file = config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  std::string classes_file = config["lable_file"].as<std::string>();

  CHAR_DETECTION_YOLOV8* char_detector = new CHAR_DETECTION_YOLOV8(
      onnx_file, engine_file, classes_file, BATCH_SIZE, IMAGE_WIDTH,
      IMAGE_HEIGHT, INPUT_CHANNEL);

  //   //   Demo Single Image
  cv::Mat img = cv::imread("../images/aligned_image.jpg");
  //   cv::Mat draw_img = img.clone();
  std::vector<Char_DET> detections;
  for (int i = 0; i < 10; i++) {
    auto start = std::chrono::system_clock::now();
    detections = char_detector->Detect(img);
    std::cout << "number of detections: " << detections.size() << std::endl;
    auto end = std::chrono::system_clock::now();
    auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(
                  end - start)
                  .count() /
              1000.;
    std::cout << "time: " << tc << std::endl;
  }

  //   cv::Mat draw_img = char_detector->DrawChar(img, detections);
  std::string lpn_str = char_detector->GetTextLPN(detections, img.rows);
  std::cout << lpn_str << std::endl;
  cv::imshow("result", img);
  cv::waitKey(0);

  //   std::cout << "number of detections: " << detections.size() << std::endl;
  //   lpn_detector->DrawLPN(draw_img, detections);
  //   cv::Mat img_crop = lpn_detector->Align_LPN(img, detections[0].kps);

  //   cv::imshow("img_crop", img_crop);
  //   cv::waitKey(0);
}
