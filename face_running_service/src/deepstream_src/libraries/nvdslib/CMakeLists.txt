set(MODULE_NAME nvdslib)
set(DEEP_STREAM_LIBS /opt/nvidia/deepstream/deepstream/lib)

list(APPEND DEEPSTREAM_INCLUDE_DIRS "${DEEP_STREAM_LIBS}/../sources/includes/")

list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvdsgst_meta.so")
list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvds_meta.so")
list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvbufsurface.so")
list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvbufsurftransform.so")
# list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libgstnvdsseimeta.so")
# list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvdsgst_smartrecord.so")
list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvds_utils.so")
list(APPEND DEEPSTREAM_LIBRARIES "${DEEP_STREAM_LIBS}/libnvdsgst_helper.so")
list(APPEND DEEPSTREAM_LIBRARIES "/opt/nvidia/deepstream/deepstream/lib/libnvds_inferutils.so")

list(APPEND TENSORRT_LIBRARIES "nvinfer")
list(APPEND TENSORRT_LIBRARIES "nvparsers")
list(APPEND TENSORRT_LIBRARIES "nvinfer_plugin")
list(APPEND TENSORRT_LIBRARIES "nvonnxparser")

add_library(${MODULE_NAME} INTERFACE)
target_include_directories(${MODULE_NAME} INTERFACE ${DEEPSTREAM_INCLUDE_DIRS})
target_link_libraries(${MODULE_NAME} INTERFACE ${DEEPSTREAM_LIBRARIES} ${TENSORRT_LIBRARIES})
