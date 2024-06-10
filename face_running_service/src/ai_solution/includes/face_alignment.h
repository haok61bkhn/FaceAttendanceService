#ifndef FACE_ALIGNMENT_H_
#define FACE_ALIGNMENT_H_
#include <opencv2/opencv.hpp>
#include <vector>
#include "types.h"
namespace models {
class FaceAligner {
 public:
  FaceAligner();
  ~FaceAligner();
  cv::Mat AlignFace(const cv::Mat& img, std::vector<types::Point>& landmark);

 private:
  cv::Mat MeanAxis0(const cv::Mat& src);
  cv::Mat ElementwiseMinus(const cv::Mat& A, const cv::Mat& B);
  cv::Mat VarAxis0(const cv::Mat& src);
  int MatrixRank(cv::Mat& M);
  cv::Mat SimilarTransform(const cv::Mat& src, const cv::Mat& dst);
};

}  // namespace models

#endif