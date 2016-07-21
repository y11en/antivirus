#ifndef __AVPAGENT_EXE_PATCHINFO_H
#define __AVPAGENT_EXE_PATCHINFO_H

#pragma once

#include <string>
#include <vector>
#include "../../../cciface/BaseMarshal.h"
#include "../../Common/Pid.h"

//! Значение ApplicationId для разных продуктов.
#define APPLICATION_ID_PERSONAL _T("Personal")
#define APPLICATION_ID_PERSONAL_PRO _T("Personal Pro")
#define APPLICATION_ID_WORKSTATION _T("Workstation")
#define APPLICATION_ID_ONSITESCANNER _T("OnSiteScanner")

//! Информация об установке патча.
struct CPatchInfo
{
	CPatchInfo()
		: m_bAskUser(true), m_bUrgent(false)
	{
	}

	//! Абсолютный путь к патчу.
	std::string m_sPath;
	//! Версии продукта для которых применим патч.
	std::vector<std::string> m_FromAppVersion;
	//! Версия продукта после применения патча.
	std::string m_sToAppVersion;
	//! Комментарий к патчу.
	std::string m_sComment;
	//! Флаг, спрашивать ли пользователя перед установкой.
	bool m_bAskUser;
	//! Флаг, срочный патч.
	bool m_bUrgent;

	PoliType& MarshalImpl(PoliType& pt)
	{
		const int nThisVersion = 2;
		int nVersion = nThisVersion;
		pt << nVersion;
		switch (nVersion)
		{
		case 2: pt << m_sComment << m_bAskUser << m_bUrgent;
		case 1: pt << m_sPath << m_FromAppVersion << m_sToAppVersion;
			break;
		default: pt.set_error_(PoliType::eDataMoreRecent);
			break;
		}
		return pt;
	}
};

IMPLEMENT_MARSHAL(CPatchInfo);

//! Список патчей.
struct CPatchList: public CCBaseMarshal<PID_PATCH_LIST>
{
	CPatchList()
	{
	}

	typedef std::vector<CPatchInfo> CPatches;
	CPatches m_Patches;
	CSysTimeSer m_tmLastRebootPatchInstallTime; //!< Время установки последнего патча, потребовавшего перезапуск.

	PoliType& MarshalImpl(PoliType& pt)
	{
		const int nThisVersion = 2;
		int nVersion = nThisVersion;
		pt << nVersion;
		switch(nVersion)
		{
		case 2: pt << m_tmLastRebootPatchInstallTime;
		case 1: pt << m_Patches;
			break;
		default: pt.set_error_(PoliType::eDataMoreRecent); break;
		}
		return pt;
	}
};

IMPLEMENT_MARSHAL(CPatchList);

#endif