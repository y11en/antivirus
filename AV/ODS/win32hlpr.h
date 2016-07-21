#ifndef __VISTAVIRTHLP_H__
#define __VISTAVIRTHLP_H__

#ifdef _WIN32
#define DEF_IMPL ;
#define DEF_IMPL2 ;
#define INLINE
#else
#define DEF_IMPL { return false; }
#define DEF_IMPL2 {}
#define INLINE inline
#endif

INLINE bool GetVirtualObject(const cStrObj &Object, cStrObj &VirtObj) DEF_IMPL

struct CNtfsChecker
{
	CNtfsChecker() : m_steps(0), m_hntfs(NULL){ m_drive[0] = m_drive[1] = 0; }
	~CNtfsChecker() DEF_IMPL2

	bool AddObject(cStrObj& sPath, cStrObj* sStream = NULL) DEF_IMPL
	bool CheckHiddenObjects(cObjPtr* pPtr, bool bCheckStream, bool bCheckFileStream) DEF_IMPL
	bool CheckHidden(const tWCHAR* name, tDWORD size, cStringObjVector& hidden) DEF_IMPL
	bool GetVerdict(cVector<cStrObj>& vecHidden) DEF_IMPL

#ifdef INDEX_ENTRY_NODE
	void CheckHiddenStreams(NTFS_FILE* file, cVector<cStrObj>& hidden, tWCHAR* name, tDWORD size) DEF_IMPL2
#endif

	tPTR             m_hntfs;

private:
	tDWORD           m_steps;
	cDwordVector     m_crcs;
	cStringObjVector m_hidden;
	tWCHAR           m_drive[2];
};

#endif // __VISTAVIRTHLP_H__
