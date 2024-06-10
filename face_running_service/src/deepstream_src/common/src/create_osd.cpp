#include "create_osd.hpp"
#include "config/config.hpp"
#include "deepstream_osd.h"

GstElement* OSD::Create() {
  NvDsOSDBin* nvds_osd_bin = new NvDsOSDBin();
  NvDsOSDConfig* nvds_osd_config = new NvDsOSDConfig();

  auto all_configs = Config::Instance()->GetAllConfig();
  int nvbuf_memory_type = all_configs["common"]["nvbuf_memory_type"];
  auto osd_config = all_configs["source"]["osd"];

  nvds_osd_config->font = "Serif";
  nvds_osd_config->clock_y_offset = 10;
  nvds_osd_config->clock_text_size = 10;
  // nvds_osd_config->mode = ;
  nvds_osd_config->gpu_id = 0;
  nvds_osd_config->nvbuf_memory_type = nvbuf_memory_type;
  nvds_osd_config->draw_text = true;
  nvds_osd_config->draw_bbox = true;
  nvds_osd_config->draw_mask = true;
  create_osd_bin(nvds_osd_config, nvds_osd_bin);

  return nvds_osd_bin->bin;
}