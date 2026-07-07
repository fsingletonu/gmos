#include <lib/math.h>

uint8_t get_power_two(uint64_t data)
{
    uint8_t power = 0;
    while (data)
    {
        data >>= 1;
        power++;
    }
    return power - 1;
}