#include "face_detection.h"

namespace models {
FaceDetection::FaceDetection(std::string& onnx_file, std::string& engine_file,
                             int batch_size, int input_width, int input_height,
                             int input_chanel, float obj_threshold,
                             float nms_threshold)
    : EngineModel(onnx_file, engine_file, batch_size),
      ImageModel(input_width, input_height, input_chanel) {
  obj_threshold_ = obj_threshold;
  nms_threshold_ = nms_threshold;

  std::cout << "FaceDetection init successfully!" << std::endl;
  LoadEngine();
  std::cout << "LoadEngine successfully!" << std::endl;

  for (auto& stride : list_stride_) {
    std::vector<std::vector<float>> anchor_centers =
        GenerateAnchorCenters(stride);
    all_anchor_centers_.emplace_back(anchor_centers);
  }
}

FaceDetection::~FaceDetection() {}
void FaceDetection::DrawFace(cv::Mat img, std::vector<FaceDetectRes>& boxes,
                             bool landmark) {
  for (int j = 0; j < boxes.size(); ++j) {
    cv::Rect rect(boxes[j].x1, boxes[j].y1, boxes[j].x2 - boxes[j].x1,
                  boxes[j].y2 - boxes[j].y1);
    cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
    char test[80];
    if (landmark) {
      sprintf(test, "%f", boxes[j].score);
      cv::putText(img, test, cv::Size((boxes[j].x1), boxes[j].y1),
                  cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0, 255, 0));
      cv::circle(img, cv::Point(boxes[j].landmark[0].x, boxes[j].landmark[0].y),
                 1, cv::Scalar(0, 0, 225), 1);
      cv::circle(img, cv::Point(boxes[j].landmark[1].x, boxes[j].landmark[1].y),
                 1, cv::Scalar(0, 255, 225), 1);
      cv::circle(img, cv::Point(boxes[j].landmark[2].x, boxes[j].landmark[2].y),
                 1, cv::Scalar(255, 0, 225), 1);
      cv::circle(img, cv::Point(boxes[j].landmark[3].x, boxes[j].landmark[3].y),
                 1, cv::Scalar(0, 255, 0), 1);
      cv::circle(img, cv::Point(boxes[j].landmark[4].x, boxes[j].landmark[4].y),
                 1, cv::Scalar(255, 0, 0), 1);
    }
  }
}

static bool CMP(FaceDetectRes& a, FaceDetectRes& b) {
  if (a.score > b.score) return true;
  return false;
}

void FaceDetection::NMS(std::vector<FaceDetectRes>& input_boxes,
                        float NMS_THRESH) {
  std::vector<float> varea(input_boxes.size());
  for (int i = 0; i < int(input_boxes.size()); ++i) {
    varea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1) *
               (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
  }
  for (int i = 0; i < int(input_boxes.size()); ++i) {
    for (int j = i + 1; j < int(input_boxes.size());) {
      float xx1 = std::max(input_boxes[i].x1, input_boxes[j].x1);
      float yy1 = std::max(input_boxes[i].y1, input_boxes[j].y1);
      float xx2 = std::min(input_boxes[i].x2, input_boxes[j].x2);
      float yy2 = std::min(input_boxes[i].y2, input_boxes[j].y2);
      float w = std::max(float(0), xx2 - xx1 + 1);
      float h = std::max(float(0), yy2 - yy1 + 1);
      float inter = w * h;
      float ovr = inter / (varea[i] + varea[j] - inter);
      if (ovr >= nms_threshold_) {
        input_boxes.erase(input_boxes.begin() + j);
        varea.erase(varea.begin() + j);
      } else {
        j++;
      }
    }
  }
}

void FaceDetection::FilterBoxes(std::vector<FaceDetectRes>& predicted_boxes) {
  std::sort(predicted_boxes.begin(), predicted_boxes.end(), CMP);
  NMS(predicted_boxes, 0.4);
}

