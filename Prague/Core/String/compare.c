#include "pstring.h"
#include "codec.h"
#include <ctype.h>

#define IMPEX_EXPORT_LIB
#include <Prague/plugin/p_string.h>

#define CHECK_SLASH(f,s,slash) \
	if ( slash ) {							 \
		if ( f == '\\' ) f = '/';	 \
		if ( s == '\\' ) s = '/';	 \
	}


#define CHECK_SLASH_F(fc,sc,slash,f,t) \
	if ( fc == '\\' ) {                  \
		if ( slash ) fc = '/';             \
	}                                    \
	else                                 \
		fc = f( (t)fc );                   \
	if ( sc == '\\' ) {                  \
		if ( slash ) sc = '/';             \
	}                                    \
	else                                 \
		sc = f( (t)sc )


#define CMP_AND_RET( fc, sc ) \
	if ( fc < sc )							\
		return cSTRING_COMP_LT;		\
	if ( fc > sc )							\
		return cSTRING_COMP_GT

#define ASCII2UPPER_OR_BREAK( c )           \
	if ( c > 0x80 )							\
		break;		                        \
	if ( c >= 'a' && c <= 'z' )				\
		c &= ~0x20;

// ---
tERROR _cmp( tPTR f, tPTR s, tCODEPAGE cp, tUINT l, tDWORD flags ) {
  tUINT i, step;
	tSYMBOL fc;
	tSYMBOL sc;
	tSTR_CASE op;
	tBOOL slash;

	if ( !l )
		return errOK;
	if ( !f != !s )
		return errNOT_MATCHED;
	if ( !f && !s )
		return errOK;

	slash = !!(flags & fSTRING_COMPARE_SLASH_INSENSITIVE);
	flags &= fSTRING_COMPARE_CASE_INSENSITIVE;
	if ( (flags == fSTRING_COMPARE_UPPER) || (flags == (fSTRING_COMPARE_UPPER|fSTRING_COMPARE_LOWER)) )
	{
		op = cSTRING_TO_UPPER;
		if ( cp == cCP_UNICODE ) {
			for( i=0; i<l; i+=sizeof(tWCHAR), ++UNIR(f), ++UNIR(s) ) {
				fc = *UNI(f);
				ASCII2UPPER_OR_BREAK(fc);
				sc = *UNI(s);
				ASCII2UPPER_OR_BREAK(sc);
				CHECK_SLASH( fc, sc, slash );
				if ( fc != sc )
					return errNOT_MATCHED;
			}
		}
		else {
			for( i=0; i<l; ++i, ++MBR(f), ++MBR(s) ) {
				fc = *MB(f); 
				ASCII2UPPER_OR_BREAK(fc);
				sc = *MB(s);
				ASCII2UPPER_OR_BREAK(sc);
				CHECK_SLASH( fc, sc, slash );
				if ( fc != sc )
					return errNOT_MATCHED;
			}
		}
		if (i == l)
			return errOK;
	}
	else if ( flags == fSTRING_COMPARE_LOWER )
		op = cSTRING_TO_LOWER;
	else if ( cp == cCP_UNICODE ) {
		for( i=0; i<l; i+=sizeof(tWCHAR), ++UNIR(f), ++UNIR(s) ) {
			fc = *UNI(f);
			sc = *UNI(s);
			CHECK_SLASH( fc, sc, slash );
			if ( fc != sc )
				return errNOT_MATCHED;
		}
		return errOK;
	}
	else {
		for( i=0; i<l; ++i, ++MBR(f), ++MBR(s) ) {
			fc = *MB(f); 
			sc = *MB(s);
			CHECK_SLASH( fc, sc, slash );
			if ( fc != sc )
				return errNOT_MATCHED;
		}
		return errOK;
	}

	if ( cp == cCP_UNICODE )
		step = sizeof(tWCHAR);
	else
		step = sizeof(tCHAR);
	
	for( i=0,fc=sc=0; i<l; i+=step, MBR(f)+=step, MBR(s)+=step ) {
		PrStringChCase ( f, step, cp, MB(&fc), op );
		PrStringChCase ( s, step, cp, MB(&sc), op );
		CHECK_SLASH( fc, sc, slash );
		if ( fc != sc )
			return errNOT_MATCHED;
	}    
	return errOK;
}

__inline tUINT GetSizeFromCP(tCODEPAGE cp)
{
	switch (cp)
	{
	case cCP_UNICODE: 
		return sizeof(tWCHAR);
	case cCP_ANSI:
	case cCP_OEM:
		return 1;
	}
	return 0;
}

