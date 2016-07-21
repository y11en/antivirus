// klav_objstack.h
//
// Object-Stack allocator

#ifndef klav_objstack_h_INCLUDED
#define klav_objstack_h_INCLUDED

#include <klava/klavdef.h>
#include <klava/kl_dyn_obj.h>

////////////////////////////////////////////////////////////////

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_ObjStack : public KLAV_Alloc
{
	virtual uint8_t * KLAV_CALL alloc (size_t size) = 0;
    virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize) = 0;  // limited functionality
    virtual void      KLAV_CALL free (void *ptr) = 0; // limited functionality
	virtual void      KLAV_CALL clear (void *block) = 0;
	virtual uint8_t * KLAV_CALL last_block () = 0;
	virtual size_t    KLAV_CALL block_size (void* block) = 0;
	virtual uint8_t * KLAV_CALL realloc_last (size_t size) = 0;
	virtual uint8_t * KLAV_CALL grow_last (size_t size) = 0;
	virtual KLAV_ERR  KLAV_CALL register_object (KLDynObject *obj) = 0;
	virtual KLAV_ERR  KLAV_CALL unregister_object (KLDynObject *obj) = 0;
	virtual void      KLAV_CALL destroy_objects (KLDynObject *last) = 0;
	KLAV_DUMMY_METHODS_DECL(KLAV_ObjStack)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_ObjStack)
{
	uint8_t * (KLAV_CALL * alloc) (struct KLAV_ObjStack *, size_t);
    uint8_t * (KLAV_CALL * realloc) (struct KLAV_ObjStack *, void *ptr, size_t newsize);
    void      (KLAV_CALL * free)  (struct KLAV_ObjStack *, void *ptr);
	void      (KLAV_CALL * clear) (struct KLAV_ObjStack *, void *);
	uint8_t * (KLAV_CALL * last_block) (struct KLAV_ObjStack *);
	size_t    (KLAV_CALL * block_size) (struct KLAV_ObjStack *, void *);
	uint8_t * (KLAV_CALL * realloc_last) (struct KLAV_ObjStack *, size_t);
	uint8_t * (KLAV_CALL * grow_last) (struct KLAV_ObjStack *, size_t);
	KLAV_ERR  (KLAV_CALL * register_object) (struct KLAV_ObjStack *, struct KLDynObject *);
	KLAV_ERR  (KLAV_CALL * unregister_object) (struct KLAV_ObjStack *, struct KLDynObject *);
	void      (KLAV_CALL * destroy_objects) (struct KLAV_ObjStack *, struct KLDynObject *);
};

KLAV_DECLARE_C_INTERFACE(KLAV_ObjStack);

#endif // __cplusplus

////////////////////////////////////////////////////////////////

#ifdef __cplusplus

class KLAV_ObjStack_Impl : public KLAV_IFACE_IMPL(KLAV_ObjStack)
{
public:
			KLAV_ObjStack_Impl (KLAV_Alloc *allocator, uint32_t chunk_size = 0);
	virtual ~KLAV_ObjStack_Impl ();

	// initialize internal structure, allocate first block
	// (called by constructor if allocator is nonzero)
	void init (KLAV_Alloc *allocator, uint32_t chunk_size = 0);

	// clear internal structure, deallocate all blocks
	// (called by destructor)
	void deinit ();

	virtual uint8_t * KLAV_CALL alloc (size_t size);
    virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize);
    virtual void      KLAV_CALL free (void *ptr);
	virtual void KLAV_CALL clear (void *from);
	virtual uint8_t * KLAV_CALL last_block ();
	virtual size_t    KLAV_CALL block_size (void* ptr);
	virtual uint8_t * KLAV_CALL realloc_last (size_t size);
	virtual uint8_t * KLAV_CALL grow_last (size_t size);
	virtual KLAV_ERR  KLAV_CALL register_object (KLDynObject *obj);
	virtual KLAV_ERR  KLAV_CALL unregister_object (KLDynObject *obj);
	virtual void      KLAV_CALL destroy_objects (KLDynObject *lastobj);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	void destroy_registered_objects ();

private:

	struct Chunk;

	struct ChunkHdr
	{
		uint8_t  * m_end;     // pointer to data past the end of this chunk
		Chunk    * m_prev;    // pointer to the previous chunk

		bool is_first () const
			{ return m_prev == 0; }
	};

	struct Chunk
	{
		union
		{
			ChunkHdr  hdr;
			void *    space [2];  // 16 or 32 depending on pointer size
		};
	};

	// single data item
	union ItemHdr
	{
		size_t m_size;
		void * space [2];  // 8 or 16 depending on pointer size
	};

	struct DynObj
	{
		KLDynObject * m_obj;
		DynObj *      m_next;
		DynObj *      m_prev;
	};

	KLAV_Alloc * m_allocator;      // allocator
	Chunk *      m_chunk;          // current chunk

	uint8_t *    m_last_item;      // ptr to data of the last allocated item
	uint8_t *    m_next_free;      // address of free memory in the current chunk
	
	DynObj *     m_first_obj;      // dynamic object list head
	DynObj *     m_last_obj;       // dynamic object list tail
	
	uint32_t     m_chunk_size;     // chunk size

	void  clear();

	static inline size_t& item_size_ref (const void* ptr)
	{
		ItemHdr *item_hdr = (ItemHdr *)((uint8_t *) ptr - sizeof (ItemHdr));
		return item_hdr->m_size;
	}

	static inline size_t get_item_size (const void *ptr)
		{ return item_size_ref (ptr); }

	static inline void set_item_size (void *ptr, size_t newsize)
		{ item_size_ref (ptr) = newsize; }

	void init_last_chunk (size_t first_item_size);

	static inline ptrdiff_t ptr2int(uint8_t * p)
	{
		return p - (uint8_t *)0;
	}

	static inline uint8_t * int2ptr (ptrdiff_t p)
	{
		return ((uint8_t *)0) + p;
	}
	
	Chunk * add_new_chunk (size_t first_item_size);
	void delete_last_chunk ();

	DynObj * add_object_to_list (KLDynObject *obj);
	void remove_object_from_list (DynObj *p);
};

#endif // __cplusplus

#endif // klav_objstack_h_INCLUDED


