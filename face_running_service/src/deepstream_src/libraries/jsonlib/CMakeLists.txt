set(MODULE_NAME jsonlib)

find_package(PkgConfig) 
pkg_check_modules(GLIB REQUIRED  glib-2.0)
pkg_check_modules(JSON-GLIB REQUIRED json-glib-1.0)
list(APPEND INCLUDE_DIRS "/usr/include/json-glib-1.0/")

add_library(${MODULE_NAME} INTERFACE)
target_include_directories(${MODULE_NAME} INTERFACE ${INCLUDE_DIRS})
