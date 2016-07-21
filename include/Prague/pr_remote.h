// -------------------------------------------
/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	Remote.h
 * \author	Pavel Mezhuev
 * \date	05.12.2002 10:25:41
 * \brief	Интерфейс для удаленного взаимодействия компонент.
 * 
 */
// -------------------------------------------

#ifndef __PR_REMOTE_H
#define __PR_REMOTE_H

#include <Prague/prague.h>
#include <Prague/pr_loadr.h>

#if defined (_WIN32)
#	ifdef DISTRIBUTED_PRODUCT_EXPORT
#		define PR_API EXTERN_C __declspec(dllexport)  
#	else
#		define PR_API EXTERN_C __declspec(dllimport) 
#   endif
#else
#   include <string.h>
#   include <stdio.h>
#	define PR_API EXTERN_C pr_call 
#endif


typedef tDWORD tSESSION;		// Дескриптор сессии
typedef tDWORD tPROCESS;		// Дескриптор процесса
typedef tDWORD tLIBRARY;		// Дескриптор библиотеки

#define PID_REMOTE_PRAGUE	        ((tPID)12345)

#define PR_PROCESS_LOCAL			(tDWORD)(0)		// Предопределенный дескриптор текущего процесса
#define PR_PROCESS_ANY				(tDWORD)(-1)	// Предопределенный дескриптор для любого процесса
#define PR_PROCESS_ALL				(tDWORD)(-2)	// Предопределенный дескриптор
#define PR_PROCESS_ALL_IMPERSONATED	(tDWORD)(-3)	// Предопределенный дескриптор

#define PR_SESSION_LOCAL			(tDWORD)(-1)	// Предопределенный дескриптор локальной сессии
#define PR_SESSION_ACTIVE			(tDWORD)(-2)	// Предопределенный дескриптор активной сессии
#define PR_SESSION_ALL			    (tDWORD)(-3)	// 
#define PR_SESSION_BY_USER_TOKEN	(tDWORD)(-4)	// Предопределенный дескриптор для сессии по имперсонации потока

#define PR_CREATE_IN_PARENTAL_PROCESS	  (tDWORD)(1)
#define PR_CREATE_IN_IMPERSONATED_PROCESS (tDWORD)(2)
#define PR_CREATE_IN_SPECIFIED_PROCESS    (tDWORD)(4)

#define IID_REMOTE_STUB		        0x81000000
#define IID_REMOTE_PROXY	        0x82000000
#define IID_REMOTE_ROOT		        0x83000000

#define PR_REMOTE_MODULE_NAME       "prremote.dll"
#define cpnPRAGUE_REMOTE_API        ((tSTRING)("cpnPRAGUE_REMOTE_API"))

#define pgPROXY_CREATE_REMOTE        mPROPERTY_MAKE_GLOBAL(pTYPE_BOOL, 0x000000ff)

// -------------------------------------------
// Local prague

#define PR_FLAG_DEFAULT					0x00000000
#define PR_FLAG_UNLOAD_LOADER		0x80000000

// При инициализации Праги под Вистой в процессе с Low Integrity (например внутри IE7)
// нужно выставлять этот флаг. (Не под Вистой игнорируется)
#define PR_FLAG_LOW_INTEGRITY_PROCESS 0x40000000

// Для того, чтобы Прага работала в процессе с Low Integrity, один (главный) процесс с 
// Medium или High Integrity должен выставить этот флаг. (Не под Вистой игнорируется)
#define PR_FLAG_LOW_INTEGRITY_HELPER  0x20000000

typedef struct tag_PRInitParams
{
	tDSKMAppId* app_id;
	tPTR        ctx;
	tERROR (*init_clb)(hROOT root, tPTR ctx, tBOOL remote);
} PRInitParams;

/*!
  \brief Инициализация локальной распределенной Prague.
		Необходимо вызвать, для использования Prague в текущем процессе.
		Если метод уже был вызван, то просто увеличится счетчик ссылок.

    \param flags		  [in]  Флаги инициализации. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRInitialize(tDWORD flags, PRInitParams* params);

/*!
  \brief Необходимо вызвать для деиницилизации Prague.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRDeinitialize();

/*!
  \brief Получить корень иерархии Prague.
	Можно вызывать после "PRInitialize".

    \param root           [out]  Корень иерархии. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRGetRoot(hROOT *root);

/*!
  \brief Остановка приема запросов.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRStopServer();

/*!
  \brief Зарегистрировать сервер

    \param port			  [in]  Номер порта для соеденения. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRRegisterServer(tDWORD port);

/*!
  \brief Зарегистрировать объект для возможности его использования
	в любой компоненте продукта.

    \param name			  [in]  Идентификатор объекта. 
    \param object         [in]  Регистрируемый объект. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRRegisterObject(tSTRING name, hOBJECT object);

/*!
  \brief Дерегистрировать объект.

    \param object         [in]  Зарегистрированный объект. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRUnregisterObject(hOBJECT object);

/*!
  \brief Зарегистрировать информацию о способе создания объекта по pid-у.

    \param pid			  [in]  Идентификатор плагина. 
    \param flags          [in]  Флаги создания. 
    \param process        [in]  Процесс для создания. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRRegisterCreationInfo(tPID pid, tDWORD flags, tPROCESS process, tSTRING exe_name);

PR_API tERROR PRUregisterCreationInfo(tPID pid);

/*!
  \brief Создать процесс в указанной сессии

    \param session_id     [in]  Идентификатор сессии. 
    \param exec_path      [in]  Имя хоста для запуска. 
    \param exec_args      [in]  Аргументы для запуска. 
    \param timeout        [in]  Таймаут ожидания запуска. 
    \param process        [out] Дескриптор процесса. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRCreateProcess(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tBOOL child, tPROCESS *process);

/*!
  \brief Запрос на закрытие процесса

    \param process        [in] Дескриптор процесса. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRCloseProcessRequest(tPROCESS process);

/*!
  \brief Запрос на закрытие процесса

    \param process        [in] Дескриптор процесса. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PREstablishConnections(tSTRING name);

/*!
  \brief Запрос на закрытие процесса

    \param process        [in] Дескриптор процесса. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRIsSystemProcess(tPROCESS process);

/*!
  \brief Создать процесс-хост для объекта в указанной сессии

    \param session_id     [in]  Идентификатор сессии. 
    \param exec_path      [in]  Имя хоста для запуска. 
    \param exec_args      [in]  Аргументы для запуска. 
    \param object_name    [in]  Имя объекта, регистрацию которого нужно дождаться.
    \param timeout        [in]  Таймаут ожидания запуска. 
    \param process        [out] Прокси на объект. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRCreateObjectProxy(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tSTRING object_name, hOBJECT *proxy);

/*!
  \brief Удаленное создание объекта.

    \param process		  [in]  Дескриптор процесса. 
    \param object         [in]  Родительский объект. 
    \param new_obj		  [out] Созданный объект. В случае, если объект создан в другом процессе, то это прокси на объект.
    \param iid			  [in]  Идентификатор интерфейса.
    \param pid			  [in]  Идентификатор плагина.
    \param subtype		  [in]  Подтип интерфейса.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRCreateObject(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);

PR_API tERROR PRCreateObjectQuick(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);


// -------------------------------------------
// Remote prague

PR_API tERROR PRRegisterHost(tSTRING host, tDWORD port, tPROCESS *process);

/*!
  \brief Получить прокси на корень иерархии в заданном процессе.

    \param process		  [in]  Дескриптор процесса. 
    \param root           [out] Корень иерархии.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRGetRootProxy(tPROCESS process, hROOT *root);

/*!
  \brief Получить прокси на объект в заданном процессе, с заданным идентификатором.

    \param process		  [in]  Дескриптор процесса. 
    \param name			  [in]  Идентификатор объекта. 
    \param object         [out] Если объет нахобиться в данном процессе,
									то обычный Prague-object,
									иначе прокси на объект в другом процессе.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRGetObjectProxy(tPROCESS process, tSTRING name, hOBJECT *object);

/*!
  \brief Получить дискриптор процеесса, в котором создан объект.

    \param object         [in] Объект.
    \param process        [out] Возвращаемый дискриптор процесса.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRGetObjectProcess(hOBJECT object, tPROCESS *process);

/*!
  \brief Необходимо вызвать после использования объекта.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRReleaseObjectProxy(hOBJECT object);


PR_API tERROR PRAddRefObjectProxy(hOBJECT object);

/*!
  \brief Выставить callback на разрыв соединения.

    \param object         [in] Прокси на объект.
    \param callback       [in] Callback на разрыв соединения.
    \param context        [in] Контекст вызова callback'а.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRSetRunDownCallback(hOBJECT object, void (*callback)(hOBJECT object, void *context), void *context);

/*!
  \brief Проверить сотояние прокси на объект.

    \param object         [in] Прокси на объект.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRIsValidProxy(hOBJECT object);

// -------------------------------------------

typedef struct tag_PRRemoteAPI
{
	tERROR (pr_call *Initialize)(tDWORD flags, PRInitParams* params);
	tERROR (pr_call *Deinitialize)();
	tERROR (pr_call *GetRoot)(hROOT *root);

	tERROR (pr_call *StopServer)();
	tERROR (pr_call *RegisterServer)(tDWORD port);
	tERROR (pr_call *RegisterHost)(tSTRING host, tDWORD port, tPROCESS *process);
	tERROR (pr_call *RegisterObject)(tSTRING name, hOBJECT object);
	tERROR (pr_call *UnregisterObject)(hOBJECT object);

	tERROR (pr_call *CreateProcess)(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tBOOL child, tPROCESS *process);
	tERROR (pr_call *CreateObjectProxy)(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tSTRING object_name, hOBJECT *proxy);
	tERROR (pr_call *CloseProcessRequest)(tPROCESS process);

	tERROR (pr_call *CreateObject)(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);
	tERROR (pr_call *CreateObjectQuick)(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);

	tERROR (pr_call *GetRootProxy)(tPROCESS process, hROOT *root);
	tERROR (pr_call *GetObjectProxy)(tPROCESS process, tSTRING name, hOBJECT *object);
	tERROR (pr_call *GetObjectProcess)(hOBJECT object, tPROCESS *process);
	tERROR (pr_call *ReleaseObjectProxy)(hOBJECT object);
	tERROR (pr_call *AddRefObjectProxy)(hOBJECT object);
	tERROR (pr_call *SetRunDownCallback)(hOBJECT object, void (*callback)(hOBJECT object, void *context), void *context);
	tERROR (pr_call *IsValidProxy)(hOBJECT object);

	tERROR (pr_call *EstablishConnections)(tSTRING name);
	tERROR (pr_call *IsSystemProcess)(tPROCESS process);

	tERROR (pr_call *RegisterCreationInfo)(tPID pid, tDWORD flags, tPROCESS process, tSTRING exe_name);
	tERROR (pr_call *UregisterCreationInfo)(tPID pid);

	tERROR (pr_call *ImpersonateAsAdmin)(hOBJECT object);

	tPTR    dummy[7]; // for expandable architecture
} PRRemoteAPI;

/*!
  \brief Получить PRRemoteAPI.

    \param api         [out] PRRemoteAPI.

    \return При успешном завершении - errOK.
*/
PR_API tERROR PRGetAPI(PRRemoteAPI *api);

