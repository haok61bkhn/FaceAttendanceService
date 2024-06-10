set(MODULE_NAME config)
file(GLOB HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${MODULE_NAME} ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(${MODULE_NAME} PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                                             PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${MODULE_NAME} PUBLIC)

target_compile_features(${MODULE_NAME} PUBLIC )
