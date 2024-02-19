

// Aloha开源Qx9-10对接，理论通杀
// 原理: ls -l /proc/*/exe 2>/dev/null | grep -E "/data/[a-z]{6} \\(deleted\\)"
// Output:
// lrwxrwxrwx 1 root root 0 2024-01-01 14:00 /proc/7665/exe -> /data/ktbqmo (deleted)
// Tg: https://t.me/wushuang669

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>
#include <malloc.h>
#include <thread>
#include <sys/mman.h>
#include <sys/uio.h>
#include <math.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <locale>
#include <string>
#include <dlfcn.h>


class c_driver {
private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;

    int fd;
    pid_t pid;

    typedef struct _COPY_MEMORY {
        pid_t pid;
        uintptr_t addr;
        void* buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _MODULE_BASE {
        pid_t pid;
        char* name;
        uintptr_t base;
    } MODULE_BASE, *PMODULE_BASE;

    enum OPERATIONS {
        OP_INIT_KEY = 0x800,
        OP_READ_MEM = 0x801,
        OP_WRITE_MEM = 0x802,
        OP_MODULE_BASE = 0x803,
    };


    int symbol_file(const char *filename) {
        int length = strlen(filename);

        for (int i = 0; i < length; i++) {
            if (isupper(filename[i])) {
                has_upper = 1;
            } else if (islower(filename[i])) {
                has_lower = 1;
            } else if (ispunct(filename[i])) {
                has_symbol = 1;
            } else if (isdigit(filename[i])) {
                has_digit = 1;
            }
        }

        return has_upper && has_lower && !has_symbol && !has_digit;
    }

    char* execCom(const char* shell)
    {
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

// 修复createDriverNode和removeDeviceNode函数
    void createDriverNode(char *path, int major_number, int minor_number)
    {
        std::string command = "mknod " + std::string(path) + " c " + std::to_string(major_number) + " " + std::to_string(minor_number);
        system(command.c_str());
        printf("[-] 创建 %s，主设备号：%d，次设备号：%d", path, major_number, minor_number);
    }

// 删除驱动节点
// 新的函数，用于删除设备节点
    void removeDeviceNode(char* path) {
        // printf("%s\n",path);
        if (unlink(path) == 0) {
            printf("[-] 驱动安全守护已激活");
            // cerr << "已删除设备节点：" << devicePath << endl;
        } else {
            printf("[-] 驱动安全守护执行错误");
            // perror("删除设备节点时发生错误");
        }
    }


public:
    c_driver() : fd(-1) {

        char *output = execCom("ls -l /proc/*/exe 2>/dev/null | grep -E \"/data/[a-z]{6} \\(deleted\\)\"");
        char filePath[256];
        char pid[56];
        if (output != NULL) {
            printf("\n\n调试输出:\n%s\n", output);
            char *procStart = strstr(output, "/proc/");

            // Extracting process ID
            char *pidStart = procStart + 6; // Move to the position after "/proc/"
            char *pidEnd = strchr(pidStart, '/');

            strncpy(pid, pidStart, pidEnd - pidStart);
            pid[pidEnd - pidStart] = '\0';

            char *arrowStart = strstr(output, "->");
            // Extracting file path
            char *start = arrowStart + 3; // Move to the position after "->"
            char *end = strchr(output, '(') - 1; // Find the position before '('
            strncpy(filePath, start, end - start + 1);
            filePath[end - start] = '\0';

            // Replace "data" with "dev" in filePath
            char *replacePtr = strstr(filePath, "data");
            if (replacePtr != NULL) {
                memmove(replacePtr + 2, replacePtr + 3, strlen(replacePtr + 3) + 1);
                memmove(replacePtr, "dev", strlen("dev"));
            }
            // Print the results
            // printf("Driver Path: %s\n", filePath);
        } else {
            printf("Error executing scripts.\n");
        }

        char cmd[256];
        // Construct the command to get fdInfo using the extracted pid
        sprintf(cmd, "ls -al -L /proc/%s/fd/3", pid);
        // Execute the command and get fdInfo
        char *fdInfo = execCom(cmd);
        int major_number, minor_number;
        sscanf(fdInfo, "%*s %*d %*s %*s %d, %d", &major_number, &minor_number);

        if (filePath!="\0") {
            // std::cout << "创建 /dev/" << driverInfo.deviceID << std::endl;
            printf("\n[-] 驱动信息载入成功：%s\n",filePath);
            createDriverNode(filePath, major_number, minor_number);
            sleep(1);
            fd = open(filePath, O_RDWR); // Use c_str() to get a C-style string
            // printf("%d",fd);
            if (fd == -1) {

                printf("\n[-] 驱动链接启动\n");
                removeDeviceNode(filePath);

            } else {
                printf("\n[-] 驱动已经启动\n");
                removeDeviceNode(filePath);
            }

        }
    }

    ~c_driver() {
        //wont be called
        if (fd > 0)
            close(fd);
    }

    void initialize(pid_t pid) {
        this->pid = pid;
    }

    bool init_key(char* key) {
        char buf[0x100];
        strcpy(buf,key);
        if (ioctl(fd, OP_INIT_KEY, buf) != 0) {
            return false;
        }
        return true;
    }

    bool read(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;

        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    bool write(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;

        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    template <typename T>
    T read(uintptr_t addr) {
        T res;
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    template <typename T>
    bool write(uintptr_t addr,T value) {
        return this->write(addr, &value, sizeof(T));
    }

    uintptr_t get_module_base(char* name) {
        MODULE_BASE mb;
        char buf[0x100];
        strcpy(buf,name);
        mb.pid = this->pid;
        mb.name = buf;

        if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) {
            return 0;
        }
        return mb.base;
    }
};


// 静态成员变量声明
static c_driver *driver;

// 初始化函数声明
void initDriver();

// 初始化函数定义
void initDriver() {
    driver = new c_driver();
}
// 读取字符信息

/*--------------------------------------------------------------------------------------------------------*/

typedef char PACKAGENAME;	// 包名
pid_t pid;	// 进程ID

int getPID(char* PackageName)
{
    FILE* fp;
    char cmd[0x100] = "pidof ";
    strcat(cmd, PackageName);
    fp = popen(cmd,"r");
    fscanf(fp,"%d", &pid);
    pclose(fp);
    if (pid > 0)
    {
        driver->initialize(pid);
    }
    return pid;
}

bool PidExamIne()
{
    char path[128];
    sprintf(path, "/proc/%d",pid);
    if (access(path,F_OK) != 0)
    {
        printf("\033[31;1m");
        puts("获取进程PID失败!");
        exit(1);
    }
    return true;
}

long getModuleBase(char* module_name)
{
    uintptr_t base=0;
    base = driver->get_module_base(module_name);
    return base;
}

long ReadValue(long addr)
{
    long he=0;
    if (addr < 0xFFFFFFFF){
        driver->read(addr, &he, 4);
    }else{
        driver->read(addr, &he, 8);
    }
    return he;
}

long ReadDword(long addr)
{
    long he=0;
    driver->read(addr, &he, 4);
    return he;
}

float ReadFloat(long addr)
{
    float he=0;
    driver->read(addr, &he, 4);
    return he;
}