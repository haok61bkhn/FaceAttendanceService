#pragma once
#include <iostream>
#include "nlohmann/json.hpp"

class AppConfig {
 public:
  bool Initialize(std::string path);
  nlohmann::json GetAllAppConfig();

  static AppConfig* Instance();
  static void DeleteInstance();
  AppConfig(const AppConfig&) = delete;
  const AppConfig& operator=(const AppConfig&) = delete;
  AppConfig(AppConfig&& other) noexcept = delete;
  AppConfig& operator=(AppConfig&& other) noexcept = delete;

 public:
  std::string data_root_directory;
  std::string db_root_directory;
  std::string related_data_directory;
  std::string data_save_root_directory;
  std::string main_image_directory;
  std::string object_image_directory;
  std::string related_image_directory;
  std::string object_image_directory_root;
  std::string identification_image_directory;
  std::string annotation_root_directory;
  std::string annotation_image_directory;
  std::string annotation_text_directory;
  std::string face_feature_directory;
  std::string log_root_directory;
  std::string setting_db_path;
  std::string camera_db_path;
  std::string lp_identification_db_path;
  std::string face_db_path;
  std::string face_manager_db_path;
  std::string playback_db_path;
  std::string face_manager_config_path;
  std::string deepstream_config_path;
  std::string mongo_uri;
  std::string mongo_db;
  std::string mongo_collection;
  std::string ai_directory;

  int num_image_saving_threads;
  int num_face_processors;
  int num_lp_processors;
  int skip_object;
  int max_camera;
  int timeout_online;
  int feature_save_step;
  int feature_save_step_time;
  int annotation_skip_frame;

 private:
  static AppConfig* m_instance;
  AppConfig();
  ~AppConfig();
  nlohmann::json m_AppConfig;
};