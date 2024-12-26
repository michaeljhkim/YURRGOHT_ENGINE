set(TARGET_NAME Engine)

# Search for libraries
find_package(Vulkan REQUIRED)
find_package(Ktx REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Jolt REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(rttr REQUIRED)

# Add the source files and header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "*.h")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# Create the target library
add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Yurrgoht")

# Link statically to Vulkan and other libraries
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARY})
target_link_libraries(${TARGET_NAME} PUBLIC SDL2)
target_link_libraries(${TARGET_NAME} PUBLIC glfw3)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC yaml-cpp)
target_link_libraries(${TARGET_NAME} PUBLIC stdc++)
target_link_libraries(${TARGET_NAME} PUBLIC rttr_core)
target_link_libraries(${TARGET_NAME} PRIVATE KTX::ktx)
target_link_libraries(${TARGET_NAME} PRIVATE Jolt)

# Include directories
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/source)
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/external)
target_include_directories(${TARGET_NAME} PUBLIC ${YURRGOHT_ROOT_DIR}/shader/include)
target_compile_definitions(${TARGET_NAME} PRIVATE VULKAN_SHADER_COMPILER=\"${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}\")

# Installation
set(INSTALL_BIN "bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>")
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_BIN})
install(TARGETS imgui DESTINATION ${INSTALL_BIN})
install(FILES ${Vulkan_LIBRARY} DESTINATION ${INSTALL_BIN})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/" DESTINATION "include/engine" FILES_MATCHING PATTERN "*.h")
