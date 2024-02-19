//
// Created by Administrator on 2024/1/23.
//
#include "Android_draw/DrawFun.h"


float matrix[16], angle, camera, r_x, r_y, r_w;
int Myteam=0,px,py;
float xyz[3];//自身坐标
std::vector<PlayerObject> dynamicRows;//对象坐标数组
std::vector<ProcessInfo> processes;//进程
uint64_t DumperAddress=0;
uint64_t LibBase,OffsetsGname,OffsetsGobject,OffsetsUworld,OffsetsMatrix;
Vector2 GGChest;Vector2 Head;Vector2 AimHead; Vector2 Chest; Vector2 Pelvis; Vector2 Left_Shoulder; Vector2 Right_Shoulder;
Vector2 Left_Elbow; Vector2 Right_Elbow;
Vector2 GGD;
Vector2 Left_Wrist; Vector2 Right_Wrist; Vector2 Left_Thigh; Vector2 Right_Thigh;
Vector2 Left_Knee;Vector2 Right_Knee;Vector2 Left_Ankle;Vector2 Right_Ankle;

bool showVirtualKeyboard = false;
std::string inputText[10];
std::string DumpFilter;
int activeEditBoxIndex = -1; // 跟踪活动编辑框的索引
int OffsetLevel= 1;

float intToFloat(int intValue) {
    FloatInt fi{};
    fi.i = intValue;
    return fi.f;
}

void renderBox(Vector2 head_at_screen, float distance_modifier) {
    int head_x = head_at_screen.X;
    int head_y = head_at_screen.Y;
    int start_x = head_x - 35 / distance_modifier;
    int start_y = head_y - 15 / distance_modifier;
    int end_x = head_x + 35 / distance_modifier;
    int end_y = head_y + 155 / distance_modifier;
    ImU32 g_esp_color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0.4F, 1));
    ImGui::GetForegroundDrawList()->AddRect(ImVec2(start_x, start_y), ImVec2(end_x, end_y), g_esp_color);
}
// 读取字符信息
void JSONGetUTF8(UTF8 * buf,UTF16 buf16[16])
{
    UTF16 *pTempUTF16 = buf16;
    UTF8 *pTempUTF8 = buf;
    UTF8 *pUTF8End = pTempUTF8 + 32;
    while (pTempUTF16 < pTempUTF16 + 28)
    {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
        {
            *pTempUTF8++ = (UTF8) * pTempUTF16;
        }
        else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
}
FTransform GetBone(cJSON* jsondata)
{
    FTransform transform;
    if (cJSON_IsArray(jsondata) && cJSON_GetArraySize(jsondata) >= 11) {
        // 解析前四个数字到 struct Quat
        for (int i = 0; i < 4; i++) {
            cJSON* element = cJSON_GetArrayItem(jsondata, i);
            if (cJSON_IsNumber(element)) {
                if (i == 0) transform.Rotation.X = intToFloat(element->valueint);
                else if (i == 1) transform.Rotation.Y = intToFloat(element->valueint);
                else if (i == 2) transform.Rotation.Z = intToFloat(element->valueint);
                else if (i == 3) transform.Rotation.W = intToFloat(element->valueint);

            }
        }
        // 解析接下来四个数字到 struct Vector3A
        for (int i = 4; i < 7; i++) {
            cJSON* element = cJSON_GetArrayItem(jsondata, i);
            if (cJSON_IsNumber(element)) {
                if (i == 4) transform.Translation.X = intToFloat(element->valueint);
                else if (i == 5) transform.Translation.Y = intToFloat(element->valueint);
                else if (i == 6) transform.Translation.Z = intToFloat(element->valueint);
            }
        }
        // 解析最后的三个数字到 struct Vector3A2
        for (int i = 7; i < 10; i++) {
            cJSON* element = cJSON_GetArrayItem(jsondata, i);
            if (cJSON_IsNumber(element)) {
                if (i == 7) transform.Scale3D.X = intToFloat(element->valueint);
                else if (i == 8) transform.Scale3D.Y = intToFloat(element->valueint);
                else if (i == 9) transform.Scale3D.Z = intToFloat(element->valueint);
            }
        }
    }
    return transform;
}

FMatrix TransformToMatrix(FTransform transform)
{
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.X;
    matrix.M[3][1] = transform.Translation.Y;
    matrix.M[3][2] = transform.Translation.Z;
    float x2 = transform.Rotation.X + transform.Rotation.X;
    float y2 = transform.Rotation.Y + transform.Rotation.Y;
    float z2 = transform.Rotation.Z + transform.Rotation.Z;
    float xx2 = transform.Rotation.X * x2;
    float yy2 = transform.Rotation.Y * y2;
    float zz2 = transform.Rotation.Z * z2;
    matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.X;
    matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.Y;
    matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.Z;
    float yz2 = transform.Rotation.Y * z2;
    float wx2 = transform.Rotation.W * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.Z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.Y;
    float xy2 = transform.Rotation.X * y2;
    float wz2 = transform.Rotation.W * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.Y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.X;
    float xz2 = transform.Rotation.X * z2;
    float wy2 = transform.Rotation.W * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.Z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.X;
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
}
FMatrix MatrixMulti(FMatrix m1, FMatrix m2)
{
    FMatrix matrix = FMatrix();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
            }
        }
    }
    return matrix;
}

