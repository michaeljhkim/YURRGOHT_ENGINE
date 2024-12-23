FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /yurrgoht

# Install necessary build tools and libraries
RUN dnf install -y \
    git \
    vulkan-tools vulkan-loader-devel vulkan-validation-layers-devel glslang glslc spirv-tools \
    SDL2 SDL2-devel \
    glfw glfw-devel \
    cereal-devel \
    glm-devel \
    fmt fmt-devel \
    librttr librttr-devel \
    spdlog spdlog-devel \
    yaml-cpp yaml-cpp-devel yaml-cpp-static \
    wget cmake make ninja-build xz clang
    

# JoltPhysics dependency
RUN wget https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.2.0.tar.gz && \
    tar -xzvf v5.2.0.tar.gz && \
    cd JoltPhysics-5.2.0/Build && \ 
    ./cmake_linux_clang_gcc.sh && \
    cd Linux_Debug && \
    make -j $(nproc) && \
    ./UnitTests && \
    cmake --install .

# Default command: Start bash to interact with the container
CMD ["/bin/bash"]

# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz
