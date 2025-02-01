set(TARGET_NAME Engine)
set(CPACK_GENERATOR "")

# Search for libraries
find_package(Vulkan REQUIRED)
find_package(Ktx REQUIRED)


# must manually link dotnet sdk libraries
set(DOTNET_RUNTIME_PATH "/usr/share/dotnet/")
file(GLOB DOTNET_RUNTIME_LIBS
    ${DOTNET_RUNTIME_PATH}/packs/Microsoft.NETCore.App.Host.*/*/runtimes/*/native/libnethost.so
    ${DOTNET_RUNTIME_PATH}/shared/Microsoft.NETCore.App/*/libcoreclr.so
    ${DOTNET_RUNTIME_PATH}/host/fxr/*/libhostfxr.so)
file(GLOB DOTNET_HEADERS ${DOTNET_RUNTIME_PATH}/packs/Microsoft.NETCore.App.Host.*/*/runtimes/*/native/)
link_directories(${DOTNET_HEADERS})

# for host pc to find linked files
file(GLOB DOTNET_RUNTIME_DIRS
    ${DOTNET_RUNTIME_PATH}/packs/Microsoft.NETCore.App.Host.*/*/runtimes/*/native/
    ${DOTNET_RUNTIME_PATH}/shared/Microsoft.NETCore.App/*/
    ${DOTNET_RUNTIME_PATH}/host/fxr/*/)
set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:${DOTNET_RUNTIME_DIRS}")


# Add the source files and header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "*.h")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# Create the target library
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Yurrgoht")
target_include_directories(${TARGET_NAME} PRIVATE 
    "${yaml-cpp_SOURCE_DIR}/include"
    ${spdlog_SOURCE_DIR}/include
    ${DOTNET_HEADERS}
)

# linking dotnet core libraries
target_link_libraries(${TARGET_NAME} PRIVATE ${DOTNET_RUNTIME_LIBS})


# Link statically to Vulkan and other libraries
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARY})
target_link_libraries(${TARGET_NAME} PUBLIC SDL3)
target_link_libraries(${TARGET_NAME} PUBLIC SDL3_image)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC yaml-cpp)
target_link_libraries(${TARGET_NAME} PUBLIC stdc++)
target_link_libraries(${TARGET_NAME} PUBLIC ktx)
target_link_libraries(${TARGET_NAME} PUBLIC meta.hpp::meta.hpp)
target_link_libraries(${TARGET_NAME} PRIVATE Jolt)
target_link_directories(${TARGET_NAME} PUBLIC /usr/lib/x86_64-linux-gnu)

# Include directories
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/source)
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/external)
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/shader/include)
target_compile_definitions(${TARGET_NAME} PRIVATE VULKAN_SHADER_COMPILER=\"${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}\")

# Installation
set(INSTALL_BIN "bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>")
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_BIN})
install(TARGETS imgui DESTINATION ${INSTALL_BIN})
#install(TARGETS Jolt DESTINATION ${INSTALL_BIN})
install(FILES ${Vulkan_LIBRARY} DESTINATION ${INSTALL_BIN})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/" DESTINATION "include/engine" FILES_MATCHING PATTERN "*.h")
