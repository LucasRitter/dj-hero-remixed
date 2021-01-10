//
// Created by lucas on 2020-12-25.
//

#pragma once

extern int g_iInitTick;
extern char * g_pLogFileName;
extern bool g_bLogFileInitFailed;
extern bool g_bLogToFile;
extern bool g_bLogToConsole;

void FSGLogInit(char const * pLogFileName, bool bLogToFile, bool bLogToConsole);
void FSGDoLog(char const * pLogText, ...);
void FSGDoLogDirect(char const * pMessage);
void FSGLogDestroy();