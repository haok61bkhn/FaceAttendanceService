#ifndef CUSTOM_LOGGER_H
#define CUSTOM_LOGGER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include "AppConfig.h"
#include "Tools.h"

class CustomLogger {
 public:
  static CustomLogger* Instance();
  void Log(std::string& message, int level = 0);

  static void DeleteInstance();
  CustomLogger(const CustomLogger&) = delete;
  const CustomLogger& operator=(const CustomLogger&) = delete;
  CustomLogger(CustomLogger&& other) noexcept = delete;
  CustomLogger& operator=(CustomLogger&& other) noexcept = delete;

 private:
  static CustomLogger* m_instance;
  CustomLogger();
  ~CustomLogger();
  std::ofstream m_logFile;
};

#endif  // LOGGER_H