#include "create_source.hpp"
#include "config/config.hpp"
#include "deepstream_sources.h"
#include "spdlog/spdlog.h"
#define GST_CAPS_FEATURES_NVMM "memory:NVMM"

static void decodebin_child_added(GstChildProxy* child_proxy, GObject* object,
                                  gchar* name, gpointer user_data) {
  g_print("Decodebin child added: %s\n", name);
  if (g_strrstr(name, "decodebin") == name) {
    g_signal_connect(G_OBJECT(object), "child-added",
                     G_CALLBACK(decodebin_child_added), user_data);
  }
  if (g_strrstr(name, "source") == name) {
    g_object_set(G_OBJECT(object), "drop-on-latency", false, NULL);
  }
}

static void cb_newpad(GstElement* decodebin, GstPad* pad, gpointer data) {
  GstCaps* caps = gst_pad_query_caps(pad, NULL);
  const GstStructure* str = gst_caps_get_structure(caps, 0);
  const gchar* name = gst_structure_get_name(str);

  g_print("decodebin new pad %s\n", name);
  if (!strncmp(name, "video", 5)) {
    CB_NEWPAD_DATA_TYPE* streamux_infor = (CB_NEWPAD_DATA_TYPE*)data;
    guint source_id = streamux_infor->source_id;
    GstElement* streammux = streamux_infor->streammux;
    gchar pad_name[16] = {0};
    GstPad* sinkpad = NULL;
    g_snprintf(pad_name, 15, "sink_%u", source_id);
    sinkpad = gst_element_get_request_pad(streammux, pad_name);
    if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
      g_print("Failed to link decodebin to pipeline\n");
    } else {
      g_print("Decodebin linked to pipeline\n");
    }
    delete streamux_infor;
    gst_object_unref(sinkpad);
  }
}

GstElement* Source::Create(guint index, const gchar* uri,
                           GstElement* streammux) {
  auto all_configs = Config::Instance()->GetAllConfig();
  auto common_config = all_configs["common"];
  auto source_config = all_configs["source"];

  GstElement* uri_decode_bin = NULL;
  gchar bin_name[16] = {};
  g_snprintf(bin_name, 15, "source-bin-%02d", index);
  uri_decode_bin = gst_element_factory_make("nvurisrcbin", bin_name);

  int drop_frame_interval = source_config["drop_frame_interval"];
  int cudadec_mem_type = common_config["cudadec_mem_type"];
  int rtsp_reconnect_interval_sec =
      source_config["rtsp_reconnect_interval_sec"];
  int latency = source_config["latency"];

  g_object_set(G_OBJECT(uri_decode_bin), "file-loop", FALSE, NULL);
  g_object_set(G_OBJECT(uri_decode_bin), "cudadec-memtype", cudadec_mem_type,
               NULL);
  g_object_set(G_OBJECT(uri_decode_bin), "drop-frame-interval",
               drop_frame_interval, "latency", latency, NULL);
  g_object_set(G_OBJECT(uri_decode_bin), "rtsp-reconnect-interval",
               rtsp_reconnect_interval_sec, NULL);
  g_object_set(G_OBJECT(uri_decode_bin), "uri", uri, NULL);

  // g_object_set(G_OBJECT(uri_decode_bin), "smart-record", 2, NULL);
  // g_object_set(G_OBJECT(uri_decode_bin), "smart-rec-cache", 5, NULL);
  // g_object_set(G_OBJECT(uri_decode_bin), "smart-rec-dir-path", "DATA/VIDEO",
  //  NULL);
  // g_object_set(G_OBJECT(uri_decode_bin), "smart-rec-mode", 1, NULL);
  // g_object_set(G_OBJECT(uri_decode_bin), "smart-rec-default-duration", 5,
  // NULL);

  CB_NEWPAD_DATA_TYPE* streamux_infor =
      new CB_NEWPAD_DATA_TYPE(streammux, index);
  g_signal_connect(G_OBJECT(uri_decode_bin), "pad-added", G_CALLBACK(cb_newpad),
                   streamux_infor);
  g_signal_connect(G_OBJECT(uri_decode_bin), "child-added",
                   G_CALLBACK(decodebin_child_added), &index);
  return uri_decode_bin;
}