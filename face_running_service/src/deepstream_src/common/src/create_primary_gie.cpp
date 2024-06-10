#include "create_primary_gie.hpp"
#include "config/config.hpp"

GstElement* PrimaryGie::Create(std::string name) {
  NvDsPrimaryGieBin* nvds_primary_gie_bin = new NvDsPrimaryGieBin();
  NvDsGieConfig* nvds_gie_config = new NvDsGieConfig();

  auto all_configs = Config::Instance()->GetAllConfig();
  auto primary_config = all_configs["gie"][name];

  std::string model_config = primary_config["model_config"];
  nvds_gie_config->config_file_path = strdup(model_config.c_str());
  auto enable = primary_config["enable"];
  nvds_gie_config->enable = enable ? 1 : 0;
  int unique_id = primary_config["unique_id"];
  nvds_gie_config->unique_id = unique_id;
  int gpu_id = primary_config["gpu_id"];
  nvds_gie_config->gpu_id = gpu_id;
  nvds_gie_config->plugin_type =
      NV_DS_GIE_PLUGIN_INFER;  // TODO: should get from config file
  int nvbuf_memory_type = all_configs["common"]["nvbuf_memory_type"];
  nvds_gie_config->nvbuf_memory_type = nvbuf_memory_type;

  create_primary_gie_bin(nvds_gie_config, nvds_primary_gie_bin);

  return nvds_primary_gie_bin->bin;
}