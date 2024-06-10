#include "database/playback_db.h"
using namespace DB;
PlaybackDB::PlaybackDB(const std::string& db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db);
  InitPlaybackDB();
  if (rc) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
  } else {
    std::cout << "Opened database successfully" << std::endl;
  }
}

PlaybackDB::~PlaybackDB() { sqlite3_close(db); }

void PlaybackDB::InitPlaybackDB() { CreatePlaybackTable(); }

void PlaybackDB::CreatePlaybackTable() {
  std::string sql =
      "CREATE TABLE IF NOT EXISTS playback ("
      "key TEXT PRIMARY KEY NOT NULL,"
      "value TEXT NOT NULL"
      ")";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error CreatePlaybackTable: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
  } else {
    std::cout << "Operation done successfully" << std::endl;
  }
}

bool PlaybackDB::GetPlayback(const std::string& key, std::string& value) {
  std::string sql = "SELECT value FROM playback WHERE key = '" + key + "'";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error GetPlayback: " << sqlite3_errmsg(db) << std::endl;
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

bool PlaybackDB::InsertPlayback(const std::string& key,
                                const std::string& value) {
  std::string sql = "INSERT INTO playback (key, value) VALUES ('" + key +
                    "', '" + value + "')";
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error InsertPlayback: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    return false;
  } else {
    std::cout << "Operation done successfully" << std::endl;
    return true;
  }
}

bool PlaybackDB::GetAllPlayback(
    std::vector<std::pair<std::string, std::string>>& values) {
  std::string sql = "SELECT * FROM playback";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error GetAllPlayback: " << sqlite3_errmsg(db)
              << std::endl;
    return false;
  }
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string key = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    std::string value = std::string(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    values.push_back(std::make_pair(key, value));
  }
  sqlite3_finalize(stmt);
  return true;
}