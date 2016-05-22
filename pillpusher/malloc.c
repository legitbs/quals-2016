#include "malloc.h"

m_manager memman_g;

void debug_print( void )
{
	pm_header thdr = NULL;
	pm_footer tftr = NULL;

	if ( memman_g.free_list == NULL ) {
		printf("Empty\n");
	}

	thdr = (pm_header)memman_g.free_list;

	while ( thdr != NULL ) {
		tftr = BLOCK_FOOTER(thdr);

		printf("Addr: %p Size: %lx Next: %p Prev: %p\n", thdr, thdr->size, tftr->pNext, tftr->pPrev);

		thdr = (pm_header)tftr->pNext;
	}

	return;
}

void *add_free_block( unsigned int size)
{
	void *new_block = NULL;
	pm_header thdr;
	pm_footer tftr;

	/// Adjust size+sizeof(malloc header) up to the nearest page
	size += sizeof( m_header );

	if ( size % PAGE_SIZE ) {
		size /= PAGE_SIZE;
		size += 1;
		size *= PAGE_SIZE;
	}

	new_block = mmap( NULL, size, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if ( new_block == NULL ) {
		exit(-1);
	}

	thdr = (pm_header)new_block;

	thdr->size = size - sizeof(m_header);

	tftr = BLOCK_FOOTER( new_block );

	/// Link in the new block
	tftr->pNext = NULL;
	tftr->pPrev = memman_g.free_list;

	memman_g.free_list = new_block;

	return new_block;
}


void *malloc( unsigned int size )
{
	void *freeWalker = NULL;
	void *final_alloc = NULL;
	void *new_block = NULL;
	unsigned int size_left = 0;

	pm_header thdr;
	pm_footer tftr;
	pm_header header_new_block;
	pm_footer footer_new_block;

	/// Each block needs to be a least the size of the footer structure
	if ( size < sizeof( m_footer ) ) {
		size = sizeof(m_footer);
	}

	/// Align to 8 bytes
	if ( size % 8 ) {
		size = ( size >> 3 ) + 1;
		size <<= 3;
	}

	/// Start at the head of the free list
	freeWalker = memman_g.free_list;

	while ( 1 ) {

		/// If there are no available blocks then allocate a new one.
		if ( freeWalker == NULL ) {
			freeWalker = add_free_block( size );
		}

		thdr = (pm_header)freeWalker;
		tftr = BLOCK_FOOTER( freeWalker );

		/// Check if the current block is large enough to fulfill the request
		/// If so then downsize as needed
		if ( ( thdr->size & ~3) >= size ) {
			final_alloc = freeWalker + sizeof(m_header);

			size_left = (thdr->size& ~3) - size;

			/// Set the in use flag
			thdr->size |= 1;

			// If there is room then create a new block
			if ( size_left >= sizeof(m_header) + sizeof( m_footer) ) {

				// Fix up the size
				thdr->size = size;
				thdr->size |= 1;

				// Set the flag indicating that there is a block following the previous one
				thdr->size |= 2;

				new_block = final_alloc + size;

				header_new_block = (pm_header)new_block;
				header_new_block->size = size_left - sizeof(m_header);
				footer_new_block = tftr;

				/// If this was the head of the list then fix it up
				if ( freeWalker == memman_g.free_list ) {

					memman_g.free_list = new_block;


					if ( footer_new_block->pNext != NULL ) {
						tftr = BLOCK_FOOTER( (void*)(footer_new_block->pNext) );
						tftr->pPrev = (pm_header)new_block;
					}

					footer_new_block->pPrev = NULL;
				} else {

					// Fix up next and prev
					if ( footer_new_block->pPrev != NULL ) {
						freeWalker = footer_new_block->pPrev;
						tftr = BLOCK_FOOTER( freeWalker );
						tftr->pNext = (pm_header)new_block;
					}

					if ( footer_new_block->pNext != NULL ) {
						freeWalker = footer_new_block->pNext;
						tftr = BLOCK_FOOTER( freeWalker);
						tftr->pPrev = (pm_header)new_block;
					}

				}

			} else {

				/// Just unlink and return this one
				if ( freeWalker == memman_g.free_list ) {
					memman_g.free_list = tftr->pNext;

					if ( memman_g.free_list ) {
						tftr = BLOCK_FOOTER( (void*)(memman_g.free_list) );
						tftr->pPrev = NULL;
					}

				} else {
					// Fix up next and prev
					if ( tftr->pPrev != NULL ) {
						freeWalker = tftr->pPrev;
						pm_footer unlink_ftr = BLOCK_FOOTER( freeWalker );
						unlink_ftr->pNext = tftr->pNext;
					}

					if ( tftr->pNext != NULL ) {
						freeWalker = tftr->pNext;

						pm_footer unlink_ftr = BLOCK_FOOTER( freeWalker);
						unlink_ftr->pPrev = tftr->pPrev;
					}
				}
			}

			/// Fix ups are done. Return the allocation
			return final_alloc;
		}

		freeWalker = (void*)tftr->pNext;

	}
}

void free( void *block )
{
	pm_header thdr;
	pm_header nhdr;
	pm_footer tftr;
	pm_footer nftr;
	unsigned long tsize = 0;

	if ( block == NULL ) {
		return;
	}

	thdr = block-sizeof(m_header);

	// Check the in use bit
	if ( !(thdr->size & 1) ) {
		return;
	}


	/// If there is a block following this one then check if it is in use. If not then coalesce
	if ( thdr->size & 2 ) {
		tsize = thdr->size >> 3;
		tsize <<= 3;

		nhdr = block + tsize;


		if ( !(nhdr->size & 1 ) ) {

			/// Update the sizes
			thdr->size += ( nhdr->size & ~3) + sizeof( m_header);

			/// If the next block does not have a block after it then remove the flag
			if ( !(nhdr->size & 2 ) ) {
				thdr->size ^= 2;
			}

			/// If the next block was the previous head then update that
			/// Otherwise the prev and next pointers need to be updated
			if ( memman_g.free_list == (void*)nhdr) {
				memman_g.free_list = (void*)thdr;	
			}

			// Update the next->prev if necessary
			tftr = BLOCK_FOOTER(thdr);

			if ( tftr->pNext ) {
				nftr = BLOCK_FOOTER(tftr->pNext);

				nftr->pPrev = thdr;
			}

			if ( tftr->pPrev ) {
				nftr = BLOCK_FOOTER(tftr->pPrev);

				nftr->pNext = thdr;
			}
			return;
		}
	}

	// If we reach here then it did not coalesce. Just stick it at the front
	tftr = BLOCK_FOOTER( (void*)thdr );

	/// Remove the in use flag
	thdr->size ^= 1;

	tftr->pNext = memman_g.free_list;
	tftr->pPrev = NULL;

	/// Base case
	if ( memman_g.free_list != NULL ) {
		nftr = BLOCK_FOOTER( memman_g.free_list );

		nftr->pPrev = thdr;
	}

	memman_g.free_list = thdr;

	return;
}
