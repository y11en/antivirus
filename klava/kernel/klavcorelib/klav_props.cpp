// klav_props.cpp
//
// 'Static' KLAV_Properties impmementation
//

#include <klava/klav_props.h>

KLAV_Properties_Impl::~KLAV_Properties_Impl ()
{
}

const klav_propval_t * KLAV_CALL KLAV_Properties_Impl::get_property (klav_propid_t id)
{
	prop_desc *p = m_props;

	for (; p != 0; p = p->m_next)
	{
		if (p->m_id == id)
		{
			return & (p->m_val);
		}
	}

	if (m_chain)
		return m_chain->get_property (id);

	return 0;
}

KLAV_ERR KLAV_CALL KLAV_Properties_Impl::set_property (klav_propid_t id, const klav_propval_t *val)
{
	if (m_chain)
		return m_chain->set_property (id, val);

	return KLAV_ENOTFOUND;
}

unsigned int  KLAV_CALL KLAV_Properties_Impl::get_property_count ()
{
	unsigned int cnt = 0;
	for (prop_desc *p = m_props; p != 0; p = p->m_next) ++cnt;

	if (m_chain != 0)
		cnt += m_chain->get_property_count ();

	return cnt;
}

klav_propid_t KLAV_CALL KLAV_Properties_Impl::get_property_by_no (unsigned int idx, klav_propval_t *val)
{
	unsigned int n = 0;
	prop_desc *p = m_props;

	for (; p != 0; ++n, p = p->m_next)
	{
		if (n == idx)
		{
			if (val != 0)
				*val = p->m_val;
			return p->m_id;
		}
	}
	if (m_chain != 0)
	{
		return m_chain->get_property_by_no (idx - n, val);
	}
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_Inherit_Properties (
		KLAV_Properties * src,
		KLAV_Properties * dst
	)
{
	if (src == 0 || dst == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = KLAV_OK;
	unsigned int prop_cnt = src->get_property_count ();

	for (unsigned int i = 0; i < prop_cnt; ++i)
	{
		klav_propval_t propval;
		klav_propid_t propid = src->get_property_by_no (i, & propval);
		if (KLAV_PROPID_INHERITABLE (propid))
		{
			KLAV_ERR err2 = dst->set_property (propid, & propval);
			if (KLAV_FAILED (err))
				err = err2;
		}
	}
	
	return err;
}
