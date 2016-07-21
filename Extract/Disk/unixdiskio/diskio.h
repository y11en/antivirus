#ifndef _DISKIO_H_
#define _DISKIO_H_

#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <libgen.h>
#include <glob.h>

#define DEVICE_DIRECTORY "/dev/"

typedef struct tag_DiskGeometry
{
  unsigned char m_Heads;
  unsigned char m_Sectors;
  unsigned int	m_Cylinders;
  unsigned int	m_SectorSize;	// in bytes
} DiskGeometry;

#if defined (__cplusplus)
extern "C" {
#endif
  unsigned int getDeviceGeometry ( const char*, DiskGeometry* );
  
  enum DeviceTypes { dtUnknown, dtFloppy, dtPhysicalDevice, dtLogicalDisk, dtCDROM };
  
  enum DeviceTypes getDeviceType ( const char* );
  
  enum DeviceModes { dmUnknown, dmReadOnly, dmReadWrite };
  
  enum DeviceModes getDeviceMode ( const char* );
  
  unsigned int getDevicePhysicalName ( const char*, char*, unsigned int );

  unsigned int selectDevices ( glob_t* );

  unsigned int getLogicalDiskByPhysicalDevice ( const char*, char*, unsigned int );

#if defined (__cplusplus)
}
#endif

#endif
