#include "engine.h"
#include <iostream>

// ----------------------------- BASE -------------------------------------------------------------//

bool cEngineBase::VerifyCheckedObjects (vector<hIO>* etalon)
{	//objects.erase(objects.begin());
	LOG_CALL("Verifying checked objects ...\n");

	size_t min_count = std::min (etalon->size(), objects.size());
	bool bResult = true;
	for (int i = 0; i < min_count; ++i)
	{
		char buf[32];
		LOG_CALL3("Verifying object ",itoa(i+1, buf, 10),"\n");
		if (!aux::isIoEquals (etalon->at(i), objects.at(i))) bResult = false;
	}

	if (etalon->size() != objects.size())
	{
		char buf[32], buf2[32];
		LOG_CALL5("\tIncorrect number of objects...",itoa(etalon->size(), buf, 10),"-",itoa(objects.size(), buf, 10),"\n");
		return false;
	}

	return bResult;
}

/////////////////////////////////////////////////////
/*void cEngineBase::FilterQuery::GetRequest (FilterEnum& type, std::string& url) 
{
	WaitForSingleObject(hRequestReceived, INFINITE);

	switch (url_filter_type)
	{
	case pmc_URL_FILTER_WEBAV: type = filter_WEBAV; break;
	case pmc_URL_FILTER_ADBLOCK: type = filter_AD; break;
	case pmc_URL_FILTER_ANTIPHISHING: type = filter_APHI; break;
	case pmc_URL_FILTER_PARCTL: type = filter_PARC; break;
	default: _ASSERTE (!"Unknown type");
	}
	url = url_filter;
}*/

tERROR cEngineBase::FilterQuery::Process (tDWORD msg_cls, tPTR buff)
{
	if (bActive)
	{
		switch (msg_cls)
		{
		case pmc_URL_FILTER_ADBLOCK:

		case pmc_URL_FILTER_ANTIPHISHING:
			m_arg2 = ((cAntiPhishingCheckRequest*)buff)->m_sURL.c_str(cCP_ANSI);
			break;
		case pmc_URL_FILTER_WEBAV:
		case pmc_URL_FILTER_PARCTL:
			m_arg2 = ((cUrlFilteringRequest*)buff)->m_sURL.c_str(cCP_ANSI);
			break;
		default:
			_ASSERTE (!"Unknown type");
		}

		switch (msg_cls)
		{
		case pmc_URL_FILTER_ADBLOCK:
			m_arg1 = filter_AD;
			break;
		case pmc_URL_FILTER_ANTIPHISHING:
			m_arg1 = filter_APHI;
			break;
		case pmc_URL_FILTER_WEBAV:
			m_arg1 = filter_WEBAV;
			break;
		case pmc_URL_FILTER_PARCTL:
			m_arg1 = filter_PARC;
			break;
		};

		SetEvent (hRequestReceived);
		DWORD waitResult = WaitForMultipleObjects(2, resEvents, false, INFINITE);
		_ASSERTE (waitResult>=WAIT_OBJECT_0 && waitResult<=(WAIT_OBJECT_0 + 1));
		return (waitResult == WAIT_OBJECT_0) ? errOK : errACCESS_DENIED;
	}
	else return errOK;
}

//////////////////////////////////////////////////////////////

cEngineBase::FilterQuery& cEngineBase::GetFilterQuery()
{
	return m_filterQuery;
};


cEngineBase::AVSQuery::AVSQuery (): bAVSProcess(false)
{

};

void cEngineBase::AVSQuery::Activate ()
{
	hAVSRequestReceived = CreateEvent (NULL, false, false, NULL);
	hAVSRequestProcessed = CreateEvent (NULL, false, false, NULL);

	bAVSProcess = true;
};

void cEngineBase::AVSQuery::Deactivate ()
{
	CloseHandle (hAVSRequestReceived);
	CloseHandle (hAVSRequestProcessed);

	bAVSProcess = false;
};


