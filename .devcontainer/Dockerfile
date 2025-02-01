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


# dotnet installation - for consistent filepath with arch package
ENV DOTNET_INSTALL_DIR=/usr/share/dotnet/
RUN wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh && \
    chmod +x ./dotnet-install.sh && \
    ./dotnet-install.sh  --install-dir ${DOTNET_INSTALL_DIR} --version latest && \
    echo "export DOTNET_ROOT=${DOTNET_INSTALL_DIR}" >> /etc/bash.bashrc && \
    echo "export PATH=$PATH:$DOTNET_ROOT:$DOTNET_ROOT/tools" >> /etc/bash.bashrc && \
    source /etc/bash.bashrc
ENV DOTNET_ROOT=${DOTNET_INSTALL_DIR}
ENV PATH="${DOTNET_ROOT}:${DOTNET_ROOT}/tools:${PATH}"
    
# cs-script - tools install
RUN dotnet tool install --global cs-script.cli && \
    dotnet tool install --global cs-syntaxer
#RUN dotnet add package CS-Script --version 4.8.27

# cs-script - release install
RUN mkdir cs-script && cd cs-script && \
    wget https://github.com/oleg-shilo/cs-script/releases/download/v4.8.26.0/cs-script.linux.v4.8.26.0.7z && \
    7z x cs-script.linux.v4.8.26.0.7z && \
    ln -s /usr/local/bin/cs-script/cscs /usr/bin/css && \
    echo 'export CSSCRIPT_DIR='"$(current_dir)" >> /etc/bash.bashrc && \
    echo 'export PATH=$PATH:'"$(current_dir)" >> /etc/bash.bashrc && \
    source /etc/bash.bashrc
    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]