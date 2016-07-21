#ifndef __VLISTINTERFACE_H__
#define __VLISTINTERFACE_H__

/////////////////////////////////////////////////////////////////////

#define VLG_NOWINDOW					0x00000001
#define VLG_STARTJUSTNOW			0x00000002
#define VLG_VIEWJUSTNOW				0x00000004
#define VLG_WAITFORCOMPLETE		0x00000008

BOOL GenerateVirusList(
						LPCTSTR pszVListFilePath,	// VList file path (NULL means path from registry will be taken)
						DWORD   dwStartFlags			// Start flags (see VLG_XXXX defines)
						);		

/////////////////////////////////////////////////////////////////////

#endif //__VLISTINTERFACE_H__