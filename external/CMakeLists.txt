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
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" 
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp" 
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.h")
 
    add_library(imgui ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC ${IMGUI_SOURCE_DIR})
    target_include_directories(imgui PUBLIC ${Vulkan_INCLUDE_DIR})
    target_link_libraries(imgui PUBLIC glfw3 ${Vulkan_LIBRARY})
    set_target_properties(imgui PROPERTIES FOLDER ${external_folder}/imgui)
endif()


if (NOT TARGET rttr_core_lib)
    option(BUILD_RTTR_DYNAMIC "" OFF)
    option(BUILD_UNIT_TESTS "" OFF)
    option(BUILD_STATIC "" ON)
    option(BUILD_WITH_STATIC_RUNTIME_LIBS "" ON)
    option(BUILD_EXAMPLES "" OFF)
    option(BUILD_DOCUMENTATION "" OFF)
    option(BUILD_INSTALLER OFF)
    option(BUILD_PACKAGE OFF)
    add_subdirectory(rttr)
    set_target_properties(rttr_core_lib PROPERTIES FOLDER ${external_folder}/rttr)
endif()