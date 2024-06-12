#include "AIController.h"

AIController::AIController(
    FaceManager* face_manager,
    LockFreeLifoQueue<types::ObjectUpdatingInfor>& updating_queue,
    std::vector<types::CameraInfor>& camera_list,
    LockFreeLifoQueue<types::FrameSaving>& frame_saving_queue)
    : face_manager(face_manager),
      updating_queue(updating_queue),
      camera_list(camera_list),
      frame_saving_queue(frame_saving_queue) {
  auto config = AppConfig::Instance();
  num_face_processors = config->num_face_processors;
  face_processor.resize(num_face_processors);
  face_attendance_db = new DB::FaceAttendanceDB();

  // cur_path = currentPath.string();
  // std::cout << "Current path is: " << cur_path << std::endl;
  InitFaceProcessor();
}

AIController::~AIController() {
  for (int i = 0; i < num_face_processors; i++) {
    face_processor[i]->Stop();
    if (face_processor[i] != nullptr) {
      delete face_processor[i];
    }
  }
}

void AIController::InitFaceProcessor() {
  for (int i = 0; i < num_face_processors; i++) {
    if (i == 0) {
      face_processor[i] = new FaceProcessor(updating_queue, face_queue, true);
    } else {
      face_processor[i] = new FaceProcessor(updating_queue, face_queue, false);
    }
    face_processor[i]->Start();
  }
}

bool AIController::GetFeatures(std::vector<std::string>& image_paths,
                               std::vector<std::vector<float>>& features,
                               std::vector<cv::Mat>& face_crops) {
  features.clear();
  face_crops.clear();
  for (auto& image_path : image_paths) {
    cv::Mat image = cv::imread(image_path);

    if (image.empty()) {
      std::cout << "Cannot read image: " << image_path << std::endl;
      return false;
    }
    std::vector<float> feature;
    cv::Mat face_crop;
    if (face_processor[0]->GetFeature(image, feature, face_crop)) {
      features.push_back(feature);
      face_crops.push_back(face_crop);
    } else {
      std::cout << "Cannot get feature: " << image_path << std::endl;
      return false;
    }
  }

  return true;
}

void AIController::ProcessUpdatingDataThread() {
  types::ObjectUpdatingInfor object_updating;
  std::vector<std::string> ids;
  std::vector<float> scores;
  auto config = AppConfig::Instance();

  while (!stop_thread) {
    if (updating_queue.size() > 500) {
      std::cout << "updating_queue " << updating_queue.size() << std::endl;
    }
    if (updating_queue.pop(object_updating)) {
      auto& feature = object_updating.feature;
      std::vector<types::FaceSearchResult> face_results =
          face_manager->Search(feature, 0.6);
      if (face_results.size() > 0) {
        auto& id = face_results[0].pid;
        std::cout << "id: " << id << std::endl;
        auto& time_stamp = object_updating.time_stamp;
        if (ident_trackers.find(id) == ident_trackers.end()) {
          ident_trackers[id] = 0;
        }
        if (time_stamp - ident_trackers[id] > 5) {
          ident_trackers[id] = time_stamp;
          auto& object_image = object_updating.object_image;
          auto& object_image_full = object_updating.object_image_full;
          auto& camera_id = object_updating.camera_id;
          auto& object_id = object_updating.object_id;
          auto& camera_name = object_updating.camera_name;

          std::string object_image_save_path =
              config->current_path + config->object_image_directory_root +
              object_id + std::to_string(time_stamp) + ".jpg";

          std::string object_image_full_save_path =
              config->current_path + config->object_image_directory_root +
              object_id + std::to_string(time_stamp) + "_full.jpg";

          FrameSaving frame_saving_full(object_image_full,
                                        object_image_full_save_path);
          frame_saving_queue.push(frame_saving_full);

          FrameSaving frame_saving(object_image, object_image_save_path);
          frame_saving_queue.push(frame_saving);

          types::FaceAttendanceInfor face_attendance(
              id, object_image_save_path, object_image_full_save_path,
              camera_id, camera_name, time_stamp);
          face_attendance_db->InsertFaceAttendance(face_attendance);
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void AIController::Start() {
  thread_updating = std::thread(&AIController::ProcessUpdatingDataThread, this);
  thread_updating.detach();
}

void AIController::PushData(std::vector<types::ObjectInforFull>& objects,
                            int camera_index) {
  for (auto& object : objects) {
    std::cout << "PushData" << std::endl;
    if (object.object.rect.width < MIN_FACE_WIDTH ||
        object.object.rect.height < MIN_FACE_HEIGHT) {
      continue;
    }
    std::cout << "PushData1" << std::endl;
    auto& object_id = object.object_id;
    if (face_trackers.find(object_id) == face_trackers.end()) {
      face_trackers[object_id] = -1;
    }

    face_trackers[object_id]++;
    if (face_trackers[object_id] % 2 == 0) face_queue.push(object);
  }
}

void AIController::Stop() { stop_thread = true; }