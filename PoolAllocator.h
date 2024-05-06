#pragma once
#include <cstdint>
#include <utility>
#include "PoolBitSizes16.h"

template<typename T, uint16_t Size>
class PoolAllocator
{
    static_assert(Size > 0U);
    static_assert(Size < 0x7FFEU); // < 32766

public:
    constexpr uint16_t kInvalidIndex() { return 0x7FFFU; }

    PoolAllocator()
    {
        for (uint16_t i = 0; i < Size; i++)
        {
            mPool[i] = T{};
            mFreeList[i] = i + 1;
        }
        mFreeList[Size - 1] = 0x7FFFU;
        mFreeIndex = 0;
    }

    std::pair<T*, uint16_t> Alloc()
    {
        // If the free index has allocated bit means we can't allocate more
        if (PoolBitSizes16::index_is_nil(mFreeIndex))
        {
            return { nullptr, kInvalidIndex() };
        }

        const uint16_t outIndex = PoolBitSizes16::remove_allocated_bit(mFreeIndex);
        mFreeIndex = mFreeList[outIndex];
        mFreeList[outIndex] = PoolBitSizes16::add_allocated_bit(mFreeIndex);

        return { &mPool[outIndex], outIndex };
    }

    void Free(uint16_t index)
    {
        uint16_t sanitized = PoolBitSizes16::remove_allocated_bit(index);

        // Check if either index is out-of-bounds, or if has already been freed.
        if (index >= Size || !PoolBitSizes16::has_allocated_bit(mFreeList[sanitized]))
        {
            return;
        }

        mFreeList[sanitized] = mFreeIndex;
        mFreeIndex = index;
    }

private:
    T mPool[Size];
    uint16_t mFreeList[Size];
    uint16_t mFreeIndex;
};
