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
    yaml-cpp yaml-cpp-devel yaml-cpp-static \
    wget cmake make ninja-build xz clang


# vulkan memory allocator dependency
RUN wget https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.1.0.tar.gz && \
    tar -xzvf v3.1.0.tar.gz && \
    cd VulkanMemoryAllocator-3.1.0 && \
    cmake -S . -B build && \
    cmake --install build --prefix build/install

# KTX software dependency (ktx textures 2)
RUN wget https://github.com/KhronosGroup/KTX-Software/releases/download/v4.3.2/KTX-Software-4.3.2-Linux-x86_64.tar.bz2 && \
    tar -xjvf KTX-Software-4.3.2-Linux-x86_64.tar.bz2

# JoltPhysics dependency
RUN wget https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.2.0.tar.gz && \
    tar -xzvf v5.2.0.tar.gz && \
    cd JoltPhysics-5.2.0/Build && \ 
    ./cmake_linux_clang_gcc.sh && \
    cd Linux_Debug && \
    make -j $(nproc) && \
    ./UnitTests && \
    cmake --install .

# spdlog dependency - these 2 lines will always cause a problem for some reason that I cannot figure out
RUN sed -i 's/#    include <spdlog\/fmt\/bundled\/core.h>/#    include <fmt\/core.h>/' /usr/include/spdlog/fmt/fmt.h && \
    sed -i 's/#    include <spdlog\/fmt\/bundled\/format.h>/#    include <fmt\/format.h>/' /usr/include/spdlog/fmt/fmt.h

# Default command: Start bash to interact with the container
CMD ["/bin/bash"]

# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz
