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
    target_link_libraries(imgui PUBLIC glfw ${Vulkan_LIBRARY})
    set_target_properties(imgui PROPERTIES FOLDER ${external_folder}/imgui)
endif()