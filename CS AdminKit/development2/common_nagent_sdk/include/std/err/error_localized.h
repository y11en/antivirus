#ifndef __KL_ERROR_LOCALIZED_H__
#define __KL_ERROR_LOCALIZED_H__


/*!
	\brief	Ниже определены макросы KLERR_LOCTHROWx для выбрасывания исключений
			с x параметрами.
		\params _module [in] имя модуля, от имени которого будет выброшено исключение
		\params _code [in] код исключения
		\params _parami [in] параметр исключения.
*/

#define KLERR_LOCTHROW0(_loc, _module, _code)	\
	KLERR_throwLocError(_loc, _module, _code, __FILE__, __LINE__, NULL, NULL)

#define KLERR_LOCTHROW1(_loc, _module, _code, _param1)	\
	KLERR_throwLocError(_loc, _module, _code, __FILE__, __LINE__, NULL, _param1)

#define KLERR_LOCTHROW2(_loc, _module, _code, _param1, _param2)	\
	KLERR_throwLocError(_loc, _module, _code, __FILE__, __LINE__, NULL, _param1, _param2)

#define KLERR_LOCTHROW3(_loc, _module, _code, _param1, _param2, _param3)	\
	KLERR_throwLocError(_loc, _module, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3)

#define KLERR_LOCTHROW4(_loc, _module, _code, _param1, _param2, _param3, _param4)	\
	KLERR_throwLocError(_loc, _module, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3, _param4)

/*!
	\brief	Ниже определены макросы KLERR_MYTHROWx для выбрасывания исключений
			даного модуля с x параметрами. Для их использования необходимо,
			чтобы в использующем их cpp-файле был определён макрос KLCS_MODULENAME
			Пример:
				#define KLCS_MODULENAME L"KLSTD"
*/

/*!
	\brief	Макросы KLERR_LOCMYTHROWx для выбрасывания исключений
			с x параметрами.
		\params _code [in] код исключения
		\params _parami [in] параметр исключения.
*/
#define KLERR_LOCMYTHROW0(_loc, _code)	\
	KLERR_throwLocError(_loc, KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, NULL)

#define KLERR_LOCMYTHROW1(_loc, _code, _param1)	\
	KLERR_throwLocError(_loc, KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1)

#define KLERR_LOCMYTHROW2(_loc, _code, _param1, _param2)	\
	KLERR_throwLocError(_loc, KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2)

#define KLERR_LOCMYTHROW3(_loc, _code, _param1, _param2, _param3)	\
	KLERR_throwLocError(_loc, KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3)

#define KLERR_LOCMYTHROW4(_loc, _code, _param1, _param2, _param3, _param4)	\
	KLERR_throwLocError(_loc, KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3, _param4)


namespace KLERR
{
    struct ErrorDescription;

    class Error;

    //! helper class for passing localization parameters
    class ErrLocAdapt
    {
    public:
        ErrLocAdapt(
                    int             nFormat,
                    const wchar_t*  szwLocModuleName = NULL,
                    const wchar_t*  szwPar1 = NULL,
                    const wchar_t*  szwPar2 = NULL,
                    const wchar_t*  szwPar3 = NULL,
                    const wchar_t*  szwPar4 = NULL,
                    const wchar_t*  szwPar5 = NULL,
                    const wchar_t*  szwPar6 = NULL,
                    const wchar_t*  szwPar7 = NULL,
                    const wchar_t*  szwPar8 = NULL,
                    const wchar_t*  szwPar9 = NULL)
            :   m_nFormat(nFormat)
            ,   m_szwLocModuleName(szwLocModuleName)
            ,   m_szwPar1(szwPar1)
            ,   m_szwPar2(szwPar2)
            ,   m_szwPar3(szwPar3)
            ,   m_szwPar4(szwPar4)
            ,   m_szwPar5(szwPar5)
            ,   m_szwPar6(szwPar6)
            ,   m_szwPar7(szwPar7)
            ,   m_szwPar8(szwPar8)
            ,   m_szwPar9(szwPar9)
        {
            ;
        };
        //! format string id
        int             m_nFormat;

