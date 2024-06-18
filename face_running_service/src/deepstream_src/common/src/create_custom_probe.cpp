#include "create_custom_probe.hpp"
#include "config/config.hpp"
#include "create_appsink.hpp"

static void ChangeSizeTiler(int max_index, int& row_tiler, int& col_tiler) {
  row_tiler = (guint)(sqrt(max_index));
  col_tiler = (guint)ceil(1.0 * max_index / row_tiler);
}

cv::Mat Probe::GetImageFromProbe(GstPadProbeInfo* info,
                                 NvDsFrameMeta* nvds_frame_meta) {
  GstMapInfo in_map_info;
  GstBuffer* buffer = static_cast<GstBuffer*>(info->data);

  memset(&in_map_info, 0, sizeof(in_map_info));
  if (!gst_buffer_map(buffer, &in_map_info, GST_MAP_READ)) {
    gst_buffer_unmap(buffer, &in_map_info);
    return cv::Mat();
  }
  NvBufSurface* surface = reinterpret_cast<NvBufSurface*>(in_map_info.data);
  int height = surface->surfaceList[nvds_frame_meta->batch_id].height;
  int width = surface->surfaceList[nvds_frame_meta->batch_id].width;
  int pitch = surface->surfaceList[nvds_frame_meta->batch_id].pitch;
  if (NvBufSurfaceMap(surface, nvds_frame_meta->batch_id, 0, NVBUF_MAP_READ) !=
      0) {
    std::cout << "NvBufSurfaceMap for getting mappedAddr to be accessed by CPU "
                 "failed";
  }

  if (surface->memType == NVBUF_MEM_SURFACE_ARRAY) {
    NvBufSurfaceSyncForCpu(surface, nvds_frame_meta->batch_id, 0);
  }

  void* mappedAddr =
      surface->surfaceList[nvds_frame_meta->batch_id].mappedAddr.addr[0];
  cv::Mat rgbaMat = cv::Mat(height, width, CV_8UC4, mappedAddr, pitch);

  cv::Mat bgrMat;
  cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGBA2BGR);

  NvBufSurfaceUnMap(surface, nvds_frame_meta->batch_id, 0);
  gst_buffer_unmap(buffer, &in_map_info);
  return bgrMat;
}

GstPadProbeReturn Probe::CustomProbe(GstPad* pad, GstPadProbeInfo* info,
                                     gpointer data) {
  NvDsObjectMeta* obj_meta = NULL;
  NvDsMetaList* l_frame = NULL;
  NvDsMetaList* l_obj = NULL;

  CustomData* customData = (CustomData*)data;
  NvDsObjEncCtxHandle ctx = customData->obj_ctx_handle;

  auto p1 = std::chrono::system_clock::now();
  int now =
      std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
          .count();

  GstBuffer* buf = (GstBuffer*)info->data;
  // GstMapInfo inmap = GST_MAP_INFO_INIT;
  // if (!gst_buffer_map(buf, &inmap, GST_MAP_READ)) {
  //   GST_ERROR("input buffer mapinfo failed");
  //   return GST_PAD_PROBE_DROP;
  // }
  // NvBufSurface* surface = (NvBufSurface*)inmap.data;
  // gst_buffer_unmap(buf, &inmap);

  NvDsBatchMeta* batch_meta = gst_buffer_get_nvds_batch_meta(buf);
  for (l_frame = batch_meta->frame_meta_list; l_frame != nullptr;
       l_frame = l_frame->next) {
    types::FrameInfor data_frame;
    NvDsFrameMeta* frame_meta = (NvDsFrameMeta*)(l_frame->data);
    data_frame.fps = customData->profilers[frame_meta->source_id].Tick();
    customData->frame_numbers[frame_meta->source_id]++;
    int now_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                        p1.time_since_epoch())
                        .count();
    data_frame.frame_number = customData->frame_numbers[frame_meta->source_id];

    std::cout << "SOURCE ID: " << frame_meta->source_id
              << " FPS: " << data_frame.fps << std::endl;
    customData->last_times[frame_meta->source_id] = now_milli;
    data_frame.source_id = frame_meta->source_id;
    // std::cout << "COLOR FORMAT: "
    //           << surface->surfaceList[frame_meta->batch_id].colorFormat
    //           << std::endl;

    if (frame_meta->num_obj_meta == 0) {
      continue;
    }

    cv::Mat cpu_mat = GetImageFromProbe(info, frame_meta);

    for (auto l_obj = frame_meta->obj_meta_list; l_obj != nullptr;
         l_obj = l_obj->next) {
      NvDsObjectMeta* obj_meta = (NvDsObjectMeta*)(l_obj->data);
      types::ObjectInfor object;

      object.rect =
          cv::Rect(obj_meta->rect_params.left, obj_meta->rect_params.top,
                   obj_meta->rect_params.width, obj_meta->rect_params.height);
      int padding = 10;
      int x1 = std::max(0, object.rect.x - padding);
      int y1 = std::max(0, object.rect.y - padding);
      int x2 =
          std::min(cpu_mat.cols, object.rect.x + object.rect.width + padding);
      int y2 =
          std::min(cpu_mat.rows, object.rect.y + object.rect.height + padding);

      if ((float)obj_meta->mask_params.size > 0) {
        float gain = std::min(obj_meta->mask_params.width / 1920.0,
                              obj_meta->mask_params.height / 1080.0);

        int num_points = 5;
        for (int i = 0; i < num_points; i += 1) {
          auto mask_params = &obj_meta->mask_params;
          int x = (mask_params->data[i * 3]) / gain - x1;
          int y = (mask_params->data[i * 3 + 1]) / gain - y1;
          cv::Point point(x, y);
          object.key_points.push_back(point);
        }
      }

      object.class_id = obj_meta->class_id;
      object.confidence = obj_meta->confidence;
      object.class_name = obj_meta->obj_label;
      object.tracking_id = obj_meta->object_id;

      object.object_image = cpu_mat(cv::Rect(x1, y1, x2 - x1, y2 - y1)).clone();

      int padding_full = 50;
      int x1_full = std::max(0, object.rect.x - padding_full);
      int y1_full = std::max(0, object.rect.y - padding_full);
      int x2_full = std::min(cpu_mat.cols,
                             object.rect.x + object.rect.width + padding_full);
      int y2_full = std::min(cpu_mat.rows,
                             object.rect.y + object.rect.height + padding_full);
      object.object_image_full =
          cpu_mat(
              cv::Rect(x1_full, y1_full, x2_full - x1_full, y2_full - y1_full))
              .clone();

      data_frame.objects.push_back(object);
    }
    data_frame.last_time = now;
    data_frame.time_stamp = now;
    customData->queue_datas->push(data_frame);
  }
  return GST_PAD_PROBE_OK;
}