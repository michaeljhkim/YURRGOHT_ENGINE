cmake_minimum_required(VERSION 3.20)
set(TARGET_NAME SourceTest)
project(${TARGET_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Platform-specific compiler options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
elseif(UNIX)
    message(STATUS "Compiling for UNIX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O2")
endif()

# File search and organization
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "source/*.h")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "source/*.cpp")
if(WIN32)
    file(GLOB RESOURCE CONFIGURE_DEPENDS "source/resource/Yurrgoht.rc")
    list(APPEND SOURCE_FILES ${RESOURCE})
endif()
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# Add executable
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Yurrgoht directory path setup
if(WIN32)
    set(Yurrgoht_DIR "I:/Yurrgoht")
elseif(UNIX)
    set(Yurrgoht_DIR "${CMAKE_SOURCE_DIR}")
endif()

if(NOT TARGET Yurrgoht)
    #add_subdirectory(${Yurrgoht_DIR} ${Yurrgoht_DIR}/build)
endif()

# Link libraries (ensure they are cross-platform)
#if(WIN32 OR UNIX)
    target_link_libraries(${TARGET_NAME} PRIVATE Editor Engine)
    target_include_directories(${TARGET_NAME} PRIVATE ${Yurrgoht_DIR}/source)
#endif()