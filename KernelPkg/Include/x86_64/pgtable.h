#ifndef _PGTABLE_H_
#define _PGTABLE_H_

#include <datatype.h>
#include <mm/zone.h>

#if defined(__i386__)

page_t *alloc_one_pd(mmzone_t* zone);
page_t *alloc_one_pt(mmzone_t* zone);

#elif defined(__x86_64__)

page_t *alloc_one_pml4(mmzone_t* zone);
page_t *alloc_one_pdpt(mmzone_t* zone);
page_t *alloc_one_pd(mmzone_t* zone);
page_t *alloc_one_pt(mmzone_t* zone);

#endif

#endif