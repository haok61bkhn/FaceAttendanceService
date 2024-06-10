set(MODULE_NAME common)
file(GLOB HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
find_package(OpenCV REQUIRED)
message(STATUS "Find OpenCV include at ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Find OpenCV libraries: ${OpenCV_LIBRARIES}")

add_library(${MODULE_NAME} ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(${MODULE_NAME} PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${MODULE_NAME} PUBLIC cuda gstreammer nvds_common config ${OpenCV_LIBRARIES} gstapp-1.0)
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_20)
