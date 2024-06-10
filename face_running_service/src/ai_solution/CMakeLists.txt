set(MODULE_NAME AI_SDK)

set(CMAKE_CXX_STANDARD 14)
if(ENABLE_EXCEPTIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

if(ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

# CUDA
find_package(CUDA REQUIRED)
message(STATUS "Find CUDA include at ${CUDA_INCLUDE_DIRS}")
message(STATUS "Find CUDA libraries: ${CUDA_LIBRARIES}")

# TensorRT
set(TENSORRT_ROOT /usr/src/tensorrt/)
find_path(TENSORRT_INCLUDE_DIR NvInfer.h
    HINTS ${TENSORRT_ROOT} PATH_SUFFIXES include/)
message(STATUS "Found TensorRT headers at ${TENSORRT_INCLUDE_DIR}")
find_library(TENSORRT_LIBRARY_INFER nvinfer
    HINTS ${TENSORRT_ROOT} ${TENSORRT_BUILD} ${CUDA_TOOLKIT_ROOT_DIR}
    PATH_SUFFIXES lib lib64 lib/x64)
find_library(TENSORRT_LIBRARY_ONNXPARSER nvonnxparser
    HINTS ${TENSORRT_ROOT} ${TENSORRT_BUILD} ${CUDA_TOOLKIT_ROOT_DIR}
    PATH_SUFFIXES lib lib64 lib/x64)
set(TENSORRT_LIBRARY ${TENSORRT_LIBRARY_INFER} ${TENSORRT_LIBRARY_ONNXPARSER})
message(STATUS "Find TensorRT libs: ${TENSORRT_LIBRARY}")

# Opencv
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED )
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/engine-tool )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/yaml-cpp )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/hnswlib )
file(GLOB SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


include_directories(${CUDA_INCLUDE_DIRS} ${TENSORRT_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(sources
    src/engine_model.cpp
    src/image_model.cpp
    src/face_detection.cpp
    src/face_extraction.cpp
    src/face_alignment.cpp
    third_party/engine-tool/common.cpp
    src/lp_detection_yolov8.cpp
    src/char_detection_yolov8.cpp
    src/detection_yolov8.cpp
    src/face_manager.cpp
    src/face_recognition.cpp
)

add_library(${MODULE_NAME} STATIC ${sources} ${SOURCE_LIST})

target_link_libraries(${MODULE_NAME}  PUBLIC nvinfer nvinfer_plugin ${OpenCV_LIBS} ${CUDA_LIBRARIES} ${TENSORRT_LIBRARY} yaml-cpp)
#target_compile_features(${MODULE_NAME} PUBLIC )

# add_subdirectory(examples)