// -------------------------------------------
// Remote library

/*!
  \brief Получить дескриптор библиотеки в заданном процессе.
	Если process=NULL, то запуск будет в текущем процессе.
	Если библиотека уже запущена, то дескриптор будет указывать на нее.

    \param process         [in]  Дескриптор процесса. 
    \param name           [in]  Полное название библиотеки. 
    \param library		  [out] Дескриптор библиотеки. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR RLGetLibrary(tPROCESS process, tSTRING name, tLIBRARY *library);

// Название экспортированной функции
#define INVOKE_HANDLER	"InvokeHandler"

// Прототип функции для вызова "RLInvokeMethod"
typedef void (*PRInvokeHandler)(tDWORD method, void *param, tDWORD param_size);

/*!
  \brief Вызвать метод INVOKE_HANDLER в заданной библиотеки.

    \param library		  [in]  Дескриптор библиотеки. 
    \param method         [in]  Идентификатор метода. 
    \param param          [in,out]  Параметры вызова, должны быть линейными. 
    \param param_size     [in]  Размер данных параметров. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR RLInvokeMethod(tLIBRARY library, tDWORD method, void *param, tDWORD param_size);

/*!
  \brief Необходимо вызвать после использования библиотеки.

    \param library		  [in]  Дескриптор библиотеки. 

    \return При успешном завершении - errOK.
*/
PR_API tERROR RLReleaseLibrary(tLIBRARY library);


