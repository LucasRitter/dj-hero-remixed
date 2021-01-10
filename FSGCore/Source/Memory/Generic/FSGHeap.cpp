//
// Created by lucas on 2020-12-30.
//

#include <Memory/FSGHeap.hpp>
#include <Core/FSGAssert.hpp>
#include <Collections/FSGString.hpp>
#include <cmath>
#include <Core/FSGLog.hpp>

class CMemoryManager {
public:
    static CMemoryManager *GetInstance();

    void AddHeap(class CHeap *);
};

CHeap::CHeap(void *pRawMemory, unsigned int uDataLength, const char *szHeapName, int a5,
             CHeap::CHeapConstructorCallback *pCallback) {
    this->m_uDataLength = uDataLength;
    this->m_szHeapName = szHeapName;
    this->m_pMemoryManager = CMemoryManager::GetInstance();

    this->m_pFirstAllocatedBlock = nullptr;
    this->m_uField0x1c = 0;
    this->m_uDefaultAlignment = 16;
    this->m_uField0x24 = 0;
    this->m_uNumSuccessfulAllocations = 0;
    this->m_uField0x2c = 0;
    this->m_uNumFailedAllocationRequests = 0;
    this->m_uLiveAllocations = 0;
    this->m_uFreeBlocks = 1;
    this->m_uUnalignedLength = 0;
    this->m_uField0x40 = 0;
    this->m_uField0x44 = 0;
    this->m_uField0x48 = 0;
    this->m_uField0x4c = 0;
    this->m_uField0x54 = 0;

    // this->m_aSnapshots = 0;

    this->m_uField0xab4 = pCallback;

    // All of these assert statements were provided by FSG.
    FSG_ASSERT(szHeapName != nullptr, "No heap name was supplied to CHeap constructor.");
    FSG_ASSERT(uDataLength >= sizeof(MemoryEntry), "Heap data size was too small to support allocations.");
    FSG_ASSERT(pRawMemory != nullptr, "A nullptr data buffer was supplied when created a CHeap object.");
    // Todo: FSG_IS_ALIGNED(pRawMemory, SYSTEM_MINIMUM_ALIGNMENT)
    FSG_ASSERT(FSG_IS_ALIGNED(pRawMemory, 4), nullptr);
    FSG_ASSERT(!(uDataLength & 3), nullptr);

    // Todo: This.
//    if ( a5 && uDataLength >> 2 )
//    {
//        v25 = (char *)pRawMemory - 4;
//        do
//        {
//            ++v17;
//            *++v25 = -2004318072;
//        }
//        while ( v17 < uDataLength >> 2 );
//    }

    this->m_pFirstBlock = static_cast<MemoryEntry *>(pRawMemory);
    CHeap::PrepareHeader(m_pFirstBlock, this, EMemoryBlockType::FRE);
    this->m_pFirstBlock->m_pBlockStart = pRawMemory;
    this->m_pFirstBlock->m_uDataLength = uDataLength;

    this->m_iCurrentSnapshot = -1;
    this->PushSnapshot(szHeapName, nullptr, 0);
    this->m_pMemoryManager->AddHeap(this);
}

CHeap::~CHeap() {
    // Todo: ~CHeap
}

class CMemoryManager *CHeap::GetMemoryManager() {
    return this->m_pMemoryManager;
}

void CHeap::PrepareHeader(MemoryEntry *pBlock, CHeap *pHeap, EMemoryBlockType eBlockType) {
    FSG_ASSERT(pBlock, nullptr);
    memset(pBlock, 0, sizeof(MemoryEntry));
    CHeap::PrepareHeader_ChangeUsage(pBlock, eBlockType);
    pBlock->m_pHeap = pHeap;
}

