#include <fnmatch.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libgen.h>
#include <limits.h>

#include "diskio.h"

unsigned int getLogicalDiskByPhysicalDevice_oldStyle ( const char* aFileName, char* aBuffer, unsigned int aSize )
{
  DeviceTypes aType = getDeviceType ( aFileName );
  if ( aType == dtUnknown )
    return 0;
  if ( !aBuffer )
    return 0;
  if ( aType != dtPhysicalDevice ) {
    strncpy ( aBuffer, aFileName, aSize );
    return 1;
  }
  char aPattern [PATH_MAX] = {0};
  strncat ( aPattern, aFileName, sizeof ( aPattern ) - 1 );
  strncat ( aPattern, "[0-9]*", sizeof ( aPattern ) - strlen ( aPattern ) );
  glob_t aGlobBuffer;
  if ( glob ( aPattern, 0, 0, &aGlobBuffer ) )
    return 0;
  bool isFound = false;
  for ( unsigned int i = 0; i < aGlobBuffer.gl_pathc; ++i ) 
    if ( ( getDeviceType ( aGlobBuffer.gl_pathv [ i ] ) != dtUnknown ) ) 
      if ( !isFound ) {
        strncpy ( aPattern, aGlobBuffer.gl_pathv [ i ], sizeof ( aPattern ) );
        isFound = true;
      }
      else {
        isFound = false;
        break;
      }
  globfree (  &aGlobBuffer );
  if ( isFound )
    strncpy ( aBuffer, aPattern, aSize );
  return isFound;
}

#if defined (__linux__) || defined (__APPLE__)
// From linux/fs.h
#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */
#define BLKROGET   _IO(0x12,94)  /* get read-only status (0 = read_write) */

// fF <linux/hdreg.h>
#define HDIO_GETGEO             0x0301  /* get device geometry */
#define HDIO_GETGEO_BIG 0x0330  /* */

struct hd_geometry {
      unsigned char heads;
      unsigned char sectors;
      unsigned short cylinders;
      unsigned long start;
};

struct hd_big_geometry {
        unsigned char heads;
        unsigned char sectors;
        unsigned int cylinders;
        unsigned long start;
};

unsigned int getDeviceGeometry ( const char* aDevice, DiskGeometry * aGeometry )
{
  struct hd_big_geometry aHDBigGeometry;
  struct hd_geometry aHDGeometry;
  int	sector_size;

  int aFD = open (aDevice, O_RDONLY);

  if ( aFD < 0 )
    return 0;

  // Obtain disk geometry
  if (ioctl (aFD, HDIO_GETGEO_BIG, &aHDBigGeometry) == 0 ) {
    aGeometry->m_Heads = aHDBigGeometry.heads;
    aGeometry->m_Sectors = aHDBigGeometry.sectors;
    aGeometry->m_Cylinders = aHDBigGeometry.cylinders;
  }
  else if (ioctl (aFD, HDIO_GETGEO, &aHDGeometry) == 0 ) {
    aGeometry->m_Heads = aHDGeometry.heads;
    aGeometry->m_Sectors = aHDGeometry.sectors;
    aGeometry->m_Cylinders = aHDGeometry.cylinders;
  }
  else {
    aGeometry->m_Heads = 2;
    aGeometry->m_Sectors = 18;
    aGeometry->m_Cylinders = 80;
  }

  // Obtain disk sector size
  if ( ioctl (aFD, BLKSSZGET, &sector_size) == 0 )
    aGeometry->m_SectorSize = sector_size;
  else
    aGeometry->m_SectorSize = 512;	// pre-defined value for Linux
		
  close (aFD);
  return 1;
}


enum DeviceModes getDeviceMode ( const char* aFileName )
{
  int aFD = open ( aFileName, O_RDONLY );
  if ( aFD == -1 )
    return dmUnknown;
  int aROState = 0;
  int aResult =  ioctl ( aFD, BLKROGET, &aROState );
  close ( aFD );
  if ( aResult ) 
    return dmUnknown;
  if ( aROState )
    return dmReadOnly;
  aFD = open ( aFileName, O_RDWR );
  DeviceModes aMode = dmReadWrite;
  if ( ( aFD == -1 ) && ( errno == EROFS ) ) 
    aMode = dmReadOnly;
  if ( ( aFD == -1 ) && ( errno != EROFS ) ) 
    aMode = dmUnknown;
  close ( aFD );
  return aMode;

}

bool isDeviceFromDevFS ( const char* aFileName )
{
  if ( !fnmatch ( DEVICE_DIRECTORY "discs/disc?/disc", aFileName, 0 ) ||
       !fnmatch ( DEVICE_DIRECTORY "cdroms/cdrom?", aFileName, 0 )  ||
       !fnmatch ( DEVICE_DIRECTORY "discs/disc?/part*", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "floppy/?", aFileName, 0 ) ) 
    return true;
  return false;
}

