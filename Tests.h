#pragma once
#include "PoolAllocator.h"
#include <cassert>
#include <tuple>
#include <random>
#include <iostream>
#include <algorithm>

struct TestStruct
{
    int dummyMember;
};

inline void single_test()
{
    PoolAllocator<TestStruct, 1> pa;
    auto [ts, i] = pa.Alloc();
    assert(ts != nullptr);
    assert(i == 0);
    ts->dummyMember = 25;

    auto [ts2, i2] = pa.Alloc();
    assert(ts2 == nullptr);
    assert(i2 == pa.kInvalidIndex());
    
    pa.Free(i);
    
    auto [ts3, i3] = pa.Alloc();
    assert(ts3 != nullptr);
    assert(i == 0);
    assert(ts3->dummyMember == 25);
    
    auto [ts4, i4] = pa.Alloc();
    assert(ts4 == nullptr);
    assert(i4 == pa.kInvalidIndex());
    
    pa.Free(i);
}

inline void two_test()
{
    PoolAllocator<TestStruct, 2> pa;
    auto [t, i] = pa.Alloc();
    assert(t != nullptr);
    assert(i == 0);
    t->dummyMember = 71;

    std::tie(t, i) = pa.Alloc();
    assert(t != nullptr);
    assert(i == 1);
    t->dummyMember = 16;

    std::tie(t, i) = pa.Alloc();
    assert(t == nullptr);
    assert(i == pa.kInvalidIndex());

    std::tie(t, i) = pa.Alloc();
    assert(t == nullptr);
    assert(i == pa.kInvalidIndex());

    pa.Free(0);

    std::tie(t, i) = pa.Alloc();
    assert(t != nullptr);
    assert(i == 0);
    assert(t->dummyMember == 71);

    std::tie(t, i) = pa.Alloc();
    assert(t == nullptr);
    assert(i == pa.kInvalidIndex());

    pa.Free(1);

    std::tie(t, i) = pa.Alloc();
    assert(t != nullptr);
    assert(i == 1);
    assert(t->dummyMember == 16);
}

template<uint16_t S>
void test1()
{
    PoolAllocator<TestStruct, S> pa;

    // Alloc exactly S times
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
        assert(i == k);
        t->dummyMember = k * 2;
    }

    // Verify that we can't allocate more
    {
        auto [t, i] = pa.Alloc();
        assert(t == nullptr);
        assert(i == pa.kInvalidIndex());
    }

    // Free exactly S times, in opposite order to restore internal free-list
    for (uint16_t k = 1; k <= S; k++)
    {
        pa.Free(S - k);
    }

    // Alloc exactly S times again, and verify we still have the dummyMembers
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
        assert(i == k);
        assert(t->dummyMember == k * 2);
    }

    // Verify that we can't allocate more
    {
        auto [t, i] = pa.Alloc();
        assert(t == nullptr);
        assert(i == pa.kInvalidIndex());
    }
}

inline void test_no_double_free()
{
    PoolAllocator<TestStruct, 1> pa;

    auto [t, i] = pa.Alloc();
    assert(t != nullptr);
    assert(i == 0);

    pa.Free(i);
    pa.Free(i);

    // Verify that we can allocate again after a double free
    std::tie(t, i) = pa.Alloc();
    assert(t != nullptr);
    assert(i == 0);
}

template<uint16_t S>
inline void test_no_double_free_n()
{
    PoolAllocator<TestStruct, S> pa;

    // Fully alloc
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
        assert(i == k);
    }

    // Fully free twice
    for (uint16_t k = 0; k < S; k++)
    {
        pa.Free(k);
        pa.Free(k);
    }

    // Verify that we can fully alloc everything again
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, _] = pa.Alloc();
        assert(t != nullptr);
    }
}

template<uint16_t S>
void random_test()
{
    PoolAllocator<TestStruct, S> pa;

    // Alloc exactly S times
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
        assert(i == k);
    }

    // Free everything twice
    for (uint16_t k = 0; k < S; k++)
    {
        pa.Free(k);
        pa.Free(k);
    }

    // Verify that we can alloc again S times
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
        assert(i == S - k - 1);
    }
}

template<uint16_t S>
void random_order_free()
{
    PoolAllocator<TestStruct, S> pa;

    // Alloc exactly S times
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
    }

    // random free order
    std::vector<uint16_t> indices;
    for (uint16_t i = 0; i < S; i++)
    {
        indices.push_back(i);
    }
    std::random_device rd;
    std::default_random_engine reng {rd()};
    std::shuffle(indices.begin(), indices.end(), reng);

    // Free in randomly shuffled order
    for (uint16_t i = 0; i < S; i++)
    {
        pa.Free(indices[i]);
    }

    // Verify that we can alloc again S times
    for (uint16_t k = 0; k < S; k++)
    {
        auto [t, i] = pa.Alloc();
        assert(t != nullptr);
    }

    // Verify that we cannot alloc anymore
    auto [t, i] = pa.Alloc();
    assert(t == nullptr);
}
