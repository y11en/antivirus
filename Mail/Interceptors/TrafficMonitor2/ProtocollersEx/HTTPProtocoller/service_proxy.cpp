/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	service_proxy.cpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	22.06.2005 18:01:55
*		
*		
*/
#define NOMINMAX
#pragma warning(disable: 4786)

#include <Prague/prague.h>
#include <Mail/structs/s_httpscan.h>
#include <ParentalControl/structs/s_urlflt.h>

#include <windows.h>
#include <Prague/wrappers/io_helpers.h>
#include <http/http.h>

#include "avp3_engine_proxy.h"
#include "stream_engine_proxy.h"
#include "service_observer.h"


///////////////////////////////////////////////////////////////////////////////
//extern declaration need to send IO on process to business logic
extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMailerPID;
extern tPROPID g_propVirtualName;
extern tPROPID g_propContentType;
extern tPROPID g_propStartScan;
extern tPROPID g_propMessageCheckOnly;
extern tPROPID g_propMsgDirection;
///////////////////////////////////////////////////////////////////////////////
http::generic_service_proxy::generic_service_proxy(hOBJECT service)
										: service_(service)
{
}
///////////////////////////////////////////////////////////////////////////////
http::generic_service_proxy::generic_service_proxy(http::generic_service_proxy const& other)
										: service_(other.service_)
{
}
///////////////////////////////////////////////////////////////////////////////
bool http::generic_service_proxy::antiphishing(unsigned int client_id, char const* url) const
{
	tERROR error;
	cAntiPhishingCheckRequest params;
	params.m_sURL = url;
	params.m_nPID = client_id;
	error = service_->sysSendMsg(pmc_URL_FILTER_ANTIPHISHING
								, msg_URL_FILTER_CHECK_URL
								, 0
								, &params
								, SER_SENDMSG_PSIZE);
	PR_TRACE((service_, prtIMPORTANT, "http\tantiphishing result %terr", error));
	return errACCESS_DENIED != error;
}
///////////////////////////////////////////////////////////////////////////////
bool http::generic_service_proxy::webav(unsigned int client_id, char const* url) const
{
	tERROR error;
	cUrlFilteringRequest request;
	request.m_sURL = url;
	request.m_nPID = client_id;
	error = service_->sysSendMsg(pmc_URL_FILTER_WEBAV
		, msg_URL_FILTER_CHECK_URL
		, 0
		, &request
		, SER_SENDMSG_PSIZE);
	PR_TRACE((service_, prtIMPORTANT, "httpprotocoller\twebav result %terr", error));
	return errACCESS_DENIED != error;
}
///////////////////////////////////////////////////////////////////////////////
bool http::generic_service_proxy::adblock(unsigned int client_id, char const* url) const
{
	tERROR error;
	cAntiPhishingCheckRequest params;
	params.m_sURL = url;
	params.m_nPID = client_id;
	error = service_->sysSendMsg(pmc_URL_FILTER_ADBLOCK
								, msg_URL_FILTER_CHECK_URL
								, 0
								, &params
								, SER_SENDMSG_PSIZE);
	PR_TRACE((service_, prtIMPORTANT, "http\tadblock result %terr", error));
	return errACCESS_DENIED != error;
}
///////////////////////////////////////////////////////////////////////////////

