#ifndef CREATE_OSD_HPP
#define CREATE_OSD_HPP
#include <glib.h>
#include <gst/gst.h>

class OSD {
 public:
  static GstElement* Create();
};
#endif  // CREATE_OSD_HPP