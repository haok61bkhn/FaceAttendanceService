#pragma once
#include <glib.h>
#include <gst/gst.h>
#include <iostream>
#include <list>
#include "gstnvdsmeta.h"
#include "nvbufsurface.h"
#include "nvds_obj_encode.h"
#include "opencv2/opencv.hpp"

class Probe {
 public:
  static GstPadProbeReturn CustomProbe(GstPad* pad, GstPadProbeInfo* info,
                                       gpointer data);
};