project(pydaisykit)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/python/third_party/pybind11)
include_directories(${CMAKE_SOURCE_DIR}/python/third_party/pybind11_opencv_numpy)
include_directories(${pybind11_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIR})

pybind11_add_module(py_ai MODULE src/main.cpp third_party/pybind11_opencv_numpy/ndarray_converter.cpp)
set_target_properties(py_ai PROPERTIES OUTPUT_NAME "py_ai")
target_link_libraries(py_ai PUBLIC AI_SDK)
set_property(TARGET py_ai PROPERTY FOLDER "python")