// kdb_utils.c
//

#define KDB_UTILS_IMPL
#include <klava/kdb_utils.h>

////////////////////////////////////////////////////////////////
// KDBID formatting

int KDBID_STRING_CHAR (unsigned int ch)
{
	return ((ch >= '0' && ch <= '9')
	 || (ch >= 'A' && ch <= 'Z')
	 || (ch >= 'a' && ch <= 'z'))
	 || ch == '@'
	 || ch == '!'
	 ? 1 : 0;
}

int KDBID_PARSE_STRING_QUAD (const char *s, uint32_t *pval)
{
	uint32_t v = 0;
	int i;
	*pval = 0;

	for (i=0; i<4; ++i)
	{
		if (s[i] == 0)
			break;

		if (s[i] == '_')
		{
			v = v << 8;
		}
		else
		{
			if (! KDBID_STRING_CHAR(s[i])) return 0;
			v = (v << 8) | s[i];
		}
	}

	if (i == 0)
		return 0; // empty string

	for (; i < 4; ++i)
	{
		v = v << 8;
	}

	*pval = v;
	return 1;
}

size_t KDBID_STRING_FORMAT (uint32_t id, char *buf)
{
	int i;
	if (id != 0) {
		char * s = buf;
		unsigned int b[4];
		b[0] = (id >> 24) & 0xFF;
		b[1] = (id >> 16) & 0xFF;
		b[2] = (id >> 8) & 0xFF;
		b[3] = (id) & 0xFF;

		for (i=0; i<4; ++i) {
			if (b[i] == 0) {
				*s++ = '_';
			} else {
				if (! KDBID_STRING_CHAR (b[i])) break;
				*s++ = (char)b[i];
			}
		}
		for (; i<4; ++i) {
			if (b[i] != 0) break;
		}
		if (i == 4) { *s = 0; return s - buf; }
	}
//	sprintf (nbuf, "0x%08X", n);
	buf[0] = '0';
	buf[1] = 'x';
	for (i=0; i<8; ++i) {
		unsigned int n = (id & 0x0F);
		buf[9-i] = (n < 10) ? n + '0' : n + 'A' - 10;
		id >>= 4;
	}
	buf[10] = 0;
	return 10;
}

////////////////////////////////////////////////////////////////
// KDB filename formatting

size_t KDBFormatID (KDBID id, char *buf, int pad)
{
	int i;
	char *s = buf;
	char *last = 0;
	unsigned int b[4];

	if (id == 0) return 0;

	b[0] = (id >> 24) & 0xFF;
	b[1] = (id >> 16) & 0xFF;
	b[2] = (id >> 8) & 0xFF;
	b[3] = (id) & 0xFF;

	for (i=0; i<4; ++i)
	{
		if (b[i] == 0)
		{
			if (! last) last = s;
			*s++ = '_';
		}
		else
		{
			unsigned int c = b[i];
			if (! KDBID_STRING_CHAR (c)) return 0;
			if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
			last = 0;
			*s++ = c;
		}
	}
	if (! pad && last != 0) s = last;
	*s = 0;
	
	return s - buf;
}

size_t KDBFormatFilename (KDBID sectionType, KDBID fragmentID, char *buf)
{
	size_t n;
	char *s = buf;

	n = KDBFormatID (sectionType, s, fragmentID != 0);
	if (n == 0) return 0;
	s += n;

	if (fragmentID != 0)
	{
		n = KDBFormatID (fragmentID, s, 0);
		if (n == 0) return 0;
		s += n;
	}
	return s - buf;
}


////////////////////////////////////////////////////////////////
// KDBParser

KDBToken KDBParser_GetToken (KDBParser *parser)
{
	for (;parser->pp < parser->pe; ++(parser->pp))
	{
		int c = *(parser->pp);
		if (c == '\n') break;
		if (! (c == ' ' || c == '\t' || c == '\r')) {
			if (c == '\"') {
				parser->pt = ++(parser->pp);
				for (; parser->pp < parser->pe; (parser->pp)++) {
					int c2 = *(parser->pp);
					if (c2 == '\n' || c2 == '\r') break;
					if (c2 == '\"') {
						parser->pte = (parser->pp)++;
						return KDB_STR;
					}
				}
				return KDB_ERROR;
			} else if (c == '$') {
				parser->pt = ++(parser->pp);
				for (; parser->pp < parser->pe; (parser->pp)++) {
					int c2 = *(parser->pp);
					if (c2 == ' ' || c2 == '\t' || c2 == '\n' || c2 == '\r') break;
					if (! ((c2 >= '0' && c2 <= '9')
					    || (c2 >= 'A' && c2 <= 'F')
					    || (c2 >= 'a' && c2 <= 'f'))) return KDB_ERROR;
				}
				parser->pte = parser->pp;
				if ((parser->pte - parser->pt) & 0x1) return KDB_ERROR; // odd number of hex
				return KDB_BIN;
			} else {
				const char *p = parser->pp;
				parser->pt = p;
				parser->ival = 0;
				if (p[0] == '0' && (parser->pe - p > 2) 
					&& (p[1] == 'x' || p[1] == 'X')) {
					// hex
					p = p + 2;;
					for (;p < parser->pe; ++p) {
						int n = 0;
						if (*p >= '0' && *p <= '9') n = *p - '0';
						else if (*p >= 'A' && *p <= 'F') n = *p - 'A' + 10;
						else if (*p >= 'a' && *p <= 'f') n = *p - 'a' + 10;
						else if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') break;
						else return KDB_ERROR;
						parser->ival = (parser->ival << 4) | n;
					}
					parser->pp = p;
				} else if (p[0] >= '0' && p[0] <= '9') {
					// dec
					parser->ival = (*p - '0');
					for (++p; p < parser->pe; ++p) {
						int n = 0;
						if (*p >= '0' && *p <= '9') n = *p - '0';
						else if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') break;
						else return KDB_ERROR;
						parser->ival = (parser->ival * 10) + n;
					}
					parser->pp = p;
					parser->pte = p;
				} else {
					return KDB_ERROR;
				}
				return KDB_INT;
			}
		}
	}
	return KDB_EOL;
}
