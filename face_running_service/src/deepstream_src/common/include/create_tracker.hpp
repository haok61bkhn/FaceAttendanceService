#ifndef CREATE_TRACKER_HPP
#define CREATE_TRACKER_HPP
#include <glib.h>
#include <gst/gst.h>

class Tracker {
 public:
  static GstElement* Create();
};
#endif  // CREATE_TRACKER_HPP