enum DeviceTypes getDeviceType_oldStyle ( const char* aFileName )
{
  int aFD = open ( aFileName, O_RDONLY );
  if ( aFD == -1 )
    return dtUnknown;
  int aROState = 0;
  int aResult =  ioctl ( aFD, BLKROGET, &aROState );
  if ( aResult ) {
    close ( aFD ); 
    return dtUnknown;
  }
  char aTmpChar = 0x0;
  aResult = read ( aFD, &aTmpChar, sizeof ( aTmpChar ) );
  close ( aFD );
  if ( aResult != sizeof ( aTmpChar ) ) 
    return dtUnknown;
  if ( !fnmatch ( DEVICE_DIRECTORY "hd?", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "sd?", aFileName, 0 ) )
    return aROState ? dtCDROM : dtPhysicalDevice;
  if ( !fnmatch ( DEVICE_DIRECTORY "hd*", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "sd*", aFileName, 0 ) )
    return aROState ? dtUnknown : dtLogicalDisk;
  if ( !fnmatch ( DEVICE_DIRECTORY "fd?", aFileName, 0 ) )
    return dtFloppy;
  return dtUnknown;    
}


enum DeviceTypes getDeviceType_newStyle ( const char* aFileName )
{
  if ( !fnmatch ( DEVICE_DIRECTORY "discs/disc?/disc", aFileName, 0 ) )
    return dtPhysicalDevice;
  if ( !fnmatch ( DEVICE_DIRECTORY "cdroms/cdrom?", aFileName, 0 ) ) 
    return dtCDROM;
  if ( !fnmatch ( DEVICE_DIRECTORY "discs/disc?/part*", aFileName, 0 ) )  
    return dtLogicalDisk;
  if ( !fnmatch ( DEVICE_DIRECTORY "floppy/?", aFileName, 0 ) ) 
    return dtFloppy;
  return dtUnknown;    
}

enum DeviceTypes getDeviceType ( const char* aFileName )
{
  if ( isDeviceFromDevFS ( aFileName ) )
    return getDeviceType_newStyle ( aFileName );
  else
    return getDeviceType_oldStyle ( aFileName );
}

unsigned int getDevicePhysicalName_oldStyle ( const char* aSource, char* aDest, unsigned int aSize )
{
  if ( !fnmatch ( DEVICE_DIRECTORY "hd*", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "sd*", aSource, 0 ) ||
       !fnmatch ( DEVICE_DIRECTORY "fd?", aSource, 0 ) ) {
    if ( aDest && ( aSize >= ( sizeof ( DEVICE_DIRECTORY ) + 4 ) ) ){
      memmove ( aDest, aSource, sizeof ( DEVICE_DIRECTORY ) + 3  );
      aDest [ sizeof ( DEVICE_DIRECTORY ) + 3 ] = 0x0;
    }
    return sizeof ( DEVICE_DIRECTORY ) + 4;
  }
  return 0;
}

unsigned int getDevicePhysicalName_newStyle ( const char* aSource, char* aDest, unsigned int aSize )
{
  if ( !fnmatch ( DEVICE_DIRECTORY "discs/disc?/disc", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "cdroms/cdrom?", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "floppy/*", aSource, 0 ) ) {
    unsigned int aLen = strlen ( aSource ) + 1;
    if ( aDest && ( aSize >= aLen ) ) {
      strcpy ( aDest, aSource );
      return aLen;
    }
  }
  if ( !fnmatch ( DEVICE_DIRECTORY "discs/disc?/part*", aSource, 0 ) ) {
    char aBuffer [PATH_MAX] = {0};
    strncat ( aBuffer, aSource, sizeof ( aBuffer ) );
    dirname ( aBuffer );
    strncat ( aBuffer, "/disc", sizeof ( aBuffer ) - strlen ( aBuffer ) );
    return strlen ( aBuffer ) + 1;
  }
  return 0;
}

unsigned int getDevicePhysicalName ( const char* aSource, char* aDest, unsigned int aSize )
{
  if ( isDeviceFromDevFS ( aSource ) )
    return getDevicePhysicalName_newStyle ( aSource, aDest, aSize );
  else
    return getDevicePhysicalName_oldStyle ( aSource, aDest, aSize );
}

unsigned int (*selectDevicesPtr) ( glob_t& aGlobBuf );

unsigned int selectDevices ( glob_t* aGlobBuf )
{
  return selectDevicesPtr ( *aGlobBuf );
}

