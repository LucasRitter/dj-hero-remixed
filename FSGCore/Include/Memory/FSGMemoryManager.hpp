//
// Created by lucas on 2020-12-25.
//

#pragma once

#include <System/FSGThread.hpp>
#include <Memory/FSGHeap.hpp>

#define MAX_DEFAULT_HEAP_STACK_SIZE 2

class CMemoryManager: CThreadMutex {
protected:
    CMemoryManager();
    ~CMemoryManager();

    CHeap * m_pSmallHeap;
    CHeap * m_pAllocHiHeap;

    CHeap * m_pFirstHeap;
    unsigned int m_uDefaultHeapTop;
    void * m_pField0x18;
    CHeap * m_aDefaultHeaps[MAX_DEFAULT_HEAP_STACK_SIZE];
    unsigned int m_uField0x24;

public:
    static CMemoryManager * sm_pSingleton;

    static void CreateSingleton();
    static void DeleteSingleton();
    static CMemoryManager * GetInstance();
    static bool IsSingletonValid();

    CHeap * SetSmallHeap(CHeap * pHeap, unsigned int unk0x24);
    CHeap * SetAllocHiHeap(CHeap * pHeap);

    void AddHeap(CHeap * pHeap);
    void PushDefaultHeap(CHeap * pHeap);

    void * Allocate(size_t uDataLength, unsigned int uUserAlignment, int a3, int a4, unsigned int a5, char const * a6);
    static void * AllocateAlignedWithoutMM(size_t uDataLength, unsigned int uUserAlignment);
};

void * InternalNewOperator(size_t uDataLength, unsigned int uUserAlignment, int a3, unsigned int a4, char const * a5, int a6);