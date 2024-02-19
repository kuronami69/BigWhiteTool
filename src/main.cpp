#include "Android_draw/draw.h"

#if defined(USE_OPENGL)
#include "imgui_image.h"
#else
#include "VulkanUtils.h"
#endif

#include "picture_ZhenAiKun_png.h"


#include "My_Utils/http.h"
#include "My_Utils/Encrypt.h"


bool main_thread_flag = true;

int abs_ScreenX = 0;
int abs_ScreenY = 0;



/**
 * imgui测试
 */
#if defined(USE_OPENGL)
TextureInfo op_img;
#else
MyTextureData vk_img;
#endif

int main(int argc, char *argv[]) {
    std::cout << "免责声明：" << std::endl << std::endl;
    std::cout << "此软件是一款用于逆向分析和测试游戏功能的工具，旨在帮助开发者理解游戏的内部工作原理以及进行相关测试。然而，请注意以下几点：" << std::endl << std::endl;

    std::cout << "1. 本软件的使用仅限于合法的研究和测试目的。任何将此软件用于非法活动、侵犯版权或其他违法行为的用户将承担相应的法律责任。" << std::endl;
    std::cout << "2. 本软件可能会影响游戏的正常运行，甚至可能导致游戏崩溃、数据损坏或其他不可预测的问题。使用本软件可能会对游戏的稳定性和功能性产生不利影响。" << std::endl;
    std::cout << "3. 本软件可能会涉及游戏厂商的知识产权和利益，因此使用本软件可能会引起游戏厂商的不满甚至法律纠纷。" << std::endl;
    std::cout << "4. 用户对使用本软件所产生的一切后果和风险负全部责任。软件开发者及其关联方不承担任何直接或间接的责任，包括但不限于因软件使用造成的损失、损害、法律纠纷或其他不良后果。" << std::endl;
    std::cout << "5. 本软件的发布和使用可能受到不同国家和地区的法律法规的限制。用户应当遵守当地的法律法规，确保在合法范围内使用本软件。" << std::endl << std::endl;

    std::cout << "请在使用本软件前详细阅读并理解上述免责声明。一旦您开始使用本软件，即代表您同意遵守上述声明，并愿意承担因此产生的一切风险和责任。" << std::endl;
    std::cout << "对于任何因使用本软件所引起的问题，我们将不负有任何法律或经济责任。" << std::endl << std::endl;
    std::cout << "（https://t.me/BigWhiteChat）" << std::endl;
    execCommand("mkdir -p /storage/emulated/0/A_BigWhiteTool/SDK");
    //获取屏幕信息
    screen_config();
    ::abs_ScreenX = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::abs_ScreenY = (displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width);
    ::native_window_screen_x = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::native_window_screen_y = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    // 初始化imgui
    if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true)) {
        return -1;
    }
    py = displayInfo.height / 2;
    px = displayInfo.width / 2;

    printf("当前程序Pid : %d\n", getpid());
#if defined(USE_OPENGL)
    op_img = createTexture_ALL_FromMem(picture_ZhenAiKun_PNG_H, sizeof(picture_ZhenAiKun_PNG_H));
#else
    LoadTextureFromMemory((const void *)&picture_ZhenAiKun_PNG_H, sizeof(picture_ZhenAiKun_PNG_H), &vk_img);
#endif

    Touch_Init(displayInfo.width, displayInfo.height, displayInfo.orientation, true);

    processes = GetUE4Processes();//获取包含libue4.so的进程


    while (main_thread_flag) {
        // imgui画图开始前调用
        drawBegin(); //
        Layout_tick_UI();
        drawEnd();// imgui画图结束调用
        std::this_thread::sleep_for(1ms);
    }

    shutdown();
    Touch_Close();
    return 0;
}




