//
// Created by lucas on 2020-12-28.
//

#ifdef WIN32

#include <Windows.h>
#include <Core/FSGLog.hpp>
#include "Core/FSGTimer.hpp"

CTimer::CTimer() {
    LARGE_INTEGER iFrequency;

    this->m_bFrozen = false;
    this->m_dElapsedTime = 0;
    QueryPerformanceCounter(&this->m_iPerformanceCounter);

    QueryPerformanceFrequency(&iFrequency);
    this->m_iFrequency = iFrequency.QuadPart;

    // FSGDoLog(" Counter: %d\n Frequency: %d\n", this->m_iPerformanceCounter.QuadPart, this->m_dElapsedTime);
}

void CTimer::Update() {
    if (!this->m_bFrozen) {
        LARGE_INTEGER iCurrentCounter;
        LARGE_INTEGER iLastCounter;
        LARGE_INTEGER iFrequency;
        QueryPerformanceCounter(&iCurrentCounter);
        iLastCounter = this->m_iPerformanceCounter;
        this->m_iPerformanceCounter = iCurrentCounter;

        QueryPerformanceFrequency(&iFrequency);
        this->m_iFrequency = iFrequency.QuadPart;

        this->m_dElapsedTime =
                (double)(iCurrentCounter.QuadPart - iLastCounter.QuadPart) / this->m_iFrequency
                + this->m_dElapsedTime;
    }
}

#endif