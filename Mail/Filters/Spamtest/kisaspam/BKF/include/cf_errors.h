#ifndef __cf_errors_h__
#define __cf_errors_h__
/*------------------------------------------------------------------------------*/
/* General Error Codes                                                          */
/*------------------------------------------------------------------------------*/
enum cf_errors
{
  CFLT_OK                    = 0,     /* no error */
  CFLT_ERR_NOTIMPL           = 1,     /* not implemented */
  CFLT_ERR_ARGINVAL          = 2,     /* invalid argument */

  CFLT_ERR_FCREATE           = 3,     /* couldn't create file */
  CFLT_ERR_FOPEN             = 4,     /* couldn't open file */
  CFLT_ERR_FREAD             = 5,     /* couldn't read file */
  CFLT_ERR_FSAVE             = 6,     /* couldn't save file */
  CFLT_ERR_FDELETE            = 7,    /* couldn't delete file */

  CFLT_ERR_NOMEMORY          = 8,     /* no memory */
  CFLT_ERR_FAIL              = 9,     /* data processing failed */
  CFLT_ERR_DATA              = 10,     /* invalid data */
  
  CFLT_ERR_EXPORT            = 11,    /* export forbidden */

  CFLT_ERR_INTERNAL          = 100    /* internal error */
};
/*------------------------------------------------------------------------------*/
#endif
