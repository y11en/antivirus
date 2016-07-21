/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	simple_params.h
 * \author	Andrew Kazachkov
 * \date	31.03.2003 13:07:28
 * \brief	
 * 
 */

#ifndef _SIMPLE_PARAMS_H_739AC644_0DF0_40dd_B6EC_F4BD2F6984E2
#define _SIMPLE_PARAMS_H_739AC644_0DF0_40dd_B6EC_F4BD2F6984E2

#include <std/par/helpers.h>

namespace KLPRCI
{
	typedef KLPAR::time_wrapper_t	wrap_time_t;

	typedef struct KLPAR::param_entry_t	event_param_t;

	KLSTD_INLINEONLY void makeParamsBody(
								event_param_t*	pData,
								int				nData,
								KLPAR::Params** ppParams)
	{
		KLPAR::CreateParamsBody(pData, nData, ppParams);
	};

	#define DECLARE_makeArrayValue(_type, _id)						\
		KLSTD_INLINEONLY void make##_id##Array(						\
						const std::vector<_type>&	data,			\
						KLPAR::ArrayValue**			ppArrayValue)	\
	{																\
		KLPAR::Create##_id##Array(data, ppArrayValue);				\
	};

	DECLARE_makeArrayValue(const wchar_t*, StringValue)
	DECLARE_makeArrayValue(std::wstring, StringValue)
	DECLARE_makeArrayValue(long, IntValue)
	DECLARE_makeArrayValue(AVP_longlong, LongValue)
	DECLARE_makeArrayValue(bool, BoolValue)
	DECLARE_makeArrayValue(time_t, DateTimeValue)
	DECLARE_makeArrayValue(const char*, DateValue)
	DECLARE_makeArrayValue(std::string, DateValue)
	DECLARE_makeArrayValue(float, FloatValue)
	DECLARE_makeArrayValue(double, DoubleValue)

	#undef DECLARE_makeArrayValue
};
#endif
