#ifndef _STRING_H_
#define _STRING_H_

#include <datatype.h>

size_t strlen(const char *string);
bool memcmp(const char *str1, const char *str2, size_t length);
void memcopy(void *dest, const void *src, size_t size);
void setmem(void *dest, size_t length, uint8_t val);

#endif
