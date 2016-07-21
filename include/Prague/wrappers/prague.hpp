/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	prague.hpp
*		\brief	
*		
*		\author ¬ладислав ќвчарик
*		\date	02.06.2004 20:45:34
*		
*		
*/
#pragma once
#pragma warning(disable : 4786)
#include <Prague/prague.h>
#include <Prague/pr_loadr.h>
#include <Prague/pr_remote.h>
#include <Prague/iface/i_loader.h>
#include <Prague/iface/i_root.h>

#include <windows.h>
#include <stdexcept>

#include "scoped_handle.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace prague
{
///////////////////////////////////////////////////////////////////////////////
using ::tERROR;
using ::hROOT;
using ::hLOADER;
using ::tCHAR;
using ::hOBJECT;
using ::cObject;
///////////////////////////////////////////////////////////////////////////////
typedef PRRemoteAPI		pr_remote_t;
typedef PRInitParams	pr_init_params_t;
///////////////////////////////////////////////////////////////////////////////
typedef tERROR pr_call fn_pr_get_api(pr_remote_t *api);
///////////////////////////////////////////////////////////////////////////////
/**
 * return prague remote api
 * can throw std::runtime_error
 */
inline pr_remote_t pr_remote_api()
{
	HMODULE lib(GetModuleHandle(TEXT("pr_remote.dll")));
	if(!lib)
	{
		lib = GetModuleHandle(TEXT("prremote.dll"));
		if(!lib)
			throw std::runtime_error("remote prague is not initialize");
	}
	fn_pr_get_api *get_api =
		reinterpret_cast<fn_pr_get_api*>(GetProcAddress(lib, "PRGetAPI"));
	
	pr_remote_t api = { 0 };
	if(tERROR err = get_api(&api))
		throw std::runtime_error("PRGetAPI return error");

	return api;
}
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
inline char* get_module_path(char* module_path, size_t size)
{
	GetModuleFileNameA(GetModuleHandle(0), module_path, size);
	size_t index;
	for(index = size; index-- && module_path[index] != '\\';);
	return module_path + (index + 1);
}
///////////////////////////////////////////////////////////////////////////////
struct remote_api
{
	explicit remote_api(int flags = PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM, pr_init_params_t* params = 0)
		: remote_(load_remote_module())
	{
		if(!remote_)
			throw std::runtime_error("can not load remote prague");

		pr_remote_t api(pr_remote_api());
		tERROR err(api.Initialize(flags, params));
		if (PR_FAIL(err))
			throw std::runtime_error("remote prague can not be initialized");
	}
	~remote_api()
	{
		try
		{
			if(remote_)
				pr_remote_api().Deinitialize();
		}
		catch(...){}
	}
private:
	static HMODULE load_remote_module()
	{
		HMODULE h = ::LoadLibraryA("pr_remote.dll");
		if(!h)
			h = ::LoadLibraryA("prremote.dll");
		return h;
	}
private:
	util::scoped_handle
		<
			HMODULE,
			BOOL (WINAPI*)(HMODULE),
			&::FreeLibrary,
			0
		>								remote_;
};
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
class init_prague
{
public:
	explicit init_prague(char const** plugins)
		:	remote_()
	{
		load_plugins(plugins);
	}
	//!load all plugins
	//! parameter plugins has not used it need for overloading constructor
	init_prague(char const** plugins, int flags)
		:	remote_(flags)
	{
	}
	init_prague(char const** plugins, int flags, pr_init_params_t* params)
		:	remote_(flags, params)
	{
	}
	init_prague(init_prague const& other)
		:	remote_()
	{
	}
	~init_prague()
	{
	}
	void load_plugins(char const** plugins)
	{
		pr_remote_t api(pr_remote_api());
		hROOT hRoot(0);
		tERROR err(api.GetRoot(&hRoot));
		if(PR_FAIL(err))
			throw std::runtime_error("root object can not be initialized");
		hLOADER hLoader(0);
		err = hRoot->sysCreateObjectQuick(reinterpret_cast<hOBJECT*>(&hLoader), IID_LOADER);
		if (PR_FAIL(err))
			throw std::runtime_error("root object can not be initialized");
		
		char module_path[MAX_PATH] = { 0 };
		char *path = detail::get_module_path(module_path, MAX_PATH);

		for(;plugins && *plugins; plugins++)
		{
			strcpy(path, *plugins);
			char *dummy = module_path;
			err = hLoader->IntegrateModules(&dummy, sizeof(char*), fLOADER_PARAM_IS_STR_POINTER_LIST);
//			if (PR_FAIL(err))
//				throw std::runtime_error(module_path);
		}

		if(hLoader)
			hLoader->sysCloseObject();
	}
private:
	detail::remote_api remote_;
};
//////////////////////////////////////////////////////////////////////////////
template<typename T> class auto_obj;
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief	—сылка на auto_obj
 */
template<typename T>
struct auto_obj_ref
{
	auto_obj_ref(auto_obj<T>& ref)
		: _ref(ref)
	{
	}
	auto_obj<T>&	_ref;
};
//////////////////////////////////////////////////////////////////////////////
/**
 * \class	auto_obj
 */
template<typename T>
class auto_obj
{
public:
	auto_obj()
		: h_(0)
	{
	}
	explicit auto_obj(T* h)
		: h_(h)
	{
	}
#if _MSC_VER > 1200
	auto_obj(auto_obj<T>& other)
		: h_(other.release())
	{
	}
#else
	auto_obj(auto_obj<T>& other)
		: h_(other.release())
	{
	}
#endif
	auto_obj(auto_obj_ref<T>& other)
		: h_(other._ref.release())
	{
	}
#if _MSC_VER > 1200
	template<typename Other>
	auto_obj(auto_obj<Other>& other)
		: h_(other.release())
	{
	}
#endif
	template<typename Other>
	operator auto_obj_ref<Other> ()
	{
		return auto_obj_ref<Other>(*this);
	}
	template<typename Other>
	operator auto_obj<Other> ()
	{
		return auto_obj<Other>(*this);
	}
	auto_obj& operator = (auto_obj& other)
	{
		this->reset(other.release());
		return *this;
	}
	~auto_obj()
	{
		if(h_)
			h_->sysCloseObject();
	}
	T* release()
	{
		T* tmp = h_;
		h_ = 0;
		return tmp;
	}
	T* get() const 
	{ 
		return h_; 
	}
	hOBJECT obj()
	{
		return reinterpret_cast<hOBJECT>(h_);
	}
	T* operator-> () const
	{
		return get();
	}
	T& operator* () const
	{
		return *get();
	}
	void reset(T* h)
	{
		if(h != h_ && h_ != 0)
			h_->sysCloseObject();
		h_ = h;
	}	
private:
	T*	h_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace prague
///////////////////////////////////////////////////////////////////////////////
