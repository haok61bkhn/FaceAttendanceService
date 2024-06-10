#pragma once

#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include "LockFreeLifoQueue.h"
#include "Tools.h"
#include "create_appsink.hpp"
#include "types/FrameSaving.h"

void SaveImageFeature(LockFreeLifoQueue<types::FrameSaving>& saving_queue,
                      bool& stop_save_image_thread);
