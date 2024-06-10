#include "create_tiler.hpp"
#include "config/config.hpp"
#include "deepstream_tiled_display.h"

GstElement* Tiler::Create() {
  NvDsTiledDisplayBin* nvds_tiler_display_bin = new NvDsTiledDisplayBin();
  NvDsTiledDisplayConfig* nvds_tiler_display_config =
      new NvDsTiledDisplayConfig();

  auto all_configs = Config::Instance()->GetAllConfig();
  int nvbuf_memory_type = all_configs["common"]["nvbuf_memory_type"];
  auto tiler_config = all_configs["tiler"];

  bool enable = tiler_config["enable"];

  int rows = tiler_config["rows"];
  int columns = tiler_config["columns"];
  int width = tiler_config["width"];
  int height = tiler_config["height"];
  int gpu_id = tiler_config["gpu_id"];
  // auto show_source = tiler_config["show_source"];

  nvds_tiler_display_config->enable =
      enable ? NV_DS_TILED_DISPLAY_ENABLE : NV_DS_TILED_DISPLAY_DISABLE;
  nvds_tiler_display_config->rows = rows;
  nvds_tiler_display_config->columns = columns;
  nvds_tiler_display_config->width = width;
  nvds_tiler_display_config->height = height;
  nvds_tiler_display_config->gpu_id = gpu_id;
  nvds_tiler_display_config->nvbuf_memory_type = nvbuf_memory_type;
  create_tiled_display_bin(nvds_tiler_display_config, nvds_tiler_display_bin);

  return nvds_tiler_display_bin->bin;
}