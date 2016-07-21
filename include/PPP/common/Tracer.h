// Tracer.h: interface for the cPrTracer class.
//

#pragma once

#include <Prague/prague.h>
#include <Prague/iface/e_ktrace.h>

#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// cPrTracer

class cPrTracer
{
public:
	cPrTracer();
	~cPrTracer();
	
	tERROR         Init(HINSTANCE hInst = NULL, bool bNoDateTime = false, tCHAR *pPrefix = NULL);
	tERROR         Deinit();
	static void    TraceLink(tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel);
	static void	   Trace1(tSTRING szMsg);
	static void	   Trace2(tSTRING szMsg, tSTRING szSourceFile, int nSourceLine);

protected:
	tDWORD         MakeTracePrefix(tCHAR* OUT p_str, tDWORD size, hOBJECT hObject, tTRACE_LEVEL dwLevel);
	void           Trace(tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel);
	void		   FreeResources();
	void		   GetTraceLevels();

protected:
	//tBOOL          m_bDbgTracingEnabled;
	//tDWORD         m_nTraceFileMaxLevel, m_nTraceFileMinLevel;
	//tDWORD         m_nTraceDbgMaxLevel, m_nTraceDbgMinLevel;
	HANDLE         m_hTraceFile;
	static void *  g_pTracer;
	HANDLE         m_hMtxFileAccess;


	tDWORD	m_nTraceFileMinLevel;
	tDWORD	m_nTraceDebugMinLevel;
	tDWORD	m_nTraceFileMaxLevel;
	tDWORD	m_nTraceDebugMaxLevel;
	
	tBOOL	m_nTraceUseDbgInfo;
	tBOOL	m_nTraceFileEnable;
	tBOOL	m_nTraceDebugEnable;

};