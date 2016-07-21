// DataBase.cpp: implementation of the CDataBase class.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "DataBase.h"
#include <Prague/prague.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <io.h>
#include <squeeze/squeeze.h>
#include <functional>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBase::CDataBase(): m_pMD5Array(NULL), m_pPDB(NULL),
                        m_pBinaryProcessor(NULL)
{
}

CDataBase::~CDataBase()
{

}

HRESULT CDataBase::Init(
                IN IStream*  pGetGSGData,
                IN IStream*  pGetHeaderData,
				IN const char *pszRecentTermsBasePath
                )
{
    HRESULT hRC = S_OK;
    
    if ( m_pMD5Array || m_pPDB || m_pBinaryProcessor) 
        Done();

    m_pBinaryProcessor = binary_init(MAX_GSG2_SIZE);
    if (NULL == m_pBinaryProcessor)
		hRC = E_FAIL;

    if ( pGetGSGData )
		m_pMD5Array = LoadMD5SigFromStream(pGetGSGData);
	if ( pGetGSGData && !m_pMD5Array ) 
		hRC = E_FAIL;
	if ( m_pMD5Array && !SortGSG() )
		hRC = E_FAIL;
    if ( SUCCEEDED(hRC) && pGetHeaderData )
		hRC = ProfileDBOpenFromStream(pGetHeaderData, m_pPDB);
	if ( SUCCEEDED(hRC) && !m_BKFBase.Reinit(pszRecentTermsBasePath))
		hRC = E_FAIL;

	if ( SUCCEEDED(hRC) )
		m_bIsValid = true;
    return hRC;
}

#define FREE(p) if (p) {free(p), p=NULL;}

void free_pdb(profile_db*& pdb)
{
    if ( pdb )
	{
		profile_db_close(pdb);
		pdb = NULL;
	}
}

HRESULT CDataBase::ProfileDBOpenFromStream(IN IStream* pGetData, profile_db*& pReturn)
{
	if ( !pGetData )
		return E_FAIL;
	
    pReturn = NULL;
    profile_db *pdb = (profile_db*)malloc(sizeof(profile_db));
	if ( !pdb )
		return E_FAIL;
    memset(pdb, 0, sizeof(profile_db));
    pdb->fd = -1;
	u_long profiles_count, profile_size;
	char   *buf       = NULL;
	size_t  buf_alloc = 0;
    ULONG ulRead = 0;
	HRESULT hr = S_OK;
    ULONG ulSignature = 0;
    SHORT usMajVer = 0, usMinVer = 0;

    // { read header (pdb signature, major version, minor version)
    hr = pGetData->Read( &ulSignature, sizeof(ulSignature), &ulRead);
    if (FAILED(hr) || !ulRead)
        return hr;
    if ( 0x20050901UL != ntohl(ulSignature) )
        return E_FAIL;

    hr = pGetData->Read( &usMajVer, sizeof(usMajVer), &ulRead);
    if (FAILED(hr) || !ulRead)
        return hr;
    if ( 1 != ntohs(usMajVer))
        return E_FAIL;

    hr = pGetData->Read( &usMinVer, sizeof(usMinVer), &ulRead);
    if (FAILED(hr) || !ulRead)
        return hr;
    // }



	hr = pGetData->Read(
        &profiles_count, 
        sizeof(u_long), 
        &ulRead
        );
    if ( FAILED(hr) || !ulRead)
	{
		free_pdb(pdb);
        return hr;
	}
	pdb->profiles.alloc = ntohl(profiles_count);
	pdb->profiles.array = (profile **)malloc(sizeof(profile *)*pdb->profiles.alloc);
	if ( !pdb->profiles.array ) hr = E_FAIL;
    if ( FAILED(hr) )
	{
		free_pdb(pdb);
		return hr;
	}
	memset(pdb->profiles.array, 0, sizeof(profile *)*pdb->profiles.alloc);

	for(pdb->profiles.used = 0; pdb->profiles.used < pdb->profiles.alloc; pdb->profiles.used++)
	{
		hr = pGetData->Read(&profile_size, sizeof(u_long), &ulRead);
        if ( FAILED(hr) || !ulRead )
            break;
		profile_size = ntohl(profile_size);

		if ( buf_alloc < profile_size )
		{
			FREE(buf);
			buf = (char *)malloc(buf_alloc = profile_size);
		}
        if ( !buf )
            break;

		if ( profile_size )
		{
			hr = pGetData->Read(buf, profile_size, &ulRead);
            if ( FAILED(hr) || !ulRead )
                break;
			pdb->profiles.array[pdb->profiles.used] = profile_load(buf, profile_size);
		}
	}

	FREE(buf);

    if (SUCCEEDED(hr))
        pReturn = pdb;
    else
		free_pdb(pdb);
    return hr;
}