void cEngineBase::AVSQuery::SetScanHTTP_options (const cDetectObjectInfo& me_DetectInfo, const cScanProcessInfo& me_ScanInfo)
{
	m_DetectInfo = me_DetectInfo;
	m_ScanInfo = me_ScanInfo;
	SetEvent (hAVSRequestReceived);
}

tERROR cEngineBase::AVSQuery::HTTPScan_process (cHTTPScanProcessParams* http_params)
{
	if (bAVSProcess)
	{//расширенные диагнозы

		WaitForSingleObject (hAVSRequestReceived, INFINITE);

		http_params->m_ScanProcessInfo.m_nProcessStatusMask = m_ScanInfo.m_nProcessStatusMask;
		http_params->m_DetectObjectInfo.m_strDetectName = m_DetectInfo.m_strDetectName;

		SetEvent (hAVSRequestProcessed);

	}
	else
	{
		http_params->m_ScanProcessInfo.m_nProcessStatusMask = object_is_ok;
		http_params->m_DetectObjectInfo.m_strDetectName = cStringObj ("Virus");
	}

	return errOK_DECIDED;

}

cEngineBase::AVSQuery& cEngineBase::GetAVSQuery()
{
	return m_AVSQuery;
};


// ------------------------------------- THIRD ---------------------------- //

tERROR cThirdEngine::sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen )
{
	if (msg_cls == pmc_URL_FILTER_WEBAV ||
		msg_cls == pmc_URL_FILTER_ADBLOCK ||
		msg_cls == pmc_URL_FILTER_ANTIPHISHING ||
		msg_cls == pmc_URL_FILTER_PARCTL)
		return m_filterQuery.Process (msg_cls, pbuff);


	prague::IO<char> obj_ptr(prague::create_temp_io());
	tQWORD size;
	((hIO)ctx)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
	cIOObj::copy (obj_ptr.get(), (cIO*)ctx, size);
	objects.push_back (obj_ptr.release());

	cHTTPScanProcessParams* http_params = reinterpret_cast<cHTTPScanProcessParams*>(pbuff);
	return m_AVSQuery.HTTPScan_process (http_params);
}


// ------------------------------------- STREAM ---------------------------- //

tERROR cStreamEngine::sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen )
{
	if (msg_cls == pmc_URL_FILTER_WEBAV ||
		msg_cls == pmc_URL_FILTER_ADBLOCK ||
		msg_cls == pmc_URL_FILTER_ANTIPHISHING ||
		msg_cls == pmc_URL_FILTER_PARCTL)
		return m_filterQuery.Process (msg_cls, pbuff);

	cHTTPScanProcessParams* param = reinterpret_cast<cHTTPScanProcessParams*>(pbuff);

	switch (param->m_nCheckAsStream)
	{
	case ss_PACKET_BEGIN:
		{
			objects_map.insert	(std::pair<int, boost::shared_ptr<_object> >
								(counter, boost::shared_ptr<_object>(new _object ())) );
			objects.push_back(objects_map[counter]->Get_IO());
			param->m_StreamCtx = (tPTR)counter;
			counter++;
			break;
		}

	case ss_PACKET_END:
		{
			boost::shared_ptr<_object> obj_ptr = objects_map[(int)param->m_StreamCtx];
			_ASSERTE (obj_ptr.get());
			obj_ptr->stop_processing ();
			//грохнем пустой пакет
			if (obj_ptr->isEmpty())
			{
				objects.erase (find (objects.begin(), objects.end(), obj_ptr->Get_IO()));
				objects_map.erase ((int)param->m_StreamCtx);
			}
			break;
		}
	case ss_PACKET_PROCESS:
		{
			boost::shared_ptr<_object> obj_ptr = objects_map[(int)param->m_StreamCtx];
			_ASSERTE (obj_ptr.get());
			obj_ptr->insert_data ((hIO)ctx);
			return m_AVSQuery.HTTPScan_process (param);
			break;
		}
	default:
		_ASSERTE (!"Mustn't be here");

	}
	return errOK_DECIDED;
}

