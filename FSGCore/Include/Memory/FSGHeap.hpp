//
// Created by lucas on 2020-12-25.
//

#pragma once

#include <System/FSGThread.hpp>
#include "MemoryPool.hpp"

#define MAX_SNAPSHOT_STACK_SIZE 6
#define MAX_HEAP_NAME 0x100

class CHeapSnapshot {
public:
    class CHeap * m_pHeap;
    unsigned int m_uIndex;
    char m_strHeapName[MAX_HEAP_NAME];
    char m_strPrevHeapName[MAX_HEAP_NAME];
    unsigned int m_uPrevIndex;

    void Create(class CHeap * pHeap, unsigned int uIndex, char const * szHeapName, char const * szPrevHeapName, unsigned int uPrevIndex);
};

class CHeap {
    typedef int (*CHeapConstructorCallback)(CHeap *pHeap, unsigned int uUnknown2);

protected:
    // 0x0
    // Todo: Type
    void * m_pMemoryBuffer;

    // 0xc
    class CMemoryManager *m_pMemoryManager;

public:
    // 0x4
    unsigned __int64 m_uDataLength;
    // 0x8
    char const * m_szHeapName;
    // 0x10
    CHeap * m_pNextHeap;
    // 0x14
    MemoryEntry * m_pFirstBlock;

    // 0x18
    MemoryEntry * m_pFirstAllocatedBlock;
    // 0x1c
    uintptr_t m_uField0x1c;
    // 0x20
    unsigned int m_uDefaultAlignment;
    // 0x24
    unsigned int m_uField0x24;
    // 0x28
    unsigned int m_uNumSuccessfulAllocations;
    // 0x2c
    unsigned int m_uField0x2c;
    // 0x30
    unsigned int m_uNumFailedAllocationRequests;
    // 0x34
    unsigned int m_uLiveAllocations;
    // 0x38
    unsigned int m_uFreeBlocks;
    // 0x3c
    unsigned int m_uUnalignedLength;
    // 0x40
    unsigned int m_uField0x40;
    // 0x44
    unsigned int m_uField0x44;
    // 0x48
    unsigned int m_uField0x48;
    // 0x4c
    unsigned __int64 m_uField0x4c;
    // 0x54
    unsigned int m_uField0x54;

    // 0x58
    CRecursiveMutex m_pRecursiveMutex;

    // 0x70
    CHeapSnapshot m_aSnapshots[MAX_SNAPSHOT_STACK_SIZE - 1];

    // 0xab0
    unsigned int m_iCurrentSnapshot;
    // 0xab4
    CHeapConstructorCallback * m_uField0xab4;

    class CMemoryManager * GetMemoryManager();

    CHeap(void *pRawMemory, unsigned int uDataLength, char const *szHeapName, int a5,
          CHeapConstructorCallback *pCallback);
    ~CHeap();

    CHeapSnapshot * PushSnapshot(char const * a2, char const * a3, unsigned int a4);
    void AttachSibling(CHeap * pHeap);
    void *Allocate(size_t uDataLength, unsigned int uUserAlignment, int a3, int a4, unsigned int a5, const char *a6);


    static void PrepareHeader(MemoryEntry * pBlock, CHeap * pHeap, EMemoryBlockType eBlockType);

    void * GetMemoryBuffer();
    size_t GetHeapSize();

private:
    void AddAllocatedBlock(MemoryEntry * pBlock);
    void RemoveAllocatedBlock(MemoryEntry * a1);
    int CombineFreeBlocks(MemoryEntry * pFirstEntry, MemoryEntry * pSecondEntry);

    void * InternalAllocate(size_t uDataLength, unsigned int uUserAlignment, unsigned int a3, const char * a4, int a5, int a6);
    static void PrepareHeader_ChangeUsage(MemoryEntry * pBlock, EMemoryBlockType eBlockType);

    void AddFreeBlock(MemoryEntry * pFreeItem);
    MemoryEntry * FindFreeBlock(size_t uDataLength, unsigned int uUserAlignment, int a3);

    size_t CalculateMemoryRequired(MemoryEntry * pBlock, unsigned int uBlockLength, unsigned int uDataLength, unsigned int uUserAlignment, int a6);
    char * DetermineAlignedMemoryPoint(MemoryEntry * pBlock, unsigned int uBlockLength, unsigned int uDataLength, unsigned int uUserAlignment, int a6);

};