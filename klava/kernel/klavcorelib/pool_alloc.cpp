// pool_alloc.cpp
//
// Pool allocator
//

#include <klava/klav_utils.h>

////////////////////////////////////////////////////////////////
// KLAV_Pool_Item_Allocator

void * KLAV_Pool_Item_Allocator::alloc ()
{
	// has item in free list ?
	if (m_free_list != 0)
	{
		void * p = m_free_list;
		m_free_list = m_free_list->m_ptr;
		return p;
	}

	// has item in last block ?
	if (m_last_block != 0 && m_last_block->m_used < m_items_in_blk)
	{
		uint8_t * p = ((uint8_t *) m_last_block) + 16 + (m_last_block->m_used * m_item_size);
		m_last_block->m_used ++;
		return p;
	}

	// need to allocate new block
	ListNode * blk = (ListNode *) m_allocator->alloc (16 + m_items_in_blk * m_item_size);
	if (blk == 0)
		return 0;

	blk->m_ptr = m_last_block;
	blk->m_used = 1;
	m_last_block = blk;

	return ((uint8_t *) blk) + 16;
}

void KLAV_Pool_Item_Allocator::free (void * data)
{
	if (data == 0)
		return;

	ListNode * node = (ListNode *) data;
	node->m_ptr = m_free_list;
	m_free_list = node;
}

void KLAV_Pool_Item_Allocator::cleanup ()
{
	m_free_list = 0;

	while (m_last_block != 0)
	{
		ListNode * blk = m_last_block;
		m_last_block = blk->m_ptr;

		m_allocator->free (blk);
	}
}

