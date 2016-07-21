#include <assert.h>
#include "../include/cregkey.h"

cRegKey::cRegKey(tRegistryKey root /* = 0 */, tcstring keyname /* = NULL */)
{	
	m_fullname = m_name = m_open_name = m_controlset_subkey = m_orig_name = 0;
	m_root = 0;
	assign(m_root, keyname);
}

cRegKey::~cRegKey()
{
	clear();
}

void cRegKey::clear()
{
	if (m_orig_name)
		tstrfree(m_orig_name);
	if (m_fullname)
		tstrfree(m_fullname);
	m_fullname = m_name = m_open_name = m_controlset_subkey = 0;
	m_root = 0;
}

bool cRegKey::assign(tRegistryKey root, tcstring keyname)
{
	clear();
	
	if (!keyname)
		return false;
	
	m_fullname = m_open_name = m_name = tstrdup(keyname);
	tstrupr((tstring)m_fullname);
	m_root = root;
	
	if (0 == m_root && tstrcmpinc(&m_name, _T("\\REGISTRY\\")))
	{
		if (tstrcmpinc(&m_name, _T("MACHINE\\")))
		{
			m_root = crHKEY_LOCAL_MACHINE;
			m_open_name = m_name;
		}
		else if (tstrcmpinc(&m_name, _T("USER\\")))
		{
			m_root = crHKEY_USERS;
			m_open_name = m_name;
		}
		else
		{
			assert(0);
		}
	}

	if (crHKEY_USERS == m_root)
	{
		// skip SID
		tcstring ptr = tstrchr(m_name, '\\');
		if (ptr)
			m_name = tstrinc(ptr);
	}

	if ((crHKEY_LOCAL_MACHINE == m_root || crHKEY_USERS == m_root)
		&& tstrcmpinc(&m_name, _T("SOFTWARE\\CLASSES\\")))
	{
		m_root = crHKEY_CLASSES_ROOT;
		m_open_name = m_name;
	}

	if (crHKEY_LOCAL_MACHINE == m_root)
	{
		if (0 == tstrncmp(m_name, _T("SYSTEM\\CURRENTCONTROLSET\\"), 25))
			m_controlset_subkey = tstrninc(m_name, 25);
		else if (0 == tstrncmp(m_name, _T("SYSTEM\\CONTROLSET"), 17))
		{
			int i;
			tcstring ptr = tstrninc(m_name, 17);
			for (i=0; i<3; i++)
			{
				tchar c = tstrchar(ptr);
				if (c < '0' || c > '9')
					break;
				ptr = tstrinc(ptr);
			}
			if (i == 3 && tstrchar(ptr) == '\\')
			{
				m_controlset_subkey = tstrinc(ptr);
			}
		}
	}
	m_orig_name = tstrdup(keyname+(m_open_name-m_fullname));
	return false;
}

cRegKey& cRegKey::operator =(const cRegKey& from)
{
	assign(from.m_root, from.m_fullname);
	return *this;
}