HRESULT CDataBase::ForciblyRead(IN IStream* pGetData, void* buf, size_t size)
{
    size_t offset = 0;
    ULONG rc;
    while(offset<size) {
        int to_read = size-offset;
        if(to_read>4096) to_read = 4096;
        HRESULT hr = pGetData->Read((void*)(((char*)buf)+offset), to_read, &rc);
        if(FAILED(hr) || rc<1)
            return hr;
        offset+=rc;
    }
    return S_OK;
}

md5sigarray* CDataBase::LoadMD5SigFromStream(IN IStream* pGetData)
{
    if (!pGetData)
        return NULL;
	
    size_t rs;
    md5sigarray_fileheader header;
    md5sigarray *ret;
    ULONG ulRead = 0;
    HRESULT hr;
    hr = pGetData->Read(&header, sizeof(header), &ulRead);
    if (FAILED(hr) || !ulRead)
        return NULL;
    ret = (md5sigarray*)malloc(sizeof(md5sigarray));
    ret->magic = header.magic;
    ret->count = header.count;
    memcpy(ret->cksum, header.cksum, sizeof(md5sum));
    rs = header.count * sizeof(md5sum);

    /*ret->mmaped_array_size = 0;*/
    ret->array = (md5sum*)malloc(rs);

    if( FAILED(ForciblyRead(pGetData, ret->array, rs))) 
    {
        FREE(ret->array);
        FREE(ret);
        return NULL;
    }
    if((ret->magic!=SIGMAGIC)||(md5checkdata(ret)!=0))
	{
	    FREE(ret->array);
        FREE(ret);
	    return NULL;
	}
    
    // read GSG configuration
    unsigned char gsg_params_magic [GSG_PARAMS_MAGIC_SIZE];
    bool gsg_params_extracted = false;

    hr = pGetData->Read(&gsg_params_magic, GSG_PARAMS_MAGIC_SIZE, &ulRead);
    if (!FAILED(hr) && GSG_PARAMS_MAGIC_SIZE == ulRead)
        if (0 == memcmp ((const void*)GSG_PARAMS_MAGIC, (const void*)&gsg_params_magic, GSG_PARAMS_MAGIC_SIZE))
        {
            unsigned int fs_size = 0;
            hr = pGetData->Read(&fs_size, sizeof (unsigned int), &ulRead); // sizeof (unsigned int) bytes has been writted
            if (!FAILED(hr) && sizeof (unsigned int) == ulRead)
            {
                unsigned char* raw_gsg_params = new unsigned char [fs_size];
                if (raw_gsg_params)
                {
                    hr = pGetData->Read(raw_gsg_params, fs_size, &ulRead);
                    if (!FAILED(hr) && fs_size == ulRead)
                        if ( (int)fs_size == binary_serialize_gsg_params(m_pBinaryProcessor, raw_gsg_params, 
                                                                    fs_size, BINARY_SEIALIZE_GSG_PARAMS_LOAD) )
                            gsg_params_extracted = true;
                    delete [] raw_gsg_params;
                }
            }
        }
    if (!gsg_params_extracted)
    {
        FREE(ret->array);
        FREE(ret);
        return NULL;
    }

    return ret;
}

HRESULT CDataBase::Done()
{
    if ( m_pMD5Array )
    {
        FREE(m_pMD5Array->array);
        FREE(m_pMD5Array);
    }
	free_pdb(m_pPDB);
    if (m_pBinaryProcessor)
        binary_free (&m_pBinaryProcessor);
	m_BKFBase.Done();
	m_bIsValid = false;
    return S_OK;
}

int CDataBase::MD5SigCmp(const void* a0, const void *b0)
{
    unsigned char *a = (unsigned char*)a0, *b = (unsigned char*)b0;
	return md5sigcmp(a,b);
}

