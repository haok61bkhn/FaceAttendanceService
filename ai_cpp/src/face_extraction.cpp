#include "face_extraction.h"
namespace models {
FaceExtraction::FaceExtraction(std::string& onnx_file, std::string& engine_file,
                               int batch_size, int input_width,
                               int input_height, int input_chanel)
    : EngineModel(onnx_file, engine_file, batch_size),
      ImageModel(input_width, input_height, input_chanel) {
  batch_size_ = batch_size;
  std::cout << "FaceExtraction init" << std::endl;
  LoadEngine();
  std::cout << "LoadEngine successfully!" << std::endl;
}

FaceExtraction::~FaceExtraction() {}

float FaceExtraction::Norm(std::vector<float> const& u) {
  float accum = 0.;
  for (int i = 0; i < u.size(); ++i) {
    accum += u[i] * u[i];
  }
  return std::sqrt(accum);
}

void FaceExtraction::L2Norm(std::vector<float>& v) {
  float k = Norm(v);
  std::transform(v.begin(), v.end(), v.begin(),
                 [k](float& c) { return c / k; });
}
void FaceExtraction::Preprocess(std::vector<cv::Mat>& imgs,
                                std::vector<float>& result) {
  int input_height_ = InputHeight();
  int input_width_ = InputWidth();
  int input_chanel_ = InputChanel();
  result.resize(batch_size_ * input_width_ * input_height_ * input_chanel_);
  float* data = result.data();

  int index = 0;
  for (const cv::Mat& src_img : imgs) {
    if (!src_img.data) continue;
    cv::Mat flt_img =
        cv::Mat::zeros(cv::Size(input_width_, input_height_), CV_8UC3);
    src_img.copyTo(flt_img(cv::Rect(0, 0, src_img.cols, src_img.rows)));
    flt_img.convertTo(flt_img, CV_32FC3, 1.f / 255);
    int channelLength = input_width_ * input_height_;
    std::vector<cv::Mat> split_img = {
        cv::Mat(input_width_, input_height_, CV_32FC1,
                data + channelLength * (index + 2)),
        cv::Mat(input_width_, input_height_, CV_32FC1,
                data + channelLength * (index + 1)),
        cv::Mat(input_width_, input_height_, CV_32FC1,
                data + channelLength * index)};
    index += 3;
    for (int i = 0; i < input_chanel_; i++) {
      split_img[i] = split_img[i] - 0.5f;
    }
    cv::split(flt_img, split_img);
  }
}

std::vector<std::vector<float>> FaceExtraction::ExtractFace(
    const std::vector<cv::Mat>& imgs) {
  std::vector<std::vector<float>> features;
  int index = 0;
  int batch_id = 0;
  int num_face = 0;
  std::vector<cv::Mat> vec_Mat(batch_size_);
  std::vector<std::string> vec_name(batch_size_);
  features.clear();
  features.resize(imgs.size());
  for (const cv::Mat& img : imgs) {
    index++;
    if (img.data) {
      vec_Mat[batch_id] = img.clone();
      batch_id++;
    }
    if (batch_id == batch_size_ or index == imgs.size()) {
      // preprocess batch images

      std::vector<float> input_data_;
      Preprocess(vec_Mat, input_data_);
      EngineInference(input_data_);
      for (int k = 0; k < batch_id; k++) {
        std::vector<float> feature{&output_infers_[0][k * 512],
                                   &output_infers_[0][k * 512] + 512};
        L2Norm(feature);
        features[num_face] = feature;
        num_face++;
      }
    }
  }
  return features;
}
}  // namespace models