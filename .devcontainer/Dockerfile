FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /workspaces


# Install necessary build tools and libraries
RUN dnf install -y \
    mesa-vulkan-drivers vulkan-tools vulkan-loader-devel vulkan-validation-layers glslang glslc spirv-tools \
    assimp-devel glm-devel cereal-devel SDL2-devel SDL2_image-devel yaml-cpp-devel spdlog-devel \
    git wget clang cmake ninja-build doxygen gdb\
# for ktx software
    freeglut-devel libXi-devel libXmu-devel mesa-libGL-devel mesa-libGLU-devel freeimage-devel \    
    graphviz xinput libXinerama 
RUN dnf config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo && \
    dnf install -y docker


# vulkan memory allocator dependency - static
RUN wget https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.1.0.tar.gz && \
    tar -xzvf v3.1.0.tar.gz && \
    cd VulkanMemoryAllocator-3.1.0 && \
    cmake -S . -B build -DBUILD_SHARED_LIBS=OFF && \
    cd build && \
    cmake --install .

# KTX software dependency (ktx textures 2) - static
RUN wget https://github.com/KhronosGroup/KTX-Software/archive/refs/tags/v4.3.2.tar.gz && \
    tar -xzvf v4.3.2.tar.gz && \
    cd KTX-Software-4.3.2 && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -DKTX_FEATURE_TESTS=OFF -DKTX_FEATURE_STATIC_LIBRARY=ON .. && \
    make -j && \
    make install

# eventpp dependency - static
RUN wget https://github.com/wqking/eventpp/archive/refs/tags/v0.1.3.tar.gz && \
    tar -xzvf v0.1.3.tar.gz && \
    cd eventpp-0.1.3 && mkdir build && cd build && \
    cmake .. -G Ninja -B . -DBUILD_SHARED_LIBS=OFF && \
    cmake --install .
    
# rttr dependency - static - manually had to set install location and then copy header files b/c rttr does not do so for static for some reason
RUN git clone https://github.com/rttrorg/rttr.git && \
    cd rttr && mkdir build && cd build && \
    cmake .. -G Ninja -B . -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_STANDARD=20 -DBUILD_RTTR_DYNAMIC=OFF -DBUILD_UNIT_TESTS=OFF -DBUILD_STATIC=ON \
    -DBUILD_WITH_STATIC_RUNTIME_LIBS=ON -DBUILD_EXAMPLES=OFF -DBUILD_DOCUMENTATION=OFF -DBUILD_INSTALLER=ON -DBUILD_PACKAGE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build . && \
    cmake --install . && \
    cd ../src/rttr && \
    find . -name "*.h" -exec install -D {} /usr/local/include/rttr/{} \;
    
    
# Download and extract the LunarG Vulkan SDK
ENV VULKAN_SDK_VERSION=1.3.296.0
RUN cd /tmp && \
    curl -L -O https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.xz && \
    tar xJf vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.xz && \
    rm vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.xz && \
    mv ${VULKAN_SDK_VERSION} /opt/vulkansdk-${VULKAN_SDK_VERSION}
    
# Setup Vulkan SDK environment variables
ENV VULKAN_SDK="/opt/vulkansdk-${VULKAN_SDK_VERSION}/x86_64"
ENV PATH="${VULKAN_SDK}/bin:$PATH"
ENV LD_LIBRARY_PATH="${VULKAN_SDK}/lib:$LD_LIBRARY_PATH"
ENV VK_LAYER_PATH="${VULKAN_SDK}/share/vulkan/explicit_layer.d"
ENV GFXRECON_LOG_LEVEL=WARN

    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]