bool CDataBase::SortGSG()
{
    if (NULL == m_pMD5Array)
        return false;
    
    qsort(m_pMD5Array->array, m_pMD5Array->count, sizeof(md5sum), MD5SigCmp);
    
    return true;
}

/************************************************************************/
/* CBKFBase                                                             */
/************************************************************************/

static const TCHAR BKF_FILTER_DLL[] = "cfilter3.dll";
static const TCHAR BKF_DATA_DLL[] = "cfdata3.dll";

CBKFBase::CBKFBase() :
	m_hBKFFilter(NULL),
	m_hBKFData(NULL),
	m_pszBasePath(NULL),
	m_filter_open(NULL),
	m_filter_close(NULL),
	m_filter_check(NULL),
	m_filter_result_free(NULL),
	m_filter_object(NULL),
	m_message_create_mime(NULL),
	m_message_destroy(NULL),
	m_flt_category_find(NULL),
	m_flt_category_get_text_id(NULL),
	m_flt_category_get_title(NULL),
	m_cfdata_compile(NULL)
{
#ifdef _PROTECT_AS_TRM_NEW
	InitializeCriticalSection(&m_csNewBase);
#endif
}

CBKFBase::~CBKFBase()
{
	Done();
	if(m_pszBasePath)
		free(m_pszBasePath);
#ifdef _PROTECT_AS_TRM_NEW
	DeleteCriticalSection(&m_csNewBase);
#endif
}

bool CBKFBase::Reinit(IN const char *pszRecentTermsBasePath)
{
	PR_TRACE((0, prtIMPORTANT, "bkf\tReloading '%s'", pszRecentTermsBasePath));
	if(m_pszBasePath)
		free(m_pszBasePath);
	m_pszBasePath = _strdup(pszRecentTermsBasePath);
	if(!m_hBKFFilter)
	{
		if((m_hBKFFilter = LoadLibrary(BKF_FILTER_DLL)) == NULL)
		{
			PR_TRACE((0, prtERROR, "bkf\tFailed to load %s (0x%08x)", BKF_FILTER_DLL, GetLastError()));
			return false;
		}
		m_filter_open = (fn_filter_open)GetProcAddress(m_hBKFFilter, "filter_open");
		m_filter_close = (fn_filter_close)GetProcAddress(m_hBKFFilter, "filter_close");
		m_filter_check = (fn_filter_check)GetProcAddress(m_hBKFFilter, "filter_check");
		m_filter_result_free = (fn_filter_result_free)GetProcAddress(m_hBKFFilter, "filter_result_free");
		m_message_create_mime = (fn_message_create_mime)GetProcAddress(m_hBKFFilter, "message_create_mime");
		m_message_destroy = (fn_message_destroy)GetProcAddress(m_hBKFFilter, "message_destroy");
		m_flt_category_find = (fn_flt_category_find)GetProcAddress(m_hBKFFilter, "flt_category_find");
		m_flt_category_get_text_id = (fn_flt_category_get_text_id)GetProcAddress(m_hBKFFilter, "flt_category_get_text_id");
		m_flt_category_get_title = (fn_flt_category_get_title)GetProcAddress(m_hBKFFilter, "flt_category_get_title");
	}
	if(!m_filter_open || !m_filter_close || !m_filter_check ||
			!m_filter_result_free || !m_message_create_mime || !m_message_destroy ||
			!m_flt_category_find || !m_flt_category_get_text_id || !m_flt_category_get_title)
	{
		PR_TRACE((0, prtERROR, "bkf\tIncorrect %s", BKF_FILTER_DLL));
		PR_ASSERT(0);
		return false;
	}
	if(!m_hBKFData)
	{
		if((m_hBKFData = LoadLibrary(BKF_DATA_DLL)) == NULL)
		{
			PR_TRACE((0, prtERROR, "bkf\tFailed to load %s (0x%08x)", BKF_DATA_DLL, GetLastError()));
			return false;
		}
		m_cfdata_compile = (fn_cfdata_compile)GetProcAddress(m_hBKFData, "cfdata_compile");
	}
	if(!m_cfdata_compile)
	{
		PR_TRACE((0, prtERROR, "bkf\tIncorrect %s", BKF_DATA_DLL));
		PR_ASSERT(0);
		return false;
	}
	bool bRet = CompileBaseIfNeeded();
	if(bRet)
	{
		PR_TRACE((0, prtERROR, "bkf\tReloading '%s' succeeded", pszRecentTermsBasePath));
	}
	else
	{
		PR_TRACE((0, prtERROR, "bkf\tReloading '%s' failed", pszRecentTermsBasePath));
	}
	return bRet;
}

