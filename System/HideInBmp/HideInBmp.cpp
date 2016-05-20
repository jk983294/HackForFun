/**
 * idea is to use BMP 24 format, RGB (3 bytes) + alpha (3 bytes)
 * hide data in alpha bytes
 * encrypt secret.txt background.bmp target.bmp
 * decrypt target.bmp out.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>

char *readFileContent(const char* path, DWORD* fileSize){
    HANDLE hFile = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE){
        printf("open file %s failed\n", path);
        return NULL;
    }
    DWORD dwRead; 
    DWORD dwSize = GetFileSize(hFile, &dwRead);
    *fileSize = dwSize;
    char* buf = new char[dwSize];
    RtlZeroMemory(buf, dwSize);
    ReadFile(hFile, buf, dwSize, &dwRead, 0);
    if(dwRead != dwSize){
        printf("read content failed\n", path);
        return NULL;
    }
    CloseHandle(hFile);
    return buf;
}
bool saveFile(const char* path, char* buf, int len){
    HANDLE hFile = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE){
        printf("open file %s failed\n", path);
        return false;
    }
    DWORD dwWrite;
    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    WriteFile(hFile, buf, len, &dwWrite, 0);
    CloseHandle(hFile);
    return true;
}
bool recovery(const char* bmpFilePath, const char* secretFilePath){
    DWORD dwBmpSize;
    char* pBmp = readFileContent(bmpFilePath, &dwBmpSize);
    auto first = (DWORD*)(pBmp + 10);
    printf("first pixel offset: %d\n", *first);
    DWORD dwSecretSize = *(DWORD*)(pBmp + *first + 3);
    char* buf = new char[dwSecretSize];
    char* posBmp = pBmp + *first + 9;
    for (int i = 0; i < dwSecretSize; posBmp += 6){
        buf[i++] = *posBmp;
        if(i < dwSecretSize) buf[i++] = *(posBmp + 1);
        if(i < dwSecretSize) buf[i++] = *(posBmp + 2);
    }
    saveFile(secretFilePath, buf, dwSecretSize);
    delete[] pBmp;
    delete[] buf;
    return true;
}
// record file into RGB alpha channel
bool hide(const char* secretFilePath, const char* bmpFilePath, const char* target){
    DWORD dwBmpSize, dwSecretSize;
    char* pBmp = readFileContent(bmpFilePath, &dwBmpSize);
    char* pSecretFile = readFileContent(secretFilePath, &dwSecretSize);
    auto *first = (DWORD*)(pBmp + 10);
    printf("first pixel offset: %d\n", *first);
    char* posBmp = pBmp + *first + 3;
    char* posSecret = pSecretFile;
    *((DWORD*)posBmp) = dwSecretSize;   // record size
    posBmp += 6;
    for (; posBmp < pBmp + dwBmpSize && posSecret < pSecretFile + dwSecretSize; posBmp +=6){
        *posBmp = *posSecret;
        if(posSecret + 1 < pSecretFile + dwSecretSize) *(posBmp + 1) = *(posSecret + 1);
        if(posSecret + 2 < pSecretFile + dwSecretSize) *(posBmp + 2) = *(posSecret + 2);
        posSecret += 3;
    }
    saveFile(target, pBmp, dwBmpSize);
    delete[] pBmp;
    delete[] pSecretFile;
    return true;
}
int main(int argc, char* argv[]){
    if(argc < 3){
        printf("need secret file and bmp file path.\n");
        return -1;
    }
    if(strcmp(argv[1], "encrypt") == 0 && argc == 5){
        hide(argv[2], argv[3], argv[4]);
    } else if(strcmp(argv[1], "decrypt") == 0 && argc == 4){
        recovery(argv[2], argv[3]);
    } else {
        printf("cmd error.\n");
    }
    return 0;
}