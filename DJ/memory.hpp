//
// Created by lucas on 2020-12-29.
//

#pragma once

#include <Memory/FSGHeap.hpp>

#ifndef HEAP_FACTOR
#define HEAP_FACTOR 1
#endif

#define SMALL_HEAP_SIZE 0x300000
#define THREED_STRINGS_HEAP_SIZE 0x1800
#define GLOBAL_HEAP_SIZE 0x7b80000

extern CHeap * g_pSmallHeap;
extern CHeap * g_p3DStringsHeap;
extern CHeap * g_pGlobalHeap;

void InitialiseMemoryManager();