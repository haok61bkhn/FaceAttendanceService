#include "AppConfig.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

std::mutex mtx;
AppConfig* AppConfig::m_instance = nullptr;
AppConfig::AppConfig(){};
AppConfig::~AppConfig() {}

AppConfig* AppConfig::Instance() {
  if (m_instance == NULL) {
    std::lock_guard<std::mutex> lock(mtx);
    if (m_instance == NULL) m_instance = new AppConfig();
  }
  return m_instance;
}

bool AppConfig::Initialize(std::string path) {
  std::ifstream ifs{path};
  m_AppConfig = nlohmann::json::parse(ifs);
  data_root_directory = m_AppConfig["DATA_ROOT_DIRECTORY"].get<std::string>();
  related_data_directory =
      m_AppConfig["DATA_SAVE_ROOT_DIRECTORY"].get<std::string>();

  data_save_root_directory = data_root_directory + related_data_directory;

  db_root_directory = m_AppConfig["DB_ROOT_DIRECTORY"].get<std::string>();
  db_root_directory = data_save_root_directory + db_root_directory;

  main_image_directory = m_AppConfig["MAIN_IMAGE_DIRECTORY"].get<std::string>();
  main_image_directory = data_save_root_directory + main_image_directory;
  object_image_directory =
      m_AppConfig["OBJECT_IMAGE_DIRECTORY"].get<std::string>();
  related_image_directory = related_data_directory + object_image_directory;
  object_image_directory_root =
      data_save_root_directory + object_image_directory;
  identification_image_directory =
      m_AppConfig["IDENTIFICATION_IMAGE_DIRECTORY"].get<std::string>();
  identification_image_directory =
      data_save_root_directory + identification_image_directory;
  annotation_root_directory =
      m_AppConfig["ANNOTATION_ROOT_DIRECTORY"].get<std::string>();
  annotation_root_directory =
      data_save_root_directory + annotation_root_directory;
  annotation_image_directory =
      m_AppConfig["ANNOTATION_IMAGE_DIRECTORY"].get<std::string>();
  annotation_image_directory =
      annotation_root_directory + annotation_image_directory;
  annotation_text_directory =
      m_AppConfig["ANNOTATION_TEXT_DIRECTORY"].get<std::string>();
  annotation_text_directory =
      annotation_root_directory + annotation_text_directory;
  face_feature_directory =
      m_AppConfig["FACE_FEATURE_DIRECTORY"].get<std::string>();
  face_feature_directory = data_save_root_directory + face_feature_directory;
  log_root_directory = m_AppConfig["LOG_ROOT_DIRECTORY"].get<std::string>();
  log_root_directory = data_save_root_directory + log_root_directory;
  setting_db_path = m_AppConfig["SETTING_DB_PATH"].get<std::string>();
  setting_db_path = db_root_directory + setting_db_path;
  camera_db_path = m_AppConfig["CAMERA_DB_PATH"].get<std::string>();
  camera_db_path = db_root_directory + camera_db_path;
  lp_identification_db_path =
      m_AppConfig["LP_IDENTIFICATION_DB_PATH"].get<std::string>();
  lp_identification_db_path = db_root_directory + lp_identification_db_path;
  face_db_path = m_AppConfig["FACE_DB_PATH"].get<std::string>();
  face_db_path = db_root_directory + face_db_path;
  face_manager_db_path = m_AppConfig["FACE_MANAGER_DB_PATH"].get<std::string>();
  face_manager_db_path = db_root_directory + face_manager_db_path;
  playback_db_path = m_AppConfig["PLAYBACK_DB_PATH"].get<std::string>();
  playback_db_path = db_root_directory + playback_db_path;
  face_manager_config_path =
      m_AppConfig["FACE_MANAGER_CONFIG_PATH"].get<std::string>();
  deepstream_config_path =
      m_AppConfig["DEEPSTREAM_CONFIG_PATH"].get<std::string>();
  ai_directory = m_AppConfig["AI_DIRECTORY"].get<std::string>();

  mongo_uri = m_AppConfig["MONGO_URI"].get<std::string>();
  mongo_db = m_AppConfig["MONGO_DB"].get<std::string>();
  mongo_collection = m_AppConfig["MONGO_COLLECTION"].get<std::string>();

  num_image_saving_threads = m_AppConfig["NUM_IMAGE_SAVING_THREADS"];
  num_face_processors = m_AppConfig["NUM_FACE_PROCESSORS"];
  num_lp_processors = m_AppConfig["NUM_LP_PROCESSORS"];
  max_camera = m_AppConfig["MAX_CAMERA"];
  timeout_online = m_AppConfig["TIMEOUT_ONLINE"];
  skip_object = m_AppConfig["SKIP_OBJECT"];
  annotation_skip_frame = m_AppConfig["ANNOTATION_SKIP_FRAME"];
  feature_save_step_time = m_AppConfig["FEATURE_SAVE_STEP_TIME"];

  return true;
}

nlohmann::json AppConfig::GetAllAppConfig() { return m_AppConfig; }