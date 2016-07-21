#ifndef __KLSTDUTILS_H__
#define __KLSTDUTILS_H__

/*KLCSAK_PUBLIC_HEADER*/

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

namespace KLSTD
{
    /*! 
        Wrapper object for AKWSTR pointer. Object has associated AKWSTR 
        pointer.
    */
    class klwstr_t
    {
    public:
        klwstr_t();

        /*!
            Constructs object and initializes it with string. 
            The string s will be copied.
        */
        klwstr_t(const klwstr_t& s);

        /*!
            Constructs object and initializes it with string. 
            The string s will be copied.
        */
        klwstr_t(const wchar_t* s);

        /*!
            Constructs object and initializes it with string. 
            If fCopy is true, string will be copied otherwise this call
            is equivalent to call Attach() after default constructor.
        */
        klwstr_t(AKWSTR wstr, bool fCopy);

        /*! Destroys objects and frees associated memory.
        */
        ~klwstr_t();

        /*! Attaches AKWSTR pointer to the object.
        */
        void attach(AKWSTR wstr);

        /*! Detaches from the object AKWSTR pointer ant returns it.
            Returned pointer must be freed by KLSTD_FreeWSTR. This method
            can be used in function to fill out-parameter on callee side. 

            Example:

                void SomeFunc(int IN x, AKWSTR& OUT result)
                {
                    klwstr_t strResult;
                    strResult = L"Foo";

                    // we must call Detach() to prevent freeing associated 
                    //AKWSTR pointer after function returns.                    
                    result = strResult.Detach();
                };
        */
        AKWSTR detach();

	    /*! Assignment operators
            Old object contents will be freed and the string s will be copied.
        */
	    klwstr_t& operator=(const klwstr_t& s);

	    /*! Assignment operators
            Old object contents will be freed and the string s will be copied.
        */
	    klwstr_t& operator=(const wchar_t* s);

	/*! Operators
    */
	    klwstr_t& operator+=(const klwstr_t& s);
        klwstr_t& operator+=(const wchar_t* s);
        klwstr_t operator+(const klwstr_t& s) const;
        klwstr_t operator+(const wchar_t* s) const;

        /*! Returns character by index. 
            Throws exception if index > string length.
        */
        wchar_t operator[](size_t index) const;

        /*! methods outref and outaddr clear object and return reference 
            (pointer) to the associated poitner.
            These method can be used while calling function to put
            out-parameter on caller side. 
            Example:

                // some function prototype
                void SomeFunc(int IN x, AKWSTR& OUT result);

                //...
                int nPar1 = 0;
                //...
                klwstr_t str;
                SomeFunc(nPar1, str.outref());
                //now str is filled by SomeFunc
                //...
        */
        AKWSTR& outref();
        AKWSTR* outaddr();

	//! Friend operators
	    friend klwstr_t operator+(const wchar_t* s1, const klwstr_t& s2);

    //! Extractors. 
        //! Returns associated pointer AKWSTR. String is not copied.
	    operator const wchar_t*() const;

	    //! Comparison operators
	    bool operator!() const;
	    bool operator==(const klwstr_t& str) const;
	    bool operator!=(const klwstr_t& str) const;
	    bool operator<(const klwstr_t& str) const;
	    bool operator>(const klwstr_t& str) const;
	    bool operator<=(const klwstr_t& str) const;
	    bool operator>=(const klwstr_t& str) const;

    //! Low-level helper functions
        /*! Copies associated string and returs the copy. Returned string
            must be freed by KLSTD_FreeWSTR.
        */
	    AKWSTR copy() const;

        /*! Returnes length of associated string in characters. 
            Equivalent to size().
        */
	    size_t length() const;

    //! basic_string compatibility methods
        /*! Returns associated pointer. 
            Equivalent to operator const wchar_t*().
        */
        const wchar_t* c_str() const;

        //! Assigns string to the object. String will be copied.
        void assign(const wchar_t* szw);

        //! Assigns string to the object. String will be copied.
        void assign(const wchar_t* szw, size_t size);

        //! Assigns string to the object. String will be copied.
        void assign(const klwstr_t& wstr);

        /*! 
            Returnes length of associated string in characters.
            Equivalent to length()
        */
        size_t size() const;

        //! returns true if associated string is empty
        bool empty() const;

        /*! compares objects
            Returned value  Relationship of this to wstr
            < 0             this less than wstr 
            0               this identical to wstr
            > 0             this greater than wstr

        */
        int compare(const klwstr_t& wstr) const;

        /*! clears object and frees associated memory. Pointers returned 
            by method c_str() and operator const wchar_t*() become invalid.
        */
        void clear()throw();
    protected:
        void do_append(const wchar_t* szw, size_t size);
    protected:
        KLSTD::AKWSTR   m_wstr;
    };

