/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prts2/taskstorage2.h
 * \author	Andrew Kazachkov
 * \date	21.03.2003 14:43:35
 * \brief	
 * 
 */
#ifndef __KLTASKSTORAGE2_H__
#define __KLTASKSTORAGE2_H__

namespace KLPRTS2
{
	struct ts_info_t
	{
		std::wstring wstrLocation;
		std::wstring wstrProduct;
		std::wstring wstrVersion;
	};

	class KLSTD_NOVTABLE TsSink : public KLSTD::KLBase
	{
	public:
		virtual void OnTasksStorageServer2MethodCall(void* reserved) = 0;
	};

    enum
    {
        CTSF_LOCAL_COPY=1,
        CTSF_CONNECT_TO_AGENT=2,
        CTSF_NOT_PUBLISH_EVENTS=4
    };
};

KLCSKCA_DECL bool KLPRTS2_UpdateTasksStorageServer(
			const std::wstring&			wstrServerObjectID, 
			KLPRTS2::ts_info_t*			pLocations,
			long						nLocations);

KLCSKCA_DECL void KLPRTS2_CreateTasksStorageServer(
			const std::wstring&			wstrServerObjectID, 
			const KLPRCI::ComponentId&	cidMaster, 
			KLPRTS2::ts_info_t*			pLocations,
			long						nLocations,
            AVP_dword                   dwFlags=KLPRTS2::CTSF_LOCAL_COPY,//CTSF_*
			KLPRTS2::TsSink*			pSink=NULL,
            const std::wstring&         wstrCommonId=L"",
			const std::wstring&         wstrHostId=L"" );

KLCSKCA_DECL void KLPRTS2_DeleteTasksStorageServer(const std::wstring& sServerObjectID);

#endif //__KLTASKSTORAGE2_H__
