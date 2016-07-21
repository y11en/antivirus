/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file helpers.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Вспомогательные функции модуля PARAMS.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef _HELPERS_H_11102C22_1458_4de9_8DD4_7A08981B188A
#define _HELPERS_H_11102C22_1458_4de9_8DD4_7A08981B188A
	#ifndef KLPRM_PARAMS_H
		#include "./params.h"
	#endif

namespace KLPAR
{
	struct time_wrapper_t
	{
		time_wrapper_t():value(-1){;};
		time_wrapper_t(time_t x):value(x){;};
		time_wrapper_t(const time_wrapper_t& x):value(x.value){;};
		operator time_t() const {return value;};
		time_wrapper_t& operator =(time_t x){value=x; return *this;};
		;
		time_t value;
	};

	struct binary_wrapper_t
    {
        binary_wrapper_t()
            :   m_pData(NULL)
            ,   m_nData(0)
        {;};

        binary_wrapper_t(const binary_wrapper_t& x)
            :   m_pData(x.m_pData)
            ,   m_nData(x.m_nData)
        {;};

        binary_wrapper_t(const void* pData, size_t nData)
            :   m_pData(pData)
            ,   m_nData(nData)
        {;};

        binary_wrapper_t(KLSTD::MemoryChunk* pChunk)
            :   m_pData(pChunk ? pChunk->GetDataPtr() : NULL)
            ,   m_nData(pChunk ? pChunk->GetDataSize() : 0)
        {;};
        ;
        const void* m_pData;
        size_t      m_nData;
    };



	#define KLPAR_DECLARE_event_param_t(_type, _member)								\
		param_entry_t(const wchar_t* szwName, KLPAR::Value::Types type, _type x):	\
				m_szwName(szwName),													\
				m_nType(type),														\
				_member(x)															\
		{																			\
			;																		\
		}

	#define KLPAR_DECLARE2_event_param_t(_ctype, _member, _vtype)			\
		param_entry_t(const wchar_t* szwName, _ctype x):					\
				m_szwName(szwName),											\
				m_nType(KLPAR::Value::_vtype),								\
				_member(x)													\
		{																	\
			;																\
		}

	struct param_entry_t
	{
		KLPAR_DECLARE_event_param_t(const wchar_t*,		m_szwData);
		KLPAR_DECLARE_event_param_t(bool,				m_bData);
		KLPAR_DECLARE_event_param_t(long,				m_lData);
		KLPAR_DECLARE_event_param_t(AVP_longlong,		m_llData);
		KLPAR_DECLARE_event_param_t(const char*,		m_szaData);
		KLPAR_DECLARE_event_param_t(float,				m_fData);
		KLPAR_DECLARE_event_param_t(double,				m_lfData);
		KLPAR_DECLARE_event_param_t(KLPAR::Params*,		m_paramsData);
		KLPAR_DECLARE_event_param_t(KLPAR::Value*,		m_valueData);

		KLPAR_DECLARE2_event_param_t(const wchar_t*,	m_szwData,		STRING_T);
		KLPAR_DECLARE2_event_param_t(bool,				m_bData,		BOOL_T);
		KLPAR_DECLARE2_event_param_t(long,				m_lData,		INT_T);
		KLPAR_DECLARE2_event_param_t(AVP_longlong,		m_llData,		LONG_T);
		KLPAR_DECLARE2_event_param_t(time_wrapper_t&,	m_tData,		DATE_TIME_T);
		KLPAR_DECLARE2_event_param_t(const time_wrapper_t&,	m_tData,		DATE_TIME_T);
		KLPAR_DECLARE2_event_param_t(const char*,		m_szaData,		DATE_T);
		KLPAR_DECLARE2_event_param_t(float,				m_fData,		FLOAT_T);
		KLPAR_DECLARE2_event_param_t(double,			m_lfData,		DOUBLE_T);
		KLPAR_DECLARE2_event_param_t(KLPAR::Params*,	m_paramsData,	PARAMS_T);
		KLPAR_DECLARE2_event_param_t(KLPAR::Value*,		m_valueData,	EMPTY_T);

		const wchar_t*			m_szwName;
		Value::Types			m_nType;
		union{
			bool				m_bData;
			long				m_lData;
			time_t				m_tData;
			AVP_longlong		m_llData;
			float				m_fData;
			double				m_lfData;
			const wchar_t*		m_szwData;
			const char*			m_szaData;
			KLPAR::Params*		m_paramsData;
			KLPAR::Value*		m_valueData;
		};
	};

	#undef KLPAR_DECLARE_event_param_t
	#undef KLPAR_DECLARE2_event_param_t