// -------------------------------------------
#if defined(__cplusplus)

#include "iface/i_root.h"

class CPRRemotePrague : public PRRemoteAPI
{
public:
#if defined (_WIN32)
	CPRRemotePrague(HINSTANCE hModule, tDWORD nFlags = 0) : m_root(NULL)
	{
		Init(hModule, nFlags);
	}
#else
	CPRRemotePrague(tDWORD nFlags, PRInitParams* params = NULL) : m_root(NULL) 
	{
		Init(nFlags);
	}
#endif
	~CPRRemotePrague()
	{
		Deinit();
	}

#if defined (_WIN32)
	tERROR Init(LPCSTR strPath, tDWORD nFlags = 0, PRInitParams* params = NULL)
	{
		m_hModule = LOAD_LIBRARY(strPath);
		if( !m_hModule )
			m_hModule = LOAD_LIBRARY(PR_REMOTE_MODULE_NAME);
		return InitRoot(nFlags, params);
	}


	tERROR Init(HINSTANCE hModule, tDWORD nFlags = 0, PRInitParams* params = NULL)
	{
		tCHAR szLoadPath[MAX_PATH];
		if (!GetModuleFileNameA(hModule, szLoadPath, sizeof(szLoadPath)))
			*szLoadPath = 0;
		tCHAR* pModuleName = strrchr(szLoadPath, '\\');
		if (!pModuleName)
			pModuleName = szLoadPath;
		else
			pModuleName++;
		strcpy_s(pModuleName, szLoadPath + countof(szLoadPath) - pModuleName, PR_REMOTE_MODULE_NAME);
		m_hModule = LOAD_LIBRARY(szLoadPath);

		if( !m_hModule )
			m_hModule = LOAD_LIBRARY(PR_REMOTE_MODULE_NAME);
		return InitRoot(nFlags, params);
	}

#else
	tERROR Init(tDWORD nFlags = 0, PRInitParams* params = NULL)
	{
		m_hModule = LOAD_LIBRARY("pr_remote." SHARED_EXT);
		return InitRoot(nFlags, params);
	}
#endif

	tERROR InitRoot(tDWORD nFlags = 0, PRInitParams* params = NULL)
	{
		if( !m_hModule )
			return m_err = errMODULE_NOT_FOUND;

		tERROR ( *hGetAPI )(PRRemoteAPI *api) = NULL;
		*(void**)&hGetAPI = GET_SYMBOL(m_hModule, "PRGetAPI");
		if( hGetAPI )
			m_err = hGetAPI(this);
		else
			Deinitialize = NULL;

		if( PR_SUCC(m_err) )
			m_err = Initialize(!nFlags ? PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH : nFlags, params);
		if( PR_SUCC(m_err) )
			m_err = GetRoot(&m_root);
		if( PR_SUCC(m_err) )
			g_root = m_root;

		return m_err;
	}

	void Deinit()
	{
		if( m_hModule )
		{
			if( Deinitialize )
				Deinitialize();
//			UNLOAD_LIBRARY(m_hModule);
			m_hModule = NULL;
		}
	}

