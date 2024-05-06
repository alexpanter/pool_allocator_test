#pragma once
#include "LenientPoolAllocator.h"
#include <cassert>
#include <tuple>
#include <random>
#include <iostream>
#include <algorithm>

struct LenientTestStruct
{
    int dummyValue;
    uint16_t index;
};

inline void lenient_test_alloc()
{
    LenientPoolAllocator<LenientTestStruct, 1> pa;
    LenientTestStruct* t = pa.Alloc();
    assert(t != nullptr);
    assert(t->index == 0);
}
