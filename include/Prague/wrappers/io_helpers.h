/*
*/

#if !defined(__KL_IO_HELPERS_H)
#define __KL_IO_HELPERS_H
///////////////////////////////////////////////////////////////////////////////
#include "prague.hpp"
#include "io.hpp"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_receiv.h>
#include <Prague/plugin/p_tempfile.h>
#include <Prague/plugin/p_win32_nfio.h>
///////////////////////////////////////////////////////////////////////////////
namespace prague
{
///////////////////////////////////////////////////////////////////////////////
typedef unsigned long (*fn_msg_receiver)(hOBJECT, unsigned long, unsigned long, hOBJECT
										 , hOBJECT, hOBJECT, void*, unsigned long*);
///////////////////////////////////////////////////////////////////////////////
inline hIO create_temp_io()
{
	
	prague::pr_remote_t api(prague::pr_remote_api());
	prague::hROOT hRoot(0);
	tERROR err(api.GetRoot(&hRoot));
	if(PR_FAIL(err) || !hRoot)
		std::runtime_error("invalid hROOT handle");
	prague::hIO hio;
	err = hRoot->sysCreateObjectQuick(reinterpret_cast<hOBJECT*>(&hio), IID_IO, PID_TEMPIO);
	if(PR_FAIL(err) || !hio)
		std::runtime_error("invalid hIO handle");
	return hio;
}
///////////////////////////////////////////////////////////////////////////////
inline hIO open_native_io(char const* fname, unsigned long flags)
{
	prague::pr_remote_t api(prague::pr_remote_api());
	prague::hROOT hRoot(0);
	tERROR error(api.GetRoot(&hRoot));
	if(PR_FAIL(error) || !hRoot)
		std::runtime_error("invalid hROOT handle");
	prague::hIO hio;
	if(PR_FAIL(CALL_SYS_ObjectCreate(hRoot, &hio, IID_IO, PID_NATIVE_FIO, 0)))
		throw std::runtime_error("can not create \'native io\'");
	prague::IO<char> io(hio);
	CALL_SYS_PropertySetDWord(io.get(), pgOBJECT_ORIGIN, OID_GENERIC_IO);
	CALL_SYS_PropertySetDWord(io.get(), pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
	CALL_SYS_PropertySetDWord(io.get(), pgOBJECT_ACCESS_MODE, flags);
	CALL_SYS_PropertySetStr(io.get(), 0, pgOBJECT_FULL_NAME, const_cast<char*>(fname), lstrlen(fname) + 1, cCP_ANSI);
	if(PR_FAIL(error = CALL_SYS_ObjectCreateDone(io.get())))
	{
		char buf[0x80] = { 0 };
		wsprintfA(buf, "create \'native io\' was failed with code 0x%08x", error);
		throw std::runtime_error(buf);
	}
	return io.release();
}
///////////////////////////////////////////////////////////////////////////////
inline hOBJECT create_msg_receiver(fn_msg_receiver fn_msg, void* p = 0)
{
	prague::pr_remote_t api(prague::pr_remote_api());
	prague::hROOT hRoot(0);
	tERROR err(api.GetRoot(&hRoot));
	if(PR_FAIL(err) || !hRoot)
		std::runtime_error("invalid hROOT handle");
	prague::hOBJECT hObj;
	err = hRoot->sysCreateObject(&hObj, IID_MSG_RECEIVER, PID_ANY, SUBTYPE_ANY);
	if(PR_FAIL(err) || !hObj)
		std::runtime_error("invalid MSG_RECEIVER handle");
	hObj->propSetDWord(pgRECEIVE_PROCEDURE, reinterpret_cast<unsigned long>(fn_msg));
	hObj->propSetPtr(pgRECEIVE_CLIENT_ID, p);
	hObj->sysCreateObjectDone();
	return hObj;
}
///////////////////////////////////////////////////////////////////////////////
}//namesapce prague
///////////////////////////////////////////////////////////////////////////////
#endif  // !defined(__KL_IO_HELPERS_H)
