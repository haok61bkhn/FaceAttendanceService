#pragma once

#define SKIP_FRAME 3
#define MAX_NUM_SOURCES 100

#include <glib.h>
#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <opencv2/imgproc/types_c.h>
#include <chrono>
#include <deque>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include "LockFreeQueue.h"
#include "config/config.hpp"
#include "create_custom_probe.hpp"
#include "deepstream_config.h"
#include "deepstream_sinks.h"
#include "gstnvdsmeta.h"
#include "nvbufsurface.h"
#include "nvds_obj_encode.h"
#include "profiler.h"
#include "types/FrameInfor.h"
#include "types/ObjectInfor.h"
#include "types/ObjectInforFull.h"

struct CustomData {
  NvDsObjEncCtxHandle obj_ctx_handle;
  bool enable_fps_sink;
  bool stop_thread = false;
  int frame_numbers[MAX_NUM_SOURCES];
  int last_times[MAX_NUM_SOURCES];
  int max_index = 0;
  cv::Mat reconnect_frame_origin;
  cv::Mat sub_reconnect_frame;
  int col_tiler;
  int row_tiler;
  std::vector<cv::Point> polygons[MAX_NUM_SOURCES];
  std::vector<std::string> main_image_paths;
  std::vector<Profiler> profilers;
  LockFreeQueue<types::FrameInfor>* queue_datas = nullptr;
  int map_source_showing[MAX_NUM_SOURCES];
  int map_showing_source[MAX_NUM_SOURCES];
  int last_runnings[MAX_NUM_SOURCES];
};

struct FrameData {
  LockFreeQueue<cv::Mat>* frame_queue;
};
class AppSink {
 public:
  static GstElement* CreateAppSink(gpointer data, std::string& appsink_name);
  static GstFlowReturn NewSample(GstElement* sink, gpointer data);
};