#include "database/face_attendance_db.h"
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using namespace DB;

FaceAttendanceDB::FaceAttendanceDB() { ConnectDB(); }
FaceAttendanceDB::~FaceAttendanceDB() {}

void FaceAttendanceDB::ConnectDB() {
  mongocxx::instance inst{};
  mongocxx::uri uri(AppConfig::Instance()->mongo_uri);
  client = mongocxx::client(uri);
  collection = client[AppConfig::Instance()->mongo_db]
                     [AppConfig::Instance()->mongo_collection];
  CreateIndex();
}

void FaceAttendanceDB::CreateIndex() {
  bsoncxx::builder::stream::document index_builder{};
  index_builder << "time_stamp" << 1;
  collection.create_index(index_builder.view());
}

void FaceAttendanceDB::InsertFaceAttendance(
    types::FaceAttendanceInfor& face_attendance) {
  bsoncxx::builder::stream::document document{};
  document << "face_id" << face_attendance.face_id << "object_image_path"
           << face_attendance.object_image_path << "object_image_full_path"
           << face_attendance.object_image_full_path << "camera_id"
           << face_attendance.camera_id << "camera_name"
           << face_attendance.camera_name << "time_stamp"
           << face_attendance.time_stamp << "score" << face_attendance.score;
  collection.insert_one(document.view());
  std::cout << "InsertFaceAttendance success" << std::endl;
}