void Layout_tick_UI() {
    static bool show_demo_window = false;
    static bool show_another_window = false;
    static bool show_finddata_window = false;
    static bool show_Dump_window = false;
    static bool show_pointerTool_window = false;
    {
        static float f = 0.0f;
        static int counter = 0;
        static int style_idx = 1;
        static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
#if defined(USE_OPENGL)
        const static char* name = "(OpenGL)->搞批(Symbol chain)";
#else
        const static char *name = "(Vulkan)->搞批(Symbol chain)";
#endif
        if (ImGui::BeginMainMenuBar())
        {
            // 计算菜单宽度
            ImVec2 menuSize = ImVec2(100, 50); // 菜单的宽度和高度
            float menuBarWidth = ImGui::GetWindowWidth();
            float menuX = (menuBarWidth - menuSize.x) * 0.5f; // 计算居中位置
            ImGui::SetCursorPosX(menuX);

            if (ReadMode == 0 && ImGui::BeginMenu("驱动")) {
                if (ImGui::MenuItem("普通")){
                    ReadMode=1;
                }
                if (ImGui::MenuItem("QX10.6")){
                    ReadMode=2;
                }
                ImGui::EndMenu();
            }

            if (BigWhitePid==0 && ReadMode >0 && ImGui::BeginMenu("进程")) {
                for (const ProcessInfo& process : processes) {
                    std::string jc = "PID: " + process.pid + " | Name: " + process.name;
                    if (ImGui::MenuItem(jc.c_str())) {
                        BigWhitePid = std::stoi(process.pid);//这里给BigWhite_pid赋值 是为了BigWhiteRead里面需要用
                        if (ReadMode==2){
                            BigWhiteinit();
                        }
                        //Gname->75e17530d0   Offset->1c34a0d0
                        LibBase = GetLibBase(BigWhitePid);
                        printf("Pid：%d\nLibBase：%lx\n",BigWhitePid,LibBase);
                        //ProcessName=process.name;//将进程名保存为全局变量
                        //ResetOffsets();//重新选择进程时 重置偏移结构体变量
                        //addr.libbase = GetLibBase(BigWhite_pid);
                        //GameBase=addr.libbase;
                        //printf("Pid：%d\nlibBase：%lx\nGname：%lx\nGobject：%lx\n",BigWhite_pid,addr.libbase,addr.GNames,addr.Gobject);
                        cout << "Init Is OK！"<<endl;
                        //cshzt = true;
                    }
                }
                ImGui::EndMenu();
            }

            if (BigWhitePid>0&&ImGui::BeginMenu("窗口"))
            {
                ImGui::MenuItem("ImguiDemo", NULL, &show_demo_window);
                ImGui::Separator();
                ImGui::MenuItem("初始化数据", NULL, &show_finddata_window);
                ImGui::MenuItem("UE4Dumper", NULL, &show_Dump_window);
                ImGui::MenuItem("指针扫描", NULL, &show_pointerTool_window);

                /*
                ImGui::MenuItem("DebugDumper", NULL, &ShowDebugDumper);
                ImGui::MenuItem("矩阵数据", NULL, &ShowDebugMatrix);*/
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("退出"))
            {
                main_thread_flag = false;
                ImGui::EndMenu();
            }


        }
/*        ImGui::Begin(name, 0);  // Create a window called "Hello, world!" and append into it.
        ImGui::Text("这是一些有用的文本.");               // Display some text (you can use a format strings too)
        if (ImGui::Combo("##主题", &style_idx, "白色主题\0蓝色主题\0紫色主题\0")) {
            switch (style_idx) {
                case 0: ImGui::StyleColorsLight(); break;
                case 1: ImGui::StyleColorsDark(); break;
                case 2: ImGui::StyleColorsClassic(); break;
            }
        }
        ImGui::Checkbox("演示窗口", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::SameLine();
        ImGui::Checkbox("绘制射线", &show_draw_Line);
        ImGui::Checkbox("坤坤窗口", &show_another_window);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit4("取色器", (float *) &clear_color); // Edit 3 floats representing a color
        if (ImGui::Button("Button")) {
            counter++;
        }

        ImGui::SameLine();
        ImGui::Text("计数 = %d", counter);
        ImGui::Text("窗口集中 = %d", ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "应用平均 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("退出gui")) {
            main_thread_flag = false;
        }
        g_window = ImGui::GetCurrentWindow();
        ImGui::End();*/
    }


    if (show_another_window) { // 3. Show another simple window.
        ImGui::Begin("另一个窗口", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("另一个窗口的 爱坤!");
#if defined(USE_OPENGL)
        ImGui::Image((ImTextureID)op_img.textureId, ImVec2(170, 170));
#else
        ImGui::Image((ImTextureID)vk_img.DS, ImVec2(170, 170));
#endif
        if (ImGui::Button("关闭这个坤口")) {
            show_another_window = false;
        }
        ImGui::End();
    }

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    if (show_finddata_window) {
        ShowFindDataWindow();
    }
    if (show_Dump_window) {
        ShowDumpWindow();
    }
    if (show_pointerTool_window) {
        ShowpointerToolWindow();
    }


    g_window = ImGui::GetCurrentWindow();
}
void InputInit(){
    uint64_t Address = 0;
    for (int i = 0; i < sizeof(inputText) / sizeof(inputText[0]); ++i) {
        if (!inputText[i].empty()){
            uint64_t inputValue;
            if (inputText[i].find("BASE")!= std::string::npos){
                inputValue = LibBase;
            }else {
                inputValue = std::stoull(inputText[i], nullptr, 16);// 第三个参数是进制，这里使用十六进制
            }
            if (i==0){
                Address += inputValue;
            }else{
                Address=GetAddr(Address+inputValue);
            }
        }
        //cout << i << endl;
    }
    DumperAddress = Address;
}
void InputClear(){
    // 使用循环将数组内容清空
    for (int i = 0; i < 10; ++i) {
        inputText[i] = "";
    }
}
void ShowDumpWindow(){
    ImGui::Begin("UE4Dump工具");
    // 渲染编辑框
    for (int i = 0; i < OffsetLevel; ++i) {
        ImGui::InputText((i == 0) ? "地址" : (std::to_string(i) + "层偏移").c_str(), inputText[i].data(), inputText[i].size() + 1); // 使用 size() + 1
        // 如果点击了编辑框，则显示虚拟键盘并获取焦点
        if (ImGui::IsItemClicked()) {
            activeEditBoxIndex = i; // 记录活动编辑框的索引
            showVirtualKeyboard = true;
            ImGui::SetKeyboardFocusHere(-1);
        }

        if (i==0){
            ImGui::SameLine();
            if (ImGui::Button("+")){
                OffsetLevel++;
                InputInit();
            }
            ImGui::SameLine();
            if (ImGui::Button("-")){
                if (OffsetLevel>1)   --OffsetLevel;
                inputText[OffsetLevel].clear();
                InputInit();
            }
        }
    }

    if (ImGui::Button("确定")){
        InputInit();
    }

    if (ImGui::Button("Uworld")){
        InputClear();
        OffsetLevel = 2;
        std::stringstream OffsetsUworldStr;
        OffsetsUworldStr << std::hex << OffsetsUworld;
        inputText[0] = "BASE";
        inputText[1] = OffsetsUworldStr.str();
        InputInit();
    }
    ImGui::SameLine();
    if (ImGui::Button("Matrix")){
        InputClear();
        OffsetLevel = 2;
        std::stringstream OffsetsMatrixStr;
        OffsetsMatrixStr << std::hex << OffsetsMatrix;
        inputText[0] = "BASE";
        inputText[1] = OffsetsMatrixStr.str();
        InputInit();
    }
    ImGui::InputText("过滤", DumpFilter.data(), DumpFilter.size());
    if (ImGui::IsItemClicked()) {
        showVirtualKeyboard = true;
        ImGui::SetKeyboardFocusHere(-1);
    }

    // 渲染虚拟键盘
    if (showVirtualKeyboard) {
        RenderVirtualKeyboard();
    }


    ImGui::SetNextWindowSize(ImVec2(1500, 1000), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::BeginTable("split", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {
        std::vector<StructureList> aaa = foreachAddress(DumperAddress);
        int i = 100000;
        for (const auto &data : aaa) {
            ImGui::PushID(i);
            ShowPlaceholderObject(data, i);
            ImGui::PopID();
            //ImGui::Separator();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();

}
void ShowFindDataWindow(){
    ImGui::SetNextWindowSize(ImVec2(800, 800)); // 设置窗口的宽度和高度

    ImGui::Begin("初始化数据");
    static bool isShow = false;
    static char GnameBuffer[0x256];
    static char GObjectBuffer[0x256];
    static char UWorldBuffer[0x256];
    static char GEngineBuffer[0x256];
    static char DebugCanvasBuffer[0x256];
    static char MatrixBuffer[0x256];
    if (BigWhitePid>0){
        ImGui::Checkbox("是否UE423以上版本\n（谨慎选择 选择错会导致卡死！卡十几秒左右是正常的！）",&isUE423);
        ImGui::Text("获取到Gname和Gobject会出现DumpSDK按钮");
        if (ImGui::Button("获取Gname",ImVec2(400,75))){
            AddrOffsets Gname = GetGname();
            OffsetsGname=Gname.Offsets;//设置全局Gname偏移
            //addr.GNames = addr.libbase + Gname.Offsets;//设置全局Gname地址
            NamePoolData = (FNamePool*)(LibBase+OffsetsGname);
            AddrGNames = (LibBase+Gname.Offsets);
            sprintf(GnameBuffer,"Gname->%lx   Offset->%x",Gname.Addr,Gname.Offsets);
            cout << GnameBuffer <<"\n" <<endl;
            isShow= true;
        }
        if (OffsetsGname>0&&ImGui::Button("获取Gobject",ImVec2(400,75))){
            AddrOffsets Gobject = GetGobject();
            OffsetsGobject = Gobject.Offsets;
            ObjObjects = (TUObjectArray*)(LibBase+Gobject.Offsets+0x10);
            AddrGObject = (LibBase+Gobject.Offsets+0x10);
            sprintf(GObjectBuffer,"Gobject->%lx   Offset->%x",Gobject.Addr,Gobject.Offsets);
            cout << GObjectBuffer <<"\n" <<endl;
            isShow= true;
        }
        if (OffsetsGname>0&&ImGui::Button("获取Matrix",ImVec2(400,75))){
            AddrOffsets Matrix = GetMatrix();
            OffsetsMatrix=Matrix.Offsets;//设置全局Matrix偏移
            sprintf(MatrixBuffer,"Matrix->%lx  Offset->%x",Matrix.Addr,Matrix.Offsets);
            cout << MatrixBuffer <<"\n" <<endl;
            isShow= true;
        }
        if (OffsetsGname>0&&ImGui::Button("获取Uworld",ImVec2(400,75))){
            AddrOffsets Uworld = GetUworld();
            OffsetsUworld = Uworld.Offsets;//设置全局Uworld偏移
            sprintf(UWorldBuffer,"Uworld->%lx  Offset->%x",Uworld.Addr,Uworld.Offsets);
            cout << UWorldBuffer <<"\n" <<endl;
            isShow= true;
        }
        if ((OffsetsGname>0&&OffsetsGobject>0) && ImGui::Button("DumpSDK",ImVec2(400,75))){
            Dumper& Dump = Dumper::GetInstance();
            try
            {
                Dump.Init(LibBase, AddrGNames, AddrGObject + 0x10);
            }
            catch (const char* error)
            {
                printf("%s\n", error);
            }
            Dump.Dump();
        }

        if (isShow && ImGui::Button("保存",ImVec2(400,75))){
            FILE* outFile = fopen("/storage/emulated/0/A_BigWhiteTool/Data.txt", "w+");
            if (outFile != NULL) {
                if (outFile) {
                    fprintf(outFile, "%s\n", GnameBuffer);
                    fprintf(outFile, "%s\n", UWorldBuffer);
                    fprintf(outFile, "%s\n", MatrixBuffer);
                    fprintf(outFile, "%s\n", GObjectBuffer);
                    fclose(outFile);
                    cout<<"保存成功路径：/storage/emulated/0/A_BigWhiteTool\n"<<endl;
                } else {
                    printf("Error opening output file\n");
                }
            }else {
                // 处理文件打开失败的情况
                perror("Error opening file");
            }

        }

        if (isShow){
            ImGui::Text("%s",GnameBuffer,ImVec2(400,75));
            ImGui::Text("%s",UWorldBuffer,ImVec2(400,75));
            ImGui::Text("%s",MatrixBuffer,ImVec2(400,75));
            ImGui::Text("%s",GObjectBuffer,ImVec2(400,75));
        }
    } else{
        ImGui::Text("请先选择进程！",ImVec2(400,75));
    }
    ImGui::End();
}
void ShowpointerToolWindow(){
    //77867a99c0 指向 76c66c5b60
    ImGui::Begin("指针工具");
    static char SearchAddr[128] = "76c66c5b60";
    ImGui::InputText("地址", SearchAddr, IM_ARRAYSIZE(SearchAddr));
    if (ImGui::Button("暂未开放")){/*
        uint64_t knownAddress=0x76c66c5b60;
        // 在此处进行指针扫描
        std::vector<uint64_t> foundPointers;
        // 假设扫描的范围是从0到一个大的地址值
        const uint64_t StartAddress = GetProcessBase(BigWhitePid, false);
        const uint64_t EndAddress = GetProcessBase(BigWhitePid, true);
        printf("%lx,%lx",StartAddress,EndAddress);
        cout << "\n"<<endl;
        // 开始扫描内存
        for (uint64_t i = StartAddress; i < EndAddress; ++i) {
            if (GetAddr(i) == knownAddress){
                foundPointers.push_back(i);
            }
        }
        // 输出找到的指针信息
        std::cout << "Pointers found:" << std::endl;
        for (int i = 0; i < foundPointers.size(); ++i) {
            std::cout << "Pointer at address: "<<foundPointers[i] << std::endl;
        }*/
    }

    ImGui::End();
}