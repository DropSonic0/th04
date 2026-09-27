#pragma option -zCSHARED -3

#include <dos.h>

#include "src/shared/runtime/api.hpp"

// Each occupied or free block starts with one paragraph of bookkeeping.
// Data begins at the following segment, so callers receive a segment handle.
struct HeapBlock {
	unsigned used;
	unsigned next;
	unsigned allocation_id;
};

static unsigned heap_base;
static unsigned heap_limit;
static unsigned heap_top;
static unsigned heap_dos_owned;

static HeapBlock far *heap_block(unsigned segment)
{
	return (HeapBlock far *)MK_FP(segment, 0);
}

static void heap_bind(unsigned base, unsigned paragraphs, unsigned dos_owned)
{
	heap_base = base;
	heap_limit = base + paragraphs;
	heap_top = heap_limit;
	heap_dos_owned = dos_owned;
}

static int heap_assign_available(void)
{
	unsigned largest = 0;
	if(_dos_allocmem(0xFFFFu, &largest) == 0) {
		_dos_freemem(largest);
		return 0;
	}
	if(largest <= 256u) {
		return 0;
	}
	unsigned paragraphs = largest - 256u;
	unsigned base = 0;
	if(_dos_allocmem(paragraphs, &base) != 0) {
		return 0;
	}
	heap_bind(base, paragraphs, 1);
	return 1;
}

static unsigned heap_allocate(unsigned paragraphs)
{
	if(paragraphs == 0) {
		return 0;
	}
	if(!heap_base && !heap_assign_available()) {
		return 0;
	}
	const unsigned total = paragraphs + 1u;
	if(total <= paragraphs) {
		return 0;
	}

	unsigned current = heap_top;
	unsigned allocated_header = 0;
	while(current < heap_limit) {
		HeapBlock far *block = heap_block(current);
		const unsigned next = block->next;
		if(next <= current || next > heap_limit) {
			return 0;
		}
		if(!block->used && (next - current) >= total) {
			const unsigned remainder = next - current - total;
			if(remainder > 1u) {
				const unsigned tail = current + total;
				HeapBlock far *free_tail = heap_block(tail);
				free_tail->used = 0;
				free_tail->next = next;
				free_tail->allocation_id = 0;
				block->next = tail;
			}
			block->used = 1;
			block->allocation_id = 0;
			allocated_header = current;
			break;
		}
		current = next;
	}

	if(!allocated_header) {
		if((heap_top - heap_base) < total) {
			return 0;
		}
		allocated_header = heap_top - total;
		HeapBlock far *block = heap_block(allocated_header);
		block->used = 1;
		block->next = heap_top;
		block->allocation_id = 0;
		heap_top = allocated_header;
	}
	return allocated_header + 1u;
}

extern "C" void TH04_PASCAL mem_assign(unsigned top_seg, unsigned parasize)
{
	heap_bind(top_seg, parasize, 0);
}

extern "C" void TH04_PASCAL mem_assign_all(void)
{
	if(!heap_base) {
		heap_assign_available();
	}
}

extern "C" int TH04_PASCAL mem_assign_dos(unsigned parasize)
{
	if(heap_base || parasize == 0) {
		return -8;
	}
	unsigned base = 0;
	const unsigned error = _dos_allocmem(parasize, &base);
	if(error) {
		return -(int)error;
	}
	heap_bind(base, parasize, 1);
	return 0;
}

extern "C" int TH04_PASCAL mem_unassign(void)
{
	if(!heap_base) {
		return 1;
	}
	const unsigned base = heap_base;
	const unsigned dos_owned = heap_dos_owned;
	heap_base = 0;
	heap_limit = 0;
	heap_top = 0;
	heap_dos_owned = 0;
	if(dos_owned && _dos_freemem(base) != 0) {
		return 0;
	}
	return 1;
}

extern "C" void __seg *TH04_PASCAL hmem_alloc(unsigned parasize)
{
	return (void __seg *)heap_allocate(parasize);
}

extern "C" void __seg *TH04_PASCAL hmem_allocbyte(unsigned bytesize)
{
	unsigned paragraphs = (bytesize >> 4);
	if(bytesize & 0xF) {
		paragraphs++;
	}
	return (void __seg *)heap_allocate(paragraphs);
}

extern "C" void TH04_PASCAL hmem_free(void __seg *memseg)
{
	if(!heap_base || !memseg) {
		return;
	}
	const unsigned data_segment = (unsigned)memseg;
	if(data_segment <= heap_top || data_segment > heap_limit) {
		return;
	}
	const unsigned target = data_segment - 1u;
	unsigned previous = 0;
	unsigned current = heap_top;
	while(current < heap_limit) {
		HeapBlock far *block = heap_block(current);
		const unsigned next = block->next;
		if(next <= current || next > heap_limit) {
			return;
		}
		if(current == target) {
			if(!block->used) {
				return;
			}
			block->used = 0;
			if(next < heap_limit) {
				HeapBlock far *after = heap_block(next);
				if(!after->used) {
					block->next = after->next;
				}
			}
			if(previous) {
				HeapBlock far *before = heap_block(previous);
				if(!before->used) {
					before->next = block->next;
				}
			}
			while(heap_top < heap_limit) {
				HeapBlock far *top = heap_block(heap_top);
				if(top->used) {
					break;
				}
				heap_top = top->next;
			}
			return;
		}
		previous = current;
		current = next;
	}
}