tUINT _icmpS_ASCII_CHAR( tSTR_COMPARE* presult, tBYTE** ps1, tUINT size1, tBYTE** ps2, tUINT size2, tUINT len, tDWORD flags) 
{
	tBYTE c1, c2;
	tSTR_COMPARE result = cSTRING_COMP_EQ;
	tBYTE *s1,*s2;

	s1 = *ps1;
	s2 = *ps2;
	while (len)
	{
		c1 = *s1;
		if (c1 >= 0x80)
			break;
		c2 = *s2;
		if (c2 >= 0x80)
			break;
		if (flags & fSTRING_COMPARE_SLASH_INSENSITIVE)
		{
			if (c1 == '\\')
				c1 = '/';
			if (c2 == '\\')
				c2 = '/';
		}
		if (flags & fSTRING_COMPARE_CASE_INSENSITIVE)
		{
			if (c1 >= 'A' && c1 <='Z')
				c1 |= 0x20;
			if (c2 >= 'A' && c2 <='Z')
				c2 |= 0x20;
		}
		if (c1 < c2)
		{
			result = cSTRING_COMP_LT;
			break;
		}
		if (c1 > c2)
		{
			result = cSTRING_COMP_GT;
			break;
		}
		
		s1++;
		s2++;
		len--;
	}
	*ps1 = s1;
	*ps2 = s2;
	*presult = result;
	return len;
}

tUINT _icmpS_ASCII_WCHAR( tSTR_COMPARE* presult, tBYTE** ps1, tUINT size1, tBYTE** ps2, tUINT size2, tUINT len, tDWORD flags) 
{
	tUINT c1, c2;
	tSTR_COMPARE result = cSTRING_COMP_EQ;
	tWCHAR *s1,*s2;

	s1 = (tWCHAR*)*ps1;
	s2 = (tWCHAR*)*ps2;
	while (len)
	{
		c1 = *s1;
		if (c1 >= 0x80)
			break;
		c2 = *s2;
		if (c2 >= 0x80)
			break;
		if (flags & fSTRING_COMPARE_SLASH_INSENSITIVE)
		{
			if (c1 == '\\')
				c1 = '/';
			if (c2 == '\\')
				c2 = '/';
		}
		if (flags & fSTRING_COMPARE_CASE_INSENSITIVE)
		{
			if (c1 >= 'A' && c1 <='Z')
				c1 |= 0x20;
			if (c2 >= 'A' && c2 <='Z')
				c2 |= 0x20;
		}
		if (c1 < c2)
		{
			result = cSTRING_COMP_LT;
			break;
		}
		if (c1 > c2)
		{
			result = cSTRING_COMP_GT;
			break;
		}
		
		s1++;
		s2++;
		len--;
	}
	*ps1 = (tBYTE*)s1;
	*ps2 = (tBYTE*)s2;
	*presult = result;
	return len;
}

tUINT _icmpS_ASCII( tSTR_COMPARE* presult, tBYTE** ps1, tUINT size1, tBYTE** ps2, tUINT size2, tUINT len, tDWORD flags) 
{
	tUINT c1, c2;
	tSTR_COMPARE result = cSTRING_COMP_EQ;
	tBYTE *s1,*s2;

	s1 = *ps1;
	s2 = *ps2;
	while (len)
	{
		c1 = ( size1 == 2 ? *(tWCHAR*)s1 : *(tBYTE*)s1 );
		if (c1 >= 0x80)
			break;
		c2 = ( size2 == 2 ? *(tWCHAR*)s2 : *(tBYTE*)s2 );
		if (c2 >= 0x80)
			break;
		if (flags & fSTRING_COMPARE_SLASH_INSENSITIVE)
		{
			if (c1 == '\\')
				c1 = '/';
			if (c2 == '\\')
				c2 = '/';
		}
		if (flags & fSTRING_COMPARE_CASE_INSENSITIVE)
		{
			if (c1 >= 'A' && c1 <='Z')
				c1 |= 0x20;
			if (c2 >= 'A' && c2 <='Z')
				c2 |= 0x20;
		}
		if (c1 < c2)
		{
			result = cSTRING_COMP_LT;
			break;
		}
		if (c1 > c2)
		{
			result = cSTRING_COMP_GT;
			break;
		}
		
		s1 += size1;
		s2 += size2;
		len--;
	}
	*ps1 = s1;
	*ps2 = s2;
	*presult = result;
	return len;
}


