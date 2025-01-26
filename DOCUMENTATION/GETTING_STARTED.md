NOTE: I have never done documentation before, so this may or may not be coherent


# YURRGOHT ENGINE

## What is Yurrgoht?
**Yurrgoht** is a lightweight game engine with high-fidelity graphics rendering and simulation functions, provides a flexible gameplay framework. Yurrgoht uses vulkan as the only graphics/computing api, linux based, and is written in C++17 (may change to C++20 in the future). Dockerfile (and vscode dev conatainer configs) are included for easy replication of development environment.

- This project was forked from the Bamboo game engine project by RobotBerry https://github.com/Code-Guy/Bamboo
- Assets for testing: https://github.com/SaschaWillems/Vulkan

## Why Yurrgoht Engine? Why make it?
THIS PROJECT IS NOT DESIGNED FOR PUBLIC USE YET - 
With that being said, this engine developed solely with the intention of being as light as possible. I have found that nearly all widely used game engines are severly bloated and I was paranoid of bugs/unnesscary overhead. With this, I would at least have full control over everything, and keep everything as LIGHT as possible. This game engine is not aiming for the most advanced graphics anyways, the most I'm hoping for in the near future is something that looks like source 1

## External libraries
All external libraries' source codes have been included in the dockerfile, or fetched with cmake fetch module, or with git submodule. 
<br>imgui    - submodule
<br>ImGuizmo - submodule
<br>meta.hpp - submodule
<br>jolt     - cmake fetch module
<br>yaml-cpp - cmake fetch module
<br>spdlog   - cmake fetch module

everything else is installed through docker

- [imgui](https://www.dearimgui.com/)(gui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)(gizmos)
- [meta.hpp](https://github.com/BlackMATov/meta.hpp)(reflection)
- [jolt](https://github.com/jrouwe/JoltPhysics)(physics system)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)(config)
- [spdlog](https://github.com/gabime/spdlog)(logging)

- [vma](https://gpuopen.com/vulkan-memory-allocator/)(vulkan resource management)
- [glm](https://glm.g-truc.net/0.9.9/)(math)
- [sdl2](https://www.libsdl.org/)(window and input)
- [tinygltf](https://github.com/syoyo/tinygltf)(model loading)
- [ktx](https://github.com/KhronosGroup/KTX-Software)(texture compression and loading)
- [cereal](https://uscilab.github.io/cereal/)(serialization)
- [json](https://www.json.org/json-en.html)(required by cereal)
- [eventpp](https://github.com/wqking/eventpp)(event system)

NOTE: imgui shader code has been modified to accomodate sRGB and not linear coloring, so you will have to modify that in imgui/backends/imgui_impl_vulkan.cpp . Shader code is provided in the "external" folder. 
<br/><br/>


## BUILDING (and running)

- Note-to-self: we will have to go over the assets quite thoroughly, so I need to figure out how to address that 
- Must explain how asset loading works, in case there are issues (serialization)

```shell
apt-get install -y vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools libassimp-dev libglm-dev libcereal-dev libsdl2-dev libglfw3-dev git wget clang cmake ninja-build doxygen zlib1g-dev graphviz mesa-utils libgl1-mesa-dev mesa-vulkan-drivers
```

```shell
docker build -t yurrgoht/image:1.0 .
docker run -v /host/path:/workspace yurrgoht/image:1.0

cmake -G Ninja -S . -B build
cmake --build build --config Release

cd build/source
./yurrgoht_editor
```

Remember to copy the "asset" and "config" folders from the "default_assets" folder to the same directory as the executable