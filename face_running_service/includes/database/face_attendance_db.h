#ifndef FACE_ATTENDANCE_DB_H
#define FACE_ATTENDANCE_DB_H

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <string>
#include "AppConfig.h"
#include "CustomLogger.h"
#include "types/FaceAttendanceInfor.h"

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

namespace DB {
class FaceAttendanceDB {
 public:
  FaceAttendanceDB();
  ~FaceAttendanceDB();
  void ConnectDB();
  void CreateIndex();
  void InsertFaceAttendance(types::FaceAttendanceInfor& face_attendance);

 private:
  mongocxx::client client;
  mongocxx::collection collection;
};
}  // namespace DB
#endif  // FACE_ATTENDANCE_DB_H