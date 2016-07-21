#ifndef __ExpatUtils_h__
#define __ExpatUtils_h__
//======================================================================//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#pragma warning (disable: 4514) // unreferenced inline function has been removed
#endif
//======================================================================//
#include <gnu/_expat.h>
#include "XMLTag.h"
#include "ZTools/ZUtils/EString.h"
//======================================================================//
int UnknownEncodingHandler(void *encodingHandlerData,
                                const XML_Char *name, XML_Encoding *info);
//---------------------------------------------------------------------//
// error codes
#define e_ParsXMLNoError              0
#define e_ParsXMLErrorOpenFile        1
#define e_ParsXMLErrorIO              2
#define e_ParsXMLErrorParsing         3
#define e_ParsXMLErrorInvalidEncoding 4
#define e_ParsXMLErrorNoDescription   5
#define e_ParsXMLErrorInitParser      6

int ParsXMLFile(const char* path, XMLTag& tag, _STL::string *err = NULL,
                            int SrcCodePage = 0, int DestCodePage = 0);
int ParsXMLBuffer(const char* buffer, size_t buf_size, XMLTag& tag,_STL::string *err = NULL,
                            int SrcCodePage = 0, int DestCodePage = 0);
int PrintXMLFile(const char* path, const XMLTag& tag, _STL::string *err = NULL);

//---------------------------------------------------------------------//
// utility functions
bool ValidateSubTag(const char* name, const _TagDescription* descr,
                                           _SubTagInfo** const subinfo);
//======================================================================//
const char* SetHTMLSpecSyms(EString& buf);
const char* SetHTMLSpecSyms(const char* str);
//======================================================================//
#endif
