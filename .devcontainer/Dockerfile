FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /workspaces


# Install necessary build tools and libraries
RUN dnf install -y \
    mesa-vulkan-drivers vulkan-tools vulkan-loader-devel vulkan-validation-layers glslang glslc spirv-tools \
    assimp-devel glm-devel cereal-devel SDL2-devel SDL2_image-devel yaml-cpp-devel spdlog-devel \
    git wget clang cmake ninja-build doxygen gdb p7zip p7zip-plugins \
# for ktx software
    freeglut-devel libXi-devel libXmu-devel mesa-libGL-devel mesa-libGLU-devel freeimage-devel \    
    graphviz xinput libXinerama \
    dotnet-sdk-9.0
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


# cs-script - tools install
RUN dotnet tool install --global cs-script.cli && \
    dotnet tool install --global cs-syntaxer
#RUN dotnet add package CS-Script --version 4.8.27

# cs-script - release install
RUN mkdir cs-script && cd cs-script && \
    wget https://github.com/oleg-shilo/cs-script/releases/download/v4.8.26.0/cs-script.linux.v4.8.26.0.7z && \
    7z x cs-script.linux.v4.8.26.0.7z && \
    ln -s /usr/local/bin/cs-script/cscs /usr/bin/css && \
    export CSSCRIPT_DIR=$(current_dir) && \
    export PATH=$PATH:$(current_dir) && \
    echo 'export CSSCRIPT_DIR='"$(current_dir)" >> ~/.bashrc && \
    echo 'export PATH=$PATH:'"$(current_dir)" >> ~/.bashrc
    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]