// NewsReader.h: interface for the CNewsReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWSREADER_H__17A61603_C569_433D_A757_C8C8C970947D__INCLUDED_)
#define AFX_NEWSREADER_H__17A61603_C569_433D_A757_C8C8C970947D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewsReader
{
public:
	static bool ReadFromFile(const cStringObj& sFileName, cGuiNews& News);
	static bool ReadFromBuffer(const char *xml, size_t len, cGuiNews& News);
};

#endif // !defined(AFX_NEWSREADER_H__17A61603_C569_433D_A757_C8C8C970947D__INCLUDED_)
