//
// Created by lucas on 2020-12-25.
//

#include "Core/FSGSystem.hpp"

const char * FSGSystem::FSGGetBuildDateAsString() {
    return __DATE__;
}

const char * FSGSystem::FSGGetBuildTimeAsString() {
    return __TIME__;
}

int FSGSystem::FSGGetGameVersion() {
#ifndef GAME_VERSION
    #define GAME_VERSION 0;
#endif
    return GAME_VERSION;
}

const char * FSGSystem::FSGGetGameName() {
#ifndef GAME_NAME
    #define GAME_NAME "UNDEFINED GAME NAME"
#endif
    return GAME_NAME;
}

const char * FSGSystem::FSGGetGameNameShort() {
#ifndef GAME_NAME_SHORT
    #define GAME_NAME_SHORT "UNDEFINED SHORTENED GAME NAME"
#endif
    return GAME_NAME_SHORT;
}