///
/// Parental control analyze
///
bool http::generic_service_proxy::parctl
	( 
		unsigned int client_id,
        hIO data,
		const message* msg,
        char const* url,
        service_observer* observer,
        int direction 
	) const
{
	tERROR error;
	cUrlFilteringRequest request;
	request.m_sURL = url;
	request.m_nPID = client_id;

	if ( msg != NULL )
	{
		request.init( msg );
	}

	error = service_->sysSendMsg(pmc_URL_FILTER_PARCTL
		, msg_URL_FILTER_CHECK_URL
		, data
		, &request
		, SER_SENDMSG_PSIZE);
	PR_TRACE((service_, prtIMPORTANT, "httpprotocoller\tparctl result %terr", error));
	return errACCESS_DENIED != error;
}
///////////////////////////////////////////////////////////////////////////////
http::avp3_engine_proxy::avp3_engine_proxy(hOBJECT service)
							: generic_service_proxy(service)
							, receiver_(prague::create_msg_receiver(&avp3_engine_proxy::scan_processing))
{
}
///////////////////////////////////////////////////////////////////////////////
http::avp3_engine_proxy::avp3_engine_proxy(http::avp3_engine_proxy const& other)
							: generic_service_proxy(other)
							, receiver_(prague::create_msg_receiver(&avp3_engine_proxy::scan_processing))
{
}
///////////////////////////////////////////////////////////////////////////////
http::avp3_engine_proxy::~avp3_engine_proxy()
{
	if(receiver_)
	{
		receiver_->sysCloseObject();
		receiver_ = 0;
	}
}
///////////////////////////////////////////////////////////////////////////////
http::antivirus_result http::avp3_engine_proxy::antivirus(unsigned int client_id, hIO data
														, char const* url
														, http::service_observer* observer
														, int dir
														, bool is_gzip) const
{
	if(receiver_ && observer)
	{
		receiver_->propSetDWord(g_propStartScan, clock());
		receiver_->propSetDWord(g_propMsgDirection, dir);
		receiver_->propSetPtr(pgRECEIVE_CLIENT_ID, observer);
		receiver_->sysRegisterMsgHandler(pmc_PROCESSING, rmhDECIDER, data, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	}
	cHTTPScanProcessParams params;
	data->propSetBool(g_propMessageIsIncoming, cTRUE);
	data->propSetDWord(g_propMailerPID, client_id);
	data->propSetStr(0, g_propVirtualName, const_cast<char*>(url));
	service_->sysSendMsg(pmc_HTTPSCAN_PROCESS, 0, data, &params, SER_SENDMSG_PSIZE);
	antivirus_result r(object_is_ok);
	r.result = static_cast<avs_result>(params.m_ScanProcessInfo.m_nProcessStatusMask);
	if((r.result & (object_was_deleted | object_detected | object_some_skipped))
		&& !params.m_DetectObjectInfo.m_strDetectName.empty())
	{
		char const* vname = params.m_DetectObjectInfo.m_strDetectName.c_str(cCP_ANSI);
		r.vir_name = vector_ptr(new std::vector<char>(vname, vname + strlen(vname) + 1));
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////
unsigned long http::avp3_engine_proxy::scan_processing(hOBJECT _this, unsigned long msg_cls
												, unsigned long msg_id, hOBJECT obj, hOBJECT ctx
												, hOBJECT receive_point, void* pbuff
												, unsigned long* blen)
{
	if(msg_cls == pmc_PROCESSING && pm_PROCESSING_YIELD == msg_id)
	{
		clock_t scan_start(_this->propGetDWord(g_propStartScan));
		clock_t scan_time(scan_start);
		service_observer* observer = static_cast<service_observer*>(_this->propGetPtr(pgRECEIVE_CLIENT_ID));
		int direction = _this->propGetDWord(g_propMsgDirection);
		if(observer && !observer->processing_yield(&scan_time, direction))
				return errOPERATION_CANCELED;
		if(scan_time != scan_start)
			_this->propSetDWord(g_propStartScan, scan_time);
	}
	return errOK;
}
///////////////////////////////////////////////////////////////////////////////
http::stream_engine_proxy::stream_engine_proxy(hOBJECT service)
							: generic_service_proxy(service)
							, stm_ctx_(0)
{
}
///////////////////////////////////////////////////////////////////////////////
http::stream_engine_proxy::stream_engine_proxy(http::stream_engine_proxy const& other)
							: generic_service_proxy(other)
							, stm_ctx_(0)
{
}
///////////////////////////////////////////////////////////////////////////////
http::stream_engine_proxy::~stream_engine_proxy()
{
	end_antivirus();
}
///////////////////////////////////////////////////////////////////////////////
http::antivirus_result http::stream_engine_proxy::antivirus(unsigned int client_id, hIO data
														, char const* url
														, http::service_observer* observer
														, int direction
														, bool is_gzip) const
{
	antivirus_result r(object_is_ok);
	tQWORD nSize = 0;
	if(stm_ctx_ && data && PR_SUCC(data->GetSize(&nSize, IO_SIZE_TYPE_EXPLICIT)) && nSize > 0)
	{
		cHTTPScanProcessParams params;
		params.m_nCheckAsStream = ss_PACKET_PROCESS;
		params.m_StreamCtx = reinterpret_cast<tPTR>(stm_ctx_);
		params.m_bGZIPped = is_gzip;
		data->propSetBool(g_propMessageIsIncoming, cTRUE);
		data->propSetDWord(g_propMailerPID, client_id);
		data->propSetStr(0, g_propVirtualName, const_cast<char*>(url));
		service_->sysSendMsg(pmc_HTTPSCAN_PROCESS, 0, data, &params, SER_SENDMSG_PSIZE);
		r.result = static_cast<avs_result>(params.m_ScanProcessInfo.m_nProcessStatusMask);
		if((r.result & (object_was_deleted | object_detected | object_some_skipped))
			&& !params.m_DetectObjectInfo.m_strDetectName.empty())
		{
			char const* vname = params.m_DetectObjectInfo.m_strDetectName.c_str(cCP_ANSI);
			r.vir_name = vector_ptr(new std::vector<char>(vname, vname + strlen(vname) + 1));
		}
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////
void http::stream_engine_proxy::begin_antivirus()
{
	if(!stm_ctx_)
	{
		cHTTPScanProcessParams params;
		params.m_nCheckAsStream = ss_PACKET_BEGIN;
		service_->sysSendMsg(pmc_HTTPSCAN_PROCESS, 0, 0, &params, SER_SENDMSG_PSIZE);
		stm_ctx_ = reinterpret_cast<void*>(params.m_StreamCtx);
	}
}
///////////////////////////////////////////////////////////////////////////////
void http::stream_engine_proxy::end_antivirus()
{
	if(stm_ctx_)
	{
		cHTTPScanProcessParams params;
		params.m_nCheckAsStream = ss_PACKET_END;
		params.m_StreamCtx = reinterpret_cast<tPTR>(stm_ctx_);
		service_->sysSendMsg(pmc_HTTPSCAN_PROCESS, 0, 0, &params, SER_SENDMSG_PSIZE);
		stm_ctx_ = 0;
	}
}
///////////////////////////////////////////////////////////////////////////////