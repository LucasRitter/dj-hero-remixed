//
// Created by lucas on 2020-12-25.
//

#ifdef WIN32

#include <Core/FSGLog.hpp>
#include <cassert>
#include <Core/FSGAssert.hpp>
#include "System/FSGThread.hpp"

// ================
// CThreadSemaphore
// ================

CThreadSemaphore::CThreadSemaphore(int iInitialCount, unsigned int uMaximumCount) {
    auto hSem = CreateSemaphoreA(nullptr, iInitialCount, uMaximumCount, nullptr);
    this->m_hSem = hSem;

    FSG_ASSERT(hSem, "Semaphore not allocated DO NOT CONTINUE");
}

CThreadSemaphore::~CThreadSemaphore() {
    CloseHandle(this->m_hSem);
}

void CThreadSemaphore::Wait() {
    auto iError = WaitForSingleObject(this->m_hSem, -1);

    FSG_ASSERT(!iError, "Semaphore not signalled");
}

void CThreadSemaphore::Post(unsigned int uCount) {
    auto bReleased = ReleaseSemaphore(this->m_hSem, uCount, nullptr);

    // Todo: Add output from GetLastError() to message.
    FSG_ASSERT(!bReleased, "Semaphore posted %d");
}


// ============
// CThreadMutex
// ============

CThreadMutex::CThreadMutex() {
    this->m_hMutex = CreateMutexA(nullptr, 0, nullptr);
}

CThreadMutex::~CThreadMutex() {
    CloseHandle(this->m_hMutex);
}

void CThreadMutex::LockMutex() {
    WaitForSingleObject(this->m_hMutex, -1);
}

void CThreadMutex::UnlockMutex() {
    ReleaseMutex(this->m_hMutex);
}


// ===============
// CRecursiveMutex
// ===============

CRecursiveMutex::CRecursiveMutex() {
    this->m_sThreadMutex = CThreadMutex();
    this->m_uLockCount = 0;

    // Todo: Check if this is actually the value.
    this->m_uThreadId = 0x0;
}

CRecursiveMutex::~CRecursiveMutex() {
    assert(this->m_uLockCount == 0);
    this->m_sThreadMutex.~CThreadMutex();
}

void CRecursiveMutex::LockMutex() {
    auto iThread = FSGThread::CurrentThreadId();
    if (this->m_uLockCount == 0 || this->m_uThreadId != iThread) {
        this->m_sThreadMutex.LockMutex();
        this->m_uThreadId = iThread;
    }
    this->m_uLockCount++;
}

void CRecursiveMutex::UnlockMutex() {
    assert(this->m_uLockCount > 0 && this->m_uThreadId == FSGThread::CurrentThreadId());
    this->m_uLockCount--;
    if (this->m_uLockCount == 0) {
        this->m_sThreadMutex.UnlockMutex();
    }
}


// =========
// FSGThread
// =========

int FSGThread::CurrentThreadId() {
    auto iThread = GetCurrentThreadId();
    return iThread;
}

#endif

