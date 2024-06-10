#ifndef PLAYBACK_DB_H
#define PLAYBACK_DB_H

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>

namespace DB {
class PlaybackDB {
 public:
  PlaybackDB(const std::string& db_path);
  ~PlaybackDB();
  void InitPlaybackDB();
  bool GetPlayback(const std::string& key, std::string& value);
  bool InsertPlayback(const std::string& key, const std::string& value);
  bool GetAllPlayback(std::vector<std::pair<std::string, std::string>>& values);

 private:
  void CreatePlaybackTable();

  sqlite3* db;
};
}  // namespace DB
#endif  // PLAYBACK_DB_H