#include "heur_emul.h"

/************************************************************************/
/* Infector                                                             */
/************************************************************************/
void cEventHandlerHeurEmulVirus::OnFileModifyPost(cEvent& event, cFile& file) {
	const tcstring fullPath = file.getFull();
	bool bInfector = false;

	if (!m_pEnvironmentHelper->PathIsNetwork(fullPath) && !m_pEnvironmentHelper->PathIsRemovable(fullPath))
		return;

	{
		cProcess pProcess = event.GetProcess();
		if (!pProcess)
			return;

		if (pProcess->m_ModifiedNonLocalFilesList.contain(fullPath))
			return;

		if (pProcess->m_CreatedImagesList.contain(fullPath))
			return;

		pProcess->m_ModifiedNonLocalFilesList.add(fullPath);
		bInfector = (pProcess->m_ModifiedNonLocalFilesList.count() >= 3);
	}

	if (bInfector) {
		m_pEventMgr->OnInfector(event, file);
	}
}

/************************************************************************/
/* Downloader                                                           */
/************************************************************************/
#define DOWNLOAD_HEADER_SIZE 0x400

void cEventHandlerHeurEmulDownloader::OnDownloadFile(cEvent& event, cFile& file)
{
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		if (pProcess->m_pLastDownloadedFileHeader 
			&& pProcess->m_nLastDownloadedFileHeaderSize < DOWNLOAD_HEADER_SIZE)
		{
			m_pEnvironmentHelper->Free(pProcess->m_pLastDownloadedFileHeader);
			pProcess->m_pLastDownloadedFileHeader = NULL;
		}
		if (!pProcess->m_pLastDownloadedFileHeader)
			pProcess->m_pLastDownloadedFileHeader = m_pEnvironmentHelper->Alloc(DOWNLOAD_HEADER_SIZE);
		if (pProcess->m_pLastDownloadedFileHeader)
		{
			uint32_t bytes_read;
			pProcess->m_nLastDownloadedFileHeaderSize = DOWNLOAD_HEADER_SIZE;
			if (!file.Read(0, pProcess->m_pLastDownloadedFileHeader, DOWNLOAD_HEADER_SIZE, &bytes_read) 
				|| bytes_read != DOWNLOAD_HEADER_SIZE)
			{
				m_pEnvironmentHelper->Free(pProcess->m_pLastDownloadedFileHeader);
				pProcess->m_pLastDownloadedFileHeader = NULL;
			}
		}
	}
	m_downloaded_list.add(event.m_pid, file.getFull());
	return;
}

void cEventHandlerHeurEmulDownloader::OnProcessCreatePre(cEvent& event, cFile& image_path, tcstring cmd_line, uint32_t creation_flags)
{
	if (m_downloaded_list.contain(image_path.getFull(), NULL))
		return m_pEventMgr->OnDownloader(event, NULL);
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	if (!pProcess->m_pLastDownloadedFileHeader) 
		return;
	if (pProcess->m_nLastDownloadedFileHeaderSize < DOWNLOAD_HEADER_SIZE)
		return;
	uint32_t bytes_read;
	char buff[DOWNLOAD_HEADER_SIZE];
	if (!image_path.Read(0, buff, DOWNLOAD_HEADER_SIZE, &bytes_read))
		return;
	if (bytes_read != DOWNLOAD_HEADER_SIZE)
		return;
	if (0 == memcmp(pProcess->m_pLastDownloadedFileHeader, buff, DOWNLOAD_HEADER_SIZE))
		return m_pEventMgr->OnDownloader(event, NULL);
	return;
}
