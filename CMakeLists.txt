cmake_minimum_required(VERSION 3.20)
project(Yurrgoht VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(YURRGOHT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${YURRGOHT_ROOT_DIR}/install")

add_subdirectory(external)
add_subdirectory(source/engine)
add_subdirectory(source/editor)
add_subdirectory(source)