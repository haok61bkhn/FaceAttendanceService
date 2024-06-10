#include "lp_detection_yolov8.h"
namespace models {
LP_DETECTION_YOLOV8::LP_DETECTION_YOLOV8(std::string& onnx_file,
                                         std::string& engine_file,
                                         int batch_size, int input_width,
                                         int input_height, int input_chanel,
                                         int topk, float score_thres,
                                         float iou_thres)
    : EngineModel(onnx_file, engine_file, batch_size),
      ImageModel(input_width, input_height, input_chanel) {
  batch_size_ = batch_size;
  score_thres_ = score_thres;
  iou_thres_ = iou_thres;
  topk_ = topk;
  LoadEngine();
}
LP_DETECTION_YOLOV8::~LP_DETECTION_YOLOV8() {}

bool LP_DETECTION_YOLOV8::Preprocess(const cv::Mat& img,
                                     std::vector<float>& result, float& ratio,
                                     int& left, int& top) {
  int input_height_ = InputHeight();
  int input_width_ = InputWidth();
  int input_chanel_ = InputChanel();
  float height = img.rows;
  float width = img.cols;
  result.clear();

  float r = std::min(input_height_ / height, input_width_ / width);
  int padw = std::round(width * r);
  int padh = std::round(height * r);

  cv::Mat tmp;
  if ((int)width != padw || (int)height != padh) {
    cv::resize(img, tmp, cv::Size(padw, padh));
  } else {
    tmp = img.clone();
  }

  float dw = input_width_ - padw;
  float dh = input_height_ - padh;

  dw /= 2.0f;
  dh /= 2.0f;
  top = int(std::round(dh - 0.1f));
  int bottom = int(std::round(dh + 0.1f));
  left = int(std::round(dw - 0.1f));
  int right = int(std::round(dw + 0.1f));

  cv::copyMakeBorder(tmp, tmp, top, bottom, left, right, cv::BORDER_CONSTANT,
                     {114, 114, 114});
  cv::Mat out;
  cv::dnn::blobFromImage(tmp, out, 1 / 255.f, cv::Size(), cv::Scalar(0, 0, 0),
                         true, false, CV_32F);
  if (!out.empty() && out.isContinuous()) {
    result =
        std::vector<float>(out.ptr<float>(), out.ptr<float>() + out.total());
  }
  ratio = 1 / r;
  left = dw;
  top = dh;
  return true;
}

float LP_DETECTION_YOLOV8::Clamp(float val, float min, float max) {
  return val > min ? (val < max ? val : max) : min;
}

std::vector<LP_DET> LP_DETECTION_YOLOV8::Detect(const cv::Mat& img) {
  std::vector<LP_DET> detections;
  int height = img.rows;
  int width = img.cols;
  detections.clear();
  float ratio;
  int left, top;
  std::vector<float> input;
  bool status = Preprocess(img, input, ratio, left, top);
  EngineInference(input);
  cv::Mat output = cv::Mat(num_channels_, num_anchors_, CV_32F);
  std::memcpy(output.data, output_infers_[0].data(),
              output_infers_[0].size() * sizeof(float));
  std::vector<cv::Rect> bboxes;
  std::vector<float> scores;
  std::vector<int> labels;
  std::vector<int> indices;
  std::vector<std::vector<float>> kpss;
  output = output.t();
  for (int i = 0; i < num_anchors_; i++) {
    auto row_ptr = output.row(i).ptr<float>();

    auto bboxes_ptr = row_ptr;
    auto scores_ptr = row_ptr + 4;
    auto kps_ptr = row_ptr + 4 + classes_.size();
    float score_max = 0;
    int cl_id = 0;
    for (int k = 0; k < classes_.size(); k++) {
      if (*(scores_ptr + k) > score_max) {
        score_max = *(scores_ptr + k);
        cl_id = k;
      }
    }
    if (score_max > score_thres_) {
      float x = *bboxes_ptr++ - left;
      float y = *bboxes_ptr++ - top;
      float w = *bboxes_ptr++;
      float h = *bboxes_ptr;

      float x0 = Clamp((x - 0.5f * w) * ratio, 0.f, width);
      float y0 = Clamp((y - 0.5f * h) * ratio, 0.f, height);
      float x1 = Clamp((x + 0.5f * w) * ratio, 0.f, width);
      float y1 = Clamp((y + 0.5f * h) * ratio, 0.f, height);

      cv::Rect_<float> bbox;
      bbox.x = x0;
      bbox.y = y0;
      bbox.width = x1 - x0;
      bbox.height = y1 - y0;
      std::vector<float> kps;
      for (int k = 0; k < num_points_; k++) {
        float kps_x = (*(kps_ptr + 3 * k) - left) * ratio;
        float kps_y = (*(kps_ptr + 3 * k + 1) - top) * ratio;
        float kps_s = *(kps_ptr + 3 * k + 2);
        kps_x = Clamp(kps_x, 0.f, width);
        kps_y = Clamp(kps_y, 0.f, height);
        kps.push_back(kps_x);
        kps.push_back(kps_y);
        kps.push_back(kps_s);
      }

      bboxes.push_back(bbox);
      labels.push_back(cl_id);
      scores.push_back(score_max);
      kpss.push_back(kps);
    }
  }
  cv::dnn::NMSBoxes(bboxes, scores, score_thres_, iou_thres_, indices);
  int cnt = 0;
  for (auto& i : indices) {
    if (cnt >= topk_) {
      break;
    }
    LP_DET obj;
    obj.rect.x = bboxes[i].x;
    obj.rect.y = bboxes[i].y;
    obj.rect.width = bboxes[i].width;
    obj.rect.height = bboxes[i].height;
    obj.prob = scores[i];
    obj.label = classes_[labels[i]];
    obj.kps = kpss[i];
    detections.push_back(obj);
    cnt += 1;
  }
  return detections;
}

cv::Mat LP_DETECTION_YOLOV8::Align_LP(const cv::Mat& img,
                                      std::vector<float>& kps) {
  cv::Mat img_crop;
  std::vector<cv::Point> not_a_rect_shape;

  not_a_rect_shape.push_back(cv::Point(int(kps[0]), int(kps[1])));
  not_a_rect_shape.push_back(cv::Point(int(kps[4]), int(kps[5])));
  not_a_rect_shape.push_back(cv::Point(int(kps[6]), int(kps[7])));
  not_a_rect_shape.push_back(cv::Point(int(kps[2]), int(kps[3])));

  cv::RotatedRect box = cv::minAreaRect(cv::Mat(not_a_rect_shape));
  // int x1 = int(kps[0]);
  // int y1 = int(kps[1]);

  // int x2 = int(kps[6]);
  // int y2 = int(kps[7]);
  // double angle = std::atan2(y2 - y1, x2 - x1) * 180 / CV_PI;
  // std::cout << "angle: " << angle << std::endl;
  // draw line
  // cv::line(img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255),
  // 2);

  cv::Point2f pts[4];
  box.points(pts);
  cv::Point2f src_vertices[4];
  src_vertices[0] = not_a_rect_shape[0];  // topLeft
  src_vertices[1] = not_a_rect_shape[1];  // Top Right
  src_vertices[2] = not_a_rect_shape[2];  // bottomRight
  src_vertices[3] = not_a_rect_shape[3];  // bottomLeft
  cv::Point2f dst_vertices[4];
  dst_vertices[0] = cv::Point(0, 0);
  dst_vertices[1] = cv::Point(box.boundingRect().width, 0);
  dst_vertices[2] =
      cv::Point(box.boundingRect().width, box.boundingRect().height);
  dst_vertices[3] = cv::Point(0, box.boundingRect().height);
  cv::Mat warpMatrix = cv::getPerspectiveTransform(src_vertices, dst_vertices);
  cv::Size size(box.boundingRect().width, box.boundingRect().height);
  cv::warpPerspective(img, img_crop, warpMatrix, size, cv::INTER_LINEAR,
                      cv::BORDER_CONSTANT);
  return img_crop;
}

cv::Mat LP_DETECTION_YOLOV8::DrawLP(cv::Mat& draw_img,
                                    std::vector<LP_DET>& detections) {
  for (auto& obj : detections) {
    for (int i = 0; i < obj.kps.size(); i += 3) {
      int x = (int)obj.kps[i];
      int y = (int)obj.kps[i + 1];
      int s = (int)obj.kps[i + 2];
      cv::circle(draw_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
    }
  }
  return draw_img;
}

}  // namespace models