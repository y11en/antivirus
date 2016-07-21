#include "stdafx.h"
#include "rpc_report.h"
#include <iface/i_report.h>


tERROR pr_call Local_Report_GetRecordCount( hREPORT _this, tDWORD* pdwRecords ) {		// -- ;
	PR_PROXY_CALL( Report_GetRecordCount( PR_THIS_PROXY, pdwRecords ) );
}
tERROR pr_call Local_Report_GetRecord( hREPORT _this, tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp ) {		// -- ;
	PR_PROXY_CALL( Report_GetRecord( PR_THIS_PROXY, result, dwIndex, (tCHAR*)pBuffer, dwSize, pdtTimestamp ) );
}
tERROR pr_call Local_Report_AddRecord( hREPORT _this, const tVOID* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex ) {		// -- ;
	PR_PROXY_CALL( Report_AddRecord( PR_THIS_PROXY, (const tCHAR*)pBuffer, dwSize, pdwRecordIndex ) );
}
tERROR pr_call Local_Report_UpdateRecord( hREPORT _this, tDWORD dwIndex, const tVOID* pBuffer, tDWORD dwSize ) {		// -- ;
	PR_PROXY_CALL( Report_UpdateRecord( PR_THIS_PROXY, dwIndex, (const tCHAR*)pBuffer, dwSize ) );
}
tERROR pr_call Local_Report_DeleteRecord( hREPORT _this, tDWORD dwIndex ) {		// -- ;
	PR_PROXY_CALL( Report_DeleteRecord( PR_THIS_PROXY, dwIndex ) );
}
tERROR pr_call Local_Report_Compact( hREPORT _this ) {		// -- ;
	PR_PROXY_CALL( Report_Compact( PR_THIS_PROXY ) );
}
iReportVtbl report_iface = {
	Local_Report_GetRecordCount,
	Local_Report_GetRecord,
	Local_Report_AddRecord,
	Local_Report_UpdateRecord,
	Local_Report_DeleteRecord,
	Local_Report_Compact,
}; // 

PR_IMPLEMENT_PROXY(IID_REPORT, report_iface)
