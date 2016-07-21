#ifndef  _APVERSION_H
#define  _APVERSION_H 1

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief Version and project dependent names and constants
 */


#define REG_KEY_COMPANY TEXT("Ashmanov & Partners")	//!< Registry parent key for options to delete on uninstall
#define REG_KEY         TEXT("Ashmanov & Partners\\SpamTest")	//!< Registry key for options
#define APPDATA_KEY TEXT("\\SpamTest")	                //!< Application data subdirectory
#define DEF_EXAMPLES_EMAIL_SPAM TEXT("undetected@spam.kaspersky.com")       //!< Email address for spam examples
#define DEF_EXAMPLES_EMAIL_NOT_SPAM TEXT("errors@spam.kaspersky.com")   //!< Email address for not spam examples

#define OLEXT_SUBKEY TEXT("SpamTest")                 //!< OL Extention regisry subkey
#define OLADDIN_SUBKEY TEXT("SpamTestOutlookAddIn")   //!< OL Addin OLE ProgID
#define OLADDIN_CLSID TEXT("{74487093-550B-4974-BC70-39A2ACBA6B26}") //!< OL Addin OLE CLSID


#define OESpamTestMutextName TEXT("OE SpamTest")


#define USE_CUSTOMRULE() 1                                //!< OL Extention custom rule action registration flag
#define OLEXTRULE_SUBKEY TEXT("SpamTest")                 //!< OL Extention custom rule action regisry subkey

#define DEFAULT_USER_THRESHOLD 95
#define DEFAULT_MAX_SAMPLES    2000

// OL Save message for example size limits
#define MAX_WRITE_MESSAGE_HTML_SIZE    ULONG(200*1024L)
#define MAX_WRITE_MESSAGE_TEXT_SIZE    ULONG(100*1024L)
#define MAX_WRITE_MESSAGE_RTF_SIZE     ULONG(200*1024L)
#define MAX_WRITE_MESSAGE_ATTACH_SIZE  ULONG(50*1024L)
#define MAX_WRITE_MESSAGE_ATTACH_NUM   UINT(6)

//  License
#define USE_LICENCE() 1 // can turn off for debug

#define USE_SCANNEWMAIL() 1 
#define USE_UPDATER() 1 // can turn off for debug
#define USE_SMTP_SEND() 0 // obsolete feature

//  RC defines

#define RC_COMPANYNAME "Ashmanov & Partners\0"		//!< RC version info Company name
#define RC_COPYRIGHT   "Copyright © 2004-2005 Ashmanov & Partners\0" //!< RC version info Copyright
#define RC_TRADEMARKS  "\0"	//!< RC version info Trade marks

#define RC_PRODUCTNAME "Kaspersky Anti-Spam Personal\0"   //!< RC version info Product name. Common components
#define RC_OL_PRODUCTNAME "Kaspersky Anti-Spam Personal for Outlook\0"   //!< RC version info Product name. OL Components
#define RC_OE_PRODUCTNAME "Kaspersky Anti-Spam Personal for Outlook Express\0" //!< RC version info Product name. OE Components

#define __VER_MINOR__ 53
#define __VER_MINOR_STR__ "53"

#define IS_RELEASE() 1 //!< 0 - beta, not 0 - release
#define RC_PRODUCTVERSION_STR "1.1." __VER_MINOR_STR__ ".0\0"    //!< RC version info Product version (string)
#define RC_PRODUCTVERSION_STR_SHORT "1.1." __VER_MINOR_STR__ "\0"    //!< RC version info Product version (string)
#define RC_PRODUCTVERSION 1,1,__VER_MINOR__,0 //!< RC version info Product version (number)

#ifdef _DEBUG
#define RC_FILEVERSION_STR "1.1." __VER_MINOR_STR__ ".0.Debug\0" //!< RC version info File version (string)
#else
#define RC_FILEVERSION_STR "1.1." __VER_MINOR_STR__ ".0\0"       //!< RC version info File version (string)
#endif
#define RC_FILEVERSION 1,1,__VER_MINOR__,0    //!< RC version info File version (number)
 

#ifdef _DEBUG
#define _RC_DEBUGFILEFLAG   VS_FF_DEBUG  
#else
#define _RC_DEBUGFILEFLAG   0
#endif

#ifdef WORKSHOP_INVOKED // BC++5 - desing time only
  #define RC_FILEFLAGS   0
#else
  #if !IS_RELEASE()
    #define RC_FILEFLAGS   _RC_DEBUGFILEFLAG | VS_FF_PRERELEASE    //!< RC version info File flags
  #else
    #define RC_FILEFLAGS                       0    //!< RC version info File flags
  #endif
#endif // WORKSHOP_INVOKED // BC++5 - desing time only

#endif

