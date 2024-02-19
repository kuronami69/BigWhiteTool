#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H

#include "Android_draw/DrawFun.h"
#if defined(USE_OPENGL)
    #include "ImGui/backends/imgui_impl_opengl3.h"
    //#include "OpenglUtils.h"
#else
    #include "ImGui/backends/imgui_impl_vulkan.h"
    #include "Android_vulkan/VulkanUtils.h"
#endif



// namespace
using namespace std;
using namespace std::chrono_literals;
//extern EGLDisplay display;
//extern EGLConfig config;
//extern EGLSurface surface;
//extern EGLContext context;

// 屏幕信息
extern android::ANativeWindowCreator::DisplayInfo displayInfo;
// 绝对屏幕X _ Y
extern int abs_ScreenX, abs_ScreenY;
extern bool g_Initialized;


extern ImGuiWindow *g_window;
// window创建的 大小
extern int native_window_screen_x, native_window_screen_y;

// Func
bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log = false);

bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log = false);

bool ImGui_init();

void screen_config();

void drawBegin();

void drawEnd();

void shutdown();
// 布局UI
void Layout_tick_UI();



#endif //NATIVESURFACE_DRAW_H
