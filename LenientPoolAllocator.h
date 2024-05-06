#pragma once
#include <cstdint>
#include <iostream>
#include <utility>
#include "PoolBitSizes16.h"

// Same as PoolAllocator<>, except it always returns a valid allocation!

template<typename T, uint16_t Size>
class LenientPoolAllocator
{
    static_assert(Size > 0U);
    static_assert(Size < 0x7FFEU); // < 32766

public:
    LenientPoolAllocator()
    {
        for (uint16_t i = 0; i < Size; i++)
        {
            mPool[i] = new T();
            mFreeList[i] = i + 1;
        }
        mFreeList[Size - 1] = 0x7FFFU;
        mFreeIndex = 0;
    }

    T* Alloc()
    {
        // If the free index has allocated bit means we can't allocate more
        if (PoolBitSizes16::index_is_nil(mFreeIndex))
        {
            return { nullptr };
        }

        auto [ret, index] = FindComponentInPool();

        if (!ret || index >= PoolBitSizes16::kMaxSize)
        {
            std::cout
                << "Pool has no more components! Consider increasing pool size!"
                << std::endl;
            ret = new T();
            index = PoolBitSizes16::kInvalidIndex;
        }

        ret->index = index;
        return ret;
    }

    void Free(T* component)
    {
        if (component == nullptr) { return; }

        uint16_t index = component->index;
        uint16_t sanitized = PoolBitSizes16::remove_allocated_bit(index);

        // Check if either index is out-of-bounds, or if has already been freed.
        if (!PoolBitSizes16::has_allocated_bit(mFreeList[sanitized]))
        {
            std::cout << "Component already freed! Index: " << index
                << ", sanitized index: " << sanitized << std::endl;
            component->index = PoolBitSizes16::kInvalidIndex;
            delete component;
            return;
        }

        if (index >= Size)
        {
            std::cout << "Component has invalid index: " << index
                << ", which indicates non-pool allocated! Directly destroys."
                << std::endl;
            component->index = PoolBitSizes16::kInvalidIndex;
            delete component;
            return;
        }

        mFreeList[sanitized] = mFreeIndex;
        mFreeIndex = index;
    }

private:
    std::pair<T*, uint16_t> FindComponentInPool()
    {
        if (PoolBitSizes16::index_is_nil(mFreeIndex))
        {
            return { nullptr, PoolBitSizes16::kInvalidIndex };
        }

        const uint16_t outIndex = PoolBitSizes16::remove_allocated_bit(mFreeIndex);
        mFreeIndex = mFreeList[outIndex];
        mFreeList[outIndex] = PoolBitSizes16::add_allocated_bit(mFreeIndex);

        return { mPool[outIndex], outIndex };
    }

    T* mPool[Size];
    uint16_t mFreeList[Size];
    uint16_t mFreeIndex;
};