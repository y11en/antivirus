#define PR_IMPEX_DEF

#include <iostream>
#include <iomanip>

#include <prague.h> 
#include <pr_loadr.h>
#include <iface/i_root.h>
#include <iface/i_os.h>
#include <iface/i_objptr.h>
#include <iface/i_io.h>

using namespace std;

template <class Ch, class Tr> 
basic_ostream <Ch,Tr>& formatted_hex ( basic_ostream<Ch,Tr>& aStream ) 
{ 
  aStream << "0x" <<  setw ( 8 ) << setfill ( '0' ) << uppercase << hex;
  return aStream; 
}

hROOT g_root = 0;

#define IMPEX_IMPORT_LIB
#include <plugin/p_nfio.h> 
#include <iface/e_ktime.h>


#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table )
#include <iface/e_ktime.h>
#include <plugin/p_nfio.h> 
IMPORT_TABLE_END

tCHAR theTestDir [] = "nfio_test_dir"; 
tCHAR thePrefix [] = "nfio_test_";
tCHAR theFileName [] = "nfio_test.tst";
tCHAR theTempFileName [ MAX_PATH ] = {0};

tCHAR theExecutable [] = "ls";
tCHAR theCmdLine [] = "-laF";
tCHAR theStartDir [] = "/";

void ObjPtr_print ( hObjPtr anObjPtr )
{
  if ( !anObjPtr )
    return;
  
  tERROR anError = errOK;

  int aLevel = 0;
  while ( true ) {
    if ( PR_FAIL ( anError = anObjPtr->Next () ) && 
         aLevel-- &&
         PR_SUCC ( anError = anObjPtr->StepUp () ) )
      continue;
      
    if ( PR_FAIL ( anError ) ) 
      break;
    char aPath [ MAX_PATH ] = {0};
    if ( PR_SUCC ( anObjPtr->propGetStr ( 0, pgOBJECT_FULL_NAME, aPath, sizeof ( aPath ) ) ) ) 
      cerr << aPath;
    if ( PR_SUCC ( anObjPtr->propGetStr ( 0, pgOBJECT_PATH, aPath, sizeof ( aPath ) ) ) ) 
      cerr << "(" << aPath << ")";
    if ( PR_SUCC ( anObjPtr->propGetStr ( 0, pgOBJECT_NAME, aPath, sizeof ( aPath ) ) ) )
      cerr << "(" << aPath << ")";
    cerr << endl;
    
    if ( anObjPtr->get_pgIS_FOLDER () &&
         PR_SUCC ( anObjPtr->StepDown () ) )
      ++aLevel;
  }
}


