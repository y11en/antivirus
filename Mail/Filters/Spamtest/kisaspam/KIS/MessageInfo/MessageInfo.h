#ifndef  _MESSAGEINFO_H
#define  _MESSAGEINFO_H

#include <windows.h>
#include <Prague/prague.h>
#include <Prague/pr_loadr.h>
#include <Prague/pr_vector.h>
#include <Prague/pr_oid.h>

#include <Prague/iface/i_os.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Extract/iface/i_mailmsg.h>

#include <Extract/plugin/p_msoe.h>
#include <Extract/plugin/p_mailmsg.h>
#include "../spamfilter.h"

struct prop_t
{
	prop_t(): id(0), sz(0), n(0) {};
	prop_t(unsigned long p_id): id(p_id), sz(0), n(0) {};

	unsigned long id;
	char* sz;
	long  n;
	void operator=(const prop_t& prop)
	{
		id = prop.id;
		sz = prop.sz;
		n  = prop.n;
	}
	bool operator==(const prop_t& prop)
	{
		return ( id == prop.id );
	}
};

class INFO_KIS : public INFO
{
public:
	INFO_KIS(hOBJECT obj);
	virtual ~INFO_KIS();
	
public:
	// GetPropStr возвращает указатель на строковое свойство (например, pgMESSAGE_FROM)
	// В случае отсутствия свойства возвращается NULL
	// Указатель жив, пока жив объект INFO.
	const char* GetPropStr(  unsigned long ulPropID );
	// GetPropLong возвращает указатель на числовое свойство (например, pgMESSAGE_PART_CONTENT_LENGTH)
	// В случае отсутствия свойства возвращается 0
	const long  GetPropLong( unsigned long ulPropID );
protected:
	typedef cVector<prop_t> props_t;
	hOBJECT m_obj;
	props_t  m_str_props;
	props_t m_long_props_t;
};

class MESSAGEPART_INFO_KIS : public INFO_KIS, public MESSAGEPART_INFO
{
public:
    MESSAGEPART_INFO_KIS(hIO io);
    virtual ~MESSAGEPART_INFO_KIS();

public:
	// GetData возвращает указатель на поток данных.
	// Указатель жив, пока не вызвано ReleaseData и жив объект MESSAGEPART_INFO. 
	// Указатель должен быть освобожден вызовом ReleaseData.
	IStream* GetData();
	void ReleaseData();
	const char* GetPropStr(  unsigned long ulPropID );
	const long  GetPropLong( unsigned long ulPropID );
private:
	hIO m_io;
	IStream* m_lpStream;
};

class MESSAGE_INFO_KIS : public INFO_KIS, public MESSAGE_INFO
{
public:
    MESSAGE_INFO_KIS(hOS p_os); 
    virtual ~MESSAGE_INFO_KIS();

    // GetMessagePart возращает указатель на MESSAGEPART_INFO по порядковому номеру num
	// Указатель должен быть освобожден вызовом ReleaseMessagePart
    MESSAGEPART_INFO* GetMessagePart(unsigned long num);
	void ReleaseMessagePart(MESSAGEPART_INFO* pMessagePart);
	const char* GetPropStr(  unsigned long ulPropID );
	const long  GetPropLong( unsigned long ulPropID );
	const char* GetRawMessage();
private:
	hOS m_os;
	hObjPtr m_optr;
	typedef cVector<hIO> ios_t;
	ios_t m_ios;
};





#endif//  _MESSAGEINFO_H
