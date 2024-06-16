#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include <types/ObjectInforFull.h>
#include <atomic>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include "AppConfig.h"
#include "Face_Processor.h"
#include "LockFreeLifoQueue.h"
#include "LockFreeQueue.h"
#include "Tools.h"
#include "database/face_attendance_db.h"
#include "face_manager.h"
#include "types/CameraInfor.h"
#include "types/FrameInfor.h"
#include "types/FrameSaving.h"
#include "types/ObjectUpdatingInfor.h"

class AIController {
 public:
  AIController(FaceManager* face_manager,
               LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue,
               std::vector<types::CameraInfor>& camera_list,
               LockFreeLifoQueue<types::FrameSaving>& frame_saving_queue);
  ~AIController();

  void Stop();
  void Start();
  void PushData(std::vector<types::ObjectInforFull>& objects, int camera_index);
  void ProcessUpdatingDataThread();
  bool GetFeatures(std::vector<std::string>& image_paths,
                   std::vector<std::vector<float>>& features,
                   std::vector<cv::Mat>& face_crops);

 private:
  void InitFaceProcessor();

 public:
  LockFreeLifoQueue<types::ObjectInforFull> face_queue;

 private:
  int num_face_processors;
  std::vector<FaceProcessor*> face_processor;
  std::vector<types::CameraInfor>& camera_list;
  bool stop_thread = false;
  std::thread thread_updating;
  std::map<std::string, float> object_confidence_map;
  LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue;
  LockFreeLifoQueue<types::FrameSaving>& frame_saving_queue;
  std::map<std::string, int> face_trackers;
  std::map<std::string, int> ident_trackers;
  DB::FaceAttendanceDB* face_attendance_db;
  FaceManager* face_manager;
  std::string cur_path;
};

#endif  // AI_CONTROLLER_H