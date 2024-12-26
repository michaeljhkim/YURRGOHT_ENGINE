FROM quay.io/pypa/manylinux_2_34_x86_64

WORKDIR /workspaces


# Install necessary build tools and libraries
RUN dnf install -y \
    vulkan-tools vulkan-loader-devel vulkan-validation-layers-devel glslang glslc spirv-tools \
    assimp assimp-devel \
    glm-devel \
    cereal-devel \
    git wget clang cmake make ninja-build \
    mesa-libGL mesa-vulkan-drivers \
    wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules \ 
    libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel libXrender-devel

    
# JoltPhysics dependency
RUN wget https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.2.0.tar.gz && \
    tar -xzvf v5.2.0.tar.gz && \
    cd JoltPhysics-5.2.0/Build && \ 
    cmake -DBUILD_SHARED_LIBS=OFF . && \
    ./cmake_linux_clang_gcc.sh && \
    cd Linux_Debug && \
    make -j $(nproc) && \
    ./UnitTests && \
    cmake --install .

# vulkan memory allocator dependency - static
RUN wget https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.1.0.tar.gz && \
    tar -xzvf v3.1.0.tar.gz && \
    cd VulkanMemoryAllocator-3.1.0 && \
    cmake -S . -B build -DBUILD_SHARED_LIBS=OFF && \
    cd build && \
    cmake --install .

# SDL2 dependency - static
RUN wget https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.10.tar.gz && \
    tar -xzvf release-2.30.10.tar.gz && \
    cd SDL-release-2.30.10 && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    make -j && \
    make install

# glfw3 dependency - static
RUN wget https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz && \
    tar -xzvf 3.4.tar.gz && \
    cd glfw-3.4 && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF .. && \
    make -j && \
    make install

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

# spdlog dependency - static
RUN wget https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz && \
    tar -xzvf v1.15.0.tar.gz && \
    cd spdlog-1.15.0 && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -SPDLOG_FMT_EXTERNAL=OFF .. && \
    make -j && \
    make install

# yaml-cpp dependency - static
RUN wget https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.tar.gz && \
    tar -xzvf 0.8.0.tar.gz && \
    cd yaml-cpp-0.8.0 && mkdir build && cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -DYAML_BUILD_SHARED_LIBS=OFF .. && \
    make -j && \
    make install


# rttr dependency - static
# manually had to set install location and then copy header files b/c rttr does not do so for static for some reason
RUN git clone https://github.com/rttrorg/rttr.git && \
    cd rttr && mkdir build && cd build && \
    cmake .. -G Ninja -B . -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_STANDARD=17 -DBUILD_RTTR_DYNAMIC=OFF -DBUILD_UNIT_TESTS=OFF -DBUILD_STATIC=ON \
    -DBUILD_WITH_STATIC_RUNTIME_LIBS=ON -DBUILD_EXAMPLES=OFF -DBUILD_DOCUMENTATION=OFF -DBUILD_INSTALLER=ON -DBUILD_PACKAGE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build . && \
    cmake --install . && \
    cd ../src/rttr && \
    find . -name "*.h" -exec install -D {} /usr/local/include/rttr/{} \;

    
# imgui dependency - I think I need to do this inside the project, cannot setup beforehand I thinkg
#RUN wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz


# Default command: Start bash to interact with the container
CMD ["/bin/bash"]