void OS_test () 
{
  tERROR anError = errOK; 
  hOS aOS = 0;

  cerr << "***** create a new instance of OS - no errors are expecting" << endl;
  anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &aOS ), IID_OS, PID_NFIO, SUBTYPE_ANY );
  cerr << "OS::sysCreateObject, error=" << formatted_hex << anError << endl; 


  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create a new instance of OS - no errors are expecting" << endl;  
    anError = aOS->sysCreateObjectDone ();
    cerr << "OS::sysCreateObjectDone, error=" << formatted_hex << anError << endl;   
  }

  hSTRING aString = 0;
  if ( PR_SUCC ( anError ) ) {
    anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &aString ), IID_STRING );
    if ( PR_SUCC ( anError ) ) 
      anError = aString->ImportFromBuff ( 0, theTestDir, 0, cCP_ANSI, cSTRING_Z );    
  }

  hObjPtr anObjPtr = 0;

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create an ObjPtr by OS on aString - no errors are expecting" << endl;  
    aOS->PtrCreate ( &anObjPtr, reinterpret_cast <hOBJECT> ( aString ) );
    cerr << "OS::PtrCreate, error=" << formatted_hex << anError << endl; 
  }

  hIO anIO = 0;
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create IO by OS on ObjPtr - an error is expecting, the source objptr is in state 'BEFORE' yet" << endl;
    anError = aOS->IOCreate ( &anIO, reinterpret_cast <hOBJECT> ( anObjPtr ), 0, 0 );
    cerr << "OS::IOCreate, error=" << formatted_hex << anError << endl;   
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }


  if ( PR_SUCC ( anError ) ) {
    cerr << "***** shift objptr to a next object - no errors are expecting" << endl;    
    anError = anObjPtr->Next () ;
    cerr << "ObjPtr::Next, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create IO by OS on ObjPtr - no errors are expecing" << endl;
    anError = aOS->IOCreate ( &anIO, reinterpret_cast <hOBJECT> ( anObjPtr ), 0, 0 );
    cerr << "OS::IOCreate, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anIO->propGetStr ( 0, pgOBJECT_FULL_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Full name = " << aName << endl;
  }


  if ( PR_SUCC ( anError ) ) {
    tCHAR aFileName [ MAX_PATH ] = {0};
    strcat ( aFileName, theTestDir );
    strcat ( aFileName , "/" );
    strcat ( aFileName, theFileName );
    anError = aString->ImportFromBuff ( 0, aFileName, 0, cCP_ANSI, cSTRING_Z );    
  }  

  if ( PR_SUCC ( anError ) ) {
    anIO->sysCloseObject ();
    anIO = 0;
    cerr << "***** create IO by OS on String - no errors are expecting" << endl;
    anError = aOS->IOCreate ( &anIO, reinterpret_cast <hOBJECT> ( aString ), 0, 0 );
    cerr << "OS::IOCreate, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anIO->propGetStr ( 0, pgOBJECT_FULL_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Full name = " << aName << endl;
  }

  hIO anIO2 = 0;

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create IO by OS on IO - no errors are expecting" << endl;
    anError = aOS->IOCreate ( &anIO2, reinterpret_cast <hOBJECT> ( anIO ), 0, 0 );
    cerr << "OS::IOCreate, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anIO2->propGetStr ( 0, pgOBJECT_FULL_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Full name = " << aName << endl;
  }


  if ( anObjPtr ) {
    anObjPtr->sysCloseObject ();
    anObjPtr = 0;
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create an ObjPtr by OS on aIO - no errors are expecting" << endl;  
    anError = aOS->PtrCreate ( &anObjPtr, reinterpret_cast <hOBJECT> ( anIO ) );
    cerr << "OS::PtrCreate, error=" << formatted_hex << anError << endl; 
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_PATH property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anObjPtr->propGetStr ( 0, pgOBJECT_PATH, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Path name = " << aName << endl;
  }

  hObjPtr anObjPtr2 = 0;

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create an ObjPtr by OS on anObjPTr - no errors are expecting" << endl;  
    anError = aOS->PtrCreate ( &anObjPtr2, reinterpret_cast <hOBJECT> ( anObjPtr ) );
    cerr << "OS::PtrCreate, error=" << formatted_hex << anError << endl; 
  }

  if ( anObjPtr2 ) {
    anObjPtr2->sysCloseObject ();
    anObjPtr2 = 0;
  }


  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create an ObjPtr by OS for root - no errors are expecting" << endl;  
    anError = aOS->PtrCreate ( &anObjPtr2, 0 );
    cerr << "OS::PtrCreate, error=" << formatted_hex << anError << endl; 
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_PATH property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anObjPtr->propGetStr ( 0, pgOBJECT_PATH, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Path name = " << aName << endl;
  }

  cout << "************************************************" << endl;
  if ( PR_SUCC ( anError ) ) 
    cout << "************** OS tests passed *****************" << endl;
  else
    cout << "************** OS tests failed *****************" << endl;
  cout << "************************************************" << endl << endl;

  if ( anIO2 ) {
    anIO2->sysCloseObject ();
    anIO2 = 0;
  }

  if ( anIO ) {
    anIO->sysCloseObject ();
    anIO = 0;
  }

  if ( anObjPtr2 ) {
    anObjPtr2->sysCloseObject ();
    anObjPtr2 = 0;
  }

  if ( anObjPtr ) {
    anObjPtr->sysCloseObject ();
    anObjPtr = 0;
  }

  if ( aString ) {
    aString->sysCloseObject ();
    aString = 0;
  }

  if ( aOS ) {
    aOS->sysCloseObject ();
    aOS = 0;
  }
}

