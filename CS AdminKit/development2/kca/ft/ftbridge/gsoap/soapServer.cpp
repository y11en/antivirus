/* soapServer.cpp
   Generated by gSOAP 2.7.0d from soapheader.h
   Copyright (C) 2001-2004 Genivia, Inc. All Rights Reserved.
   This software is released under one of the following three licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
   See README.txt for further details.
*/
#include "soapH.h"

SOAP_BEGIN_NAMESPACE(soap)

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.7.0d 2006-06-16 13:36:41 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
	unsigned int n = soap->max_keep_alive;
	do
	{	soap_begin(soap);
		if (!--n)
			soap->keep_alive = 0;
		if (soap_begin_recv(soap))
		{	if (soap->error < SOAP_STOP)
				return soap_send_fault(soap);
			soap_closesock(soap);
			continue;
		}
		if (soap_envelope_begin_in(soap)
		 || soap_recv_header(soap)
		 || soap_body_begin_in(soap)
		 || soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap)))
			return soap_send_fault(soap);
		
	} while (soap->keep_alive);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "klftbridge-CreateReceiver"))
		return soap_serve_klftbridge_CreateReceiver(soap);
	if (!soap_match_tag(soap, soap->tag, "klftbridge-Connect"))
		return soap_serve_klftbridge_Connect(soap);
	if (!soap_match_tag(soap, soap->tag, "klftbridge-GetFileInfo"))
		return soap_serve_klftbridge_GetFileInfo(soap);
	if (!soap_match_tag(soap, soap->tag, "klftbridge-GetNextFileChunk"))
		return soap_serve_klftbridge_GetNextFileChunk(soap);
	if (!soap_match_tag(soap, soap->tag, "klftbridge-ReleaseReceiver"))
		return soap_serve_klftbridge_ReleaseReceiver(soap);
	return soap->error = SOAP_NO_METHOD;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_klftbridge_CreateReceiver(struct soap *soap)
{	struct klftbridge_CreateReceiver soap_tmp_klftbridge_CreateReceiver;
	struct klftbridge_CreateReceiverResponse r;
	soap_default_klftbridge_CreateReceiverResponse(soap, &r);
	soap_default_klftbridge_CreateReceiver(soap, &soap_tmp_klftbridge_CreateReceiver);
	soap->encodingStyle = NULL;
	soap_get_klftbridge_CreateReceiver(soap, &soap_tmp_klftbridge_CreateReceiver, "klftbridge-CreateReceiver", NULL);
	if (soap->error)
		return soap->error;
	
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
#ifndef WITH_LEANER
	 || soap_resolve_attachments(soap)
#endif
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = klftbridge_CreateReceiver(soap, soap_tmp_klftbridge_CreateReceiver.useUpdaterFolder, r);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_klftbridge_CreateReceiverResponse(soap, &r);
	soap_begin_count(soap);
	if (soap->mode & SOAP_IO_LENGTH)
	{	soap_envelope_begin_out(soap);
		soap_putheader(soap);
		soap_body_begin_out(soap);
		soap_put_klftbridge_CreateReceiverResponse(soap, &r, "klftbridge-CreateReceiverResponse", "");
		soap_body_end_out(soap);
		soap_envelope_end_out(soap);
	};
	if (soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_klftbridge_CreateReceiverResponse(soap, &r, "klftbridge-CreateReceiverResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_klftbridge_Connect(struct soap *soap)
{	struct klftbridge_Connect soap_tmp_klftbridge_Connect;
	struct klftbridge_ConnectResponse r;
	soap_default_klftbridge_ConnectResponse(soap, &r);
	soap_default_klftbridge_Connect(soap, &soap_tmp_klftbridge_Connect);
	soap->encodingStyle = NULL;
	soap_get_klftbridge_Connect(soap, &soap_tmp_klftbridge_Connect, "klftbridge-Connect", NULL);
	if (soap->error)
		return soap->error;
	
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
#ifndef WITH_LEANER
	 || soap_resolve_attachments(soap)
#endif
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = klftbridge_Connect(soap, soap_tmp_klftbridge_Connect.receiverId, soap_tmp_klftbridge_Connect.toMasterServer, r);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_klftbridge_ConnectResponse(soap, &r);
	soap_begin_count(soap);
	if (soap->mode & SOAP_IO_LENGTH)
	{	soap_envelope_begin_out(soap);
		soap_putheader(soap);
		soap_body_begin_out(soap);
		soap_put_klftbridge_ConnectResponse(soap, &r, "klftbridge-ConnectResponse", "");
		soap_body_end_out(soap);
		soap_envelope_end_out(soap);
	};
	if (soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_klftbridge_ConnectResponse(soap, &r, "klftbridge-ConnectResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_klftbridge_GetFileInfo(struct soap *soap)
{	struct klftbridge_GetFileInfo soap_tmp_klftbridge_GetFileInfo;
	struct klftbridge_GetFileInfoResponse r;
	soap_default_klftbridge_GetFileInfoResponse(soap, &r);
	soap_default_klftbridge_GetFileInfo(soap, &soap_tmp_klftbridge_GetFileInfo);
	soap->encodingStyle = NULL;
	soap_get_klftbridge_GetFileInfo(soap, &soap_tmp_klftbridge_GetFileInfo, "klftbridge-GetFileInfo", NULL);
	if (soap->error)
		return soap->error;
	
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
#ifndef WITH_LEANER
	 || soap_resolve_attachments(soap)
#endif
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = klftbridge_GetFileInfo(soap, soap_tmp_klftbridge_GetFileInfo.receiverId, soap_tmp_klftbridge_GetFileInfo.fileName, r);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_klftbridge_GetFileInfoResponse(soap, &r);
	soap_begin_count(soap);
	if (soap->mode & SOAP_IO_LENGTH)
	{	soap_envelope_begin_out(soap);
		soap_putheader(soap);
		soap_body_begin_out(soap);
		soap_put_klftbridge_GetFileInfoResponse(soap, &r, "klftbridge-GetFileInfoResponse", "");
		soap_body_end_out(soap);
		soap_envelope_end_out(soap);
	};
	if (soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_klftbridge_GetFileInfoResponse(soap, &r, "klftbridge-GetFileInfoResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_klftbridge_GetNextFileChunk(struct soap *soap)
{	struct klftbridge_GetNextFileChunk soap_tmp_klftbridge_GetNextFileChunk;
	struct klftbridge_GetNextFileChunkResponse r;
	soap_default_klftbridge_GetNextFileChunkResponse(soap, &r);
	soap_default_klftbridge_GetNextFileChunk(soap, &soap_tmp_klftbridge_GetNextFileChunk);
	soap->encodingStyle = NULL;
	soap_get_klftbridge_GetNextFileChunk(soap, &soap_tmp_klftbridge_GetNextFileChunk, "klftbridge-GetNextFileChunk", NULL);
	if (soap->error)
		return soap->error;
	
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
#ifndef WITH_LEANER
	 || soap_resolve_attachments(soap)
#endif
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = klftbridge_GetNextFileChunk(soap, soap_tmp_klftbridge_GetNextFileChunk.receiverId, soap_tmp_klftbridge_GetNextFileChunk.fileName, soap_tmp_klftbridge_GetNextFileChunk.startFilePos, soap_tmp_klftbridge_GetNextFileChunk.neededSize, r);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_klftbridge_GetNextFileChunkResponse(soap, &r);
	soap_begin_count(soap);
	if (soap->mode & SOAP_IO_LENGTH)
	{	soap_envelope_begin_out(soap);
		soap_putheader(soap);
		soap_body_begin_out(soap);
		soap_put_klftbridge_GetNextFileChunkResponse(soap, &r, "klftbridge-GetNextFileChunkResponse", "");
		soap_body_end_out(soap);
		soap_envelope_end_out(soap);
	};
	if (soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_klftbridge_GetNextFileChunkResponse(soap, &r, "klftbridge-GetNextFileChunkResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_klftbridge_ReleaseReceiver(struct soap *soap)
{	struct klftbridge_ReleaseReceiver soap_tmp_klftbridge_ReleaseReceiver;
	struct klftbridge_ReleaseReceiverResponse r;
	soap_default_klftbridge_ReleaseReceiverResponse(soap, &r);
	soap_default_klftbridge_ReleaseReceiver(soap, &soap_tmp_klftbridge_ReleaseReceiver);
	soap->encodingStyle = NULL;
	soap_get_klftbridge_ReleaseReceiver(soap, &soap_tmp_klftbridge_ReleaseReceiver, "klftbridge-ReleaseReceiver", NULL);
	if (soap->error)
		return soap->error;
	
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
#ifndef WITH_LEANER
	 || soap_resolve_attachments(soap)
#endif
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = klftbridge_ReleaseReceiver(soap, soap_tmp_klftbridge_ReleaseReceiver.receiverId, r);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_klftbridge_ReleaseReceiverResponse(soap, &r);
	soap_begin_count(soap);
	if (soap->mode & SOAP_IO_LENGTH)
	{	soap_envelope_begin_out(soap);
		soap_putheader(soap);
		soap_body_begin_out(soap);
		soap_put_klftbridge_ReleaseReceiverResponse(soap, &r, "klftbridge-ReleaseReceiverResponse", "");
		soap_body_end_out(soap);
		soap_envelope_end_out(soap);
	};
	if (soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_klftbridge_ReleaseReceiverResponse(soap, &r, "klftbridge-ReleaseReceiverResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	soap_closesock(soap);
	return SOAP_OK;
}

SOAP_END_NAMESPACE(soap)

/* end of soapServer.cpp */