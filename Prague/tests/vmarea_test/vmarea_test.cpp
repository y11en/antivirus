#define PR_IMPEX_DEF

#include <iostream>
#include <iomanip>

#include <prague.h> 
#include <pr_loadr.h>
#include <iface/i_root.h>
#include <iface/i_simple.h>

using namespace std;

template <class Ch, class Tr> 
basic_ostream <Ch,Tr>& formatted_hex ( basic_ostream<Ch,Tr>& aStream ) 
{ 
  aStream << "0x" <<  setw ( 8 ) << setfill ( '0' ) << uppercase << hex;
  return aStream; 
}

hROOT g_root = 0;

#define IMPEX_IMPORT_LIB
#include <plugin/p_vmarea.h> 
#include <iface/e_ktime.h>


#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table )
#include <iface/e_ktime.h>
#include <plugin/p_vmarea.h> 
IMPORT_TABLE_END



void test ()
{
  tERROR anError = errOK; 

  hOBJECT anArea = 0;

  cerr << "***** get page size " << endl;
  cerr << "***** page size  = " << vm_pagesize () << endl;
  

  // check of object creating 
  cerr << "***** create a new instance of VMArea - no errors are expecting" << endl;
  anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &anArea ), IID_SIMPLEOBJECT, PID_VMAREA, SUBTYPE_ANY );
  cerr << "VMArea::sysCreateObject, error=" << formatted_hex << anError << endl; 
    
  // check of setting/getting properties
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set plSIZE property - no errors are expecting" << endl;
    anError = anArea->propSetDWord ( plSIZE, 100 );
    cerr << "VMArea::propSetDWord, error=" << formatted_hex << anError << endl;   
  }

  // check of object creating 
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** finish creating of the new instance of VMArea - no errors are expecting" << endl;
    anError = anArea->sysCreateObjectDone ();
    cerr << "VMArea::sysCreateObjectDone, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get plSIZE property - no errors are expecting" << endl;
    tDWORD aSize = anArea->propGetDWord ( plSIZE );
    cerr << "Address = " << dec << aSize << endl;   
  }

  tBYTE* anAddress = 0;
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** get plADDRESS property - no errors are expecting" << endl;
    anAddress = (tBYTE*)anArea->propGetPtr ( plADDRESS );
    cerr << "Address = " << reinterpret_cast <void*> ( anAddress ) << endl;   
  }
  
  if ( PR_SUCC ( anError ) ) 
    *(anAddress + 10 ) = 1;
  
  if ( PR_SUCC ( anError ) ) {
    cerr << "***** set plPROTECTION property - no errors are expecting" << endl;
    anError = anArea->propSetDWord ( plPROTECTION, PROTECTION_READ );
    cerr << "VMArea::propSetDWord, error=" << formatted_hex << anError << endl;   
  }

  if ( PR_SUCC ( anError ) ) {
    tBYTE aByte = *(anAddress + 10 );
    cerr << "read OK: " <<  aByte << endl;       
  }
 
  if ( PR_SUCC ( anError ) ) {
    
    cerr << "***** testing for writing to write protected region, must be segmentation fault" << endl;           
    *(anAddress + 10 ) = 2;
    
    cerr << "write OK?: " <<   *(anAddress + 10 ) << endl;       
  } 
  cout << "************************************************" << endl;
  if ( PR_SUCC ( anError ) )
    cout << "************** VMArea tests passed *****************" << endl;
  else
    cout << "************** VMArea tests failed *****************" << endl;    
  cout << "************************************************" << endl << endl;    
  if ( anArea ) {
    cerr << "***** destroy the instance of VMArea - no errors are expecting" << endl;
    anError = anArea->sysCloseObject ();
    anArea = 0;
    cerr << "VMArea::sysCloseObject, error=" << formatted_hex << anError << endl;   
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

  hOBJECT anArea = 0;
  
  anError = g_root->sysCreateObjectQuick ( reinterpret_cast<hOBJECT*> ( &anArea ), IID_SIMPLEOBJECT, PID_VMAREA, SUBTYPE_ANY );
  if ( PR_SUCC ( anError ) ) {
    anArea->sysCloseObject ();
    anArea = 0;
  }

  cerr << "***** resolving import table - no errors are expecting" << endl;
  tDWORD anCount = 0;
  if ( PR_FAIL ( anError = g_root->ResolveImportTable ( &anCount,import_table,PID_VMAREA ) ) )
    cerr << "resolve import table, error=" << formatted_hex << anError << endl;

  if ( PR_SUCC ( anError ) ) {
    test ();
  }

  return 0;
}
