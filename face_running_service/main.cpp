#include <grpc++/grpc++.h>
#include <iostream>
#include <memory>
#include <string>
#include "AIController.h"
#include "AppConfig.h"
#include "CameraManager.h"
#include "DeepStreamManager.h"
#include "LockFreeLifoQueue.h"
#include "SaveImageThread.h"
#include "database/camera_db.h"
#include "database/face_attendance_db.h"
#include "deepstream_service.grpc.pb.h"
#include "face_manager.h"
#include "types/FeatureInfor.h"
#include "types/FrameSaving.h"
#include "types/ObjectUpdatingInfor.h"

#define CONFIG_FILE "resource/app_config/config.json"
AppConfig* config = AppConfig::Instance();

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using deepstream_service::CameraID;
using deepstream_service::CameraItem;
using deepstream_service::CameraStatus;
using deepstream_service::CameraStatusList;
using deepstream_service::DeepstreamService;
using deepstream_service::Empty;
using deepstream_service::FaceID;
using deepstream_service::FaceIDs;
using deepstream_service::FaceItem;
using deepstream_service::InsertStatusReponse;
using deepstream_service::MainImage;
using deepstream_service::ROI;
using deepstream_service::StatusResponse;

LockFreeLifoQueue<types::FrameSaving> image_save_queue;
LockFreeLifoQueue<types::FeatureInfor> feature_queue;
LockFreeLifoQueue<types::ObjectUpdatingInfor> updating_queue;
std::vector<types::CameraInfor> camera_list;
bool stop_save_image_thread = false;
std::vector<std::thread> image_saving_threads;

class DeepstreamServicempl final : public DeepstreamService::Service {
 public:
  DeepstreamServicempl(CameraManager* camera_manager,
                       AIController* ai_controller, FaceManager* face_manager)
      : camera_manager(camera_manager),
        ai_controller(ai_controller),
        face_manager(face_manager) {
    current_path = "./";
  }

  Status InsertCamera(ServerContext* context,
                      const deepstream_service::CameraItem* request,
                      StatusResponse* reply) override {
    std::string camera_id = request->camera_id();
    std::string camera_url = request->camera_url();
    std::string camera_name = request->camera_name();
    std::string image_path;
    int index = camera_manager->AddCamera(camera_id, camera_url, camera_name);
    if (index == -1) {
      reply->set_status(false);
      reply->set_message("Camera ID already exists");
      return Status::OK;
    } else {
      reply->set_status(true);
      reply->set_message("Camera added");
    }
    return Status::OK;
  }
  Status RemoveCamera(ServerContext* context, const CameraID* request,
                      StatusResponse* reply) override {
    std::string camera_id = request->camera_id();
    std::cout << "Removing camera: " << camera_id << "\n";
    bool status = camera_manager->RemoveCamera(camera_id);
    if (status) {
      reply->set_status(true);
      reply->set_message("Camera removed");
    } else {
      reply->set_status(false);
      reply->set_message("Camera not found");
    }
    return Status::OK;
  }
  Status RestartCamera(ServerContext* context, const CameraID* request,
                       StatusResponse* reply) override {
    std::string camera_id = request->camera_id();
    std::cout << "Restarting camera: " << camera_id << "\n";
    bool status = camera_manager->RestartCamera(camera_id);
    if (status) {
      reply->set_status(true);
      reply->set_message("Camera restarted");
    } else {
      reply->set_status(false);
      reply->set_message("Camera not found");
    }
    return Status::OK;
  }

  Status UpdateMainImage(ServerContext* context, const CameraID* request,
                         StatusResponse* reply) override {
    std::string camera_id = request->camera_id();
    if (camera_manager->camera_id_to_index.find(camera_id) ==
        camera_manager->camera_id_to_index.end()) {
      reply->set_status(false);
      reply->set_message("Camera ID not found");
    } else {
      int index = camera_manager->camera_id_to_index[camera_id];
      camera_manager->deepstream_manager->ResetMainImage(index);
      reply->set_status(true);
      reply->set_message("Main image updated");
    }
    return Status::OK;
  }

