set(MODULE_NAME spdlog)

find_package(PkgConfig) 
find_package(spdlog REQUIRED)
list(APPEND DEPS spdlog::spdlog)

add_library(${MODULE_NAME} INTERFACE)
target_link_libraries(${MODULE_NAME} INTERFACE ${DEPS})