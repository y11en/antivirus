// klav_propbag.cpp
//
// dynamic KLAV_Property_Bag implementation
//

#include <klava/klav_props.h>
#include <klava/klavstl/vector.h>

#include <klava/klav_utils.h>

////////////////////////////////////////////////////////////////

class KLAV_Property_Bag_Impl : public KLAV_IFACE_IMPL(KLAV_Property_Bag)
{
public:
	KLAV_Property_Bag_Impl (KLAV_Alloc *allocator, KLAV_Properties *chain);
	virtual ~KLAV_Property_Bag_Impl ();

	virtual const klav_propval_t * KLAV_CALL get_property (klav_propid_t id);
	virtual KLAV_ERR KLAV_CALL set_property (klav_propid_t id, const klav_propval_t *val);

	virtual unsigned int  KLAV_CALL get_property_count ();
	virtual klav_propid_t KLAV_CALL get_property_by_no (unsigned int idx, klav_propval_t *val);

	virtual KLAV_ERR    KLAV_CALL set_properties (KLAV_Properties *props);
	virtual void        KLAV_CALL clear_properties ();

	virtual void        KLAV_CALL destroy ();

private:
	struct prop_desc
	{
		klav_propval_t m_val;
		klav_propid_t  m_id;

		prop_desc () : m_id (0) { m_val.l = 0; }
	};

	typedef klavstl::trivial_vector <prop_desc, klav_allocator> prop_desc_vector;
	prop_desc_vector  m_props;
	KLAV_Alloc *      m_allocator;
	KLAV_Properties * m_chain;

	prop_desc * find_prop (klav_propid_t id);

	bool copy_prop (prop_desc *dst, klav_propid_t id, const klav_propval_t *src);
	void destroy_prop (prop_desc *pd);

	void clear ();
};

KLAV_Property_Bag_Impl::KLAV_Property_Bag_Impl (
		KLAV_Alloc * allocator,
		KLAV_Properties * chain
	) :
		m_props (allocator),
		m_allocator (allocator),
		m_chain (chain)
{
}

KLAV_Property_Bag_Impl::~KLAV_Property_Bag_Impl ()
{
	clear ();
}

KLAV_Property_Bag_Impl::prop_desc *
KLAV_Property_Bag_Impl::find_prop (klav_propid_t id)
{
	size_t i, cnt = m_props.size ();
	for (i = 0; i < cnt; ++i)
	{
		prop_desc& prop = m_props [i];
		if (prop.m_id == id)
			return & prop;
	}

	return 0;
}

void KLAV_Property_Bag_Impl::clear ()
{
	size_t i, cnt = m_props.size ();
	for (i = 0; i < cnt; ++i)
	{
		destroy_prop (& m_props [i]);
	}
	m_props.resize (0);
}

const klav_propval_t * KLAV_CALL KLAV_Property_Bag_Impl::get_property (klav_propid_t id)
{
	size_t i, cnt = m_props.size ();
	for (i = 0; i < cnt; ++i)
	{
		const prop_desc& prop = m_props [i];
		if (prop.m_id == id)
			return & prop.m_val;
	}

	if (m_chain != 0)
		return m_chain->get_property (id);

	return 0;
}

unsigned int KLAV_CALL KLAV_Property_Bag_Impl::get_property_count ()
{
	unsigned int cnt = (unsigned int) m_props.size ();

	if (m_chain != 0)
		cnt += m_chain->get_property_count ();

	return cnt;
}

klav_propid_t KLAV_CALL KLAV_Property_Bag_Impl::get_property_by_no (unsigned int idx, klav_propval_t *val)
{
	unsigned int prop_cnt = (unsigned int) m_props.size ();

	if (idx >= prop_cnt)
	{
		if (m_chain != 0)
			return m_chain->get_property_by_no (idx - prop_cnt, val);
			
		return KLAV_PROPID_INVALID;
	}

	const prop_desc& prop = m_props [idx];
	*val = prop.m_val;

	return prop.m_id;
}

KLAV_ERR KLAV_CALL KLAV_Property_Bag_Impl::set_property (klav_propid_t id, const klav_propval_t *val)
{
	size_t i, cnt = m_props.size ();

	if (m_chain != 0)
	{
		KLAV_ERR err = m_chain->set_property (id, val);
		if (err != KLAV_ENOTFOUND)
			return err;
	}

	for (i = 0; i < cnt; ++i)
	{
		prop_desc& prop = m_props [i];
		if (prop.m_id == id)
		{
			if (val == 0)
			{
				// delete property
				m_props.erase (m_props.begin () + i, m_props.begin () + i + 1);
				return true;
			}
			if (! copy_prop (&prop, id, val))
				return KLAV_ENOMEM;

			return KLAV_OK;
		}
	}

	if (val == 0)
		return KLAV_OK; // deleting property that is already missing ?

	if (! m_props.resize (cnt + 1))
		return KLAV_ENOMEM;

	if (! copy_prop (& m_props [cnt], id, val))
	{
		m_props.resize (cnt);
		return KLAV_ENOMEM;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Property_Bag_Impl::set_properties (KLAV_Properties *props)
{
	unsigned int i, cnt = props->get_property_count ();

	for (i = 0; i < cnt; ++i)
	{
		klav_propval_t pv;
		klav_propid_t id = props->get_property_by_no (i, &pv);
		if (id != 0)
		{
			KLAV_ERR err = set_property (id, &pv);
			if (KLAV_FAILED (err))
				return err;
		}
	}

	return KLAV_OK;
}

void KLAV_CALL KLAV_Property_Bag_Impl::clear_properties ()
{
	m_props.resize (0);
}

void KLAV_CALL KLAV_Property_Bag_Impl::destroy ()
{
	KLAV_DELETE (this, m_allocator);
}

bool KLAV_Property_Bag_Impl::copy_prop (prop_desc *dst, klav_propid_t id, const klav_propval_t *src)
{
	switch (KLAV_PROPID_TYPE (id))
	{
	case KLAV_PROP_TYPE_STR: {
		  const char *ss = src->cs;
		  char * ns = 0;
		  if (ss != 0)
		  {
			size_t sz = strlen (ss);
			ns = (char *) m_allocator->alloc (sz + 1);
			if (ns == 0)
				return false;
			memcpy (ns, ss, sz);

			// clear old value, if present
			if (dst->m_val.s != 0)
				m_allocator->free (dst->m_val.s);
		  }
		  dst->m_val.s = ns;
		} break;
	default:
		dst->m_val = *src;
		break;
	}

	dst->m_id = id;
	return true;
}

void KLAV_Property_Bag_Impl::destroy_prop (prop_desc *pd)
{
	switch (KLAV_PROPID_TYPE (pd->m_id))
	{
	case KLAV_PROP_TYPE_STR:
		if (pd->m_val.s != 0)
			m_allocator->free (pd->m_val.s);
		break;
	default:
		break;
	}
	pd->m_val.l = 0;
	pd->m_id = 0;
}

KLAV_Property_Bag * KLAV_CALL KLAV_Property_Bag_Create (KLAV_Alloc *allocator)
{
	if (allocator == 0)
		return 0;

	KLAV_Property_Bag * pbag = KLAV_NEW (allocator) KLAV_Property_Bag_Impl (allocator, 0);
	return pbag;
}

KLAV_Property_Bag * KLAV_CALL KLAV_Property_Bag_Create_Ex (KLAV_Alloc *allocator, KLAV_Properties *chain)
{
	if (allocator == 0)
		return 0;

	KLAV_Property_Bag * pbag = KLAV_NEW (allocator) KLAV_Property_Bag_Impl (allocator, chain);
	return pbag;
}

