/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file   XmlFilesPool.h
 * \author Victor V. Troitsky
 * \brief  XML files memory pool
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 11.01.05: v 1.00 created by VVT
 *==========================================================================
 */
#ifndef __XMLFILESPOOL_H__
#define __XMLFILESPOOL_H__

#include "ProfileCompile.h"
#include "ExpatUtilsExt.h"

// error codes
#define e_ParsXMLErrorXMLFileNotFoundInArchive		10
#define e_ParsXMLErrorNoXMLFilesPool				11

//XML files pool
struct XmlFilesPool {

public:
	//Interface functions
	XmlFilesPool(SrcXmlFileData *pPool, size_t filesCount) : m_pXmlFiles(pPool),m_nCount(filesCount) {};
	SrcXmlFileData *getXMLFile(const char *filename);

	//Implementation
protected:
	SrcXmlFileData *m_pXmlFiles;
	size_t m_nCount;
};


// returns 0 if succeed, otherwise non-zero value (error code)
int LoadXMLFileFromPool(const char* filename, XmlFilesPool *pXmlPool, XMLTag& tag, enum eXMLTypes type,
    _STL::string *err = NULL, int SrcCodePage = 0, int DestCodePage = 0);

#endif //__XMLFILESPOOL_H__