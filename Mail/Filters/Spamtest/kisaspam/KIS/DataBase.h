// DataBase.h: interface for the CDataBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABASE_H__71BBD358_04F1_482E_9673_E6D7F5F8AEF2__INCLUDED_)
#define AFX_DATABASE_H__71BBD358_04F1_482E_9673_E6D7F5F8AEF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "smtp-filter/libsigs/libsigs.h"
#include "smtp-filter/libgsg2/gsg.h"
#include "client-filter/librules/profile.h"
#include "client-filter/librules/profile_db_impl.h"
#include "client-filter/librules/profile_db.h"
#include "kis_face.h"

#include <filter.h>
#include <catid.h>
#include <categories.h>
#include <cfdata.h>

class CBKFBase
{
	typedef content_filter* (*fn_filter_open)(const char* name);
	typedef int (*fn_filter_close)(content_filter* cf, unsigned int flags);
	typedef filter_result* (*fn_filter_check)(content_filter* cf, message* m, filter_result* fr, unsigned int flags);
	typedef void (*fn_filter_result_free)(filter_result* fr);
	typedef int (*fn_message_create_mime)(message* msg, const char* msg_text, unsigned long msg_length);
	typedef void (*fn_message_destroy)(message* msg);
	typedef const flt_category* (*fn_flt_category_find)(const content_filter *cf, cf_category_id cid);
	typedef const char* (*fn_flt_category_get_text_id)(const flt_category *c);
	typedef const char* (*fn_flt_category_get_title)(const flt_category *c);
	typedef enum cf_errors (*fn_cfdata_compile)(const char* out, const struct cfd_compile_config *cfd_cc, 
								cfd_state_config* sc, const char* template_base, cfd_file_info* fi, unsigned int fi_size);

	HMODULE m_hBKFFilter;
	fn_filter_open m_filter_open;
	fn_filter_close m_filter_close;
	fn_filter_check m_filter_check;
	fn_filter_result_free m_filter_result_free;
	fn_message_create_mime m_message_create_mime;
	fn_message_destroy m_message_destroy;
	fn_flt_category_find m_flt_category_find;
	fn_flt_category_get_text_id m_flt_category_get_text_id;
	fn_flt_category_get_title m_flt_category_get_title;

	HMODULE m_hBKFData;
	fn_cfdata_compile m_cfdata_compile;

	content_filter *m_filter_object;

	char *m_pszBasePath;

	// критическа€ секци€ дл€ сериализации доступа к .new базе
	// так как доступ производитс€ из CheckLoadBase и CompileBaseIfNeeded, а доступ к этим функци€м
	// сериализуетс€ с помощью CSpamtestService::m_lock, то надобности в этой секции нет
#ifdef _PROTECT_AS_TRM_NEW
	CRITICAL_SECTION m_csNewBase;
#endif

public:
	CBKFBase();
	~CBKFBase();

	// реинициализаци€
	// загружает BKF, производит компил€цию базы при необходимости
	bool Reinit(IN const char *pszRecentTermsBasePath);
	// выгрузка
	void Done();
	// обработка MIME-сообщени€
	bool ProcessMIMEMessage(const char *pszMessage, unsigned int *pRelevantIndex);
	// обработка сообщени€ по телу и теме
	bool ProcessMessage(const char *pszSubject, const char *pszBody, unsigned int *pRelevantIndex);

protected:
	// обработка сформированного BKF message
	bool ProcessBKFMessage(message &msg, unsigned int *pRelevantIndex) const;
	// провер€ет загруженность рабочей базы
	bool CheckLoadBase();
	// компилирует базу, если необходимо
	bool CompileBaseIfNeeded();
};

class CDataBase
{
    md5sigarray     *m_pMD5Array;
    profile_db		*m_pPDB;
	binary_data_t   *m_pBinaryProcessor;
	CBKFBase		m_BKFBase;
public:
    CDataBase();
    ~CDataBase();

	HRESULT Init(IN IStream*  pGsg, IN IStream*  pHeader, IN const char *pszRecentTermsBasePath);
	HRESULT Done();
	
	bool IsValid() const
	{
		return m_bIsValid;
	}

    profile_db*  GetPDB() const
	{
		return m_pPDB;
	}
    md5sigarray* GetMD5Array() const
	{
		return m_pMD5Array;
	}
    binary_data_t* GetBinaryProcessor() const
	{
		return m_pBinaryProcessor;
	}
	CBKFBase &GetBKFBase()
	{
		return m_BKFBase;
	}

protected:
	bool m_bIsValid;
    static int      MD5SigCmp(const void *a, const void *b);
	bool            SortGSG();
    HRESULT         ProfileDBOpenFromStream (
                        IN  IStream* pPdb,
                        OUT profile_db*& pReturn
                        );
    HRESULT         ForciblyRead(
                        IN IStream*  pGsg, 
                        void* buf,
                        size_t size);
    md5sigarray*    LoadMD5SigFromStream (IN IStream*  pGsg);

};

#endif // !defined(AFX_DATABASE_H__71BBD358_04F1_482E_9673_E6D7F5F8AEF2__INCLUDED_)
