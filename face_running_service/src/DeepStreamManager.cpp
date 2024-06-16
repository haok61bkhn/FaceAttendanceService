#include "DeepStreamManager.h"

DeepStreamManager::DeepStreamManager(AIController* ai_controller,
                                     DB::CameraDB* camera_db,
                                     bool& save_annotation)
    : ai_controller(ai_controller),
      save_annotation(save_annotation),
      camera_db(camera_db) {
  stop_thread = false;
  data = reinterpret_cast<CustomData*>(g_malloc0(sizeof(CustomData)));
  frame_data = reinterpret_cast<FrameData*>(g_malloc0(sizeof(FrameData)));
  frame_data->frame_queue = new LockFreeQueue<cv::Mat>();
  data->queue_datas = new LockFreeQueue<types::FrameInfor>();
  data->main_image_paths.resize(MAX_NUM_SOURCES);
  image_queues = new LockFreeQueue<types::FrameSubmitInfor>();
  process_data_frame_thread =
      new std::thread(&DeepStreamManager::ProcessDataFrameThread, this);
  process_data_frame_thread->detach();
  data->reconnect_frame_origin = cv::imread("resource/images/video_add.png");
  InitPipeline();
  RunThread();
}

DeepStreamManager::~DeepStreamManager() {}

void DeepStreamManager::Init() {
  show_mode = -1;
  g_object_set(G_OBJECT(tiler), "show-source", show_mode, NULL);
  showing = true;
}

void DeepStreamManager::SetShowAll() {
  if (show_mode != -1) {
    show_mode = -1;
    g_object_set(G_OBJECT(tiler), "show-source", show_mode, NULL);
  }
}

void DeepStreamManager::SetShowOne(int x, int y, int width, int height) {
  if (show_mode != -1) return;
  int index_x = x * data->col_tiler / width;
  int index_y = y * data->row_tiler / height;
  int index = index_y * data->col_tiler + index_x;
  if (show_mode != index) {
    show_mode = index;
    g_object_set(G_OBJECT(tiler), "show-source", show_mode, NULL);
  }
}

void DeepStreamManager::StopReleaseSource(gint source_id) {
  GstStateChangeReturn state_return;
  gchar pad_name[16];
  GstPad* sinkpad = NULL;
  std::cout << "STOP RELEASE SOURCE " << source_id << std::endl;
  if (data->map_source_showing[source_id] == -1) {
    return;
  }

  state_return =
      gst_element_set_state(g_source_bin_list[source_id], GST_STATE_NULL);
  switch (state_return) {
    case GST_STATE_CHANGE_SUCCESS:
      g_print("STATE CHANGE SUCCESS\n\n");
      g_snprintf(pad_name, 15, "sink_%u", data->map_source_showing[source_id]);
      sinkpad = gst_element_get_static_pad(streammux, pad_name);
      gst_pad_send_event(sinkpad, gst_event_new_eos());
      gst_pad_send_event(sinkpad, gst_event_new_flush_stop(FALSE));
      gst_element_release_request_pad(streammux, sinkpad);
      g_print("STATE CHANGE SUCCESS %p\n\n", sinkpad);
      gst_object_unref(sinkpad);
      gst_bin_remove(GST_BIN(pipeline), g_source_bin_list[source_id]);
      g_print("G NUM SOURCES %d\n", g_num_sources);
      g_num_sources--;
      g_print("G NUM SOURCES %d\n", g_num_sources);

      break;
    case GST_STATE_CHANGE_FAILURE:
      g_print("STATE CHANGE FAILURE\n\n");
      break;
    case GST_STATE_CHANGE_ASYNC:
      g_print("STATE CHANGE ASYNC\n\n");
      g_snprintf(pad_name, 15, "sink_%u", data->map_source_showing[source_id]);
      sinkpad = gst_element_get_static_pad(streammux, pad_name);
      gst_pad_send_event(sinkpad, gst_event_new_eos());
      gst_pad_send_event(sinkpad, gst_event_new_flush_stop(FALSE));
      gst_element_release_request_pad(streammux, sinkpad);
      g_print("STATE CHANGE ASYNC %p\n\n", sinkpad);
      gst_object_unref(sinkpad);
      gst_bin_remove(GST_BIN(pipeline), g_source_bin_list[source_id]);
      g_num_sources--;

      break;
    case GST_STATE_CHANGE_NO_PREROLL:
      g_print("STATE CHANGE NO PREROLL\n\n");
      break;
    default:
      break;
  }
}

