#include "config/config.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

// std::mutex mtx;
Config* Config::m_instance = nullptr;
Config::Config(){};
Config::~Config() {}

Config* Config::Instance() {
  if (m_instance == NULL) {
    // std::lock_guard<std::mutex> lock(mtx);
    if (m_instance == NULL) m_instance = new Config();
  }
  return m_instance;
}

bool Config::Initialize(std::string path) {
  std::ifstream ifs{path};
  m_config = nlohmann::json::parse(ifs);
  return true;
}

nlohmann::json Config::GetAllConfig() { return m_config; }