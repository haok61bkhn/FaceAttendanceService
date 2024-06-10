cmake_minimum_required(VERSION 3.8.2)

project(qt_template)



include_directories(includes/types  includes src/ai_solution/includes src/ai_solution/third_party/engine-tool src/ai_solution/third_party/hnswlib src/ai_solution/third_party/yaml-cpp .)
add_subdirectory(src/ai_solution)
add_subdirectory(src/deepstream_src)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE RelWithDebInfo)


find_package(OpenCV REQUIRED)
message(STATUS "Find OpenCV include at ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Find OpenCV libraries: ${OpenCV_LIBRARIES}")


set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()


find_package(gRPC REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

set (proto_name deepstream_service)
get_filename_component(proto "protos/${proto_name}.proto" ABSOLUTE)
get_filename_component(proto_dir "${proto}" DIRECTORY)
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_dir}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${proto}"
    DEPENDS "${proto}"
)

set(V_NOABI_BSON_DIR /opt/mongo-cxx-driver/include/bsoncxx/v_noabi)
set(V_NOABI_MONGOCXX_DIR /opt/mongo-cxx-driver/include/mongocxx/v_noabi)
set(V_NOABI_BSON_LIB /opt/mongo-cxx-driver/lib)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${V_NOABI_BSON_DIR}
    ${V_NOABI_MONGOCXX_DIR}
)
link_directories(${V_NOABI_BSON_LIB})

add_executable(main 
              main.cpp            
              src/Tools.cpp
              src/CustomLogger.cpp
              src/AppConfig.cpp
              src/timer.cpp
              src/profiler.cpp
              src/SaveImageThread.cpp
              src/AIController.cpp
              src/DeepStreamManager.cpp
              src/CameraManager.cpp
              src/Face_Processor.cpp
              ${proto_srcs} 
              ${grpc_srcs}

              src/database/camera_db.cpp
              src/database/face_attendance_db.cpp
              )

target_link_libraries(main PRIVATE   gRPC::grpc++_reflection gRPC::grpc++ ${_PROTOBUF_LIBPROTOBUF} ${OpenCV_LIBRARIES} opencv_core opencv_highgui opencv_imgcodecs  opencv_imgproc mongocxx bsoncxx AI_SDK cuda gstreammer nvds_common config common utils sqlite3  )
