#include "create_custom_probe.hpp"
#include <cuda_runtime.h>
#include <nvbufsurface.h>
#include <nvbufsurftransform.h>
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
  std::cout << "Color Format: "
            << surface->surfaceList[nvds_frame_meta->batch_id].colorFormat
            << std::endl;

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
  // gst_buffer_unmap(buffer, &in_map_info);
  return bgrMat;
}

GstPadProbeReturn Probe::CustomProbe(GstPad* pad, GstPadProbeInfo* info,
                                     gpointer data) {
  NvDsObjectMeta* obj_meta = NULL;
  NvDsMetaList* l_frame = NULL;
  NvDsMetaList* l_obj = NULL;
  NvDsBatchMeta* batch_meta;
  NvDsFrameMeta* frame_meta = NULL;

  CustomData* customData = (CustomData*)data;
  NvDsObjEncCtxHandle ctx = customData->obj_ctx_handle;

  auto p1 = std::chrono::system_clock::now();
  int now =
      std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
          .count();

  GstBuffer* buf = (GstBuffer*)info->data;
  GstMapInfo in_map_info;
  memset(&in_map_info, 0, sizeof(in_map_info));
  NvBufSurface* surface = NULL;

  if (gst_buffer_map(buf, &in_map_info, GST_MAP_READWRITE)) {
    surface = (NvBufSurface*)in_map_info.data;

    batch_meta = gst_buffer_get_nvds_batch_meta(buf);

    for (l_frame = batch_meta->frame_meta_list; l_frame != NULL;
         l_frame = l_frame->next) {
      frame_meta = (NvDsFrameMeta*)(l_frame->data);
      int offset = 0;
      float fps = customData->profilers[frame_meta->source_id].Tick();
      std::cout << "fps: " << fps << std::endl;

      cv::Mat src_mat_BGRA = GetImageFromProbe(info, frame_meta);
      // std::cout << "src_mat_BGRA: " << src_mat_BGRA.size() << std::endl;
      // cv::imwrite("src_mat_BGRA.jpg", src_mat_BGRA);
    }
  }

  gst_buffer_unmap(buf, &in_map_info);
  return GST_PAD_PROBE_OK;
}
