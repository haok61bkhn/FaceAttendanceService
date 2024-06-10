#ifndef FACE_MANAGER_H_
#define FACE_MANAGER_H_
#include <types.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "hnswlib.h"
#include "yaml-cpp/yaml.h"

using idx_t = hnswlib::labeltype;
namespace models {
class FaceManager {
 public:
  FaceManager(const std::string& save_path, int max_size, int topk, int dim,
              float threshold);
  FaceManager(const std::string& config_path, const std::string& save_path);
  ~FaceManager();

  int GetNumDatas();
  bool Insert(const std::vector<float>& feature, const std::string& pid);
  bool InsertMultiple(const std::vector<std::vector<float>>& features,
                      const std::string& pid);
  std::vector<std::string> GetAllPids();

  std::vector<types::FaceSearchResult> Search(const std::vector<float>& feature,
                                              float min_score = -1.0);
  std::vector<std::vector<types::FaceSearchResult>> SearchMutilple(
      const std::vector<std::vector<float>>& features, float min_score = -1.0);
  void Dist2Percent(float& dist, float& percent);
  bool DeleteByPId(const std::string& pid);
  bool DeleteByIds(const std::vector<int>& ids);
  bool DeleteById(const int id);
  float threshold_;
  bool LoadData();

 private:
  bool SaveData();

 private:
  int max_size_;
  int dim_;
  int topk_;
  std::string save_path_;
  std::shared_ptr<hnswlib::L2Space> space_;
  hnswlib::HierarchicalNSW<float>* alg_hnsw_;
};
}  // namespace models

#endif
