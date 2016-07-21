#define H0 0x67452301UL
#define H1 0xEFCDAB89UL
#define H2 0x98BADCFEUL
#define H3 0x10325476UL
#define H4 0xC3D2E1F0UL

#define Kt0 0x5A827999UL
#define Kt1 0x6ED9EBA1UL
#define Kt2 0x8F1BBCDCUL
#define Kt3 0xCA62C1D6UL

#define ft0 (unsigned long)((B & C) | ((~B) & D))
#define ft1 (unsigned long)(B ^ C ^ D)
#define ft2 (unsigned long)((B & C) | (B & D) | (C & D))
#define ft3 ft1

#define Sn(x, n) ( ((x) << (n) ) | ((x) >> (32-n)) )

