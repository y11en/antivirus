////////////////////////////////////////////////////////////////////
//
//  CALCSUM.C
//  AVP CalcSum implementation.
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include "calcsum.h"

unsigned long CalcSum( unsigned char* mass,unsigned long len)
{
	register unsigned long i;
	register unsigned long s;
	register unsigned char* m;
	s=0;
	m=mass;
	if(len<4){
		for (i=0; i<len ; i++,m++){
			s^=s<<8;
			s^=*m;
		}
		return s;
	}
	
	for (i=0; i<4 ; i++,m++){
		s^=s<<8;
		s^=*m;
	}
	for ( ;i<len; i++,m++){
		s^=s<<1;
		s^=*m;
	}
	return s;
}