void CHeap::PrepareHeader_ChangeUsage(MemoryEntry *pBlock, EMemoryBlockType eBlockType) {
    FSG_ASSERT(pBlock, nullptr);
    switch (eBlockType) {
        case FRE:
            pBlock->m_cBlockType[0] = 'F';
            pBlock->m_cBlockType[1] = 'R';
            pBlock->m_cBlockType[2] = 'E';
            pBlock->m_cBlockType[3] = '\0';
            return;
        case DEL:
            pBlock->m_cBlockType[0] = 'D';
            pBlock->m_cBlockType[1] = 'E';
            pBlock->m_cBlockType[2] = 'L';
            pBlock->m_cBlockType[3] = '\0';
            return;
        case FSG:
            pBlock->m_cBlockType[0] = 'F';
            pBlock->m_cBlockType[1] = 'S';
            pBlock->m_cBlockType[2] = 'G';
            pBlock->m_cBlockType[3] = '\0';
            return;
        case STA:
            pBlock->m_cBlockType[0] = 'S';
            pBlock->m_cBlockType[1] = 'T';
            pBlock->m_cBlockType[2] = 'A';
            pBlock->m_cBlockType[3] = '\0';
            return;
        case INV:
            pBlock->m_cBlockType[0] = 'I';
            pBlock->m_cBlockType[1] = 'N';
            pBlock->m_cBlockType[2] = 'V';
            pBlock->m_cBlockType[3] = '\0';
            return;
        case MAX:
            break;
    }
    FSG_ASSERT(eBlockType < EMemoryBlockType::MAX, "Invalid block type");
}

CHeapSnapshot *CHeap::PushSnapshot(const char *a2, const char *a3, unsigned int a4) {
    auto pSectionLock = new CCriticalSectionLock(&this->m_pRecursiveMutex);

    this->m_iCurrentSnapshot++;
    FSG_ASSERT(this->m_iCurrentSnapshot >= 0, "Invalid snapshot index!");
    FSG_ASSERT(this->m_iCurrentSnapshot < MAX_SNAPSHOT_STACK_SIZE, "Invalid snapshot index!");

    auto index = this->m_iCurrentSnapshot;
    this->m_aSnapshots[index].Create(this, index, a2, a3, a4);

    delete pSectionLock;
}

void CHeap::AttachSibling(CHeap *pHeap) {
    auto pSectionLock = new CCriticalSectionLock(&this->m_pRecursiveMutex);
    this->m_pNextHeap = pHeap;
    delete pSectionLock;
}

char *CHeap::DetermineAlignedMemoryPoint(MemoryEntry *pBlock, unsigned int uBlockLength, unsigned int uDataLength,
                                         unsigned int uUserAlignment, int a6) {
    // Todo: FSG_IS_ALIGNED(pBlock, SYSTEM_MINIMUM_ALIGNMENT)
    FSG_ASSERT(FSG_IS_ALIGNED(pBlock, 4), nullptr);
    FSG_ASSERT(uUserAlignment >= this->m_uDefaultAlignment, nullptr);

    if (!a6)
        return (char *) ((this->m_uField0x1c + uUserAlignment + 3) / uUserAlignment * uUserAlignment);

    auto pEndOfBlock = (char *) (pBlock + uBlockLength);
    // Todo: FSG_IS_ALIGNED(pEndOfBlock, SYSTEM_MINIMUM_ALIGNMENT)
    FSG_ASSERT(FSG_IS_ALIGNED(pEndOfBlock, 4), nullptr);
    return pEndOfBlock;
}

size_t CHeap::CalculateMemoryRequired(MemoryEntry *pBlock, unsigned int uBlockLength, unsigned int uDataLength,
                                      unsigned int uUserAlignment, int a6) {
    FSG_ASSERT(FSG_IS_ALIGNED(pBlock, 4), nullptr);
    auto pAlignedMemoryBlock = this->DetermineAlignedMemoryPoint(pBlock, uBlockLength, uDataLength, uUserAlignment, a6);
    FSG_ASSERT(FSG_IS_ALIGNED(pAlignedMemoryBlock, 4), nullptr);
    // Todo: pAlignedMemoryBlock != (char *)((unsigned int)pAlignedMemoryBlock / uUserAlignment * uUserAlignment)
    FSG_ASSERT(FSG_IS_ALIGNED(pAlignedMemoryBlock, uUserAlignment + 1), nullptr);

    if (a6) {
        // Fixme: Not sure how this works.
        return (uintptr_t) pBlock + 4 * ((1 - (((uintptr_t) pAlignedMemoryBlock - 0x1d) >> 2)) + uBlockLength);
        // return pBlock + 4 * (1 - ((*pAlignedMemoryBlock - 29) >> 2))
    }
    // Todo: Replace 3 with SYSTEM_ALIGNMENT - 1
    return ((uintptr_t) pAlignedMemoryBlock + uDataLength + 3) & (size_t(-1) - 3) - (uintptr_t) pBlock;
}

void *
CHeap::Allocate(size_t uDataLength, unsigned int uUserAlignment, int a3, int a4, unsigned int a5, const char *a6) {
    return nullptr;
}