tBOOL _cmpS_ASCII( tSTR_COMPARE* presult, tStrDsc* ps1, tStrDsc* ps2, tDWORD flags ) {
	tSTR_COMPARE result = cSTRING_COMP_EQ;
	tUINT len, len1, len2, cmp_len;
	tBYTE *s1, *s2;
	tUINT size1, size2;
	tBOOL empty1, empty2;

	empty1 = !ps1->m_ptr || !ps1->m_size;
	empty2 = !ps2->m_ptr || !ps2->m_size;

	if ( empty1 && !empty2 ) {
		*presult = cSTRING_COMP_LT;
		return cTRUE;
	}

	if ( !empty1 && empty2 ) {
		*presult = cSTRING_COMP_GT;
		return cTRUE;
	}

	if ( empty1 && empty2 )	{
		*presult = cSTRING_COMP_EQ;
		return cTRUE;
	}

	size1 = GetSizeFromCP( ps1->m_cp );
	if ( !size1 ) {
		*presult = cSTRING_COMP_UNDEF;
		return cFALSE;
	}

	size2 = GetSizeFromCP( ps2->m_cp );
	if ( !size2 ) {
		*presult = cSTRING_COMP_UNDEF;
		return cFALSE;
	}

	s1 = (tBYTE*)ps1->m_ptr;
	s2 = (tBYTE*)ps2->m_ptr;

	len1 = ((size1 == 2) ? ps1->m_size/2 : ps1->m_size);
	len2 = ((size2 == 2) ? ps2->m_size/2 : ps2->m_size);
	cmp_len = len = (len1 < len2 ? len1 : len2);

//#if 1
	if ( size1 == size2 ) {
		if ( size1 == 1 ) {
			// timing:
			//  7640  ANSI-ANSI (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE) +30% по сравнению с _icmpS_ASCII
			//  3800  ANSI-ANSI (0) +70% по сравнению с _icmpS_ASCII
			len = _icmpS_ASCII_CHAR(&result, &s1, size1, &s2, size2, len, flags); // 
		}
		else {
			// timing:
			//  7340  UNI-UNI (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE) +35% по сравнению с _icmpS_ASCII
			//  3800  UNI-UNI (0) +70% по сравнению с _icmpS_ASCII
			len = _icmpS_ASCII_WCHAR(&result, &s1, size1, &s2, size2, len, flags); // +28% по сравнению с _icmpS_ASCII
		}
	}
	else {
		// timing:
		//  9940  ANSI-UNI  (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE) + 10% по сравнению с циклом здесь
		//  6480  ANSI-UNI  (0) 
		len = _icmpS_ASCII(&result, &s1, size1, &s2, size2, len, flags); // 
	}
/*
#else
	tUINT c1, c2;
	// timing:
	// 10940  ANSI-ANSI (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE)
	//  6700  ANSI-ANSI (0)
	// 11000  ANSI-UNI  (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE)
	//  6700  ANSI-UNI  (0)
	// 10960  UNI-UNI   (fSTRING_COMPARE_SLASH_INSENSITIVE | fSTRING_COMPARE_CASE_INSENSITIVE)
	//  6750  UNI-UNI   (0)
	while (len)
	{
		c1 = ( size1 == 2 ? *(tWCHAR*)s1 : *(tBYTE*)s1 );
		if (c1 >= 0x80)
			break;
		c2 = ( size2 == 2 ? *(tWCHAR*)s2 : *(tBYTE*)s2 );
		if (c2 >= 0x80)
			break;
		if (flags & fSTRING_COMPARE_SLASH_INSENSITIVE)
		{
			if (c1 == '\\')
				c1 = '/';
			if (c2 == '\\')
				c2 = '/';
		}
		if (flags & fSTRING_COMPARE_CASE_INSENSITIVE)
		{
			if (c1 >= 'A' && c1 <='Z')
				c1 |= 0x20;
			if (c2 >= 'A' && c2 <='Z')
				c2 |= 0x20;
		}
		if (c1 < c2)
		{
			result = cSTRING_COMP_LT;
			break;
		}
		if (c1 > c2)
		{
			result = cSTRING_COMP_GT;
			break;
		}
		
		s1 += size1;
		s2 += size2;
		len--;
	}	
#endif
*/
	len = cmp_len - len;
	ps1->m_ptr = s1;
	ps1->m_size -= (size1 == 2 ? len+len : len);
	ps2->m_ptr = s2;
	ps2->m_size -= (size2 == 2 ? len+len : len);
	if ( result == cSTRING_COMP_EQ ) {
		if ( len < cmp_len ) {
			*presult = cSTRING_COMP_UNDEF;
			return cFALSE;
		}
		
		if (len1 < len2)
			result = cSTRING_COMP_LT;
		else if (len1 > len2)
			result = cSTRING_COMP_GT;
	}
	*presult = result;
	return cTRUE;
}


