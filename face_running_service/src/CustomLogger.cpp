#include "CustomLogger.h"

CustomLogger* CustomLogger::m_instance = nullptr;

CustomLogger::CustomLogger() {
  std::string log_path =
      AppConfig::Instance()->log_root_directory + GetTimeStampStr() + ".log";
  m_logFile.open(log_path, std::ios::app);
}

CustomLogger::~CustomLogger() { m_logFile.close(); }

CustomLogger* CustomLogger::Instance() {
  if (m_instance == NULL) {
    // std::lock_guard<std::mutex> lock(mtx);
    if (m_instance == NULL) m_instance = new CustomLogger();
  }
  return m_instance;
}

void CustomLogger::Log(std::string& message, int level) {
  CustomLogger* logger = CustomLogger::Instance();
  std::string log_level;
  switch (level) {
    case 0:
      log_level = "INFO";
      break;
    case 1:
      log_level = "WARNING";
      break;
    case 2:
      log_level = "ERROR";
      break;
    default:
      log_level = "INFO";
  }
  std::string log_message =
      "[" + GetDateTimeStr() + "] " + log_level + ": " + message;
  logger->m_logFile << log_message << std::endl;
}

void CustomLogger::DeleteInstance() {
  if (m_instance != NULL) {
    // std::lock_guard<std::mutex> lock(mtx);
    if (m_instance != NULL) {
      delete m_instance;
      m_instance = NULL;
    }
  }
}
