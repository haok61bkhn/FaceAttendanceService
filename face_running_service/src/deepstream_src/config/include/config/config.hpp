#pragma once
#include <iostream>
#include "nlohmann/json.hpp"

class Config {
 public:
  bool Initialize(std::string path);
  nlohmann::json GetAllConfig();

  static Config* Instance();
  static void DeleteInstance();
  Config(const Config&) = delete;
  const Config& operator=(const Config&) = delete;
  Config(Config&& other) noexcept = delete;
  Config& operator=(Config&& other) noexcept = delete;

 private:
  static Config* m_instance;
  Config();
  ~Config();
  nlohmann::json m_config;
};