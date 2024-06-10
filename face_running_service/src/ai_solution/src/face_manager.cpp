
#include "face_manager.h"

namespace models {
FaceManager::FaceManager(const std::string& save_path, int max_size, int topk,
                         int dim, float threshold) {
  save_path_ = save_path;
  max_size_ = max_size;
  topk_ = topk;
  dim_ = dim;
  threshold_ = threshold;
  space_ = std::make_shared<hnswlib::L2Space>(dim_);
  alg_hnsw_ = new hnswlib::HierarchicalNSW<float>(&*space_, max_size_);
  LoadData();
}
FaceManager::FaceManager(const std::string& config_path,
                         const std::string& save_path) {
  YAML::Node root = YAML::LoadFile(config_path);
  YAML::Node config = root["HNSW"];
  save_path_ = save_path;
  max_size_ = config["MAX_SIZE"].as<int>();
  topk_ = config["TOP_K"].as<int>();
  dim_ = config["DIM"].as<int>();
  threshold_ = config["THRESHOLD"].as<float>();
  space_ = std::make_shared<hnswlib::L2Space>(dim_);
  alg_hnsw_ = new hnswlib::HierarchicalNSW<float>(&*space_, max_size_);
  LoadData();
}

FaceManager::~FaceManager() = default;

bool CompareFaceinfor(const types::FaceSearchResult& f1,
                      const types::FaceSearchResult& f2) {
  return (f1.min_distance < f2.min_distance);
}

bool CheckExistsFile(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
}

bool FaceManager::SaveData() {
  alg_hnsw_->saveIndex(save_path_);
  return true;
}

bool FaceManager::LoadData() {
  if (!CheckExistsFile(save_path_)) SaveData();
  try {
    alg_hnsw_->loadIndex(save_path_, &*space_, max_size_);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::remove(save_path_.c_str());
    return false;
  }
  return true;
}

int FaceManager::GetNumDatas() { return alg_hnsw_->id_to_pid.size(); }

std::vector<std::string> FaceManager::GetAllPids() {
  std::vector<std::string> pids;
  std::unordered_map<std::string, bool> pid_map;
  for (auto& it : alg_hnsw_->id_to_pid) {
    if (pid_map.find(it.second) == pid_map.end()) {
      if (pid_map.find(it.second) == pid_map.end())
        pids.emplace_back(it.second);
      pid_map[it.second] = true;
    }
  }
  return pids;
}

bool FaceManager::Insert(const std::vector<float>& feature,
                         const std::string& pid) {
  int new_index = alg_hnsw_->cur_element_count;
  alg_hnsw_->addPoint(feature.data(), new_index);
  alg_hnsw_->id_to_pid[new_index] = pid;
  SaveData();
  return true;
}

bool FaceManager::InsertMultiple(
    const std::vector<std::vector<float>>& features, const std::string& pid) {
  int new_index = alg_hnsw_->cur_element_count;
  for (int i = 0; i < features.size(); i++) {
    alg_hnsw_->addPoint(features[i].data(), new_index + i);
    alg_hnsw_->id_to_pid[new_index + i] = pid;
  }
  SaveData();
  return true;
}

bool FaceManager::DeleteByPId(const std::string& pid) {
  int final_index = alg_hnsw_->cur_element_count;
  for (int i = 0; i < final_index; i++) {
    if (alg_hnsw_->id_to_pid[i] == pid) {
      alg_hnsw_->markDelete(i);
      alg_hnsw_->id_to_pid.erase(i);
    }
  }
  SaveData();
  return true;
}
void FaceManager::Dist2Percent(float& dist, float& percent) {
  if (dist <= 0.7) {
    percent = 1.0;
  } else if (dist <= 1.1) {
    percent = (1.5 - dist) / 0.8;
  } else if (dist > 1.3) {
    percent = 0.0;
  } else {
    percent = (1.3 - dist) / 0.4;
  }
}

bool FaceManager::DeleteById(const int id) {
  alg_hnsw_->markDelete(id);
  alg_hnsw_->id_to_pid.erase(id);
  return true;
}

bool FaceManager::DeleteByIds(const std::vector<int>& ids) {
  for (int i = 0; i < ids.size(); i++) {
    DeleteById(ids[i]);
  }
  return true;
}

std::vector<types::FaceSearchResult> FaceManager::Search(
    const std::vector<float>& feature, float min_score) {
  std::vector<types::FaceSearchResult> result;
  int index;
  int num_data = GetNumDatas();
  result.clear();
  if (num_data == 0)
    return result;
  else {
    const void* p = feature.data();
    int topk;
    if (topk_ >= num_data)
      topk = num_data;
    else
      topk = topk_;
    auto gd = alg_hnsw_->searchKnn(p, topk);
    while (!gd.empty()) {
      if (gd.top().first <= threshold_) {
        types::FaceSearchResult faceif;
        index = gd.top().second;
        faceif.min_distance = gd.top().first;
        Dist2Percent(faceif.min_distance, faceif.score);
        if (faceif.score < min_score && min_score != -1.0) {
          gd.pop();
          continue;
        }
        faceif.pid = alg_hnsw_->id_to_pid[index];
        result.emplace_back(faceif);
      }
      gd.pop();
    }
    if (topk_ > 1) std::sort(result.begin(), result.end(), CompareFaceinfor);
  }
  return result;
}

std::vector<std::vector<types::FaceSearchResult>> FaceManager::SearchMutilple(
    const std::vector<std::vector<float>>& features, float min_score) {
  std::vector<std::vector<types::FaceSearchResult>> results;
  int num_data = GetNumDatas();
  results.clear();
  if (num_data == 0) {
    results.resize(features.size());
    return results;
  } else {
    for (int i = 0; i < features.size(); i++) {
      std::vector<types::FaceSearchResult> result =
          Search(features[i], min_score);
      results.emplace_back(result);
    }
    return results;
  }
}

}  // namespace models
