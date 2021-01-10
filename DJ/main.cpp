//
// Created by lucas on 2020-12-25.
//

#ifdef WIN32
    #define LOG_FILE_NAME "LogWin32.log"
#else
    #define LOG_FILE_NAME "Log.log"
#endif

#include <main.hpp>
#include <memory.hpp>
#include <Core/FSGLog.hpp>
#include <Core/FSGSystem.hpp>
#include <Core/FSGTimer.hpp>
#include <Memory/FSGMemoryManager.hpp>
#include <cstdio>


int main(int argc, char **argv) {
    CMemoryManager::CreateSingleton();
    auto bHasLogFile = FSGMainHasCommand("logfile", argc, argv);
    FSGLogInit(LOG_FILE_NAME, bHasLogFile, true);

    // Logging build information
    auto szBuildTime = FSGSystem::FSGGetBuildTimeAsString();
    auto szBuildDate = FSGSystem::FSGGetBuildDateAsString();
    auto iGameVersion = FSGSystem::FSGGetGameVersion();
    auto szGameName = FSGSystem::FSGGetGameName();

    FSGDoLog("***\nCurrent %s (ver %d) Build %s - %s using libraries dated %s - %s\n***\n", szGameName, iGameVersion, __DATE__, __TIME__, szBuildDate, szBuildTime);

    // Initialisation
    // Todo: Initialise Game, Network, Render ....
    FSGGameInitialise();

    auto pTimer = new CTimer();

    // Main loop
    auto bContinueRunning = true;
    while (bContinueRunning) {
        pTimer->Update();
        auto iElapsed = pTimer->m_dElapsedTime;

        bContinueRunning = FSGGameMain(iElapsed);

        if (pTimer->m_dElapsedTime <= iElapsed) {
            pTimer->m_dElapsedTime = 0;
        } else {
            pTimer->m_dElapsedTime = pTimer->m_dElapsedTime - iElapsed;
        }
    }

    // Destruction loop
    auto bDestroyed = false;
    while (!bDestroyed) {
        bDestroyed = FSGGameDestroy();
    }

    // Todo: Delete Memory Manager
    return 0;
}

void FSGGameInitialise() {
    // Todo: FSGGameInitialise()
    FSGDoLog("FSGGameInitialise()...\n");
    // FSG_ASSERT(XSetFileCacheSize(X360_DISC_CACHE_SIZE, nullptr);
    // Todo: g_pDjSaveAttributesCallback = DJSaveAttributesCallback;
    // XNotifyPositionUI(5);
    InitialiseMemoryManager();
    // CreateRender();
}

int FSGGameMain(double dElapsed) {
    // Todo: FSGGameMain()
    return true;
}

int FSGGameDestroy() {
    // Todo: FSGGameDestroy()
    FSGDoLog("FSGGameDestroy()...\n");

    FSGDoLog("Shutting down game...\n");
    return true;
}

bool FSGMainHasCommand(char const *szCommand, int argc, char **argv) {
    const char * pArg;

    char cArgChar;
    char cCommandChar;

    if (0 < argc) {
        for (int i = 0; i < argc; i++) {
            pArg = argv[i];
            if (*pArg == '-') {
                for (int charIndex = 0; charIndex < strlen(pArg); charIndex++) {
                    cArgChar = *(pArg + 1 + charIndex);
                    cCommandChar = *(szCommand + charIndex);
                    if (cArgChar != cCommandChar)
                        break;

                    if (cArgChar == '\0')
                        break;
                }
                if (cArgChar == cCommandChar)
                    return true;
            }
        }
    }

    return false;
}

