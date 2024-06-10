set(MODULE_NAME cuda)

find_package(PkgConfig) 
find_package(CUDA REQUIRED)
message(STATUS "Find CUDA include at ${CUDA_INCLUDE_DIRS}")
message(STATUS "Find CUDA libraries: ${CUDA_LIBRARIES}")

add_library(${MODULE_NAME} INTERFACE)
target_include_directories(${MODULE_NAME} INTERFACE ${CUDA_INCLUDE_DIRS})
target_link_libraries(${MODULE_NAME} INTERFACE ${CUDA_LIBRARIES})