#include <lib/string.h>

size_t strlen(const char *string)
{
    size_t i = 0;
    while (string[i] != '\0')
        i++;
    return i;
}

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

/*
 * 这个实现不好 仅仅是能跑 没有达到工业级
 * 原因有如下：
 * 没有边界检查
 * 都是逐字节的 没有充分利用机器指令
 * 这一版的所有代码 都不过多考虑效率 先以跑通原型为主
 */
void memcopy(void *dest, const void *src, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        const uint8_t *in = (const uint8_t *)src + i;
        uint8_t *out = (uint8_t *)dest + i;
        *out = *in;
    }
}

void setmem(void *dest, size_t length, uint8_t val)
{
    for (size_t i = 0; i < length; i++)
    {
        uint8_t *cube = (uint8_t *)dest + i;
        *cube = val;
    }
}