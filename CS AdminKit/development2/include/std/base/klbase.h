/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file klbase.h
 * \author Andrew Kazachkov
 * \date 2002
 * \brief Reference counters support
 *
 */
 
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLBASE_H_79BC6D63_AD8E_4568_9161_5AD86C9788F8
#define KLBASE_H_79BC6D63_AD8E_4568_9161_5AD86C9788F8

#ifndef KLDEFS_H_AAE8D350_6234_4a8a_A525_56B7D38B5A65
# include "kldefs.h"
#endif

#ifdef N_PLAT_NLM 		// Novell Netware
//# include <wchar_t.h>
//# include <nwmalloc.h>
//# include <unistd.h>
# include <nwtime.h>
# include <cwchar>
#include <nwconio.h>
# define UNICODE
#endif

#ifdef __unix
#include <std/conv/wcharcrt.h>
#endif

/*KLCSAK_BEGIN_PRIVATE*/
#include <string>
#include <vector>
#include <list>
/*KLCSAK_END_PRIVATE*/

KLCSC_DECL void KLSTD_Initialize();
KLCSC_DECL void KLSTD_Deinitialize();

KLCSC_DECL void KLPAR_Initialize();
KLCSC_DECL void KLPAR_Deinitialize();

KLCSC_DECL long KLSTD_FASTCALL KLSTD_InterlockedIncrement(long volatile* x);
KLCSC_DECL long KLSTD_FASTCALL KLSTD_InterlockedDecrement(long volatile* x);


namespace KLSTD{

//#ifndef __unix
#ifdef _WIN32
const wchar_t SLASH[]=L"\\";
#else
	#ifdef N_PLAT_NLM
		const wchar_t SLASH[]=L"\\";
	#else
		const wchar_t SLASH[]=L"/";
	#endif
#endif
	
	const wchar_t szwModule[]=L"KLSTD";


	const size_t c_nMaxLocallyUniqStringLen=100;

    //!string
    typedef wchar_t* AKWSTR;

    //!array of strings
    struct AKWSTRARR
    {
        AKWSTRARR()
        :   m_pwstr(NULL)
        ,   m_nwstr(0)
        {;};

        KLSTD::AKWSTR*  m_pwstr;
        size_t          m_nwstr;
    };

	/*!
		\brief Базовый класс для всех классов, поддерживающих счётчики ссылок

	*/
	
	class KLSTD_NOVTABLE KLBase{
	public:
		virtual unsigned long AddRef()=0;
		virtual unsigned long Release()=0;
	};

	/*!
		\brief Базовый класс для всех классов, поддерживающих счётчики ссылок
        и множественные интерфейсы. 

	*/

    //!\brief идентификатор интерфейса
    typedef const char* KLSTD_PIID;

    //!\brief Обёртка для идентификатора интерфейса
    class KLSTD_IID
    {
    public:
        inline KLSTD_IID(KLSTD_PIID iid):m_iid(iid){;};
        inline KLSTD_IID(const KLSTD_IID& iid):m_iid(iid.m_iid){;};
        inline operator KLSTD_PIID() const{return m_iid;};
        inline bool operator == (KLSTD_IID x) const {return compare(x) == 0;};
        inline bool operator < (KLSTD_IID x) const {return compare(x) < 0;};
        inline int compare(KLSTD_IID x) const {return strcmp(m_iid, x);}
    protected:        
        KLSTD_PIID m_iid;
    };
    
    /*!
        дефайн, возвращающий идентификатор идентификатор интерфейса, 
        параметр _interface имеет вид: <namespace>::<name>, например,
        KLSTD_IIDOF(KLSTD::Foo)
    */
    #define KLSTD_PIIDOF(_interface) #_interface
    #define KLSTD_IIDOF(_interface) KLSTD::KLSTD_IID(KLSTD_PIIDOF(_interface))
    
