#include "CameraManager.h"

CameraManager::CameraManager(DeepStreamManager* deepstream_manager,
                             DB::CameraDB* camera_db,
                             std::vector<types::CameraInfor>& camera_list)
    : deepstream_manager(deepstream_manager),
      camera_db(camera_db),
      camera_list(camera_list) {
  Init();
}

void CameraManager::ConvertPolygonToString(std::vector<cv::Point>& polygon,
                                           std::string& polygon_str) {
  polygon_str = "";
  for (int i = 0; i < polygon.size(); ++i) {
    polygon_str +=
        std::to_string(polygon[i].x) + " " + std::to_string(polygon[i].y) + " ";
  }
  if (polygon.size() > 0) {
    polygon_str.pop_back();
  }
}

void CameraManager::ConvertStringToPolygon(std::string& polygon_str,
                                           std::vector<cv::Point>& polygon) {
  polygon.clear();
  std::istringstream iss(polygon_str);
  std::string token;
  while (std::getline(iss, token, ' ')) {
    cv::Point point;
    point.x = std::stoi(token);
    std::getline(iss, token, ' ');
    point.y = std::stoi(token);
    polygon.push_back(point);
  }
}

void CameraManager::Init() {
  for (int i = 0; i < 100; ++i) {
    types::CameraInfor camera;
    camera.status = "Offline";
    camera_list.push_back(camera);
  }
  std::vector<types::CameraInfor> cameras;
  camera_db->GetAllCameras(cameras);
  for (auto& camera : cameras) {
    if (camera.camera_index >= 0 && camera.camera_index < camera_list.size()) {
      ConvertStringToPolygon(camera.polygon_str, camera.polygon);
      camera.status = "Offline";
      camera_id_to_name[camera.camera_id] = camera.camera_name;
      camera_id_to_index[camera.camera_id] = camera.camera_index;
      camera_list[camera.camera_index] = camera;
      RunCamera(camera_list[camera.camera_index]);
      max_camera = std::max(max_camera, camera.camera_index + 1);
    }
  }
}

CameraManager::~CameraManager() {}

void CameraManager::FindMaxCamera() {
  max_camera = 0;
  for (int i = 0; i < AppConfig::Instance()->max_camera; ++i) {
    auto& camera = camera_list[i];
    if (camera.camera_url != "") {
      if (i >= max_camera) {
        max_camera = i + 1;
      }
    }
  }
  std::cout << "Max camera " << max_camera << std::endl;
}

void CameraManager::RunCamera(types::CameraInfor& camera) {
  int index = camera.camera_index;
  camera.status = "Online";
  deepstream_manager->AddSource(index, camera.camera_url, camera.camera_id,
                                camera.camera_name, camera.main_image_path,
                                camera.polygon);

  if (index >= max_camera) {
    max_camera = index + 1;
  }
}

int CameraManager::AddCamera(std::string& camera_id, std::string& camera_url,
                             std::string& camera_name) {
  for (int i = 0; i < camera_list.size(); ++i) {
    auto& camera = camera_list[i];
    if (camera.status == "Offline") {
      camera.camera_id = camera_id;
      camera.camera_url = camera_url;
      camera.camera_name = camera_name;
      camera.camera_index = i;
      bool status = camera_db->AddCamera(camera);
      if (!status) return -1;
      camera_id_to_name[camera.camera_id] = camera_name;
      camera_id_to_index[camera.camera_id] = i;
      RunCamera(camera);
      return i;
    }
  }
  return -1;
}

bool CameraManager::UpdatePolygon(std::string& camera_id,
                                  std::vector<cv::Point>& polygon) {
  try {
    if (camera_id_to_index.find(camera_id) == camera_id_to_index.end()) {
      return false;
    }
    int index = camera_id_to_index[camera_id];
    auto& camera = camera_list[index];
    ConvertPolygonToString(polygon, camera.polygon_str);
    camera.polygon = polygon;
    types::CameraInfor camera_infor(camera.camera_id, camera.camera_name,
                                    camera.camera_url, index);
    camera_infor.polygon_str = camera.polygon_str;
    camera_infor.main_image_path = camera.main_image_path;
    camera_db->UpdateCamera(camera_infor);
    return true;
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return false;
  }
}

bool CameraManager::RemoveCamera(std::string& camera_id) {
  try {
    if (camera_id_to_index.find(camera_id) == camera_id_to_index.end()) {
      return false;
    }
    int index = camera_id_to_index[camera_id];
    std::cout << "Remove camera " << index << std::endl;
    camera_id_to_name.erase(camera_list[index].camera_id);
    camera_id_to_index.erase(camera_list[index].camera_id);

    auto& camera = camera_list[index];
    if (camera.camera_name == "") {
      return false;
    }
    camera_db->DeleteCamera(camera.camera_id);

    camera.status = "Offline";
    camera.camera_id = "";
    camera.camera_name = "";
    camera.camera_url = "";
    FindMaxCamera();
    deepstream_manager->DeleteSource(index);
    return true;
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return false;
  }
}

void CameraManager::StopAllCameras() {
  for (int i = 0; i < camera_list.size(); ++i) {
    auto& camera = camera_list[i];
    if (camera.status == "Offline") {
      continue;
    }
  }
}

bool CameraManager::RestartCamera(std::string& camera_id) {
  try {
    if (camera_id_to_index.find(camera_id) == camera_id_to_index.end()) {
      return false;
    }

    int index = camera_id_to_index[camera_id];
    deepstream_manager->DeleteSource(index);
    auto& camera = camera_list[index];
    RunCamera(camera);
    return true;
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return false;
  }
}