set(MODULE_NAME nvds_common)
file(GLOB HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB DS_HEADER "/opt/nvidia/deepstream/deepstream/sources/includes/*.h")
file(GLOB SOURCE_YAML "${CMAKE_CURRENT_SOURCE_DIR}/src/deepstream-yaml/*.cpp")

include_directories(${YAMLCPP_INCLUDE_DIRS})

add_library(${MODULE_NAME} ${SOURCE_LIST} ${HEADER_LIST} ${DS_HEADER} ${SOURCE_YAML})
target_include_directories(${MODULE_NAME} PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
                                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/deepstream-yaml")
target_link_libraries(${MODULE_NAME} PUBLIC  cuda gstreammer nvdslib jsonlib yaml-cpp  )
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_17)