gboolean DeepStreamManager::DeleteSource(int source_id) {
  std::cout << "DELETE SOURCE " << source_id << std::endl;
  g_mutex_lock(&eos_lock);
  if (g_eos_list[source_id] == TRUE &&
      g_source_enabled[data->map_source_showing[source_id]] == TRUE) {
    g_source_enabled[data->map_source_showing[source_id]] = FALSE;
    data->profilers[source_id].Reset();
    data->main_image_paths[source_id] = "";
    data->polygons[source_id].clear();
    StopReleaseSource(source_id);
    data->map_source_showing[source_id] = -1;
    data->last_runnings[source_id] = 0;
  }
  g_mutex_unlock(&eos_lock);
  return TRUE;
}

void DeepStreamManager::GetMainImagePath(int source_id,
                                         std::string& main_image_path) {
  main_image_path = main_image_paths[source_id];
}

int DeepStreamManager::GetMinIndex() {
  for (int i = 0; i < MAX_NUM_SOURCES; i++) {
    if (g_source_enabled[i] == FALSE) {
      return i;
    }
  }
  return -1;
}
gboolean DeepStreamManager::AddSource(int source_id, std::string& uri,
                                      std::string& camera_id,
                                      std::string& camera_name,
                                      std::string& main_image_path,
                                      std::vector<cv::Point>& polygon) {
  if (data->map_source_showing[source_id] != -1) {
    return FALSE;
  }

  GstElement* source_bin;
  GstStateChangeReturn state_return;
  int min_index = GetMinIndex();
  std::cout << "MIN INDEX " << min_index << std::endl;
  source_bin = Source::Create(min_index, uri.c_str(), streammux);
  if (!source_bin) {
    g_printerr("Failed to create source bin. Exiting.\n");
    return FALSE;
  }
  data->map_source_showing[source_id] = min_index;
  data->map_showing_source[min_index] = source_id;

  g_source_bin_list[source_id] = source_bin;
  gst_bin_add(GST_BIN(pipeline), source_bin);
  g_source_enabled[min_index] = TRUE;

  const auto p1 = std::chrono::system_clock::now();
  int now = std::chrono::duration_cast<std::chrono::milliseconds>(
                p1.time_since_epoch())
                .count();
  data->frame_numbers[source_id] = 0;
  data->last_times[source_id] = now;
  data->profilers[source_id].Reset();
  init_time_strs[source_id] = GetTimeStampStr();
  camera_ids[source_id] = camera_id;
  camera_names[source_id] = camera_name;
  data->main_image_paths[source_id] = main_image_path;
  object_trackers[source_id] = std::map<int, int>();
  track_to_object[source_id] = std::map<int, std::string>();
  track_to_class[source_id] = std::map<int, std::string>();
  data->polygons[source_id].clear();
  int now_s =
      std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
          .count();
  data->last_runnings[source_id] = now_s;
  for (auto& point : polygon) {
    data->polygons[source_id].push_back(point);
  }
  MakeMaskPolygon(source_id, MUXER_OUTPUT_WIDTH, MUXER_OUTPUT_HEIGHT,
                  polygon_masks[source_id]);
  state_return =
      gst_element_set_state(g_source_bin_list[source_id], GST_STATE_PLAYING);
  switch (state_return) {
    case GST_STATE_CHANGE_SUCCESS:
      g_print("STATE CHANGE SUCCESS\n\n");
      break;
    case GST_STATE_CHANGE_FAILURE:
      g_print("STATE CHANGE FAILURE\n\n");
      break;
    case GST_STATE_CHANGE_ASYNC:
      g_print("STATE CHANGE ASYNC\n\n");
      state_return = gst_element_get_state(g_source_bin_list[source_id], NULL,
                                           NULL, GST_CLOCK_TIME_NONE);
      break;
    case GST_STATE_CHANGE_NO_PREROLL:
      g_print("STATE CHANGE NO PREROLL\n\n");
      break;
    default:
      break;
  }

  g_num_sources++;
  return TRUE;
}

void DeepStreamManager::SetShowing(bool showing) { this->showing = showing; }

void DeepStreamManager::GetFPS(int source_id, float& fps, float& uptime) {
  fps = data->profilers[source_id].CurrentFPS();
  uptime = data->profilers[source_id].GetUptime();
}

