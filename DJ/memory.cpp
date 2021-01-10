//
// Created by lucas on 2020-12-29.
//

#include <memory.hpp>
#include <Memory/FSGMemoryManager.hpp>
#include <cstdlib>

CHeap *g_pSmallHeap;
CHeap *g_p3DStringsHeap;
CHeap *g_pGlobalHeap;

void InitialiseMemoryManager() {
    void * pRawMemory;

    // Small Heap
    auto pSmallHeap = malloc(SMALL_HEAP_SIZE * HEAP_FACTOR);
    g_pSmallHeap = new CHeap(pSmallHeap, SMALL_HEAP_SIZE * HEAP_FACTOR, "global_small_alloc", 0, nullptr);

    // 3D Strings Heap
    auto p3DStringsHeap = malloc(THREED_STRINGS_HEAP_SIZE * HEAP_FACTOR);
    g_p3DStringsHeap = new CHeap(p3DStringsHeap, THREED_STRINGS_HEAP_SIZE * HEAP_FACTOR, "3d_strings_heap", 0, nullptr);

    // Global Alloc
    auto pGlobalAllocHeap = malloc(GLOBAL_HEAP_SIZE * HEAP_FACTOR);
    g_pGlobalHeap = new CHeap(pGlobalAllocHeap, GLOBAL_HEAP_SIZE * HEAP_FACTOR, "global_alloc", 0, nullptr);

    // Alignment?
    auto uOldField0x20 = g_pGlobalHeap->m_uDefaultAlignment;
    g_pGlobalHeap->m_uField0x24 = 0x80;
    if (uOldField0x20 > 0x80)
        g_pGlobalHeap->m_uField0x24 = uOldField0x20;

    // Assign heaps to memory manager.
    auto pMemoryManager = CMemoryManager::GetInstance();
    pMemoryManager->SetSmallHeap(g_pSmallHeap, 0x80);

    // Fixme: IMPLEMENT THIS CUNT
    pMemoryManager->PushDefaultHeap(g_pGlobalHeap);
    g_pSmallHeap->PushSnapshot("SDJ:Small", nullptr, 0);
    g_pGlobalHeap->PushSnapshot("SDJ:Global", nullptr, 0);
}

