// obj_queue.cpp
//

#include <klava/obj_queue.h>

KlavPtrQueueBase::KlavPtrQueueBase (KLAV_Alloc *pa)
	: m_head (0), m_tail (0),
	  m_head_idx (0), m_tail_idx (0),
	  m_allocator (pa), m_free_list (0)
{
}

KlavPtrQueueBase::~KlavPtrQueueBase ()
{
	clear ();
	release_free_pages ();
}

inline void KlavPtrQueueBase::release_page (page_t *p)
{
	p->m_next = m_free_list;
	m_free_list = p;
}

void KlavPtrQueueBase::clear ()
{
	m_head_idx = 0;
	m_tail_idx = 0;

	while (m_head != 0)
	{
		page_t *p = m_head;
		m_head = p->m_next;

		release_page (p);
	}

	m_tail = 0;
}

void KlavPtrQueueBase::release_free_pages ()
{
	while (m_free_list != 0)
	{
		page_t *p = m_free_list;
		m_free_list = p->m_next;
		m_allocator->free (p);
	}
}

void * KlavPtrQueueBase::dequeue_head ()
{
	page_t *page = m_head;

	if (page != m_tail)
	{
		void * obj = page->m_objects [m_head_idx++];

		if (m_head_idx == PAGE_SIZE)
		{
			m_head = page->m_next;
			m_head_idx = 0;
			release_page (page);
		}

		return obj;
	}
	else
	{
		if (m_head_idx < m_tail_idx)
		{
			void * obj = page->m_objects [m_head_idx++];

			if (m_head_idx == m_tail_idx)
			{
				m_head_idx = 0;
				m_tail_idx = 0;
			}

			return obj;
		}
	}

	return 0;
}

KlavPtrQueueBase::page_t * KlavPtrQueueBase::alloc_page ()
{
	page_t *p = (page_t *) m_allocator->alloc (sizeof (page_t));

	if (p != 0)
	{
		p->m_next = 0;
	}
	return p;
}

bool KlavPtrQueueBase::enqueue_tail (void *obj)
{
	if (m_tail == 0)
	{
		// assert (m_tail_idx == 0)
		m_head = m_tail = alloc_page ();
		if (m_head == 0) return false;
	}
	else
	{
		if (m_tail_idx == PAGE_SIZE)
		{
			page_t *page = alloc_page ();
			if (page == 0) return false;

			m_tail->m_next = page;
			m_tail = page;
			m_tail_idx = 0;
		}
	}

	m_tail->m_objects [m_tail_idx++] = obj;
	return true;
}

////////////////////////////////////////////////////////////////
// KlavDynObjQueueBase

KlavDynObjQueueBase::~KlavDynObjQueueBase ()
{
	clear ();
	release_free_pages ();
}

void KlavDynObjQueueBase::clear ()
{
	while (m_head != 0)
	{
		page_t *p = m_head;
		m_head = p->m_next;

		unsigned int top = (p == m_tail ? m_tail_idx : PAGE_SIZE);

		for (unsigned int i = m_head_idx; i < top; ++i)
		{
			KLDynObject *obj = (KLDynObject *)(p->m_objects[i]);
			if (obj != 0) obj->destroy ();
		}

		m_head_idx = 0;

		release_page (p);
	}

	m_tail_idx = 0;
	m_tail = 0;
}



