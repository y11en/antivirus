// utf8.h
//
// UTF8 encoding/decoding routines
//
// (C) 2003 Kaspersky Labs
//

#ifndef	utf8_h_INCLUDED
#define	utf8_h_INCLUDED

#include <stdlib.h>

const unsigned int UTF8_MAX_CHAR = 0xFFFFU;
const unsigned int UTF8_MAX_CHAR_SIZE = 3;
const unsigned int UTF8_MAX_GROW_FACTOR = UTF8_MAX_CHAR_SIZE;

// returns size of UTF-8 character (in bytes) (0 if character is invalid)
// this method does not check character validity or normalization
inline int utf8_char_size (unsigned int wc)
{
	if (wc <= 0x007F) return 1;
	if (wc <= 0x07FF) return 2;
	if (wc <= 0xFFFF) return 3;
	return 0;
}

// returns size of UTF-8 character (in bytes) (0 if character is invalid)
// this method does not check character validity or normalization
inline unsigned int utf8_char_size (const char *s)
{
	const unsigned char c = * (const unsigned char *)s;
	if ((c & 0x80) == 0x00) return 1;
	if ((c & 0xE0) == 0xC0) return 2;
	if ((c & 0xF0) == 0xE0) return 3;
	return 0;
}

// checks character structure and normalization
// return character size in bytes or 0 on error
// zero character is considered invalid
inline unsigned int utf8_char_validate (const char *s, const char *se)
{
	if (s < se)
	{
		const unsigned char c = s[0];
		if ((c & 0x80) == 0x00)
		{
			return (c == 0) ? 0 : 1;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			if (se - s < 2) return 0;          // invalid length
			const unsigned char c1 = s[1];
			if ((c1 & 0xC0) != 0x80) return 0; // invalid structure
			if ((c & 0x1E) == 0) return 0;     // invalid normalization
			return 2;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			if (se - s < 3) return 0;            // invalid length
			const unsigned char c1 = s[1];
			if ((c1 & 0xC0) != 0x80) return 0;   // invalid structure
			if (((c & 0x0F) | (c1 & 0x80)) == 0) return 0; // invalid normalization
			const unsigned char c2 = s[2];
			if ((c2 & 0xC0) != 0x80) return 0;   // invalid structure
			return 3;
		}
	}
	return 0;
}

inline bool utf8_string_validate (const char *s, size_t src_len, size_t *codepoint_cnt)
{
	*codepoint_cnt = 0;

	const char *se = s + src_len;
	while (s < se)
	{
		unsigned int len = utf8_char_validate (s, se);
		if (len == 0)
			return false;
		s += len;
		++(*codepoint_cnt);
	}
	return true;
}

inline bool utf8_string_validate (const char *s, size_t src_len)
{
	const char *se = s + src_len;
	while (s < se)
	{
		unsigned int len = utf8_char_validate (s, se);
		if (len == 0)
			return false;
		s += len;
	}
	return true;
}

// deprecated, added for backward compatibility
inline bool utf8_valid_string (const char *s, size_t src_len, size_t *cp_len)
{
	if (cp_len == 0)
		return utf8_string_validate (s, src_len);
	else
		return utf8_string_validate (s, src_len, cp_len);
}

inline bool utf8_valid_string (const char *s, size_t src_len)
{
	return utf8_string_validate (s, src_len);
}

// encode wc into buffer, return number of bytes produced, 0 on error
template <typename WC>
inline int utf8_encode_char (WC wc, char *s)
{
	if (wc <= 0x7F) {
		s[0] = (char)wc;
		return 1;
	} else if (wc <= 0x07FF) {
		s[0] = (char)(0xC0 | ((wc >> 6) & 0x1F));
		s[1] = (char)(0x80 | (wc & 0x3F));
		return 2;
	} else if (wc <= 0xFFFF) {
		s[0] = (char)(0xE0 | ((wc >> 12) & 0x0F));
		s[1] = (char)(0x80 | ((wc >> 6) & 0x3F));
		s[2] = (char)(0x80 | (wc & 0x3F));
		return 3;
	}
	return 0;
}

// decode wc from buffer, return number of bytes consumed, 0 on error
template <typename WC>
inline int utf8_decode_char (const char *s, WC *wc)
{
	if ((s[0] & 0x80) == 0x00) {
		*wc = s[0];
		return 1;
	}
	if ((s[0] & 0xE0) == 0xC0) {
		*wc = (((wchar_t)(s[0] & 0x1F)) << 6) | (s[1] & 0x3F);
		return 2;
	}
	if ((s[0] & 0xF0) == 0xE0) {
		*wc = (((((wchar_t)(s[0] & 0x0F)) << 6) | (s[1] & 0x3F)) << 6)
			| (s[2] & 0x3F);
		return 3;
	}
	return 0;
}

// string functions
// functions assume that output buffer has enough capacity (at least *3)
// return number of output characters

template<class WC>
int	utf8_encode_string (const WC *src, size_t src_len, char *dst)
{
	char *p = dst;
	const WC *wse = src + src_len;
	while(src < wse) {
		const WC wc = *src;
		int len = utf8_encode_char (wc, p);
		if (len == 0) { *p = '?'; len = 1; }
		p += len;
		++src;
	}
	return (int)(p-dst);
}

template<class WC>
int	utf8_decode_string (const char *src, size_t src_len, WC *dst)
{
	WC *p = dst;
	const char *s = src;
	const char *se = s+src_len;
	while (s < se) {
		WC wc = *p;
		int len = utf8_decode_char (s, &wc);
		if (len == 0) {
			wc = '?';
			// skip UTF8 continuation characters
			for (len=1; s+len < se && (s[len] & 0xC0) == 0x80; ++len);
		}
		*p++ = (WC)wc;
		s += len;
	}
	return (int)(p-dst);
}

#endif	// utf8_h_INCLUDED

