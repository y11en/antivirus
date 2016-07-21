#define KL_MAX_WORDSIZE 64
#include <kl_byteswap.h>

#include <fcntl.h>
#include <stdio.h>
#if defined (_WIN32)
#include <io.h>
#endif
#if defined (__unix__)
#include <unistd.h>
#define O_BINARY 0
#define _open open
#define _write write
#define _close close
#endif
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include <klsys/filemapping/wholefilemapping.h>
#include <klsys/memorymanager/guardedmm.h>
#include <klsys/guards/guard.h>

#include "../metadata/md5.h"

extern KLSysNS::GuardedMM<KLSysNS::Guard> theMetadataMemoryManager;

using KLSysNS::WholeFileMapping;

#include "../metadata.h"
#include "../metautils.h"

const char MetadataSignaturePart2 [] = "metadata";
const char MetadataSignaturePart1 [] = "Prague";

extern unsigned char* theStart;
extern unsigned char* theEnd;

unsigned char* load ( StaticProperty& aProperty, unsigned char* aBuffer );
unsigned char* save (  StaticProperty& aProperty, unsigned char* aBuffer, bool aNeedSwap );

unsigned char* load ( InterfaceMetadata& aMetadata, unsigned char* aBuffer );  
unsigned char* save ( InterfaceMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap );

unsigned char* load ( PluginMetadata& aMetadata, unsigned char* aBuffer );
unsigned char* loadExports ( PluginMetadata& aMetadata, unsigned char* aBuffer );
unsigned char* loadInterfaces ( PluginMetadata& aMetadata, unsigned char* aBuffer );
unsigned char* saveExports ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap );
unsigned char* saveInterfaces ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap );  
unsigned char* save ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap );

static bool outOfRange ( unsigned char* anAddr )
{
  if ( ( anAddr < theStart ) || ( anAddr > theEnd ) )
    return true;
  return false;
}

void swap ( short& aValue )
{
  aValue = kl_bswap_16(aValue);
}

void swap ( unsigned short& aValue )
{
  aValue = kl_bswap_16(aValue);
}

void swap ( int& aValue )
{
  aValue = kl_bswap_32(aValue);
}

void swap ( unsigned int& aValue )
{
  aValue = kl_bswap_32(aValue);
}

void swap ( long& aValue )
{
  aValue = kl_bswap_32(aValue);
}

void swap ( unsigned long& aValue )
{
  aValue = kl_bswap_32(aValue);
}

#if !defined(__LP64__)
void swap ( tQWORD& aValue )
{
  aValue = kl_bswap_64(aValue);
}

void swap ( tLONGLONG& aValue )
{
  aValue = kl_bswap_64(aValue);
}
#endif

void swap ( StaticProperty& aValue )
{
  swap ( aValue.thePropertyID );
  swap ( aValue.thePropertySize );
}

void swapStaticPropertyValue ( StaticProperty& aProperty, void* aValue )
{
  unsigned int aType = ( aProperty.thePropertyID & pTYPE_MASK ) >> 20;
  switch ( aType ) {
  case tid_WCHAR :
  case tid_WSTRING :
//#warning "Consideration is needed!"    
    break;

  case tid_CHAR    :
  case tid_BYTE    :
  case tid_SBYTE   :
  case tid_STRING  :    
  case tid_BINARY :
    break;
  
  default :
    switch ( aProperty.thePropertySize ) {
    case 2: 
      swap ( *( reinterpret_cast <tSHORT*> ( aValue ) ) );
      break;
    case 4: 
      swap ( *( reinterpret_cast <tDWORD*> ( aValue ) ) );
      break;
    case 8: 
      swap ( *( reinterpret_cast <tLONGLONG*> ( aValue ) ) );
      break;
    }
  }

}

void swap ( InterfaceMetadata& aValue )
{
  swap ( aValue.theStaticPropertiesCount );
  swap ( aValue.thePID );
  swap ( aValue.theSub );
  swap ( aValue.theVersion );
  swap ( aValue.theVID );
  swap ( aValue.theFlags );
  swap ( aValue.theCompat );
  swap ( aValue.theIID );
  swap ( aValue.theCodePage );
  //-- add byteswapping of a new field here 

}

void swap ( PluginMetadata& aValue )
{
  swap ( aValue.theInterfacesCount );
  swap ( aValue.theExportsCount );
  swap ( aValue.theAutoStart );
  swap ( aValue.thePluginID );
  swap ( aValue.theCodePage );
  //-- add byteswapping of a new field here 
}

