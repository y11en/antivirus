/*
 * File:	imagif.cc
 * Purpose:	Platform Independent GIF Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright: (c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * 6/15/97 Randy Spann: Added GIF87a writing support
 *         R.Spann@ConnRiver.net
 */

// #include "stdafx.h"

#include "imagif.h"
#if CIMAGE_SUPPORT_GIF

#include "imaiter.h"
#include "gifdecod.h"
#include <stdio.h>

// Force byte alignment (Borland)
#if defined  __BORLANDC__
#pragma option -a1
#elif defined _MSV_VER
#pragma option /Zp1
#endif

//#include <fstream.h>
//static ofstream log("x.log");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////// AD - for interlace ///////////////////////////////
int interlaced, iypos, istep, iheight, ipass;
/////////////////////////////////////////////////////////////////////////////

#define GIFBUFTAM 16384

struct rgb { byte r,g,b; };

////////////////////////// AD - for transparency ///////////////////////////////
static struct {
  byte transpcolflag:1;
  byte userinputflag:1;
  byte dispmeth:3;
  byte res:3;
  byte delaytimehi;
  byte delaytimelo;
  byte transpcolindex;
} gifgce;

static struct {                 /* Logic Screen Descriptor  */
  char header[6];        /* Firma and version */
  USHORT scrwidth;
  USHORT scrheight;
  char pflds;
  char bcindx;
  char pxasrat;
} dscgif;

static struct {             /* Image Descriptor */
  USHORT l;
  USHORT t;
  USHORT w;
  USHORT h;
  byte   pf;
} image;

static struct {           /*   Tabla de colores  */
  SHORT colres; /* color resolution */
  SHORT sogct;  /* size of global color table */
  rgb paleta[256];    /*  paleta  */
} TabCol;

static CImageIterator* iter;
static FILE *f;

static  int ibf = GIFBUFTAM+1;
static  byte buf[GIFBUFTAM];

int out_line(unsigned char *pixels, int linelen);

BOOL CImageGIF::ReadFile(const CString& imageFileName)
{
  if (imageFileName != "")
    filename = imageFileName;
//  puts("Empezando... ");

  if ((f = fopen((const char *)filename, "rb"))==NULL) {
 //		printf("no se pudo abrir!");
	return FALSE;
  }

	fread((char*)&dscgif,/*sizeof(dscgif)*/13,1,f);
	 //if (strncmp(dscgif.header,"GIF8",3)!=0) {
	 if (strncmp(dscgif.header,"GIF8",4)!=0) {
//		puts("no es la firma!");
		return FALSE;
	 }

////////////////////////// AD - for interlace ///////////////////////////////

	 //TabCol.sogct = 1<<((dscgif.pflds & 0x0007) + 0x0001);
	 //TabCol.colres = (dscgif.pflds>>6) & 7 + 1;
	 TabCol.sogct = 1 << (dscgif.pflds & 0x07)+1;
	 TabCol.colres = ((int)(dscgif.pflds & 0x70) >> 3) + 1;

	 // Global colour map?

	 if (dscgif.pflds & 0x80)
	 {
		fread((char*)TabCol.paleta,/*sizeof(struct rgb)*/3*TabCol.sogct,1,f);
		//log << "Global colour map" << endl;
	 }

	 char ch;

loop:

	 if (fread(&ch, 1, 1, f) <= 0)
		goto done;

	 if (ch == '!')                     // extension
	 {
		unsigned char count;
		unsigned char fc;

		//log << "EXTENSION: ";

		if (fread(&fc, 1, 1, f) <= 0)   // function-code
			goto done;

		//log << hex << (int)fc << dec << endl;

		//////////////// AD - for transparency //////////////////////////
		if (fc == 0xF9)
		{
			//log << "Transparent" << endl;

			if (fread(&count, 1, 1, f) <= 0)
				goto done;
			if (fread(&gifgce, 1, /*sizeof(gifgce)*/4, f) != count)
				goto done;

			if (gifgce.transpcolflag)
				bgindex = gifgce.transpcolindex;

		///////////////////////////////////////////////////////////////////
		}

		while (fread(&count, 1, 1, f) && count)
		{
			//log << "Skipping " << count << " bytes" << endl;
			fseek(f, count, SEEK_CUR);
		}

		goto loop;
	 }

	 else if (ch == ',')                 // image
	 {
		fread((char*)&image,/*sizeof(image)*/9,1,f);
		//log << "Image header" << endl;

		// Local colour map?

		if (image.pf & 0x80)
		{
			TabCol.sogct = 1 << ((image.pf & 0x07) +1);
			fread((char*)TabCol.paleta,/*sizeof(struct rgb)*/3*TabCol.sogct,1,f);
			//log << "Local colour map" << endl;
		}

		Create(image.w, image.h, 8);

		if ((image.pf & 0x80) || (dscgif.pflds & 0x80))
		{
			unsigned char r[256], g[256], b[256];

			int i, has_white = 0;

			for (i=0; i < TabCol.sogct; i++)
			{
				r[i] = TabCol.paleta[i].r;
				g[i] = TabCol.paleta[i].g;
				b[i] = TabCol.paleta[i].b;

				if (RGB(r[i],g[i],b[i]) == 0xFFFFFF)
					has_white = 1;
			}

			// Make transparency colour black...
			if (bgindex != -1)
				r[bgindex] = g[bgindex] = b[bgindex] = 0;

			// Fill in with white // AD
			if (bgindex != -1)
			{
				while (i < 256)
				{
					has_white = 1;
					r[i] = g[i] = b[i] = 255;
					i++;
				}
			}

			// Force last colour to white...   // AD
			if ((bgindex != -1) && !has_white)
				r[255] = g[255] = b[255] = 255;

			SetPalette((bgindex != -1 ? 256 : TabCol.sogct), r, g, b);
			//log << "Set colour map" << endl;
		}

		iter = new CImageIterator(this);
		iter->Upset();
		int badcode;
		ibf = GIFBUFTAM+1;
		GIFDecoder gifdec;

		interlaced = image.pf & 0x40;
		iheight = image.h;
		istep = 8;
		iypos = 0;
		ipass = 0;

		//if (interlaced) log << "Interlaced" << endl;

		gifdec.decoder(GetWidth(), badcode);
		delete iter;
		goto loop;
	 }

done:

	 fclose(f);

  return TRUE;
}