void CBKFBase::Done()
{
	if(m_filter_close && m_filter_object)
	{
		m_filter_close(m_filter_object, FILTER_DEFAULTS);
		m_filter_object = NULL;
	}
	if(m_hBKFFilter)
	{
		FreeLibrary(m_hBKFFilter);
		m_hBKFFilter = NULL;
	}
	if(m_hBKFData)
	{
		FreeLibrary(m_hBKFData);
		m_hBKFData = NULL;
	}
}

bool CBKFBase::CheckLoadBase()
{
	if(!m_pszBasePath || !m_filter_open)
		return false;

	char szNewBasePath[MAX_PATH];
	strcpy_s(szNewBasePath, _countof(szNewBasePath), m_pszBasePath);
	// проверим есть ли .new файл
	if(strcat_s(szNewBasePath, _countof(szNewBasePath), ".new") != ERANGE)
	{
#ifdef _PROTECT_AS_TRM_NEW
		EnterCriticalSection(&m_csNewBase);
#endif
		if(_access(szNewBasePath, 0) != -1)
		{
			// выгружаем загруженную базу
			if(m_filter_close && m_filter_object)
			{
				m_filter_close(m_filter_object, FILTER_DEFAULTS);
				m_filter_object = NULL;
			}
			_unlink(m_pszBasePath);
			rename(szNewBasePath, m_pszBasePath);
		}
#ifdef _PROTECT_AS_TRM_NEW
		LeaveCriticalSection(&m_csNewBase);
#endif
	}

	if(m_filter_object)
		return true; // уже загружена
	m_filter_object = m_filter_open(m_pszBasePath);
	if(!m_filter_object)
	{
		PR_TRACE((0, prtERROR, "bkf\tFailed to open '%s' (0x%08x)", m_pszBasePath, GetLastError()));
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "bkf\t'%s' loaded", m_pszBasePath));
	}
	return m_filter_object != NULL;
}

struct BKFBaseItem
{
	std::string name;
	time_t time;
	int method;

	bool operator ==(const BKFBaseItem &item) const
	{
		return name == item.name && time == item.time;
	}
};

class compare_name_pred : public std::binary_function<BKFBaseItem, std::string, bool>
{
public:
	bool operator()(const BKFBaseItem& _L, const std::string &_R) const
	{
		return _L.name == _R;
	}
};

void prague_tracer(const char *str...)
{
    va_list list;
    va_start(list, str);
    char buff[1024];
    vsnprintf(buff, sizeof(buff), str, list);
    PR_TRACE((0, prtERROR, "bkf\t%s", buff));
    va_end(list);
}

