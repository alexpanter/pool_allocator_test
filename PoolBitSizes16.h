#pragma once
#include <cstdint>

/**
 * @brief Collection of useful bitfields / values for 16-bit pool allocators
 * with explicit, indexed free-lists.
 */
class PoolBitSizes16
{
public:
    static constexpr uint16_t kInvalidIndex = 0x7FFFU; // 01111111 11111111
    static constexpr uint16_t kMaxSize = 0x7FFEU;      // 01111111 11111110 = 32766
    static constexpr uint16_t kAllocatedBit = 0x8000U; // 10000000 00000000

    static constexpr bool has_allocated_bit(uint16_t num)
    {
        return num & kAllocatedBit;
    }

    static constexpr uint16_t add_allocated_bit(uint16_t num)
    {
        return num | kAllocatedBit;
    }

    static constexpr uint16_t remove_allocated_bit(uint16_t num)
    {
        return num & kInvalidIndex;
    }

    static constexpr bool index_is_nil(uint16_t num)
    {
        return (num & kInvalidIndex) == kInvalidIndex;
    }
};