	KLCSC_DECL void CreateParamsBody(
									KLPAR::param_entry_t*	pData,
									int						nData,
									KLPAR::Params**			ppParams);

/*KLCSAK_BEGIN_PRIVATE*/
	#define KLPAR_DECLARE_makeArrayValue(_type, _id)				\
			KLCSC_DECL void Create##_id##Array(						\
						const std::vector<_type>&	data,			\
						KLPAR::ArrayValue**			ppArrayValue);

		KLPAR_DECLARE_makeArrayValue(const wchar_t*, StringValue)
		KLPAR_DECLARE_makeArrayValue(std::wstring, StringValue)
		KLPAR_DECLARE_makeArrayValue(long, IntValue)
		KLPAR_DECLARE_makeArrayValue(AVP_longlong, LongValue)
		KLPAR_DECLARE_makeArrayValue(bool, BoolValue)
		KLPAR_DECLARE_makeArrayValue(time_t, DateTimeValue)
		KLPAR_DECLARE_makeArrayValue(const char*, DateValue)
		KLPAR_DECLARE_makeArrayValue(std::string, DateValue)
		KLPAR_DECLARE_makeArrayValue(float, FloatValue)
		KLPAR_DECLARE_makeArrayValue(double, DoubleValue)

	#undef KLPAR_DECLARE_makeArrayValue

    KLCSC_DECL void CreateStringValueArray(
                const wchar_t**             pData,
                size_t                      nData,
                KLPAR::ArrayValue**			ppArrayValue);

    //! fast way to create array value
    inline KLSTD::CAutoPtr<KLPAR::ArrayValue> CreateArrayValue()
    {
        KLSTD::CAutoPtr<KLPAR::ArrayValue> pResult;
        KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;
        KLPAR_CreateValuesFactory(&pFactory);
        pFactory->CreateArrayValue(&pResult);
        return pResult;
    };

/*KLCSAK_END_PRIVATE*/

    /*!
      \brief	Функции создания экземпляров классов *Value.
				Используют внутренний экземпляр класса ValuesFactory,
				создаваемый при инициализации модуля KLPAR.
     */
	KLCSC_DECL void CreateValue(const wchar_t*,	StringValue** ppValue);
	KLCSC_DECL void CreateValue(bool,			BoolValue** ppValue);
	KLCSC_DECL void CreateValue(long,			IntValue** ppValue);
	KLCSC_DECL void CreateValue(AVP_longlong,	LongValue** ppValue);
	KLCSC_DECL void CreateValue(time_t,			DateTimeValue** ppValue);
	KLCSC_DECL void CreateValue(const char *,	DateValue** ppValue);
	KLCSC_DECL void CreateValue(const char *,	DateValue** ppValue);
	KLCSC_DECL void CreateValue(float,			FloatValue** ppValue);
	KLCSC_DECL void CreateValue(double,			DoubleValue** ppValue);
	KLCSC_DECL void CreateValue(Params*,		ParamsValue** ppValue);
	KLCSC_DECL void CreateValue(const binary_wrapper_t&, BinaryValue** ppBinaryValue);

	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, StringValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, BoolValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, IntValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, LongValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, DateTimeValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, DateValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, BinaryValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, FloatValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, DoubleValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, ParamsValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, ArrayValue** pVaslue);

/*KLCSAK_BEGIN_PRIVATE*/
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, StringValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, BoolValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, IntValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, LongValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, DateTimeValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, DateValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, BinaryValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, FloatValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, DoubleValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, ParamsValue** pVaslue);
	KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, ArrayValue** pVaslue);

	KLCSC_DECL std::wstring GetStringValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL bool GetBoolValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL long GetIntValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL AVP_longlong GetLongValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL time_t GetDateTimeValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL std::string GetDateValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL float GetFloatValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL double GetDoubleValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL KLSTD::CAutoPtr<Params> GetParamsValue(Params* pParams, const wchar_t* name);
	KLCSC_DECL KLSTD::CAutoPtr<ArrayValue> GetArrayValue(Params* pParams, const wchar_t* name);

	inline std::wstring GetStringValue(Params* pParams, const std::wstring& name)
	{
        return GetStringValue(pParams, name.c_str());
	};

	inline bool GetBoolValue(Params* pParams, const std::wstring& name)
	{
		return GetBoolValue(pParams, name.c_str());
	};

	inline long GetIntValue(Params* pParams, const std::wstring& name)
	{
		return GetIntValue(pParams, name.c_str());
	};

	inline AVP_longlong GetLongValue(Params* pParams, const std::wstring& name)
	{
		return GetLongValue(pParams, name.c_str());
	};

	inline time_t GetDateTimeValue(Params* pParams, const std::wstring& name)
	{
		return GetDateTimeValue(pParams, name.c_str());
	};

	inline std::string GetDateValue(Params* pParams, const std::wstring& name)
	{
		return GetDateValue(pParams, name.c_str());
	};

	inline float GetFloatValue(Params* pParams, const std::wstring& name)
	{
		return GetFloatValue(pParams, name.c_str());
	};

	inline double GetDoubleValue(Params* pParams, const std::wstring& name)
	{
		return GetDoubleValue(pParams, name.c_str());
	};

	inline KLSTD::CAutoPtr<Params> GetParamsValue(Params* pParams, const std::wstring& name)
	{
		return GetParamsValue(pParams, name.c_str());
	};

	inline KLSTD::CAutoPtr<ArrayValue> GetArrayValue(Params* pParams, const std::wstring& name)
	{
		return GetArrayValue(pParams, name.c_str());
	};
