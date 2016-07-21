#ifndef _DMADORE_MD5_H
#define _DMADORE_MD5_H

struct md5_ctx {
  /* The four chaining variables */
  unsigned long buf[4];
  /* Count number of message bits */
  unsigned long bits[2];
  /* Data being fed in */
  unsigned long in[16];
  /* Our position within the 512 bits (always between 0 and 63) */
  int b;
};

void  __fastcall MD5_transform (unsigned long buf[4], const unsigned long in[16]);
void  __fastcall MD5_start (struct md5_ctx *context);
void  __fastcall MD5_feed (struct md5_ctx *context, unsigned char inb);
void  __fastcall MD5_stop (struct md5_ctx *context, unsigned char digest[16]);

#endif /* not defined _DMADORE_MD5_H */
