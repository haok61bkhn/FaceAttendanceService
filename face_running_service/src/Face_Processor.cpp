#include "Face_Processor.h"

FaceProcessor::FaceProcessor(
    LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue,
    LockFreeLifoQueue<types::ObjectInforFull>& face_queue, bool init_detector)
    : updating_queue(updating_queue), face_queue(face_queue) {
  InitFaceAlignment();
  InitFaceExtraction();
  if (init_detector) {
    InitFaceDetection();
  }
}
FaceProcessor::~FaceProcessor() {
  delete face_aligner;
  delete face_extractor;
  if (face_detector != nullptr) {
    delete face_detector;
  }
}

void FaceProcessor::InitFaceAlignment() { face_aligner = new FaceAligner(); }

void FaceProcessor::InitFaceDetection() {
  std::string config_file =
      AppConfig::Instance()->ai_directory + "config/face_det.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["FACE_DET"];

  std::string onnx_file = AppConfig::Instance()->ai_directory +
                          config["onnx_file"].as<std::string>();
  std::string engine_file = AppConfig::Instance()->ai_directory +
                            config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  float obj_threshold = config["obj_threshold"].as<float>();
  float nms_threshold = config["nms_threshold"].as<float>();
  face_detector = new FaceDetection(onnx_file, engine_file, BATCH_SIZE,
                                    IMAGE_WIDTH, IMAGE_HEIGHT, INPUT_CHANNEL,
                                    obj_threshold, nms_threshold);
}

void FaceProcessor::InitFaceExtraction() {
  std::string config_file =
      AppConfig::Instance()->ai_directory + "config/face_extract.yaml";
  YAML::Node root = YAML::LoadFile(config_file);
  YAML::Node config = root["FACE_EXTRACT"];
  std::string onnx_file = AppConfig::Instance()->ai_directory +
                          config["onnx_file"].as<std::string>();
  std::string engine_file = AppConfig::Instance()->ai_directory +
                            config["engine_file"].as<std::string>();
  int BATCH_SIZE = config["BATCH_SIZE"].as<int>();
  int INPUT_CHANNEL = config["INPUT_CHANNEL"].as<int>();
  int IMAGE_WIDTH = config["IMAGE_WIDTH"].as<int>();
  int IMAGE_HEIGHT = config["IMAGE_HEIGHT"].as<int>();
  face_extractor = new FaceExtraction(onnx_file, engine_file, BATCH_SIZE,
                                      IMAGE_WIDTH, IMAGE_HEIGHT, INPUT_CHANNEL);
}
void FaceProcessor::Start() {
  thread_face_detector = std::thread(&FaceProcessor::RunFaceDetect, this);
  thread_face_detector.detach();
  thread_face_extractor = std::thread(&FaceProcessor::RunFaceExtract, this);
  thread_face_extractor.detach();
}

void FaceProcessor::Stop() { stop_thread = true; }

void FaceProcessor::RunFaceDetect() {
  int padding = 10;
  while (!stop_thread) {
    if (face_queue.size() > 500) {
      std::cout << "Face queue " << face_queue.size() << std::endl;
    }
    types::ObjectInforFull object;

    if (face_queue.pop(object)) {
      try {
        cv::Mat aligned_face = face_aligner->AlignFace(
            object.object.object_image, object.landmark);
        float area = (object.object.rect.width * object.object.rect.height);

        types::FaceDetFullInfor face_det_item(
            aligned_face, object.object.object_image,
            object.object.object_image_full, object.object_id,
            object.time_stamp, area, object.camera_id, object.camera_name);
        face_det_queue.push(face_det_item);
      }

      catch (std::exception& e) {
        std::cout << "Error in face detection: " << e.what() << std::endl;
      }
    } else
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void FaceProcessor::RunFaceExtract() {
  types::FaceDetFullInfor face_det_item;

  while (!stop_thread) {
    if (face_det_queue.size() > 500) {
      std::cout << "Face det queue " << face_det_queue.size() << std::endl;
    }
    if (face_det_queue.pop(face_det_item)) {
      try {
        types::ObjectUpdatingInfor object_updating(
            face_det_item.object_id, face_det_item.object_image,
            face_det_item.object_image_full, face_det_item.area,
            face_det_item.time_stamp, face_det_item.camera_id,
            face_det_item.camera_name);

        face_extractor->ExtractFace(face_det_item.aligned_face,
                                    object_updating.feature);
        updating_queue.push(object_updating);
      } catch (std::exception& e) {
        std::cout << "Error in face extraction: " << e.what() << std::endl;
      }
    } else
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

bool FaceProcessor::GetFeature(cv::Mat& image, std::vector<float>& feature,
                               cv::Mat& face_crop) {
  try {
    std::vector<FaceDetectRes> face_dets = face_detector->Detect(image);
    if (face_dets.size() != 1) {
      return false;
    }
    FaceDetectRes& face_det = face_dets[0];
    cv::Mat aligned_face = face_aligner->AlignFace(image, face_det.landmark);
    face_extractor->ExtractFace(aligned_face, feature);

    int padding = 20;
    int x1 = std::max(0, (int)face_det.x1 - padding);
    int y1 = std::max(0, (int)face_det.y1 - padding);
    int x2 = std::min(image.cols, (int)face_det.x2 + padding);
    int y2 = std::min(image.rows, (int)face_det.y2 + padding);
    cv::Rect rect(x1, y1, x2 - x1, y2 - y1);
    face_crop = image(rect);
    return true;
  } catch (std::exception& e) {
    std::cout << "Error in GetFeature: " << e.what() << std::endl;
    return false;
  }
}