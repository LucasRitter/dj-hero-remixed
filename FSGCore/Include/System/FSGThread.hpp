//
// Created by lucas on 2020-12-25.
//

#pragma once

typedef void (*VoidFunction)();

#ifdef WIN32
#include "Windows.h"
#endif

class CThreadSemaphore {
protected:
#ifdef WIN32
    HANDLE m_hSem;
#endif

public:
    CThreadSemaphore(int iInitialCount, unsigned int uMaximumCount);
    ~CThreadSemaphore();

    void Wait();
    void Post(unsigned int uCount);
};

class CBaseMutex {
public:
    virtual void LockMutex() = 0;
    virtual void UnlockMutex() = 0;
};

class CThreadMutex: public CBaseMutex {
protected:
#ifdef WIN32
    HANDLE m_hMutex;
#endif

public:
    CThreadMutex();
    ~CThreadMutex();

    void LockMutex();
    void UnlockMutex();
};

class CRecursiveMutex: public CBaseMutex {
protected:
    CThreadMutex m_sThreadMutex;
    unsigned int m_uLockCount;
    unsigned int m_uThreadId;

public:
    CRecursiveMutex();
    ~CRecursiveMutex();

    void LockMutex();
    void UnlockMutex();
};

class CCriticalSectionLock {
private:
    CBaseMutex * m_pMutex;

public:
    CCriticalSectionLock(CBaseMutex * pMutex);
    ~CCriticalSectionLock();
};

namespace FSGThread {
    int CurrentThreadId();
}
