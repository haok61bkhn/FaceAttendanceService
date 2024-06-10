#include "database/setting_db.h"
using namespace DB;
SettingDB::SettingDB(const std::string& db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db);
  InitSetting();

  if (rc) {
    std::cout << "Init Database Setting" << std::endl;
  } else {
    std::cout << "Opened database successfully" << std::endl;
  }
}
SettingDB::~SettingDB() { sqlite3_close(db); }
void SettingDB::InitSetting() { CreateSettingTable(); }

bool SettingDB::InsertSetting(const std::string& key,
                              const std::string& value) {
  std::string sql = "INSERT INTO setting (key, value) VALUES ('" + key +
                    "', '" + value + "')";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    // std::cerr << "SQL error: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    std::cout << "Operation done successfully" << std::endl;
    return true;
  }
}

bool SettingDB::UpdateSetting(const std::string& key,
                              const std::string& value) {
  std::string sql =
      "UPDATE setting SET value = '" + value + "' WHERE key = '" + key + "'";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error UpdateSetting: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    std::cout << "Operation done successfully" << std::endl;
    return true;
  }
}
bool SettingDB::GetSetting(const std::string& key, std::string& value) {
  std::string sql = "SELECT value FROM setting WHERE key = '" + key + "'";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error GetSetting: " << sqlite3_errmsg(db) << std::endl;
    return false;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    value = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    sqlite3_finalize(stmt);
    return true;
  }
  sqlite3_finalize(stmt);
  return false;
}
void SettingDB::CreateSettingTable() {
  std::string sql =
      "CREATE TABLE IF NOT EXISTS setting (key TEXT PRIMARY KEY, "
      "value TEXT NOT NULL)";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error CreateSettingTable: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);

  } else {
    InsertSetting("grid_size", "1");
    InsertSetting("num_item_row_show", "3");
    InsertSetting("num_item_col_show", "3");
    std::cout << "Table created successfully" << std::endl;
  }
}
