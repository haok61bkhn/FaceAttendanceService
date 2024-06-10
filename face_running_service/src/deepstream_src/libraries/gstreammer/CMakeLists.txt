set(MODULE_NAME gstreammer)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-base-1.0 gstreamer-video-1.0 gstreamer-rtsp-server-1.0)

add_library(${MODULE_NAME} INTERFACE)
target_include_directories(${MODULE_NAME} INTERFACE ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(${MODULE_NAME} INTERFACE ${GSTREAMER_LIBRARIES})