        //! Localisation module name. if NULL Error::GetModuleName() is used.
        const wchar_t*  m_szwLocModuleName;

        //! substitution parameters 1..9
        const wchar_t*  m_szwPar1;
        const wchar_t*  m_szwPar2;
        const wchar_t*  m_szwPar3;
        const wchar_t*  m_szwPar4;
        const wchar_t*  m_szwPar5;
        const wchar_t*  m_szwPar6;
        const wchar_t*  m_szwPar7;
        const wchar_t*  m_szwPar8;
        const wchar_t*  m_szwPar9;
    };

    //! provides error localization info
    class KLSTD_NOVTABLE ErrorLocalization
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief    Writes localization info into specified error. 

          \param	szwValue IN locinfo 
        */
        KLSTD_NOTHROW virtual void LocSetInfo(
                            const ErrLocAdapt&  locinfo) throw() = 0;
        
        /*!
          \brief	Returns localization format string id

          \return	localization format string id
        */
        KLSTD_NOTHROW virtual long LocGetFormatId() throw() = 0;
       
        /*!
          \brief	Returns localization format string. 

          \return	localization description value, returns L"" if description was not specified
        */
        KLSTD_NOTHROW virtual const wchar_t* LocGetFormatString() throw() = 0;

        /*!
          \brief	Returns localization substitution parameter

          \param	nIndex IN parameter one-based index: 1..9
          \return   parameter value, returns L"" if no such parameter
        */
        KLSTD_NOTHROW virtual const wchar_t* LocGetPar(size_t nIndex) throw() = 0;

        /*!
          \brief	Returns substitution parameters count

          \return	substitution parameters count
        */
        KLSTD_NOTHROW virtual size_t LocGetParCount() throw() = 0;

        /*!
          \brief	Checks whether error is localized

          \return	true if localized
        */
        KLSTD_NOTHROW virtual bool IsLocalized() throw() = 0;
        
        /*!
          \brief	GetLocModuleName

          \return	module name
        */
        KLSTD_NOTHROW virtual const wchar_t* GetLocModuleName() throw() = 0;
    };

    typedef KLSTD::CAutoPtr<KLERR::ErrorLocalization> ErrorLocalizationPtr;
};

/*! creates error with localization parameters and throws it
    See description of function KLERR_throwError
*/
KLCSC_DECL KLSTD_NORETURN void KLERR_throwLocError(
    const KLERR::ErrLocAdapt&   locinfo,
	const wchar_t*	            szwModule,
	int				            nId,
	const char*		            szaFile, 
	int				            nLine,
	const wchar_t*	            szwMessage,...);

/*! creates error with localization parameters
    See description of function KLERR_CreateError2
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_CreateLocError(
    const KLERR::ErrLocAdapt&   locinfo,
	KLERR::Error**              ppError,
	const wchar_t*	            szwModule,
	int				            nId,
	const char*		            szaFile, 
	int				            nLine,
	const wchar_t*	            szwMessage, ...) throw();

/*!
  \brief	Writes localization info into specified error. See 
            description of KLSTD_FormatString know more about 
            szwFormatString format.

  \param	pError           IN error to localize. If pError is NULL, 
                                function does nothing. 
  \param	szwLocModuleName  IN localization module name. if NULL Error::GetModuleName() is used.
  \param	nFormatStringId  IN localization format string id
  \param	szwPar1..szwPar9 IN substitution parameters
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_SetErrorLocalization(
                KLERR::Error*   pError,
                long            nFormatStringId,
                const wchar_t*  szwLocModuleName=   NULL,
                const wchar_t*  szwPar1 =           NULL,
                const wchar_t*  szwPar2 =           NULL,
                const wchar_t*  szwPar3 =           NULL,
                const wchar_t*  szwPar4 =           NULL,
                const wchar_t*  szwPar5 =           NULL,
                const wchar_t*  szwPar6 =           NULL,
                const wchar_t*  szwPar7 =           NULL,
                const wchar_t*  szwPar8 =           NULL,
                const wchar_t*  szwPar9 =           NULL);

/*!
    \brief	Checks whether error is localized

    \return	true if localized
*/
KLSTD_NOTHROW KLCSC_DECL bool KLERR_IsErrorLocalized(KLERR::Error* pError);