void ObjPtr_test ()
{
  tERROR anError = errOK; 
  hObjPtr anObjPtr = 0; 
  
  cerr << "***** create a new instance of ObjPtr - no errors are expecting" << endl;
  anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &anObjPtr ), IID_OBJPTR, PID_NFIO, SUBTYPE_ANY );
  cerr << "ObjPtr::sysCreateObject, error=" << formatted_hex << anError << endl; 
    
  
  if ( PR_SUCC ( anError ) ) {
    anError = anObjPtr->propSetStr ( 0, pgOBJECT_PATH, theTestDir, sizeof ( theTestDir ) );
    cerr << "ObjPtr::propSetStr, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    anError = anObjPtr->sysCreateObjectDone ();
    cerr << "ObjPtr::sysCreateObjectDone, error=" << formatted_hex << anError << endl;   
  }

  cerr << "**** print all files and folders recursively" << endl;
  ObjPtr_print ( anObjPtr );
    
  cerr << "***** set mask - no errors are expecting" << endl;
  char aMask [] = "*nfio*";
  anError = anObjPtr->propSetStr ( 0, pgMASK, aMask, sizeof ( aMask ) );
  cerr << "ObjPtr::propSetStr, error=" << formatted_hex << anError << endl;   

  if ( PR_SUCC ( anError ) ) {
    cerr << "**** print all files according to the mask=" << aMask << endl;
    ObjPtr_print ( anObjPtr );
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set mask is applied for folders - no errors are expecting" << endl;
    anError = anObjPtr->propSetBool ( plMASK_APPLIED_TO_FOLDERS, cTRUE );
    cerr << "ObjPtr::propSetBool, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** reset objptr - no errors are expecting" << endl;    
    anError = anObjPtr->Reset ( cFALSE );
    cerr << "ObjPtr::Reset, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "**** print all files and folders according to the mask=" << aMask << endl;
    ObjPtr_print ( anObjPtr );
  }


  hIO anIO;
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create IO by ObjPtr - an error is expecting, the source objptr is in state 'AFTER' yet" << endl;
    anError = anObjPtr->IOCreate ( &anIO, 0, 0, 0 );
    cerr << "ObjPtr::IOCreate, error=" << formatted_hex << anError << endl;   
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** reset objptr - no errors are expecting" << endl;    
    anError = anObjPtr->Reset (cFALSE);
    cerr << "ObjPtr::Reset, error=" << formatted_hex << anError << endl;   
  }
  
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** shift objptr to a next object - no errors are expecting" << endl;    
    anError = anObjPtr->Next () ;
    cerr << "ObjPtr::Next, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create IO by ObjPtr - no errors are expecing" << endl;
    anError = anObjPtr->IOCreate ( &anIO, 0, 0, 0 );
    cerr << "ObjPtr::IOCreate, error=" << formatted_hex << anError << endl;   
  }

  hObjPtr aCloneObjPtr = 0;   
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** clone the instance of ObjPtr - no errors are expecting" << endl;
    anError = anObjPtr->Clone ( &aCloneObjPtr );
    cerr << "ObjPtr::Clone, error=" << formatted_hex << anError << endl;   
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }

  cout << "************************************************" << endl;  
  if ( PR_SUCC ( anError ) ) 
    cout << "************ ObjPtr tests passed ***************" << endl;
  else
    cout << "************ ObjPtr tests failed ***************" << endl;
  cout << "************************************************" << endl << endl;


  if ( aCloneObjPtr ) {
    aCloneObjPtr->sysCloseObject ();
    aCloneObjPtr = 0;
  }

  if ( anObjPtr ) {
    anObjPtr->sysCloseObject ();
    anObjPtr = 0;
  }
}


void printTime ( tDATETIME aDateTime ) {
  tDWORD aYear, aMonth, aDay, anHour, aMinute, aSecond, aNanoseconds;
  DTGet ( reinterpret_cast<tDATETIME*> ( aDateTime )
          , &aYear, &aMonth, &aDay, &anHour, &aMinute, &aSecond, &aNanoseconds );
  cerr << dec 
       << aDay << "." << aMonth << "." << aYear << " " 
       << anHour << ":" << aMinute << ":" << aSecond << "." <<  aNanoseconds << endl;
}

