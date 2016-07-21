// klav_objstack.cpp
//
//

#include <klava/klav_objstack.h>

#include <string.h>

#ifdef _MSC_VER
# pragma intrinsic(memcpy)
#endif


// Determine default alignment
//namespace Private { struct FooAlign {char c; uint64_t ll;};}
//#define DEFAULT_ALIGNMENT
//	((ptrdiff_t) ((char *) &((Private::FooAlign *) 0)->ll - (char *) 0))

#define OBJSTACK_ALIGNMENT (2 * sizeof (void *))
#define OBJSTACK_ALIGNMENT_MASK (OBJSTACK_ALIGNMENT - 1)

#define ALIGN(P,A) ((P + ((A)-1)) & ~((A)-1))

#define ALLOC_EXTRA ALIGN(ALIGN(12,OBJSTACK_ALIGNMENT)+4,OBJSTACK_ALIGNMENT)
#define DEFAULT_CHUNK_SIZE (4096-ALLOC_EXTRA)

KLAV_ObjStack_Impl::KLAV_ObjStack_Impl (KLAV_Alloc *allocator, uint32_t chunk_size) :
		m_allocator (0),
		m_chunk (0),
		m_last_item (0),
		m_next_free (0),
		m_first_obj (0),
		m_last_obj (0),
		m_chunk_size (DEFAULT_CHUNK_SIZE)
{
	if (allocator != 0)
		init (allocator, chunk_size);
}

KLAV_ObjStack_Impl::~KLAV_ObjStack_Impl ()
{
	deinit ();
}

uint8_t * KLAV_ObjStack_Impl::grow_last (size_t size)
{
	const size_t oldsize = get_item_size (m_last_item);
	return realloc_last (oldsize + size) + oldsize;
}

uint8_t * KLAV_ObjStack_Impl::realloc_last (size_t new_size)
{	
	if (m_chunk != 0 && new_size <= (size_t)(m_chunk->hdr.m_end - m_last_item))
	{
		// fast path: just increment block size
		set_item_size (m_last_item, new_size);
		m_next_free = m_last_item + new_size;
		return m_last_item;
	}

	// slow path: allocate new chunk and copy data
	const uint8_t * old_data = m_last_item;
	const size_t old_size = get_item_size (m_last_item);

	if (add_new_chunk (new_size) == 0) return 0;
	memcpy (m_last_item, old_data, old_size);

	return m_last_item;
}

uint8_t * KLAV_ObjStack_Impl::alloc (size_t size)
{
	if (size == 0)
		size = 1;

	// freeze the last allocated object
	// grow m_next_free to alignment address + size block
	if (m_chunk != 0)
	{
		uint8_t * next_free = (uint8_t *) ALIGN ((uintptr_t) m_next_free, OBJSTACK_ALIGNMENT);
		if (next_free < m_chunk->hdr.m_end && (size + sizeof (ItemHdr)) <= (size_t)(m_chunk->hdr.m_end - next_free))
		{
			// fast path - allocate block in the same chunk
			m_last_item = next_free + sizeof (ItemHdr);
			m_next_free = m_last_item + size;
			set_item_size (m_last_item, size);
			::memset (m_last_item, 0, size);
			return m_last_item;
		}
	}

	// slow path: allocate new chunk
	if (add_new_chunk (size) == 0)
		return 0;
		
	::memset (m_last_item, 0, size);
	return m_last_item;
}

uint8_t * KLAV_ObjStack_Impl::realloc (void *ptr, size_t newsize)
{
	// realloc only the last chunk
	if (ptr == 0)
		return alloc (newsize);

	if (ptr == m_last_item)
		return realloc_last (newsize);

	const size_t oldsize = get_item_size (ptr);
	if (oldsize <= newsize)
		return (uint8_t *) ptr;

	uint8_t * nptr = alloc (newsize);
	if (nptr != 0)
	{
		memcpy (nptr, ptr, oldsize);
	}
	return nptr;
}

void KLAV_ObjStack_Impl::free (void *ptr)
{
	// free only the last chunk
	if (ptr == 0 || ptr != m_last_item)
		return;
	
	// really, the block is not freed anyway
	realloc_last (0);
}

void KLAV_ObjStack_Impl::clear (void* from_item)
{
	if (from_item == 0)
	{
		clear ();
		return;
	}

	// free chunks, while block is not in the current chunk
	while (! m_chunk->hdr.is_first () && 
		(from_item <= (void *)m_chunk || from_item >= (void *)m_chunk->hdr.m_end))
	{
		delete_last_chunk ();
	}
	
//	if (block <= (void *)m_chunk || block >= (void *)m_chunk->end)
//		// throw some exception
	
	m_last_item = (uint8_t *)from_item;
	m_next_free = m_last_item + get_item_size (m_last_item);
}