unsigned char* load ( StaticProperty& aProperty, unsigned char* aBuffer )
{
  if ( outOfRange ( aBuffer + sizeof ( StaticProperty ) - sizeof ( aProperty.thePropertyValue ) ) )
    return 0;
  memmove ( &aProperty, aBuffer, sizeof ( StaticProperty ) - sizeof (  aProperty.thePropertyValue )  );
  
  aBuffer += sizeof ( StaticProperty ) - sizeof (  aProperty.thePropertyValue );
#if defined (KL_BIG_ENDIAN)
  swap ( aProperty );
#endif
  if ( outOfRange ( aBuffer +  aProperty.thePropertySize ) )
    return 0;
  aProperty.thePropertyValue = theMetadataMemoryManager.allocate ( aProperty.thePropertySize );
  if ( !aProperty.thePropertyValue )
    return 0;
  memmove ( aProperty.thePropertyValue, aBuffer, aProperty.thePropertySize );
#if defined (KL_BIG_ENDIAN)
  swapStaticPropertyValue ( aProperty, aProperty.thePropertyValue );
#endif  
  return aBuffer + aProperty.thePropertySize;
}

unsigned char* save ( StaticProperty& aProperty, unsigned char* aBuffer, bool aNeedSwap )
{
  tDWORD aSize = sizeof ( StaticProperty ) - sizeof ( aProperty.thePropertyValue );
  memmove ( aBuffer, &aProperty, aSize );
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap ) 
    swap ( *(reinterpret_cast<StaticProperty*> ( aBuffer ) ) );
#endif
  aBuffer += sizeof ( StaticProperty ) - sizeof ( aProperty.thePropertyValue );
  memmove ( aBuffer, aProperty.thePropertyValue, aProperty.thePropertySize );
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap ) 
    swapStaticPropertyValue ( aProperty, aBuffer );
#endif
  return aBuffer + aProperty.thePropertySize;
}

unsigned char* load ( InterfaceMetadata& aMetadata, unsigned char* aBuffer )
{
  if ( outOfRange ( aBuffer + sizeof ( tDWORD ) ) )
    return 0;
  tDWORD aSize;
  memmove ( &aSize, aBuffer, sizeof ( tDWORD ) );
#if defined (KL_BIG_ENDIAN)
  swap ( aSize );
#endif
  aBuffer += sizeof ( tDWORD );
  tDWORD aNeededSize = ( aSize >= ( sizeof ( InterfaceMetadata ) ) - sizeof ( aMetadata.theStaticProperties  ) ) ? 
    sizeof ( InterfaceMetadata ) - sizeof ( aMetadata.theStaticProperties  ): 
    aSize;
  if ( outOfRange ( aBuffer + aNeededSize ) )
    return 0;  
  memmove ( &aMetadata, aBuffer, aNeededSize );
#if defined (KL_BIG_ENDIAN)
  swap ( aMetadata );
#endif
  aBuffer += aSize;
  if ( !aMetadata.theStaticPropertiesCount ) 
    return aBuffer;
  if ( !aMetadata.allocateStaticProperties () )
    return 0;
  for ( unsigned int i = 0; i < aMetadata.theStaticPropertiesCount; i++ ) {
    aBuffer = load ( aMetadata.theStaticProperties [i], aBuffer );
    if ( !aBuffer )
      return 0; 
  }
  return aBuffer;
}

unsigned char* save ( InterfaceMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap )
{
  tDWORD aSize = sizeof ( InterfaceMetadata ) - sizeof ( aMetadata.theStaticProperties );
  memmove ( aBuffer, &aSize, sizeof ( tDWORD ) );  
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap ) 
    swap ( *(reinterpret_cast<tDWORD*> ( aBuffer ) ) );
#endif
  aBuffer += sizeof ( tDWORD );
  memmove ( aBuffer, &aMetadata, aSize );
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap ) 
    swap ( *(reinterpret_cast<InterfaceMetadata*> ( aBuffer ) ) );
#endif
  aBuffer += aSize;
  if ( !aMetadata.theStaticPropertiesCount ) 
    return aBuffer;
  for ( unsigned int i = 0; i < aMetadata.theStaticPropertiesCount; i++ )
    aBuffer = save ( aMetadata.theStaticProperties [i], aBuffer, aNeedSwap  );
  return aBuffer;
}

