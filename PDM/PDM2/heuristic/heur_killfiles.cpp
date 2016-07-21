#include "heur_killfiles.h"
#include "srv.h"

void cEventHandlerHeurKillFiles::OnFileDeletePost(cEvent& event, cFile& file)
{
	if (!srvIsWinSystemFile(m_pEnvironmentHelper, file))
		return;

	cProcess proc = event.GetProcess();
	if (!proc)
		return;

	long nDeletedSystemFiles = 0;
	for (cFileFormatList::const_iterator it = proc->m_DeletedFilesList.begin();
		it != proc->m_DeletedFilesList.end();
		++it)
	{
		cFile file(m_pEnvironmentHelper, (*it).fullpath, (*it).fullpath);
		if (srvIsWinSystemFile(m_pEnvironmentHelper, file))
			nDeletedSystemFiles++;
	}
	proc.release();
	if (nDeletedSystemFiles >= 4)
		return m_pEventMgr->OnKillSystemFiles(event, nDeletedSystemFiles);

	return;
}
