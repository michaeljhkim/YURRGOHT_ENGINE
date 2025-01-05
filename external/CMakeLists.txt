include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(external_folder "External")

if (NOT TARGET Vulkan)
    find_package(Vulkan)
    if (NOT Vulkan_FOUND)
        message(FATAL_ERROR "vulkan not found")
    elseif(NOT Vulkan_glslangValidator_FOUND)
        message(FATAL_ERROR "vulkan shader compiler not found")
    elseif(NOT Vulkan_INCLUDE_DIR)
        message(FATAL_ERROR "vulkan include not found")
    endif()
endif()

if(NOT TARGET imgui)
    set(BUILD_SHARED_LIBS OFF)
    set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${IMGUI_SOURCE_DIR}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp" 
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl2.h"
    #"${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp" 
    #"${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.h"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp" 
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.h")
 
    add_library(imgui ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC ${IMGUI_SOURCE_DIR})
    target_include_directories(imgui PUBLIC ${Vulkan_INCLUDE_DIR})
    find_package(SDL2 REQUIRED)
    target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIR})
    target_link_libraries(imgui PUBLIC SDL2 ${Vulkan_LIBRARY})
    set_target_properties(imgui PROPERTIES FOLDER ${external_folder}/imgui)
endif()


if(NOT TARGET Jolt)
    option(TARGET_HELLO_WORLD "" OFF)
    option(TARGET_PERFORMANCE_TEST "" OFF)
    option(TARGET_SAMPLES "" OFF)
    option(TARGET_UNIT_TESTS "" OFF)
    option(TARGET_VIEWER "" OFF)

    add_subdirectory(jolt)
    #set_target_properties(Jolt PROPERTIES FOLDER ${external_folder}/jolt)
endif()


FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG "0.8.0"
)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG "11.1.1"
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG "v1.15.0"
)
FetchContent_MakeAvailable(yaml-cpp fmt spdlog)