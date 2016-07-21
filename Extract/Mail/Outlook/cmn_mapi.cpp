//////////////////////////////////////////////////////////////////////////
// Source file for MAPI routine

#include "cmn_mapi.h"

#include <Prague/iface/e_ktime.h>

//////////////////////////////////////////////////////////////////////////
// buffer methods

#define CHECK_BUFFER() \
	if( !p_buff->buffer ) return errOK; \
	if( p_buff->size < p_buff->out_size ) \
		return errBUFFER_TOO_SMALL;

tERROR	PRBuffSetNULL(PRProp_Buff *p_buff)
{
	p_buff->out_size += 1;
	CHECK_BUFFER()
	*p_buff->buffer = 0;
	return errOK;
}

tERROR	PRBuffSetCHAR(PRProp_Buff *p_buff, tCHAR p_ch)
{
	p_buff->out_size += sizeof(tCHAR)+1;
	CHECK_BUFFER()
	*p_buff->buffer = p_ch;
	p_buff->buffer ++;
	*p_buff->buffer = 0;
	return errOK;
}

tERROR	PRBuffSetSTRING(PRProp_Buff *p_buff, tSTRING p_val)
{
	if( p_buff->type == tid_PTR )
		return PRBuffSetBINARY(p_buff, (tBYTE*)p_val, p_val ? (tDWORD)(strlen(p_val)+1) : 0);

	tDWORD l_len = (tDWORD)strlen(p_val)+1;
	p_buff->out_size += l_len;
	CHECK_BUFFER()
	memcpy(p_buff->buffer, p_val, l_len);
	p_buff->buffer += l_len-1;
	return errOK;
}

tERROR	PRBuffSetDWORD(PRProp_Buff *p_buff, tDWORD p_val)
{
	p_buff->out_size += sizeof(tDWORD);
	CHECK_BUFFER()
	*(tDWORD*)p_buff->buffer = p_val;
	p_buff->buffer += sizeof(tDWORD);
	return errOK;
}

tERROR	PRBuffSetQWORD(PRProp_Buff *p_buff, tQWORD p_val)
{
	p_buff->out_size += sizeof(tQWORD);
	CHECK_BUFFER()
	*(tQWORD*)p_buff->buffer = p_val;
	p_buff->buffer += sizeof(tQWORD);
	return errOK;
}

tERROR	PRBuffSetDT(PRProp_Buff *p_buff, FILETIME *p_val)
{
	FILETIME l_ft;
	FileTimeToLocalFileTime(p_val, &l_ft);

	SYSTEMTIME l_st;
	FileTimeToSystemTime(&l_ft, &l_st);

	if( p_buff->type == tid_STRING )
	{
		p_buff->out_size += PRBUFF_SDATELEN;
		CHECK_BUFFER()
        _snprintf_s(p_buff->buffer, PRBUFF_SDATELEN, _TRUNCATE, PRBUFF_SDATEFMT, l_st.wYear, l_st.wMonth, l_st.wDay,
			l_st.wHour, l_st.wMinute, l_st.wSecond, l_st.wMilliseconds);

		p_buff->buffer += PRBUFF_SDATELEN-1;
	}
	else
	{
		p_buff->out_size += sizeof(tDT);
		CHECK_BUFFER()

		if( !DTSet )
			return errNOT_IMPLEMENTED;

		DTSet((tDT*)p_buff->buffer, l_st.wYear, l_st.wMonth, l_st.wDay,
			l_st.wHour, l_st.wMinute, l_st.wSecond, l_st.wMilliseconds );
	}
	return errOK;
}

