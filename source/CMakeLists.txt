cmake_minimum_required(VERSION 3.20)
set(TARGET_NAME yurrgoht_editor)
set(YURRGOHT_DIR ${CMAKE_SOURCE_DIR})
project(${TARGET_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Platform-independent header and source file search
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "project_source/*.h")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "project_source/*.cpp")
file(GLOB RESOURCE CONFIGURE_DEPENDS "project_source/resource/Yurrgoht.rc")
source_group(TREE "${YURRGOHT_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE})

# Create the executable
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE})

# Link dependencies
target_link_libraries(${TARGET_NAME} PRIVATE Editor)
target_link_libraries(${TARGET_NAME} PRIVATE Engine)

# Add include directories
target_include_directories(${TARGET_NAME} PRIVATE ${YURRGOHT_DIR}/project_source)
