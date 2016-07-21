/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	analyzer.h
*		\brief	интерфайс анализатора для HTTP
*		
*		\author Владислав Овчарик
*		\date	25.04.2005 16:10:52
*		
*		
*/		
#if !defined(_HTTP_ANALYZER_INCLUDE_H_)
#define _HTTP_ANALYZER_INCLUDE_H_
#include <ProtocollersEx/common/ProtocolAnalyzerImplAdapter.h>
#include "analyzerimpl.h"
///////////////////////////////////////////////////////////////////////////////
namespace TrafficProtocoller
{
///////////////////////////////////////////////////////////////////////////////
typedef Util::ProtocolAnalyzerImplAdapter<http::analyzer> HttpProtocolAnalyzer;
///////////////////////////////////////////////////////////////////////////////
}//namespace TrafficProtocoller
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_ANALYZER_INCLUDE_H_)