    //! wrapper for AKWSTRARR object
    class klwstrarr_t
    {
    public:
        klwstrarr_t();

        klwstrarr_t(size_t n);

        ~klwstrarr_t();

        /*!
          \brief creates array of n NULL pointers. Pointers may be replaced 
            with setat method.

          \param	n size
        */
        void create(size_t n);

        /*!
          \brief	Returns value by index.

          \param	size_t index
          \return	KLSTD::AKWSTR value
        */
        const KLSTD::AKWSTR operator[] (size_t index) const;
        KLSTD::AKWSTR operator[] (size_t index);
        KLSTD::AKWSTR getat(size_t index);

        /*!
          \brief	Replaces value by index and frees previous value

          \param	index   value index
          \param	szw     new value
        */
        void setat(size_t index, KLSTD::AKWSTR szw);

        /*!
          \brief	Frees array and all values
        */
        void clear();

        /*!
          \brief	Returns number of values in array

          \return	number of values
        */
        size_t size() const;

        /*!
          \brief	Attaches AKWSTRARR to object
        */
        void attach(const AKWSTRARR& data);

        /*!
          \brief	Detaches AKWSTRARR from object
        */
        AKWSTRARR detach();

        /*!
          \brief	clears object and returns refernece for out parameter
        */
        AKWSTRARR& outref();

        /*!
          \brief	clears object and returns pointer for out parameter
        */
        AKWSTRARR* outaddr();

        /*!
          \brief	returns reference for in parameter
        */
        const AKWSTRARR& inref() const;
    protected:
        AKWSTRARR   m_data;
    };


    inline klwstrarr_t::klwstrarr_t()
    {
        ;
    };

    inline klwstrarr_t::klwstrarr_t(size_t n)
    {
        KLSTD_AllocArrayWSTR(n, m_data);
    };

    inline klwstrarr_t::~klwstrarr_t()
    {
        clear();
    };

    inline void klwstrarr_t::create(size_t n)
    {
        clear();
        KLSTD_AllocArrayWSTR(n, m_data);
    };

    inline const KLSTD::AKWSTR klwstrarr_t::operator[] (size_t index) const
    {
        if(index >= size())
        {
            KLSTD_THROW(KLSTD::STDE_BOUND);
        };
        return m_data.m_pwstr[index];
    };

    inline KLSTD::AKWSTR klwstrarr_t::operator[] (size_t index)
    {
        if(index >= size())
        {
            KLSTD_THROW(KLSTD::STDE_BOUND);
        };
        return m_data.m_pwstr[index];
    };

    inline KLSTD::AKWSTR klwstrarr_t::getat(size_t index)
    {
        if(index >= size())
        {
            KLSTD_THROW(KLSTD::STDE_BOUND);
        };
        return m_data.m_pwstr[index];
    };

    inline void klwstrarr_t::setat(size_t index, KLSTD::AKWSTR szw)
    {
        if(index >= size())
        {
            KLSTD_THROW(KLSTD::STDE_BOUND);
        };
        KLSTD::AKWSTR szwOld = m_data.m_pwstr[index];
        m_data.m_pwstr[index] = szw;
        KLSTD_FreeWSTR(szwOld);
    };

    inline void klwstrarr_t::clear()
    {
        KLSTD_FreeArrayWSTR(m_data);
    };

    inline size_t klwstrarr_t::size() const
    {
        return m_data.m_nwstr;
    };

    inline void klwstrarr_t::attach(const AKWSTRARR& data)
    {
        clear();
        m_data = data;
    };

    inline AKWSTRARR klwstrarr_t::detach()
    {
        AKWSTRARR data = m_data;
        m_data.m_pwstr = NULL;
        m_data.m_nwstr = 0;
        return data;
    };

    //! clears object and returns refernece for out parameter
    inline AKWSTRARR& klwstrarr_t::outref()
    {
        clear();
        return m_data;
    };

    //! clears object and returns pointer for out parameter
    inline AKWSTRARR* klwstrarr_t::outaddr()
    {
        clear();
        return &m_data;
    };

    inline const AKWSTRARR& klwstrarr_t::inref() const
    {
        return m_data;
    };

    inline klwstr_t::klwstr_t()
        :   m_wstr(NULL)
    {
        ;
    };

    inline klwstr_t::klwstr_t(const klwstr_t& s)
        :   m_wstr(NULL)
    {
        assign(s);
    };

    inline klwstr_t::klwstr_t(const wchar_t* s)
        :   m_wstr(NULL)
    {
        assign(s);
    };

    inline klwstr_t::klwstr_t(AKWSTR wstr, bool fCopy)
        :   m_wstr(NULL)
    {
        if(fCopy)
            assign(wstr);
        else
            attach(wstr);
    };

