#include <iostream>
#include <iomanip>

#include "../metadata.h"

using namespace std;

template <class Ch, class Tr>
basic_ostream <Ch,Tr>& formatted_hex(basic_ostream<Ch,Tr>& Stream)
{
	return Stream << "0x"<< setw(8) << setfill('0') << uppercase << hex;
}


void printPluginMetadata ( PragueMetadata& aPluginMetadata );


int main ( int argc, char** argv )
{
  if ( argc < 2 ) {
    cout << "Metadata info" << endl;
    cout << "Usage: metainfo <metadata filename>" << endl;
    return -1;
  }
  //  int count=1000;
  //  while(count--){
  PragueMetadata aMetadata;
  for ( int i = 1; i < argc; i++ ) {
    if ( !load( argv [ i ], aMetadata ) ) {
      cerr << "Can't load metadata from " << argv [ i ] << endl;
      continue;
    }
  }
  //  }
  cout << "******************************" << endl;
  printPluginMetadata ( aMetadata );
  cout << "******************************" << endl;
  return 0;
}

void printStaticProperty ( StaticProperty& aStaticProperty )
{
  cout << "    property ID: " << formatted_hex << aStaticProperty.thePropertyID << endl;
  cout << "    property size: " << dec << aStaticProperty.thePropertySize << endl;
  cout << "    property type: ";
  unsigned int aType = ( aStaticProperty.thePropertyID & pTYPE_MASK ) >> 20;
  switch ( aType ) {
  case tid_CHAR: {
    cout << "tCHAR" << endl;
    tCHAR* aTmp = reinterpret_cast <tCHAR*> ( aStaticProperty.thePropertyValue );
    cout << "    value " << *aTmp << endl;
    break;
  }
  case tid_SBYTE: {
    cout << "tSBYTE" << endl;
    tSBYTE* aTmp = reinterpret_cast <tSBYTE*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << static_cast<int>( *aTmp ) << endl;
    break;
  }
  case tid_BYTE: {
    cout << "tBYTE" << endl;
    tBYTE* aTmp = reinterpret_cast <tBYTE*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << static_cast<int>( *aTmp ) << endl;
    break;
  }
  case tid_SHORT: {
    cout << "tSHORT" << endl;
    tSHORT* aTmp = reinterpret_cast <tSHORT*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_WORD: {
    cout << "tWORD" << endl;
    tWORD* aTmp = reinterpret_cast <tWORD*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_WCHAR: {
    cout << "tWCHAR" << endl;
    tWCHAR* aTmp = reinterpret_cast <tWCHAR*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_INT: {
    cout << "tINT" << endl;
    tINT* aTmp = reinterpret_cast <tINT*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_LONG: {
    cout << "tLONG" << endl;
    tLONG* aTmp = reinterpret_cast <tLONG*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_BOOL: {
    cout << "tBOOL" << endl;
    tBOOL* aTmp = reinterpret_cast <tBOOL*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_DWORD: {
    cout << "tDWORD" << endl;
    tDWORD* aTmp = reinterpret_cast <tDWORD*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_CODEPAGE: {
    cout << "tCODEPAGE" << endl;
    tCODEPAGE* aTmp = reinterpret_cast <tCODEPAGE*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_ERROR: {
    cout << "tERROR" << endl;
    tERROR* aTmp = reinterpret_cast <tERROR*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_UINT: {
    cout << "tUINT" << endl;
    tUINT* aTmp = reinterpret_cast <tUINT*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_IID: {
    cout << "tIID" << endl;
    tIID* aTmp = reinterpret_cast <tIID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_PID: {
    cout << "tPID" << endl;
    tPID* aTmp = reinterpret_cast <tPID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_ORIG_ID: {
    cout << "tORIG_ID" << endl;
    tORIG_ID* aTmp = reinterpret_cast <tORIG_ID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_OS_ID: {
    cout << "tOS_ID" << endl;
    tOS_ID* aTmp = reinterpret_cast <tOS_ID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_VID: {
    cout << "tVID" << endl;
    tVID* aTmp = reinterpret_cast <tVID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_PROPID: {
    cout << "tPROPID" << endl;
    tPROPID* aTmp = reinterpret_cast <tPROPID*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_VERSION: {
    cout << "tVERSION" << endl;
    tVERSION* aTmp = reinterpret_cast <tVERSION*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << formatted_hex << *aTmp << endl;
    break;
  }
  case tid_DATA: {
    cout << "tDATA" << endl;
    tDATA* aTmp = reinterpret_cast <tDATA*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_LONGLONG: {
    cout << "tLONGLONG" << endl;
    //tLONGLONG* aTmp = reinterpret_cast <tLONGLONG*> ( aStaticProperty.thePropertyValue );
    //cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_QWORD: {
    cout << "tQWORD" << endl;
    //tQWORD* aTmp = reinterpret_cast <tQWORD*> ( aStaticProperty.thePropertyValue );
    //cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_DATETIME: {
    cout << "tDATETIME" << endl;
    tDATETIME* aTmp = reinterpret_cast <tDATETIME*> ( aStaticProperty.thePropertyValue );
    cout << "    value: " << *aTmp << endl;
    break;
  }
  case tid_STRING: {
    cout << "tSTRING" << endl;
    tSTRING aTmp = reinterpret_cast <tSTRING> ( aStaticProperty.thePropertyValue );
    cout << "    value: \"" << aTmp << "\"" << endl;
    break;
  }
  case tid_WSTRING: {
    cout << "tWSTRING" << endl;
    cout << "    value: ";
    for ( unsigned int i = 0; i < aStaticProperty.thePropertySize; i++ ) {
      int aTmp = ( reinterpret_cast <unsigned char*> ( aStaticProperty.thePropertyValue ) ) [ i ];
      cout << setw(2) << setfill('0') << uppercase << hex << aTmp << " ";
    }
    cout << endl;
    break;
  }
  case tid_BINARY: {
    cout << "tBINARY" << endl;
    cout << "    value: ";
    for ( unsigned int i = 0; i < aStaticProperty.thePropertySize; i++ ) {
      int aTmp = ( reinterpret_cast <unsigned char*> ( aStaticProperty.thePropertyValue ) ) [ i ];
      cout << setw(2) << setfill('0') << uppercase << hex << aTmp << " ";
    }
    cout << endl;
    break;
  }
  case tid_IFACEPTR: {
    cout << "tIFACEPTR" << endl;
    cout << "    value: ";
    for ( unsigned int i = 0; i < aStaticProperty.thePropertySize; i++ ) {
      int aTmp = ( reinterpret_cast <unsigned char*> ( aStaticProperty.thePropertyValue ) ) [ i ];
      cout << setw(2) << setfill('0') << uppercase << hex << aTmp << " ";
    }
    cout << endl;
    break;
  }
  case tid_PTR: {
    cout << "tPTR" << endl;
    cout << "    value: ";
    for ( unsigned int i = 0; i < aStaticProperty.thePropertySize; i++ ) {
      int aTmp = ( reinterpret_cast <unsigned char*> ( aStaticProperty.thePropertyValue ) ) [ i ];
      cout << setw(2) << setfill('0') << uppercase << hex << aTmp << " ";
    }
    cout << endl;
    break;
  }
  case tid_OBJECT: {
    cout << "tOBJECT" << endl;
    cout << "    value: ";
    for ( unsigned int i = 0; i < aStaticProperty.thePropertySize; i++ ) {
      int aTmp = ( reinterpret_cast <unsigned char*> ( aStaticProperty.thePropertyValue ) ) [ i ];
      cout << setw(2) << setfill('0') << uppercase << hex << aTmp << " ";
    }
    cout << endl;
    break;
  }
  default :
    cout << "unexpected type?" << endl;
  }
}

void printInterfaceMetadata ( InterfaceMetadata& aMetadata )
{
  cout << "  plugin ID: " << formatted_hex << aMetadata.thePID << endl;
  cout << "  subtype ID: " << formatted_hex << aMetadata.theSub << endl;
  cout << "  version: " << formatted_hex << aMetadata.theVersion << endl;
  cout << "  vendor ID: " << formatted_hex << aMetadata.theVID << endl;
  cout << "  flags: " << formatted_hex << aMetadata.theFlags << endl;
  cout << "  compatibility: " << formatted_hex << aMetadata.theCompat << endl;
  cout << "  interface ID: " << formatted_hex << aMetadata.theIID << endl;
  cout << "  code page: " << formatted_hex << aMetadata.theCodePage << endl;
  
  if ( aMetadata.theStaticPropertiesCount ) {
    cout << "------------------------------" << endl;
    cout << "  static properties: " << endl;
  }
  for ( unsigned int i = 0; i < aMetadata.theStaticPropertiesCount; i++ ) {
    cout << "------------------------------" << endl;
    cout << "   property number " << dec << i << endl;
    printStaticProperty ( aMetadata.theStaticProperties [ i ] );

  }
}

void printPluginMetadata ( PragueMetadata& aMetadatas )
{
	unsigned int count = 0;
	while( const PluginMetadata* aMetadata = aMetadatas.getMetadata(count)){
		cout << "+++++++++++++++++++++++++++++++" << endl;
		cout << "metadata file: " << aMetadatas.getFileName(count++) << endl;		
		cout << "plugin ID: " << formatted_hex << aMetadata->thePluginID << endl;
		cout << "auto start: " << static_cast <bool>( aMetadata->theAutoStart ) << endl;
		cout << "code page: " << formatted_hex << aMetadata->theCodePage << endl;

		if ( aMetadata->theExportsCount ) {
			cout << "==============================" << endl;
			cout << "exports:" << endl;
		}
		for ( unsigned int i = 0; i < aMetadata->theExportsCount; i++ ) {
			cout << "  " << formatted_hex << aMetadata->theExports [ i ] << endl;
		}

		if ( aMetadata->theInterfacesCount ) {
			cout << "==============================" << endl;
			cout << "interfaces:" << endl;
		}
		for ( unsigned int j = 0; j < aMetadata->theInterfacesCount; j++ ) {
			cout << "==============================" << endl;
			printInterfaceMetadata ( aMetadata->theInterfaces [ j ] );
		}
	}
}