void IO_test ()
{
  tERROR anError = errOK; 

  hIO anIO = 0;

  // check of object creating 
  cerr << "***** create a new instance of IO - no errors are expecting" << endl;
  anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &anIO ), IID_IO, PID_NFIO, SUBTYPE_ANY );
  cerr << "IO::sysCreateObject, error=" << formatted_hex << anError << endl; 
    
  // check of setting/getting properties
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set pgOBJECT_PATH property - no errors are expecting" << endl;
    char aPath [] = "/tmp";
    anError = anIO->propSetStr ( 0, pgOBJECT_PATH, aPath, sizeof ( aPath ) );
    cerr << "IO::propSetStr, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set pgOBJECT_NAME property - no errors are expecting" << endl;
    char aName [] = "tempfile.tmp";
    anError = anIO->propSetStr ( 0, pgOBJECT_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propSetStr, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anIO->propGetStr ( 0, pgOBJECT_FULL_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Full name = " << aName << endl;
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    anError = anIO->propSetStr ( 0, pgOBJECT_FULL_NAME, theTempFileName, strlen ( theTempFileName ) );
    cerr << "IO::propSetStr, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get pgOBJECT_FULL_NAME property - no errors are expecting" << endl;
    char aName [ MAX_PATH ] = {0};
    anError = anIO->propGetStr ( 0, pgOBJECT_FULL_NAME, aName, sizeof ( aName ) );
    cerr << "IO::propGetStr, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) )
      cerr << "Full name = " << aName << endl;
  }

  // check of object creating 
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** finish creating of the new instance of IO - no errors are expecting" << endl;
    anError = anIO->sysCreateObjectDone ();
    cerr << "IO::sysCreateObjectDone, error=" << formatted_hex << anError << endl;   
  }
 
  // check of getting size
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file size - no errors are expecting, size must be zero, the file is empty" << endl;
    tQWORD aSize = 0;
    anError = anIO->GetSize ( &aSize, 0 );
    cerr << "IO::GetSize, error=" << formatted_hex << anError << endl;   
    cerr << "size=" << dec << aSize << endl;   
  }
  
  // check of read/write functions
  if ( PR_SUCC ( anError ) ) {
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try read from the file " << dec << sizeof (aBuffer ) 
         << " bytes at zero offset - an error is expecting, result must be zero, the file is empty" << endl;
    tDWORD aResult = 0;
    anError = anIO->SeekRead ( &aResult, 0, aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekRead, error=" << formatted_hex << anError << endl;   
    cerr << "result=" << dec << aResult << " (must be 0)" << endl;    
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }
  
  if ( PR_SUCC ( anError ) ) {
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try write to the file " <<  sizeof ( aBuffer )
         << "bytes at zero offset - an error is expecting, result must be zero, the file is readonly" << endl;    
    memset ( aBuffer, 0x20, sizeof ( aBuffer ) );
    tDWORD aResult = 0;
    anError = anIO->SeekWrite ( &aResult, 0, aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekWrite, error=" << formatted_hex << anError << endl;   
    cerr << "result=" << dec << aResult << endl;    
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }

  if ( PR_SUCC ( anError ) ) {  
    cerr << "***** set read-write access mode - no errors" << endl;        
    anError = anIO->propSetDWord ( pgOBJECT_ACCESS_MODE, fACCESS_RW );
    cerr << "IO::propSetDword, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try write to the file " << dec << sizeof ( aBuffer )
         << " bytes at zero offset - no errors are expecting, result must be equal " << dec << sizeof ( aBuffer ) << endl;    
    tDWORD aResult = 0;
    anError = ( PR_SUCC ( anError ) ) ? anIO->SeekWrite ( &aResult, 0, aBuffer, sizeof ( aBuffer ) ) : anError;
    cerr << "IO::SeekWrite, error=" << formatted_hex << anError << endl;   
    cerr << "result=" << dec << aResult << endl;    
  }
  
  if ( PR_SUCC ( anError ) ) {
    tDWORD aResult = 0;
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try read from the file " << dec << sizeof (aBuffer ) 
         << " bytes at zero offset - no error are expecting, result must be " << dec << sizeof ( aBuffer ) << " bytes" << endl;
    anError = anIO->SeekRead ( &aResult, 0, aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekRead, error=" << formatted_hex << anError << " (must be 0)" << endl;   
    cerr << "result=" << dec << aResult << endl;
  }

  if ( PR_SUCC ( anError ) ) {
    tDWORD aResult = 0;
    tCHAR aBuffer [ MAX_PATH ] = {0};
    memset ( aBuffer, 0x20, sizeof ( aBuffer ) );
    cerr << "***** try write to the file " << dec << sizeof ( aBuffer )
         << " bytes at " << dec << sizeof ( aBuffer ) 
         << " offset - no errors are expecting, result must be equal " << dec << sizeof ( aBuffer ) << endl;   
    anError = anIO->SeekWrite ( &aResult, sizeof ( aBuffer ), aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekWrite, error=" << formatted_hex << anError << " (must be 0)" << endl;   
    cerr << "result=" << dec << aResult << endl;    
  }
   
  if ( PR_SUCC ( anError ) ) {
    tDWORD aResult = 0;
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try read from the file " << dec << sizeof ( aBuffer ) 
         << " bytes at " <<  sizeof ( aBuffer ) * 2 + 100  
         << "  offset - an error is expecting, result must be zero, attempt to read after end of the file" << endl;
    anError = anIO->SeekRead ( &aResult, sizeof ( aBuffer ) * 2 + 100, aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekRead, error=" << formatted_hex << anError << endl;   
    cerr << "result=" << dec << aResult << endl;    
  }

  if ( PR_SUCC ( anError ) ) {
    tDWORD aResult = 0;
    tCHAR aBuffer [ MAX_PATH ] = {0};
    cerr << "***** try read from the file " << dec << sizeof ( aBuffer ) 
         << " bytes at " << dec << sizeof ( aBuffer ) * 2 + 100  
         << "  offset - an error is expecting, result must be zero, attempt to read after end of the file" << endl;
    anError = anIO->SeekRead ( &aResult, sizeof ( aBuffer ) * 2, aBuffer, sizeof ( aBuffer ) );
    cerr << "IO::SeekRead, error=" << formatted_hex << anError << endl;   
    cerr << "result=" << dec << aResult << endl;    
  }

  // check of getting/setting size
  if ( PR_SUCC ( anError ) ) {    
    cerr << "***** get file size - no errors are expecting, size must be " << dec << MAX_PATH * 2 << " bytes" << endl;
    tQWORD aSize = 0;
    anError = anIO->GetSize ( &aSize, 0 );
    cerr << "IO::GetSize, error=" << formatted_hex << anError << endl;   
    cerr << "size=" << dec << aSize << endl;   
  }

  if ( PR_SUCC ( anError ) ) {    
    cerr << "***** set file size to 100 bytes - no errors are expecting" << endl;
    anError = anIO->SetSize ( 100 );
    cerr << "IO::SetSize, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {    
    cerr << "***** get file size - no errors are expecting, size must be 100 bytes" << endl;
    tQWORD aSize = 0;
    anError = anIO->GetSize ( &aSize, 0 );
    cerr << "IO::GetSize, error=" << formatted_hex << anError << endl;   
    cerr << "size=" << dec << aSize << endl;   
  }

  // check of get/set file time
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of creation - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_CREATION_TIME( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of creation: ";
      printTime ( aTime );
    }
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of last access  - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_LAST_ACCESS_TIME( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of last access: ";
      printTime ( aTime );
    }
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of last write - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_LAST_WRITE_TIME( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of last write: ";
      printTime ( aTime );
    }
  }

  if ( PR_SUCC ( anError ) ) {
    tDATETIME aDateTime;
    DTSet ( &aDateTime, 2010, 10, 10, 10, 10, 10, 0 );
    cerr << "***** set file time of creation - no errors are expecting, but modification time of creation is not allowed" << endl;
    anError = anIO->set_pgOBJECT_CREATION_TIME( &aDateTime );
    cerr << "IO::SetFileTime, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    tDATETIME aDateTime;
    DTSet ( &aDateTime, 2011, 11, 11, 11, 11, 11, 0 );
    cerr << "***** set file time of last access to 11.11.2011 11:11:11.0 - no errors are expecting" << endl;
    anError = anIO->set_pgOBJECT_LAST_ACCESS_TIME( &aDateTime );
    cerr << "IO::SetFileTime, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    tDATETIME aDateTime;
    DTSet ( &aDateTime, 2012, 12, 12, 12, 12, 12, 0 );
    cerr << "***** set file time of last write to 12.12.2012 12:12:12.0 - no errors are expecting" << endl;
    anError = anIO->set_pgOBJECT_LAST_WRITE_TIME( &aDateTime );
    cerr << "IO::SetFileTime, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of creation - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_CREATION_TIME( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of creation: ";
      printTime ( aTime );
    }
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of last access - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_LAST_ACCESS_TIME( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of last access: ";
      printTime ( aTime );
    }
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get file time of last write - no errors are expecting" << endl;
    tDATETIME aTime;
    anError = anIO->get_pgOBJECT_LAST_WRITE_TIME ( &aTime );
    cerr << "IO::GetFileTime, error=" << formatted_hex << anError << endl;   
    if ( PR_SUCC ( anError ) ) {
      cerr << "time of last write: ";
      printTime ( aTime );
    }
  }

  cout << "************************************************" << endl;
  if ( PR_SUCC ( anError ) )
    cout << "************** IO tests passed *****************" << endl;
  else
    cout << "************** IO tests failed *****************" << endl;    
  cout << "************************************************" << endl << endl;    
  if ( anIO ) {
    cerr << "***** destroy the instance of IO - no errors are expecting" << endl;
    anError = anIO->sysCloseObject ();
    anIO = 0;
    cerr << "IO::sysCloseObject, error=" << formatted_hex << anError << endl;   
  }
}

void plugin_test () 
{
  hSTRING aString = 0;
  tERROR anError = errOK;

  anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &aString ), IID_STRING );
  if ( PR_SUCC ( anError ) ) 
    anError = aString->ImportFromBuff ( 0, theTestDir, 0, cCP_ANSI, cSTRING_Z );    

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create a new directory \"" << theTestDir << "\" - no errors are expecting" << endl;
    anError = PrCreateDir ( reinterpret_cast <hOBJECT> ( aString ) );
    cerr << "PrCreateDir error=" << formatted_hex << anError << endl; 
  }

  anError = errOK; // directory may exist

  tCHAR aNewDir [ MAX_PATH ] = {0};
  strcat ( aNewDir, theTestDir );
  strcat ( aNewDir , "/" );
  strcat ( aNewDir, theTestDir );

  if ( PR_SUCC ( anError ) ) 
    anError = aString->ImportFromBuff ( 0, aNewDir, 0, cCP_ANSI, cSTRING_Z );    


  if ( PR_SUCC ( anError ) ) {
    cerr << "***** create a new directory \"" << aNewDir << "\" - no errors are expecting" << endl;
    anError = PrCreateDir ( reinterpret_cast <hOBJECT> ( aString ) );
    cerr << "PrCreateDir error=" << formatted_hex << anError << endl; 
  }
  
  anError = errOK; // directory may exist
  
  if ( PR_SUCC ( anError ) ) 
    anError = aString->ImportFromBuff ( 0, theTestDir, 0, cCP_ANSI, cSTRING_Z );    

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get a name of temp file - no errors are expecting" << endl;
    anError = PrGetTempFileName ( reinterpret_cast <hOBJECT> ( aString ), thePrefix, cCP_ANSI );
    cerr << "PrGetTempFileName, error=" << formatted_hex << anError << endl;
  }

  if ( PR_SUCC ( anError ) ) 
    aString->ExportToBuff (0, cSTRING_WHOLE, theTempFileName, sizeof ( theTempFileName ), cCP_ANSI, cSTRING_Z );


  hSTRING aNewString = 0;
  tCHAR aName [ MAX_PATH ] = {0};
  anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &aNewString ), IID_STRING );

  if ( PR_SUCC ( anError ) ) {
    strcat ( aName, theTestDir );
    strcat ( aName, "/" );
    strcat ( aName, theFileName );
    anError = aNewString->ImportFromBuff ( 0, aName, 0, cCP_ANSI, cSTRING_Z );    
  }

  IO_test ();

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** copy the file into itself without overwriting - an error is expecting, overwriting is not allowed" << endl;    
    anError = PrCopy ( reinterpret_cast <hOBJECT> ( aString ), reinterpret_cast <hOBJECT> ( aString ), 0 );
    cerr << "PrCopy, error=" << formatted_hex << anError << endl;
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }
   
  if ( PR_SUCC ( anError ) ) { 
    cerr << "***** copy the file into a different file - no errors are expecting" << endl;    
    anError = PrCopy ( reinterpret_cast <hOBJECT> ( aString ), reinterpret_cast <hOBJECT> ( aNewString ), fPR_FILE_REPLACE_EXISTING );
    cerr << "PrCopy, error=" << formatted_hex << anError << endl;
  }


  if ( PR_SUCC ( anError ) ) {
    cerr << "***** move the file into itself without overwriting - an error is expecting, overwriting is not allowed" << endl;    
    anError = PrMoveFileEx ( reinterpret_cast <hOBJECT> ( aString ), reinterpret_cast <hOBJECT> ( aString ), 0 );
    cerr << "PrMoveFileEx, error=" << formatted_hex << anError << endl;
    if ( PR_FAIL ( anError ) )
      anError = errOK;
  }

  if ( PR_SUCC ( anError ) ) {
    strcat ( aNewDir, "/" );    
    strcat ( aNewDir, theFileName );
    anError = aNewString->ImportFromBuff ( 0, aNewDir, 0, cCP_ANSI, cSTRING_Z );    
  }
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** move the file into a different file - no errors are expecting" << endl;    
    anError = PrMoveFileEx ( reinterpret_cast <hOBJECT> ( aString ), reinterpret_cast <hOBJECT> ( aNewString ), fPR_FILE_REPLACE_EXISTING );
    cerr << "PrMoveFileEx, error=" << formatted_hex << anError << endl;
  } 
  

  hSTRING anExecutable = 0;
  if ( PR_SUCC ( anError ) ) 
    anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &anExecutable ), IID_STRING );

  if ( PR_SUCC ( anError ) ) 
    anError = anExecutable->ImportFromBuff ( 0, theExecutable, 0, cCP_ANSI, cSTRING_Z );    
  
   hSTRING aCommandLine = 0;
   if ( PR_SUCC ( anError ) ) 
     anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &aCommandLine ), IID_STRING );
   
   if ( PR_SUCC ( anError ) ) 
     anError = aCommandLine->ImportFromBuff ( 0, theCmdLine, 0, cCP_ANSI, cSTRING_Z );    

   hSTRING aStartDir = 0;
   if ( PR_SUCC ( anError ) ) 
    anError = g_root->sysCreateObjectQuick ( reinterpret_cast <hOBJECT*> ( &aStartDir ), IID_STRING );

  if ( PR_SUCC ( anError ) )
    anError = aStartDir->ImportFromBuff ( 0, theStartDir, 0, cCP_ANSI, cSTRING_Z );    

  if ( PR_SUCC ( anError ) ) {
    cout << "***** execute a command \"" << theExecutable 
         << " " <<  theCmdLine 
         << "\" in \""  << theStartDir << "\" directory - no errors are expecting" << endl;
    tDWORD aResult = 0;
    anError = PrExecute ( reinterpret_cast <hOBJECT> ( anExecutable ), aCommandLine, aStartDir, &aResult,  fPR_EXECUTE_WAIT );
    cerr << "PrExecute, error=" << formatted_hex << anError << endl;
    if ( PR_SUCC ( anError ) )
      cout << "aResult=" << formatted_hex << aResult << endl;
  }
  if ( anExecutable ) {
    anExecutable->sysCloseObject ();
    anExecutable = 0;
  }

  if ( aCommandLine ) {
    aCommandLine->sysCloseObject ();
    aCommandLine = 0;
  }

  if ( aStartDir ) {
    aStartDir->sysCloseObject ();
    aStartDir = 0;
  }

 
  cout << "************************************************" << endl;
  if ( PR_SUCC ( anError ) )
    cout << "************ plugin tests passed ***************" << endl;
  else
    cout << "************ plugin tests failed ***************" << endl;
  cout << "************************************************" << endl << endl;   

  if ( aNewString ) {
    aNewString->sysCloseObject ();
    aNewString = 0;
  }

  if ( aString ) {
    aString->sysCloseObject ();
    aString = 0;
  }

}

int main()
{
  CPrague aPrague ( 0 );
  
  if ( !g_root ) {
    cerr << "Can't load Prague" << endl;
    return -1;
  }
  
  tERROR anError = errOK; 
  
 
  hIO anIO = 0;
  
  anError = g_root->sysCreateObjectQuick ( reinterpret_cast<hOBJECT*> ( &anIO ), IID_IO, PID_NFIO, SUBTYPE_ANY );
  if ( PR_SUCC ( anError ) ) {
    anIO->sysCloseObject ();
    anIO = 0;
  }

  cerr << "***** resolving import table - no errors are expecting" << endl;
  tDWORD anCount = 0;
  if ( PR_FAIL ( anError = g_root->ResolveImportTable ( &anCount,import_table,PID_NFIO ) ) )
    cerr << "resolve import table, error=" << formatted_hex << anError << endl;

  if ( PR_SUCC ( anError ) ) {
    plugin_test ();
    OS_test ();
    ObjPtr_test ();
  }

  return 0;
}