bool CBKFBase::CompileBaseIfNeeded()
{
	// получим путь до recent_terms.last и recent_terms.set
	if(!m_pszBasePath)
		return false;
	char szBasesPath[MAX_PATH];
	char szRTLastPath[MAX_PATH];
	char szRTSetPath[MAX_PATH];
	char szNewBasePath[MAX_PATH];
	strcpy_s(szRTLastPath, _countof(szRTLastPath), m_pszBasePath);
	char *pLastSlash = strrchr(szRTLastPath, '\\');
	if(pLastSlash && (pLastSlash + 1) < szRTLastPath + _countof(szRTLastPath))
		*(pLastSlash + 1) = 0;
	strcpy_s(szBasesPath, _countof(szBasesPath), szRTLastPath);
	strcpy_s(szRTSetPath, _countof(szRTSetPath), szRTLastPath);
	if(strcat_s(szRTLastPath, _countof(szRTLastPath), "rterms.last") == ERANGE)
		return false;
	if(strcat_s(szRTSetPath, _countof(szRTSetPath), "rterms.set") == ERANGE)
		return false;
	strcpy_s(szNewBasePath, _countof(szNewBasePath), m_pszBasePath);
	if(strcat_s(szNewBasePath, _countof(szNewBasePath), ".new") == ERANGE)
		return false;

	// посмотрим, что есть из транспортных баз на диске
	std::vector<BKFBaseItem> base_files_actual;
	if(FILE *f = fopen(szRTSetPath, "rt"))
	{
		char line[512], path[MAX_PATH];
		int method, s;
		while(fgets(line, _countof(line), f))
		{
			method = CFD_FI_XML_IN_MEMORY;
			s = sscanf_s(line, "%[^,],%d", path, _countof(path), &method);
			if(s == 1 || s == 2)
			{
				struct _stat s;
				std::string sPath = szBasesPath;
				sPath += path;
				if(_stat(sPath.c_str(), &s) == 0)
				{
					BKFBaseItem item;
					item.name = sPath;
					item.time = s.st_mtime;
					item.method = method;
					base_files_actual.push_back(item);
				}
			}
		}		
		fclose(f);
	}
	// теперь, что есть из последней компиляции
	std::vector<BKFBaseItem> base_files_last;
	if(FILE *f = fopen(szRTLastPath, "rt"))
	{
		char line[512], path[MAX_PATH];
		time_t t;
		while(fgets(line, _countof(line), f))
		{
			if(sscanf_s(line, "%s %I64d", path, _countof(path), &t) == 2)
			{
				BKFBaseItem item;
				item.name = path;
				item.time = t;
				item.method = -1;
				base_files_last.push_back(item);
			}
		}		
		fclose(f);
	}
	// если ничего не поменялось и файл базы есть, выходим
	if(base_files_actual == base_files_last && 
			(_access(m_pszBasePath, 0) == 0 || _access(szNewBasePath, 0) == 0))
	{
		PR_TRACE((0, prtIMPORTANT, "bkf\tNothing changed"));
		return true;
	}
	// проверим, не надо ли удалить старые файлы
	for(size_t i = 0, size = base_files_last.size(); i < size; ++i)
	{
		if(std::find_if(base_files_actual.begin(),
						base_files_actual.end(),
						std::bind2nd(compare_name_pred(), base_files_last[i].name)) == base_files_actual.end())
		{
			PR_TRACE((0, prtIMPORTANT, "bkf\tDeleting '%s'", base_files_last[i].name.c_str()));
			_unlink(base_files_last[i].name.c_str());
		}
	}
	// компилируем базу
	bool base_compiled_ok = true;
	if(!base_files_actual.empty())
	{
		// unsqueeze
        std::vector<std::pair<Squeeze::CFile *, int> > unsqueezed_files;
		for(size_t i = 0, size = base_files_actual.size(); i < size; ++i)
		{
			std::vector<Squeeze::CFile *> files;
			int unsqu_res;
			PR_TRACE((0, prtIMPORTANT, "bkf\Unpacking '%s'", base_files_actual[i].name.c_str()));
			try
			{
				unsqu_res = Squeeze::unsqueeze(base_files_actual[i].name.c_str(), files, prague_tracer);
			}
			catch(std::exception &e)
			{
				unsqu_res = 1;
				PR_TRACE((0, prtERROR, "bkf\tException in unpack (%s)", e.what()));
			}
			base_compiled_ok = unsqu_res == 0 && files.size() == 1;
			if(!base_compiled_ok)
			{
				PR_TRACE((0, prtERROR, "bkf\tFailed to unpack base %s (error = %d, files = %d)",
					base_files_actual[i].name.c_str(), unsqu_res, files.size()));
			}
			else
			{
				// transfer ownership to unsqueezed_files array
				unsqueezed_files.push_back(std::make_pair(files[0], base_files_actual[i].method));
				files.erase(files.begin());
			}
			for(size_t j = 0, szf = files.size(); j < szf; ++j)
				delete files[j];
			if(!base_compiled_ok)
				break;
		}
		
		if(base_compiled_ok)
		{
			size_t size = unsqueezed_files.size();
			// compilation
			cfd_file_info *fi = new cfd_file_info[size];
			if(!fi)
				return false;
			for(size_t i = 0; i < size; ++i)
			{
				fi[i].name = NULL;
				fi[i].type = unsqueezed_files[i].second;
				fi[i].password = NULL;
				fi[i].file_contents = (const char *)&unsqueezed_files[i].first->body[0];
				fi[i].file_contents_size = unsqueezed_files[i].first->body.size();
			}
			cfd_state_config sc;
			memset(&sc, 0, sizeof(sc));
			
#ifdef _PROTECT_AS_TRM_NEW
			EnterCriticalSection(&m_csNewBase);
#endif
			if(!(base_compiled_ok = m_cfdata_compile(szNewBasePath, NULL, &sc, NULL, fi, size) == CFLT_OK))
			{
				PR_TRACE((0, prtERROR, "bkf\tFailed to compile base '%s' (error = %d, comp = %d)",
								szNewBasePath, sc.error.err_code, sc.error.err_component));
			}
			else
			{
				PR_TRACE((0, prtIMPORTANT, "bkf\tBase compiled successfully"));
			}
#ifdef _PROTECT_AS_TRM_NEW
			LeaveCriticalSection(&m_csNewBase);
#endif

			delete []fi;
		}

		for(size_t j = 0, szf = unsqueezed_files.size(); j < szf; ++j)
			delete unsqueezed_files[j].first;
		
		if(!base_compiled_ok)
			return false;
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "bkf\tNo files to compile"));
		// выставляем успех, если в этом случае есть откомпилированная база
		base_compiled_ok = _access(m_pszBasePath, 0) == 0;
	}
	// записываем новый recent_terms.last
	FILE *f;
	if(base_compiled_ok && (f = fopen(szRTLastPath, "wt")) != NULL)
	{
		char line[512];
		for(size_t i = 0, size = base_files_actual.size(); i < size; ++i)
		{
			sprintf_s(line, _countof(line), "%s %I64d\n", base_files_actual[i].name.c_str(), base_files_actual[i].time);
			fputs(line, f);
		}
		fclose(f);
	}
	return base_compiled_ok;
}

