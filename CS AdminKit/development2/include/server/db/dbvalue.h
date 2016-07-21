#ifndef __KL_DBVALUE_H__
#define __KL_DBVALUE_H__

namespace KLDB
{
    enum dbvalue_type_t
    {
        dbvtEmpty,
        dbvtTinyInt,
        dbvtSmallInt,
        dbvtInteger,
        dbvtBigInt,
        dbvtUnsignedTinyInt,
        dbvtUnsignedSmallInt,
        dbvtUnsignedInt,
        dbvtUnsignedBigInt,
        dbvtSingle,
        dbvtDouble,
        dbvtBoolean,
        dbvtDateTime,
        dbvtChar,
        dbvtVarChar,
        dbvtWChar,
        dbvtVarWChar,
        dbvtBinary,
        dbvtVarBinary
    };

    class DbValue;

    typedef KLSTD::CAutoPtr<DbValue> DbValuePtr;

    /*!
        DbValue is an abstraction for values of various type.
        Implementation isn't thread-safe so access to variable 
        must be serialized.
    */
    class KLSTD_NOVTABLE DbValue : public KLSTD::KLBaseQI
    {
    public:
    //constructors
        /*!
          \brief	CreateEmpty Initializes to empty value
        */
        virtual void CreateEmpty() = 0;


        /*!
          \brief	methods Create initialize DbValue to specified value
                    type of parameter value and parameter dbvalue_type_t
                    must be compatible

                    Compatibility is checked according to following table:
                    --------------------------------+---------------------
                    C++ type                        |dbvalue_type_t
                    --------------------------------+---------------------
                    int, unsigned int, AVP_longlong,|dbvtTinyInt,dbvtSmallInt,
                    AVP_qword, bool                 |dbvtInteger, dbvtBigInt,
                                                    |dbvtUnsignedTinyInt, 
                                                    |dbvtUnsignedSmallInt, 
                                                    |dbvtUnsignedInt, 
                                                    |dbvtUnsignedBigInt, 
                                                    |dbvtBoolean
                    --------------------------------+---------------------
                    double, float                   |dbvtDouble, dbvtSingle
                    --------------------------------+---------------------
                    KLSTD::precise_time_t, time_t   |dbvtDateTime
                    --------------------------------+---------------------
                    std::string, std::wstring       |dbvtVarWChar, dbvtVarChar, 
                                                    |dbvtWChar, dbvtChar
                    --------------------------------+---------------------
                    KLSTD::MemoryChunkPtr           |dbvtBinary , dbvtVarBinary
                    --------------------------------+---------------------

          \param	value IN    value to store
          \param	nType IN    type
          \exception    DB_ERR_INVALID_TYPECAST type of value and 
                        specified nType are not compatible
        */        
        virtual void Create(int value, dbvalue_type_t nType = dbvtInteger) = 0;
        virtual void Create(unsigned int value, dbvalue_type_t nType = dbvtUnsignedInt) = 0;
        virtual void Create(AVP_longlong value, dbvalue_type_t nType = dbvtBigInt) = 0;
        virtual void Create(AVP_qword value, dbvalue_type_t nType = dbvtUnsignedBigInt) = 0;
        virtual void Create(double value, dbvalue_type_t nType = dbvtDouble) = 0;
        virtual void Create(float value, dbvalue_type_t nType = dbvtSingle) = 0;
        virtual void Create(bool value, dbvalue_type_t nType = dbvtBoolean) = 0;
        virtual void Create(KLSTD::precise_time_t value, dbvalue_type_t nType = dbvtDateTime) = 0;
        virtual void Create(const std::string& value, dbvalue_type_t nType = dbvtVarChar) = 0;
        virtual void Create(const std::wstring& value, dbvalue_type_t nType = dbvtVarWChar) = 0;
        virtual void Create(const char* value, dbvalue_type_t nType = dbvtVarChar) = 0;
        virtual void Create(const wchar_t* value, dbvalue_type_t nType = dbvtVarWChar) = 0;
        virtual void Create(const void* value, size_t size, dbvalue_type_t nType = dbvtVarBinary) = 0;
        virtual void Create(KLSTD::MemoryChunkPtr pData, dbvalue_type_t nType = dbvtVarBinary) = 0;
        virtual void Create(DbValue* pX) = 0;
    //state

        /*!
          \brief	Returns type of value
          \return	type of value
        */
        virtual dbvalue_type_t  Type() = 0;
        /*!
          \brief	Dumps value to string. For debug purposes only !

          \return	resulting string
        */
        virtual std::wstring    ToString() = 0;
    //conversion methods

        /*!
          \brief    Converts value to type int
                    Conversion is supported for types
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \return	converted value
          \exception    DB_ERR_INVALID_TYPECAST
        */
        virtual int                     GetInt() = 0;

        /*!
          \brief    Converts value to type unsigned int
                    Conversion is supported for types
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \return	converted value
          \exception    DB_ERR_INVALID_TYPECAST
        */
        virtual unsigned int            GetUnsignedInt() = 0;

        /*!
          \brief    Converts value to type AVP_longlong
                    Conversion is supported for types
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value
        */
        virtual AVP_longlong            GetLongLong() = 0;

        /*!
          \brief    Converts value to type AVP_qword
                    Conversion is supported for types
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value
        */
        virtual AVP_qword               GetUnsignedLongLong() = 0;

        /*!
          \brief    Converts value to type double
                    Conversion is supported for types
                        dbvtSingle,
                        dbvtDouble

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value
        */
        virtual double                  GetDouble() = 0;

