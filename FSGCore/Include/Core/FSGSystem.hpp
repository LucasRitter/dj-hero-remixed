//
// Created by lucas on 2020-12-25.
//

#pragma once

namespace FSGSystem {

    // Build Information
    const char * FSGGetBuildDateAsString();
    const char * FSGGetBuildTimeAsString();
    const char * FSGGetGameName();
    const char * FSGGetGameNameShort();

    int FSGGetGameVersion();
}