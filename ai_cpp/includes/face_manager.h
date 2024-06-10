#ifndef FACE_MANAGER_H_
#define FACE_MANAGER_H_
#include "hnswlib.h"
#include <fstream>
#include <iostream>
#include <types.h>
#include <vector>

using idx_t = hnswlib::labeltype;
namespace models {
class FaceManager {
public:
  FaceManager(const std::string &save_path, int max_size, int topk, int dim,
              float threshold);
  FaceManager(const std::string &config_path);
  ~FaceManager();

  int GetNumDatas();
  bool Insert(const std::vector<float> &feature, const std::string &pid);
  bool InsertMultiple(const std::vector<std::vector<float>> &features,
                      const std::string &pid);

  std::vector<types::FaceSearchResult>
  Search(const std::vector<float> &feature);
  std::vector<std::vector<types::FaceSearchResult>>
  SearchMutilple(const std::vector<std::vector<float>> &features);

  bool DeleteByPId(const std::string &pid);
  bool DeleteByIds(const std::vector<int> &ids);
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
  hnswlib::HierarchicalNSW<float> *alg_hnsw_;
};
} // namespace models

#endif
