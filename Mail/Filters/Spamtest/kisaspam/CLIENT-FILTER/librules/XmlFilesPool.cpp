/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file   XmlFilesPool.cpp
 * \author Victor V. Troitsky
 * \brief  XML files memory pool implementation
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 11.01.05: v 1.00 created by VVT
 *==========================================================================
 */
#include "commhdr.h"
#pragma hdrstop

#include <string.h>

#include "XmlFilesPool.h"
#include "ExpatUtilsExt.h"

SrcXmlFileData * XmlFilesPool::getXMLFile(const char *filename)
{
	//filename preprocessing (cut off 'hidden/' prefix if any)
	char *pos;
	char *fn;
	//_splitpath(filename, NULL, NULL, fn, ext);
	pos = strrchr(filename, '/'); 
	if(pos)
		fn = pos + 1;
	else
		fn = (char*)filename;
	//end of file preprocessing

	for(size_t i=0; i < m_nCount; i++)
		if(strcmp(m_pXmlFiles[i].szFileName,fn) == 0)
			return &m_pXmlFiles[i];
	return NULL;
}


// returns 0 if succeed, otherwise non-zero value (error code)
int LoadXMLFileFromPool(const char* filename, XmlFilesPool *pXmlPool, XMLTag& tag, enum eXMLTypes type,
    _STL::string *err, int SrcCodePage, int DestCodePage)

{
	if(pXmlPool)
	{
		SrcXmlFileData *pXMLFile = pXmlPool->getXMLFile(filename);
		if(pXMLFile)
		{
			return LoadXMLFileFromMemory(pXMLFile->szFileData, pXMLFile->dwFileLen, tag, type, err, SrcCodePage, DestCodePage);	
		}
		else
		{
			if(err)
				(*err) = "XML file not found in archive";
			return e_ParsXMLErrorXMLFileNotFoundInArchive;
		}
	}
	
	if(err)
		(*err) = "XML files pool is missed";
	return e_ParsXMLErrorNoXMLFilesPool;
}

