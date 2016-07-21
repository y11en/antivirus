/* DECODE.C - An LZW decoder for GIF
 * Copyright (C) 1987, by Steven A. Bennett
 * Copyright (C) 1994, C++ version by Alejandro Aguilar Sierra
*
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * In accordance with the above, I want to credit Steve Wilhite who wrote
 * the code which this is heavily inspired by...
 *
 * GIF and 'Graphics Interchange Format' are trademarks (tm) of
 * Compuserve, Incorporated, an H&R Block Company.
 *
 * Release Notes: This file contains a decoder routine for GIF images
 * which is similar, structurally, to the original routine by Steve Wilhite.
 * It is, however, somewhat noticably faster in most cases.
 *
 */

#define LOCAL static
#define FAST register

typedef short SHORT;      				// 16 bits integer
typedef unsigned short USHORT;		// 16 bits unsigned integer
typedef unsigned char byte;             // 8 bits unsigned integer
typedef unsigned long ULONG;            // 32 bits unsigned integer
typedef int INT;                        // 16 bits integer

#ifndef LONG
typedef long LONG;                      // 32 bits integer
#endif

/* Various error codes used by decoder
 */
#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4
//#define NULL   0L
#define MAX_CODES   4095

class GIFDecoder
{
protected:
  SHORT init_exp(SHORT size);
  SHORT get_next_code();
public:
  SHORT decoder(SHORT linewidth, INT&  bad_code_count);
/* bad_code_count is incremented each time an out of range code is read.
 * When this value is non-zero after a decode, your GIF file is probably
 * corrupt in some way...
 */

  friend int get_byte(void);
//   - This external (machine specific) function is expected to return
// either the next byte from the GIF file, or a negative error number.


friend int out_line(unsigned char *pixels, int linelen);
/*   - This function takes a full line of pixels (one byte per pixel) and
 * displays them (or does whatever your program wants with them...).  It
 * should return zero, or negative if an error or some other event occurs
 * which would require aborting the decode process...  Note that the length
 * passed will almost always be equal to the line length passed to the
 * decoder function, with the sole exception occurring when an ending code
 * occurs in an odd place in the GIF file...  In any case, linelen will be
 * equal to the number of pixels passed...
*/

protected:
/* Static variables */
SHORT curr_size;                     /* The current code size */
SHORT clear;                         /* Value for a clear code */
SHORT ending;                        /* Value for a ending code */
SHORT newcodes;                      /* First available code */
SHORT top_slot;                      /* Highest code for current size */
SHORT slot;                          /* Last read code */

/* The following static variables are used
 * for seperating out codes
 */
SHORT navail_bytes;              /* # bytes left in block */
SHORT nbits_left;                /* # bits left in current byte */
byte b1;                           /* Current byte */
byte byte_buff[257];               /* Current block */
byte *pbytes;                      /* Pointer to next byte in block */
};