bool CBKFBase::ProcessBKFMessage(message &msg, unsigned int *pRelevantIndex) const
{
	bool bResult = false;
	filter_result* result = m_filter_check(m_filter_object, &msg, NULL, FILTER_DEFAULTS);
	if(result)
	{
		if(result->error_code == FR_OK)
		{
			for(size_t i = 0; i < result->used; ++i)
			{
				const flt_category *pCategory = m_flt_category_find(m_filter_object, result->res[i].CID);
				if(pCategory)
				{
					const char* szTextId = m_flt_category_get_text_id(pCategory);
					if(_stricmp(szTextId, "Spam") == 0)
					{
						if(pRelevantIndex)
							*pRelevantIndex = result->res[i].relevant_index;
						break;
					}
				}
			}
			bResult = true;
		}
		else
		{
			PR_TRACE((0, prtERROR, "bkf\tFailed to check message (0x%08x)", result->error_code));
		}
		m_filter_result_free(result);
	}
	return bResult;
}

bool CBKFBase::ProcessMIMEMessage(const char *pszMessage, unsigned int *pRelevantIndex)
{
	if(!pszMessage)
	{
		PR_ASSERT(0);
		return false;
	}
	if(!CheckLoadBase())
		return false;
	if(pRelevantIndex)
		*pRelevantIndex = -1;
	bool bResult = false;
	int res;
	message msg;
	memset(&msg, 0, sizeof(msg));
	if((res = m_message_create_mime(&msg, pszMessage, strlen(pszMessage))) == 0)
	{
		bResult = ProcessBKFMessage(msg, pRelevantIndex);
		m_message_destroy(&msg);
	}
	else
	{
		PR_TRACE((0, prtERROR, "bkf\tFailed to create MIME (0x%08x)", res));
	}
	return bResult;
}

bool CBKFBase::ProcessMessage(const char *pszSubject, const char *pszBody, unsigned int *pRelevantIndex)
{
	if(!pszSubject || !pszBody)
	{
		PR_ASSERT(0);
		return false;
	}
	if(!CheckLoadBase())
		return false;
	if(pRelevantIndex)
		*pRelevantIndex = -1;
	message msg;
	memset(&msg, 0, sizeof(msg));
	msg.body.ptr = pszBody;
	msg.body.count = strlen(pszBody);
	msg.subject.ptr = pszSubject;
	msg.subject.count = strlen(pszSubject);
	return ProcessBKFMessage(msg, pRelevantIndex);
}