void *CHeap::InternalAllocate(size_t uDataLength, unsigned int uUserAlignment, unsigned int a3, const char *a4, int a5,
                              int a6) {
    // FIXME: NEED A LOT OF CLEANUP.

    if (uDataLength)
        FSG_ASSERT(uDataLength > 0, "Allocations of zero bytes not allowed.");
    else
        return nullptr;

    auto pLock = new CCriticalSectionLock(&this->m_pRecursiveMutex);
    if (uUserAlignment < 0x10)
        uUserAlignment = 0x10;

    MemoryEntry * pReturnVal = nullptr;

    // Fixme: Probably not a6
    auto pFreeBlock = this->FindFreeBlock(uDataLength, uUserAlignment, a6);
    auto v19 = pFreeBlock;
    if (pFreeBlock) {
        if (pFreeBlock->m_pNext)
            pFreeBlock->m_pNext->m_pPrevious = pFreeBlock->m_pPrevious;
        if (pFreeBlock->m_pPrevious)
            pFreeBlock->m_pPrevious->m_pNext = pFreeBlock->m_pNext;
        else
            this->m_pFirstBlock = pFreeBlock->m_pNext;

        pFreeBlock->m_pNext = nullptr;
        pFreeBlock->m_pPrevious = nullptr;

        this->m_uFreeBlocks--;

        auto pUserMem = reinterpret_cast<MemoryEntry *>(this->DetermineAlignedMemoryPoint(pFreeBlock,
                                                                                                   pFreeBlock->m_uDataLength,
                                                                                                   uDataLength,
                                                                                                   uUserAlignment, a6));
        auto uFreeBlockLength = pFreeBlock->m_uDataLength;

        size_t uNextMemoryBlockLength = 0;
        if (a6)
            // Fixme: This memory block calculation.
            uNextMemoryBlockLength = (uintptr_t) pFreeBlock +
                                     ((uFreeBlockLength - (uintptr_t) pUserMem + uDataLength + 3) & -0x5);
        else
            uNextMemoryBlockLength = uFreeBlockLength -
                                     this->CalculateMemoryRequired(pFreeBlock, pFreeBlock->m_uDataLength, uDataLength,
                                                               uUserAlignment, 0);

        auto pHeaderLocation = (MemoryEntry *)(pUserMem - 1);
        auto pNextMemoryBlock = (pFreeBlock + uFreeBlockLength - uNextMemoryBlockLength);

        FSG_ASSERT(FSG_IS_ALIGNED(pHeaderLocation, 4), nullptr);

        uintptr_t uSizeBefore = pHeaderLocation - pFreeBlock;

        FSG_ASSERT(FSG_IS_ALIGNED(uSizeBefore, 4), nullptr);

        auto uMemoryRequired = this->CalculateMemoryRequired(pNextMemoryBlock, uNextMemoryBlockLength, 0x10, this->m_uDefaultAlignment, a6);

        if (uMemoryRequired <= uNextMemoryBlockLength && uNextMemoryBlockLength - uMemoryRequired >= this->m_uField0x24) {
            pFreeBlock->m_uDataLength -= uNextMemoryBlockLength;
            CHeap::PrepareHeader(pNextMemoryBlock, this, EMemoryBlockType::FRE);
            pNextMemoryBlock->m_pBlockStart = pNextMemoryBlock;
            pNextMemoryBlock->m_uDataLength = uNextMemoryBlockLength;
            this->AddFreeBlock(pNextMemoryBlock);
        }

        MemoryEntry v37;

        memcpy(&v37, pFreeBlock, sizeof(v37));
        auto v34 = this->CalculateMemoryRequired(pFreeBlock, pHeaderLocation - pFreeBlock, 0x10, this->m_uDefaultAlignment, a6);
        if (v34 <= uSizeBefore && uSizeBefore - v34 >= this->m_uField0x24) {
            v37.m_pBlockStart = pUserMem - sizeof(MemoryEntry);
            v37.m_uDataLength -= uSizeBefore;
            CHeap::PrepareHeader(pFreeBlock, this, EMemoryBlockType::FRE);
            pFreeBlock->m_pBlockStart = pFreeBlock;
            pFreeBlock->m_uDataLength = uSizeBefore;
            this->AddFreeBlock(pFreeBlock);
            pFreeBlock = nullptr;
        }

        CHeap::PrepareHeader_ChangeUsage(&v37, EMemoryBlockType::FSG);
        v37.m_uUnalignedLength = uDataLength;
        v37.m_pPrevious = nullptr;
        v37.m_pNext = nullptr;
        // Todo: v37.m_cBlockType[3] = (v13/a5? << 6) & 0x40 | v37.m_cBlockType[3] & 0xBF;
        if (pFreeBlock)
            CHeap::PrepareHeader(pFreeBlock, this, EMemoryBlockType::INV);
        memcpy(&pUserMem - sizeof(MemoryEntry), &v37, sizeof(MemoryEntry));
        this->AddAllocatedBlock(pUserMem - 1);

        FSG_ASSERT(FSG_IS_ALIGNED(pUserMem, uUserAlignment), "CHeap::InternalAllocate() [s] - ERROR Allocation logic error, memory start is incorrectly aligned.");

        pReturnVal = pUserMem;
    } else {
        if (this->m_aSnapshots[0].m_strPrevHeapName[0] || this->m_uField0xab4) {
            if (!this->m_uField0x54)
                FSGDoLog("CHeap::InternalAllocate() [%s] - ERROR Memory allocation failed for %i bytes, please check heap size.\n", this->m_szHeapName, uDataLength);
        }
        FSG_ASSERT(this->m_uField0x54 != 0, "CHeap::InternalAllocate() [s] - ERROR Memory allocation failed, please check heap size.");

        pReturnVal = nullptr;
        this->m_uNumFailedAllocationRequests++;
    }

    delete pLock;
    return pReturnVal;
}

