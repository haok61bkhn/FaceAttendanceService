
#include "char_detection_yolov8.h"
namespace models {

CHAR_DETECTION_YOLOV8::CHAR_DETECTION_YOLOV8(std::string& onnx_file,
                                             std::string& engine_file,
                                             std::string& classes_file,
                                             int batch_size, int input_width,
                                             int input_height, int input_chanel)
    : EngineModel(onnx_file, engine_file, batch_size),
      ImageModel(input_width, input_height, input_chanel) {
  batch_size_ = batch_size;
  classes_ = ReadClasses(classes_file);
  LoadEngine();
}

CHAR_DETECTION_YOLOV8::~CHAR_DETECTION_YOLOV8() {}
std::vector<std::string> CHAR_DETECTION_YOLOV8::ReadClasses(
    const std::string& filename) {
  std::ifstream file(filename);     // Open the file for reading
  std::vector<std::string> result;  // Vector to store the file contents
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      result.push_back(line);  // Add each line to the vector
    }
    file.close();  // Close the file
  }

  return result;
}
float CHAR_DETECTION_YOLOV8::Clamp(float val, float min, float max) {
  return val > min ? (val < max ? val : max) : min;
}
bool CHAR_DETECTION_YOLOV8::Preprocess(const cv::Mat& img,
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

std::vector<Char_DET> CHAR_DETECTION_YOLOV8::Detect(const cv::Mat& img) {
  std::vector<Char_DET> detections;
  int height = img.rows;
  int width = img.cols;
  detections.clear();
  float ratio;
  int left, top;
  std::vector<float> input;
  bool status = Preprocess(img, input, ratio, left, top);
  std::vector<int> numdets;
  std::vector<float> boxes;
  std::vector<float> scores;
  std::vector<int> labels;
  EngineInference2(input, numdets, boxes, scores, labels);
  auto* num_dets = numdets.data();
  auto* boxes_ptr = boxes.data();
  auto* scores_ptr = scores.data();
  auto* labels_ptr = labels.data();
  for (int i = 0; i < num_dets[0]; i++) {
    float* ptr = boxes_ptr + i * 4;
    float x0 = *ptr++ - left;
    float y0 = *ptr++ - top;
    float x1 = *ptr++ - left;
    float y1 = *ptr - top;
    x0 = Clamp(x0 * ratio, 0.f, width);
    y0 = Clamp(y0 * ratio, 0.f, height);
    x1 = Clamp(x1 * ratio, 0.f, width);
    y1 = Clamp(y1 * ratio, 0.f, height);
    Char_DET obj;
    obj.rect.x = x0;
    obj.rect.y = y0;
    obj.rect.width = x1 - x0;
    obj.rect.height = y1 - y0;
    obj.prob = *(scores_ptr + i);
    obj.label = (int)*(labels_ptr + i);
    detections.push_back(obj);
  }
  return detections;
}

bool compareObject(Char_DET obj1, Char_DET obj2) {
  return (obj1.rect.x + obj1.rect.width / 2 <
          obj2.rect.x + obj2.rect.width / 2);
}

std::string CHAR_DETECTION_YOLOV8::GetTextLPN(std::vector<Char_DET>& detections,
                                              int image_height) {
  // Divide to 2 lines
  std::vector<Char_DET> objectList1;
  std::vector<Char_DET> objectList2;

  if (detections.size() == 0) return "";

  float averaged_height = 0.;
  for (Char_DET obj : detections) {
    averaged_height += obj.rect.height;
  }
  averaged_height /= detections.size();

  // sort by y
  sort(detections.begin(), detections.end(),
       [](const Char_DET& a, const Char_DET& b) {
         return (a.rect.y + a.rect.height / 2) < (b.rect.y + b.rect.height / 2);
       });
  int next_line = -1;
  for (int i = 0; i < detections.size() - 1; i++) {
    float center_y = detections[i].rect.y + detections[i].rect.height / 2;
    float next_center_y =
        detections[i + 1].rect.y + detections[i + 1].rect.height / 2;

    if (abs(next_center_y - center_y) < averaged_height * 0.6) {
      objectList1.push_back(detections[i]);
    } else {
      objectList1.push_back(detections[i]);
      for (int j = i + 1; j < detections.size(); j++) {
        objectList2.push_back(detections[j]);
      }
      break;
    }
  }
  if (objectList2.size() == 0) {
    objectList1.push_back(detections[detections.size() - 1]);
  }

  // Sort each line
  std::string licensePlate = "";
  if (objectList1.size() > 0) {
    sort(objectList1.begin(), objectList1.end(), compareObject);
    for (Char_DET obj : objectList1) {
      licensePlate += classes_[int(obj.label)];
    }
  }
  if (objectList2.size() > 0) {
    sort(objectList2.begin(), objectList2.end(), compareObject);
    for (Char_DET obj : objectList2) {
      licensePlate += classes_[int(obj.label)];
    }
  }

  return licensePlate;
}

cv::Mat CHAR_DETECTION_YOLOV8::DrawChar(cv::Mat& img,
                                        std::vector<Char_DET>& detections) {
  cv::Mat result = img.clone();
  for (auto& obj : detections) {
    float x0 = obj.rect.x;
    float y0 = obj.rect.y;
    float x1 = obj.rect.x + obj.rect.width;
    float y1 = obj.rect.y + obj.rect.height;
    // cv::rectangle(result, obj.rect, cv::Scalar(0, 255, 0), 2);
    cv::rectangle(result, cv::Point(x0, y0), cv::Point(x1, y1),
                  cv::Scalar(0, 255, 0), 2);
    std::string label = classes_[int(obj.label)];
    int baseLine = 0;
    cv::Size label_size =
        cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    cv::rectangle(
        result,
        cv::Rect(cv::Point(obj.rect.x, obj.rect.y - label_size.height),
                 cv::Size(label_size.width, label_size.height + baseLine)),
        cv::Scalar(0, 255, 0), cv::FILLED);
    cv::putText(result, label, cv::Point(obj.rect.x, obj.rect.y),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
  }
  return result;
}
}  // namespace models