#include <lib/string.h>

size_t strlen(const char *string)
{
    size_t i = 0;
    while (string[i] != '\0')
        i++;
    return i;
}

// 为了标准 我妥协了
bool memcmp(const char *str1, const char *str2, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (str1[i] != str2[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}

// 先空着 等重构完内存管理再写
void memcopy(void *dest, const void *src, size_t size)
{
}

void setmem(void *dest, size_t length, uint8_t val)
{
    for (size_t i = 0; i < length; i++)
    {
        uint8_t *cube = (uint8_t *)dest + i;
        *cube = val;
    }
}