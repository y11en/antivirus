/***
*xtoa.c - convert integers/longs to ASCII string
*
*       Copyright (c) 1989-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       The module has code to convert integers/longs to ASCII strings.  See
*
*******************************************************************************/

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

static void __cdecl xtoa( unsigned long val, char *buf, unsigned radix, int is_neg ) {
  char *firstdig;         /* pointer to first digit */
  char temp;              /* temp char */
  unsigned digval;        /* value of digit */
  char* p = buf;          /* pointer to traverse string */

  if ( is_neg ) {
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
// ---
char * __cdecl itoa ( int val, char *buf, int radix ) {
  if ( (radix == 10) && (val < 0) )
    xtoa( (unsigned long)val, buf, radix, 1 );
  else
    xtoa( (unsigned long)(unsigned int)val, buf, radix, 0 );
  return buf;
}



// ---
char * __cdecl ltoa ( long val, char *buf, int radix ) {
  xtoa( (unsigned long)val, buf, radix, (radix == 10 && val < 0));
  return buf;
}

char * __cdecl ultoa ( unsigned long val, char *buf, int radix ) {
  xtoa( val, buf, radix, 0 );
  return buf;
}

#ifndef _NO_INT64

// ---
static unsigned __stdcall i64rem( __int64 v, unsigned d ) {
  unsigned long rh = *(((unsigned long*)&v)+1);
  if ( rh ) {
    rh %= d;
    if ( rh ) {
      rh = (rh * (((unsigned long)v) >> 16)) % d;
      if ( rh )
        return (rh * ((unsigned short)v)) % d;
      else
        return 0;
    }
    else
      return 0;
  }
  else
    return ((unsigned long)v) % d;
}


// ---
static __int64 __stdcall i64div( __int64 v, unsigned d ) {

  unsigned long rh = *(((unsigned long*)&v)+1);
  if ( rh ) {
    __int64 res;
    unsigned long rlh;
    unsigned long rem = rh % d;                             // reminder
    rh /= d;                                                // result high dword
    rlh = (rem * 0xffff) + (((unsigned long)v) / 0xffff);   // high word of low dword with previous reminder
    rem = rlh % d;                                          
    rlh /= d;                                               // result high word of low dword
    ((unsigned long)res) = ((rem * 0xffff) + (((unsigned short)v))) / d;  // result low word of low dword
    *(((unsigned long*)&res)+1) = rh;
    return res;
  }
  else
    return ((unsigned long)v) / d;
}


// ---
/* stdcall is faster and smaller... Might as well use it for the helper. */
static void __stdcall x64toa( unsigned __int64 val, char *buf, unsigned radix, int is_neg ) {
  char *firstdig;         /* pointer to first digit */
  char temp;              /* temp char */
  unsigned digval;        /* value of digit */
  char* p = buf;          /* pointer to traverse string */

  if ( is_neg ) {
    *p++ = '-';         /* negative, so output '-' and negate */
    val = (unsigned __int64)(-(__int64)val);
  }

  firstdig = p;           /* save pointer to first digit */

  do {
    digval = i64rem( val, radix );
    val = i64div( val, radix );       /* get next digit */

    if ( digval > 9 )                     /* convert to ascii and store */
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

char * __cdecl i64toa( __int64 val, char *buf, int radix ) {
  x64toa( (unsigned __int64)val, buf, radix, (radix == 10 && val < 0) );
  return buf;
}

char * __cdecl ui64toa ( unsigned __int64 val, char *buf, int radix ) {
  x64toa(val, buf, radix, 0);
  return buf;
}

#endif  /* _NO_INT64 */
