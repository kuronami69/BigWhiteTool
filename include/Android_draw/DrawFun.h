//
// Created by Administrator on 2024/1/23.
//

#ifndef ANDROIDIMGUI_DRAWFUN_H
#define ANDROIDIMGUI_DRAWFUN_H
// System libs
#include <iostream>
#include <thread>
#include <chrono>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

// User libs
#include "native_surface/ANativeWindowCreator.h"
#include "ImGui/imgui.h"
#include "ImGui/font/Font.h"
#include "ImGui/imgui_internal.h"
#include "My_Utils/cJSON.h"
#include "BigWhiteRead.h"
#include "Android_dump/generic.h"
#include "Android_dump/dumper.h"
#include "Android_dump/engine.h"
//#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "Android_touch/TouchHelperA.h"
#include "Android_draw/Struct.h"

#endif //ANDROIDIMGUI_DRAWFUN_H



extern float matrix[16], angle, camera, r_x, r_y, r_w;
extern int Myteam,px,py;
extern float xyz[3];

extern Vector2 GGChest, Head, AimHead, Chest, Pelvis, Left_Shoulder, Right_Shoulder;
extern Vector2 Left_Elbow, Right_Elbow;
extern Vector2 GGD;
extern Vector2 Left_Wrist, Right_Wrist, Left_Thigh, Right_Thigh;
extern Vector2 Left_Knee, Right_Knee, Left_Ankle, Right_Ankle;

extern std::vector<PlayerObject> dynamicRows;//对象坐标数组
extern std::vector<ProcessInfo> processes;//进程
extern uint64_t LibBase,OffsetsGname,OffsetsGobject,OffsetsUworld,OffsetsMatrix;
extern uint64_t DumperAddress;

extern bool showVirtualKeyboard;
extern std::string inputText[10];
extern std::string DumpFilter;
extern int activeEditBoxIndex; // 跟踪活动编辑框的索引
extern int OffsetLevel;
float intToFloat(int intValue);
void renderBox(Vector2 head_at_screen, float distance_modifier);
void JSONGetUTF8(UTF8 * buf,UTF16 buf16[16]);
FTransform GetBone(cJSON* jsondata);
FMatrix TransformToMatrix(FTransform transform);
FMatrix MatrixMulti(FMatrix m1, FMatrix m2);
Vector3 MarixToVector(FMatrix matrix);
Vector2 WorldToScreen(Vector3 obj, float matrix[16], float ViewW);
std::vector<ProcessInfo> GetUE4Processes();

char* execCommand(const char* shell);

void ShowFindDataWindow();
void ShowpointerToolWindow();
void ShowDumpWindow();
void RenderVirtualKeyboard();//偏移虚拟键盘
void InputInit();
void InputClear();
bool isValidHex(const std::string& str);
vector<StructureList> foreachAddress(uint64_t Address);
void ShowPlaceholderObject(const StructureList& data, int uid);
AddrOffsets GetGname();
AddrOffsets GetGobject();
AddrOffsets GetMatrix();
AddrOffsets GetUworld();