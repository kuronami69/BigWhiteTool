//
// Created by Administrator on 2024/2/2.
//

#ifndef ANDROIDIMGUI_BIGWHITEREAD_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <malloc.h>
#include <math.h>
#include <thread>
#include <iostream>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include<stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <locale>
#include <string>
#include <codecvt>
#include <dlfcn.h>
#define ANDROIDIMGUI_BIGWHITEREAD_H
typedef unsigned short UTF16;
typedef char UTF8;
typedef uintptr_t kaddr;

extern int BigWhitePid;
extern int ReadMode;
uint64_t GetLibBase(int pid);
uint64_t GetProcessBase(int pid,bool mode);
// 获取指针
uint64_t GetAddr(uint64_t addr);
//读地址
bool ReadAddr(uintptr_t address, void *buffer, size_t size);
//读四字节整形
int GetDword(uintptr_t addr);
//读浮点型
float GetFloat(uintptr_t addr);
//写四字节整形
int WriteDword(uintptr_t addr, int value);
//写浮点型
float WriteFloat(uintptr_t addr, float value);

void BigWhiteinit();
// 进程读写内存
bool BigWhite_pvm(void *address, void *buffer, size_t size, bool iswrite);



template<typename T>
T XY_TRead(void* address) {
    T data{};
    BigWhite_pvm(reinterpret_cast<void *>(address), reinterpret_cast<void *>(&data), sizeof(T), false);
    return data;
}

// 模板函数用于读取内存（重载）
template<typename T>
T XY_TRead(kaddr address) {
    T data{};
    BigWhite_pvm(reinterpret_cast<void *>(address), reinterpret_cast<void *>(&data), sizeof(T), false);
    return data;
}

#endif //ANDROIDIMGUI_BIGWHITEREAD_H
