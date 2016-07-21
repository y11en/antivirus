#include "heur_virus.h"
#include "trace.h"
#include "srv.h"

cEventHandlerHeurVirus::cEventHandlerHeurVirus()
{
}

cEventHandlerHeurVirus::~cEventHandlerHeurVirus()
{
//	for (tmodifypendingvector::iterator _it = m_pending.begin(); _it != m_pending.end(); _it++)
//		tstrfree(_it->fullpath);
}

bool cEventHandlerHeurVirus::Init()
{
	m_sProgramFilesMask.assign(m_pEnvironmentHelper, _T("%ProgramFiles%\\*"));
	return true;
}

bool cEventHandlerHeurVirus::AnalyzeModifications(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* fi_old)
{
	if (!fi_old)
		return false;
	if (AnalyzeModifications1(event, pProcess, file, fi_old))
		return true;
	if (AnalyzeModifications2(event, pProcess, file, fi_old))
		return true;
// 	if (AnalyzeModifications3(event, pProcess, file, fi_old))
// 		return true;
	return false;
}

// if modified only last section and last section become executable
// if modified only last section and last section raw size growed, and timestamp is the same
bool cEventHandlerHeurVirus::AnalyzeModifications1(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* fi_old)
{
	FILE_FORMAT_INFO* fi_new = &file.fi;
	size_t first_modified_sect = (size_t)-1;
	if (0 == fi_old->sect_count || 	fi_old->sect_count != fi_new->sect_count)
		return false;
	for (size_t i=0; i<countof(fi_new->sections) && i<fi_old->sect_count && i<fi_new->sect_count; i++)
	{
		if (0 != memcmp(&fi_new->sections[i], &fi_old->sections[i], sizeof(EXECUTABLE_SECT_INFO)))
		{
			first_modified_sect = i;
			break;
		}
	}
	if (first_modified_sect == fi_old->sect_count-1) // modified only last section
	{
		if (0 == (fi_old->sections[first_modified_sect].characteristics & IMAGE_SCN_MEM_EXECUTE) &&
			0 != (fi_new->sections[first_modified_sect].characteristics & IMAGE_SCN_MEM_EXECUTE))
		{
			return true;
		}

		if (fi_old->sections[first_modified_sect].section_raw_size < fi_new->sections[first_modified_sect].section_raw_size)
		{
			if (fi_old->timestamp == fi_new->timestamp 
				|| fi_old->entry_point == fi_new->entry_point) 
				return true;
		}
	}

	// entrypoint data changed
	if (/*first_modified_sect == (size_t)-1 && */(fi_old->timestamp == fi_new->timestamp || 
		(fi_old->checksum != 0  && fi_old->checksum == fi_new->checksum)))
	{
		if (0 != memcmp(fi_old->entry_point_data, fi_new->entry_point_data, sizeof(fi_old->entry_point_data)))
			return true;
	}


	return false;
}

// if two executable files modified and overlay appear in new file, and old files images (stubs) were 
// different but new images are the same 
bool cEventHandlerHeurVirus::AnalyzeModifications2(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* fi_old)
{
	FILE_FORMAT_INFO* fi_new = &file.fi;
	if (0 == fi_old->sect_count || 0 == fi_new->sect_count)
		return false;
	if (0 != fi_old->overlay_size)
		return false;
	if (0 == fi_new->overlay_size)
		return false;
	size_t count = pProcess->m_ModifiedImagesList.size();
	if (count < 1)
		return false;
	cFileFormatList::iterator it = pProcess->m_ModifiedImagesList.end();
	it--;
	if (0 == tstrcmp(it->fullpath, file.getFull()))
	{
		// modified the same file
		if (count < 2)
			return false;
		it--;
		if (0 == tstrcmp(it->fullpath, file.getFull()))
			return false;
	}

	FILE_FORMAT_INFO* fi_old2 = it->pfi;
	bool bOldFilesDifferent = false;
	if (fi_old2 && fi_old2->sect_count)
	{
		for (size_t i=0; i<countof(fi_old2->sections) && i<fi_old2->sect_count; i++)
		{
			if (0 != memcmp(&fi_new->sections[i], &fi_old->sections[i], sizeof(EXECUTABLE_SECT_INFO)))
			{
				bOldFilesDifferent = true;
				break;
			}
		}
	}
	if (!bOldFilesDifferent)
		return false;
	cFile old_file(m_pEnvironmentHelper, it->fullpath, it->fullpath, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 0, fi_old);
	return srvIsImageCopy(m_pEnvironmentHelper, old_file, file);
}