int get_byte()
{
  if (ibf>=GIFBUFTAM)
 {
// FW 06/02/98 >>>
// fread(buf,GIFBUFTAM,1,f);
  int n = fread( buf , 1 , GIFBUFTAM , f ) ;
  if( n < GIFBUFTAM ) buf[ n ] = -1 ;
// FW 06/02/98 <<<
  ibf = 0;
  }

  return buf[ibf++];
}

int out_line(unsigned char *pixels, int linelen)
{
////////////////////////// AD - for interlace ///////////////////////////////
  if (interlaced)
  {
	 iter->SetY(iheight-iypos-1);
	 iter->SetRow(pixels, linelen);

	 if ((iypos += istep) >= iheight)
	 {
		do
			{
				if (ipass++ > 0)
					istep /= 2;

				iypos = istep / 2;
			}
			  while (iypos > iheight);
	 }

	 return 0;
  }
  else
/////////////////////////////////////////////////////////////////////////////
  if (iter->ItOK()) {
	 iter->SetRow(pixels, linelen);
	 (void)iter->PrevRow();
	 return 0;
  }
  else {
//	 puts("chafeo");
	 return -1;
  }
}

/////////////////////////////////////////////////////////////////////
// SaveFile - writes GIF87a gif file
// Randy Spann 6/15/97
// R.Spann@ConnRiver.net
//
BOOL CImageGIF::SaveFile(const CString& imageFileName)
{
	FILE *fp;
	int B;

	int LeftOfs, TopOfs;
	int Resolution;
	int ColorMapSize;
	int InitCodeSize;	
	int BackGround = 0;

	Width = GetWidth();
	Height = GetHeight();
	BitsPerPixel = 8;
	
	if(GetDepth() > 8)
	{
		AfxMessageBox("GIF Images must be 8 bit or less");
		return FALSE;
	}

	fp=fopen(filename,"wb");
	if (fp==NULL) {
		AfxMessageBox("Can't open GIF for writing");
		return FALSE;
	}
	
	ColorMapSize = 1 << BitsPerPixel;

	LeftOfs = TopOfs = 0;

	cur_accum = 0;
	cur_bits = 0;

	Resolution = BitsPerPixel;

 	CountDown = (long)Width * (long)Height;

	if (BitsPerPixel <=1)
	InitCodeSize=2;
	else
	InitCodeSize = BitsPerPixel;

	curx = 0;
	cury = Height - 1;//because we read the image bottom to top

	fwrite("GIF87a",1,6,fp);	   //GIF Header

	Putword(Width,fp);			   //Logical screen descriptor
	Putword(Height,fp);

	B=0x80;						   
	
	B |=(Resolution -1) << 5;

	B |=(BitsPerPixel - 1);

	fputc(B,fp);				   //GIF "packed fields"

	fputc(BackGround,fp);
	
	fputc(0,fp);				   //GIF "pixel aspect ratio"

	long bufsize = CountDown;
	bufsize *= 3;
/*
	buffer= (BYTE *) new char [bufsize];
		
	if (buffer==NULL) {
		AfxMessageBox("Out of Memory in GIFWrite");
		fclose(fp);
		return FALSE;
	}
*/
	buffer = RawImage;

	CImagePalette* pPal = GetPalette();
	int nNumEntries = pPal->GetEntryCount();
	palPalEntry = new PALETTEENTRY [(256 * 3)];//!
	pPal->GetPaletteEntries(0, nNumEntries, palPalEntry);
	for(int i=0; i<ColorMapSize; ++i) 
	{
		fputc(palPalEntry[i].peRed,fp);
		fputc(palPalEntry[i].peGreen,fp);
		fputc(palPalEntry[i].peBlue,fp);
	}

	fputc(',',fp);

	Putword(LeftOfs,fp);
	Putword(TopOfs,fp);
	Putword(Width,fp);
	Putword(Height,fp);

	fputc(0x00,fp);

	 // Write out the initial code size
	fputc( InitCodeSize, fp );
	 // Go and actually compress the data
	compress(  InitCodeSize+1, fp);

	 // Write out a Zero-length packet (to end the series)
	fputc( 0, fp );

	 // Write the GIF file terminator
	fputc( ';', fp );

	 // And close the file
	fclose( fp );

    // PJO - Free up the memory
    delete[] palPalEntry;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImageGIF methods

//  Bump the 'curx' and 'cury' to point to the next pixel
void CImageGIF::BumpPixel()
{
	// Bump the current X position
	++curx;
	if( curx == Width ) {
		curx = 0;
		cury--;	             //bottom to top
	}
}

// Return the next pixel from the image
int CImageGIF::GIFNextPixel( )
{
	unsigned long index;
	int r;
	
	if( CountDown == 0 )
		return EOF;
	--CountDown;
	index= (unsigned long)curx + (unsigned long)cury * 
		(((unsigned long)Width * BitsPerPixel + 31) / 32) * 4;//effective width
	r = *(buffer+index);
	BumpPixel();
	return r;
}

void CImageGIF::Putword(int w, FILE *fp )
{
	fputc( w & 0xff, fp );
	fputc( (w / 256) & 0xff, fp );
}
/***************************************************************************
 *
 *  GIFCOMPR.C       -     LZW GIF Image compression routines
 *
 ***************************************************************************/
#define BITS    12
#define HSIZE  5003            /* 80% occupancy */
typedef        unsigned char   char_type;

static int n_bits;                        /* number of bits/code */
static int maxbits = BITS;                /* user settable max # bits/code */
static code_int maxcode;                  /* maximum code, given n_bits */
static code_int maxmaxcode = (code_int)1 << BITS; /* should NEVER generate this code */

#define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)

static count_int htab [HSIZE];
static unsigned short codetab [HSIZE];
#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]

static code_int free_ent = 0;                  /* first unused entry */

/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes, start over.
 */
static int clear_flg = 0;

/*
 * compress pixels to GIF packets
 */

static int g_init_bits;
static FILE* g_outfile;

static int ClearCode;
static int EOFCode;

void CImageGIF::compress( int init_bits, FILE* outfile)
{
	register long fcode;
	register code_int i /* = 0 */;
	register int c;
	register code_int ent;
	register code_int disp;
	register int hshift;

	/*
	 * Set up the globals:  g_init_bits - initial number of bits
	 *                      g_outfile   - pointer to output file
	 */
	g_init_bits = init_bits;
	g_outfile = outfile;

	 // Set up the necessary values
	clear_flg = 0;
	maxcode = MAXCODE(n_bits = g_init_bits);

	ClearCode = (1 << (init_bits - 1));
	EOFCode = ClearCode + 1;
	free_ent = ClearCode + 2;

	char_init();

	ent = GIFNextPixel( );

	hshift = 0;
	for ( fcode = (long) HSIZE;  fcode < 65536L; fcode *= 2L )
		++hshift;
	hshift = 8 - hshift;                /* set hash code range bound */

	cl_hash( (count_int) HSIZE);            /* clear hash table */

	output( (code_int)ClearCode );

	while ( (c = GIFNextPixel( )) != EOF ) {    

		fcode = (long) (((long) c << maxbits) + ent);
		i = (((code_int)c << hshift) ^ ent);    /* xor hashing */

		if ( HashTabOf (i) == fcode ) {
			ent = CodeTabOf (i);
			continue;
		} else if ( (long)HashTabOf (i) < 0 )      /* empty slot */
			goto nomatch;
		disp = HSIZE - i;           /* secondary hash (after G. Knott) */
		if ( i == 0 )
			disp = 1;
probe:
		if ( (i -= disp) < 0 )
			i += HSIZE;

		if ( HashTabOf (i) == fcode ) {
			ent = CodeTabOf (i);
			continue;
		}
		if ( (long)HashTabOf (i) > 0 )
			goto probe;
nomatch:
		output ( (code_int) ent );
		ent = c;
		if ( free_ent < maxmaxcode ) {  
			CodeTabOf (i) = free_ent++; /* code -> hashtable */
			HashTabOf (i) = fcode;
		} else
				cl_block();
	}
	 // Put out the final code.
	output( (code_int)ent );
	output( (code_int) EOFCode );
}

static unsigned long masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
								  0x001F, 0x003F, 0x007F, 0x00FF,
								  0x01FF, 0x03FF, 0x07FF, 0x0FFF,
								  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

void CImageGIF::output( code_int  code)
{
	cur_accum &= masks[ cur_bits ];

	if( cur_bits > 0 )
		cur_accum |= ((long)code << cur_bits);
	else
		cur_accum = code;

	cur_bits += n_bits;

	while( cur_bits >= 8 ) {
		char_out( (unsigned int)(cur_accum & 0xff) );
		cur_accum >>= 8;
		cur_bits -= 8;
	}

	/*
	 * If the next entry is going to be too big for the code size,
	 * then increase it, if possible.
	 */
   if ( free_ent > maxcode || clear_flg ) {
		if( clear_flg ) {
			maxcode = MAXCODE (n_bits = g_init_bits);
			clear_flg = 0;
		} else {
			++n_bits;
			if ( n_bits == maxbits )
				maxcode = maxmaxcode;
			else
				maxcode = MAXCODE(n_bits);
		}
	}
	
	if( code == EOFCode ) {
		 // At EOF, write the rest of the buffer.
		while( cur_bits > 0 ) {
			char_out( (unsigned int)(cur_accum & 0xff) );
			cur_accum >>= 8;
			cur_bits -= 8;
		}
	
		flush_char();
	
		fflush( g_outfile );
	
		if( ferror( g_outfile ) ) {
			AfxMessageBox("Write Error in GIF file",MB_OK);
		}
	}
}

void CImageGIF::cl_block()
{
	cl_hash((count_int)HSIZE);
	free_ent=ClearCode+2;
	clear_flg=1;

	output((code_int)ClearCode);
}

void CImageGIF::cl_hash(register count_int hsize)

{
	register count_int *htab_p = htab+hsize;

	register long i;
	register long m1 = -1L;

	i = hsize - 16;

	do {
		*(htab_p-16)=m1;
		*(htab_p-15)=m1;
		*(htab_p-14)=m1;
		*(htab_p-13)=m1;
		*(htab_p-12)=m1;
		*(htab_p-11)=m1;
		*(htab_p-10)=m1;
		*(htab_p-9)=m1;
		*(htab_p-8)=m1;
		*(htab_p-7)=m1;
		*(htab_p-6)=m1;
		*(htab_p-5)=m1;
		*(htab_p-4)=m1;
		*(htab_p-3)=m1;
		*(htab_p-2)=m1;
		*(htab_p-1)=m1;
		
		htab_p-=16;
	} while ((i-=16) >=0);

	for (i+=16;i>0;--i)
		*--htab_p=m1;
}

/*******************************************************************************
*   GIF specific
*******************************************************************************/

static int a_count;

void CImageGIF::char_init()
{
	a_count=0;
}

static char accum[256];

void CImageGIF::char_out(int c)
{
	accum[a_count++]=c;
	if (a_count >=254)
		flush_char();
}

void CImageGIF::flush_char()
{
	if (a_count > 0) {
		fputc(a_count,g_outfile);
		fwrite(accum,1,a_count,g_outfile);
		a_count=0;
	}
}
#endif 	//	 CIMAGE_SUPPORT_GIF
