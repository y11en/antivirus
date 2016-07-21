// ---
unsigned long iCountCRC32str( const char* ptr ) {
  unsigned long c;
  unsigned long d = ~0;
  while( 0 != (c=*ptr++) ) {
    unsigned long i;
    for( i=0; i<8; i++ ) {
      if ( ((d^c)&1) != 0 )
        d = (d>>1) ^ 0xEDB88320;
      else
        d = (d>>1);
      c >>= 1;
    }
  }
  return (~d);
}
