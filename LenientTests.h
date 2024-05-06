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

inline void lenient_test_alloc_too_much()
{
    LenientPoolAllocator<LenientTestStruct, 1> pa;
    LenientTestStruct* t = pa.Alloc();
    assert(t != nullptr);
    assert(t->index == 0);

    t = pa.Alloc();
    assert(t == nullptr);
}

inline void lenient_test_alloc_free_alloc()
{
    LenientPoolAllocator<LenientTestStruct, 1> pa;
    LenientTestStruct* t = pa.Alloc();
    assert(t != nullptr);
    assert(t->index == 0);
    t->dummyValue = 43;

    pa.Free(t);

    t = pa.Alloc();
    assert(t != nullptr);
    assert(t->index == 0);
    assert(t->dummyValue == 43);
}

template<uint16_t Size>
inline void lenient_test_alloc_n()
{
    LenientPoolAllocator<LenientTestStruct, Size> pa;

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
    }

    LenientTestStruct* t = pa.Alloc();
    assert(t == nullptr);
}

template<uint16_t Size>
inline void lenient_test_alloc_free_alloc_n()
{
    LenientPoolAllocator<LenientTestStruct, Size> pa;
    std::vector<LenientTestStruct*> v;
    v.reserve(Size);

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
        v.push_back(t);
    }

    {
        LenientTestStruct* t = pa.Alloc();
        assert(t == nullptr);
    }

    for (auto p : v)
    {
        pa.Free(p);
    }

    pa.Free(nullptr);
    v.clear();

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
    }

    {
        LenientTestStruct* t = pa.Alloc();
        assert(t == nullptr);
    }
}

template<uint16_t Size>
inline void lenient_test_no_double_free()
{
    LenientPoolAllocator<LenientTestStruct, Size> pa;
    std::vector<LenientTestStruct*> v;
    v.reserve(Size);

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
        v.push_back(t);
    }

    for (auto p : v)
    {
        pa.Free(p);
    }
    for (auto p : v)
    {
        pa.Free(p);
    }

    pa.Free(nullptr);
    v.clear();

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
    }
}

template<uint16_t Size>
inline void lenient_test_random_order_free()
{
    LenientPoolAllocator<LenientTestStruct, Size> pa;
    std::vector<LenientTestStruct*> v;
    v.reserve(Size);

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
        v.push_back(t);
    }

    std::random_device rd;
    std::default_random_engine reng {rd()};
    std::shuffle(v.begin(), v.end(), reng);

    for (auto p : v)
    {
        pa.Free(p);
    }
    pa.Free(nullptr);
    v.clear();

    for (uint16_t i = 0; i < Size; i++)
    {
        LenientTestStruct* t = pa.Alloc();
        assert(t != nullptr);
    }

    {
        LenientTestStruct* t = pa.Alloc();
        assert(t == nullptr);
    }
}
