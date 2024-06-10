#include "create_appsink.hpp"

// callback function
GstFlowReturn AppSink::NewSample(GstElement* sink, gpointer data) {
  FrameData* frame_data = (FrameData*)data;

  GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
  GstCaps* caps = gst_sample_get_caps(sample);
  GstBuffer* buffer = gst_sample_get_buffer(sample);
  GstStructure* structure = gst_caps_get_structure(caps, 0);
  const int width =
      g_value_get_int(gst_structure_get_value(structure, "width"));
  const int height =
      g_value_get_int(gst_structure_get_value(structure, "height"));
  GstMapInfo map;
  gst_buffer_map(buffer, &map, GST_MAP_READ);
  cv::Mat img(height * 3 / 2, width, CV_8UC1, map.data);
  cv::cvtColor(img, img, cv::COLOR_YUV2BGR_I420);
  frame_data->frame_queue->push(img);
  gst_buffer_unmap(buffer, &map);
  gst_sample_unref(sample);
  return GST_FLOW_OK;
}

GstElement* AppSink::CreateAppSink(gpointer data, std::string& appsink_name) {
  GstElement* appsink;
  appsink = gst_element_factory_make("appsink", appsink_name.c_str());
  GstCaps* caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING,
                                      "I420", "width", G_TYPE_INT, 1920,
                                      "height", G_TYPE_INT, 1080, NULL);

  g_object_set(G_OBJECT(appsink), "sync", FALSE, NULL);
  g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);
  g_object_set(G_OBJECT(appsink), "drop", TRUE, NULL);
  g_object_set(G_OBJECT(appsink), "async", TRUE, NULL);
  g_object_set(G_OBJECT(appsink), "caps", caps, "emit-signals", TRUE, NULL);
  g_object_set(G_OBJECT(appsink), "enable-last-sample", TRUE, NULL);
  g_object_set(G_OBJECT(appsink), "qos", 0, NULL);

  g_signal_connect(appsink, "new-sample", G_CALLBACK(NewSample), data);
  gst_caps_unref(caps);
  return appsink;
}