// ---
tSTR_COMPARE _cmpS( tPTR f, tPTR s, tCODEPAGE cp, tUINT l, tDWORD flags ) {
	tUINT i;
	tCHAR as[2];
	tWCHAR us[2];
	tBOOL slash;

	if ( !l )
		return errOK;
	if ( !f && s )
		return cSTRING_COMP_LT;
	if ( f && !s )
		return cSTRING_COMP_GT;
	if ( !f && !s )
		return cSTRING_COMP_EQ;

	slash = !!(flags & fSTRING_COMPARE_SLASH_INSENSITIVE);
	switch ( flags & fSTRING_COMPARE_CASE_INSENSITIVE ) {
		case fSTRING_COMPARE_UPPER:
		case fSTRING_COMPARE_UPPER|fSTRING_COMPARE_LOWER:
			switch( cp ) {
				case cCP_UNICODE :
					for( i=0; i<l; i+=sizeof(tWCHAR),++UNIR(f),++UNIR(s) ) {
						us[0] = *UNI(f);
						us[1] = *UNI(s);
						CHECK_SLASH_F( us[0], us[1], slash, towupper, tWCHAR );
						CMP_AND_RET( us[0], us[1] );
					}
					break;

				case cCP_ANSI:
					for( i=0; i<l; ++i,++MBR(f),++MBR(s) ) {
						as[0] = *MB(f);
						as[1] = *MB(s);
						CHECK_SLASH_F( as[0], as[1], slash, toupper, tCHAR );
						CMP_AND_RET( as[0], as[1] );
					}
					break;

				default:
					for( i=0; i<l; ++i,++MBR(f),++MBR(s) ) {
						as[0] = *MB(f);
						as[1] = *MB(s);
						PrStringCodec ( as, sizeof(as), cCP_OEM, as, sizeof(as), cCP_ANSI, NULL );
						CHECK_SLASH_F( as[0], as[1], slash, toupper, tCHAR );
						CMP_AND_RET( as[0], as[1] );
					}
					break;
			}
			break;

		case fSTRING_COMPARE_LOWER:
			switch( cp ) {
				case cCP_UNICODE:
					for( i=0; i<l; i+=sizeof(tWCHAR),++UNIR(f),++UNIR(s) ) {
						us[0] = *UNI(f);
						us[1] = *UNI(s);
						CHECK_SLASH_F( us[0], us[1], slash, towlower, tWCHAR );
						CMP_AND_RET( us[0], us[1] );
					}
					break;

				case cCP_ANSI:
					for( i=0; i<l; ++i,++MBR(f),++MBR(s) ) {
						as[0] = *MB(f);
						as[1] = *MB(s);
						CHECK_SLASH_F( as[0], as[1], slash, tolower, tCHAR );
						CMP_AND_RET( as[0], as[1] );
					}
					break;

				default:
					for( i=0; i<l; ++i,++MBR(f),++MBR(s) ) {
						as[0] = *MB(f);
						as[1] = *MB(s);
						PrStringCodec ( as, sizeof(as), cCP_OEM, as, sizeof(as), cCP_ANSI, NULL );
						CHECK_SLASH_F( as[0], as[1], slash, tolower, tCHAR );
						CMP_AND_RET( as[0], as[1] );
					}
					break;
			}
			break;

		default:
			switch( cp ) {
				case cCP_UNICODE:
					for( i=0; i<l; i+=sizeof(tWCHAR),++UNIR(f),++UNIR(s) ) {
						us[0] = *UNI(f);
						us[1] = *UNI(s);
						CHECK_SLASH( us[0], us[1], slash );
						CMP_AND_RET( us[0], us[1] );
					}
					break;

				default:
					for( i=0; i<l; ++i,++MBR(f),++MBR(s) ) {
						as[0] = *MB(f);
						as[1] = *MB(s);
						CHECK_SLASH( as[0], as[1], slash );
						CMP_AND_RET( as[0], as[1] );
					}
					break;
			}
			break;
	}
	return cSTRING_COMP_EQ;
}




