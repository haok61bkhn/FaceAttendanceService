#include "SaveImageThread.h"

void SaveImageFeature(LockFreeLifoQueue<types::FrameSaving>& saving_queue,
                      bool& stop_save_image_thread) {
  while (!stop_save_image_thread) {
    types::FrameSaving frame_if;

    if (saving_queue.pop(frame_if)) {
      if (saving_queue.size() > 500) {
        std::cout << "saving_queue " << saving_queue.size() << std::endl;
      }

      auto future = std::async(std::launch::async, [frame_if]() {
        if (!frame_if.image_path.empty())
          cv::imwrite(frame_if.image_path, frame_if.frame);
      });
    } else
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  std::cout << "SaveImageFeature stop" << std::endl;
}