/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: base64.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/


#include <ctype.h>			//for isspace()
#include <stdlib.h>			//for calloc(), free()
#include <string.h>			//for strlen()


/* NOTE **
3/14/03 Paul Ryan
This code acquired from libs2\shared\pgpLicenseNumber.c which itself seems to 
have been acquired from libs2\pgpsdk\priv\external\common\tis\cms\src\base64.c. 
Modifications went to remove all the hooks into PGP SDK so it will run 
outside of PGP.
*/


typedef enum	{
	FALSE, 
	TRUE
}  BOOL;

#define SKIPWS(c) while (isspace ((int) (*(c)))) c++
static const unsigned char BaseChars[] = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define BaseChar(c) BaseChars[c]
static const unsigned char BaseVals[] = {
  0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,62, 0xff,0xff,0xff,63,
  52,53,54,55, 56,57,58,59, 60,61,0xff,0xff, 0xff,0xff,0xff,0xff,
  0xff, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
  15,16,17,18, 19,20,21,22, 23,24,25,0xff, 0xff,0xff,0xff,0xff,
  0xff,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
  41,42,43,44, 45,46,47,48, 49,50,51,0xff, 0xff,0xff,0xff,0xff
};
#define BaseVal(c) BaseVals[c]


/** f_base64encode( ***
Base-64 encode the content buffer provided.

--- parameters & return ----
out: Output. Address of the pointer to receive the address of the memory buffer 
	allocated to accommodate the encoded output. Caller is responsible for 
	freeing this buffer. In case of an error, the pointer is guaranteed to be 
	null.
outlen: Output. Address of the variable to receive the length of the encoded 
	output content.
in: address of the input content to be encoded
inlen: length of the input content to be encoded
RETURN: TRUE if successful; FALSE if any input is invalid or if memory for the 
	output buffer could not be allocated

--- revision history -------
3/14/03 PR: copied & adapted	*/
BOOL f_base64encode( unsigned char * *const  out, 
						size_t *const  outlen,
						const unsigned char * in, 
						size_t  inlen)	{
  unsigned char * curpos;
  unsigned char  a, b, c;

  if (!( out && outlen && in && inlen))
	  return FALSE;

  *out = NULL;

  /* determine how long the output will be.  if it is not a multple of 3
   * bytes, increase length so that it is so that the pad characters are
   * accounted for.
   */
  if (inlen % 3 != 0)
    *outlen = inlen + 3 - inlen % 3;
  else
    *outlen = inlen;

  /* base64 encoding creates 4 output chars for every 3 input chars */
  *outlen = 4 * (*outlen) / 3;

  if ((*out = calloc( *outlen + 1, 1)) == NULL)
	  return FALSE;

  curpos = *out;

  while (inlen)
  {
    a = *in++;
    inlen--;

    *curpos++ = BaseChar (a >> 2);

    if (inlen)
      b = *in++;
    else
      b = 0;

    *curpos++ = BaseChar (((a & 0x03) << 4) | (b >> 4));

    if (!inlen)
    {
      *curpos++ = '=';
      *curpos++ = '=';
      break;
    }
    inlen--;

    if (inlen)
      c = *in++;
    else
      c = 0;

    *curpos++ = BaseChar (((b & 0x0f) << 2) | (c >> 6));

    if (!inlen)
    {
      *curpos++ = '=';
      break;
    }
    inlen--;

    *curpos++ = BaseChar (c & 0x3f);
  }

  *curpos = 0;

  return TRUE;
} //f_base64encode(


/** f_base64decode( ***
Base-64 decode the content buffer provided.

--- parameters & return ----
out: Output. Address of the pointer to receive the address of the memory buffer 
	allocated to accommodate the decoded output. Caller is responsible for 
	freeing this buffer.
outlen: Output. Address of the variable to receive the length of the encoded 
	output content.
in: address of the ASCII null-terminated input content to be decoded
RETURN: TRUE if successful; FALSE if any input is invalid, or memory for the 
	output buffer could not be allocated, or a parsing error occurred

--- revision history -------
3/14/03 PR: copied & adapted	*/
BOOL f_base64decode( unsigned char * *const  out, 
						size_t *const  outlen, 
						const unsigned char * in)	{
  unsigned char * curpos;
  unsigned char a, b;
  BOOL  f_fail;

  if (!( out && outlen && in))
	  return FALSE;

  *out = NULL;

  *outlen = (3 * strlen( in)) / 4; /* maximum length */
  if ((*out = calloc( *outlen, 1)) == NULL)
	  return FALSE;

  curpos = *out;

  SKIPWS (in);
  while (*in)
  {
    a = *in++;
    SKIPWS (in);
    if (f_fail = !*in)
	  break;

    b = *in++;
    SKIPWS (in);
    if (f_fail = !*in)
	  break;

    *curpos++ = (BaseVal (a) << 2) | (BaseVal (b) >> 4);

    a = b;
    b = *in++;
    SKIPWS (in);
    if (f_fail = !*in)
	  break;

    if (b == '=')
      break;

    *curpos++ = (BaseVal (a) << 4) | (BaseVal (b) >> 2);

    a = b;
    b = *in++;
    SKIPWS (in);

    if (b == '=')
      break;

    *curpos++ = (BaseVal (a) << 6) | BaseVal (b);
  }

  if (!f_fail)
	*outlen = curpos - *out; /* real length */
  else  {
	free( *out );
	*out = NULL;
	return FALSE;
  }

  return TRUE;
} //f_base64decode(



