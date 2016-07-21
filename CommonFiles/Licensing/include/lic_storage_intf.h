/*!
 * \file S:\ISAV\DEVELOPMENT\SOURCE\Kernel\Licensing\licensing\include\lic_storage_intf.h
 * \author Kaspersky Lab
 * \version 
 * \date 2003/06/21
 * \brief 
 * 
 * \purpose:	
*/
#if !defined (__LIC_STORAGE_H__)
#define __LIC_STORAGE_H__

#if defined (_MSC_VER)
  #pragma warning (disable : 4786)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

typedef int (*READ_SECURE_DATA_PROC)    (char **data,  unsigned int *size, void *context);
typedef int (*WRITE_SECURE_DATA_PROC)   (const char *data, unsigned int size, void *context);

typedef struct
{

  READ_SECURE_DATA_PROC    readSecureData;
  WRITE_SECURE_DATA_PROC   writeSecureData;

} ILicenseStorage;

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
