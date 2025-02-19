FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /workspaces


# Install necessary build tools and libraries
RUN dnf install -y \
    mesa-vulkan-drivers vulkan-tools vulkan-loader-devel vulkan-validation-layers glslang glslc spirv-tools \
    assimp-devel glm-devel cereal-devel yaml-cpp-devel spdlog-devel \
    git wget clang cmake ninja-build doxygen gdb p7zip p7zip-plugins \
# for ktx software
    freeglut-devel libXi-devel libXmu-devel mesa-libGL-devel mesa-libGLU-devel freeimage-devel \    
    graphviz xinput libXinerama \
# dotnet dependencies
    krb5-libs libicu openssl-libs zlib
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
RUN git clone --branch v4.3.2 --depth 1 https://github.com/KhronosGroup/KTX-Software.git && \
    cd KTX-Software && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -DKTX_FEATURE_TESTS=OFF -DKTX_FEATURE_STATIC_LIBRARY=ON .. && \
    make -j && \
    make install

# eventpp dependency - static
RUN wget https://github.com/wqking/eventpp/archive/refs/tags/v0.1.3.tar.gz && \
    tar -xzvf v0.1.3.tar.gz && \
    cd eventpp-0.1.3 && mkdir build && cd build && \
    cmake .. -G Ninja -B . -DBUILD_SHARED_LIBS=OFF && \
    cmake --install .

# SDL3 dependency
RUN wget https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/SDL3-3.2.0.tar.gz && \
    tar -xzvf SDL3-3.2.0.tar.gz && \
    cd SDL3-3.2.0 && mkdir build && \
    cmake -S . -B build -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build && \
    cmake --install build --prefix /usr/local
    
# SDL3/SDL_image dependency
RUN wget https://github.com/libsdl-org/SDL_image/releases/download/release-3.2.0/SDL3_image-3.2.0.tar.gz && \
    tar -xzvf SDL3_image-3.2.0.tar.gz && \
    cd SDL3_image-3.2.0 && mkdir build && \
    cmake -S . -B build -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build && \
    cmake --install build --prefix /usr/local


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
    
# angelscript dependency - experimental NOT latest stable release
#RUN wget https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip && \
RUN wget https://github.com/codecat/angelscript-mirror/archive/refs/heads/master.zip && \
    unzip master.zip && \
    cd angelscript-mirror-master/sdk/angelscript/projects/cmake && mkdir build && \
    cmake -S . -B build -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build && \
    cmake --install build --prefix /usr/local
    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]