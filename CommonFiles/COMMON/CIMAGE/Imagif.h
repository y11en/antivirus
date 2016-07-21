/*
 * File:	imagif.h
 * Purpose:	Declaration of the Platform Independent GIF Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#if !defined(__ImaGIF_h)
#define __ImaGIF_h

#include "imafile.h"

typedef short int       code_int;   
typedef long int        count_int;
typedef unsigned char   pixval;

class CImageGIF: public CFileImageImpl
{
public:
  CImageGIF(const CImageGIF * ima): CFileImageImpl(ima)
  {
  }
  CImageGIF(const CString& imageFileName): CFileImageImpl(imageFileName)
  {
  }
  ~CImageGIF() {};

  virtual BOOL ReadFile(const CString& imageFileName="");
  virtual BOOL SaveFile(const CString& imageFileName="");

private:
	int          Width, Height;
	int             curx, cury;
	int           BitsPerPixel;
	long             CountDown;
	unsigned long    cur_accum;
	int              cur_bits;
	unsigned char    *buffer;
	PALETTEENTRY *palPalEntry;

// Implementation
protected:
	void BumpPixel (  );
	int GIFNextPixel ( );
	void Putword ( int w, FILE* fp );
	void compress ( int init_bits, FILE* outfile);
	void output ( code_int code );
	void cl_block (  );
	void cl_hash ( count_int hsize );
	void writeerr (  );
	void char_init (  );
	void char_out ( int c );
	void flush_char (  );
};

#endif

