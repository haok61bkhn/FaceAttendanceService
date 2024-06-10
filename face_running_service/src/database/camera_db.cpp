#include "database/camera_db.h"
using namespace DB;

CameraDB::CameraDB(const std::string& db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db);
  CreateCameraTable();
  if (rc) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
  } else {
    std::cout << "Opened database successfully" << std::endl;
  }
}
CameraDB::~CameraDB() { sqlite3_close(db); }
void CameraDB::CreateCameraTable() {
  std::string sql =
      "CREATE TABLE IF NOT EXISTS camera ("
      "camera_id TEXT PRIMARY KEY NOT NULL,"
      "camera_name TEXT NOT NULL UNIQUE,"
      "camera_url TEXT NOT NULL,"
      "camera_index INTEGER NOT NULL,"
      "main_image_path TEXT NOT NULL,"
      "polygon TEXT NOT NULL"
      ")";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error CreateCameraTable CAMERA DB: " << zErrMsg
              << std::endl;
    sqlite3_free(zErrMsg);
  } else {
    std::cout << "Operation done successfully" << std::endl;
  }
}
bool CameraDB::AddCamera(const types::CameraInfor& camera) {
  std::string sql =
      "INSERT INTO camera (camera_id, camera_name, camera_url, "
      "camera_index, main_image_path, polygon) VALUES ('" +
      camera.camera_id + "','" + camera.camera_name + "','" +
      camera.camera_url + "'," + std::to_string(camera.camera_index) + ",'" +
      camera.main_image_path + "','" + camera.polygon_str + "')";

  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error AddCamera: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    std::cout << "Insert  Camera done successfully" << std::endl;
    return true;
  }
}
bool CameraDB::UpdateCamera(const types::CameraInfor& camera) {
  std::string sql = "UPDATE camera SET camera_name = '" + camera.camera_name +
                    "', camera_url = '" + camera.camera_url +
                    "', camera_index = " + std::to_string(camera.camera_index) +
                    ", main_image_path = '" + camera.main_image_path +
                    "', polygon = '" + camera.polygon_str + "'" +
                    " WHERE camera_id = '" + camera.camera_id + "'";

  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error UpdateCamera: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    return true;
  }
}
bool CameraDB::DeleteCamera(const std::string& camera_id) {
  std::string sql = "DELETE FROM camera WHERE camera_id = '" + camera_id + "'";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error DeleteCamera: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    std::cout << "Delete Camera done successfully" << std::endl;
    return true;
  }
}

bool CameraDB::GetCamera(const std::string& camera_id,
                         types::CameraInfor& camera) {
  std::string sql =
      "SELECT * FROM camera WHERE camera_id = '" + camera_id + "'";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error GetCamera: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    camera.camera_id = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    camera.camera_name = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    camera.camera_url = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
    camera.camera_index = sqlite3_column_int(stmt, 3);
    camera.main_image_path = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    camera.polygon_str = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
    sqlite3_finalize(stmt);
    return true;
  }
  sqlite3_finalize(stmt);
  return false;
}
bool CameraDB::GetAllCameras(std::vector<types::CameraInfor>& cameras) {
  std::string sql = "SELECT * FROM camera";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error GetAllCameras: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    std::string camera_id = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    std::string camera_name = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    std::string camera_url = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
    int camera_index = sqlite3_column_int(stmt, 3);
    types::CameraInfor camera(camera_id, camera_name, camera_url, camera_index);
    camera.main_image_path = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    camera.polygon_str = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
    cameras.push_back(camera);
  }
  if (rc != SQLITE_DONE) {
    std::cerr << "SQL error GetAllCameras: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    return false;
  }
  sqlite3_finalize(stmt);
  return true;
}

bool CameraDB::UpdateMainImage(const std::string& camera_id,
                               const std::string& main_image_path) {
  std::string sql = "UPDATE camera SET main_image_path = '" + main_image_path +
                    "' WHERE camera_id = '" + camera_id + "'";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error UpdateMainImage: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    return true;
  }
}