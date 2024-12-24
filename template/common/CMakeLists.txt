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

# Add subdirectory for Yurrgoht if not already present
if(NOT TARGET Yurrgoht)
    #add_subdirectory(${YURRGOHT_DIR} ${YURRGOHT_DIR}/build)
endif()

target_link_libraries(${TARGET_NAME} PRIVATE Editor)
target_link_libraries(${TARGET_NAME} PRIVATE Engine)
target_include_directories(${TARGET_NAME} PRIVATE ${YURRGOHT_DIR}/source)

# MSVC-specific compile options
if(MSVC)
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_OPTIONS "/GL")
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_OPTIONS "/INCREMENTAL:NO;/LTCG")
endif()

# this is supposed to make the executable run from where it's located, but it does not do this - fix tomorrow 
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/template/common")
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/build/template/common")

# Set subsystem and entry point based on the platform
if(WIN32)
    # Windows-specific linker options
    set(SUBSYSTEM_LINKER_OPTIONS "$<IF:$<CONFIG:Debug>,/SUBSYSTEM:CONSOLE,/SUBSYSTEM:WINDOWS>" "/ENTRY:mainCRTStartup")
else()
    # Unix-specific linker options (for example, Linux)
    set(SUBSYSTEM_LINKER_OPTIONS "$<IF:$<NOT:$<CONFIG:Debug>>,-mwindows,>")
endif()

target_link_options(${TARGET_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})
