#ifndef DEEPSTREAM_MANAGER_H
#define DEEPSTREAM_MANAGER_H

#define GPU_ID 0
#define SET_GPU_ID(object, gpu_id) \
  g_object_set(G_OBJECT(object), "gpu-id", gpu_id, NULL);
#define CONFIG_GPU_ID "gpu-id"

#define MUXER_OUTPUT_WIDTH 1920
#define MUXER_OUTPUT_HEIGHT 1080
#define MAX_NUM_SOURCES 100

#include <cuda_runtime_api.h>
#include <glib.h>
#include <gmodule.h>
#include <gst/gst.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "AIController.h"
#include "AppConfig.h"
#include "Tools.h"
#include "create_appsink.hpp"
#include "create_custom_probe.hpp"
#include "create_osd.hpp"
#include "create_primary_gie.hpp"
#include "create_sink.hpp"
#include "create_source.hpp"
#include "create_tiler.hpp"
#include "create_tracker.hpp"
#include "database/camera_db.h"
#include "gst-nvmessage.h"
#include "gstnvdsmeta.h"
#include "nvdsmeta.h"
#include "types.h"
#include "types/FrameSubmitInfor.h"
#include "utils/call_bus.hpp"
class DeepStreamManager {
 public:
  DeepStreamManager(AIController* ai_controller, DB::CameraDB* camera_db,
                    bool& save_annotation);
  ~DeepStreamManager();

 public:
  void Init();
  void StopThread();
  void Run();
  void RunThread();
  void GetFPS(int source_id, float& fps, float& uptime);
  void SetShowAll();
  void GetMainImagePath(int source_id, std::string& main_image_path);
  gboolean DeleteSource(int source_id);
  gboolean AddSource(int source_id, std::string& uri, std::string& camera_id,
                     std::string& camera_name, std::string& main_image_path,
                     std::vector<cv::Point>& polygon);
  void SetShowing(bool showing);
  void SetShowOne(int x, int y, int width, int height);
  void UpdatePolygon(int source_id, std::vector<cv::Point>& polygon);
  void ResetMainImage(int source_id);

 private:
  void InitPipeline();
  void StopReleaseSource(gint source_id);
  void TrackIdToObjectId(int& tracking_id, std::string& class_name,
                         int& source_id, std::string& camera_id,
                         std::string& object_id, int& time_stamp);
  void DrawObjects(cv::Mat& frame, std::vector<types::ObjectInfor>& objects);
  void MakeMaskPolygon(int source_id, int width, int height, cv::Mat& mask);
  int GetMinIndex();

 private:
  AppConfig* config = AppConfig::Instance();
  AIController* ai_controller;
  DB::CameraDB* camera_db;

  GMutex eos_lock;
  GstElement* streammux = NULL;
  GMainLoop* loop = NULL;
  GstBus* bus = NULL;
  gint g_num_sources = 0;
  bool showing = false;
  gint g_source_id_list[MAX_NUM_SOURCES];
  gboolean g_eos_list[MAX_NUM_SOURCES];
  gboolean g_source_enabled[MAX_NUM_SOURCES];
  std::string init_time_strs[MAX_NUM_SOURCES];
  GstElement** g_source_bin_list = NULL;

  guint bus_watch_id;
  guint i, num_sources;
  guint tiler_rows, tiler_columns;
  guint pgie_batch_size;

  GstElement *pipeline = NULL, *detection = NULL, *tracker = NULL,
             *appsink_queue = NULL, *appsink = NULL, *tiler = NULL,
             *sink_queue = NULL, *stream_demux = NULL, *nvvideoconvert = NULL,
             *osd = NULL, *sink = NULL;
  std::vector<GstElement*> sink_list;
  int max_index = 0;
  int row_tiler = 0;
  int col_tiler = 0;
  int show_mode = -1;

 private:
  void PreSubmitFrame(cv::Mat& image, int show_mode);
  void ProcessDataFrameThread();
  void PushImageThread();
  void SubmitObjectData(types::FrameInfor& data_frame, int source_id);
  bool& save_annotation;
  bool stop_thread = false;
  std::thread* process_data_frame_thread;
  std::thread* push_image_thread;
  LockFreeQueue<types::FrameSubmitInfor>* image_queues;
  CustomData* data;
  FrameData* frame_data;

 private:
  std::string camera_ids[MAX_NUM_SOURCES];
  std::string camera_names[MAX_NUM_SOURCES];
  std::string main_image_paths[MAX_NUM_SOURCES];
  cv::Mat polygon_masks[MAX_NUM_SOURCES];
  std::map<int, std::string> track_to_object[MAX_NUM_SOURCES];
  std::map<int, std::string> track_to_class[MAX_NUM_SOURCES];
  std::map<int, int> track_to_timestamp[MAX_NUM_SOURCES];

  std::map<int, int> object_trackers[MAX_NUM_SOURCES];
};

#endif  // DEEPSTREAM_MANAGER_H