unsigned char* load ( PluginMetadata& aMetadata, unsigned char* aBuffer )
{
  if ( outOfRange ( aBuffer + sizeof ( tDWORD ) ) )
    return 0;
  tDWORD aSize;
  memmove ( &aSize, aBuffer, sizeof ( tDWORD ) );
#if defined (KL_BIG_ENDIAN)
  swap ( aSize );
#endif
  aBuffer += sizeof ( tDWORD );
  tDWORD aNeededSize = ( aSize >= ( sizeof ( PluginMetadata ) - 
                                    sizeof ( aMetadata.theExports ) - 
                                    sizeof ( aMetadata.theInterfaces ) ) ) ? 
    sizeof ( PluginMetadata ) -  sizeof ( aMetadata.theExports ) - sizeof ( aMetadata.theInterfaces ) : 
    aSize;
  memmove ( &aMetadata, aBuffer, aNeededSize );
#if defined (KL_BIG_ENDIAN)
  swap ( aMetadata );
#endif
  return aBuffer + aSize;
}

unsigned char* save ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap )
{
  tDWORD aSize = sizeof ( PluginMetadata ) - sizeof ( aMetadata.theExports ) - sizeof ( aMetadata.theInterfaces );
  memmove ( aBuffer, &aSize, sizeof ( tDWORD ) );  
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap ) 
    swap ( *(reinterpret_cast<tDWORD*> ( aBuffer ) ) );
#endif
  aBuffer += sizeof ( tDWORD );
  memmove ( aBuffer, &aMetadata, aSize );
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap )   
    swap ( *(reinterpret_cast<PluginMetadata*> ( aBuffer ) ) );
#endif
  return aBuffer + aSize;
}

unsigned char* loadExports ( PluginMetadata& aMetadata, unsigned char* aBuffer )
{
  if ( !aMetadata.theExportsCount )
    return aBuffer;
  tDWORD aSize = aMetadata.theExportsCount * sizeof ( tDWORD );
  if ( outOfRange ( aBuffer + aSize ) ) 
    return 0;
  // allocating memory blocks 
  if ( !aMetadata.allocateExports () ) 
    return 0;
  // reading the export and the interfaces
  for ( unsigned int i = 0; i < aMetadata.theExportsCount; i++ ) {
    memmove ( &aMetadata.theExports [ i ] , aBuffer + sizeof ( tDWORD ) * i, sizeof ( tDWORD ) );
#if defined (KL_BIG_ENDIAN)
    swap ( aMetadata.theExports [ i ] );
#endif
  }
  return aBuffer + aSize;
}

unsigned char* loadInterfaces ( PluginMetadata& aMetadata, unsigned char* aBuffer )
{
  if ( !aMetadata.theInterfacesCount )
    return aBuffer;
  if ( !aMetadata.allocateInterfaces () )
    return 0;
  for ( unsigned int i = 0; i < aMetadata.theInterfacesCount; i++ ) {
    aBuffer = load ( aMetadata.theInterfaces [ i ], aBuffer );
    if ( !aBuffer )
      return 0;
  }
  return aBuffer;
}

int load ( PluginMetadata& aMetadata, const char* aFileName )
{
  if ( !aFileName )
    return 0;

  WholeFileMapping::kl_size_t aMappedSize = 0;
  WholeFileMapping aFileMapping;
  unsigned char* aMap = reinterpret_cast<unsigned char*> ( aFileMapping.map ( aFileName, KL_FILEMAP_READ, &aMappedSize ) );
  if ( !aMap || ( aMap == reinterpret_cast<unsigned char*> ( -1 ) ) )
    return 0;

  theEnd = aMap + aMappedSize; 
  unsigned char* aPosition = theEnd - sizeof ( MetadataSignaturePart1 ) - sizeof ( MetadataSignaturePart2 );

  // searching a metadata marker
  while ( ( aPosition >= aMap ) && 
          ( memcmp ( aPosition, MetadataSignaturePart1,  sizeof ( MetadataSignaturePart1 ) ) ||
            memcmp ( aPosition +  sizeof ( MetadataSignaturePart1 ), MetadataSignaturePart2,  sizeof ( MetadataSignaturePart2 ) ) ) )
    --aPosition;
  
  if ( aPosition < aMap )
    return 0;

  theStart = aPosition;

  aPosition += sizeof ( MetadataSignaturePart1 ) + sizeof ( MetadataSignaturePart2 );
  
  aPosition = load ( aMetadata, aPosition );
  if ( !aPosition ) {
    aMetadata.clear ();
    return 0;
  }

  aPosition = loadExports ( aMetadata, aPosition );
  if ( !aPosition ) {
    aMetadata.clear ();
    return 0;
  }

  aPosition = loadInterfaces ( aMetadata, aPosition );
  if ( !aPosition ) {
    aMetadata.clear ();
    return 0;
  }

  if ( !aMetadata.check_md5 ( theStart, aPosition ) ) {
    aMetadata.clear ();
    return 0;
  }

  return 1;
}


