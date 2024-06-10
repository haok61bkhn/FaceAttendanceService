#ifndef FEATURE_MANAGER_H
#define FEATURE_MANAGER_H

#include <iostream>
#include <mutex>
#include <thread>
#include "AppConfig.h"
#include "LockFreeLifoQueue.h"
#include "Tools.h"
#include "face_manager.h"
#include "types/FeatureInfor.h"

class FeatureManager {
 public:
  FeatureManager(std::string& save_dir_path,
                 LockFreeLifoQueue<types::FeatureInfor>& feature_queue);
  ~FeatureManager();

 private:
  int CurrentFeatureID();
  std::string GetFeatureSavePath(int feature_id);
  void GetFeatureIDs(int StartTimeStamp, int EndTimeStamp,
                     std::vector<int>& feature_ids);
  void GetFeaturePath(std::vector<int>& feature_ids,
                      std::vector<std::string>& feature_paths);
  void Run();

 public:
  void Stop();
  void Start();
  void SearchFeatures(
      int StartTimeStamp, int EndTimeStamp,
      std::vector<std::vector<float>>& features,
      std::vector<std::vector<types::FaceSearchResult>>& results,
      float min_score = -1.0);
  void AddFeature(std::vector<float>& feature, std::string& face_id,
                  int time_stamp);

 private:
  AppConfig* config = AppConfig::Instance();

  int current_feature_id;
  std::string save_dir_path;
  bool stop_thread = false;
  std::mutex feature_mutex;
  LockFreeLifoQueue<types::FeatureInfor>& feature_queue;
  std::shared_ptr<models::FaceManager> current_face_manager;
};

#endif  // FEATURE_MANAGER_H