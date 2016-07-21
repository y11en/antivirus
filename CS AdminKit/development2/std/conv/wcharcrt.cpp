#ifdef __cplusplus
extern "C" {
#endif

#include <std/conv/wcharcrt.h>

#ifndef __unix

#include <time.h>
#include <ctype.h>

//#include <powertv.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "_print.h"



using namespace std;

void xtoa (
        unsigned long val,
        char *buf,
        unsigned radix,
        int is_neg
        );


#ifndef UNIX_PLATFORM

int iswspace( wchar_t c )
{
	if (c==L' ' || c==0x09 || c==0x0A || c==0x0B || c==0x0C || c==0x0D ) return 1;
	else return 0;
}

int iswdigit( wchar_t c )
{
	if (c>=L'0' && c<=L'9') return 1;
	else return 0;
}

int iswalpha( wchar_t c )
{
	if ((c>=L'A' && c<=L'Z') ||
		(c>=L'a' && c<=L'z')) return 1;
	else return 0;
}

int iswalnum( wchar_t c )
{ return iswalpha( c ) | iswdigit( c ); }


/***
*wcslen - return the length of a null-terminated wide-character string
*
*Purpose:
*       Finds the length in wchar_t's of the given string, not including
*       the final null wchar_t (wide-characters).
*
*Entry:
*       const wchar_t * wcs - string whose length is to be computed
*
*Exit:
*       length of the string "wcs", exclusive of the final null wchar_t
*
*Exceptions:
*
*******************************************************************************/

#if 0
size_t wcslen (
        const wchar_t * wcs
        )
{
        const wchar_t *eos = wcs;

        while( *eos ) eos++;

        return( (size_t)(eos - wcs) );
}
#endif

/***
*wchar_t *wcscpy(dst, src) - copy one wchar_t string over another
*
*Purpose:
*       Copies the wchar_t string src into the spot specified by
*       dest; assumes enough room.
*
*Entry:
*       wchar_t * dst - wchar_t string over which "src" is to be copied
*       const wchar_t * src - wchar_t string to be copied over "dst"
*
*Exit:
*       The address of "dst"
*
*Exceptions:
*******************************************************************************/

#if 0
wchar_t * wcscpy(wchar_t * dst, const wchar_t * src)
{
        wchar_t * cp = dst;

        while( (*cp++ = *src++)!=0 )
                ;               /* Copy src over dst */

        return( dst );
}
#endif
/***
*wchar_t *wcsncpy(dest, source, count) - copy at most n wide characters
*
*Purpose:
*       Copies count characters from the source string to the
*       destination.  If count is less than the length of source,
*       NO NULL CHARACTER is put onto the end of the copied string.
*       If count is greater than the length of sources, dest is padded
*       with null characters to length count (wide-characters).
*
*
*Entry:
*       wchar_t *dest - pointer to destination
*       wchar_t *source - source string for copy
*       size_t count - max number of characters to copy
*
*Exit:
*       returns dest
*
*Exceptions:
*
*******************************************************************************/
//#if 0
wchar_t * wcsncpy (
        wchar_t * dest,
        const wchar_t * source,
        size_t count
        )
{
		wchar_t *start = dest;

       while (count && (*dest++ = *source++))    /* copy string */
                count--;

        if (count)                              /* pad out with zeroes */
                while (--count)
                        *dest++ = L'\0';

        return(start);
}
//#endif

/***
*wcscmp - compare two wchar_t strings,
*        returning less than, equal to, or greater than
*
*Purpose:
*       wcscmp compares two wide-character strings and returns an integer
*       to indicate whether the first is less than the second, the two are
*       equal, or whether the first is greater than the second.
*
*       Comparison is done wchar_t by wchar_t on an UNSIGNED basis, which is to
*       say that Null wchar_t(0) is less than any other character.
*
*Entry:
*       const wchar_t * src - string for left-hand side of comparison
*       const wchar_t * dst - string for right-hand side of comparison
*
*Exit:
*       returns -1 if src <  dst
*       returns  0 if src == dst
*       returns +1 if src >  dst
*
*Exceptions:
*
*******************************************************************************/
#if 0
int wcscmp (
        const wchar_t * src,
        const wchar_t * dst
        )
{
        int ret = 0 ;

        while( ! (ret = (int)(*src - *dst)) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}
#endif
/***
*wchar_t *wcscat(dst, src) - concatenate (append) one wchar_t string to another
*
*Purpose:
*       Concatenates src onto the end of dest.  Assumes enough
*       space in dest.
*
*Entry:
*       wchar_t *dst - wchar_t string to which "src" is to be appended
*       const wchar_t *src - wchar_t string to be appended to the end of "dst"
*
*Exit:
*       The address of "dst"
*
*Exceptions:
*
*******************************************************************************/
 //#if 0
wchar_t * wcscat(
        wchar_t * dst,
        const wchar_t * src
        )
{
        wchar_t * cp = dst;

        while( *cp )
                cp++;                   /* find end of dst */

        while( (*cp++ = *src++)!=0 ) ;       /* Copy src to end of dst */

        return( dst );                  /* return dst */

}
//#endif
/***
*wchar_t *wcschr(string, c) - search a string for a wchar_t character
*
*Purpose:
*       Searches a wchar_t string for a given wchar_t character,
*       which may be the null character L'\0'.
*
*Entry:
*       wchar_t *string - wchar_t string to search in
*       wchar_t c - wchar_t character to search for
*
*Exit:
*       returns pointer to the first occurence of c in string
*       returns NULL if c does not occur in string
*
*Exceptions:
*
*******************************************************************************/
//#if 0
wchar_t * wcschr (
        const wchar_t * string,
        wchar_t ch
        )
{
        while (*string && *string != (wchar_t)ch)
                string++;

        if (*string == (wchar_t)ch)
                return((wchar_t *)string);
        return(NULL);
}
//#endif
/***
*int wcsncmp(first, last, count) - compare first count chars of wchar_t strings
*
*Purpose:
*       Compares two strings for lexical order.  The comparison stops
*       after: (1) a difference between the strings is found, (2) the end
*       of the strings is reached, or (3) count characters have been
*       compared (wide-character strings).
*
*Entry:
*       wchar_t *first, *last - strings to compare
*       size_t count - maximum number of characters to compare
*
*Exit:
*       returns <0 if first < last
*       returns  0 if first == last
*       returns >0 if first > last
*
*Exceptions:
*
*******************************************************************************/
#if 0
int wcsncmp (
        const wchar_t * first,
        const wchar_t * last,
        size_t count
        )
{
        if (!count)
                return(0);

        while (--count && *first && *first == *last)
        {
                first++;
                last++;
        }

        return((int)(*first - *last));
}
#endif

enum AK_LANGS {
	AK_RUS
};

template<AK_LANGS lang>
wchar_t toUnicode(unsigned char c)
{
	return c;
}

template<>
wchar_t toUnicode<AK_RUS>(unsigned char c)
{
	if(c < 128)
		return c;

	if(c >= 0xC0 && c <= 0xFF)
		return static_cast<int>(c) + 0x350;


	switch(c) {
		case 0xA8: return 0x0401;
		case 0xB8: return 0x0451;
	}

	return c;
}

/***
*size_t mbstowcs() - Convert multibyte char string to wide char string.
*
*Purpose:
*       Convert a multi-byte char string into the equivalent wide char string,
*       according to the LC_CTYPE category of the current locale.
*       [ANSI].
*
*Entry:
*       wchar_t *pwcs = pointer to destination wide character string buffer
*       const char *s = pointer to source multibyte character string
*       size_t      n = maximum number of wide characters to store
*
*Exit:
*       If s != NULL, returns:  number of words modified (<=n)
*               (size_t)-1 (if invalid mbcs)
*
*Exceptions:
*       Returns (size_t)-1 if s is NULL or invalid mbcs character encountered
*
*******************************************************************************/


size_t  mbstowcs
        (
        wchar_t  *pwcs,
        const char *s,
        size_t n
        )
{
	if(n == 0 &&  pwcs == NULL) {
		if (s==NULL)
			return 0;
		else
			return strlen(s);
	}
	if (s==NULL) return 0;

	size_t count = 0;
	while (count < n)
	{
		*pwcs = toUnicode<AK_RUS>(s[count++]);
		if(*pwcs == L'\0')
			return count;
		pwcs++;
	}
	return count;
}

inline unsigned char fromUnicode(wchar_t c)
{
	if(c >= 0 && c < 128) // No conversion required
		return c;

	// 1) Try find russian symbols
	if (c >= 0x0410 && c <= 0x044F)
		return c - 0x350;

	switch(c) { // Handle YO/yo russian symbols
		case 0x0401: return 0xA8;
		case 0x0451: return 0xB8;
	}

	// 2) Try find deutch symbols or hohland
	//...

	if(c > 255)
		return EOF;

	return c;
}

/***
*size_t wcstombs() - Convert wide char string to multibyte char string.
*
*Purpose:
*       Convert a wide char string into the equivalent multibyte char string,
*       according to the LC_CTYPE category of the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       char *s            = pointer to destination multibyte char string
*       const wchar_t *pwc = pointer to source wide character string
*       size_t           n = maximum number of bytes to store in s
*
*Exit:
*       If s != NULL, returns    (size_t)-1 (if a wchar cannot be converted)
*       Otherwise:       Number of bytes modified (<=n), not including
*                    the terminating NUL, if any.
*
*Exceptions:
*       Returns (size_t)-1 if s is NULL or invalid mb character encountered.
*
*******************************************************************************/


size_t wcstombs
        (
        char * s,
        const wchar_t * pwcs,
        size_t n
        )
{
 
	size_t count = 0;

        if (s && n == 0)
            /* dest string exists, but 0 bytes converted */
            return (size_t) 0;

        /* if destination string exists, fill it in */
        if (s)
        {
            /* C locale: easy and fast */
            while(count < n)
            {
				char c = fromUnicode(*pwcs);
				if(c < 0)
					return -1;

                s[count++] = c;
                if (c == '\0')
                    return count;
				pwcs++;
            }
            return count;

        } else { /* s == NULL, get size only, pwcs must be NUL-terminated */
            const wchar_t *eos = pwcs;

            while( *eos++ ) ;

            return( (size_t)(eos - pwcs - 1) );
        }
} 


/***
*int mbtowc() - Convert multibyte char to wide character.
*
*Purpose:
*       Convert a multi-byte character into the equivalent wide character,
*       according to the LC_CTYPE category of the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       wchar_t  *pwc = pointer to destination wide character
*       const char *s = pointer to multibyte character
*       size_t      n = maximum length of multibyte character to consider
*
*Exit:
*       If s = NULL, returns 0, indicating we only use state-independent
*       character encodings.
*       If s != NULL, returns:  0 (if *s = null char)
*                               -1 (if the next n or fewer bytes not valid mbc)
*                               number of bytes comprising converted mbc
*
*Exceptions:
*
*******************************************************************************/


int mbtowc
        (
        wchar_t  *pwc,
        const char *s,
        size_t n
        )
{        

	if ( !s || n == 0 )
            /* indicate do not have state-dependent encodings,
               handle zero length string */
            return 0;

        if ( !*s )
        {
            /* handle NULL char */
            if (pwc)
                *pwc = 0;
            return 0;
        }


        /* stuck the "C" locale again */
        if (pwc)
            *pwc = (wchar_t)(unsigned char)*s;
        return sizeof(char);
}

#endif // if not def UNIX_PLATFORM

/***
*int __cdecl wcsncnt - count wide characters in a string, up to n.
*
*Purpose:
*       Internal local support function. Counts characters in string including NULL.
*       If NULL not found in n chars, then return n.
*
*Entry:
*       const wchar_t *string   - start of string
*       int n                   - character count
*
*Exit:
*       returns number of wide characters from start of string to
*       NULL (inclusive), up to n.
*
*Exceptions:
*
*******************************************************************************/
#if 0
int wcsncnt (
        const wchar_t *string,
        int cnt
        )
{
        int n = cnt+1;
        wchar_t *cp = (wchar_t *)string;

        while (--n && *cp)
            cp++;

        if (n && !*cp)
            return cp - string + 1;
        return cnt;
}
#endif

/***
*size_t wcsftime(wstring, maxsize, format, timeptr) - Format a time string
*
*Purpose:
*       The wcsftime functions is equivalent to to the strftime function, except
*       that the argument 'wstring' specifies an array of a wide string into
*       which the generated output is to be placed. The wcsftime acts as if
*       strftime were called and the result string converted by mbstowcs().
*       [ISO]
*
*Entry:
*       wchar_t *wstring = pointer to output string
*       size_t maxsize = max length of string
*       const wchar_t *format = format control string
*       const struct tm *timeptr = pointer to tb data structure
*
*Exit:
*       !0 = If the total number of resulting characters including the
*       terminating null is not more than 'maxsize', then return the
*       number of wide chars placed in the 'wstring' array (not including the
*       null terminator).
*
*       0 = Otherwise, return 0 and the contents of the string are
*       indeterminate.
*
*Exceptions:
*
*******************************************************************************/

size_t wcsftime (
        wchar_t *wstring,
        size_t maxsize,
        const wchar_t *wformat,
        const struct tm *timeptr
        )
{
        size_t retval = 0;
        char *format = NULL, *string = NULL;
        int flen = wcslen(wformat) + 1;

        if ((string = (char *)malloc(sizeof(char) * maxsize * 2)) == NULL)
            return 0;

        if ((format = (char *)malloc(sizeof(char) * flen * 2)) == NULL)
            goto done;

        if (wcstombs(format, wformat, flen * 2) == -1)
            goto done;

        if (strftime(string, maxsize * 2, format, timeptr))
        {
            if ((retval = mbstowcs(wstring, string, maxsize)) == -1)
                retval = 0;
        }

done:
        free(string);
        free(format);
        return retval;
}



/***
*wcstol, wcstoul(nptr,endptr,ibase) - Convert ascii string to long un/signed
*       int.
*
*Purpose:
*       Convert an ascii string to a long 32-bit value.  The base
*       used for the caculations is supplied by the caller.  The base
*       must be in the range 0, 2-36.  If a base of 0 is supplied, the
*       ascii string must be examined to determine the base of the
*       number:
*           (a) First char = '0', second char = 'x' or 'X',
*               use base 16.
*           (b) First char = '0', use base 8
*           (c) First char in range '1' - '9', use base 10.
*
*       If the 'endptr' value is non-NULL, then wcstol/wcstoul places
*       a pointer to the terminating character in this value.
*       See ANSI standard for details
*
*Entry:
*       nptr == NEAR/FAR pointer to the start of string.
*       endptr == NEAR/FAR pointer to the end of the string.
*       ibase == integer base to use for the calculations.
*
*       string format: [whitespace] [sign] [0] [x] [digits/letters]
*
*Exit:
*       Good return:
*           result
*
*       Overflow return:
*           wcstol -- LONG_MAX or LONG_MIN
*           wcstoul -- ULONG_MAX
*
*       No digits or bad base return:
*           0
*           endptr = nptr*
*
*Exceptions:
*       None.
*******************************************************************************/

/* flag values */
#define FL_UNSIGNED   1       /* wcstoul called */
#define FL_NEG        2       /* negative sign found */
#define FL_OVERFLOW   4       /* overflow occured */
#define FL_READDIGIT  8       /* we've read at least one correct digit */


static unsigned long wcstoxl (
        const wchar_t *nptr,
        const wchar_t **endptr,
        int ibase,
        int flags
        )
{
        const wchar_t *p;
        wchar_t c;
        unsigned long number;
        unsigned digval;
        unsigned long maxval;

        p = nptr;           /* p is our scanning pointer */
        number = 0;         /* start with zero */

        c = *p++;           /* read char */
        while (iswspace(c))
            c = *p++;       /* skip whitespace */

        if (c == '-') {
            flags |= FL_NEG;    /* remember minus sign */
            c = *p++;
        }
        else if (c == '+')
            c = *p++;       /* skip sign */

        if (ibase < 0 || ibase == 1 || ibase > 36) {
            /* bad base! */
            if (endptr)
                /* store beginning of string in endptr */
                *endptr = nptr;
            return 0L;      /* return 0 */
        }
        else if (ibase == 0) {
            /* determine base free-lance, based on first two chars of
               string */
            if (c != L'0')
                ibase = 10;
            else if (*p == L'x' || *p == L'X')
                ibase = 16;
            else
                ibase = 8;
        }

        if (ibase == 16) {
            /* we might have 0x in front of number; remove if there */
            if (c == L'0' && (*p == L'x' || *p == L'X')) {
                ++p;
                c = *p++;   /* advance past prefix */
            }
        }

        /* if our number exceeds this, we will overflow on multiply */
        maxval = ULONG_MAX / ibase;


        for (;;) {  /* exit in middle of loop */
            /* convert c to value */
            if (iswdigit(c))
                digval = c - L'0';
            else if (iswalpha(c))
                digval = towupper(c) - L'A' + 10;
            else
                break;
            if (digval >= (unsigned)ibase)
                break;      /* exit loop if bad digit found */

            /* record the fact we have read one digit */
            flags |= FL_READDIGIT;

            /* we now need to compute number = number * base + digval,
               but we need to know if overflow occured.  This requires
               a tricky pre-check. */

            if (number < maxval || (number == maxval &&
            (unsigned long)digval <= ULONG_MAX % ibase)) {
                /* we won't overflow, go ahead and multiply */
                number = number * ibase + digval;
            }
            else {
                /* we would have overflowed -- set the overflow flag */
                flags |= FL_OVERFLOW;
            }

            c = *p++;       /* read next digit */
        }

        --p;                /* point to place that stopped scan */

        if (!(flags & FL_READDIGIT)) {
            /* no number there; return 0 and point to beginning of
               string */
            if (endptr)
                /* store beginning of string in endptr later on */
                p = nptr;
            number = 0L;        /* return 0 */
        }
        else if ( (flags & FL_OVERFLOW) ||
              ( !(flags & FL_UNSIGNED)
              	&&( 	( 	(flags & FL_NEG) && (number > (LONG_MAX+1L)	 )
                		)
                	|| (  !(flags & FL_NEG) && (number > LONG_MAX)  )
                    )
              )
            )
        {
            /* overflow or signed overflow occurred */
            if ( flags & FL_UNSIGNED )
                number = ULONG_MAX;
            else if ( flags & FL_NEG )
                number = (unsigned long)(0);
            else
                number = LONG_MAX;
        }

        if (endptr != NULL)
            /* store pointer to char that stopped the scan */
            *endptr = p;

        if (flags & FL_NEG)
            /* negate result if there was a neg sign */
            number = (unsigned long)(-(long)number);

        return number;          /* done. */
}

/***
*wchar_t *wcsstr(string1, string2) - search for string2 in string1
*       (wide strings)
*
*Purpose:
*       finds the first occurrence of string2 in string1 (wide strings)
*
*Entry:
*       wchar_t *string1 - string to search in
*       wchar_t *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/
#if 0
wchar_t * __cdecl wcsstr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
        wchar_t *cp = (wchar_t *) wcs1;
        wchar_t *s1, *s2;

        while (*cp)
        {
                s1 = cp;
                s2 = (wchar_t *) wcs2;

                while ( *s1 && *s2 && !(*s1-*s2) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);
}

/***
*wchar_t *wcsrchr(string, ch) - find last occurrence of ch in wide string
*
*Purpose:
*       Finds the last occurrence of ch in string.  The terminating
*       null character is used as part of the search (wide-characters).
*
*Entry:
*       wchar_t *string - string to search in
*       wchar_t ch - character to search for
*
*Exit:
*       returns a pointer to the last occurrence of ch in the given
*       string
*       returns NULL if ch does not occurr in the string
*
*Exceptions:
*
*******************************************************************************/

wchar_t * __cdecl wcsrchr (
        const wchar_t * string,
        wchar_t ch
        )
{
        wchar_t *start = (wchar_t *)string;

        while (*string++)                       /* find end of string */
                ;
                                                /* search towards front */
        while (--string != start && *string != (wchar_t)ch)
                ;

        if (*string == (wchar_t)ch)             /* wchar_t found ? */
                return( (wchar_t *)string );

        return(NULL);
}
#endif // 0

int _wtoi (
        const wchar_t *nptr
        )
{
        long res;
        res = (long)wcstoxl(nptr,NULL, 10, 0) ;

        return (res>0)?( (res > INT_MAX)? 0 : res ) : ( (res<INT_MIN)? 0 : res);
}


#ifndef UNIX_PLATFORM

long wcstol (
        const wchar_t *nptr,
        wchar_t **endptr,
        int ibase
        )
{
        return (long) wcstoxl(nptr, (const wchar_t **)endptr, ibase, 0);
}

unsigned long wcstoul (
        const wchar_t *nptr,
        wchar_t **endptr,
        int ibase
        )
{
        return wcstoxl(nptr, (const wchar_t **)endptr, ibase, FL_UNSIGNED);
}

#endif   // #ifndef UNIX_PLATFORM	

/***
*char *_itoa, *_ltoa, *_ultoa(val, buf, radix) - convert binary int to ASCII
*       string
*
*Purpose:
*       Converts an int to a character string.
*
*Entry:
*       val - number to be converted (int, long or unsigned long)
*       int radix - base to convert into
*       char *buf - ptr to buffer to place result
*
*Exit:
*       fills in space pointed to by buf with string result
*       returns a pointer to this buffer
*
*Exceptions:
*
*******************************************************************************/

/* helper routine that does the main job. */

void xtoa (
        unsigned long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if (is_neg) {
            /* negative, so output '-' and negate */
            *p++ = '-';
            val = (unsigned long)(-(long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * _itoa (
        int val,
        char *buf,
        int radix
        )
{
        if (radix == 10 && val < 0)
            xtoa((unsigned long)val, buf, radix, 1);
        else
            xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
        return buf;
}

char * _ltoa (
        long val,
        char *buf,
        int radix
        )
{
        if (radix == 10 && val < 0)
            ltoa((unsigned long)val, buf, radix);
        else
            ltoa((unsigned long)(unsigned int)val, buf, radix);
        return buf;
}

/***
*wchar_t *_itow, *_ltow, *_ultow(val, buf, radix) - convert binary int to wide
*       char string
*
*Purpose:
*       Converts an int to a wide character string.
*
*Entry:
*       val - number to be converted (int, long or unsigned long)
*       int radix - base to convert into
*       wchar_t *buf - ptr to buffer to place result
*
*Exit:
*       calls ASCII version to convert, converts ASCII to wide char into buf
*       returns a pointer to this buffer
*
*Exceptions:
*
*******************************************************************************/

wchar_t * _itow (
        int val,
        wchar_t *buf,
        int radix
        )
{
#define MAX_INT_LENGTH	256

        char astring[MAX_INT_LENGTH];

        _itoa (val, astring, radix);
        mbstowcs(buf,astring,MAX_INT_LENGTH);

        return (buf);
}

wchar_t * _ltow (
        long val,
        wchar_t *buf,
        int radix
        )
{
#define MAX_INT_LENGTH	256

        char astring[MAX_INT_LENGTH];

        _ltoa (val, astring, radix);
        mbstowcs(buf,astring,MAX_INT_LENGTH);

        return (buf);
}


#define MAX_NUMBER_OF_ARGS		20
#define _INTSIZEOF1(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

// sizeof  (max_digit) == sizeof(long double) <=12 byte {for linux} ;
#define	MAX_LEN_FOR_NOSTRING_ARG	30

//value  for  maxlen arg in aux_printf()
#define	 PROCESSED_FULL_LINE	(-1)

#if 0
// function prototype
int aux_printf(void* stream, const wchar_t *format, va_list arglist, int isFile, size_t maxlen);

// Auxiliary function for string formating
// stream - buffer (wchar_t*) if isFile==false
//			stream (FILE *)	  if isFile==true
int aux_printf(void* stream, const wchar_t *format, va_list arglist, int isFile, size_t maxlen)
{
#if 0
	size_t char_format_size,buff_size;
	char *char_format = NULL,*buff = NULL;
	char *buff_ptr = NULL;
	int arg_counter;
	int wchar_arg_pos[MAX_NUMBER_OF_ARGS];
	int wchar_n_args=0;
	int use_maxlen=0;
	
	char_format_size = wcslen(format)+1;
	
	if (maxlen !=PROCESSED_FULL_LINE && maxlen < (char_format_size-1)  ){
	 	char_format_size = maxlen+1;
	 	use_maxlen = 1;
	}
		
	
	char_format = (char *)malloc(char_format_size);
	buff = NULL;
	buff_size = char_format_size;
	
	if (char_format==NULL) return 0;
	wcstombs(char_format,format,char_format_size);
	if (use_maxlen)  char_format[char_format_size]=(char)0;
	
	wchar_arg_pos[0]=-1;

	// search wchar_t argument
	arg_counter=0;
	for(buff_ptr = char_format;*buff_ptr;buff_ptr++)
	{
		if (*buff_ptr=='%') 
		{
			char *saveBuffPtr = buff_ptr + 1;
			while( (isdigit( *saveBuffPtr ) || *saveBuffPtr=='-') && *saveBuffPtr ) saveBuffPtr++;
			if ( (buff_ptr+1)!=saveBuffPtr ) buff_ptr = (saveBuffPtr - 1);
			
			char s;
			s =*(buff_ptr+1);
			switch (s)
			{
				case 'S':
					// this is char string
					*(buff_ptr+1)='s';
					break;
				case 'l':
					if ( *(buff_ptr+2)=='s' ) {
						// convert wchar_t string to char string
						wchar_arg_pos[wchar_n_args++]=arg_counter;				
						*(buff_ptr+1) = 'h'; // exchange 'l' to 'h'
					}
					break;
				case 'C':
					// this is char character
					*(buff_ptr+1)='c';
					break;
				case 'c':
					// convert wchar_t character to char character...
					break;	
				default:
					buff_size += MAX_LEN_FOR_NOSTRING_ARG;				
			}
			arg_counter++;			
		}
		if (wchar_n_args==MAX_NUMBER_OF_ARGS) break;
	} //for		

	if (wchar_n_args!=0)
	{
		// convert wchar agruments
		for(arg_counter=0;arg_counter<wchar_n_args;arg_counter++)
		{
			char *char_buff;
			wchar_t *param_wchar;
			int char_buff_size;

			param_wchar = *((wchar_t **)((arglist)[0] + (_INTSIZEOF1(wchar_t* )) * wchar_arg_pos[arg_counter]));

			char_buff_size = wcslen (param_wchar)+1;
			
			char_buff = (char *)malloc(char_buff_size);
			if (char_buff==NULL) return 0;			
			wcstombs(char_buff,param_wchar,char_buff_size);

			buff_size +=   char_buff_size;
				
			*((char **)((arglist)[0] + (_INTSIZEOF1(wchar_t* )) * wchar_arg_pos[arg_counter]))
				 = char_buff;
		}
	}


	if (isFile) {
		// Write formatted output to file stream
		buff_size = vfprintf((FILE *) stream,char_format,arglist);
	}
	else {
		// malloc for  buff
		buff = (char *)malloc(buff_size);		
		
		// Write formatted output to the memory
		
		vsprintf(buff,char_format,arglist);
		// convert result
		buff_size =strlen( buff)+1;
		mbstowcs((wchar_t*) stream,buff,buff_size);
	}

	// free allocation memory
	if (wchar_n_args!=0)
	{
		// convert wchar agruments
		for(arg_counter=0;arg_counter<wchar_n_args;arg_counter++)
		{
			char *char_buff = *((char **)((arglist)[0] + (_INTSIZEOF1(wchar_t* )) * wchar_arg_pos[arg_counter]));

			if (char_buff!=NULL) free(char_buff);
		}
	}

	if (buff!=NULL) free(buff);
	if (char_format!=NULL) free(char_format);

	return (buff_size-1);
#else // 0
//    *((wchar_t *)stream) = L'X';
//    *((wchar_t *)stream + 1) = 0;
    *((wchar_t *)stream) = 0;
    wcerr << __FILE__ << ":" << __LINE__ << " " << format << endl;
    return 1;
#endif
}

#endif // 0

int vswprintf(wchar_t *buffer, const wchar_t *format, va_list arglist)
{
  wstringstream ws;
  _safe_printer( ws, format, arglist );
  wstring s = ws.str();
  //wcerr << s.c_str() << endl; 
  wcsncpy( buffer, s.c_str(), s.size() + 1 );
  return s.size();
  
//	return aux_printf(buffer,format,arglist,0,PROCESSED_FULL_LINE);
}

int	vfwprintf(FILE *stream, const wchar_t *format, va_list arglist)
{
  wofstream fs( fileno(stream) );
  _safe_printer( fs, format, arglist );
  return 100; // don't know
}

int	swprintf(wchar_t *buffer, const wchar_t *format, ...)
{
	va_list arglist;
	int ret;

    va_start(arglist, format);
	ret=vswprintf(buffer,format,arglist);
	va_end(arglist);
	return ret;
}

int	fwprintf(FILE *stream, const wchar_t *format, ...)
{
	va_list arglist;
	int ret;

    va_start(arglist, format);
	ret=vfwprintf(stream,format,arglist);
	va_end(arglist);
	return ret;
}

int	wprintf(const wchar_t *format, ...)
{
	va_list arglist;
	int ret;

    va_start(arglist, format);
	ret=vfwprintf(stdout,format,arglist);
	va_end(arglist);
	return ret;
}

int vsnwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, va_list arglist);
int snwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, ...);

int vsnwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, va_list arglist)
{
  wstringstream ws;
  _safe_printer( ws, format, arglist );
  size_t sz = min(ws.str().size() + 1, maxlen);
  wcsncpy( str, ws.str().c_str(), sz );
  return sz;
  // return aux_printf(str,format,arglist,0, maxlen);
}

int snwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, ...)
{
	va_list arglist;
	int ret;

   	va_start(arglist, format);
	ret=  vsnwprintf (str,maxlen,format,arglist);
	va_end(arglist);
	return ret;			
}

// ################################################################################
// ################################################################################
/*	for  support porting  IO from windows-platform   	*/
// ################################################################################

int _vsnwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, va_list arglist)
{
	return vsnwprintf  (str, maxlen,format,arglist);
}

int _snwprintf (wchar_t *str, size_t maxlen, const wchar_t *format, ...)
{
	va_list arglist;
	int ret;

    	va_start(arglist, format);
    	char my_str[256];
    	char my_format[256];
        wcstombs(my_format,format,256);
	ret=  vsprintf (my_str,my_format,arglist);

        mbstowcs(str,my_str,strlen(my_str)+1);

	va_end(arglist);
	return ret;
	
}
// Morozov
int _snprintf (char *str, size_t maxlen, const char *format, ...)
{
	va_list arglist;
	int ret;

    	va_start(arglist, format);
	ret=  vsprintf (str,format,arglist);
	va_end(arglist);
	return ret;	

}

 #ifdef N_PLAT_NLM
	wchar_t initNovellPath_[256];
  	extern wchar_t g_programPathW[256];
 	
	wchar_t* initNovellPath(const wchar_t* fileName){
		initNovellPath_[0] = 0;
		wcscat(initNovellPath_,g_programPathW);
		wcscat(initNovellPath_,fileName);
	return initNovellPath_; 
}
#endif


#ifdef UNIX_PLATFORM	

int _vsprintf(char *buffer , const char *format, va_list argptr)
{
	return vsprintf(buffer,format,argptr);
}

int _vsnprintf(char *buffer , size_t size, const char *format, va_list argptr)
{	
	return vsnprintf(buffer,size,format,argptr);
}

#endif // if not def UNIX_PLATFORM

int iswpunct( int  c )
{
   return ispunct( c );
}

#else // !__unix

#ifndef __unix__

/***
*wchar_t towupper(c) - convert wide character to upper case
*
*Purpose:
*       towupper() returns the uppercase equivalent of its argument
*
*Entry:
*       c - wchar_t value of character to be converted
*
*Exit:
*       if c is a lower case letter, returns wchar_t value of upper case
*       representation of c. otherwise, it returns c.
*
*Exceptions:
*
*******************************************************************************/

wchar_t towupper (
        wchar_t c
        )
{
	if ( (c >= L'a') && (c <= L'z') )
		c = c - L'a' + L'A';
	return c;        
}

/***
*wchar_t towlower(c) - convert wide character to lower case
*
*Purpose:
*       towlower() returns the lowercase equivalent of its argument
*
*Entry:
*       c - wchar_t value of character to be converted
*
*Exit:
*       if c is an upper case letter, returns wchar_t value of lower case
*       representation of c. otherwise, it returns c.
*
*Exceptions:
*
*******************************************************************************/

wchar_t towlower (
			  wchar_t c
				  )
{
	
	if ( (c >= L'A') && (c <= L'Z') )
		c = c - L'A' + L'a';
    return c;    
}

#endif // __unix__

/***
*int wcsicmp(dst, src) - compare wide-character strings, ignore case
*
*Purpose:
*       _wcsicmp perform a case-insensitive wchar_t string comparision.
*       _wcsicmp is independent of locale.
*
*Entry:
*       wchar_t *dst, *src - strings to compare
*
*Return:
*       <0 if dst < src
*        0 if dst = src
*       >0 if dst > src
*       This range of return values may differ from other *cmpcoll functions.
*
*Exceptions:
*
*******************************************************************************/

int wcsicmp (
        const wchar_t * dst,
        const wchar_t * src
        )
{
	wchar_t f,l;
	do  {
		f = towlower( (wchar_t )(*(dst++)) );
		l = towlower( (wchar_t )(*(src++)) );
	} while ( (f) && (f == l) );

	return (int)(f - l);
}

#endif // !__unix

#ifdef __cplusplus
}
#endif
