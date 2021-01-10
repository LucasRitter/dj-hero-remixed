#ifdef WIN32

#include "Core/FSGLog.hpp"
#include "windows.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <Core/FSGAssert.hpp>

extern int g_iInitTick = 0;
extern char * g_pLogFileName = nullptr;
extern bool g_bLogFileInitFailed = false;
extern bool g_bLogToFile = false;
extern bool g_bLogToConsole = false;

void FSGLogInit(char const * pLogFileName, bool bLogToFile, bool bLogToConsole) {
    g_iInitTick = GetTickCount();

    g_bLogToFile = bLogToFile;
    g_bLogToConsole = bLogToConsole;

    if (bLogToFile && pLogFileName != nullptr) {
        auto pCwd = new char[MAX_PATH]();
        GetCurrentDirectoryA(MAX_PATH, pCwd);
        FSG_ASSERT(strlen(pLogFileName) > 0, "Logging has no file name defined (ie pLogFileName[0] = '\\0')");

        g_pLogFileName = new char[MAX_PATH]();
        sprintf_s(g_pLogFileName, MAX_PATH, "%s\\%s", pCwd, pLogFileName);
        return;
    }

    g_pLogFileName = nullptr;
    g_bLogToFile = false;
    g_bLogFileInitFailed = true;
}

#define FORMAT_BUFFER_SIZE 0x800

void FSGDoLog(const char * pLogText, ...) {
    char * pBuffer = new char[FORMAT_BUFFER_SIZE]();
    HANDLE hFile;

    FSG_ASSERT(pLogText, nullptr);

    va_list args;
    va_start(args, pLogText);
    vsprintf_s(pBuffer, 0x7fe, pLogText, args);
    va_end(args);

    if (g_bLogToConsole) {
        // Todo: Replace printf
        printf("%s", pBuffer);
        OutputDebugStringA(pBuffer);
    }

    if (g_bLogToFile) {
        hFile = CreateFileA(g_pLogFileName, 0x40000000, 0, 0, 3, 0x80, 0);
        if (hFile != INVALID_HANDLE_VALUE) {
            SetFilePointer(hFile, 0, nullptr, 2);
            int iLengthUntilNull = 0;
            for (int i = 0; i < FORMAT_BUFFER_SIZE; i++) {
                if (*(pBuffer + i) == '\0') {
                    iLengthUntilNull = i;
                    break;
                }
            }

            auto iNumberOfBytesWritten = nullptr;
            WriteFile(hFile, pBuffer, iLengthUntilNull, iNumberOfBytesWritten, nullptr);
            CloseHandle(hFile);
        }
    }
}

#endif