  Status UpdateROI(ServerContext* context, const ROI* request,
                   StatusResponse* reply) override {
    std::string camera_id = request->camera_id();
    std::vector<int> roi =
        std::vector<int>(request->polygon().begin(), request->polygon().end());
    std::cout << "Camera ID: " << camera_id << " ROI updated: ";
    for (int i = 0; i < roi.size(); i++) {
      std::cout << roi[i] << " ";
    }
    std::cout << "\n";
    reply->set_status("OK");
    reply->set_message("ROI updated");
    return Status::OK;
  };

  Status GetMainImage(ServerContext* context, const CameraID* request,
                      MainImage* reply) override {
    std::string camera_id = request->camera_id();
    if (camera_manager->camera_id_to_index.find(camera_id) ==
        camera_manager->camera_id_to_index.end()) {
      reply->set_image_path("");
    } else {
      int index = camera_manager->camera_id_to_index[camera_id];

      std::string image_path;
      camera_manager->deepstream_manager->GetMainImagePath(index, image_path);
      if (image_path != "") {
        image_path = current_path + "/" + image_path;
      }
      reply->set_image_path(image_path);
    }
    return Status::OK;
  }
  Status GetCameraStatus(ServerContext* context, const CameraID* request,
                         CameraStatus* reply) override {
    std::string camera_id = request->camera_id();
    if (camera_manager->camera_id_to_index.find(camera_id) ==
        camera_manager->camera_id_to_index.end()) {
      reply->set_active(false);
      reply->set_fps(0);
    } else {
      int index = camera_manager->camera_id_to_index[camera_id];
      auto& camera = camera_manager->camera_list[index];
      float fps, uptime;
      camera_manager->deepstream_manager->GetFPS(camera.camera_index, fps,
                                                 uptime);
      if (uptime < 5) {
        reply->set_active(true);
        reply->set_fps(fps);
      } else {
        reply->set_active(false);
        reply->set_fps(0);
      }
    }
    return Status::OK;
  }
  Status GetCameraStatusList(ServerContext* context, const Empty* request,
                             CameraStatusList* reply) override {
    std::cout << "Camera status list requested\n";
    for (int i = 0; i < camera_manager->camera_list.size(); i++) {
      auto& camera = camera_manager->camera_list[i];
      if (camera.camera_id != "") {
        CameraStatus* camera_status = reply->add_camera_status_list();
        float fps, uptime;
        camera_manager->deepstream_manager->GetFPS(camera.camera_index, fps,
                                                   uptime);
        if (uptime < 5) {
          camera_status->set_active(true);
          camera_status->set_fps(fps);
        } else {
          camera_status->set_active(false);
          camera_status->set_fps(0);
        }
        camera_status->set_camera_id(camera.camera_id);
        camera_status->set_camera_name(camera.camera_name);
      }
    }
    return Status::OK;
  }

