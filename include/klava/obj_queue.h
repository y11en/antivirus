// obj_queue.h
//

#ifndef obj_queue_h_INCLUDED
#define obj_queue_h_INCLUDED

#include <klava/klavdef.h>
#include <klava/kl_dyn_obj.h>

////////////////////////////////////////////////////////////////
// KlavPtrQueueBase

class KlavPtrQueueBase
{
public:
	KlavPtrQueueBase (KLAV_Alloc *pa = 0);
	~KlavPtrQueueBase ();

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	void set_allocator (KLAV_Alloc *pa)
		{ m_allocator = pa; }

	void clear ();
	void release_free_pages ();

	bool empty () const
		{ return m_head == m_tail && m_head_idx == m_tail_idx; }

	bool enqueue_tail (void *p);

	void * dequeue_head ();

	void * peek_head () const
		{ return (m_head != m_tail || m_head_idx < m_tail_idx) ? m_head->m_objects[m_head_idx] : 0; }

protected:

	enum { PAGE_SIZE = 254 };

	struct page_t;
	friend struct page_t;

	struct page_t
	{
		page_t * m_next;
		void * m_objects [PAGE_SIZE];
	};

	page_t * m_head;
	page_t * m_tail;

	unsigned int m_head_idx; // idx of the first obj
	unsigned int m_tail_idx; // idx of the first empty slot

	KLAV_Alloc * m_allocator;

	page_t * m_free_list;

	page_t * alloc_page ();
	void release_page (page_t *p);

	KlavPtrQueueBase (const KlavPtrQueueBase&);
	KlavPtrQueueBase& operator= (const KlavPtrQueueBase&);
};

////////////////////////////////////////////////////////////////
// KlavPtrQueue: typed pointers

template <class T>
class KlavPtrQueue : public KlavPtrQueueBase
{
public:
	typedef KlavPtrQueueBase Base;

	KlavPtrQueue (KLAV_Alloc *pa = 0)
		: KlavPtrQueueBase (pa) {}

	bool enqueue_tail (T *p)
			{ return Base::enqueue_tail (p); }

	T * dequeue_head ()
			{ return (T *) Base::dequeue_head (); }

	T * peek_head () const
			{ return (T *) Base::peek_head (); }

};

////////////////////////////////////////////////////////////////
// KlavObjQueueBase: queue for destroy()-able objects

class KlavDynObjQueueBase : public KlavPtrQueue <KLDynObject>
{
public:
	typedef KlavPtrQueue <KLDynObject> Base;

	KlavDynObjQueueBase (KLAV_Alloc *pa = 0)
		: KlavPtrQueue <KLDynObject> (pa) {}

	~KlavDynObjQueueBase ();

	void clear ();
};

////////////////////////////////////////////////////////////////
// queue for KLDynObject-derived objects

template <class T>
class KlavDynObjQueue : public KlavDynObjQueueBase
{
public:
	typedef KlavDynObjQueueBase Base;

	KlavDynObjQueue (KLAV_Alloc *pa = 0)
		: KlavDynObjQueueBase (pa) {}

	bool enqueue_tail (T *p)
			{ return Base::enqueue_tail (p); }

	T * dequeue_head ()
			{ return (T *) Base::dequeue_head (); }

	T * peek_head () const
			{ return (T *) Base::peek_head (); }

};


#endif // obj_queue_h_INCLUDED


