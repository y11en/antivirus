#include "heur_install.h"

//
// Service Types (Bit Mask)
//
#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008

#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | \
                                        SERVICE_FILE_SYSTEM_DRIVER | \
                                        SERVICE_RECOGNIZER_DRIVER)

#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
                                        SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_INTERACTIVE_PROCESS    0x00000100

#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | \
                                        SERVICE_ADAPTER | \
                                        SERVICE_DRIVER  | \
                                        SERVICE_INTERACTIVE_PROCESS)

cEventHandlerHeurInstall::cEventHandlerHeurInstall()
{
}

cEventHandlerHeurInstall::~cEventHandlerHeurInstall()
{
	if (m_drivers_path)
		tstrfree(m_drivers_path);
}

bool cEventHandlerHeurInstall::Init()
{
	m_drivers_path = m_pEnvironmentHelper->ExpandEnvironmentStr(_T("%SYSTEMROOT%\\SYSTEM32\\DRIVERS\\*.SYS"));
	if (!m_drivers_path)
		return false;
	tstrupr(m_drivers_path);
	return true;
}
tPid cEventHandlerHeurInstall::iFindProcessByDroppedFile(tcstring file)
{
	tPid* arrPids = NULL;
	size_t nPidCount = 0;
	if (!m_pProcessList->GetSnapshot(&arrPids, &nPidCount))
		return 0;
	tPid ret_pid = 0;
	for (uint32_t i=0; i<nPidCount; i++)
	{
		cProcess pProcess = m_pProcessList->FindProcess(arrPids[i]);
		if (!pProcess)
			continue;
		if (pProcess->m_CreatedImagesList.contain(file))
		{
			ret_pid = pProcess->m_uniq_pid;
			break;
		}
	}
	m_pProcessList->ReleaseSnapshot(&arrPids);
	return ret_pid;
}

void cEventHandlerHeurInstall::OnRegKeyCreatePost(cEvent& event, const cRegKey& key)
{
	return;
	/*bool bService = false;
	if (key.m_root == crHKEY_LOCAL_MACHINE && tstrMatchWithPattern(key.m_name, _T("SYSTEM\\CONTROLSET???\\SERVICES\\*"), false))
	{
		bService = true;
	}
	else if (key.m_root == crHKEY_LOCAL_MACHINE && tstrMatchWithPattern(key.m_name, _T("SYSTEM\\CURRENTCONTROLSET\\SERVICES\\*"), false))
	{
		bService = true;
	}
	if (!bService)
		return;
	tcstring name = tstrrchr(key.m_name, '\\');
	tstring simagepath = tstrdup(_T("%systemroot%\\system32\\drivers\\*.sys"));
	simagepath = tstrappend(simagepath, name+1);
	simagepath = tstrappend(simagepath, _T(".sys"));
	cPath imagepath(m_pEnvironmentHelper, simagepath);
	tstrfree(simagepath);
	m_registered_drivers.add(event.m_pid, imagepath.getFull());
	tPid pid = iFindProcessByDroppedFile(imagepath.getFull());
	if (!pid)
		return;
	cEvent new_event(event, pid);
	return m_pEventMgr->OnInstallDriver(new_event, imagepath);
	*/
}

void cEventHandlerHeurInstall::OnFileCloseModifiedPost(cEvent& event, cFile& file)
{
	tPid new_pid;
	if (!file.isExecutable())
		return;
	if (tstrMatchWithPattern(file.getFull(), m_drivers_path, true))
		return m_pEventMgr->OnInstallDriver(event, file);
	if (m_registered_drivers.resolve_contain(m_pEnvironmentHelper, file.getFull(), &new_pid))
	{
		cEvent new_event(event, new_pid);
		return m_pEventMgr->OnInstallDriver(new_event, file);
	}
	return;
	//if (!m_registered_drivers.contain(file.getFull(), NULL))
	//	return;	
	//return m_pEventMgr->OnInstallDriver(event, file);
}

void cEventHandlerHeurInstall::OnRegValueSetStrPre(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sNewData, tcstring sValueData)
{
	if (!sValueName || !sNewData)
		return;
	if (0 != tstricmp(sValueName, _T("imagepath")))
		return;
	uint32_t type;
	if (!m_pEnvironmentHelper->Reg_QueryValueEx(sKey, _T("type"), NULL, &type, sizeof(type), NULL))
		return;
	if (0 == (type & SERVICE_DRIVER))
		return;
	cPath imagepath(m_pEnvironmentHelper, sNewData);
	tPid pid = iFindProcessByDroppedFile(imagepath.getFull());
	if (!pid)
	{
		m_registered_drivers.add(event.m_pid, imagepath.getFull());
		return;
	}
	cEvent new_event(event, pid);
	return m_pEventMgr->OnInstallDriver(new_event, imagepath);
}

void cEventHandlerHeurInstall::OnRegValueSetPre(cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nNewType, const void* pNewData, size_t nNewDataSize, uint32_t nValueType, const void* pValueData, size_t nValueDataSize)
{
	if (!sValueName)
		return;
	if (0 != tstricmp(sValueName, _T("type")))
		return;
	if (!pNewData || nNewDataSize < sizeof(uint32_t))
		return;
	uint32_t type = *(uint32_t*)pNewData;
	if (0 == (type & SERVICE_DRIVER))
		return;
	tstring simagepath = m_pEnvironmentHelper->Reg_QueryValueStrEx(key, _T("imagepath"));
	if (!simagepath)
		return;
	cPath imagepath(m_pEnvironmentHelper, simagepath);
	tstrfree(simagepath);
	tPid pid = iFindProcessByDroppedFile(imagepath.getFull());
	if (!pid)
	{
		m_registered_drivers.add(event.m_pid, imagepath.getFull());
		return;
	}
	cEvent new_event(event, pid);
	return m_pEventMgr->OnInstallDriver(new_event, imagepath);
}

void cEventHandlerHeurInstall::OnRegAutoRunRegisterResolved(cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart, tcstring sFilename)
{
	cPath pa(m_pEnvironmentHelper, sFilename);
	return m_pEventMgr->OnInstall(event, key, sValueName, pa);
}

void cEventHandlerHeurInstall::iIsInstallInStream(cEvent& event, cPath& file)
{
	if (!tstrrchr(file.getFilename(), ':'))
		return;
	tstring sHostFile = tstrdup(file.getFull());
	if (!sHostFile)
		return;
	tstring stream = (tstring)tstrrchr(sHostFile, ':');
	if (stream)
	{
		*stream = 0;
		cPath host(m_pEnvironmentHelper, sHostFile, sHostFile);
		if (host.getAttributes() != FILE_ATTRIBUTE_INVALID)
			m_pEventMgr->OnInstallInNTFSStream(event, file);
	}
	tstrfree(sHostFile);
	return;
}

void cEventHandlerHeurInstall::OnInstallDriver(cEvent& event, cPath& driver)
{
	return iIsInstallInStream(event, driver);
}

void cEventHandlerHeurInstall::OnInstall(cEvent& event, const cRegKey& key, tcstring sValueName, cPath& file)
{
	if (!event.GetProcess())
		return;
	if (0 != tstrcmp(file.getFull(), event.GetProcess()->m_image.getFull()))
		m_pEventMgr->OnInstallNotSelf(event, key, sValueName, file);
	iIsInstallInStream(event, file);
	return;
}
