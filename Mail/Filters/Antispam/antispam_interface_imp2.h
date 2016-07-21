#ifndef _ANTISPAM_INTERFACE_IMP_2_H_
#define _ANTISPAM_INTERFACE_IMP_2_H_

#include "htmlfilter.h"
#include "EncodingTables/charsetdecoder.h"
#include "EncodingTables/encodingtables.h"
#include "textfilter.h"
#ifndef USE_FISHER
	#include "bayesfilter.h"
#else
	#include "bfilter.h"
#endif//USE_FISHER
#include "toupper.h"
#include "whitelist.h"

#include <Prague/pr_loadr.h>
#include <Prague/pr_remote.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_objptr.h>

#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_msoe.h>
#include <Mail/structs/s_antispam.h>

#define PRAGUE_DEFINED
#include "antispam_interface2.h"

namespace CAntispamCaller
{
	struct CObjectToCheck_hObject : public CObjectToCheck
	{
	public:
		CObjectToCheck_hObject();
		CObjectToCheck_hObject(hOBJECT obj);
		virtual ~CObjectToCheck_hObject();
		virtual tERROR PropertyGet( 
			OUT tDWORD* out_size, 
			IN tPROPID prop_id, 
			IN tPTR buffer, 
			IN tDWORD size 
			);
		hOBJECT GetObject();
	protected:
		hOBJECT m_obj;
		bool m_bReadyToWork;
	};

	struct CObjectToCheck_hIO : public CObjectIOToCheck, CObjectToCheck_hObject
	{
	public:
		CObjectToCheck_hIO();
		CObjectToCheck_hIO(hOBJECT obj);
		CObjectToCheck_hIO(CObjectToCheck* pobj);
		virtual ~CObjectToCheck_hIO();
		tERROR SeekRead( 
			OUT tDWORD* out_size, 
			IN tQWORD offset, 
			IN tPTR buffer, 
			IN tDWORD size 
			);
		tERROR GetSize( OUT tQWORD* out_size );
		hIO GetIO();
	
	private:
		hIO m_io;
	};

	struct CObjectToCheck_hOS : public CObjectSystemToCheck, CObjectToCheck_hObject
	{
	public:
		CObjectToCheck_hOS(hOS os);
		CObjectToCheck_hOS(CObjectToCheck* pobj);
		virtual ~CObjectToCheck_hOS();
		tERROR PtrCreate(OUT tUINT* pObjPtr);
		tERROR ObjectCreate(IN tUINT pObjPtr, OUT CObjectToCheck** ppObject);
		tERROR ObjectClose(IN tUINT pObjPtr, IN CObjectToCheck* pObject);
		tERROR PtrClose(IN tUINT pObjPtr);
		hOS GetOS();
	
	private:
		hOS m_os;
		bool m_bOSCreatedInternal;
	};
};


namespace CAntispamImplementer
{
	class CKLAntispamFilter : public CAntispamFilter
	{
	public:
		CKLAntispamFilter();
		virtual ~CKLAntispamFilter();

		struct InitParams_t
		{
			cAntispamSettings* pAntispamSettings;
			hREGISTRY persistance_storage;
			hOBJECT hAntispamTask;
		};
		typedef cSerializable ProcessParams_t;
		typedef cAntispamStatistics Statistics_t;
		typedef cAntispamStatisticsEx ProcessStatistics_t;
		struct TrainParams_t : public cSerializable
		{
			cAntispamTrainParams::Action_t Action; //!< Действие при тренировке
			CKLAntispamFilter* pFilter;            //!< Второе ядро для действия MergeDatabase
		};
		tERROR Init(CAntispamCaller::CMemoryAlloc* pMemoryAllocator, InitParams_t* pInitParams);
		tERROR GetStatistics( IN Statistics_t* p_Statistics );

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Process
		//! \brief			:	Если вдвинуть статистику, то процессирование может быть 
		//!						построено не на объекте, а на данных, содержащихся в статистике
		//! \return			: tERROR 
		//! \param          : IN  CAntispamCaller::CObjectSystemToCheck* pObject
		//! \param          : IN  ProcessParams_t* pProcessParams
		//! \param          : OUT long double* pResult
		//! \param          : IN  tDWORD dwTimeOut
		//! \param          : IN OUT ProcessStatistics_t* pProcessStat
		tERROR Process(
			IN  CAntispamCaller::CObjectSystemToCheck* pObject, 
			IN  ProcessParams_t* pProcessParams, 
			OUT long double* pResult, 
			IN  tDWORD dwTimeOut,
			IN OUT ProcessStatistics_t* pProcessStat
			);
		tERROR Train(
			IN CAntispamCaller::CObjectSystemToCheck* pObject, 
			IN TrainParams_t* pTrainParams,
			IN  tDWORD dwTimeOut,
			IN OUT ProcessStatistics_t* pProcessStat
			);

	public:
		inline CAddressList* GetAddressList() const { return m_pAddressList; };
		inline CBayesFilter* GetBayesFilter() const { return m_pBayesFilter; };
		
	private:
		Statistics_t m_statistics;
		InitParams_t m_settings;
		CHtmlFilter* m_pHtmlFilter;
		CCharsetDecoder* m_pDecoder;
		CTextFilter* m_pTextFilter;
		CBayesFilter* m_pBayesFilter;
		CAddressList* m_pAddressList;
		tERROR FindAndFilterBody(
			IN  CAntispamCaller::CObjectSystemToCheck* pObject, 
			IN  ProcessParams_t* pProcessParams, 
			OUT long double* pResult,
			IN  tDWORD dwTimeOut,
			IN OUT ProcessStatistics_t* pProcessStat,
			IN TrainParams_t* pTrainParams
			);

		bool IsHTML( hIO hBody );
		bool IsRTF( hIO hBody );
		bool IsRTFHTML( hIO hBody );
		bool IsXMacCyrillic( hOS hMessage );
		void CopyBodyToText( hIO hBody, string& szBody, tQWORD qwSize = -1 );
		void UpdateSysTokens(CHtmlFilter::results_t& results_html, CTextFilter::results_t& results_text);
		tERROR GetStringData(hOBJECT hMessage, tPROPID prop_id, cStringObj& szData);
		tERROR GetStringData(hOBJECT hMessage, tPROPID prop_id, string& szData);
		tERROR FilterBody( 
			IN hOS hMessage,
			IN hIO hBody, 
			IN bool bTrain, 
			IN long double* pS, 
			IN cAntispamTrainParams::Action_t TrainAction,
			IN OUT ProcessStatistics_t* pProcessStat
			);

		tDWORD AsBwListFindPattern(const cAsBwListRules &patterns,
									const char *text,
									tDWORD dwMaxLevel,
									ProcessStatistics_t* pProcessStat, tDWORD dwStatAlg);

		void PutAddressesInList(const std::string& addresses, const cStringObj& userName, address_type_t type, CAddressList* pList);
	
//	private:
//		tERROR CheckTrainingWizard(tDWORD dwPID, tBOOL bIsUnknownTrainState);
	};

}

#endif//_ANTISPAM_INTERFACE_IMP_2_H_