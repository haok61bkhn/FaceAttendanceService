#include "Tools.h"

std::string RandomString(int length) {
  const std::string characters =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, characters.size() - 1);

  std::string randomString;
  for (int i = 0; i < length; ++i) {
    randomString += characters[distribution(generator)];
  }

  return randomString;
}

std::string GetTimeStampStr() { return std::to_string(GetTimeStamp()); }
int GetTimeStamp() {
  auto currentTimePoint = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::seconds>(
             currentTimePoint.time_since_epoch())
      .count();
}

bool CreateFolder(std::string& folderPath) {
  struct stat info;
  if (stat(folderPath.c_str(), &info) != 0) {
    if (mkdir(folderPath.c_str(), 0777) == 0) {
      std::cout << "Folder '" << folderPath << "' created successfully."
                << std::endl;
      return true;
    } else {
      std::cerr << "Error creating folder '" << folderPath << "'." << std::endl;
      return false;
    }
  } else if (info.st_mode & S_IFDIR) {
    // Folder exists
    std::cout << "Folder '" << folderPath << "' already exists." << std::endl;
    return true;
  } else {
    std::cerr << "'" << folderPath << "' is not a directory." << std::endl;
    return false;
  }
}

std::string ConvertTimeStampToDateTime(int timestamp) {
  time_t time = timestamp;
  struct tm* timeinfo;
  timeinfo = localtime(&time);
  char buffer[80];
  strftime(buffer, 80, "%H:%M:%S %d-%m-%Y", timeinfo);
  return std::string(buffer);
}

std::string ConvertTimeStampToRTSPDateTime(int timestamp) {
  time_t time = timestamp;
  struct tm* timeinfo;
  timeinfo = localtime(&time);
  char buffer[80];
  strftime(buffer, 80, "%Y%m%dt%H%M%Sz", timeinfo);
  return std::string(buffer);
}

bool FileExists(const std::string& filename) {
  std::ifstream file(filename);
  return file.good();  // Check if file stream is in good state
}

std::string GetDateTimeStr() {
  auto currentTimePoint = std::chrono::system_clock::now();
  std::time_t currentTime =
      std::chrono::system_clock::to_time_t(currentTimePoint);
  std::string dateTimeStr = std::ctime(&currentTime);
  dateTimeStr.pop_back();  // Remove the newline character at the end
  return dateTimeStr;
}

int GetDateTimeInt() {
  std::time_t now = std::time(nullptr);
  std::tm* local_time = std::localtime(&now);
  int day = local_time->tm_mday;
  int month = local_time->tm_mon + 1;
  int year = local_time->tm_year + 1900;
  return year * 10000 + month * 100 + day;
}

void Dist2Percent(float& dist, float& percent) {
  if (dist <= 0.7) {
    percent = 1.0;
  } else if (dist <= 1.1) {
    percent = (1.5 - dist) / 0.8;
  } else if (dist > 1.3) {
    percent = 0.0;
  } else {
    percent = (1.3 - dist) / 0.4;
  }
}
