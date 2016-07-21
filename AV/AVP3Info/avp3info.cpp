// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  07 October 2004,  12:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- avp3info.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AVP3Info_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "avp3info.h"
// AVP Prague stamp end

#define far

typedef char CHAR ;
typedef long LONG ;
typedef long BOOL ;

#include <Bases/Format/Typedef.h>
#include <Bases/Format/Base.h>

#include <Prague/pr_oid.h>
#include <Prague/iface/i_seqio.h>

#include <AV/structs/s_avp3info.h>

#define IMPEX_IMPORT_LIB
   #include <Prague/plugin/p_win32_nfio.h>
   #include <Prague/iface/e_ktime.h>




#define _ERROR(exp) \
    error = exp ;       \
    if (PR_FAIL(error)) \
    {                   \
        return error ;  \
    }

#define AVP_SET   "avp_x.set"
#define COMMENT   ';'

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVP3Info". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetBaseFileInfo )
// Parameters are:
tERROR CAVP3Info::GetBaseFileInfo( hOBJECT p_hObj, cSerializable* p_pInfo ) 
{
   tERROR error = errOK ;
   hIO hFile = NULL;
   cAutoObj<cIO>    hBase;

   PR_TRACE_FUNC_FRAME("AVP3Info::GetBaseFileInfo method") ;

   if (p_pInfo == NULL || !p_pInfo->isBasedOn(cAVP3FileInfo::eIID))
   {
      return errPARAMETER_INVALID ;
   }

   cAVP3FileInfo* pFileInfo = (cAVP3FileInfo*)p_pInfo ;
   pFileInfo->m_dwViruseCount = 0 ;

   if (PR_SUCC(error = sysCheckObject(p_hObj, IID_IO)))
   {
	   hFile = (hIO)p_hObj;
   }
   else if (PR_SUCC(error = sysCheckObject(p_hObj, IID_STRING)))
   {
	   hSTRING name = (hSTRING)p_hObj;
        _ERROR(sysCreateObject(hBase, IID_IO, PID_NATIVE_FIO)) ;
		_ERROR(name->ExportToProp(NULL, cSTRING_WHOLE, hBase, pgOBJECT_FULL_NAME));
		_ERROR(hBase->set_pgOBJECT_OPEN_MODE(fOMODE_OPEN_IF_EXIST));
		_ERROR(hBase->set_pgOBJECT_ACCESS_MODE(fACCESS_READ));
		_ERROR(hBase->sysCreateObjectDone());
		hFile = hBase;
   }
   else
   {
	   return errPARAMETER_INVALID;
   }


   AVP_BaseHeader rcBaseHeader;
   tDWORD dwResult = 0 ;
   _ERROR(hFile->SeekRead(&dwResult, 0, &rcBaseHeader, sizeof(rcBaseHeader))) ;

   _ERROR(DTSet(&pFileInfo->m_dtFileDate, 
                rcBaseHeader.GMTModificationTime.wYear, 
                rcBaseHeader.GMTModificationTime.wMonth,
                rcBaseHeader.GMTModificationTime.wDay,
                rcBaseHeader.GMTModificationTime.wHour,
                rcBaseHeader.GMTModificationTime.wMinute,
                rcBaseHeader.GMTModificationTime.wSecond,
                rcBaseHeader.GMTModificationTime.wMilliseconds)) ;

   tQWORD qwResult = 0 ;
   tQWORD qwOffset = rcBaseHeader.BlockHeaderTableFO;

   for (tDWORD dwIndex = 0; dwIndex < rcBaseHeader.BlockHeaderTableSize; ++dwIndex)
   {
      AVP_BlockHeader rcBlockHeader ;
      if (PR_FAIL(hFile->SeekRead(&dwResult, qwOffset, &rcBlockHeader, sizeof(rcBlockHeader))))
		  break;

	  if (dwResult == 0)
	  {
		  error = errOBJECT_READ;
		  break;
	  }

      if (rcBlockHeader.BlockType != BT_RECORD)
      {
         continue ;
      }

      pFileInfo->m_dwViruseCount += rcBlockHeader.NumRecords ;
	  qwOffset += sizeof(rcBlockHeader);
   }

   return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetBasesInfo )
