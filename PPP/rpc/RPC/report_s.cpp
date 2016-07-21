#include "stdafx.h"
#include "rpc_report.h"
#include <iface/i_report.h>

#define PR_REPORT_STUB		((hREPORT)PR_THIS_STUB)

extern "C" tERROR PRReport_GetRecordCount( handle_t IDL_handle, rpcReport _this, tDWORD* pdwRecords ) {
	return CALL_Report_GetRecordCount( PR_REPORT_STUB, pdwRecords );
}
extern "C" tERROR PRReport_GetRecord( handle_t IDL_handle, rpcReport _this, tDWORD* result, tDWORD dwIndex, tCHAR* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp ) {
	return CALL_Report_GetRecord( PR_REPORT_STUB, result, dwIndex, pBuffer, dwSize, pdtTimestamp );
}
extern "C" tERROR PRReport_AddRecord( handle_t IDL_handle, rpcReport _this, const tCHAR* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex ) {
	return CALL_Report_AddRecord( PR_REPORT_STUB, pBuffer, dwSize, pdwRecordIndex );
}
extern "C" tERROR PRReport_UpdateRecord( handle_t IDL_handle, rpcReport _this, tDWORD dwIndex, const tCHAR* pBuffer, tDWORD dwSize ) {
	return CALL_Report_UpdateRecord( PR_REPORT_STUB, dwIndex, pBuffer, dwSize );
}
extern "C" tERROR PRReport_DeleteRecord( handle_t IDL_handle, rpcReport _this, tDWORD dwIndex ) {
	return CALL_Report_DeleteRecord( PR_REPORT_STUB, dwIndex );
}
extern "C" tERROR PRReport_Compact( handle_t IDL_handle, rpcReport _this ) {
	return CALL_Report_Compact( PR_REPORT_STUB );
}
PR_IMPLEMENT_STUB(IID_REPORT, PRrpc_report_ServerIfHandle)
