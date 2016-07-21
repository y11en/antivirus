/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	admsrv_sinks.h
 * \author	Andrew Kazachkov
 * \date	06.02.2004 10:56:31
 * \brief	
 * 
 */

#ifndef __KL_ADMSRV_SINKS_H__
#define __KL_ADMSRV_SINKS_H__

#include <transport/ev/sinks_base.h>

namespace KLADMSRV
{
	typedef KLEV::EvSink AdmServerSink;
	typedef KLEV::EvSinks AdmServerSinks;
	typedef KLEV::subscription_data_t subscription_data_t;
};

#endif //__KL_ADMSRV_SINKS_H__