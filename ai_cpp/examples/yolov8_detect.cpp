#include <iostream>
#include <opencv2/opencv.hpp>
#include "detection_yolov8.h"
#include "yaml-cpp/yaml.h"

using namespace models;

int main(int argc, char** argv) {
  std::string config_file = "../config/yolov8_det.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["OBJECT_DET"];

  std::string onnx_file = config["onnx_file"].as<std::string>();
  std::string engine_file = config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  std::string classes_file = config["lable_file"].as<std::string>();

  DETECTION_YOLOV8* detector =
      new DETECTION_YOLOV8(onnx_file, engine_file, classes_file, BATCH_SIZE,
                           IMAGE_WIDTH, IMAGE_HEIGHT, INPUT_CHANNEL);

  // //   //   Demo Single Image
  // cv::Mat img = cv::imread("../images/test_det.jpg");
  // std::vector<DET> detections;
  // for (int i = 0; i < 10; i++) {
  //   auto start = std::chrono::system_clock::now();
  //   detections = detector->Detect(img);
  //   std::cout << "number of detections: " << detections.size() << std::endl;
  //   auto end = std::chrono::system_clock::now();
  //   auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(
  //                 end - start)
  //                 .count() /
  //             1000.;
  //   std::cout << "time: " << tc << std::endl;
  // }
  // cv::Mat draw_img = detector->DrawDet(img, detections);
  // cv::imshow("result", draw_img);
  // cv::waitKey(0);

  //   Demo Video
  std::string video_path = "/home/haobk/video.mp4";
  cv::VideoCapture cap(video_path);
  cv::Mat img;
  std::vector<DET> detections;
  while (cap.isOpened()) {
    cap >> img;
    if (img.empty()) break;
    auto start = std::chrono::system_clock::now();
    detections = detector->Detect(img);
    auto end = std::chrono::system_clock::now();
    auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(
                  end - start)
                  .count() /
              1000.;
    std::cout << "time: " << tc << std::endl;
    std::cout << "number of detections: " << detections.size() << std::endl;
    cv::Mat draw_img = detector->DrawDet(img, detections);
    cv::imshow("result", draw_img);
    cv::waitKey(1);
  }

  return 0;
}