/*KLCSAK_END_PRIVATE*/

//KLCSAK_BEGIN_PRIVATE
    /*
//KLCSAK_END_PRIVATE
	inline const wchar_t* GetStringValue(Params* pParams, const wchar_t* name, int x = 0)
	{
		KLSTD::CAutoPtr<StringValue> pResult;
		GetValue(pParams, name, &pResult);
		const wchar_t* szwResult=pResult->GetValue();
		return  szwResult ? szwResult : L"";
	};

	inline bool GetBoolValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<BoolValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline long GetIntValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<IntValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline AVP_longlong GetLongValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<LongValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline time_t GetDateTimeValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<DateTimeValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline const char* GetDateValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<DateValue> pResult;
		GetValue(pParams, name, &pResult);
		const char* szResult=pResult->GetValue();
		return szResult ? szResult : "";
	};

	inline float GetFloatValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<FloatValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline double GetDoubleValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<DoubleValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	inline KLSTD::CAutoPtr<Params> GetParamsValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<ParamsValue> pResult;
		GetValue(pParams, name, &pResult);
		KLSTD::CAutoPtr<Params> pParVal;
		if(pResult)
			pParVal=pResult->GetValue();
		return pParVal;
	};

	inline KLSTD::CAutoPtr<ArrayValue> GetArrayValue(Params* pParams, const wchar_t* name)
	{
		KLSTD::CAutoPtr<ArrayValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult;
	};
//KLCSAK_BEGIN_PRIVATE
    */
//KLCSAK_END_PRIVATE
};

/*KLCSAK_BEGIN_PRIVATE*/
/*!
  \brief	Чтение переменной с использованием пути. Если переменная не найдена,
				выбрасывается исключение NOT_EXIST
			\param pMountPoint	[in] Исходный контейнер Params
			\param ppPath		[in] Путь - массив указателей на строки, завершается нулевым указателем.
			\param wstrName		[in] Имя переменной, которую нужно получить
			\param ppValue		[out] Указатель на переменную, в которую будет записан указатель на интерфейс Value

 */
KLCSC_DECL void KLPAR_GetValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value**		ppValue);

/*!
  \brief	Установка переменой с использованием пути.
			Путь - это массив указателей на строки, завершается нулевым указателем.
 */
KLCSC_DECL void KLPAR_SetValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue);

/*!
  \brief	Добавление переменой с использованием пути.
			Путь - это массив указателей на строки, завершается нулевым указателем.
 */
KLCSC_DECL void KLPAR_AddValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue);


/*!
  \brief	Замещение переменой с использованием пути.
			Путь - это массив указателей на строки, завершается нулевым указателем.
 */
KLCSC_DECL void KLPAR_ReplaceValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue);
/*KLCSAK_END_PRIVATE*/

/*!
  \brief	Replaces value by path.

  \param	pMountPoint [in] src container
  \param	ppPath      [in] array pointers to string, last elemet is NULL pointer
  \param	szwName     [in] value name
  \param	pValue      [in] value
*/
KLCSC_DECL void KLPAR_ReplaceValueByPath(
                    KLPAR::Params*  pMountPoint,
                    const wchar_t** ppPath,
                    const wchar_t*  szwName,
                    KLPAR::Value*   pValue);

/*!
  \brief	Reads value by path. If variable was not found, exception NOT_EXIST is thrown.
            \param pMountPoint	[in] src container
            \param ppPath		[in] array pointers to string, last elemet is NULL pointer
            \param wstrName		[in] value name
            \param ppValue		[out] value

 */
KLCSC_DECL void KLPAR_GetValueByPath(
					KLPAR::Params*	pMountPoint,
					const wchar_t**	ppPath,
					const wchar_t*  szwName,
					KLPAR::Value**	ppValue);