void DeepStreamManager::InitPipeline() {
  Config::Instance()->Initialize(config->deepstream_config_path);
  auto all_configs = Config::Instance()->GetAllConfig();
  auto common_config = all_configs["common"];
  gst_debug_set_default_threshold(GST_LEVEL_NONE);
  int current_device = -1;
  cudaGetDevice(&current_device);
  struct cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, current_device);
  gboolean sync = TRUE;
  gboolean display = TRUE;
  gboolean enc_hw_support = TRUE;
  num_sources = 1;
  gst_init(NULL, NULL);
  loop = g_main_loop_new(NULL, FALSE);
  for (int i = 0; i < MAX_NUM_SOURCES; i++) {
    g_eos_list[i] = TRUE;
    data->profilers.push_back(Profiler(1));
  }

  g_source_bin_list =
      (GstElement**)g_malloc0(sizeof(GstElement*) * MAX_NUM_SOURCES);
  g_num_sources = 0;

  g_mutex_init(&eos_lock);
  pipeline = gst_pipeline_new("pipeline");
  streammux = gst_element_factory_make("nvstreammux", "stream-muxer");
  gint batched_push_timeout = all_configs["streamuxer"]["batched_push_timeout"];
  g_object_set(G_OBJECT(streammux), "batched-push-timeout",
               batched_push_timeout, NULL);
  g_object_set(G_OBJECT(streammux), "batch-size", 1, NULL);
  int cuda_memory_type = common_config["cudadec_mem_type"];
  g_object_set(G_OBJECT(streammux), "width", MUXER_OUTPUT_WIDTH, "height",
               MUXER_OUTPUT_HEIGHT, "gpu-id", GPU_ID, NULL);
  gst_bin_add(GST_BIN(pipeline), streammux);
  // g_object_set(G_OBJECT(streammux), "live-source", 0, "max-latency", 1,
  // NULL);
  g_object_set(G_OBJECT(streammux), "drop-pipeline-eos", TRUE, NULL);
  if (!pipeline || !streammux) {
    g_printerr("One element could not be created. Exiting.\n");
    return;
  }

  detection = PrimaryGie::Create("detection");
  tracker = Tracker::Create();
  // osd = gst_element_factory_make("nvdsosd", "nvdsosd");
  // g_object_set(G_OBJECT(osd), "process-mode", 1, "display-text", 0, NULL);
  // tiler = gst_element_factory_make("nvmultistreamtiler", "nvtiler");
  // g_object_set(G_OBJECT(tiler), "rows", 1, "columns", 1, "width",
  //  MUXER_OUTPUT_WIDTH, "height", MUXER_OUTPUT_HEIGHT, NULL);
  nvvideoconvert =
      gst_element_factory_make("nvvideoconvert", "nvvideo-converter");

  GstElement* caps_filter = gst_element_factory_make("capsfilter", NULL);
  GstCaps* caps =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGBA", NULL);

  GstCapsFeatures* feature = gst_caps_features_new(MEMORY_FEATURES, NULL);
  gst_caps_set_features(caps, 0, feature);
  g_object_set(G_OBJECT(caps_filter), "caps", caps, NULL);
  gst_caps_unref(caps);

  // sink_queue = gst_element_factory_make("queue", "sink-queue");

  // sink_list = Sink::Create(false);
  // sink = sink_list[0];

  // auto obj_ctx_handle = nvds_obj_enc_create_context(0);
  // data->obj_ctx_handle = obj_ctx_handle;
  data->enable_fps_sink = true;

  if (!detection || !nvvideoconvert || !caps_filter || !tracker) {
    g_printerr("One element could not be created. Exiting.\n");
    return;
  }

  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  BUS_CALL_DATA_TYPE* bus_call_data =
      new BUS_CALL_DATA_TYPE(loop, eos_lock, g_eos_list);
  bus_watch_id = gst_bus_add_watch(bus, bus_call, bus_call_data);
  gst_object_unref(bus);
  gst_bin_add_many(GST_BIN(pipeline), detection, nvvideoconvert, caps_filter,
                   tracker, NULL);

  if (!gst_element_link_many(streammux, detection, nvvideoconvert, caps_filter,
                             tracker, NULL)) {
    g_printerr("Elements could not be linked. Exiting.\n");
    return;
  }

  GstPad* src_pad = gst_element_get_static_pad(tracker, "src");
  gst_pad_add_probe(src_pad, GST_PAD_PROBE_TYPE_BUFFER, Probe::CustomProbe,
                    (gpointer)data, NULL);

  gst_element_set_state(pipeline, GST_STATE_PAUSED);
  if (gst_element_set_state(pipeline, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE) {
    g_printerr("Failed to set pipeline to playing. Exiting.\n");
    return;
  }
  for (int i = 0; i < MAX_NUM_SOURCES; i++) {
    g_source_enabled[i] = FALSE;
    data->map_source_showing[i] = -1;
    data->last_runnings[i] = 0;
  }
}

void DeepStreamManager::StopThread() {
  stop_thread = true;
  g_main_loop_quit(loop);
}

void DeepStreamManager::RunThread() {
  std::thread deepstream_thread(&DeepStreamManager::Run, this);
  deepstream_thread.detach();
}

void DeepStreamManager::Run() {
  g_print("Running...\n");
  g_main_loop_run(loop);
  g_print("Returned, stopping playback\n");
  gst_element_set_state(pipeline, GST_STATE_NULL);
  g_print("Deleting pipeline\n");
  gst_object_unref(GST_OBJECT(pipeline));
  g_source_remove(bus_watch_id);
  g_main_loop_unref(loop);
  g_free(g_source_bin_list);
  g_mutex_clear(&eos_lock);
}

