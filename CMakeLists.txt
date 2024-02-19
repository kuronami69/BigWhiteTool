cmake_minimum_required(VERSION 3.21)
# 设置NDK路径
set(NDK_PATH D:/Path/android-ndk-r26b)
##################### Android设置 #####################
set(CMAKE_SYSTEM_NAME ANDROID) # 设置目标编译平台参数 Android
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_SYSTEM_VERSION 21) # 系统版本
set(ANDROID_PLATFORM 21) # 平台版本
set(ANDROID_SDK_TOOLS_VERSION 20) # 平台版本
set(ANDROID_ABI arm64-v8a) # 设置目标构架 armeabi-v7a arm64-v8a x86 x86_64
set(ANDROID_NDK ${NDK_PATH}) # 设置ndk路径
set(CMAKE_TOOLCHAIN_FILE ${NDK_PATH}/build/cmake/android.toolchain.cmake)
set(ANDROID_SDK_ROOT ${NDK_PATH})


project(BigWhiteHack)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
# 使用opengl绘制，否则使用vulkan绘制 (1=opengl)(0=vulkan)
option(OPENGL_DRAW "Use OpenGL for drawing" OFF)

if(OPENGL_DRAW)
        set(MODULE_NAME imgui_chain_1_3_op)
        add_definitions(-DUSE_OPENGL)
else()
        set(MODULE_NAME imgui_chain_1_3_vk)
        add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
endif()
##################### 输出文件重定向 #####################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/outputs/${CMAKE_ANDROID_ARCH_ABI}/
        ) # 重定向输出产物(可执行文件)
##################### 输出文件重定向 #####################


# 引入头文件到全局
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/My_Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Android_vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Android_Read
        ${CMAKE_CURRENT_SOURCE_DIR}include/Android_dump
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGui/backends
)
##################### CMake头文件设置 #####################

##################### CMake源文件设置 #####################
FILE(GLOB_RECURSE FILE_SOURCES # 遍历子目录下所有符合情况的源文件
        src/*.c*
        )
##################### CMake源文件设置 #####################



##################### 设置三方库文件目录 #####################
#add_subdirectory(DumperSDK)

##################### 添加产物 #####################
add_executable(BigWhiteHack
        src/main.cpp
        src/Android_draw/draw.cpp
        src/Android_draw/DrawFun.cpp
        src/Android_touch/TouchHelperA.cpp
        src/ImGui/imgui.cpp
        src/ImGui/imgui_demo.cpp
        src/ImGui/imgui_draw.cpp
        src/ImGui/imgui_tables.cpp
        src/ImGui/imgui_widgets.cpp
        src/ImGui/backends/imgui_impl_android.cpp
        src/My_Utils/stb_image.cpp
        src/My_Utils/cJSON.c
        src/Android_Read/BigWhiteRead.cpp
        src/Android_dump/dumper.cpp
        src/Android_dump/engine.cpp
        src/Android_dump/generic.cpp
)

if(OPENGL_DRAW)
        target_sources(BigWhiteHack PRIVATE
                src/ImGui/backends/imgui_impl_opengl3.cpp
                src/My_Utils/imgui_image.cpp
        )
else()
        target_sources(BigWhiteHack PRIVATE
                src/ImGui/backends/imgui_impl_vulkan.cpp
                src/Android_vulkan/vulkan_wrapper.cpp
                src/Android_vulkan/VulkanUtils.cpp
        )
endif()
##################### 连接库文件 #####################
# 可以整合第三方库 需要打开注释即可
target_link_libraries(BigWhiteHack
        log
        android
        EGL
        GLESv3
)
