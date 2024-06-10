#ifndef FACE_PROCESSOR_H
#define FACE_PROCESSOR_H
#define MIN_FACE_WIDTH 50
#define MIN_FACE_HEIGHT 50

#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include "AppConfig.h"
#include "FeatureManager.h"
#include "LockFreeLifoQueue.h"
#include "LockFreeQueue.h"
#include "face_alignment.h"
#include "face_detection.h"
#include "face_extraction.h"
#include "types/FaceDetFullInfor.h"
#include "types/ObjectInforFull.h"
#include "types/ObjectUpdatingInfor.h"
#include "yaml-cpp/yaml.h"
using namespace models;

class FaceProcessor {
 public:
  FaceProcessor(LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue,
                LockFreeLifoQueue<types::ObjectInforFull>& face_queue,
                bool init_detector = false);
  ~FaceProcessor();

  void Run();
  void Stop();
  void Start();
  bool GetFeature(cv::Mat& image, std::vector<float>& feature,
                  cv::Mat& face_crop);

 private:
  void InitFaceDetection();
  void InitFaceAlignment();
  void InitFaceExtraction();
  void RunFaceDetect();
  void RunFaceExtract();

 public:
  LockFreeLifoQueue<types::ObjectInforFull>& face_queue;
  LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue;
  LockFreeQueue<types::FaceDetFullInfor> face_det_queue;

 private:
  bool stop_thread = false;
  std::thread thread_face_detector;
  std::thread thread_face_extractor;
  FaceAligner* face_aligner = nullptr;
  FaceExtraction* face_extractor = nullptr;
  FaceDetection* face_detector = nullptr;
};

#endif  // FACE_PROCESSOR_H