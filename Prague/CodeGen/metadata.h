#ifndef _METADATA_H_
#define _METADATA_H_

#include "prague.h"

#pragma pack(4) 

struct StaticProperty {
  StaticProperty () 
    : thePropertyID ( 0 ),
      thePropertySize ( 0 ),
      thePropertyValue ( 0 ) {}

  ~StaticProperty ();

  int allocateValue ();
  void clear ();
  unsigned char* load ( unsigned char* aBuffer, unsigned char* anEnd );

  tDWORD thePropertyID;
  tDWORD thePropertySize;
  tPTR thePropertyValue;
};

struct InterfaceMetadata
{
  InterfaceMetadata ()
    : theStaticPropertiesCount ( 0 ),
      thePID ( PID_NONE ),
      theSub ( 0 ),
      theVersion ( 0 ),
      theVID ( VID_ANY ),
      theFlags ( 0 ),
      theCompat ( IID_NONE ),
      theIID ( IID_NONE ),
      theCodePage ( cCP_ANSI ),
      theStaticProperties ( 0 ) {}

  ~InterfaceMetadata ();
  int allocateStaticProperties ();
  void clear ();  
  unsigned char* load ( unsigned char* aBuffer, unsigned char* anEnd );  

  tDWORD          theStaticPropertiesCount;
  tPID            thePID;
  tDWORD          theSub;
  tVERSION        theVersion;
  tVID            theVID;
  tDWORD          theFlags;
  tIID            theCompat;
  tIID            theIID;
  tCODEPAGE       theCodePage;
  // -- place new fields here
  // -- don't forget to add byteswapping of an every new field to function from metautils.cpp (signature of the function see below)
  // -- unsigned char* loadReversed ( InterfaceMetadata&, unsigned char*, unsigned char*)

  // -- end of place for new fields
  StaticProperty* theStaticProperties;
};

struct PluginMetadata
{
  PluginMetadata ()
    : theInterfacesCount ( 0 ),
      theExportsCount ( 0 ),
      theAutoStart ( cFALSE ),
      thePluginID ( PID_NONE ),
      theCodePage ( cCP_ANSI ),
      theExports ( 0 ),
      theInterfaces ( 0 ) {}

  ~PluginMetadata ();
  
  int allocateExports ();
  int allocateInterfaces ();
  void clear ();
  int check ( unsigned char* aCheckStart, unsigned char* aCheckEnd , unsigned char* anEnd );
  unsigned char* loadExports ( unsigned char* aBuffer, unsigned char* anEnd );
  unsigned char* loadInterfaces ( unsigned char* aBuffer, unsigned char* anEnd );
  unsigned char* load ( unsigned char* aBuffer, unsigned char* anEnd );
  int load ( const char* aFileName ); 

  tDWORD          theInterfacesCount;
  tDWORD          theExportsCount;
  tBOOL           theAutoStart;
  tDWORD          thePluginID;
  tCODEPAGE       theCodePage;
  // -- place new fields here
  // -- don't forget to add byteswapping of an every new field to function from metautils.cpp (signature of the function see below)
  // -- unsigned char* loadReversed ( PluginMetadata&, unsigned char*, unsigned char*)

  // -- end of place for new fields
  tDWORD* theExports;
  InterfaceMetadata* theInterfaces;
};

#pragma pack ()

#endif //_METADATA_H_
