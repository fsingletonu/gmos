#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#define DEBUG asm volatile("nop")

#define TRUE 1
#define FALSE 0

#define NULL 0

// 布尔类型
typedef unsigned char bool;

// 标准整数类型
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// 默认数据类型
typedef uint64_t size_t;

// 地址类型地址
typedef uint64_t addr_t;
typedef addr_t phys_addr;
typedef addr_t virt_addr;

#endif
