#ifndef CameraManager_h
#define CameraManager_h

#include <iostream>
#include <thread>
#include "AIController.h"
#include "DeepStreamManager.h"
#include "LockFreeLifoQueue.h"
#include "Tools.h"
#include "database/camera_db.h"
#include "database/playback_db.h"
#include "types/CameraInfor.h"

class CameraManager {
 public:
  CameraManager(DeepStreamManager* deepstream_manager, DB::CameraDB* camera_db,
                std::vector<types::CameraInfor>& camera_list);
  ~CameraManager();

 public:
  void Init();
  std::map<std::string, std::string> camera_id_to_name;
  std::map<std::string, int> camera_id_to_index;
  int AddCamera(std::string& camera_id, std::string& camera_url,
                std::string& camera_name);
  bool RemoveCamera(std::string& camera_id);
  bool RestartCamera(std::string& camera_id);
  void FindMaxCamera();
  void RunCamera(types::CameraInfor& camera);
  void StopAllCameras();
  // bool UpdateMainImage(std::string& camera_id);
  bool UpdatePolygon(std::string& camera_id, std::vector<cv::Point>& polygon);
  void ConvertPolygonToString(std::vector<cv::Point>& polygon,
                              std::string& polygon_str);
  void ConvertStringToPolygon(std::string& polygon_str,
                              std::vector<cv::Point>& polygon);

 public:
  std::vector<types::CameraInfor>& camera_list;
  DB::CameraDB* camera_db;
  AIController* ai_controller;
  DeepStreamManager* deepstream_manager;

 private:
  int max_camera = 0;
};

#endif  // CameraManager_h