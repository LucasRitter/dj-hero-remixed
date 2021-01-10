//
// Created by lucas on 2020-12-28.
//

#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

class CTimer {
protected:
#ifdef WIN32
    LONGLONG m_iFrequency;
    int m_bFrozen;
    int m_iField0x14;
    LARGE_INTEGER m_iPerformanceCounter;
#endif
public:
#ifdef WIN32
    double m_dElapsedTime;
#endif

    CTimer();
    void Update();
};