    class KLSTD_NOVTABLE KLBaseQI : public KLBase
    {
	public:
        virtual bool QueryInterface(KLSTD_IID iid, void** ppObject) = 0;
	};

    const KLSTD_IID KLBaseQI_IID=KLSTD_IIDOF(KLSTD::KLBaseQI);


	/*!
		\brief Для счётчиков ссылок используется следующее соглашение:
			Если функция возвращает указатель в out-параметре, 
			то она же его перед этим должна инкрементировать,
			если указатель возвращается посредством return,
			то инкрементирует его вызывающий код.
	*/

	/*!
		\brief Предоставяет нитебезопасную имплементацию KLBase

	*/
	template<class Base, bool bMultiThread=true>
	class KLBaseImpl: public Base{
		volatile unsigned long m_cRef;
	public:
		KLBaseImpl(): Base(), m_cRef(1){;}
		virtual ~KLBaseImpl()
		{
			//KLSTD_ASSERT(m_cRef==0);
		};
		
		virtual unsigned long AddRef()
	{
			return bMultiThread ? KLSTD_InterlockedIncrement((long*)&m_cRef): ++m_cRef;
		};

		virtual unsigned long Release()
		{
			long cRef=bMultiThread ? KLSTD_InterlockedDecrement((long*)&m_cRef) : --m_cRef;
			if(!cRef)
				delete this;
			return cRef;
		};
	};

	/*!
		\brief Implementation of class with reference counter.

	*/
	template<class Base, bool bMultiThread=true>
	class RcClassImpl: public Base{
		volatile unsigned long m_cRef;
	public:
		RcClassImpl():Base(), m_cRef(1){;}
		virtual ~RcClassImpl()
		{			
			// KLSTD_ASSERT(m_cRef==0);
		};		
		virtual unsigned long AddRef()
		{
			return bMultiThread ? KLSTD_InterlockedIncrement((long*)&m_cRef) : ++m_cRef;
		};

		virtual unsigned long Release()
		{
			long cRef=bMultiThread ? KLSTD_InterlockedDecrement((long*)&m_cRef) : --m_cRef;
			if(!cRef)
				delete this;
			return cRef;
		};
	};

	class KLSTD_NOVTABLE MemoryChunk: public KLBase
	{
	public:
		virtual void*	GetDataPtr()=0;
		virtual size_t	GetDataSize() const =0;
	};

	/*!
		\brief Для внутреннего использования

	*/
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

	/*!
		\brief Автоуказатель для классов, не унаследованных от KLBase
	*/
	template<class T> 
	class CPointer 
	{
	public:
		CPointer(T* pointer) : P(pointer) {}
		CPointer() : P(0) {}
		virtual ~CPointer() { if (P) delete P; }

		CPointer<T>& operator =(T* src) 
		{
			if (P) delete P;
			P = src; 
			return *this;
		}
		T* operator->() {return P;}  // Could throw exception if P==0
		T&   operator  *() {return *P;}
			 operator T*() {return P;}
			 operator const T*() const {return P;}
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(P == NULL);
			return &P;
		};

		int  operator  !() const {return P == 0;}
		T*   Relinquish() {T* p = P; P = 0; return p;}

	protected:
		T* P;

