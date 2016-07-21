/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_system_c.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Prague-root-proxies.
 * 
 */
//************************************************************************

#include "pr_common.h"

PRSharedProps g_shp;

//************************************************************************

bool PRSharedProps::CompareKey(hRecord record, void *key, void *data)
{
	return !strcmp(((PRShareProp*)data)->m_Name, (const char*)key);
}

bool PRSharedProps::InitKeyData(hRecord record, void *key, void *data)
{
	strcpy(((PRShareProp*)data)->m_Name, (const char*)key);
	return true;
}

void PRSharedProps::ClearProcess(hRecord record, void *data, long process)
{
	PRShareProp *l_share_prop = (PRShareProp*)data;
	for(tDWORD i = 0; i < l_share_prop->m_Count; i++)
	{
		tPROCESS l_process = l_share_prop->m_Process[i];
		if( l_process != process )
			continue;

		memcpy(l_share_prop->m_Process + i, l_share_prop->m_Process + i + 1,
			(l_share_prop->m_Count-i-1)*sizeof(tPROCESS));
		i--; l_share_prop->m_Count--;
	}
}

tERROR PRSharedProps::RegisterCustomPropId(hROOT _this, tDWORD* result, tSTRING name, tDWORD type)
{
	tDWORD l_prop = 0;
	tERROR error = errOK;
	if( name && *name )
	{
		hRecord l_record = InsertRecord(name);
		if( l_record != SHTR_INVALID_HANDLE )
		{
			if( type != pTYPE_NOTHING )
				l_prop = mPROPERTY_MAKE_CUSTOM( type, cPROPERTY_NAMED_CUSTOM + l_record );
			else
				l_prop = cPROPERTY_NAMED_CUSTOM + l_record;
		}
		else
			error = errUNEXPECTED;
	}
	else
		error = errPARAMETER_INVALID;

	if( result )
		*result = l_prop;

	return error;
}

tERROR PRSharedProps::UnregisterCustomPropId(hROOT _this, tSTRING name)
{
	return errOK;
	tERROR error = errOK;
	if( name && *name )
		DeleteRecord(name);
	else
		error = errPARAMETER_INVALID;

	return error;
}

tERROR PRSharedProps::GetCustomPropId(hROOT _this, tDWORD* result, tSTRING name, tDWORD type)
{
	tDWORD l_prop = 0;
	tERROR error = errOK;
	if( name && *name )
	{
		hRecord l_record = FindRecord(name, 0);
		if( l_record != SHTR_INVALID_HANDLE )
		{
			if( type != pTYPE_NOTHING )
				l_prop = mPROPERTY_MAKE_CUSTOM( type, cPROPERTY_NAMED_CUSTOM + l_record );
			else
				l_prop = cPROPERTY_NAMED_CUSTOM + l_record;
		}
		else
			error = errUNEXPECTED;
	}
	else
		error = errPARAMETER_INVALID;

	if( result )
		*result = l_prop;

	return error;
}

tERROR PRSharedProps::GetCustomPropName(hROOT _this, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size)
{
	hRecord l_record = (pNUMBER_MASK & prop_id) - cPROPERTY_NAMED_CUSTOM;
	PRShareProp l_data;
	if( !GetRecordData(l_record, &l_data) )
		return errUNEXPECTED;

	tDWORD len = (tDWORD)strlen(l_data.m_Name)+1;
	if( out_size )
		*out_size = len;

	if( buffer && size < len )
		return errBUFFER_TOO_SMALL;

	if( buffer )
		strcpy((char*)buffer, l_data.m_Name);

	return errOK;
}

//************************************************************************

// ---
tERROR pr_call Local_Root_RegisterCustomPropId( hROOT _this, tDWORD* result, tSTRING name, tDWORD type ) { // -- allocates unique property id;
	return g_shp.RegisterCustomPropId(_this, result, name, type);
}
// ---
tERROR pr_call Local_Root_UnregisterCustomPropId( hROOT _this, tSTRING name ) { // -- deallocates unique property id;
	return g_shp.UnregisterCustomPropId(_this, name);
}
// ---
tERROR pr_call Local_Root_GetCustomPropId( hROOT _this, tDWORD* result, tSTRING name, tDWORD type ) { // -- returns id of the named property identifier;
	return g_shp.GetCustomPropId(_this, result, name, type);
}
// ---
tERROR pr_call Local_Root_GetCustomPropName( hROOT _this, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ) { // -- returns registered custom property name by identifier;
	return g_shp.GetCustomPropName(_this, out_size, prop_id, buffer, size);
}

// ---
iRootVtbl root_iface;

void PRRemoteManagerImpl::InitRootIFace()
{
	LOCAL_ROOT->RegisterCustomPropId = Local_Root_RegisterCustomPropId;
	LOCAL_ROOT->UnregisterCustomPropId = Local_Root_UnregisterCustomPropId;
	LOCAL_ROOT->GetCustomPropId = Local_Root_GetCustomPropId;
	LOCAL_ROOT->GetCustomPropName = Local_Root_GetCustomPropName;
	memcpy(&root_iface, LOCAL_ROOT, sizeof(iRootVtbl));
}


