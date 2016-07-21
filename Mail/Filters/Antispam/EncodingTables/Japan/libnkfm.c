/*
 *  libnkfm- Kanji conversion library using nkf
 *
 *  Copyright (C), KUBO Takehiro <kubo-t@cx.airnet.ne.jp>
 *
 *  include している nkf.c は、nkf1.9 の nkf.c そのもので、まったく手
 *  を加えていません。
 *  このコードは nkf1.9 のソースに梱包されている NKF.xs を参考に作成しました。
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include "libnkfm.h"

static const unsigned char *input;
static unsigned char *output;
static size_t input_ctr, i_len;
static size_t output_ctr, o_len;
static size_t incsize;
static jmp_buf jbuf;
static int expand;

#undef getc
#undef ungetc
#define getc(f)   	((input_ctr>i_len) ? -1 : input[input_ctr++])
#define ungetc(c,f)	input_ctr--

#define INCSIZE		32
#undef putchar
#undef TRUE
#undef FALSE
#define putchar(c) ((o_len != -1 && output_ctr >= o_len) ? libnkf_overrun(c) : 0, (output[output_ctr++] = c))

static void libnkf_overrun(int c)
{
  unsigned char *newbuf;

  
  if (expand == 1) { /* nkf_convert_new */
    incsize += INCSIZE;
    newbuf = (unsigned char *)realloc(output, incsize);
    if (newbuf == NULL) {
      free(output);
      output = NULL;
      longjmp(jbuf, 1);
      }
    output = newbuf;
  } else { /* nkf_n_convert */
    longjmp(jbuf, o_len);
    }
}


#define PERL_XS 1
#include "nkf.c"

void nkf_init(const char *option)
{
	const char *opt_end;
	
	reinit();
	if (option == NULL) {
		return;
	}
	opt_end = option + strlen(option);
	for (; option < opt_end; option++) {
		if (*option == '-') {
			options((char *)option);
		}
	}
	
    if(iso8859_f && (oconv != j_oconv || !x0201_f )) {
		iso8859_f = FALSE;
    } 

	return;
}

char *nkf_convert(char *outbuf, const char *inbuf)
{
	input = inbuf;
	input_ctr = 0;
	i_len = strlen(inbuf);
	
	output = outbuf;
	output_ctr = 0;
	o_len = -1;
	
	kanji_convert(NULL);
	output[output_ctr] = '\0';
	
	return output;
}

char *nkf_n_convert(char *outbuf, size_t outbuflen, const char *inbuf)
{
	int rval;
	
	input = inbuf;
	input_ctr = 0;
	i_len = strlen(inbuf);
	
	output = outbuf;
	output_ctr = 0;
	o_len = outbuflen - 1;
	expand = 0;	
	
	if ((rval = setjmp(jbuf)) == 0) {
		kanji_convert(NULL);
		output[output_ctr] = '\0';
	} else {
		output[rval] = '\0';
	}
	
	return (rval == 0) ? output : NULL;
}

char *nkf_convert_new(const char *inbuf)
{	
	input = inbuf;
	input_ctr = 0;
	i_len = strlen(inbuf);
	
	incsize = INCSIZE;
	output = (char *)malloc(incsize);
	output_ctr = 0;
	o_len = incsize;
	expand = 1;
	
	if (output == NULL) {
		return NULL;
	}
	
	if (setjmp(jbuf) == 0) {
		kanji_convert(NULL);
		output[output_ctr] = '\0';
	}	
	
	return output;
}
