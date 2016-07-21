#ifndef __PROPERTYSETGET_H__
#define __PROPERTYSETGET_H__

#include "property/property.h"

bool       CheckBinaryValue( const TCHAR *pStr );

int				 GetDataDatasCount( HDATA hData );
int				 GetDataPropsCount( HDATA hData );

TCHAR			*GetDataPropertyString( HDATA hData, AVP_dword nPID );

void			 SetPropValue( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat );
void			 SetPropArrayValue( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat );

void			 SetPropNumericValue( HPROP hProp, AVP_dword nValueValue );
void			 SetPropArrayNumericValue( HPROP hProp, int nPos, AVP_dword nValueValue );

void			 SetPropValueCC( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat );
void			 SetPropArrayValueCC( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat );

void			 SetPropDateValue( HPROP hProp, const SYSTEMTIME *pSysTime );
void			 SetPropDateArrayValue( HPROP hProp, int nPos, const SYSTEMTIME *pSysTime );

AVP_dword	 GetPropNumericValueAsDWord( HPROP hProp );
AVP_dword	 GetPropArrayNumericValueAsDWord( HPROP hProp, int nPos );

TCHAR			*GetPropValueAsString( HPROP hProp, const TCHAR *pFormat );
TCHAR			*GetPropArrayValueAsString( HPROP hProp, int nPos, const TCHAR *pFormat );

TCHAR			*GetPropValueAsStringCC( HPROP hProp, const TCHAR *pFormat );
TCHAR			*GetPropArrayValueAsStringCC( HPROP hProp, int nPos, const TCHAR *pFormat );

TCHAR			*GetDataValueAsComboString( HDATA hData );
TCHAR			*GetDataNumericValueAsComboString( HDATA hData );

bool			 SetDataNumericValueAsComboString( HDATA hData, const TCHAR *pValueText );
bool			 SetDataArrayNumericValueAsComboString( HDATA hData, int nPos, const TCHAR *pValueText );

int				 CompareValueInRangeI( HDATA hData, const TCHAR *pValueText, 
																 AVP_dword nMinID, AVP_dword nMaxID, const TCHAR *pFormat );
int				 CompareValueInRangeH( HDATA hData, const TCHAR *pValueText, 
																 HPROP hMinProp, HPROP hMaxProp, const TCHAR *pFormat );
int				 CompareDateValueInRangeI( HDATA hData, const SYSTEMTIME *pValueTime, 
																		 AVP_dword nMinID, AVP_dword nMaxID );
int				 CompareDateValueInRangeH( HDATA hData, const SYSTEMTIME *pValueTime, 
																		 HPROP hMinProp, HPROP hMaxProp );

void       ExchangeValue( HPROP hFromProp, HPROP hToProp );

HDATA			 CreateNodesSequence( HDATA hStartData, AVP_dword *pAddrSequence );

void			 GetDateEditFormat( TCHAR *pFormat, DWORD nSize );
void			 GetTimeEditFormat( TCHAR *pFormat, DWORD nSize );
void			 GetDateTimeEditFormat( TCHAR *pFormat, DWORD nSize );

TCHAR			*TypeID2TypeName( int nType );
int				 TypeName2TypeID( const TCHAR *pTypeName );
int				 TypeNameMaxLength();

#endif