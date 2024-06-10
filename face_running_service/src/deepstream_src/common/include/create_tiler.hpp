#ifndef CREATE_TILER_HPP
#define CREATE_TILER_HPP
#include <glib.h>
#include <gst/gst.h>

class Tiler {
 public:
  static GstElement* Create();
};
#endif  // CREATE_TILER_HPP
