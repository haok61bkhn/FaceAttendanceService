#ifndef AI_CONTROLLER_INFOR_H
#define AI_CONTROLLER_INFOR_H
#include "types/ObjectInforFull.h"

namespace types {
class AIControllerInfor {
 public:
  AIControllerInfor(){};
  AIControllerInfor(std::vector<types::ObjectInforFull>& objects)
      : objects(objects) {}
  ~AIControllerInfor() = default;
  std::vector<types::ObjectInforFull> objects;
};
}  // namespace types
#endif  // AI_CONTROLLER_INFOR_H