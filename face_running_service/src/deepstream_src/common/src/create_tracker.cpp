#include "create_tracker.hpp"
#include "config/config.hpp"
#include "deepstream_tracker.h"

GstElement* Tracker::Create() {
  NvDsTrackerBin* nvds_tracker_bin = new NvDsTrackerBin();
  NvDsTrackerConfig* nvds_tracker_config = new NvDsTrackerConfig();
  auto all_configs = Config::Instance()->GetAllConfig();
  auto tracking_config = all_configs["tracking"];
  nvds_tracker_config->display_tracking_id =
      tracking_config["display_tracking_id"] == 1 ? true : false;
  nvds_tracker_config->width = tracking_config["tracker_width"];
  nvds_tracker_config->height = tracking_config["tracker_height"];
  nvds_tracker_config->gpu_id = tracking_config["gpu_id"];
  std::string ll_config_file =
      tracking_config["ll_config_file"].get<std::string>();
  if (!ll_config_file.empty()) {
    nvds_tracker_config->ll_config_file = strdup(
        ((std::string)tracking_config["ll_config_file"].get<std::string>())
            .c_str());
  }
  nvds_tracker_config->ll_lib_file = strdup(
      ((std::string)tracking_config["ll_lib_file"].get<std::string>()).c_str());
  create_tracking_bin(nvds_tracker_config, nvds_tracker_bin);
  return nvds_tracker_bin->bin;
}