	CPRRemotePrague() : m_hModule(NULL), m_root(g_root)
	{
		if( !m_root )
			return;

		tPROPID nProp;
		m_err = CALL_Root_RegisterCustomPropId(m_root, &nProp, cpnPRAGUE_REMOTE_API, pTYPE_PTR);
		if( PR_SUCC(m_err) )
		{
			PRRemoteAPI* pAPI = NULL;
			tDWORD size = sizeof(pAPI);

			m_err = CALL_SYS_PropertyGet(m_root, &size, nProp, &pAPI, size);

			if( pAPI )
				memcpy(this, pAPI, sizeof(PRRemoteAPI));
			else
				m_err = errINTERFACE_NOT_FOUND;
		}
	}

	tERROR LoadProxyStub(tSTRING strModule)
	{
		if( PR_FAIL(m_err) ) return m_err;
		return CALL_Root_LoadModule(m_root, NULL, strModule, (tDWORD)strlen(strModule), cCP_ANSI);
	}

	tERROR GetError()          { return m_err; };
	operator hROOT() const     { return m_root; };
	hROOT operator ->() const  { return m_root; };

private:
#if defined (_WIN32)
	HMODULE   m_hModule;
#else
        void*     m_hModule;
#endif
	hROOT     m_root;
	tERROR    m_err;
};

template <class Type = hOBJECT>
class CPRRemoteProxy
{
public:
	CPRRemoteProxy(CPRRemotePrague& pPrague) :
		m_pPrague(pPrague),
		m_strProxyName(NULL),
		m_hProxyObj(NULL)
	{
	}
	
	CPRRemoteProxy(CPRRemotePrague& pPrague, tSTRING strProxyName) :
		m_pPrague(pPrague),
		m_strProxyName(NULL),
		m_hProxyObj(NULL)
	{
		Init(strProxyName);
	}

	bool Init(tSTRING strProxyName)
	{
		if (!(hROOT)m_pPrague)
			return false;
		tDWORD len = (tDWORD)strlen(strProxyName) + 1;
		if( PR_FAIL(CALL_SYS_ObjHeapAlloc((hROOT)m_pPrague, (tPTR*)&m_strProxyName, len)) )
			return false;
		memcpy(m_strProxyName, strProxyName, len);
		return true;
	}

	~CPRRemoteProxy()
	{
		if( m_strProxyName && (hROOT)m_pPrague )
			CALL_SYS_ObjHeapFree((hROOT)m_pPrague, m_strProxyName);
		Detach();
	}

	bool operator !() const { return !m_hProxyObj; }
	operator Type() const   { return m_hProxyObj; }
	operator bool() const   { return !!m_hProxyObj; }
	Type operator ->()      { return m_hProxyObj; }

	bool IsValid()
	{
		return m_hProxyObj && PR_SUCC(m_pPrague.IsValidProxy((hOBJECT)m_hProxyObj));
	}

	bool Attach()
	{
		Detach();
		if (!(hROOT)m_pPrague)
			return false;
		return PR_SUCC(m_pPrague.GetObjectProxy(PR_PROCESS_ANY, m_strProxyName, (hOBJECT*)&m_hProxyObj));
	}

	void Detach()
	{
		if( m_hProxyObj )
			m_pPrague.ReleaseObjectProxy((hOBJECT)m_hProxyObj);
		m_hProxyObj = NULL;
	}

	bool Refresh(bool* bCreated = NULL)
	{
		if( IsValid() )
		{
			if( bCreated ) *bCreated = false;
			return true;
		}
		if( Attach() )
		{
			if( bCreated ) *bCreated = true;
			return true;
		}
		return false;
	}

private:
	CPRRemotePrague& m_pPrague;
	tSTRING     m_strProxyName;
	Type        m_hProxyObj;
};

#endif // __cplusplus

// -------------------------------------------

#endif  // __PR_REMOTE_H