tERROR	PRBuffSetBINARY(PRProp_Buff *p_buff, tBYTE *p_val, tDWORD p_size)
{
	if( p_buff->type == tid_PTR )
	{
		if( PR_FAIL(PRBuffSetDWORD(p_buff, p_size)) )
			return errBUFFER_TOO_SMALL;

		p_buff->out_size += p_size;
		CHECK_BUFFER()

		memcpy(p_buff->buffer, p_val, p_size);
		p_buff->buffer += p_size;
	}
	else
	{
		p_buff->out_size += 2*p_size + 1;
		CHECK_BUFFER()

		for(tDWORD i = 0; i < p_size; i++, p_buff->buffer += 2)
			sprintf_s(p_buff->buffer, 3, "%02X", p_val[i]);
		*p_buff->buffer = 0;
	}
	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// common routine

tERROR	ObjectGetSProp(IMAPIProp *p_obj, ULONG p_prop, LPSPropValue *p_entry)
{
	if( !p_obj )
		return errOBJECT_INVALID;

	SizedSPropTagArray(1, sptaColumns) = {1, {p_prop}};

	ULONG l_num = 0;
	if( HR_FAILED(p_obj->GetProps((LPSPropTagArray)&sptaColumns, 0, &l_num, p_entry)) || !p_entry )
		return errOBJECT_INVALID;
	return errOK;
}

tERROR	ObjectGetProp(IMAPIProp *p_obj, ULONG p_prop, tTYPE_ID p_type, PRProp_Buff *p_buff)
{
	if( !p_obj )
		return errOBJECT_INVALID;

	SizedSPropTagArray(1, sptaColumns) = {1, {p_prop}};

	LPSPropValue l_value = NULL; ULONG l_num = 0;
	HRESULT l_result = p_obj->GetProps((LPSPropTagArray)&sptaColumns, 0, &l_num, &l_value);

	tERROR l_error = errPROPERTY_NOT_FOUND;
	if( HR_FAILED(l_result) )
		;
	else if( (l_value->ulPropTag & PROP_TYPE_MASK) == PT_ERROR )
	{
		if( !p_buff && l_value->Value.err != MAPI_E_NOT_FOUND )
			l_error = errOK;
	}
	else if( p_buff )
// convert property value
		switch( p_type )
		{
		case tid_STRING:
		case tid_CHAR:		l_error = PRBuffSetSTRING(p_buff, l_value->Value.lpszA); break;
		case tid_DWORD:		l_error = PRBuffSetDWORD(p_buff, l_value->Value.ul); break;
		case tid_QWORD:		l_error = PRBuffSetQWORD(p_buff, l_value->Value.ul); break;
		case tid_DATETIME:	l_error = PRBuffSetDT(p_buff, &l_value->Value.ft); break;
		case tid_PTR:		l_error = PRBuffSetBINARY(p_buff, l_value->Value.bin.lpb, l_value->Value.bin.cb); break;
		}
	else
		l_error = errOK;

	if( l_value )
		MAPIFreeBuffer(l_value);
	return l_error;
}

tERROR	ObjectSetProp(IMAPIProp *p_obj, ULONG p_prop, tTYPE_ID p_type, PRProp_Buff *p_buff)
{
	if( !p_obj )
		return errOBJECT_INVALID;

	SizedSPropTagArray(1, sptaColumns) = {1, {p_prop}};
	SPropValue l_value;
	l_value.ulPropTag = p_prop;
	l_value.dwAlignPad = 0;

	switch( p_type )
	{
	case tid_STRING:
	case tid_CHAR:		l_value.Value.lpszA = p_buff->buffer; break;
	case tid_DWORD:		l_value.Value.ul = *(ULONG*)p_buff->buffer; break;
	default:
		return errPROPERTY_INVALID;
	}
	
	if( FAILED(p_obj->SetProps(1, &l_value, NULL)) )
		return errPROPERTY_NOT_FOUND;
	return errOK;
}

tDWORD	ObjectGetPropDWORD(IMAPIProp *p_obj, ULONG p_prop)
{
	ULONG l_prop = 0;
	PRProp_Buff l_buff = {(tCHAR*)&l_prop, sizeof(tDWORD), tid_VOID};
	ObjectGetProp(p_obj, p_prop, tid_DWORD, &l_buff);
	return l_prop;
}

tERROR	ObjectSetPropDWORD(IMAPIProp *p_obj, ULONG p_prop, tDWORD p_value)
{
	PRProp_Buff l_buff = {(tCHAR*)&p_value, sizeof(tDWORD), tid_VOID};
	return ObjectSetProp(p_obj, p_prop, tid_DWORD, &l_buff);
}

tERROR	MessageGetName(IMAPIProp *p_obj, PRProp_Buff *p_buff)
{
	tCHAR l_time_name[PRBUFF_SDATELEN];
	PRProp_Buff l_time_buff = {l_time_name, PRBUFF_SDATELEN, tid_STRING, 0};

	tERROR l_error;
	if( !PR_FAIL(l_error = ObjectGetProp(p_obj, PR_PROVIDER_SUBMIT_TIME, tid_DATETIME, &l_time_buff)) )
	{
		l_error = PRBuffSetSTRING(p_buff, OBJMSG_TO);
		if( !PR_FAIL(l_error) )
			l_error = ObjectGetProp(p_obj, PR_DISPLAY_TO, tid_CHAR, p_buff);
	}
	else
	{
		if( l_error != errPROPERTY_NOT_FOUND )
			return l_error;
		if( !PR_FAIL(l_error = ObjectGetProp(p_obj, PR_CLIENT_SUBMIT_TIME, tid_DATETIME, &l_time_buff)) )
		{
			l_error = PRBuffSetSTRING(p_buff, OBJMSG_FROM);
			if( !PR_FAIL(l_error) )
			{
				l_error = ObjectGetProp(p_obj, PR_SENDER_NAME, tid_CHAR, p_buff);
				if( l_error == errPROPERTY_NOT_FOUND )
					l_error = PRBuffSetSTRING(p_buff, OBJMSG_NONE);
			}
		}
		else
		{
			if( l_error != errPROPERTY_NOT_FOUND )
				return l_error;
			ObjectGetProp(p_obj, PR_CREATION_TIME, tid_DATETIME, &l_time_buff);
			if( PR_SUCC(l_error = PRBuffSetSTRING(p_buff, OBJMSG_FROM)) )
				l_error = PRBuffSetSTRING(p_buff, OBJMSG_NONE);
		}
	}
	if( PR_FAIL(l_error) )
		return l_error;

	l_error = PRBuffSetSTRING(p_buff, OBJMSG_SUBJECT);
	if( !PR_FAIL(l_error) )
	{
		l_error = ObjectGetProp(p_obj, PR_SUBJECT, tid_CHAR, p_buff);
		if( PR_FAIL(l_error) && l_error == errPROPERTY_NOT_FOUND )
			l_error = errOK;
	}
	if( !PR_FAIL(l_error) )
		l_error = PRBuffSetSTRING(p_buff, OBJMSG_TIME);
	if( !PR_FAIL(l_error) )
		l_error = PRBuffSetSTRING(p_buff, l_time_name);
	if( !PR_FAIL(l_error) )
		l_error = PRBuffSetCHAR(p_buff, ']');
	return l_error;//ObjectAppendKey(p_obj, p_buff);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
