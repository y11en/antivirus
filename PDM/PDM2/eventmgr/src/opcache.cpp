#include "../include/opcache.h"
#include <memory.h>
#include <assert.h>

cOpCache::cOpCache()
{
	m_hint = 0;
	memset(&m_items, 0, sizeof(m_items));
}

cOpCache::~cOpCache()
{
}

bool cOpCache::Add(uint64_t uniq, void* op_result)
{
	if (!uniq)
		return false;
	cAutoLock lock(m_lock);
	if (m_items[m_hint].valid == false)
	{
		assert(m_items[m_hint].result == (void*)0 || m_items[m_hint].result == (void*)1);
		m_items[m_hint].result = op_result;
		m_items[m_hint].uniq = uniq;
		m_items[m_hint].valid = true;
		return true;
	}
	for (int i=0; i<countof(m_items); i++)
	{
		if (m_items[i].valid == false || m_items[i].uniq == uniq)
		{
			assert(m_items[i].result == (void*)0 || m_items[i].result == (void*)1);
			m_items[i].result = op_result;
			m_items[i].uniq = uniq;
			m_items[i].valid = true;
			return true;
		}
	}
	return false;
}

bool cOpCache::Set(uint64_t uniq, void* op_result)
{
	if (!uniq)
		return false;
	cAutoLock lock(m_lock);
	if (m_items[m_hint].valid == true && m_items[m_hint].uniq == uniq)
	{
		assert(m_items[m_hint].result == (void*)0 || m_items[m_hint].result == (void*)1);
		m_items[m_hint].result = op_result;
		return true;
	}
	for (int i=0; i<countof(m_items); i++)
	{
		if (m_items[i].valid == true && m_items[i].uniq == uniq)
		{
			assert(m_items[i].result == (void*)0 || m_items[i].result == (void*)1);
			m_items[i].result = op_result;
			return true;
		}
	}
	return false;
}

bool cOpCache::Get(uint64_t uniq, void** op_result)
{
	if (!uniq)
		return false;
	cAutoLock lock(m_lock);
	if (m_items[m_hint].valid == true && m_items[m_hint].uniq == uniq)
	{
		*op_result = m_items[m_hint].result;
		return true;
	}
	for (int i=0; i<countof(m_items); i++)
	{
		if (m_items[i].valid == true && m_items[i].uniq == uniq)
		{
			*op_result = m_items[i].result;
			return true;
		}
	}
	return false;
}

bool cOpCache::Remove(uint64_t uniq, void** op_result)
{
	if (!uniq)
		return false;
	cAutoLock lock(m_lock);
	if (m_items[m_hint].valid == true && m_items[m_hint].uniq == uniq)
	{
		if (op_result)
			*op_result = m_items[m_hint].result;
		m_items[m_hint].result = NULL;
		m_items[m_hint].valid = false;
		return true;
	}
	for (int i=0; i<countof(m_items); i++)
	{
		if (m_items[i].valid == true && m_items[i].uniq == uniq)
		{
			if (op_result)
				*op_result = m_items[i].result;
			m_items[i].result = NULL;
			m_items[i].valid = false;
			return true;
		}
	}
	return false;
}

bool cOpCache::Add(uint64_t uniq, bool op_result)
{
	return Add(uniq, (void*)op_result);
}

bool cOpCache::Set(uint64_t uniq, bool op_result)
{
	return Set(uniq, (void*)op_result);
}
bool cOpCache::Get(uint64_t uniq, bool& op_result)
{
	void* result;
	bool ret = Get(uniq, &result);
	op_result = !!result;
	return ret;
}

bool cOpCache::Remove(uint64_t uniq, bool& op_result)
{
	void* result;
	bool ret = Remove(uniq, &result);
	op_result = !!result;
	return ret;
}