        /*!
          \brief    Converts value to type float
                    Conversion is supported for types
                        dbvtSingle,
                        dbvtDouble

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value
        */
        virtual float                   GetFloat() = 0;

        /*!
          \brief    Converts value to type bool
                    Conversion is supported for types
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value
        */
        virtual bool                    GetBool() = 0;

        /*!
          \brief    Converts value to type KLSTD::precise_time_t
                    Conversion is supported for types
                        dbvtEmpty
                        dbvtDateTime

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value. In case of dbvtEmpty returns (-1, 0)
        */
        virtual KLSTD::precise_time_t   GetPreciseTime() = 0;

        /*!
          \brief    Converts value to type time_t
                    Conversion is supported for types
                        dbvtEmpty
                        dbvtDateTime

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value. In case of dbvtEmpty returns -1
        */
        virtual time_t                  GetTime() = 0;

        /*!
          \brief    Converts value to type std::string
                    Conversion is supported for types
                        dbvtEmpty,
                        dbvtChar,
                        dbvtVarChar,
                        dbvtWChar,
                        dbvtVarWChar

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value. In case of dbvtEmpty returns empty string
        */
        virtual std::string             GetString() = 0;

        /*!
          \brief    Converts value to type std::wstring
                    Conversion is supported for types
                        dbvtEmpty,
                        dbvtChar,
                        dbvtVarChar,
                        dbvtWChar,
                        dbvtVarWChar

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value. In case of dbvtEmpty returns empty string
        */
        virtual std::wstring            GetWstring() = 0;

        /*!
          \brief    Converts value to type KLSTD::MemoryChunkPtr
                    Conversion is supported for types
                        dbvtEmpty,
                        dbvtBinary,
                        dbvtVarBinary

          \exception    DB_ERR_INVALID_TYPECAST
          \return	converted value. In case of dbvtEmpty returns NULL pointer.
        */
        virtual KLSTD::MemoryChunkPtr   GetBinary() = 0;

        /*!
          \brief    Converts value to KLPAR::Value
                    Conversion is supported for all types

          \return	converted value.  In case of dbvtEmpty returns NULL pointer.
        */
        virtual KLSTD::CAutoPtr<KLPAR::Value>   GetValue() = 0;

        /*!
          \brief	Creates a copy of value

          \return	Copy
        */
        virtual DbValuePtr Clone() = 0;

        /*!
          \brief	Checks whether value has type dbvtEmpty

          \return	true if type is dbvtEmpty
        */       
        virtual bool IsEmpty() = 0;

        /*!
          \brief	Checks whether value has a numeric type. 
                    Numeric type is one of following
                        dbvtTinyInt,
                        dbvtSmallInt,
                        dbvtInteger,
                        dbvtBigInt,
                        dbvtUnsignedTinyInt,
                        dbvtUnsignedSmallInt,
                        dbvtUnsignedInt,
                        dbvtUnsignedBigInt,
                        dbvtBoolean

          \return	true if type is numeric
        */       
        virtual bool IsNumeric() = 0;

        /*!
          \brief	Checks whether value has a string type. 
                    String type is one of following
                        dbvtChar,
                        dbvtVarChar,
                        dbvtWChar,
                        dbvtVarWChar

          \return	true if type is string
        */
        virtual bool IsString() = 0;

        /*!
          \brief	Checks whether value has a datetime type. 
                    Datetime type is one of following
                        dbvtDateTime

          \return	true if type is datetime
        */        
        virtual bool IsDateTime() = 0;

        /*!
          \brief	Checks whether value has a floating type. 
                    Floating type is one of following
                        dbvtSingle,
                        dbvtDouble

          \return	true if type is floating
        */        
        virtual bool IsFloating() = 0;

        /*!
          \brief	Checks whether value has a binary type. 
                    Binary type is one of following
                        dbvtBinary,
                        dbvtVarBinary

          \return	true if type is binary
        */        
        virtual bool IsBinary() = 0;
    };

    class KLSTD_NOVTABLE DbValueFactory
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual DbValuePtr CreateDbValue() = 0;
    };    
    
    typedef KLSTD::CAutoPtr<DbValueFactory> DbValueFactoryPtr;

    /*!
        Helper class for transparent type conversion.
        Usage
            const std::wstring& wstrHostName = (KLDB::DbVal)pRs->Fields()->Item(0u);
    */
    class DbVal
    {
    public:
        DbVal(DbValuePtr pVal)
            :   m_pVal(pVal)
        {;};
        inline operator int()
        {
            return m_pVal->GetInt();
        };
        inline operator long()
        {
            return m_pVal->GetUnsignedInt();
        };
        inline operator unsigned int()
        {
            return m_pVal->GetUnsignedInt();
        };
        inline operator unsigned long()
        {
            return m_pVal->GetUnsignedInt();
        };
        inline operator AVP_longlong()
        {
            return m_pVal->GetLongLong();
        };
        inline operator AVP_qword()
        {
            return m_pVal->GetUnsignedLongLong();
        };
        inline operator double()
        {
            return m_pVal->GetDouble();
        };           
        inline operator float()
        {
            return m_pVal->GetFloat();
        };
        inline operator bool()
        {
            return m_pVal->GetBool();
        };
        inline operator KLSTD::precise_time_t()
        {
            return m_pVal->GetPreciseTime();
        };
        inline operator std::string()
        {
            return m_pVal->GetString();
        };
        inline operator std::wstring()
        {
            return m_pVal->GetWstring();
        };
        inline operator KLSTD::MemoryChunkPtr()
        {
            return m_pVal->GetBinary();
        };
    protected:
        DbValuePtr  m_pVal;
    };
};

#endif //__KL_DBVALUE_H__