Vector3 MarixToVector(FMatrix matrix)
{
    return Vector3(matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]);
}

Vector2 WorldToScreen(Vector3 obj, float matrix[16], float ViewW)
{
    float x =
            px + (matrix[0] * obj.X + matrix[4] * obj.Y + matrix[8] * obj.Z + matrix[12]) / ViewW * px;
    float y =
            py - (matrix[1] * obj.X + matrix[5] * obj.Y + matrix[9] * obj.Z + matrix[13]) / ViewW * py;

    return Vector2(x, y);
}

//获取包含LibUE4.so的进程
std::vector<ProcessInfo> GetUE4Processes() {
    std::vector<ProcessInfo> process;
    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string pid = entry.path().filename().string();
            std::string cmdlineFilePath = entry.path() / "cmdline";

            std::ifstream cmdlineFile(cmdlineFilePath, std::ios::binary);
            if (cmdlineFile.is_open()) {
                std::string processName;
                std::getline(cmdlineFile, processName, '\0');
                cmdlineFile.close();

                // 先进行初步过滤，检查processName是否包含com.
                std::string mapsFilePath = entry.path() / "maps";
                std::ifstream mapsFile(mapsFilePath);
                std::string line;
                bool hasLibUE4 = false;
                while (std::getline(mapsFile, line)) {
                    if (line.find("libUE4.so") != std::string::npos) {
                        hasLibUE4 = true;
                        break;
                    }
                }
                mapsFile.close();

                if (hasLibUE4) {
                    ProcessInfo processInfo;
                    processInfo.pid = pid;
                    processInfo.name = processName;
                    processInfo.isSelected = false;
                    process.push_back(processInfo);
                }

            }
        }
    }

    return process;
}

char* execCommand(const char* shell){

    FILE *fp = popen(shell, "r");

    if (fp == NULL)
    {
        perror("popen failed");
        return NULL;
    }

    char buffer[256];
    char *result = (char *)malloc(1000); // allocate memory for the result string
    result[0] = '\0';                  // initialize as an empty string

    // Read and append output of the first command to result
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        strcat(result, buffer);
    }
    pclose(fp);
    return result;
}

AddrOffsets GetGname(){
    AddrOffsets addrOffsets;
    addrOffsets.Addr=0;
    addrOffsets.Offsets=0;
    for (int i = 20000000;; i++) {
        uint64_t TMP;
        if (isUE423){
            TMP = LibBase + (0x8*i) + 0x40;
            if (TMP != 0){
                uint64_t TMPGnames = GetAddr(TMP);
                char name[0x100];
                ReadAddr(TMPGnames+0x8, name, 0xc);
                std::string aa = name;
                if (aa.find("ByteProperty") != std::string::npos){
                    addrOffsets.Addr=LibBase+(0x8*i);
                    addrOffsets.Offsets=(0x8*i);
                    break;
                }
            }
        } else{
            TMP = LibBase + (0x8*i);

            if (TMP < 0x1000000000) continue;
            uint64_t TMP1 = GetAddr(TMP);
            if (TMP1 < 0x1000000000) continue;
            uint64_t TMP2 = GetAddr(TMP1);
            if (TMP2 < 0x1000000000) continue;
            uint64_t TMP3 = GetAddr(TMP2);
            if (TMP3 < 0x1000000000) continue;
            uint64_t TMPGnames = TMP3;
            char name[0x100];
            ReadAddr(TMPGnames+0x24, name, 0xc);
            std::string aa = name;
            if (aa.find("ByteProperty") != std::string::npos){
                addrOffsets.Addr=LibBase+(0x8*i);
                addrOffsets.Offsets=(0x8*i);
                break;
            }
        }
    }
    return addrOffsets;
}

