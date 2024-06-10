#ifndef CAMERA_DB_H
#define CAMERA_DB_H
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
#include "types/CameraInfor.h"

namespace DB {
class CameraDB {
 public:
  CameraDB(const std::string& db_path);
  ~CameraDB();
  bool AddCamera(const types::CameraInfor& camera);
  bool UpdateCamera(const types::CameraInfor& camera);
  bool DeleteCamera(const std::string& camera_id);
  bool GetCamera(const std::string& camera_id, types::CameraInfor& camera);
  bool GetAllCameras(std::vector<types::CameraInfor>& cameras);

 private:
  void CreateCameraTable();
  sqlite3* db;
};

}  // namespace DB
#endif  // CAMERA_DB_H