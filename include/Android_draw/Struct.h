//
// Created by Administrator on 2024/1/23.
//

#ifndef ANDROIDIMGUI_STRUCT_H
#define ANDROIDIMGUI_STRUCT_H
union FloatInt {
    int i;
    float f;
};

struct Vector2
{
    float X;
    float Y;

    Vector2()
    {
        this->X = 0;
        this->Y = 0;
    }

    Vector2(float x, float y)
    {
        this->X = x;
        this->Y = y;
    }
};

struct Vector3
{
    float X;
    float Y;
    float Z;

    Vector3()
    {
        this->X = 0;
        this->Y = 0;
        this->Z = 0;
    }

    Vector3(float x, float y, float z)
    {
        this->X = x;
        this->Y = y;
        this->Z = z;
    }

};

struct Quat
{
    float X;
    float Y;
    float Z;
    float W;
};

struct FTransform
{
    Quat Rotation;
    Vector3 Translation;
    Vector3 Scale3D;
};

struct PlayerObject {
    float x;
    float y;
    float z;
    int team;
    int isbot;
    int money;
    float TotalWeight;
    int armorHeadLevel;
    int armorBodyLevel;
    int Debug;
    std::string weapon;
    bool IsBox;
    std::string ClassName;


    FTransform meshtrans;
    FTransform headtrans;

    FTransform chesttrans;
    FTransform pelvistrans;
    FTransform lshtrans;
    FTransform rshtrans;
    FTransform lelbtrans;
    FTransform relbtrans;
    FTransform lwtrans;
    FTransform rwtrans;
    FTransform Llshtrans;
    FTransform Lrshtrans;
    FTransform Llelbtrans;
    FTransform Lrelbtrans;
    FTransform Llwtrans;
    FTransform Lrwtrans;
};

struct FMatrix
{
    float M[4][4];
};

//进程选择器结构
struct ProcessInfo {
    std::string pid;
    std::string name;
    bool isSelected; // New variable to store selection state
};

struct AddrOffsets
{
    uint64_t Addr;
    int Offsets;
};

struct StructureList
{
    uint64_t address;
    int offset;
    string type;
    string name;
    string trans;

    uint64_t P;
    float F;
    int D;

};
#endif //ANDROIDIMGUI_STRUCT_H