unsigned char* saveExports ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap )
{
  if ( !aMetadata.theExportsCount )
    return aBuffer;
  memmove ( aBuffer, aMetadata.theExports, sizeof ( tDWORD ) * aMetadata.theExportsCount );
#if defined (KL_LITTLE_ENDIAN)
  if ( aNeedSwap )
    for ( unsigned int i = 0; i < aMetadata.theExportsCount; i++ ) 
      swap ( *( reinterpret_cast<tDWORD*> ( aBuffer + sizeof ( tDWORD ) * i ) ) );
#endif
  return aBuffer + sizeof ( tDWORD ) * aMetadata.theExportsCount;
}

unsigned char* saveInterfaces ( PluginMetadata& aMetadata, unsigned char* aBuffer, bool aNeedSwap )
{
  if ( !aMetadata.theInterfacesCount )
    return aBuffer;
  for ( unsigned int i = 0; i < aMetadata.theInterfacesCount; i++ )
    aBuffer = save ( aMetadata.theInterfaces [ i ], aBuffer, aNeedSwap );
  return aBuffer;
}

int save ( PluginMetadata& aMetadata, const char* aFileName, bool aNeedSwap )
{
  int aDataSize = 
    sizeof ( MetadataSignaturePart1 ) + sizeof ( MetadataSignaturePart2 ) + 
    sizeof ( tDWORD ) + sizeof ( PluginMetadata ) - sizeof ( aMetadata.theExports ) - sizeof ( aMetadata.theInterfaces ) + 
    sizeof ( tDWORD ) * aMetadata.theExportsCount;
  
  for ( unsigned int i = 0; i < aMetadata.theInterfacesCount; i++ ) {
    aDataSize += sizeof ( tDWORD ) + sizeof ( InterfaceMetadata ) - sizeof ( aMetadata.theInterfaces [ i ].theStaticProperties );
    for ( unsigned int j = 0; j < aMetadata.theInterfaces [ i ].theStaticPropertiesCount; j++ )
      aDataSize += sizeof ( StaticProperty ) - sizeof ( aMetadata.theInterfaces [ i ].theStaticProperties [ j ].thePropertyValue ) + 
        aMetadata.theInterfaces [ i ].theStaticProperties [ j ].thePropertySize;
  }

  unsigned char* aBuffer = reinterpret_cast <unsigned char*> ( theMetadataMemoryManager.allocate ( aDataSize ) );

  if ( !aBuffer ) 
    return 0;

  int aFD = _open ( aFileName, O_BINARY | O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE );
  if ( aFD < 0)
    return 0;

  unsigned char* aPosition = aBuffer;
  memcpy ( aPosition, MetadataSignaturePart1, sizeof ( MetadataSignaturePart1 ) );
  aPosition += sizeof ( MetadataSignaturePart1 );
  memcpy ( aPosition, MetadataSignaturePart2, sizeof ( MetadataSignaturePart2 ) );
  aPosition += sizeof ( MetadataSignaturePart2 );

  aPosition = save ( aMetadata,  aPosition, aNeedSwap );
  if ( !aPosition ) {
    _close ( aFD );
    theMetadataMemoryManager.deallocate ( aBuffer );
    return 0;
  }
  aPosition = saveExports ( aMetadata, aPosition, aNeedSwap  );
  if ( !aPosition ) {
    _close ( aFD );
    theMetadataMemoryManager.deallocate ( aBuffer );
    return 0;
  }

  aPosition = saveInterfaces ( aMetadata, aPosition, aNeedSwap  );
  if ( !aPosition ) {
    _close ( aFD );
    theMetadataMemoryManager.deallocate ( aBuffer );
    return 0;
  }

  unsigned char aChecksum [ 16 ];
  MD5_CTX aCtx;
  MD5Init ( &aCtx );
  MD5Update ( &aCtx, aBuffer, aDataSize );
  MD5Final ( aChecksum, &aCtx );

  int aResult = ( _write ( aFD, aBuffer, aDataSize ) == aDataSize ) && 
    ( _write ( aFD, aChecksum, sizeof ( aChecksum ) ) == sizeof ( aChecksum ) );
  
  _close ( aFD );
  theMetadataMemoryManager.deallocate ( aBuffer );
  return aResult;
}