    inline klwstr_t::~klwstr_t()
    {
        clear();
    };

    inline void klwstr_t::attach(AKWSTR wstr)
    {
        clear();
        m_wstr = wstr;
    };

    inline AKWSTR klwstr_t::detach()
    {
        AKWSTR wstr = m_wstr;
        m_wstr = NULL;
        return wstr;
    };

	inline klwstr_t& klwstr_t::operator=(const klwstr_t& s)
    {
        assign(s);
        return *this;
    };

	inline klwstr_t& klwstr_t::operator=(const wchar_t* s)
    {
        assign(s);
        return *this;
    };

	inline klwstr_t& klwstr_t::operator+=(const klwstr_t& s)
    {
        do_append(s.c_str(), s.size());
        return *this;
    };

    inline klwstr_t& klwstr_t::operator+=(const wchar_t* s)
    {
        do_append(s, wcslen(s));
        return *this;
    };

    inline klwstr_t klwstr_t::operator+(const klwstr_t& s) const
    {
        klwstr_t result = *this;
        result += s;
        return result;
    };

    inline klwstr_t klwstr_t::operator+(const wchar_t* s) const
    {
        klwstr_t result = *this;
        result += s;
        return result;
    };

    inline wchar_t klwstr_t::operator[](size_t index) const
    {
#if _DEBUG
        KLSTD_ASSERT(index < size());
#endif
        return m_wstr
                ?   m_wstr[index]
                :   wchar_t();
    };

    inline AKWSTR& klwstr_t::outref()
    {
        clear();
        return m_wstr;
    };

    inline AKWSTR* klwstr_t::outaddr()
    {
        clear();
        return &m_wstr;
    };

	inline klwstr_t operator+(const wchar_t* s1, const klwstr_t& s2)
    {
        klwstr_t result = s1;
        result.do_append(s2.c_str(), s2.size());
        return result;
    };

	inline klwstr_t::operator const wchar_t*() const
    {
        return c_str();
    };

	inline bool klwstr_t::operator!() const
    {
        return empty();
    };

	inline bool klwstr_t::operator==(const klwstr_t& str) const
    {
        return compare(str) == 0;
    };

	inline bool klwstr_t::operator!=(const klwstr_t& str) const
    {
        return compare(str) != 0;
    };

	inline bool klwstr_t::operator<(const klwstr_t& str) const
    {
        return compare(str) < 0;
    };

	inline bool klwstr_t::operator>(const klwstr_t& str) const
    {
        return compare(str) > 0;
    };

	inline bool klwstr_t::operator<=(const klwstr_t& str) const
    {
        return compare(str) <= 0;
    };

	inline bool klwstr_t::operator>=(const klwstr_t& str) const
    {
        return compare(str) >= 0;
    };

    inline AKWSTR klwstr_t::copy() const
    {
        return KLSTD_AllocWSTRLen(c_str(), size());
    };

	inline size_t klwstr_t::length() const
    {
        return size();
    };

    inline const wchar_t* klwstr_t::c_str() const
    {
        return m_wstr ? m_wstr : L"";
    };

    inline void klwstr_t::assign(const wchar_t* szw)
    {
        clear();
        m_wstr = KLSTD_AllocWSTR(szw);
    };

    inline void klwstr_t::assign(const wchar_t* szw, size_t size)
    {
        clear();
        m_wstr = KLSTD_AllocWSTRLen(szw, size);
    };

    inline void klwstr_t::assign(const klwstr_t& wstr)
    {
        clear();
        m_wstr = KLSTD_AllocWSTRLen(wstr.c_str(), wstr.size());
    };

    inline size_t klwstr_t::size() const
    {
        return m_wstr ? KLSTD_GetWSTRLen(m_wstr) : 0;
    };

    inline bool klwstr_t::empty() const
    {
        return (m_wstr && m_wstr[0]) ? false : true;
    };

    inline int klwstr_t::compare(const klwstr_t& wstr) const
    {
        return KLSTD_CompareWSTR(m_wstr, wstr.m_wstr);
    };

    inline void klwstr_t::clear()throw()
    {
        if(m_wstr)
        {
            KLSTD_FreeWSTR(m_wstr);
            m_wstr = NULL;
        };
    };

    inline void klwstr_t::do_append(const wchar_t* szw, size_t size)
    {
        size_t n1 = length();
		AKWSTR w;
		w = ::KLSTD_AllocWSTRLen(NULL, n1 + size);
		memcpy(w, m_wstr, n1*sizeof(wchar_t));
		memcpy(w + n1, szw, size*sizeof(wchar_t));
		w[n1+size] = wchar_t();
		KLSTD_FreeWSTR(m_wstr);
		m_wstr = w;
    };
};


#endif //__KLSTDUTILS_H__
