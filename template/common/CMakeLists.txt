cmake_minimum_required(VERSION 3.20)
set(TARGET_NAME Test)
set(YURRGOHT_DIR ${CMAKE_SOURCE_DIR})
project(${TARGET_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Platform-independent header and source file search
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "source/*.h")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "source/*.cpp")
file(GLOB RESOURCE CONFIGURE_DEPENDS "source/resource/Yurrgoht.rc")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE})

add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/template/common/libs)
set(CMAKE_INSTALL_RPATH "$ORIGIN/libs")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

target_link_libraries(${TARGET_NAME} PRIVATE Editor)
target_link_libraries(${TARGET_NAME} PRIVATE Engine)
target_include_directories(${TARGET_NAME} PRIVATE ${YURRGOHT_DIR}/source)

# Set subsystem and entry point based on the platform
set(SUBSYSTEM_LINKER_OPTIONS "$<IF:$<NOT:$<CONFIG:Debug>>,-mwindows,>")
target_link_options(${TARGET_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})


# Copy dependency libraries
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/template/common/libs)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying shared libraries..."
    COMMAND ldd ${CMAKE_BINARY_DIR}/template/common/${TARGET_NAME} | cut -d' ' -f3 |  grep -v 'linux-vdso.so.1' |  xargs -I {} cp -v {} ${CMAKE_BINARY_DIR}/template/common/libs/
)
