#pragma once
#include <glib.h>
#include <gst/gst.h>
#include <stdio.h>
#include <iostream>
#include "gst-nvmessage.h"
#include "gstnvdsmeta.h"
#include "nvdsmeta.h"

struct BUS_CALL_DATA_TYPE {
  BUS_CALL_DATA_TYPE(GMainLoop* loop, GMutex& eos_lock, gboolean* eos_list)
      : loop(loop), eos_lock(eos_lock), eos_list(eos_list) {}
  GMutex& eos_lock;
  gboolean* eos_list;
  GMainLoop* loop;
};
static gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
  BUS_CALL_DATA_TYPE* bus_call_data = (BUS_CALL_DATA_TYPE*)data;
  GMainLoop* loop = bus_call_data->loop;
  GMutex& eos_lock = bus_call_data->eos_lock;
  gboolean* g_eos_list = bus_call_data->eos_list;
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:

    case GST_MESSAGE_WARNING: {
      gchar* debug;
      GError* error;
      gst_message_parse_warning(msg, &error, &debug);
      g_printerr("WARNING from element %s: %s\n", GST_OBJECT_NAME(msg->src),
                 error->message);
      g_free(debug);
      g_printerr("Warning: %s\n", error->message);
      g_error_free(error);
      break;
    }
    case GST_MESSAGE_ERROR: {
      gchar* debug;
      GError* error;
      gst_message_parse_error(msg, &error, &debug);
      g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME(msg->src),
                 error->message);
      if (debug) g_printerr("Error details: %s\n", debug);
      g_free(debug);
      g_error_free(error);
      g_main_loop_quit(loop);
      break;
    }
    case GST_MESSAGE_ELEMENT: {
      if (gst_nvmessage_is_stream_eos(msg)) {
        guint stream_id;
        if (gst_nvmessage_parse_stream_eos(msg, &stream_id)) {
          g_print("Got EOS from stream %d\n", stream_id);
          g_mutex_lock(&eos_lock);
          g_eos_list[stream_id] = TRUE;
          g_mutex_unlock(&eos_lock);
        }
      }
      break;
    }
    default:
      break;
  }
  return TRUE;
}