	private:
		void* operator new(size_t) { return 0;}  // prohibit use of new
	};

	/*!
		\brief Автоуказатель для памяти распределенной с помощью malloc
	*/
	
	template<class T> 
	class CArrayPointer
	{
	public:
		CArrayPointer(T* pointer) : P(pointer) {}
		CArrayPointer() : P(0) {}
		virtual ~CArrayPointer() { if (P) free(P); }

		CArrayPointer<T>& operator =(T* src) 
		{
			if (P) free(P);
			P = src; 
			return *this;
		}		
		T&   operator  *() {return *P;}
			 operator T*() {return P;}
			 operator const T*() const {return P;}
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(P == NULL);
			return &P;
		};

		int  operator  !() const {return P == 0;}
		T*   Relinquish() {T* p = P; P = 0; return p;}

	protected:
		T* P;

	private:
		void* operator new(size_t) { return 0;}  // prohibit use of new
	};
	
	/*!
		\brief Автоуказатель для памяти распределенной с помощью new[]
	*/
	
	template<class T> 
	class CArrayPointer2
	{
	public:
		CArrayPointer2(T* pointer) : P(pointer) {}
		CArrayPointer2() : P(0) {}
		virtual ~CArrayPointer2() { if (P) delete[] P; }

		CArrayPointer2<T>& operator =(T* src) 
		{
			if (P) delete[] P;
			P = src; 
			return *this;
		}		
		T&   operator  *() {return *P;}
			 operator T*() {return P;}
			 operator const T*() const {return P;}
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(P == NULL);
			return &P;
		};

		T* get() { return P; }

		int  operator  !() const {return P == 0;}
		T*   Relinquish() {T* p = P; P = 0; return p;}

	protected:
		T* P;

	private:
		void* operator new(size_t) { return 0;}  // prohibit use of new
	};
	
	/*!
		\brief Автоуказатель для классов, унаследованный от KLBase

	*/
/*
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
			KLBase* pTemp = p;
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
			KLSTD_ASSERT(p!=NULL);
			return *p;
		};

		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(p==NULL);
			return &p;
		};

		_NoAddRefReleaseOnCAutoPtr<T>* operator->() const
		{
			KLSTD_ASSERT(p!=NULL);
			return (_NoAddRefReleaseOnCAutoPtr<T>*)p;
		};

		T* operator=(T* lp)
		{
			return (T*)_AutoPtrAssign((KLBase**)&p, lp);
		};

		T* operator=(const CAutoPtr<T>& lp)
		{
			return (T*)_AutoPtrAssign((KLBase**)&p, lp.p);
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
			KLSTD_ASSERT(ppT != NULL);
			if (ppT == NULL)
				return false;
			*ppT = p;
			if (p)
				p->AddRef();
			return true;
		};
	private:
		static KLBase* _AutoPtrAssign(KLBase** pp, KLBase* lp)
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
*/

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
			KLSTD_ASSERT(p!=NULL);
			return *p;
		};

		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(p==NULL);
			return &p;
		};

		_NoAddRefReleaseOnCAutoPtr<T>* operator->() const
		{
			KLSTD_ASSERT(p!=NULL);
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
			KLSTD_ASSERT(ppT != NULL);
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

	/*!
		\brief Автоуказатель для классов с множественным наследованием, унаследованный от KLBase

	*/
	/*
	template <class T>
	class CAutoMIPtr
	{
	public:
		typedef T _PtrClass;
		CAutoMIPtr(){
			p=NULL;
		};

		CAutoMIPtr(T* lp){
			if ((p = lp) != NULL)
				p->AddRef();
		};

		CAutoMIPtr(const CAutoMIPtr<T>& lp){
			if ((p = lp.p) != NULL)
				p->AddRef();
		};

		~CAutoMIPtr(){
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
			KLSTD_ASSERT(p!=NULL);
			return *p;
		};

		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T** operator&()
		{
			KLSTD_ASSERT(p==NULL);
			return &p;
		};

		_NoAddRefReleaseOnCAutoPtr<T>* operator->() const
		{
			KLSTD_ASSERT(p!=NULL);
			return (_NoAddRefReleaseOnCAutoPtr<T>*)p;
		};

		T* operator=(T* lp)
		{
			return _CAutoMIPtrAssign(&p, lp);
		};

		T* operator=(const CAutoMIPtr<T>& lp)
		{
			return _CAutoMIPtrAssign(&p, lp.p);
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
			KLSTD_ASSERT(ppT != NULL);
			if (ppT == NULL)
				return false;
			*ppT = p;
			if (p)
				p->AddRef();
			return true;
		};
	private:
		static T* _CAutoMIPtrAssign(T** pp, T* lp)
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
*/

	template <class T>
	class KLAdapt
	{
	public:
		KLAdapt()
		{
		}
		KLAdapt(const T& rSrc)
		{
			m_T = rSrc;
		}

		KLAdapt(const KLAdapt& rSrCA)
		{
			m_T = rSrCA.m_T;
		}

		KLAdapt& operator=(const T& rSrc)
		{
			m_T = rSrc;
			return *this;
		}
		bool operator<(const T& rSrc) const
		{
			return m_T < rSrc;
		}
		bool operator==(const T& rSrc) const
		{
			return m_T == rSrc;
		}
		operator T&()
		{
			return m_T;
		}

		operator const T&() const
		{
			return m_T;
		}

		T m_T;
	};


    typedef KLSTD::CAutoPtr<MemoryChunk> MemoryChunkPtr;

};

