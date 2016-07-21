#ifndef _BINMACRO_H_
#define _BINMACRO_H_

#define BIN8(x) BIN___(0##x)
#define BIN___(x)                                    \
	(                                                \
	((x / 01ul) % 010)*(2>>1) +                      \
	((x / 010ul) % 010)*(2<<0) +                     \
	((x / 0100ul) % 010)*(2<<1) +                    \
	((x / 01000ul) % 010)*(2<<2) +                   \
	((x / 010000ul) % 010)*(2<<3) +                  \
	((x / 0100000ul) % 010)*(2<<4) +                 \
	((x / 01000000ul) % 010)*(2<<5) +                \
	((x / 010000000ul) % 010)*(2<<6)                 \
	)

#define BIN16(x1,x2) \
    ((BIN8(x1)<<8)+BIN8(x2))

#define BIN24(x1,x2,x3) \
    ((BIN8(x1)<<16)+(BIN8(x2)<<8)+BIN8(x3))

#define BIN32(x1,x2,x3,x4) \
    ((BIN8(x1)<<24)+(BIN8(x2)<<16)+(BIN8(x3)<<8)+BIN8(x4))

#endif//_BINMACRO_H_