#if !defined(__ENGINE_H)
#define __ENGINE_H

#include <boost/shared_ptr.hpp>
#include "_aux.h"

#include <Mail/structs/s_httpscan.h>
#include <ParentalControl/structs/s_urlflt.h>

#include "query.hpp"

#include "Tracer.h"

struct pr_novtable cEngineBase : public cObj
{
public:
	virtual tERROR pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen ) = 0;
	bool VerifyCheckedObjects (vector<hIO>* etalon);
	virtual ~cEngineBase ()
	{
		//закрыть io
	}

	enum URLFilterEnum
	{
		filter_WEBAV = 0,
		filter_APHI,
		filter_AD,
		filter_PARC,
		filter_count
	} ;

	class FilterQuery: public CommonQuery<errACCESS_DENIED, URLFilterEnum, std::string>
	{
	public:

		tERROR Process (tDWORD msg_cls, tPTR buff);

	};

	class AVSQuery
	{
	public:

		AVSQuery();
		void Activate ();
		void Deactivate ();
		void SetScanHTTP_options (const cDetectObjectInfo& me_DetectInfo, const cScanProcessInfo& me_ScanInfo);
		tERROR HTTPScan_process (cHTTPScanProcessParams* http_params);

	private:
		bool bAVSProcess;
		HANDLE hAVSRequestReceived;
		HANDLE hAVSRequestProcessed;
		cDetectObjectInfo m_DetectInfo;
		cScanProcessInfo  m_ScanInfo;
	};


	FilterQuery& GetFilterQuery();
	AVSQuery& GetAVSQuery();

protected:

	vector<hIO> objects;
	FilterQuery m_filterQuery;
	AVSQuery m_AVSQuery;

};

class cStreamEngine: public cEngineBase
{
public:

	virtual tERROR pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );
	cStreamEngine (): counter(1)
	{

	};

private:

	struct _object
	{
		enum enum_state
		{
			state_not_initialized,
			state_checking,
			state_finished
		};

		_object(): state (state_not_initialized), io (NULL)
		{
			start_processing ();
		};

		~_object()
		{
			//io->sysCloseObject()
		};

		void start_processing ()
		{
			prague::IO<char> obj_ptr(prague::create_temp_io());
			io = obj_ptr.release();
			state = state_checking;
		};

		void stop_processing ()
		{
			state = state_finished;

		};

		void insert_data (hIO io_source)
		{
			_ASSERTE (state == state_checking);

			tDWORD result;
			tQWORD size_source = 0;
			io_source->GetSize ( &size_source, IO_SIZE_TYPE_EXPLICIT);
			tQWORD size_dest = 0;
			io->GetSize ( &size_dest, IO_SIZE_TYPE_EXPLICIT);

			if (!size_source) return;
			vector<char> buffer;
			buffer.reserve (size_source);
			buffer.resize(1);
			char* buf = &buffer[0];

			io_source->SeekRead (&result, 0, buf, size_source);
			io->SeekWrite (&result, size_dest, buf, size_source);

		};

		hIO Get_IO ()
		{
			return io;
		}

		bool isEmpty ()
		{
			tQWORD size;
			io->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
			return size == 0;
		}

	private:
		enum_state state;
		hIO io;

	};

	map<int, boost::shared_ptr<_object>  > objects_map;
	int counter;
};

class cThirdEngine: public cEngineBase
{
public:

	virtual tERROR pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );

};



#endif