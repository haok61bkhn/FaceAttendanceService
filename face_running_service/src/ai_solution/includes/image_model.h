#ifndef IMAGE_MODEL_H_
#define IMAGE_MODEL_H_

#include <opencv2/opencv.hpp>

namespace models {
class ImageModel {
 public:
  ImageModel(int input_size, int input_chanel);
  ImageModel(int input_width, int input_height, int input_chanel);

 protected:
  /// Preprocess function for image model.
  /// Get an cv::Mat in and return a net input.
  bool Preprocess(const cv::Mat& img, std::vector<float>& result, float& ratio);
  bool Preprocess(const cv::Mat& img, std::vector<float>& result, float& ratio,
                  int& left, int& top);
  /// Get net input width
  int InputWidth();
  /// Get net input height
  int InputHeight();
  /// Get input size (in case input width = input height)
  int InputSize();
  /// Get input channel
  int InputChanel();

 private:
  int input_width_;
  int input_height_;
  int intput_chanel_;
};

}  // namespace models

#endif