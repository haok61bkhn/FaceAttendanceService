#include "FeatureManager.h"

FeatureManager::FeatureManager(
    std::string& save_dir_path,
    LockFreeLifoQueue<types::FeatureInfor>& feature_queue)
    : save_dir_path(save_dir_path), feature_queue(feature_queue) {
  current_feature_id = CurrentFeatureID();
  std::cout << "load feature id: " << current_feature_id << std::endl;
  std::string current_feature_path = GetFeatureSavePath(current_feature_id);
  current_face_manager = std::make_shared<models::FaceManager>(
      config->face_manager_config_path, current_feature_path);
  std::cout << "load feature path: " << current_feature_path << std::endl;
}

FeatureManager::~FeatureManager() {}

int FeatureManager::CurrentFeatureID() {
  return GetTimeStamp() / config->feature_save_step_time;
}

std::string FeatureManager::GetFeatureSavePath(int feature_id) {
  return save_dir_path + std::to_string(feature_id) + ".feature";
}

void FeatureManager::AddFeature(std::vector<float>& feature,
                                std::string& face_id, int time_stamp) {
  types::FeatureInfor feature_infor(face_id, feature, time_stamp);
  feature_queue.push(feature_infor);
}

void FeatureManager::Run() {
  while (!stop_thread) {
    types::FeatureInfor feature_infor;
    if (feature_queue.pop(feature_infor)) {
      std::unique_lock<std::mutex> lock(feature_mutex);
      if (feature_infor.time_stamp / config->feature_save_step_time >
          current_feature_id) {
        current_feature_id = CurrentFeatureID();
        std::string current_feature_path =
            GetFeatureSavePath(current_feature_id);
        current_face_manager = std::make_shared<models::FaceManager>(
            config->face_manager_config_path, current_feature_path);
      }
      current_face_manager->Insert(feature_infor.feature, feature_infor.pid);
      lock.unlock();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void FeatureManager::GetFeatureIDs(int StartTimeStamp, int EndTimeStamp,
                                   std::vector<int>& feature_ids) {
  int start_id = StartTimeStamp / config->feature_save_step_time - 1;
  int end_id = EndTimeStamp / config->feature_save_step_time + 1;
  for (int i = start_id; i <= end_id; i++) {
    feature_ids.push_back(i);
  }
}

void FeatureManager::GetFeaturePath(std::vector<int>& feature_ids,
                                    std::vector<std::string>& feature_paths) {
  for (auto& feature_id : feature_ids) {
    feature_paths.push_back(GetFeatureSavePath(feature_id));
  }
}

void FeatureManager::SearchFeatures(
    int StartTimeStamp, int EndTimeStamp,
    std::vector<std::vector<float>>& features,
    std::vector<std::vector<types::FaceSearchResult>>& results,
    float min_score) {
  results.resize(features.size());
  std::vector<int> feature_ids;
  GetFeatureIDs(StartTimeStamp, EndTimeStamp, feature_ids);
  std::vector<std::string> feature_paths;
  GetFeaturePath(feature_ids, feature_paths);
  for (int i = 0; i < feature_paths.size(); i++) {
    std::string feature_path = feature_paths[i];
    if (feature_ids[i] == current_feature_id) {
      std::unique_lock<std::mutex> lock(feature_mutex);
      std::vector<std::vector<types::FaceSearchResult>> result =
          current_face_manager->SearchMutilple(features, min_score);
      for (int i = 0; i < features.size(); i++) {
        results[i].insert(results[i].end(), result[i].begin(), result[i].end());
      }
      lock.unlock();
      continue;
    }
    if (!FileExists(feature_path)) {
      continue;
    }
    models::FaceManager face_manager(config->face_manager_config_path,
                                     feature_path);
    std::vector<std::vector<types::FaceSearchResult>> result =
        face_manager.SearchMutilple(features, min_score);
    for (int i = 0; i < features.size(); i++) {
      results[i].insert(results[i].end(), result[i].begin(), result[i].end());
    }
  }
}

void FeatureManager::Stop() { stop_thread = true; }

void FeatureManager::Start() {
  stop_thread = false;
  std::thread feature_thread(&FeatureManager::Run, this);
  feature_thread.detach();
}