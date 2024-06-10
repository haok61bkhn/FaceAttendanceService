#pragma once
#include "config/config.hpp"
#include "deepstream_primary_gie.h"

class PrimaryGie {
 public:
  /**
   * It creates a primary gie bin with the given name and config
   *
   * @param name The name of the GIE.
   *
   * @return A GstElement
   */
  static GstElement* Create(std::string name);
};