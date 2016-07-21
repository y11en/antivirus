#ifndef __COMMON_AUTOPTR_H
#define __COMMON_AUTOPTR_H
#include <crtdbg.h>

template <class T>
class _NoAddRefReleaseOnCAutoPtr : public T
{
        protected:
			_NoAddRefReleaseOnCAutoPtr() :
				 T()
				 { }
		private:
			virtual unsigned long AddRef()=0;
			virtual unsigned long Release()=0;
};


template <class T>
class CAutoPtr
{
public:
	typedef T _PtrClass;
	CAutoPtr(){
		p=NULL;
	};

	CAutoPtr(T* lp){
		if ((p = lp) != NULL)
			p->AddRef();
	};

	CAutoPtr(const CAutoPtr<T>& lp){
		if ((p = lp.p) != NULL)
			p->AddRef();
	};

	~CAutoPtr(){
		if (p)
			p->Release();
	};

	void Release(){
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	};

	operator T*() const{
		return (T*)p;
	};

	T& operator*() const{
		_ASSERT(p!=NULL);
		return *p;
	};

	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&()
	{
		_ASSERT(p==NULL);
		return &p;
	};

	_NoAddRefReleaseOnCAutoPtr<T>* operator->() const
	{
		_ASSERT(p!=NULL);
		return (_NoAddRefReleaseOnCAutoPtr<T>*)p;
	};

	T* operator=(T* lp)
	{
		return _AutoPtrAssign(&p, lp);
	};

	T* operator=(const CAutoPtr<T>& lp)
	{
		return _AutoPtrAssign(&p, lp.p);
	};

	bool operator!() const
	{
		return (p == NULL);
	};
	
	bool operator<(T* pT) const
	{
		return p < pT;
	};
	
	bool operator==(T* pT) const
	{
		return p == pT;
	};
	
	void Attach(T* p2)
	{
		if (p)
			p->Release();
		p = p2;
	};
	
	T* Detach()
	{
		T* pt = p;
		p = NULL;
		return pt;
	};
	
	bool CopyTo(T** ppT)
	{
		_ASSERT(ppT != NULL);
		if (ppT == NULL)
			return false;
		*ppT = p;
		if (p)
			p->AddRef();
		return true;
	};
private:
	static T* _AutoPtrAssign(T** pp, T* lp)
	{
		if (lp != NULL)
			lp->AddRef();
		if (*pp)
			(*pp)->Release();
		*pp = lp;
		return lp;
	}
	
	T* p;
};

struct _AUTO_PTR_REF_COUNTER
{
	_AUTO_PTR_REF_COUNTER() : m_c_auto_ptr_Ref(1){}
	volatile unsigned long m_c_auto_ptr_Ref;
};
#define DECLARE_AUTO_PTR	\
	_AUTO_PTR_REF_COUNTER m_ref_counter_imp; \
	virtual unsigned long AddRef(){	return InterlockedIncrement((long*)&m_ref_counter_imp.m_c_auto_ptr_Ref);} \
	virtual unsigned long Release() \
{ \
	long cRef=InterlockedDecrement((long*)&m_ref_counter_imp.m_c_auto_ptr_Ref); \
	if(!cRef) \
	delete this; \
	return cRef; \
}

template <class T> class CCRefCountImpl : public T
{
public:
	DECLARE_AUTO_PTR;
};


#endif //__COMMON_AUTOPTR_H