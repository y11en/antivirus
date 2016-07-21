unsigned int crc16_table[0x100];
#define updcrc( cp, crc ) (crc16_table[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)

// ---
unsigned short iCountCRC16str( const char* ptr ) {
  unsigned int   c;
  unsigned short val16 = ~0;
  unsigned int   crc   = val16;
  while( 0 != (c=*ptr++) )
    crc = crc16_table[((crc >> 8) & 0xff)] ^ (crc << 8) ^ c;
  val16 = crc;
  return ~val16;
}