unsigned int selectDevices_oldStyle ( glob_t& aGlobBuf )
{
  unsigned int aResult = glob ( DEVICE_DIRECTORY "fd?", 0, 0, &aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "hd?", GLOB_APPEND, 0, &aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "hd[a-z][0-9]*", GLOB_APPEND, 0, &aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "sd?", GLOB_APPEND, 0, &aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "sd[a-z][0-9]*", GLOB_APPEND, 0, &aGlobBuf );
  return !aResult;
}

unsigned int selectDevices_newStyle ( glob_t& aGlobBuf )
{
  unsigned int aResult = glob ( DEVICE_DIRECTORY "floppy/?", 0, 0, &aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "cdroms/cdrom?", GLOB_APPEND, 0, &aGlobBuf );
  glob_t aDisksGlobBuf;
  if ( !glob ( DEVICE_DIRECTORY "discs/disc*", GLOB_MARK, 0, &aDisksGlobBuf ) )
    for ( unsigned int i = 0; i < aDisksGlobBuf.gl_pathc; ++i ) {
      char aBuffer [PATH_MAX] = {0};
      strncat ( aBuffer, aDisksGlobBuf.gl_pathv [i], sizeof ( aBuffer ) );
      strcat ( aBuffer, "*" );
      aResult &= glob ( aBuffer, GLOB_APPEND, 0, &aGlobBuf );
    }
  globfree ( &aDisksGlobBuf );
  return !aResult;
}

unsigned int getLogicalDiskByPhysicalDevice_newStyle ( const char* aFileName, char* aBuffer, unsigned int aSize )
{
  DeviceTypes aType = getDeviceType ( aFileName );
  if ( aType == dtUnknown )
    return 0;
  if ( !aBuffer )
    return 0;
  if ( aType != dtPhysicalDevice ) {
    strncpy ( aBuffer, aFileName, aSize );
    return 1;
  }

  char aPattern [PATH_MAX] = {0};
  strncat ( aPattern, aFileName, sizeof ( aPattern ) - 1 );
  dirname ( aPattern );
  strncat ( aPattern, "/*", sizeof ( aPattern ) - strlen ( aPattern ) );
  glob_t aGlobBuf;
  if ( glob ( aPattern, 0, 0, &aGlobBuf )  || ( aGlobBuf.gl_pathc != 2 ) ){
    globfree ( &aGlobBuf );
    return 0;
  }
  strncpy ( aBuffer, ( strcmp ( aFileName, aGlobBuf.gl_pathv [ 0 ] ) ) ? aGlobBuf.gl_pathv [ 0 ] : aGlobBuf.gl_pathv [ 1 ] , aSize );
  globfree ( &aGlobBuf );
  return 1;
}

unsigned int getLogicalDiskByPhysicalDevice ( const char* aFileName, char* aBuffer, unsigned int aSize )
{
  if ( isDeviceFromDevFS ( aFileName ) )
    return getLogicalDiskByPhysicalDevice_newStyle ( aFileName, aBuffer, aSize );
  else
    return getLogicalDiskByPhysicalDevice_oldStyle ( aFileName, aBuffer, aSize );
}

class LinuxDiskIOInitHelper {
public:
  LinuxDiskIOInitHelper ();
private:
  static bool isInitialized;
};

static LinuxDiskIOInitHelper theLinuxDiskIOInitHelper;

LinuxDiskIOInitHelper::LinuxDiskIOInitHelper ()
{
  if ( !isInitialized ) {
    isInitialized = true;
    if ( !access ( "/dev/.devfsd", F_OK ) || !access ( "/dev/.udev", F_OK )  ) {
      selectDevicesPtr = selectDevices_newStyle;
    }
    else {
      selectDevicesPtr = selectDevices_oldStyle;
    }
  }
}

bool LinuxDiskIOInitHelper::isInitialized = false;


#elif defined (__FreeBSD__)
#include <sys/disklabel.h>
#include <sys/disk.h>