AddrOffsets GetGobject(){
    AddrOffsets addrOffsets{};
    addrOffsets.Addr=0;
    addrOffsets.Offsets=0;
    for (int i = 0;; i++) {
        uint64_t TMP;
        TMP = LibBase + OffsetsGname + (0x8*i) +0x10;
        if (TMP < 0x1000000000) continue;
        uint64_t TMP1 = GetAddr(TMP);
        if (TMP1 < 0x1000000000) continue;
        uint64_t TMP2 = GetAddr(TMP1);
        if (TMP2 < 0x1000000000) continue;
        auto* TMP3 = XY_TRead<UE_UObject*>(TMP2);
        auto* TMP4 = XY_TRead<UE_UObject*>(TMP2+0x18);
        string TMP3Class = TMP3->GetClass()->GetName();
        string TMP3Outer = TMP3->GetName();
        string TMP4Class = TMP4->GetClass()->GetName();
        string TMP4Outer = TMP3->GetName();
        if (TMP3Class.find("Package") != std::string::npos && TMP3Outer.find("/Script/CoreUObject") != std::string::npos ){
            if (TMP4Class.find("Class") != std::string::npos && TMP4Outer.find("Object") != std::string::npos ){
                addrOffsets.Addr=TMP-0x10;
                addrOffsets.Offsets=OffsetsGname + (0x8*i);
                break;
            }
        }
    }
    return addrOffsets;
}

AddrOffsets GetMatrix(){
    AddrOffsets addrOffsets;
    addrOffsets.Addr=0;
    addrOffsets.Offsets=0;
    int i=0;

    while (true){
        uint64_t TMPMatrix = GetAddr(LibBase + OffsetsGname + (0x8*i));
        if (TMPMatrix != 0){
            UE_UObject* Tmp = XY_TRead<UE_UObject*>((TMPMatrix)+0x20);
            if (Tmp->GetClass()->GetName()== "Canvas"){
                addrOffsets.Addr=TMPMatrix;
                addrOffsets.Offsets=OffsetsGname + (0x8*i);
                break;
            }
        }
        i++;
    }
    return addrOffsets;
}
AddrOffsets GetUworld(){
    AddrOffsets addrOffsets;
    addrOffsets.Addr=0;
    addrOffsets.Offsets=0;
    for (int i = 0;; i++) {
        uint64_t TMP;
        TMP = LibBase + OffsetsGname + (i*0x8);
        UE_UObject* TMP1 = XY_TRead<UE_UObject*>(TMP);
        string TMP1Class = TMP1->GetClass()->GetName();
        if (TMP1Class=="World"){
            addrOffsets.Addr=TMP;
            addrOffsets.Offsets=OffsetsGname + (i*0x8);
            break;
        }
    }
    return addrOffsets;
}