void KLAV_ObjStack_Impl::clear ()
{
	if (m_chunk == 0)
		return;

	while (! m_chunk->hdr.is_first ())
	{
		delete_last_chunk ();
	}
	init_last_chunk (0);
}

uint8_t * KLAV_CALL KLAV_ObjStack_Impl::last_block ()
{
	return m_last_item;
}

size_t KLAV_CALL KLAV_ObjStack_Impl::block_size (void* ptr)
{
	return get_item_size (ptr);
}

void KLAV_ObjStack_Impl::init (KLAV_Alloc *allocator, uint32_t chunk_size)
{
	m_allocator = allocator;

	if (chunk_size == 0)
		chunk_size = DEFAULT_CHUNK_SIZE;
	
	m_chunk_size = chunk_size;
}

void KLAV_ObjStack_Impl::deinit ()
{
	destroy_objects (0);

	if (m_chunk != 0)
	{
		// deallocate all chunks except the first
		clear ();
		// deallocate first chunk
		delete_last_chunk ();
	}

	m_chunk = 0;
	m_last_item = 0;
	m_next_free = 0;
	m_chunk_size = DEFAULT_CHUNK_SIZE;
}

KLAV_ObjStack_Impl::Chunk* KLAV_ObjStack_Impl::add_new_chunk (size_t first_item_size)
{
	// Compute size for new chunk, aligning to 4K - ALLOC_EXTRA
	size_t new_chunk_size = ((sizeof (Chunk) + sizeof (ItemHdr) + first_item_size + ALLOC_EXTRA + 0xFFF) & ~0xFFF) - ALLOC_EXTRA;
	if (new_chunk_size < m_chunk_size)
		new_chunk_size = m_chunk_size;
	
	// Allocate and initialize the new chunk.
	Chunk * new_chunk = (Chunk *)(allocator ()->alloc (new_chunk_size));
	// check for out-of-memory condition
	if (new_chunk == 0)
		return 0;

	new_chunk->hdr.m_end = (uint8_t *)(new_chunk) + new_chunk_size;
	new_chunk->hdr.m_prev = m_chunk;

	m_chunk = new_chunk;
	init_last_chunk (first_item_size);
	
	return m_chunk;
}

void KLAV_ObjStack_Impl::init_last_chunk (size_t first_item_size)
{
	m_last_item = (uint8_t *)m_chunk + sizeof (Chunk) + sizeof (ItemHdr);
	m_next_free = m_last_item + first_item_size;
	set_item_size (m_last_item, first_item_size);
}

void KLAV_ObjStack_Impl::delete_last_chunk ()
{
	Chunk *chunk = m_chunk;
	m_chunk = chunk->hdr.m_prev;

	// NOTE!!! m_last_block and m_next_free are undefined now!!!
	allocator ()->free (chunk);
}

////////////////////////////////////////////////////////////////
// object deallocator

KLAV_ERR KLAV_ObjStack_Impl::register_object (KLDynObject *obj)
{
	if (obj == 0)
		return KLAV_EINVAL;

	// TODO: optionally (debug mode) check if the object is already registered
	if (add_object_to_list (obj) == 0)
		return KLAV_ENOMEM;

	return KLAV_OK;
}

KLAV_ERR KLAV_ObjStack_Impl::unregister_object (KLDynObject *obj)
{
	if (obj == 0)
		return KLAV_EINVAL;

	DynObj *p = m_last_obj;

	for (; p != 0; p = p->m_prev)
	{
		if (p->m_obj == obj)
		{
			remove_object_from_list (p);
			return KLAV_OK;
		}
	}
	return KLAV_ENOTFOUND;
}

void KLAV_CALL KLAV_ObjStack_Impl::destroy_objects (KLDynObject *lastobj)
{
	while (m_last_obj != 0)
	{
		KLDynObject *obj = m_last_obj->m_obj;
		remove_object_from_list (m_last_obj);

		obj->destroy ();

		if (lastobj != 0 && obj == lastobj)
			break;
	}
}

KLAV_ObjStack_Impl::DynObj *
KLAV_ObjStack_Impl::add_object_to_list (KLDynObject *obj)
{
	DynObj *p = (DynObj *) alloc (sizeof (DynObj));
	if (p == 0)
		return 0;

	p->m_obj = obj;
	p->m_next = 0;
	p->m_prev = m_last_obj;
	m_last_obj = p;

	if (p->m_prev == 0)
		m_first_obj = p;
	else
		p->m_prev->m_next = p;

	return p;
}

void KLAV_ObjStack_Impl::remove_object_from_list (DynObj *p)
{
	if (p->m_prev != 0)
		p->m_prev->m_next = p->m_next;
	else
		m_first_obj = p->m_next;		

	if (p->m_next != 0)
		p->m_next->m_prev = p->m_prev;
	else
		m_last_obj = p->m_prev;

	p->m_prev = p->m_next = 0;
}

