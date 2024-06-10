#ifndef SETTING_DB_H
#define SETTING_DB_H

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>

namespace DB {
class SettingDB {
 public:
  SettingDB(const std::string& db_path);
  ~SettingDB();
  void InitSetting();
  bool UpdateSetting(const std::string& key, const std::string& value);
  bool GetSetting(const std::string& key, std::string& value);
  bool InsertSetting(const std::string& key, const std::string& value);

 private:
  void CreateSettingTable();

  sqlite3* db;
};
}  // namespace DB
#endif  // SETTING_DB_H