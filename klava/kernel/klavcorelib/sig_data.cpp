// sig_data.cpp
//
//

#include <klava/klav_sig.h>
#include <utf8.h>

////////////////////////////////////////////////////////////////
// Internal signature element layout
//
//	KLAV_SIG_ELT_BYTES     <UTF8-coded length> <bytes...>
//	KLAV_SIG_ELT_IBYTES    <UTF8-coded length> <bytes...>
//	KLAV_SIG_ELT_ANYBYTES, <UTF8-coded length>
//	KLAV_SIG_ELT_BITMASK,  <&-MASK> <VALUE>
//	KLAV_SIG_ELT_RANGE,    <LBOUND> <HBOUND>
//	KLAV_SIG_ELT_STAR      <LBOUND> <HBOUND>
//
////////////////////////////////////////////////////////////////
// KLAV_Sig_Element

size_t KLAV_CALL KLAV_Sig_Element_Scan (
		const uint8_t * ptr,
		size_t len,
		struct KLAV_Sig_Element *elt
	)
{
	if (len == 0)
		return 0;

	elt->m_type = KLAV_SIG_ELT_INVALID;
	elt->m_size = 0;
	elt->m_args[0] = 0;
	elt->m_args[1] = 0;

	const uint8_t code = ptr[0];

	switch (code)
	{
	case KLAV_SIG_ELT_BYTES:
	case KLAV_SIG_ELT_IBYTES:
		if (utf8_char_validate ((const char *)ptr + 1, (const char *)(ptr + len)) != 0)
		{
			elt->m_type = code;
			elt->m_args[1] = 1 + utf8_decode_char ((const char *)ptr + 1, &(elt->m_args[0]));
			elt->m_size = elt->m_args[1] + elt->m_args[0];
		}
		else
		{
			return 0;
		}
		break;

	case KLAV_SIG_ELT_ANYBYTES:
	case KLAV_SIG_ELT_STAR:
		if (utf8_char_validate ((const char *)ptr + 1, (const char *)(ptr + len)) != 0)
		{
			uint16_t val = 0;
			elt->m_type = code;
			elt->m_size = 1 + utf8_decode_char ((const char *)ptr + 1, & val);
			elt->m_args[0] = (uint8_t) (val & 0xFF);
			elt->m_args[1] = (uint8_t) ((val >> 8) & 0xFF);
		}
		else
		{
			return 0;
		}
		break;

	case KLAV_SIG_ELT_BITMASK:
	case KLAV_SIG_ELT_RANGE:
		elt->m_type = code;
		elt->m_size = 3;
		elt->m_args[0] = ptr[1];
		elt->m_args[1] = ptr[2];
		break;

	default:
		return 0;
	}

	if (elt->m_size > len)
		return 0;

	return elt->m_size;
}

KLAV_EXTERN_C
unsigned int KLAV_CALL KLAV_Sig_Element_Flags (
		const uint8_t * data,
		size_t data_size
	)
{
	unsigned int flags = 0;

	const uint8_t *p = data;
	const uint8_t *pe = p + data_size;

	KLAV_Sig_Element elt;

	while (p < pe)
	{
		size_t sz = elt.scan (p, pe - p);
		if (sz == 0)
		{
			flags |= KLAV_SIG_F_INVALID;
			break;
		}

		p += sz;

		switch (elt.type ())
		{
		case KLAV_SIG_ELT_BYTES:
			break;
		case KLAV_SIG_ELT_IBYTES:
			flags |= KLAV_SIG_F_HAS_IBYTES;
			break;
		case KLAV_SIG_ELT_ANYBYTES:
			flags |= (KLAV_SIG_F_HAS_ANYBYTES | KLAV_SIG_F_HAS_WILDCARDS);
			break;
		case KLAV_SIG_ELT_BITMASK:
			flags |= (KLAV_SIG_F_HAS_BITMASKS | KLAV_SIG_F_HAS_WILDCARDS);
			break;
		case KLAV_SIG_ELT_RANGE:
			flags |= (KLAV_SIG_F_HAS_RANGES | KLAV_SIG_F_HAS_WILDCARDS);
			break;
		case KLAV_SIG_ELT_STAR:
			flags |= (KLAV_SIG_F_HAS_STARS | KLAV_SIG_F_HAS_WILDCARDS);
			break;
		default:
			flags |= KLAV_SIG_F_INVALID;
			return flags;
		}
	}

	return flags;
}
