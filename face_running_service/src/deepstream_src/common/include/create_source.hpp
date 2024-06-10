#ifndef CREATE_SOURCE_HPP
#define CREATE_SOURCE_HPP
#include <glib.h>
#include <gst/gst.h>
#include "deepstream_sources.h"
#include "nlohmann/json.hpp"

struct CB_NEWPAD_DATA_TYPE {
  CB_NEWPAD_DATA_TYPE(GstElement* streammux, int source_id)
      : streammux(streammux), source_id(source_id) {}
  GstElement* streammux;
  GstElement** source_bin_list;
  int source_id;
};

class Source {
 public:
  /**
   * It creates a source element.
   *
   * @return GstElement*
   */
  static GstElement* Create(guint index, const gchar* uri,
                            GstElement* streammux);
};
#endif  // CREATE_SOURCE_HPP
