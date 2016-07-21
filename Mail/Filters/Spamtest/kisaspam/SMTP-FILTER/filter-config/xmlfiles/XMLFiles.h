/*******************************************************************************
 * Project: SMTP Filtration                                                    *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: XMLFiles.h                                                            *
 * --------------------------------------------------------------------------- *
 * Kirill Zorky, zorky@ashmanov.com                                           *
 *******************************************************************************/
#ifndef __XMLFiles_h
#define __XMLFiles_h

#include "ZTools/ExpatUtils/ExpatUtils.h"
//---------------------------------------------------------------------//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#pragma warning (disable: 4786)
#pragma warning (disable: 4514) // unreferenced inline function has been removed
#pragma warning (disable: 4710) // function not inlined
#endif
//---------------------------------------------------------------------//
// SMTP-Filter Configuration XML File Types
enum eXMLTypes
{
  tXMLUnknown,

  // files
  tXMLMain,
  tXMLProfilesRefs,
  tXMLIPListsRefs,
  tXMLEMailListsRefs,
  tXMLDNSListsRefs,
  
  tXMLIPList,
  tXMLEMailList,
  tXMLDNSList,
  tXMLProfile,

  tXMLSampleList,
  tXMLSample,

  tXMLCatList,

  // some tags
  tXMLRule,
  tXMLConditions,
  tXMLActions
};
//---------------------------------------------------------------------//
// functions
//---------------------------------------------------------------------//
// returns 0 if succeed, otherwise non-zero value (error code)
int LoadXMLFile(const char* path, XMLTag& tag, enum eXMLTypes type,
    _STL::string *err = NULL, int SrcCodePage = 0, int DestCodePage = 0);

// returns NULL if failed
_TagDescription* GetXMLFileDescription(enum eXMLTypes type);
//---------------------------------------------------------------------//
#endif
/*
 * <eof XMLFiles.h>
 */
