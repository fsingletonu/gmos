#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#define EOF -1

#define nullptr 0

#define NULL 0

#define EOS '\0'

#define true 1
#define false 0

#define page_aligned 4096

#define _packed __attribute__((packed))
#define _page_aligned __attribute__((aligned(page_aligned)))
#define _bitwise __attribute__((bitwise))
#define _aligned(size) __attribute__((aligned(size)))

typedef unsigned char bool;

typedef unsigned int size_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define MACHINE_MEMORY_4G
#ifdef MACHINE_MEMORY_4G
typedef uint32_t phys_addr_t;
#else 
typedef uint64_t phys_addr_t;
#endif

typedef void *handler_t;
typedef void *addr_t;

#define DEBUG asm volatile("nop")
#define CODE_ALIGN asm volatile("nop");

#endif