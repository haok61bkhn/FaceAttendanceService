#include "create_sink.hpp"

void Sink::CreateSingle(int index, std::string type,
                        NvDsSinkBin* nvds_sink_bin) {
  NvDsSinkSubBinConfig* nvds_sink_sub_bin_configs = new NvDsSinkSubBinConfig();
  NvDsSinkSubBinConfig* nvdss_sink_bin_config = new NvDsSinkSubBinConfig();
  nvdss_sink_bin_config->enable = true;
  nvdss_sink_bin_config->source_id = index;
  nvdss_sink_bin_config->sync = 1;
  if (type == "rtsp") {
    NvDsSinkEncoderConfig sink_encode_config;
    nvdss_sink_bin_config->type = NV_DS_SINK_UDPSINK;
    sink_encode_config.type = NV_DS_SINK_UDPSINK;
    sink_encode_config.codec = NV_DS_ENCODER_H265;
    sink_encode_config.enc_type = NV_DS_ENCODER_TYPE_HW;
    sink_encode_config.bitrate = 4000000;
    sink_encode_config.profile = 0;
    sink_encode_config.sync = 0;
    sink_encode_config.gpu_id = 0;
    sink_encode_config.rtsp_port = 8554;
    sink_encode_config.udp_port = 9000;
    sink_encode_config.udp_buffer_size = 0;
    sink_encode_config.iframeinterval = 0;
    nvdss_sink_bin_config->encoder_config = sink_encode_config;
    NvDsSinkBinSubBin udp_sink;
    nvds_sink_bin->sub_bins[0] = udp_sink;  // i->0
  } else if (type == "screen") {
    std::cout << "screen" << std::endl;
    NvDsSinkRenderConfig sink_render_config;
    nvdss_sink_bin_config->type = NV_DS_SINK_RENDER_EGL;
    sink_render_config.type = NV_DS_SINK_RENDER_EGL;
    sink_render_config.gpu_id = 0;
    sink_render_config.nvbuf_memory_type = 0;
    sink_render_config.sync = 0;
    nvdss_sink_bin_config->render_config = sink_render_config;
    NvDsSinkBinSubBin screen_sink;
    nvds_sink_bin->sub_bins[0] = screen_sink;  // i->0

  } else {
    std::cout << "not support" << std::endl;
  }
  nvds_sink_sub_bin_configs[0] = *nvdss_sink_bin_config;  // i-> 0
  create_sink_bin(1, nvds_sink_sub_bin_configs, nvds_sink_bin, index);
}

std::vector<GstElement*> Sink::Create(bool enable_fps_sink) {
  auto all_configs = Config::Instance()->GetAllConfig();
  auto sink_config = all_configs["sink"];
  auto sink_list = sink_config["sink_list"];
  int number_sink = sink_list.size();
  std::vector<GstElement*> sinks;
  sinks.resize(number_sink);

  for (auto i = 0; i < number_sink; i++) {
    auto sink = sink_list[i];
    auto type = sink["type"];
    NvDsSinkBin* nvds_sink_bin = new NvDsSinkBin();
    CreateSingle(i, type, nvds_sink_bin);
    sinks[i] = nvds_sink_bin->bin;
  }

  if (enable_fps_sink && sinks.size() > 0) {
    GstElement* fps_sink =
        gst_element_factory_make("fpsdisplaysink", "fps_sink");
    g_object_set(G_OBJECT(fps_sink), "text-overlay", FALSE, "video-sink",
                 sinks[sinks.size() - 1], NULL);
    sinks.pop_back();
    sinks.push_back(fps_sink);
  }
  std::cout << "sinks size: " << sinks.size() << std::endl;
  return sinks;
}