void RenderVirtualKeyboard() {
    ImGui::Begin("虚拟键盘");

    static const char keyboardChars[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";

    for (int i = 0; i < strlen(keyboardChars); ++i) {
        if (i % 10 != 0) {
            ImGui::SameLine();
        }

        std::string a(1, keyboardChars[i]);
        if (ImGui::Button(a.c_str(), ImVec2(80, 80))) {
            if (activeEditBoxIndex >= 0 && activeEditBoxIndex < 10) {
                char c[2] = { keyboardChars[i], '\0' };
                inputText[activeEditBoxIndex] += c;
            }
        }
    }

    if (ImGui::Button("Back", ImVec2(80, 80))) {
        if (activeEditBoxIndex >= 0 && activeEditBoxIndex < 10 && !inputText[activeEditBoxIndex].empty()) {
            inputText[activeEditBoxIndex].pop_back();
        }
    }

    if (ImGui::Button("关闭键盘", ImVec2(100, 80))) {
        showVirtualKeyboard = false;
    }

    ImGui::End();
}

vector<StructureList> foreachAddress(uint64_t Address) {
    std::vector<StructureList> structureList; // 使用std::vector存储输出内容
    for (size_t i = 0; i < 0x1500; i+=4) {
        UE_UClass* Tmp = XY_TRead<UE_UClass*>(Address + i);
        string KlassName = "Class:"+Tmp->GetClass()->GetName();
        string outerName = "Name:"+Tmp->GetName();
//        string trans = ItemData::UamoGetString(KlassName);
        string trans = "";

        StructureList data;
        data.address = (Address + i);
        data.type = KlassName;
        data.name = outerName;
        data.trans = trans;
        data.offset=i;

        data.P = GetAddr(data.address);
        data.F= GetFloat(data.address);
        data.D= GetDword(data.address);
//        if (outerName.find("None") && KlassName.find("null") && getPtr64(data.address)==0)

        structureList.push_back(data);

    }
    return structureList;
}


// 显示节点信息
void ShowPlaceholderObject(const StructureList& data, int uid) {
    // 使用对象的地址作为标识符，确保唯一性。
    ImGui::PushID(data.address);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    char formattedData[1000000]; // 足够大的缓冲区来存储格式化后的数据
    char DumpFilterStr[1000000]; // 足够大的缓冲区来存储格式化后的数据

    if (DumpFilter.empty()) {
        sprintf(DumpFilterStr, "%x%lx%s%s%f%d%lx",data.offset,  data.address,data.type.c_str(), data.name.c_str(), data.F,data.D,data.P);
        if (strstr(DumpFilterStr, DumpFilter.c_str()) == nullptr){
            ImGui::PopID();
            return;
        }
    }

    if (data.P<0xffffffffff && data.P>0x1000000000){
        sprintf(formattedData, "(%x)—%lx—P->%lx-%s———%s %s",data.offset,  data.address, data.P, data.type.c_str(), data.name.c_str(),data.trans.c_str());
        if (ImGui::TreeNodeEx("Tree", ImGuiTreeNodeFlags_DefaultOpen, "%s", formattedData)) {
            ImGui::TableSetColumnIndex(0);
            if (ImGui::IsItemClicked()) {
                OffsetLevel++;
                static float placeholder_members[8] = {0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f};
                std::stringstream OffsetsTmpStr;
                OffsetsTmpStr << std::hex << data.offset;
                inputText[OffsetLevel-1] = OffsetsTmpStr.str();
                InputInit();
/*            std::vector<StructureList> aaa = foreachAddress(data.P);
            int i = 0;
            for (const auto &item: aaa) {
                ImGui::PushID(i); // Use field index as identifier.
                ShowPlaceholderObject(item, i);
                ImGui::PopID();
                i++;
            }*/
            }
            ImGui::TreePop();
        }
    }
    else if (data.F>0.001f){
        sprintf(formattedData, "(%x)—%lx—Class:%s————————Name:%s————————F->%f",
                data.offset,  data.address,data.type.c_str(), data.name.c_str(), data.F);
        ImGui::Text("      %s", formattedData);
    } else if (data.D>0){
        sprintf(formattedData, "(%x)—%lx—Class:%s————————Name:%s————————D->%d",
                data.offset,  data.address,data.type.c_str(), data.name.c_str(),data.D);
        ImGui::Text("      %s", formattedData);
    } else{
        sprintf(formattedData, "(%x)—%lx—Class:%s————————Name:%s————————F->%f D->%d",
                data.offset,  data.address,data.type.c_str(), data.name.c_str(), data.F, data.D);
        ImGui::Text("      %s", formattedData);
    }



    ImGui::PopID();
}


bool isValidHex(const std::string& str) {
    // 正则表达式用于匹配十六进制字符串
    std::regex hexRegex("^[0-9a-fA-F]+$");
    // 使用 regex_match 检查字符串是否符合正则表达式
    return std::regex_match(str, hexRegex);
}