bool FaceDetection::Preprocess(const cv::Mat& img, std::vector<float>& result,
                               float& ratio) {
  int input_height_ = InputHeight();
  int input_width_ = InputWidth();
  int input_chanel_ = InputChanel();
  result.clear();

  int long_side = std::max(img.cols, img.rows);
  int kmodel_inputsize = std::max(input_height_, input_width_);

  cv::Mat roi;
  if (long_side <= kmodel_inputsize) {
    ratio = 1.0;
    roi = img.clone();
  } else {
    ratio = std::min(float(input_width_) / float(img.cols),
                     float(input_height_) / float(img.rows));
    cv::resize(img, roi, cv::Size(), ratio, ratio, cv::INTER_LINEAR);
  }

  int pad_width_size = input_width_ - roi.cols;
  int pad_height_size = input_height_ - roi.rows;
  cv::copyMakeBorder(roi, roi, 0, pad_height_size, 0, pad_width_size,
                     cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
  roi.convertTo(roi, CV_32FC3, 1 / 128.f);
  result.resize(input_height_ * input_width_ * input_chanel_);
  float* data = result.data();
  int channelLength = input_height_ * input_width_;
  std::vector<cv::Mat> split_img = {
      cv::Mat(input_width_, input_height_, CV_32FC1, data + channelLength * 2),
      cv::Mat(input_width_, input_height_, CV_32FC1, data + channelLength),
      cv::Mat(input_width_, input_height_, CV_32FC1, data)};
  cv::split(roi, split_img);
  return true;
}

std::vector<std::vector<float>> FaceDetection::GenerateAnchorCenters(
    int stride) {
  int input_height_ = InputHeight();
  int input_width_ = InputWidth();
  int input_chanel_ = InputChanel();
  assert(input_width_ % 32 == 0 and input_height_ % 32 == 0);

  std::vector<std::vector<float>> anchors;
  int size_height = input_height_ / stride;
  int size_width = input_width_ / stride;
  float cy = 0;
  for (int i = 0; i < size_height; i++) {
    float cx = 0;
    for (int k = 0; k < size_width; k++) {
      std::vector<float> anchor{cx, cy};
      anchors.emplace_back(anchor);
      anchors.emplace_back(anchor);
      cx = cx + (float)stride;
    }
    cy = cy + (float)stride;
  }
  return anchors;
}

void FaceDetection::GenerateProposals(
    const std::vector<std::vector<float>>& anchors, int feat_stride,
    const std::vector<float>& score_blob, const std::vector<float>& bbox_blob,
    const std::vector<float>& kps_blob,
    std::vector<FaceDetectRes>& faceobjects) {
  for (int i = 0; i < score_blob.size(); i++) {
    float prob = score_blob[i];
    if (prob >= obj_threshold_) {
      float box0 = bbox_blob[i * 4 + 0];
      float box1 = bbox_blob[i * 4 + 1];
      float box2 = bbox_blob[i * 4 + 2];
      float box3 = bbox_blob[i * 4 + 3];

      float dx = box0 * feat_stride;
      float dy = box1 * feat_stride;
      float dw = box2 * feat_stride;
      float dh = box3 * feat_stride;

      float cx = anchors[i][0];
      float cy = anchors[i][1];

      float x1 = cx - dx;
      float y1 = cy - dy;
      float x2 = cx + dw;
      float y2 = cy + dh;

      FaceDetectRes box;
      box.x1 = x1;
      box.y1 = y1;
      box.x2 = x2;
      box.y2 = y2;
      box.score = prob;

      for (int k = 0; k < 5; k++) {
        box.landmark[k].x = cx + kps_blob[i * 10 + 2 * k] * feat_stride;
        box.landmark[k].y = cy + kps_blob[i * 10 + 2 * k + 1] * feat_stride;
      }
      faceobjects.push_back(box);
    }
  }
}

std::vector<FaceDetectRes> FaceDetection::Detect(const cv::Mat& img) {
  float scale;
  std::vector<FaceDetectRes> detections;
  detections.clear();
  std::vector<float> processed_image;
  if (!Preprocess(img, processed_image, scale)) return detections;
  // Inference
  EngineInference(processed_image);

  // Postprocess

  for (int i = 0; i < list_stride_.size(); i++) {
    std::vector<FaceDetectRes> face_objects;
    GenerateProposals(all_anchor_centers_[i], list_stride_[i],
                      output_infers_[i * 3], output_infers_[i * 3 + 1],
                      output_infers_[i * 3 + 2], face_objects);

    detections.insert(detections.end(), face_objects.begin(),
                      face_objects.end());
  }

  FilterBoxes(detections);

  for (auto& box : detections) {
    box.x1 = box.x1 / scale;
    box.y1 = box.y1 / scale;
    box.x2 = box.x2 / scale;
    box.y2 = box.y2 / scale;
    for (int k = 0; k < 5; k++) {
      box.landmark[k].x = box.landmark[k].x / scale;
      box.landmark[k].y = box.landmark[k].y / scale;
    }
  }
  return detections;
}

cv::Mat FaceDetection::Demo(cv::Mat& img) {
  cv::Mat draw_img = img.clone();
  std::vector<FaceDetectRes> detections = Detect(img);
  DrawFace(draw_img, detections, true);
  return draw_img;
}

}  // namespace models