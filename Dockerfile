FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /workspaces


# Install necessary build tools and libraries
RUN dnf install -y \
    vulkan-tools vulkan-loader-devel vulkan-validation-layers-devel glslang glslc spirv-tools \
    assimp assimp-devel \
    SDL2 SDL2-devel \
    glfw glfw-devel \
    cereal-devel \
    glm-devel \
    fmt fmt-devel \
    librttr librttr-devel \
    spdlog spdlog-devel \
    yaml-cpp yaml-cpp-devel yaml-cpp-static
RUN dnf install -y \
    git wget xz clang cmake make automake gcc gcc-c++ kernel-devel libzstd-devel ninja-build doxygen graphviz \
    mesa-libGL mesa-libGL-devel mesa-vulkan-drivers assimp-devel


# KTX software dependency (ktx textures 2)
RUN wget https://github.com/KhronosGroup/KTX-Software/archive/refs/tags/v4.3.2.tar.gz && \
    tar -xzvf v4.3.2.tar.gz && \
    cd KTX-Software-4.3.2 && \ 
    cmake . -G Ninja -B build && \
    cmake --build build && \
    cd build && \
    cmake --install .

# vulkan memory allocator dependency
RUN wget https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.1.0.tar.gz && \
    tar -xzvf v3.1.0.tar.gz && \
    cd VulkanMemoryAllocator-3.1.0 && \
    cmake -S . -B build && \
    cd build && \
    cmake --install .

# JoltPhysics dependency
RUN wget https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.2.0.tar.gz && \
    tar -xzvf v5.2.0.tar.gz && \
    cd JoltPhysics-5.2.0/Build && \ 
    ./cmake_linux_clang_gcc.sh && \
    cd Linux_Debug && \
    make -j $(nproc) && \
    ./UnitTests && \
    cmake --install .
    
# eventpp dependency
RUN wget https://github.com/wqking/eventpp/archive/refs/tags/v0.1.3.tar.gz && \
    tar -xzvf v0.1.3.tar.gz && \
    cd eventpp-0.1.3 && \
    mkdir build && \
    cmake . -G Ninja -B build && \
    cd build && \
    cmake --install .

    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz

# spdlog dependency - these 2 lines will always cause a problem for some reason that I cannot figure out
RUN sed -i 's/#    include <spdlog\/fmt\/bundled\/core.h>/#    include <fmt\/core.h>/' /usr/include/spdlog/fmt/fmt.h && \
    sed -i 's/#    include <spdlog\/fmt\/bundled\/format.h>/#    include <fmt\/format.h>/' /usr/include/spdlog/fmt/fmt.h


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]