#ifndef CREATE_SINK_HPP
#define CREATE_SINK_HPP
#include <glib.h>
#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <string>
#include <vector>
#include "config/config.hpp"
#include "deepstream_config.h"
#include "deepstream_sinks.h"

class Sink {
 public:
  static std::vector<GstElement*> Create(bool enable_fps_sink = true);
  static void CreateSingle(int index, std::string type,
                           NvDsSinkBin* nvds_sink_bin);
};
#endif  // CREATE_SINK_HPP