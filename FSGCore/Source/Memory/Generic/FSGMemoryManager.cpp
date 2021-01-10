//
// Created by lucas on 2020-12-28.
//

#include <Core/FSGAssert.hpp>
#include <Memory/FSGMemoryManager.hpp>

#ifdef WIN32
#include <corecrt_malloc.h>
#endif

// Check if 64 or 32 bits.
#if _WIN32 || _WIN64
#if _WIN64
#define BITS 64
#else
#define BITS 32
#endif
#endif

CMemoryManager * CMemoryManager::sm_pSingleton = nullptr;

void CMemoryManager::CreateSingleton() {
    FSG_ASSERT(sm_pSingleton == nullptr, "CMemoryManager::CreateSingleton() - Singleton has already been called");
    sm_pSingleton = new CMemoryManager();
}

CMemoryManager * CMemoryManager::GetInstance() {
    FSG_ASSERT(sm_pSingleton, "CMemoryManager::GetInstance() - Singleton has not been created.");
    return sm_pSingleton;
}

CMemoryManager::CMemoryManager(): CThreadMutex() {
    this->m_pSmallHeap = nullptr;
    this->m_pAllocHiHeap = nullptr;

    this->m_pFirstHeap = nullptr;
    this->m_uDefaultHeapTop = 0;
    this->m_pField0x18 = nullptr;

    this->m_uField0x24 = 0x20;
}

CMemoryManager::~CMemoryManager() {

}

CHeap *CMemoryManager::SetSmallHeap(CHeap *pHeap, unsigned int unk0x24) {
    FSG_ASSERT(this == pHeap->GetMemoryManager(), "Small heap being set does not belong to the memory manager.");
    auto pLastHeap = this->m_pSmallHeap;
    this->m_pSmallHeap = pHeap;
    this->m_uField0x24 = unk0x24;
    return pLastHeap;
}

CHeap *CMemoryManager::SetAllocHiHeap(CHeap *pHeap) {
    return nullptr;
}

void CMemoryManager::AddHeap(CHeap *pHeap) {
    FSG_ASSERT(pHeap, nullptr);
    pHeap->AttachSibling(this->m_pFirstHeap);
    this->m_pFirstHeap = pHeap;
}

void CMemoryManager::PushDefaultHeap(CHeap *pHeap) {
    auto pLock = new CCriticalSectionLock(this);

    FSG_ASSERT(this->m_uDefaultHeapTop < MAX_DEFAULT_HEAP_STACK_SIZE - 1, nullptr);
    auto uPrevHeapTop = this->m_uDefaultHeapTop;
    this->m_uDefaultHeapTop = uPrevHeapTop + 1;
    this->m_aDefaultHeaps[uPrevHeapTop] = pHeap;

    delete pLock;
}

void * CMemoryManager::Allocate(size_t uDataLength, unsigned int uUserAlignment, int a3, int a4, unsigned int a5, char const * a6) {
    CHeap * pHeap;
    if (a4 && (pHeap = this->m_pAllocHiHeap) != nullptr
        || (pHeap = this->m_pSmallHeap) != nullptr && uDataLength < this->m_uField0x24
        || (pHeap = (CHeap *)*(&this->m_pField0x18 + this->m_uDefaultHeapTop)) != nullptr) {
        return pHeap->Allocate(uDataLength, uUserAlignment, a3, a4, a5, a6);
    }
    FSG_ASSERT(false, "No heaps created.");
}

void *CMemoryManager::AllocateAlignedWithoutMM(size_t uDataLength, unsigned int uUserAlignment) {
#ifdef WIN32
    FSG_ASSERT(CMemoryManager::IsSingletonValid(), "Memory manager is active, you cannot call this function!");
    auto uOffset = (uUserAlignment + (sizeof(MemoryEntry) - 1)) / uUserAlignment * uUserAlignment;
    auto pData = _aligned_malloc(uOffset + uDataLength, uUserAlignment);
    FSG_ASSERT(pData, nullptr);

    // Fixme: Clean-up.
    auto pMemory = (void *)((uintptr_t)pData + uOffset);
    auto pEntryHeader = (MemoryEntry *)(((uintptr_t)pMemory) - uOffset);
    CHeap::PrepareHeader(pEntryHeader, nullptr, EMemoryBlockType::FSG);
    pEntryHeader->m_uUnalignedLength = uDataLength;
    pEntryHeader->m_uDataLength = uOffset + uDataLength;
    pEntryHeader->m_pBlockStart = (void *)((uintptr_t)pData + uOffset - sizeof(MemoryEntry));

    return pMemory;
#endif
    return nullptr;
}

bool CMemoryManager::IsSingletonValid() {
    return sm_pSingleton != nullptr;
}

void * InternalNewOperator(size_t uDataLength, unsigned int uUserAlignment, int a3, unsigned int a4, const char *a5, int a6) {
    void * pMemory;

    if (CMemoryManager::sm_pSingleton != nullptr)
        pMemory = CMemoryManager::sm_pSingleton->Allocate(uDataLength, uUserAlignment, a3, a6, a4, a5);
    else
        pMemory = CMemoryManager::AllocateAlignedWithoutMM(uDataLength, uUserAlignment);

    return pMemory;
}