/*!
  \brief	Returns specified error. Uses specified localized format 
            string szwFormatString. See description of 
            KLSTD_FormatString know more about szwFormatString format.

            Code sample:
                    KLSTD::klwstr_t strResult;
                    KLERR_LocFormatErrorString(
                            pError,
                            strResult.outref());

  \param	pError           IN error to localize. If pError is NULL, 
                                function does nothing. 
  \param	wstrResult      OUT result.
  \return   false if error doesn't have localization info
*/
KLSTD_NOTHROW KLCSC_DECL bool KLERR_LocFormatErrorString(
                KLERR::Error*       pError,
                KLSTD::AKWSTR&      wstrResult);
    

/*!
  \brief    Initializes localization defaults for module. 
            See description of KLERR_InitModuleDescriptions

  \param	szwModule   IN  module name
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_InitModuleLocalizationDefaults( 
    const wchar_t*					szwModule, 
    const KLERR::ErrorDescription*	descr,
    int								arraySize) throw();

/*!
  \brief    Deinitializes localization defaults for module. 
            See description of KLERR_DeinitModuleDescriptions 

  \param	szwModule   IN  module name
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_DeinitModuleLocalizationDefaults(
                                    const wchar_t* szwModule) throw();

/*!
  \brief    Sets localization file for module
  \param	szwModule   IN  module name
  \param	szwFileName IN  file name
  \param	nStart      IN  id of first module string in file
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_InitModuleLocalization( 
                                    const wchar_t*	szwModule, 
                                    const wchar_t*	szwFileName, 
                                    int             nStart) throw();

/*!
  \brief    Sets localization file for module
  \param	szwModule   IN  module name
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_DeinitModuleLocalization(
                                    const wchar_t* szwModule) throw();

/*!
  \brief	Loads localization string for specified module. if failed 
            uses default szwDefault and returns false.

            Code sample:
                    KLSTD::klwstr_t strResult;
                    KLERR_FindLocString(
                            KLCS_MODULENAME,
                            ERRL_ERROR_4,
                            strResult.outref());

  \param	szwModule IN module name.
  \param	nCode IN id of string
  \param	szwDefault IN default value, NULL is equivalent to L"". 
            If default is NULL, defaults set by 
            KLERR_InitModuleLocalizationDefaults is used
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_FindLocString(
                                    const wchar_t*	szwModule, 
                                    int             nCode,
                                    KLSTD::AKWSTR&  wstrResult,
                                    const wchar_t*  szwDefault = NULL) throw();





/*!
  \brief	Formats localized string. 

  \param	szwModule   IN localization module name
  \param	nCode       IN localization string id
  \param	wstrResult  OUT result string
  \param	szwPar1..9  IN  string arguments
  \return	true if success and wstrResult is not empty string
*/
KLSTD_NOTHROW KLCSC_DECL bool KLERR_LocFormatModuleString(
                const wchar_t*  szwModule,
                int             nCode,
                KLSTD::AKWSTR&  wstrResult,
                const wchar_t*  szwPar1 = NULL,
                const wchar_t*  szwPar2 = NULL,
                const wchar_t*  szwPar3 = NULL,
                const wchar_t*  szwPar4 = NULL,
                const wchar_t*  szwPar5 = NULL,
                const wchar_t*  szwPar6 = NULL,
                const wchar_t*  szwPar7 = NULL,
                const wchar_t*  szwPar8 = NULL,
                const wchar_t*  szwPar9 = NULL) throw();


/*!
  \brief	Returns error localized description. If error is not localized then 
            pError->GetMsg() is returned. 

  \param	pError          IN  error 
  \param	wstrMesage      OUT result string
*/
KLSTD_NOTHROW KLCSC_DECL void KLERR_GetErrorLocDesc(
                KLERR::Error*   pError, 
                KLSTD::AKWSTR&  wstrMessage) throw();

#endif //__KL_ERROR_LOCALIZED_H__