unsigned int getDeviceGeometry (const char * aDevice, DiskGeometry * aGeometry)
{
  struct disklabel dl;

  memset (&dl, 0, sizeof(struct disklabel));
  int aFD = open (aDevice, O_RDONLY);

  if ( aFD < 0 )
    return false;

  // Obtain disk geometry
  if ( ioctl (aFD, DIOCGDINFO, &dl) == 0 ) {
    aGeometry->m_Heads = dl.d_ntracks;
    aGeometry->m_Sectors = dl.d_nsectors;
    aGeometry->m_Cylinders = dl.d_ncylinders;
    aGeometry->m_SectorSize = dl.d_secsize;
  }
  else {
    unsigned int aSectorSize;
    off_t aMediaSize;
    unsigned int aSectors;
    unsigned int aHeads;
    if ( !ioctl ( aFD, DIOCGSECTORSIZE, &aSectorSize ) &&
         !ioctl ( aFD, DIOCGMEDIASIZE, &aMediaSize ) &&
         !ioctl ( aFD, DIOCGFWSECTORS, &aSectors ) &&
         !ioctl ( aFD, DIOCGFWHEADS, &aHeads )) {
      aGeometry->m_Heads = aHeads;
      aGeometry->m_Sectors = aSectors;
      aGeometry->m_Cylinders = aMediaSize / aSectorSize / aSectors / aHeads;
      aGeometry->m_SectorSize = aSectorSize;
    }
    else {
      aGeometry->m_Heads = 2;
      aGeometry->m_Sectors = 18;
      aGeometry->m_Cylinders = 80;
      aGeometry->m_SectorSize = 512; 
    }
  }
  close (aFD);
  return true;
}

 unsigned int selectDevices ( glob_t* aGlobBuf )
{
  unsigned int aResult = glob ( DEVICE_DIRECTORY "fd?", 0, 0, aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "ad*", GLOB_APPEND, 0, aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "da*", GLOB_APPEND, 0, aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "acd?", GLOB_APPEND, 0, aGlobBuf );
  aResult &= glob ( DEVICE_DIRECTORY "cd?", GLOB_APPEND, 0, aGlobBuf );
  return !aResult;
}

enum DeviceTypes getDeviceType ( const char* aFileName )
{
  int aFD = open ( aFileName, O_RDONLY );
  if ( aFD == -1 )
    return dtUnknown;
  char aTmp [512] = {0};
  int aResult = read ( aFD, &aTmp, sizeof ( aTmp ) );
  close ( aFD );
  if ( aResult != sizeof ( aTmp ) ) 
    return dtUnknown;
  if ( !fnmatch ( DEVICE_DIRECTORY "ad?", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "da?", aFileName, 0 ) ) 
    return dtPhysicalDevice;
  if ( !fnmatch ( DEVICE_DIRECTORY "acd?", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "cd?", aFileName, 0 ) ) 
    return dtCDROM;
  if ( !fnmatch ( DEVICE_DIRECTORY "ad*", aFileName, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "da*", aFileName, 0 ) ) 
    return dtLogicalDisk;
  if ( !fnmatch ( DEVICE_DIRECTORY "fd?", aFileName, 0 ) ) 
    return dtFloppy;
  return dtUnknown;    
}

enum DeviceModes getDeviceMode ( const char* aFileName )
{
  int aFD = open ( aFileName, O_RDONLY );
  if ( aFD == -1 )
    return dmUnknown;
  close ( aFD );
  aFD = open ( aFileName, O_RDWR );
  DeviceModes aMode = dmReadWrite;
  if ( ( aFD == -1 ) && ( errno == EROFS ) ) 
    aMode = dmReadOnly;
  if ( ( aFD == -1 ) && ( errno != EROFS ) ) 
    aMode = dmUnknown;
  close ( aFD );
  return aMode;

}

unsigned int getDevicePhysicalName ( const char* aSource, char* aDest, unsigned int aSize )
{
  if ( !fnmatch ( DEVICE_DIRECTORY "ad?", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "da?", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "cd?", aSource, 0 ) || 
       !fnmatch ( DEVICE_DIRECTORY "fd?", aSource, 0 ) ) {
    if ( aDest && ( aSize >= ( sizeof (  DEVICE_DIRECTORY ) + 4  ) ) ){
      memmove ( aDest, aSource, sizeof (  DEVICE_DIRECTORY ) + 3 );
      aDest [ sizeof (  DEVICE_DIRECTORY ) + 3 ] = 0x0;
    }
    return sizeof (  DEVICE_DIRECTORY ) + 4;
  } 
  if ( !fnmatch ( DEVICE_DIRECTORY "acd?", aSource, 0 ) ){
    if (  aDest &&  ( aSize >= ( sizeof (  DEVICE_DIRECTORY ) + 5 ) ) ){
      memmove ( aDest, aSource, sizeof (  DEVICE_DIRECTORY ) + 4 );
      aDest [ sizeof (  DEVICE_DIRECTORY ) + 4 ] = 0x0;
    }
    return sizeof (  DEVICE_DIRECTORY ) + 5;
  }
  return 0;
}

unsigned int getLogicalDiskByPhysicalDevice ( const char* aFileName, char* aBuffer, unsigned int aSize )
{
  return getLogicalDiskByPhysicalDevice_oldStyle ( aFileName, aBuffer, aSize );
}

#else
#error "Implementation is needed"
#endif