MemoryEntry *CHeap::FindFreeBlock(size_t uDataLength, unsigned int uUserAlignment, int a3) {
    // Todo: Function requires clean-up.

    MemoryEntry *v14 = nullptr;
    auto pFreeBlock = this->m_pFirstBlock;
    if (a3) {
        for (MemoryEntry *pBlock = pFreeBlock->m_pNext; pBlock; pBlock = pBlock->m_pNext)
            pFreeBlock = pBlock;
    }

    FSG_ASSERT(uUserAlignment >= m_uDefaultAlignment,
               "CHeap::FindFreeBlock() - ERROR - user alignment less than default heap alignment\n");
    while (true) {
        // Todo: Add proper VA support to FSG_ASSERT
        FSG_ASSERT(((pFreeBlock->m_pNext == nullptr) || ((((void *) pFreeBlock->m_pNext) >= this->GetMemoryBuffer()) &&
                                                         (((uintptr_t *) pFreeBlock->m_pNext) <
                                                          (((uintptr_t *) this->GetMemoryBuffer()) +
                                                           this->GetHeapSize())))), "next pointer [p] is bad!");

        FSG_ASSERT(
                ((pFreeBlock->m_pPrevious == nullptr) || ((((void *) pFreeBlock->m_pPrevious) >= GetMemoryBuffer()) &&
                                                          (((uintptr_t *) pFreeBlock->m_pPrevious) <
                                                           (((uintptr_t *) GetMemoryBuffer()) + GetHeapSize())))),
                "previous pointer [p] is bad!");


        auto uBlockLength = pFreeBlock->m_uDataLength;
        if (uDataLength > uBlockLength) {
            if (!a3) {
                pFreeBlock = pFreeBlock->m_pNext;
            } else {
                pFreeBlock = pFreeBlock->m_pPrevious;
            }

            if (!pFreeBlock)
                return a3 != 0 ? 0 : v14;
        }

        auto uMemoryRequired = this->CalculateMemoryRequired(pFreeBlock, uBlockLength, uDataLength, uUserAlignment, a3);
        auto uMemoryAvailable = pFreeBlock->m_uDataLength;

        if (uMemoryRequired > uMemoryAvailable) {
            if (!a3) {
                pFreeBlock = pFreeBlock->m_pNext;
            } else {
                pFreeBlock = pFreeBlock->m_pPrevious;
            }

            if (!pFreeBlock)
                return a3 != 0 ? 0 : v14;
        }

        if (a3)
            return pFreeBlock;

        if (v14 && uMemoryAvailable >= v14->m_uDataLength) {
            pFreeBlock = pFreeBlock->m_pNext;
        }

        if (!a3) {
            pFreeBlock = pFreeBlock->m_pNext;
        } else {
            pFreeBlock = pFreeBlock->m_pPrevious;
        }

        if (!pFreeBlock)
            return a3 != 0 ? 0 : v14;

    }
}

void *CHeap::GetMemoryBuffer() {
    return this->m_pMemoryBuffer;
}

size_t CHeap::GetHeapSize() {
    return this->m_uDataLength;
}

