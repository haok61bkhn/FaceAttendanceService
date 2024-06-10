
#include "image_model.h"

#include <iostream>
#include <string>
#include <vector>

namespace models {

ImageModel::ImageModel(int input_size, int input_chanel) {
  input_height_ = input_size;
  input_width_ = input_size;
  intput_chanel_ = input_chanel;
}

ImageModel::ImageModel(int input_width, int input_height, int input_chanel) {
  input_height_ = input_height;
  input_width_ = input_width;
  intput_chanel_ = input_chanel;
}

int ImageModel::InputWidth() { return input_width_; }

int ImageModel::InputHeight() { return input_height_; }

int ImageModel::InputChanel() { return intput_chanel_; }

bool ImageModel::Preprocess(const cv::Mat& img, std::vector<float>& result,
                            float& ratio) {
  std::cerr << "Preprocess function is not implemented." << std::endl;
  return false;
}

bool ImageModel::Preprocess(const cv::Mat& img, std::vector<float>& result,
                            float& ratio, int& left, int& top) {
  std::cerr << "Preprocess function is not implemented." << std::endl;
  return false;
}

int ImageModel::InputSize() {
  if (input_width_ != input_height_)
    std::cerr
        << "Input width is different from Input height. Getting input size "
           "may be wrong."
        << std::endl;
  return input_width_;
}

}  // namespace models
