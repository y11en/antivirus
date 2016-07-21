// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  15 February 2005,  17:22 --------
// File Name   -- (null)i_inifile.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_inifile__a68f12df_8646_4a40_a2e3_d7f4f72544a8 )
#define __i_inifile__a68f12df_8646_4a40_a2e3_d7f4f72544a8
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IniFile interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_INIFILE  ((tIID)(58023))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hINIFILE;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iIniFileVtbl;
typedef struct iIniFileVtbl iIniFileVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIniFile;
	typedef cIniFile* hINIFILE;
#else
	typedef struct tag_hINIFILE 
	{
		const iIniFileVtbl* vtbl; // pointer to the "IniFile" virtual table
		const iSYSTEMVtbl*  sys;  // pointer to the "SYSTEM" virtual table
	} *hINIFILE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IniFile_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIniFile_Deserialize)      ( hINIFILE _this, cSerializable* pSerializable ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_GetSectionsCount) ( hINIFILE _this, tDWORD* pdwSectionsCount ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_FindSection)      ( hINIFILE _this, tSTRING sSectionName, tDWORD* pdwSectionIndex ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_EnumSections)     ( hINIFILE _this, tDWORD dwSectionIndex, tSTRING* psSectionName, tDWORD* pdwValuesCount ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_GetValuesCount)   ( hINIFILE _this, tDWORD dwSectionIndex, tDWORD* pdwValuesCount ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_EnumValues)       ( hINIFILE _this, tDWORD dwSectionIndex, tDWORD dwValueIndex, tSTRING* psValueName, tSTRING* psValueData ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_GetValue)         ( hINIFILE _this, tSTRING sSectionName, tSTRING sValueName, tSTRING* psValueData ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_SetValue)         ( hINIFILE _this, tSTRING sSectionName, tSTRING sValueName, tSTRING sValueData ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_DelValue)         ( hINIFILE _this, tSTRING sSectionName, tSTRING sValueName ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_DelSection)       ( hINIFILE _this, tSTRING sSectionName ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_Commit)           ( hINIFILE _this );                // -- ;
	typedef   tERROR (pr_call *fnpIniFile_SetValueByIndex)  ( hINIFILE _this, tDWORD dwSectionIndex, tDWORD dwValueIndex, tSTRING sValueData ); // -- ;
	typedef   tERROR (pr_call *fnpIniFile_DelValueByIndex)  ( hINIFILE _this, tDWORD dwSectionIndex, tDWORD dwValueIndex ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iIniFileVtbl 
{
	fnpIniFile_Deserialize       Deserialize;
	fnpIniFile_GetSectionsCount  GetSectionsCount;
	fnpIniFile_FindSection       FindSection;
	fnpIniFile_EnumSections      EnumSections;
	fnpIniFile_GetValuesCount    GetValuesCount;
	fnpIniFile_EnumValues        EnumValues;
	fnpIniFile_GetValue          GetValue;
	fnpIniFile_SetValue          SetValue;
	fnpIniFile_DelValue          DelValue;
	fnpIniFile_DelSection        DelSection;
	fnpIniFile_Commit            Commit;
	fnpIniFile_SetValueByIndex   SetValueByIndex;
	fnpIniFile_DelValueByIndex   DelValueByIndex;
}; // "IniFile" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IniFile_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000040 )
//! #define pgOBJECT_FULL_NAME  mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000041 )
//! #define pgOBJECT_PATH       mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000044 )
#define pgm_hIO             mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
//! #define pgPLUGIN_CODEPAGE   mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c5 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IniFile_Deserialize( _this, pSerializable )                                                     ((_this)->vtbl->Deserialize( (_this), pSerializable ))
	#define CALL_IniFile_GetSectionsCount( _this, pdwSectionsCount )                                             ((_this)->vtbl->GetSectionsCount( (_this), pdwSectionsCount ))
	#define CALL_IniFile_FindSection( _this, sSectionName, pdwSectionIndex )                                     ((_this)->vtbl->FindSection( (_this), sSectionName, pdwSectionIndex ))
	#define CALL_IniFile_EnumSections( _this, dwSectionIndex, psSectionName, pdwValuesCount )                    ((_this)->vtbl->EnumSections( (_this), dwSectionIndex, psSectionName, pdwValuesCount ))
	#define CALL_IniFile_GetValuesCount( _this, dwSectionIndex, pdwValuesCount )                                 ((_this)->vtbl->GetValuesCount( (_this), dwSectionIndex, pdwValuesCount ))
	#define CALL_IniFile_EnumValues( _this, dwSectionIndex, dwValueIndex, psValueName, psValueData )             ((_this)->vtbl->EnumValues( (_this), dwSectionIndex, dwValueIndex, psValueName, psValueData ))
	#define CALL_IniFile_GetValue( _this, sSectionName, sValueName, psValueData )                                ((_this)->vtbl->GetValue( (_this), sSectionName, sValueName, psValueData ))
	#define CALL_IniFile_SetValue( _this, sSectionName, sValueName, sValueData )                                 ((_this)->vtbl->SetValue( (_this), sSectionName, sValueName, sValueData ))
	#define CALL_IniFile_DelValue( _this, sSectionName, sValueName )                                             ((_this)->vtbl->DelValue( (_this), sSectionName, sValueName ))
	#define CALL_IniFile_DelSection( _this, sSectionName )                                                       ((_this)->vtbl->DelSection( (_this), sSectionName ))
	#define CALL_IniFile_Commit( _this )                                                                         ((_this)->vtbl->Commit( (_this) ))
	#define CALL_IniFile_SetValueByIndex( _this, dwSectionIndex, dwValueIndex, sValueData )                      ((_this)->vtbl->SetValueByIndex( (_this), dwSectionIndex, dwValueIndex, sValueData ))
	#define CALL_IniFile_DelValueByIndex( _this, dwSectionIndex, dwValueIndex )                                  ((_this)->vtbl->DelValueByIndex( (_this), dwSectionIndex, dwValueIndex ))
#else // if !defined( __cplusplus )
	#define CALL_IniFile_Deserialize( _this, pSerializable )                                                     ((_this)->Deserialize( pSerializable ))
	#define CALL_IniFile_GetSectionsCount( _this, pdwSectionsCount )                                             ((_this)->GetSectionsCount( pdwSectionsCount ))
	#define CALL_IniFile_FindSection( _this, sSectionName, pdwSectionIndex )                                     ((_this)->FindSection( sSectionName, pdwSectionIndex ))
	#define CALL_IniFile_EnumSections( _this, dwSectionIndex, psSectionName, pdwValuesCount )                    ((_this)->EnumSections( dwSectionIndex, psSectionName, pdwValuesCount ))
	#define CALL_IniFile_GetValuesCount( _this, dwSectionIndex, pdwValuesCount )                                 ((_this)->GetValuesCount( dwSectionIndex, pdwValuesCount ))
	#define CALL_IniFile_EnumValues( _this, dwSectionIndex, dwValueIndex, psValueName, psValueData )             ((_this)->EnumValues( dwSectionIndex, dwValueIndex, psValueName, psValueData ))
	#define CALL_IniFile_GetValue( _this, sSectionName, sValueName, psValueData )                                ((_this)->GetValue( sSectionName, sValueName, psValueData ))
	#define CALL_IniFile_SetValue( _this, sSectionName, sValueName, sValueData )                                 ((_this)->SetValue( sSectionName, sValueName, sValueData ))
	#define CALL_IniFile_DelValue( _this, sSectionName, sValueName )                                             ((_this)->DelValue( sSectionName, sValueName ))
	#define CALL_IniFile_DelSection( _this, sSectionName )                                                       ((_this)->DelSection( sSectionName ))
	#define CALL_IniFile_Commit( _this )                                                                         ((_this)->Commit( ))
	#define CALL_IniFile_SetValueByIndex( _this, dwSectionIndex, dwValueIndex, sValueData )                      ((_this)->SetValueByIndex( dwSectionIndex, dwValueIndex, sValueData ))
	#define CALL_IniFile_DelValueByIndex( _this, dwSectionIndex, dwValueIndex )                                  ((_this)->DelValueByIndex( dwSectionIndex, dwValueIndex ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iIniFile 
	{
		virtual tERROR pr_call Deserialize( cSerializable* pSerializable ) = 0;
		virtual tERROR pr_call GetSectionsCount( tDWORD* pdwSectionsCount ) = 0;
		virtual tERROR pr_call FindSection( tSTRING sSectionName, tDWORD* pdwSectionIndex ) = 0;
		virtual tERROR pr_call EnumSections( tDWORD dwSectionIndex, tSTRING* psSectionName, tDWORD* pdwValuesCount ) = 0;
		virtual tERROR pr_call GetValuesCount( tDWORD dwSectionIndex, tDWORD* pdwValuesCount ) = 0;
		virtual tERROR pr_call EnumValues( tDWORD dwSectionIndex, tDWORD dwValueIndex, tSTRING* psValueName, tSTRING* psValueData ) = 0;
		virtual tERROR pr_call GetValue( tSTRING sSectionName, tSTRING sValueName, tSTRING* psValueData ) = 0;
		virtual tERROR pr_call SetValue( tSTRING sSectionName, tSTRING sValueName, tSTRING sValueData ) = 0;
		virtual tERROR pr_call DelValue( tSTRING sSectionName, tSTRING sValueName ) = 0;
		virtual tERROR pr_call DelSection( tSTRING sSectionName ) = 0;
		virtual tERROR pr_call Commit() = 0;
		virtual tERROR pr_call SetValueByIndex( tDWORD dwSectionIndex, tDWORD dwValueIndex, tSTRING sValueData ) = 0;
		virtual tERROR pr_call DelValueByIndex( tDWORD dwSectionIndex, tDWORD dwValueIndex ) = 0;
	};

	struct pr_abstract cIniFile : public iIniFile, public iObj {

		OBJ_IMPL( cIniFile );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hINIFILE()   { return (hINIFILE)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		hOBJECT pr_call get_pgm_hIO() { return (hOBJECT)getObj(pgm_hIO); }
		tERROR pr_call set_pgm_hIO( hOBJECT value ) { return setObj(pgm_hIO,(hOBJECT)value); }

		tCODEPAGE pr_call get_pgPLUGIN_CODEPAGE() { return (tCODEPAGE)getDWord(pgPLUGIN_CODEPAGE); }
		tERROR pr_call set_pgPLUGIN_CODEPAGE( tCODEPAGE value ) { return setDWord(pgPLUGIN_CODEPAGE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_inifile__a68f12df_8646_4a40_a2e3_d7f4f72544a8
// AVP Prague stamp end