// Parameters are:
tERROR CAVP3Info::GetBasesInfo( hSTRING p_hBasesPath, cSerializable* p_pInfo ) 
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("AVP3Info::GetBasesInfo method") ;

    if (p_pInfo == NULL || !p_pInfo->isBasedOn(cAVP3BasesInfo::eIID))
    {
       return errPARAMETER_INVALID ;
    }

	PR_TRACE((this, prtNOTIFY, "avp3info\tGetBasesInfo for %tstr...", p_hBasesPath));

    cAVP3BasesInfo* pBasesInfo = (cAVP3BasesInfo*)p_pInfo ;
    pBasesInfo->m_dwViruseCount = 0 ;

    cStringObj strRootFile ;
    strRootFile = p_hBasesPath ;
    strRootFile.add_path_sect(AVP_SET, cCP_ANSI) ;

	cIOObj avpIo((cObj*)this, cAutoString(strRootFile), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
	if( PR_FAIL(avpIo.last_error()) )
	{
		strRootFile = p_hBasesPath ;
		strRootFile.add_path_sect("kavbase.mft", cCP_ANSI);
		cIOObj klavIo((cObj*)this, cAutoString(strRootFile), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if( PR_FAIL(klavIo.last_error()) )
			return klavIo.last_error();

		(cAutoObj<cIO>&)avpIo = klavIo.relinquish();
	}

    tQWORD qwRootFileSize = 0 ;
    _ERROR(avpIo->GetSize(&qwRootFileSize, IO_SIZE_TYPE_EXPLICIT)) ;

    cBuff<tCHAR, 2048> buff ;
    _ERROR(avpIo->SeekRead(NULL, 0, buff.get((tUINT)qwRootFileSize + 1, false), (tDWORD)qwRootFileSize)) ;

	error = GetBaseFileInfo_KDB(avpIo, buff.ptr (), buff.size (), pBasesInfo);
	if (error == errOK)
	{
		// KLAVA database info read successfully
		return errOK;
	}

    cStringObj strFileArray ;
    strFileArray = (tSTRING)buff ;
    tDWORD dwPos = 0 ;
    tDWORD dwLength = strFileArray.length() ;
    tBOOL  blFirstFileInfo = cTRUE ;
    while (dwPos < dwLength && dwPos != cSTRING_EMPTY_LENGTH)
    {
       tDWORD dwStrEnd = strFileArray.find_first_of("\r\n", dwPos) ;
       if (dwStrEnd == cSTRING_EMPTY_LENGTH)
       {
          dwStrEnd = dwLength ;
       }

       if (dwStrEnd != dwPos && ((tSTRING)buff)[dwPos] != COMMENT)
       {
          cStringObj strFileName ;
          strFileName += p_hBasesPath ;
          strFileName.add_path_sect(strFileArray.substr(dwPos, dwStrEnd - dwPos)) ;

          cAVP3FileInfo info ;
          tERROR err = GetBaseFileInfo(cAutoString(strFileName), &info) ;
          if (PR_SUCC(err))
          {
             if (blFirstFileInfo)
             {
				blFirstFileInfo = cFALSE;
                DTCpy(&pBasesInfo->m_dtBasesDate, &info.m_dtFileDate) ;
             }
             else
             {
                if (DTCmp(&pBasesInfo->m_dtBasesDate, &info.m_dtFileDate) == cCOMPARE_LESS)
                {
                   DTCpy(&pBasesInfo->m_dtBasesDate, &info.m_dtFileDate) ;
                }
             }

             pBasesInfo->m_dwViruseCount += info.m_dwViruseCount ;
          }
       }

       dwPos = strFileArray.find_first_not_of("\r\n", dwStrEnd) ;
    }

	PR_TRACE((this, prtNOTIFY, "avp3info\tGetBasesInfo for %tstr done, viruses=%d, %tgdt", p_hBasesPath, pBasesInfo->m_dwViruseCount, pBasesInfo->m_dtBasesDate));

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVP3Info". Register function
tERROR CAVP3Info::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVP3Info)
mINTERNAL_TABLE_END(AVP3Info)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVP3Info)
    mEXTERNAL_METHOD(AVP3Info, GetBaseFileInfo)
    mEXTERNAL_METHOD(AVP3Info, GetBasesInfo)
mEXTERNAL_TABLE_END(AVP3Info)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

    PR_TRACE_FUNC_FRAME_( *root, "AVP3Info::Register method", &error );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_AVP3INFO,                           // interface identifier
        PID_AVP3INFO,                           // plugin identifier
        0x00000000,                             // subtype identifier
        AVP3Info_VERSION,                       // interface version
        VID_GUSCHIN,                            // interface developer
        &i_AVP3Info_vtbl,                       // internal(kernel called) function table
        (sizeof(i_AVP3Info_vtbl)/sizeof(tPTR)), // internal function table size
        &e_AVP3Info_vtbl,                       // external function table
        (sizeof(e_AVP3Info_vtbl)/sizeof(tPTR)), // external function table size
        NULL,                                   // property table
        0,                                      // property table size
        sizeof(CAVP3Info)-sizeof(cObjImpl),     // memory size
        IFACE_SYSTEM                            // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"AVP3Info(IID_AVP3INFO) registered [%terr]",error) );
    #endif // _PRAGUE_TRACE_

// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call AVP3Info_Register( hROOT root ) { return CAVP3Info::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