// // if TIMESTAMP NOT CHANGED, and added new section(s), 
// // and RVAs of old sections not modified, 
// // and at least one of new sections is executable and/or writable, 
// // and code at EP modified
// bool cEventHandlerHeurVirus::AnalyzeModifications3(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* fi_old)
// {
// 	FILE_FORMAT_INFO* fi_new = &file.fi;
// 	if (fi_old->sect_count == 0 || fi_old->sect_count >= countof(fi_new->sections))
// 		return false;
// 	if (fi_new->sect_count <= fi_old->sect_count)
// 		return false;
// 	if (fi_old->timestamp != fi_new->timestamp)
// 		return false;
// 	for (size_t i=0; i<fi_old->sect_count; i++)
// 	{
// 		if (fi_new->sections[i].section_rva_offset != fi_old->sections[i].section_rva_offset)
// 			return false;
// 	}
// 	bool bSuspiciousSectionAdded = false;
// 	for (size_t i=fi_old->sect_count; i<countof(fi_new->sections) && i<fi_new->sect_count; i++)
// 	{
// 		if ((fi_new->sections[i].characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_WRITE)) != 0)
// 		{
// 			bSuspiciousSectionAdded = true;
// 			break;
// 		}
// 	}
// 
// 	if (!bSuspiciousSectionAdded)
// 		return false;
// 
// 	if (0 == memcmp(fi_old->entry_point_data, fi_new->entry_point_data, sizeof(fi_old->entry_point_data)))
// 		return false;
// 
// 	return true;
// }

void cEventHandlerHeurVirus::iOnFileCloseModified(cEvent& event, cFile& file)
{
	if (!file.isExecutable())
		return;

	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;

	bool bInfector = false;
	// multiple ProgramFiles subfolders modifications
	if (tstrMatchWithPattern(file.getFull(), m_sProgramFilesMask.getFull(), false))
	{
		size_t level = 0;
		tcstring ptr = file.getFull();
		while (ptr = tstrchr(ptr, '\\'))
		{
			level++;
			ptr = tstrinc(ptr);
		}
		if (level == 3)
		{
			tstring dir = tstrdup(file.getFull());
			ptr = tstrrchr(dir, '\\');
			tstrchar((tstring)ptr) = 0;
			if (!pProcess->m_ModifiedProgFilesDirsList.contain(dir))
			{
				pProcess->m_ModifiedProgFilesDirsList.add(dir);
				if (pProcess->m_ModifiedProgFilesDirsList.count() >= 3)
					bInfector = true;
			}
			tstrfree(dir);
		}
	}

	// executable modifications
	if (!bInfector)
	{
		cFileFormatList::iterator it = pProcess->m_ModifiedImagesList.find(file.getFull());
		if (it != pProcess->m_ModifiedImagesList.end())
		{
			assert(it->pfi->filesize);
			bInfector = AnalyzeModifications(event, pProcess, file, it->pfi);
		}
	}

	pProcess.release();
	if (bInfector)
		m_pEventMgr->OnInfector(event, file);

	return;
}

void cEventHandlerHeurVirus::OnFileCloseModifiedPre(cEvent& event, cFile& file)
{
	if (m_OpCacheFileCloseModified.DoPre(event, file))
		return iOnFileCloseModified(event, file);
	return;
}

void cEventHandlerHeurVirus::OnFileCloseModifiedPost(cEvent& event, cFile& file)
{
	if (m_OpCacheFileCloseModified.DoPost(event, file))
		return iOnFileCloseModified(event, file);
	return;
}