  Status InsertFace(ServerContext* context, const FaceItem* request,
                    InsertStatusReponse* reply) override {
    std::string face_id = request->face_id();
    std::vector<std::string> face_image_paths;
    for (int i = 0; i < request->face_image_paths_size(); i++) {
      face_image_paths.push_back(request->face_image_paths(i));
    }
    std::cout << "Number of face image paths: " << face_image_paths.size()
              << "\n";
    std::vector<std::vector<float>> features;
    std::vector<cv::Mat> face_crops;
    if (ai_controller->GetFeatures(face_image_paths, features, face_crops)) {
      bool status = true;
      std::string match_pid;
      std::vector<std::vector<types::FaceSearchResult>> face_search_results =
          face_manager->SearchMutilple(features, 0.8);
      for (int i = 0; i < face_search_results.size(); i++) {
        for (auto& face_search_result : face_search_results[i]) {
          if (face_search_result.pid != face_id) {
            status = false;
            match_pid = face_search_result.pid;
            break;
          }
        }
      }
      if (!status) {
        reply->set_status(false);
        reply->set_message("Face ID already exists matched with " + match_pid);
        return Status::OK;
      }

      if (face_manager->InsertMultiple(features, face_id)) {
        std::cout << "Insert face success\n";
      } else {
        reply->set_status(false);
        reply->set_message("Insert face failed");
        return Status::OK;
      }

      reply->set_status(true);
      reply->set_message("Face added");

      for (auto& face_crop : face_crops) {
        std::string face_crop_path =
            config->face_feature_directory + RandomString(5) + ".jpg";
        cv::imwrite(face_crop_path, face_crop);
        reply->add_face_crop_image_paths(face_crop_path);
      }
    } else {
      reply->set_status(false);
      reply->set_message("Face feature extraction failed");
      return Status::OK;
    }
    return Status::OK;
  }
  Status RemoveFace(ServerContext* context, const FaceID* request,
                    StatusResponse* reply) override {
    std::string face_id = request->face_id();
    if (face_manager->DeleteByPId(face_id)) {
      reply->set_status(true);
      reply->set_message("Face removed");
    } else {
      reply->set_status(false);
      reply->set_message("Face not found");
    }
    return Status::OK;
  }
  Status GetFaceIDs(ServerContext* context, const Empty* request,
                    FaceIDs* reply) override {
    std::vector<std::string> face_ids = face_manager->GetAllPids();
    for (auto& face_id : face_ids) {
      reply->add_face_ids(face_id);
    }
    return Status::OK;
  }

 private:
  CameraManager* camera_manager;
  AIController* ai_controller;
  FaceManager* face_manager;
  std::string current_path;
};

void InitFolder() {
  CreateFolder(config->data_root_directory);
  CreateFolder(config->data_save_root_directory);
  CreateFolder(config->db_root_directory);
  CreateFolder(config->main_image_directory);
  CreateFolder(config->object_image_directory_root);
  CreateFolder(config->identification_image_directory);
  CreateFolder(config->annotation_root_directory);
  CreateFolder(config->annotation_image_directory);
  CreateFolder(config->annotation_text_directory);
  CreateFolder(config->face_feature_directory);
  CreateFolder(config->log_root_directory);
}

void InitImageSavingProcess() {
  for (int i = 0; i < config->num_image_saving_threads; ++i) {
    std::cout << "InitImageSavingProcess\n";
    std::thread save_image_thread(SaveImageFeature, std::ref(image_save_queue),
                                  std::ref(stop_save_image_thread));

    image_saving_threads.push_back(std::move(save_image_thread));
  }
}

void RunServer() {
  bool save_annotation = false;
  std::string face_manager_config_path =
      AppConfig::Instance()->ai_directory + "config/face_manager.yaml";
  FaceManager* face_manager =
      new FaceManager(face_manager_config_path, config->face_db_path);
  AIController* ai_controller = new AIController(face_manager, updating_queue,
                                                 camera_list, image_save_queue);
  ai_controller->Start();

  DB::CameraDB* camera_db = new DB::CameraDB(config->camera_db_path);
  DeepStreamManager* deepstream_manager =
      new DeepStreamManager(ai_controller, camera_db, save_annotation);

  CameraManager* camera_manager =
      new CameraManager(deepstream_manager, camera_db, camera_list);

  std::string server_address("0.0.0.0:50051");
  DeepstreamServicempl* service =
      new DeepstreamServicempl(camera_manager, ai_controller, face_manager);
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  std::filesystem::path cwd = std::filesystem::current_path();
  std::cout << "Current path is: " << cwd << std::endl;
  config->Initialize(CONFIG_FILE);
  config->current_path = cwd.string() + "/";
  InitFolder();
  InitImageSavingProcess();
  RunServer();
  return 0;
}