#ifndef __MALLOC__
#define __MALLOC__

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct m_header {
	/// 8 byte aligned so 3 bits for flags
	unsigned long size;
} m_header, *pm_header;

typedef struct m_footer {
	m_header *pNext;
	m_header *pPrev;
} m_footer, *pm_footer;

typedef struct m_manager {
	void *free_list;
} m_manager, *pm_manager;

#define PAGE_SIZE	4096

#define BLOCK_FOOTER( block ) ((pm_footer)(  ((char *)block) + (( ( ((pm_header)block)->size & ~3) + sizeof(m_header) ) - sizeof( m_footer))))

void *dc_malloc( unsigned int size);
void dc_free( void*ptr);
void debug_print();

#endif
