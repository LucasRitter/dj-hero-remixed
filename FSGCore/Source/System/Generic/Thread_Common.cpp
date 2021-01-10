//
// Created by lucas on 2020-12-30.
//

#include <System/FSGThread.hpp>
#include <Core/FSGAssert.hpp>

// ====================
// CCriticalSectionLock
// ====================
CCriticalSectionLock::CCriticalSectionLock(CBaseMutex *pMutex) {
    FSG_ASSERT(pMutex, nullptr);
    this->m_pMutex = pMutex;
    this->m_pMutex->LockMutex();
}

CCriticalSectionLock::~CCriticalSectionLock() {
    this->m_pMutex->UnlockMutex();
}
