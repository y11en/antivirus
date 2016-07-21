#include <stdarg.h>
#include "prop_in.h"


// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Comp_Sequences( AVP_dword* seq1, AVP_dword* seq2 ) {
  if ( seq1 && seq2 ) {
    AVP_bool equ;
    while( 1 ) {
      equ = *seq1 == *seq2;
      if ( !*seq1 || !*seq2 || !equ )
        break;
      else
        seq1++; seq2++;
    };
    return equ;
  }
  else
    return 0;
}