//! Allocates a string initialized with szw
KLCSC_DECL KLSTD::AKWSTR KLSTD_AllocWSTR(const wchar_t* szw);

//! Allocates a string of length nLen initialized with szw. If szw is NULL string is unitialized
KLCSC_DECL KLSTD::AKWSTR KLSTD_AllocWSTRLen(const wchar_t* szw, size_t nLen);

//! Returns length of string wstr. If wstr is NULL returns 0.
KLCSC_DECL size_t KLSTD_GetWSTRLen(const KLSTD::AKWSTR wstr);

//! Deallocated string wstr. If wstr is NULL does nothing.
KLCSC_DECL void KLSTD_FreeWSTR(KLSTD::AKWSTR wstr);

//! compares two strings 
KLCSC_DECL int KLSTD_CompareWSTR(const KLSTD::AKWSTR wstr1, const KLSTD::AKWSTR wstr2);

//! allocated array of styrings
KLCSC_DECL void KLSTD_AllocArrayWSTR(size_t size, KLSTD::AKWSTRARR& arr);

//! deinitializes array of strings
KLCSC_DECL void KLSTD_FreeArrayWSTR(KLSTD::AKWSTRARR& arr);



/*!
  \brief	Выделяет блок памяти

  \param	IN nSize требуемый размер блока памяти
  \param	OUT указатель переменную, в которую будет записан указатель
			на интерфейс KLSTD::MemoryChunk
  \except	STDE_NOMEMORY недостаточно памяти
*/
KLCSC_DECL void KLSTD_AllocMemoryChunk(size_t nSize, KLSTD::MemoryChunk** ppChunk);

/*KLCSAK_BEGIN_PRIVATE*/

/*!
  \brief	Создаёт локально уникальную строку, длиной не более, чем 
			KLSTD::c_nMaxLocallyUniqStringLen.

  \return	Локально уникальная строка
*/
KLCSC_DECL std::wstring KLSTD_CreateLocallyUniqueString();

/*!
  \brief	Создаёт глобально уникальную строку совместимую с именами файлов,
			НЕ CASE-SENSITIVE

  \return	уникальная строка
*/
KLCSC_DECL std::wstring KLSTD_CreateGUIDString();

/*KLCSAK_END_PRIVATE*/

/*!
  \brief	Creates globally unique string suitable both for case sensetive 
            and case insensetive comparison 

  \param	pwstrResult [out] resulting string 
*/
KLCSC_DECL void KLSTD_CreateGloballyUniqueString(
                                        KLSTD::AKWSTR* pwstrResult);


/*!
  \brief Генерация случайного числа в указанном диапазоне
*/
KLCSC_DECL int KLSTD_Random(int nMin, int nMax);


