
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
FaceManager::FaceManager(const std::string& config_path) {
  // TODO
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

// Load and Save new data
bool FaceManager::SaveData() {
  alg_hnsw_->saveIndex(save_path_);
  return true;
}

bool FaceManager::LoadData() {
  if (!CheckExistsFile(save_path_)) return false;
  alg_hnsw_->loadIndex(save_path_, &*space_, max_size_);
  std::cout << "Load data success with size: " << GetNumDatas() << std::endl;
  return true;
}

int FaceManager::GetNumDatas() { return alg_hnsw_->id_to_pid.size(); }

bool FaceManager::Insert(const std::vector<float>& feature,
                         const std::string& pid) {
  int num_data = GetNumDatas();
  alg_hnsw_->addPoint(feature.data(), num_data);
  alg_hnsw_->id_to_pid[num_data] = pid;
  SaveData();
  return true;
}

bool FaceManager::InsertMultiple(
    const std::vector<std::vector<float>>& features, const std::string& pid) {
  int num_data = GetNumDatas();

  for (int i = 0; i < features.size(); i++) {
    alg_hnsw_->addPoint(features[i].data(), num_data + i);
    alg_hnsw_->id_to_pid[num_data + i] = pid;
  }
  SaveData();
  return true;
}

bool FaceManager::DeleteByPId(const std::string& pid) {
  int num_data = GetNumDatas();

  for (int i = 0; i < num_data; i++) {
    if (alg_hnsw_->id_to_pid[i] == pid) {
      alg_hnsw_->markDelete(i);
      alg_hnsw_->id_to_pid.erase(i);
    }
  }
  SaveData();
  return true;
}

bool FaceManager::DeleteById(const int id) {
  alg_hnsw_->markDelete(id);
  return true;
}

bool FaceManager::DeleteByIds(const std::vector<int>& ids) {
  for (int i = 0; i < ids.size(); i++) {
    DeleteById(ids[i]);
  }
  return true;
}

std::vector<types::FaceSearchResult> FaceManager::Search(
    const std::vector<float>& feature) {
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
    const std::vector<std::vector<float>>& features) {
  std::vector<std::vector<types::FaceSearchResult>> results;
  int num_data = GetNumDatas();
  results.clear();
  if (num_data == 0)
    return results;
  else {
    for (int i = 0; i < features.size(); i++) {
      std::vector<types::FaceSearchResult> result = Search(features[i]);
      results.emplace_back(result);
    }
    return results;
  }
}

}  // namespace models