void CHeap::AddFreeBlock(MemoryEntry *pFreeItem) {
    FSG_ASSERT(pFreeItem != nullptr, "A nullptr block was specified for addition to the CHeap free list.");

    MemoryEntry * v6 = nullptr;
    pFreeItem->m_pNext = nullptr;
    pFreeItem->m_pPrevious = nullptr;

    auto pBlock = this->m_pFirstBlock;
    if (pBlock) {
        while ((uintptr_t)pFreeItem >= (uintptr_t)pBlock) {
            if (!pBlock->m_pNext) {
                pFreeItem->m_pPrevious = pBlock;
                pBlock->m_pNext = pFreeItem;
                this->m_uFreeBlocks++;
                this->CombineFreeBlocks(pBlock, pFreeItem);
                return;
            }
            v6 = pBlock;
            pBlock = pBlock->m_pNext;
        }
        if (v6)
            v6->m_pNext = pFreeItem;
        else
            this->m_pFirstBlock = pFreeItem;

        pFreeItem->m_pNext = pBlock;
        pFreeItem->m_pPrevious = v6;
        pBlock->m_pPrevious = pFreeItem;
        this->m_uFreeBlocks++;
        this->CombineFreeBlocks(pFreeItem, pBlock);

        if (v6) {
            pBlock = v6;
            this->CombineFreeBlocks(pBlock, pFreeItem);
        } else {
            this->m_pFirstBlock = pFreeItem;
            this->m_uFreeBlocks++;
        }
    }
}

int CHeap::CombineFreeBlocks(MemoryEntry * pFirstEntry, MemoryEntry * pSecondEntry) {
    FSG_ASSERT(pFirstEntry != nullptr, "An attempt was made to combine a nullptr memory block.");
    FSG_ASSERT(pSecondEntry != nullptr, "An attempt was made to combine a nullptr memory block.");
    FSG_ASSERT(pFirstEntry < pSecondEntry, "The first entry of a combine request did not come first in memory.");
    FSG_ASSERT(pFirstEntry == pFirstEntry->m_pBlockStart, nullptr);
    FSG_ASSERT(pSecondEntry == pSecondEntry->m_pBlockStart, nullptr);

    auto uDataLength = pFirstEntry->m_uDataLength;
    if ((uintptr_t)pFirstEntry->m_pBlockStart + uDataLength != (uintptr_t)pSecondEntry->m_pBlockStart) {
        return false;
    }
    pFirstEntry->m_uDataLength = pSecondEntry->m_uDataLength + uDataLength;
    pFirstEntry->m_pNext = pSecondEntry->m_pNext;
    if (pSecondEntry->m_pNext)
        pSecondEntry->m_pNext->m_pPrevious = pFirstEntry;
    this->m_uFreeBlocks--;
    return true;
}

void CHeap::AddAllocatedBlock(MemoryEntry *pBlock) {
    this->m_uNumSuccessfulAllocations++;
    this->m_uLiveAllocations++;

    this->m_uField0x40 = pBlock->m_uDataLength + m_uField0x40;

    this->m_uUnalignedLength += pBlock->m_uUnalignedLength;
    if (this->m_uUnalignedLength > this->m_uField0x44)
        this->m_uField0x44 = this->m_uUnalignedLength;

    if (this->m_uField0x40 > this->m_uField0x48)
        this->m_uField0x48 = this->m_uField0x40;

    pBlock->m_pPrevious = nullptr;
    pBlock->m_pNext = this->m_pFirstAllocatedBlock;

    if (this->m_pFirstAllocatedBlock)
        this->m_pFirstAllocatedBlock->m_pPrevious = pBlock;

    this->m_pFirstAllocatedBlock = pBlock;
}


void CHeapSnapshot::Create(CHeap *pHeap, unsigned int uIndex, char const *szHeapName, char const *szPrevHeapName,
                           unsigned int uPrevIndex) {
    this->m_pHeap = pHeap;
    this->m_uIndex = uIndex;

    if (szHeapName) {
        CString::StringCopy(this->m_strHeapName, MAX_HEAP_NAME, szHeapName);
    } else {
        this->m_strHeapName[0] = '\0';
    }

    if (szPrevHeapName) {
        CString::StringCopy(this->m_strPrevHeapName, MAX_HEAP_NAME, szPrevHeapName);
    } else {
        this->m_strPrevHeapName[0] = '\0';
    }

    this->m_uPrevIndex = uPrevIndex;
}