void DeepStreamManager::DrawObjects(cv::Mat& frame,
                                    std::vector<types::ObjectInfor>& objects) {
  for (auto& object : objects) {
    cv::Scalar color;
    if (object.class_id == 0) {
      color = cv::Scalar(0, 255, 0);
    } else if (object.class_id == 1) {
      color = cv::Scalar(0, 0, 255);
    } else if (object.class_id == 2) {
      color = cv::Scalar(255, 0, 0);
    } else if (object.class_id == 3) {
      color = cv::Scalar(255, 255, 0);
    } else if (object.class_id == 5) {
      color = cv::Scalar(0, 255, 255);
    }
    cv::Rect rect = object.rect;
    cv::rectangle(frame, rect, color, 2);
    std::string label = std::to_string(object.tracking_id);
    int baseLine = 0;
    cv::Size labelSize =
        cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    cv::putText(frame, label, cv::Point(rect.x, rect.y),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
  }
}

void DeepStreamManager::ProcessDataFrameThread() {
  types::FrameInfor data_frame;
  while (!stop_thread) {
    if (data->queue_datas->pop(data_frame)) {
      int source_id = data_frame.source_id;
      SubmitObjectData(data_frame, data_frame.source_id);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void DeepStreamManager::ResetMainImage(int source_id) {
  main_image_paths[source_id] = "";
  camera_db->UpdateMainImage(camera_ids[source_id], "");
}
void DeepStreamManager::UpdatePolygon(int source_id,
                                      std::vector<cv::Point>& polygon) {
  data->polygons[source_id].clear();
  for (auto& point : polygon) {
    data->polygons[source_id].push_back(point);
  }
}

void DeepStreamManager::MakeMaskPolygon(int source_id, int width, int height,
                                        cv::Mat& mask) {
  auto polygon = data->polygons[source_id];
  if (polygon.size() == 0) {
    mask = cv::Mat::ones(height, width, CV_8UC1);
    return;
  }
  mask = cv::Mat::zeros(height, width, CV_8UC1);
  std::vector<std::vector<cv::Point>> contours = {polygon};
  cv::fillPoly(mask, contours, cv::Scalar(1));
}

void DeepStreamManager::TrackIdToObjectId(
    int& tracking_id, std::string& class_name, int& source_id,
    std::string& camera_id, std::string& object_id, int& time_stamp) {
  if (track_to_object[source_id].find(tracking_id) !=
      track_to_object[source_id].end())
    if (time_stamp - track_to_timestamp[source_id][tracking_id] < 60 &&
        track_to_class[source_id][tracking_id] == class_name) {
      object_id = track_to_object[source_id][tracking_id];
      return;
    }

  object_id = camera_id + "_" + class_name + "_" + std::to_string(time_stamp) +
              "_" + init_time_strs[source_id];
  track_to_object[source_id][tracking_id] = object_id;
  track_to_class[source_id][tracking_id] = class_name;
}

void DeepStreamManager::SubmitObjectData(types::FrameInfor& data_frame,
                                         int source_id) {
  int time_stamp = data_frame.time_stamp;
  cv::Mat frame = data_frame.frame;
  int number_processing_objects = 0;
  std::string& camera_id = camera_ids[source_id];
  std::vector<types::ObjectInforFull> objects_full;
  for (auto& object : data_frame.objects) {
    int x_center = object.rect.x + object.rect.width / 2;
    int y_center = object.rect.y + object.rect.height / 2;
    if (polygon_masks[source_id].at<uchar>(y_center, x_center) == 0) {
      continue;
    }
    if (object_trackers[source_id].find(object.tracking_id) !=
        object_trackers[source_id].end()) {
      if (data_frame.frame_number -
              object_trackers[source_id][object.tracking_id] <
          config->skip_object) {
        continue;
      }
    }
    object_trackers[source_id][object.tracking_id] = data_frame.frame_number;
    number_processing_objects++;
    types::ObjectInforFull object_full;
    object_full.object = object;
    for (auto& point : object.key_points) {
      types::Point p;
      p.x = point.x;
      p.y = point.y;
      p.prob = 1.0;
      object_full.landmark.push_back(p);
    }
    if (object_full.landmark.size() != 5) continue;
    object_full.time_stamp = time_stamp;
    object_full.camera_id = camera_id;
    object_full.camera_name = camera_names[source_id];
    TrackIdToObjectId(object.tracking_id, object.class_name, source_id,
                      camera_id, object_full.object_id, time_stamp);
    track_to_timestamp[source_id][object.tracking_id] = time_stamp;
    objects_full.push_back(object_full);
    number_processing_objects++;
  }
  if (number_processing_objects > 0) {
    ai_controller->PushData(objects_full, source_id);
  }
}
