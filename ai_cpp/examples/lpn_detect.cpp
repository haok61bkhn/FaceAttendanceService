#include <iostream>
#include <opencv2/opencv.hpp>
#include "lpn_detection_yolov8.h"
#include "yaml-cpp/yaml.h"

using namespace models;

int main(int argc, char** argv) {
  std::string config_file = "../config/lpn_det.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["LPN_DET"];

  std::string onnx_file = config["onnx_file"].as<std::string>();
  std::string engine_file = config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  int topk = config["topk"].as<int>();
  float score_thres = config["score_thres"].as<float>();
  float iou_thres = config["iou_thres"].as<float>();

  LPN_DETECTION_YOLOV8* lpn_detector = new LPN_DETECTION_YOLOV8(
      onnx_file, engine_file, BATCH_SIZE, IMAGE_WIDTH, IMAGE_HEIGHT,
      INPUT_CHANNEL, topk, score_thres, iou_thres);

  //   Demo Single Image
  cv::Mat img = cv::imread("../images/test2.jpg");
  cv::Mat draw_img = img.clone();
  std::vector<LPN_DET> detections;
  for (int i = 0; i < 10; i++) {
    auto start = std::chrono::system_clock::now();
    detections = lpn_detector->Detect(img);
    auto end = std::chrono::system_clock::now();
    auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(
                  end - start)
                  .count() /
              1000.;
    std::cout << "time: " << tc << std::endl;
  }

  std::cout << "number of detections: " << detections.size() << std::endl;
  lpn_detector->DrawLPN(draw_img, detections);
  cv::Mat img_crop = lpn_detector->Align_LPN(img, detections[0].kps);

  cv::imshow("img_crop", img_crop);
  cv::waitKey(0);

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

  //   cv::VideoCapture video = cv::VideoCapture("../videos/test.mp4");
  //   cv::Mat frame;
  //   while (true) {
  //     video >> frame;
  //     clock_t begin = clock();
  //     cv::Mat draw_img = detector->Demo(frame);
  //     clock_t end = clock();
  //     double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  //     std::cout << "time: " << elapsed_secs << std::endl;
  //     // cv::imshow("result", draw_img);
  //     // cv::waitKey(1);
  //   }
  //   return 0;
}
