#ifndef __hashutil_h
#define __hashutil_h

#ifdef __cplusplus
extern "C" {
#endif

  // crc functions
  unsigned long  iCountCRC32( int size, const unsigned char* ptr, unsigned long dwOldCRC32 );
  unsigned long  iCountCRC32str( const char* str );

  unsigned short iCountCRC16( int size, const unsigned char* ptr, unsigned short wOldCRC16 );
  unsigned short iCountCRC16str( const char* ptr );
    
#ifdef __cplusplus
}
#endif


#endif //__hashutil_h
