/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file	ExpatUtilsExt.cpp
 * \author	Victor V. Troitsky
 * \brief	Helper for parsing XML file from memory buffer
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 11.01.05 v 1.00 created by VVT 
 *==========================================================================
 */

#include "commhdr.h"
#pragma hdrstop

#include "ExpatUtilsExt.h"
#include <smtp-filter/filter-config/xmlfiles/XMLFiles.h>

//---------------------------------------------------------------------//
// return 0 if succeed, otherwise non-zero value
// see error codes in ExpatUtils.h
int LoadXMLFileFromMemory(const char* buffer, size_t size, XMLTag& tag, enum eXMLTypes type,
                    _STL::string *err, int SrcCodePage, int DestCodePage)
{
  tag.SetDescr(GetXMLFileDescription(type));
  if(!tag.Descr())
  {
    if(err)
      (*err) = "No XML file description";
    return e_ParsXMLErrorNoDescription;
  }

  return ParsXMLBuffer(buffer, size, tag, err, SrcCodePage, DestCodePage);
}