/*KLCSAK_BEGIN_PRIVATE*/
/*!
  \brief	KLSTD_SplitString - выделяет из строки wstr элементы, разделенные строкой wstrDelim,
        и заполняет ими вектор vectValues, в том же порядке, в каком они встречались в исходной строке.
        При этом wstrDelim не добавляется в качестве элемента, а пробелы равнозначны с другими символами, 
        т.е. пробелы могут быть как частью элементов, так и частью wstrDelim.
        Если wstrDelim отсутсвует, то возвращается вся строка wstr. Если после финального wstrDelim
        ничего нет, то в конце списка будет пустая строка. Если два wstrDelim следуют
        друг за другом, то в соответсующем месте будет пустая строка.
        В начале работы функция очищает vectValues.

  \param	 const std::wstring & wstr - [in] исходная строка
  \param	const std::wstring & wstrDelim - [in] разделитель
  \param	std::vector<std::wstring> & vectValues - [out] результат
*/
KLCSC_DECL void KLSTD_SplitString( const std::wstring & wstr,
                        const std::wstring & wstrDelim,
                        std::vector<std::wstring> & vectValues );


/*!
  \brief	KLSTD_TrimString - убирает начальные и финальные элементы из строки.

  \param	std::wstring & wstr - [in/out] строка
  \return	std::wstring & ссылка на wstr
*/
KLCSC_DECL std::wstring & KLSTD_TrimString( std::wstring & wstr );

KLCSC_DECL void KLSTD_GetDefaultProductAndVersion(
                                 std::wstring& wstrProduct,
                                 std::wstring& wstrVersion);

KLCSC_DECL void KLSTD_SetDefaultProductAndVersion(
                                            const std::wstring& wstrProduct,
                                            const std::wstring& wstrVersion);


/*!
  \brief	Sets process shutdown flag. 
*/
KLCSC_DECL void KLSTD_SetShutdownFlag();

/*!
  \brief	Returns process shutdown flag. 
*/
KLCSC_DECL bool KLSTD_GetShutdownFlag();

/*!
  \brief	Clears process shutdown flag. 
*/
KLCSC_DECL void KLSTD_ClearShutdownFlag();

/*KLCSAK_END_PRIVATE*/


/*KLCSAK_BEGIN_PRIVATE*/
#ifdef _DEBUG

namespace KLSTD {
	class __CKLStdClassDbgInfo__ {
	public:
		std::wstring m_sClassName;
		void* m_pClassPointer;

		__CKLStdClassDbgInfo__(const wchar_t* pwcClassName, void* pClassPointer);
	};
};

	typedef std::list<KLSTD::__CKLStdClassDbgInfo__> KLSTD_CLASS_DBG_INFO_LIST_TYPE;
	
	KLCSC_DECL KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator KLSTD_CLASS_DBG_INFO_RegisterClass(const wchar_t* pwcClassName, void* pClassPointer);
	KLCSC_DECL void KLSTD_CLASS_DBG_INFO_UnregisterClass(KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator itGlobalListPosition);

	#define KLSTD_CLASS_DBG_INFO(className) \
		class __CKLStdClassDbgInfoProvider__{ \
		public: \
			__CKLStdClassDbgInfoProvider__(){ \
				KLSTD_USES_CONVERSION; \
				m_itGlobalListPosition = KLSTD_CLASS_DBG_INFO_RegisterClass(KLSTD_A2W(#className), (void*)((unsigned char*)this - offsetof(className, __m_KLStdClassDbgInfoProvider__))); \
			} \
			~__CKLStdClassDbgInfoProvider__(){ \
				KLSTD_CLASS_DBG_INFO_UnregisterClass(m_itGlobalListPosition); \
			} \
		private: \
			KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator m_itGlobalListPosition; \
		}; \
		__CKLStdClassDbgInfoProvider__ __m_KLStdClassDbgInfoProvider__; \
		friend class __CKLStdClassDbgInfoProvider__;
#else
	#define KLSTD_CLASS_DBG_INFO(className)
#endif
/*KLCSAK_END_PRIVATE*/

#endif // KLBASE_H_79BC6D63_AD8E_4568_9161_5AD86C9788F8

// Local Variables:
// mode: C++
// End:
