#ifndef FEATURE_INFOR_H
#define FEATURE_INFOR_H
#include <iostream>
#include <string>
#include <vector>

namespace types {
class FeatureInfor {
 public:
  FeatureInfor() {}
  FeatureInfor(std::string& pid, std::vector<float>& feature, int time_stamp)
      : pid(pid), feature(feature), time_stamp(time_stamp) {}
  std::string pid;
  std::vector<float> feature;
  int time_stamp;
};
}  // namespace types
#endif  // FEATURE_INFOR_H