/*KLCSAK_BEGIN_PRIVATE*/

KLCSC_DECL void KLPAR_SerializeToFileID(int id, const KLPAR::Params* pParams);

KLCSC_DECL void KLPAR_SerializeToFileName(const std::wstring& wstrName, const KLPAR::Params* pParams);

/*KLCSAK_END_PRIVATE*/

/*!
  \brief	Serializes container Params to file specified by its name

  \param	szwName [in] file name
  \param	pParams [in] container params to serialize
*/
KLCSC_DECL void KLPAR_SerializeToFile(
                    const wchar_t*          szwName,
                    const KLPAR::Params*    pParams);

/*!
  \brief	Serializes container Params into memory chunk

  \param	pParams [in] container params to serialize
  \param	ppChunk [out] resulting memory chunk
*/
KLCSC_DECL void KLPAR_SerializeToMemory(
                    const KLPAR::Params*    pParams,
                    KLSTD::MemoryChunk**    ppChunk);

/*KLCSAK_BEGIN_PRIVATE*/

KLCSC_DECL void KLPAR_DeserializeFromFileID(int id, KLPAR::Params** ppParams);

KLCSC_DECL void KLPAR_DeserializeFromFileName(const std::wstring& wstrName, KLPAR::Params** ppParams);

/*KLCSAK_END_PRIVATE*/


/*!
  \brief	Deserializes container Params from file specified by its name

  \param	szwName     [in] file name
  \param	ppParams    [out] deserialized container params
*/
KLCSC_DECL void KLPAR_DeserializeFromFile(
                    const wchar_t* szwName,
                    KLPAR::Params** ppParams);


/*!
  \brief	Deserializes container Params from specified memory location

  \param	pData       [in] pointer to memory location
  \param	nData       [in] size of memory area
  \param	ppParams    [out] deserialized container params
*/
KLCSC_DECL void KLPAR_DeserializeFromMemory(
                    const void* pData,
                    size_t nData,
                    KLPAR::Params** ppParams);

/*KLCSAK_BEGIN_PRIVATE*/
typedef enum
{
    KLPAR_SF_SOAP   =   0x00000000,//store as soap xml
    KLPAR_SF_BIN    =   0x00000001,//store binary
    KLPAR_SF_HEX    =   0x00000002,//encode serialized data as hex
    KLPAR_SF_BASE64 =   0x00000004,//encode serialized data as base64
    KLPAR_SF_USE_UTF8 = 0x00010000,//store unicode strings as UTF8
    KLPAR_SF_COMPRESS = 0x00020000,

    KLPAR_SF_BINARY =   KLPAR_SF_BIN,
    KLPAR_SF_BINHEX =   KLPAR_SF_BIN|KLPAR_SF_HEX,
    KLPAR_SF_BINB64 =   KLPAR_SF_BIN|KLPAR_SF_BASE64,
    KLPAR_SF_BIN_COMPRESS = KLPAR_SF_BIN|KLPAR_SF_COMPRESS
}KLPAR_SERFLAGS;

KLCSC_DECL void KLPAR_SerializeToFileID2(
                                int                     id,
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams);

KLCSC_DECL void KLPAR_SerializeToFileName2(
                                const std::wstring&    wstrName,
                                AVP_dword              lFlags,
                                KLPAR::Params*         pParams);

KLCSC_DECL void KLPAR_SerializeToMemory2(
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams,
                                KLSTD::MemoryChunk**    ppChunk);

KLCSC_DECL void KLPAR_DeserializeFromFileID2(
                                int             id,
                                AVP_dword&      lFlags,
                                KLPAR::Params** ppParams);

KLCSC_DECL void KLPAR_DeserializeFromFileName2(
                                const std::wstring& wstrName,
                                AVP_dword&          lFlags,
                                KLPAR::Params**     ppParams);

KLCSC_DECL void KLPAR_DeserializeFromMemory2(
                                const void*     pData,
                                size_t          nData,
                                AVP_dword&      lFlags,
                                KLPAR::Params** ppParams);

KLCSC_DECL void KLPAR_Write(KLPAR::Params* pOldParams, KLPAR::Params* pNewParams, AVP_dword dwFlags);

KLCSC_DECL void KLPAR_Concatenate(
				KLPAR::Params*	pMaster,
				KLPAR::Params*	pSlave,
				KLPAR::Params** ppResult);


KLCSC_DECL void KLPAR_GetParamsSubtree(
					KLPAR::Params* pSrc,
					KLPAR::Params* pFilter,
					KLPAR::Params** ppDst);

/*KLCSAK_END_PRIVATE*/


#endif // _HELPERS_H_11102C22_1458_4de9_8DD4_7A08981B188A
