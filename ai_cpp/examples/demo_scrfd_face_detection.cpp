#include <iostream>
#include <opencv2/opencv.hpp>
#include "scrfd_face.h"
#include "yaml-cpp/yaml.h"

using namespace models;

int main(int argc, char** argv) {
  std::string config_file = "../config/scrfd.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["SCRFD"];

  std::string onnx_file = config["onnx_file"].as<std::string>();
  std::string engine_file = config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  float obj_threshold = config["obj_threshold"].as<float>();
  float nms_threshold = config["nms_threshold"].as<float>();

  SCRFD* detector =
      new SCRFD(onnx_file, engine_file, BATCH_SIZE, IMAGE_WIDTH, IMAGE_HEIGHT,
                INPUT_CHANNEL, obj_threshold, nms_threshold);

  // Demo Single Image
  // cv::Mat img = cv::imread("../images/st.jpg");
  // cv::Mat draw_img = img.clone();
  // std::vector<FaceDetectRes> detections;
  // detector->Detect(img, detections);
  // detector->DrawFace(draw_img, detections, true);

  // cv::imshow("result", draw_img);
  // cv::waitKey(0);

  //   // Demo Camera
  //   cv::VideoCapture cap(0);
  //   cv::Mat frame;
  //   while (true) {
  //     cap >> frame;
  //     cv::Mat draw_img = frame.clone();
  //     std::vector<FaceDetectRes> detections;
  //     detector->Detect(frame, detections);
  //     detector->DrawFace(draw_img, detections);
  //     cv::imshow("result", draw_img);
  //     cv::waitKey(1);
  //   }

  // cv::Mat img = cv::imread("../images/st.jpg");
  // cv::Mat draw_img = detector->Demo(img);

  cv::VideoCapture video = cv::VideoCapture("../videos/test.mp4");
  cv::Mat frame;
  while (true) {
    video >> frame;
    auto start = std::chrono::system_clock::now();
    cv::Mat draw_img = detector->Demo(frame);
    auto end = std::chrono::system_clock::now();
    auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(
                  end - start)
                  .count() /
              1000.;
    std::cout << "time: " << tc << std::endl;
    // cv::imshow("result", draw_img);
    // cv::waitKey(1);
  }
  return 0;
}
