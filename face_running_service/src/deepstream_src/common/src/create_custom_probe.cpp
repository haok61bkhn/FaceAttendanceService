#include "create_custom_probe.hpp"
#include "config/config.hpp"
#include "create_appsink.hpp"

static void ChangeSizeTiler(int max_index, int& row_tiler, int& col_tiler) {
  row_tiler = (guint)(sqrt(max_index));
  col_tiler = (guint)ceil(1.0 * max_index / row_tiler);
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
  GstMapInfo inmap = GST_MAP_INFO_INIT;
  if (!gst_buffer_map(buf, &inmap, GST_MAP_READ)) {
    GST_ERROR("input buffer mapinfo failed");
    return GST_PAD_PROBE_DROP;
  }
  NvBufSurface* surface = (NvBufSurface*)inmap.data;
  gst_buffer_unmap(buf, &inmap);

  NvDsBatchMeta* batch_meta = gst_buffer_get_nvds_batch_meta(buf);
  for (l_frame = batch_meta->frame_meta_list; l_frame != nullptr;
       l_frame = l_frame->next) {
    types::FrameInfor data_frame;
    NvDsFrameMeta* frame_meta = (NvDsFrameMeta*)(l_frame->data);
    int source_id = frame_meta->source_id;
    if (source_id >= customData->max_index) {
      customData->max_index = source_id + 1;
      ChangeSizeTiler(customData->max_index, customData->row_tiler,
                      customData->col_tiler);
      int width = 1920 / customData->col_tiler;
      int height = 1080 / customData->row_tiler;
      customData->sub_reconnect_frame =
          customData->reconnect_frame_origin.clone();
      cv::resize(customData->sub_reconnect_frame,
                 customData->sub_reconnect_frame, cv::Size(width, height));
    }
    customData->last_runnings[source_id] = now;
    source_id = customData->map_showing_source[source_id];
    data_frame.fps = customData->profilers[source_id].Tick();
    customData->frame_numbers[source_id]++;
    int now_milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                        p1.time_since_epoch())
                        .count();
    data_frame.frame_number = customData->frame_numbers[source_id];
    data_frame.fps = 1000.0 / (now_milli - customData->last_times[source_id]);
    customData->last_times[source_id] = now_milli;
    data_frame.source_id = source_id;
    void* data_ptr = surface->surfaceList[frame_meta->batch_id].dataPtr;
    int src_height = surface->surfaceList[frame_meta->batch_id].height;
    int src_width = surface->surfaceList[frame_meta->batch_id].width;
    int data_size = surface->surfaceList[frame_meta->batch_id].dataSize;
    uint32_t src_pitch = surface->surfaceList[frame_meta->batch_id].pitch;
    cv::cuda::GpuMat gpu_mat = cv::cuda::GpuMat(src_height * 1.5, src_width,
                                                CV_8UC1, data_ptr, src_pitch);
    cv::Mat cpu_mat;
    gpu_mat.download(cpu_mat);
    cv::cvtColor(cpu_mat, cpu_mat, cv::COLOR_YUV2BGR_NV12);
    if (customData->main_image_paths[source_id] == "") {
      int height = data_frame.frame.rows;
      int width = data_frame.frame.cols;
      data_frame.frame = cpu_mat;
    }
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
        float pad_x = (obj_meta->mask_params.width - 1920.0 * gain) / 2.0;
        float pad_y = (obj_meta->mask_params.height - 1080.0 * gain) / 2.0;

        int num_points = 5;
        for (int i = 0; i < num_points; i += 1) {
          auto mask_params = &obj_meta->mask_params;
          int x = (mask_params->data[i * 3] - pad_x) / gain - x1;
          int y = (mask_params->data[i * 3 + 1] - pad_y) / gain - y1;
          cv::Point point(x, y);
          object.key_points.push_back(point);
        }
      }

      object.class_id = obj_meta->class_id;
      object.confidence = obj_meta->confidence;
      object.class_name = obj_meta->obj_label;

      object.tracking_id = obj_meta->object_id;
      object.object_image = cpu_mat(cv::Rect(x1, y1, x2 - x1, y2 - y1)).clone();
      data_frame.objects.push_back(object);
    }

    data_frame.last_time = now;
    data_frame.time_stamp = now;
    customData->queue_datas->push(data_frame);
  }
  return GST_PAD_PROBE_OK;
}