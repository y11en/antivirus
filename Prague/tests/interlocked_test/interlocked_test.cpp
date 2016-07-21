#define PR_IMPEX_DEF

#include <iostream>
#include <iomanip>

#include <prague.h> 
#include <pr_loadr.h>
#include <iface/i_root.h>

using namespace std;

template <class Ch, class Tr> 
basic_ostream <Ch,Tr>& formatted_hex ( basic_ostream<Ch,Tr>& aStream ) 
{ 
  aStream << "0x" <<  setw ( 8 ) << setfill ( '0' ) << uppercase << hex;
  return aStream; 
}

hROOT g_root = 0;

#define IMPEX_IMPORT_LIB
#include <iface/e_loader.h>


#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table )
#include <iface/e_loader.h>
IMPORT_TABLE_END


int main()
{
  CPrague aPrague ( 0 );    
  
  if ( !g_root ) {
    cerr << "Can load Prague" << endl;
    return -1;
  }

  tERROR anError = errOK; 
  cerr << "***** resolving import table - no errors are expecting" << endl;
  tDWORD anCount = 0;
  if ( PR_FAIL ( anError = g_root->ResolveImportTable ( &anCount,import_table,PID_LOADER ) ) )
    cerr << "resolve import table, error=" << formatted_hex << anError << endl;

  if ( PR_SUCC ( anError ) ) {
    tLONG aValue = 10;
    if (  PrInterlockedIncrement( &aValue ) != 11 )
      anError = errNOT_OK;
    else
      cout << "PrInterlockedIncrement is OK" << endl;
  }

  if ( PR_SUCC ( anError ) ) {
    tLONG aValue = 10;
    if ( PrInterlockedDecrement( &aValue ) != 9 )
      anError = errNOT_OK;
    else
      cout << "PrInterlockedDecrement is OK" << endl;
  }

  if ( PR_SUCC ( anError ) ) {
    tLONG aValue = 10;
    if ( ( PrInterlockedExchange( &aValue, 100 ) != 10 ) || ( aValue != 100 ) )
      anError = errNOT_OK;
    else
      cout << "PrInterlockedExchanged is OK" << endl;
  }


  if ( PR_SUCC ( anError ) ) {
    tLONG aValue = 10;
    if ( ( PrInterlockedCompareExchange( &aValue, 100, 10 ) != 10 ) || ( aValue != 100 ) )
      anError = errNOT_OK;
    else
      cout << "PrInterlockedCompareExchange is OK" << endl;
  }

  if ( PR_SUCC ( anError ) ) {
    tLONG aValue = 10;
    if ( ( PrInterlockedCompareExchange( &aValue, 100, 100 ) != 10 ) || ( aValue != 10 ) )
      anError = errNOT_OK;
    else
      cout << "PrInterlockedCompareExchange is OK" << endl;
  }


  cout << "************************************************" << endl;
  if ( PR_SUCC ( anError ) )
    cout << "********* Interlocked tests passed *************" << endl;
  else
    cout << "********* Interlocked tests failed *************" << endl;    
  cout << "************************************************" << endl << endl;    

  return 0;
}
