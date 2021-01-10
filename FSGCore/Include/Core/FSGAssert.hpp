//
// Created by lucas on 2020-12-28.
//

#pragma once

#include <cstdio>

// Todo: Add formatting to FSG_ASSERT
#define FSG_ASSERT(EXPRESSION, MESSAGE, ...) do { if (!(EXPRESSION)) { printf("\nERROR:\n  %s\n  %s\n  %s:%d\n  %s\n \n", #EXPRESSION, MESSAGE == nullptr ? "" : MESSAGE, __VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); abort(); }} while (0)

#define FSG_IS_ALIGNED(OFFSET, ALIGNMENT) (reinterpret_cast<uintptr_t>(OFFSET) & (ALIGNMENT - 1)) == 0