//////////////////////////////////////////////////////////////////////////
// Source file for OE base object
#include "msoe_base.h"

#include <Prague/iface/i_buffer.h>
//////////////////////////////////////////////////////////////////////////

PROEFindObject::~PROEFindObject()
{
	if( folder )
		folder->Release();
	if( object )
		object->Release();
}

PROEObject * PROEObject::GetParent()
{
	if( m_parent )
		m_parent->AddRef();
	return m_parent;
}

PROEObject * PROEObject::GetRoot()
{
	PROEObject *l_root = this;
	while(l_root->m_parent)
		l_root = l_root->m_parent;
	l_root->AddRef();
	return l_root;
}

bool PROEObject::GetObject(tSTRING p_full_name, PROEFindObject &p_find)
{
	char *l_next = strchr(p_full_name, '\\');
	if( l_next )
		*l_next++ = 0;

	PROEObject *l_child = *p_full_name ? GetChild(p_full_name) : NULL;

	if( !l_child )
	{
		if( l_next || !(p_find.mask & FIND_OBJNAME) )
			return false;

		if( p_find.mask & FIND_FOLDER )
		{
			AddRef();
			p_find.folder = this;
		}
		if( p_find.mask & FIND_OBJNAME )
			strcpy_s(p_find.objname, countof(p_find.objname), p_full_name);

		return true;
	}

	if( !l_next )
	{
		if( p_find.mask & FIND_FOLDER )
		{
			AddRef();
			p_find.folder = this;
		}
		if( p_find.mask & FIND_OBJECT )
			p_find.object = l_child;

		if( p_find.mask & FIND_OBJNAME )
			strcpy_s(p_find.objname, countof(p_find.objname), p_full_name);

		return true;
	}
	bool f_ret = l_child->GetObject(l_next, p_find);
	l_child->Release();
	return f_ret;
}

bool PROEObject::GetObject(hOBJECT p_name, PROEFindObject &p_find)
{
	if( !this )
		return false;

	if( !p_name )
	{
		if( p_find.mask & FIND_FOLDER )
		{
			AddRef();
			p_find.folder = this;
		}
		else
			return false;
	}
	else
	{
		tDWORD l_iid = CALL_SYS_PropertyGetDWord(p_name, pgINTERFACE_ID);
		if( l_iid == IID_OBJPTR )
		{
			if( p_find.mask & FIND_OBJANY )
				p_find.object = ObjPtr_GetObject((hObjPtr)p_name);
			if( p_find.mask & FIND_FOLDER )
				p_find.folder = ObjPtr_GetPtrObj((hObjPtr)p_name);
		}
		else if( l_iid == IID_IO )
		{
			if( p_find.mask & FIND_OBJANY )
				p_find.object = IO_GetObject((hIO)p_name);
			if( (p_find.mask & FIND_FOLDER) && p_find.object )
				p_find.folder = p_find.object->GetParent();
		}
		else if( l_iid == IID_STRING )
		{
			tDWORD l_len;
			CALL_String_LengthEx((hSTRING)p_name, &l_len, cSTRING_WHOLE, cCP_ANSI, cSTRING_Z);

			tCHAR *l_buff = new tCHAR[l_len];
			CALL_String_ExportToBuff((hSTRING)p_name, NULL, cSTRING_WHOLE, l_buff, l_len, cCP_ANSI, cSTRING_Z);

			bool f_ret = GetObject(l_buff, p_find);

			delete [] l_buff;
			return f_ret;
		}
		else if( l_iid == IID_BUFFER )
		{
			tQWORD l_size;
			CALL_IO_GetSize((hIO)p_name, &l_size, IO_SIZE_TYPE_EXPLICIT);

			tPTR l_buffer;
			CALL_Buffer_Lock((hBUFFER)p_name, &l_buffer);

			PRPrmBuff l_buff((tCHAR*)l_buffer, (tDWORD)l_size);
			p_find.object = LoadObject(l_buff);

			CALL_Buffer_Unlock((hBUFFER)p_name);

			if( p_find.mask & FIND_FOLDER )
				p_find.folder = p_find.object->GetParent();
		}
		else
			return false;

		if( p_find.mask & FIND_FOLDER )
		{
			if( !p_find.folder )
				return false;
		}
		else if( (p_find.mask & FIND_OBJANY) && !p_find.object )
			return false;

		if( (p_find.mask & FIND_OBJNAME) && p_find.object )
		{
			PRPrmBuff l_buff(p_find.objname, MAX_NAME_SIZE);
			p_find.object->GetNameZ(l_buff);
		}
	}
	return true;
}

PROEObject * PROEObject::LoadObject(PRPrmBuff &p_buff)
{
	PROEObject *l_obj = this; AddRef();
	while(l_obj && p_buff.m_cur_size < p_buff.m_size)
	{
		PROEObject *l_child = l_obj->LoadChild(p_buff);
		l_obj->Release();
		l_obj = l_child;
	}
	if( l_obj )
		l_obj->CheckInit();
	return l_obj;
}

tERROR	PROEObject::SaveObject(PRPrmBuff &p_buff)
{
	tERROR l_error = errOK;
	if( !m_parent )
		return l_error;

	if( m_parent->f_folder && m_parent->m_parent )
		l_error = m_parent->SaveObject(p_buff);

	if( PR_SUCC(l_error) )
		l_error = m_parent->SaveChild(p_buff, this);
	return l_error;
}

tERROR	PROEObject::GetFullName(PRPrmBuff &p_buff, tBOOL f_path)
{
	tERROR l_error;
	if( !m_parent )
		return p_buff.Null();

	if( m_parent->f_folder )
	{
		if( PR_FAIL(l_error = m_parent->GetFullName(p_buff, cFALSE)) )
			return l_error;

		if( m_parent->m_parent )
			p_buff << '\\';
	}
	else
		p_buff.Null();


	if( f_path )
		return errOK;

	return GetName(p_buff);
}

PROEObject * PROEObject::GetChild(tSTRING p_name)
{
	tCHAR l_name[MAX_NAME_SIZE];

	PROEObject *l_child = NULL, *l_next = NULL;
	while( PR_SUCC(GetNext(l_next)) )
	{
		if( l_child )
			l_child->Release();
		l_child = l_next;

		PRPrmBuff l_buff(l_name, MAX_NAME_SIZE);
		if( PR_SUCC(l_next->GetNameZ(l_buff)) )
			if( !strcmp(l_name, p_name) )
				return l_next;
	}
	if( l_child )
		l_child->Release();
	return NULL;
}

PROEObject * PROEObject::LoadChild(PRPrmBuff &p_buff)
{
	tSTRING l_name; p_buff >> l_name;
	return GetChild(l_name);
}

//////////////////////////////////////////////////////////////////////////
