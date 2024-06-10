#include <includes/ai.h>
#include <includes/types.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace types;
using namespace AI_MODEL;

int main() {
  AI* ai = new AI("model_path", 224, 0.5);
  cv::Mat image = cv::imread("../images/test2.jpg");
  Result result = ai->Process(image);
  std::cout << result.label << std::endl;
  std::cout << result.confidence << std::endl;
  for (int i = 0; i < result.feature.size(); i++) {
    std::cout << result.feature[i] << " ";
  }
  std::cout << std::endl;
  for (int i = 0; i < 4; i++) {
    std::cout << result.box[i] << " ";
  }
  std::cout << std::endl;
  cv::imwrite("test2_result.jpg", result.cropped_image);
  return 0;
}