// sig_parser.cpp
//
//

#include <klava/klav_sig.h>
#include <klava/klavstl/buffer.h>
#include <utf8.h>

#include "cp1251.h"

struct KLAV_Sig_Text_Parser
{
	KLAV_Bin_Writer * m_writer;
	size_t       m_sig_bytes;
	size_t       m_total_size;
	unsigned int m_flags;
	unsigned int m_error;  // nonzero - error condition
	const char * m_errmsg;

	unsigned int m_last_elt;
	klavstl::buffer <klav_allocator> m_tmpbuf;

	KLAV_Sig_Text_Parser (hKLAV_Alloc allocator, hKLAV_Bin_Writer writer)
		: m_writer (writer),
		  m_sig_bytes (0),
		  m_total_size (0),
		  m_flags (0),
		  m_error (0),
		  m_errmsg (0),
		  m_last_elt (KLAV_SIG_ELT_INVALID),
		  m_tmpbuf (allocator)
	{
	}

	void init (unsigned int flags);

	bool parse (const char *p, size_t len, unsigned int flags);

	bool sig_data_empty () const
		{ return m_total_size == 0; }

	const char * errmsg () const
		{ return m_errmsg; }

	bool output_elt (int elt_type, uint8_t b1 = 0, uint8_t b2 = 0);
	bool flush_last_elt (bool last);
	
	bool push_byte (uint8_t byte)
		{ return push_bytes (& byte, 1); }

	bool push_bytes (const void *data, size_t size);
};

////////////////////////////////////////////////////////////////

struct KLAV_Sig_Text_Scanner
{
	KLAV_Sig_Text_Parser& m_parser;
	const char * m_pp;
	const char * m_pe;

	KLAV_Sig_Text_Scanner (KLAV_Sig_Text_Parser& parser)
		: m_parser (parser), m_pp (0), m_pe (0) {}

	bool parse (const char *p, size_t len);

	void skip_blanks ()
		{ for (; m_pp < m_pe; ++m_pp)
		  {
		    uint8_t ch = *m_pp;
		    if (! (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
			  break;
		  }
		}

	bool scan_string (bool case_insensitive);
	bool scan_hex_bytes ();
	bool scan_special ();

	bool allow_incomplete () const
		{ return ((m_parser.m_flags & KLAV_SIG_PARSE_ALLOW_INCOMPLETE) != 0); }

	inline bool eof () const
		{ return m_pp == m_pe; }

	inline uint8_t getchar () const
		{ return *m_pp; }

	inline void next ()
		{ ++m_pp; }

	void set_errmsg (const char *s)
		{ if (m_parser.m_errmsg == 0) m_parser.m_errmsg = s; }
};

////////////////////////////////////////////////////////////////

static inline int h2i (uint8_t ch)
{
	if (ch >= '0' && ch <= '9') return ch - '0';
	ch |= 0x20;
	if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	return -1;
}

static inline bool ANY_HB (char c)
{
	return c == '?' || c == '.';
}

void KLAV_Sig_Text_Parser::init (unsigned int flags)
{
	m_flags = flags;
	m_errmsg = 0;
	m_last_elt = KLAV_SIG_ELT_INVALID;
	m_tmpbuf.clear ();
	m_sig_bytes = 0;
	m_total_size = 0;
}

bool KLAV_Sig_Text_Parser::parse (const char *p, size_t len, unsigned int flags)
{
	init (flags);

	KLAV_Sig_Text_Scanner scanner (*this);

	if (! scanner.parse (p, len))
		return false;

	return flush_last_elt (true);
}

bool KLAV_Sig_Text_Parser::output_elt (int elt_type, uint8_t b1, uint8_t b2)
{
	if (elt_type != m_last_elt)
	{
		if (! flush_last_elt (false))
			return false;
	}

	switch (elt_type)
	{
	case KLAV_SIG_ELT_BYTES:
	case KLAV_SIG_ELT_IBYTES:
	case KLAV_SIG_ELT_ANYBYTES:
	case KLAV_SIG_ELT_STAR:
		m_tmpbuf.push_back (b1);
		m_sig_bytes += 1;
		break;
	case KLAV_SIG_ELT_BITMASK:
	case KLAV_SIG_ELT_RANGE:
		if (! push_byte ((uint8_t)elt_type)
		 || ! push_byte (b1)
		 || ! push_byte (b2))
			return false;
		m_sig_bytes += 1;
		break;
	default:
		break;
	}

	if (m_sig_bytes > KLAV_SIG_MAX_BYTES)
	{
		m_errmsg = "signature size too large";
		return false;
	}

	m_last_elt = elt_type;
	return true;
}

bool KLAV_Sig_Text_Parser::flush_last_elt (bool last)
{
	bool write_body = true;

	switch (m_last_elt)
	{
	case KLAV_SIG_ELT_ANYBYTES:
	case KLAV_SIG_ELT_STAR:
		if (last || m_total_size == 0)  // ignore leading/trailing STAR elements
		{
			m_errmsg = "signature cannot start or end with wildcard character";
			return false;
		}
		write_body = false;
		// fallback
	case KLAV_SIG_ELT_BYTES:
	case KLAV_SIG_ELT_IBYTES:
		{
			unsigned int bufsz = (unsigned int)m_tmpbuf.size ();
			if (bufsz != 0)
			{
				uint8_t utf8_buf [8];
				unsigned int nb = utf8_encode_char (bufsz, (char *)utf8_buf);
				if (! push_byte ((uint8_t)m_last_elt)
				 || ! push_bytes (utf8_buf, nb))
					return false;
				if (write_body)
				{
					if (! push_bytes (& m_tmpbuf [0], m_tmpbuf.size ()))
						return false;
				}
			}
		}
		break;
	default:
		break;
	}

	m_last_elt = KLAV_SIG_ELT_INVALID;
	m_tmpbuf.clear ();

	return true;
}

bool KLAV_Sig_Text_Parser::push_bytes (const void *data, size_t size)
{
	if (m_writer != 0)
	{
		if (! m_writer->write (data, (uint32_t) size))
		{
			m_errmsg = "error writing output data (not enough memory?)";
			return false;
		}
	}
	m_total_size += size;
	return true;
}

////////////////////////////////////////////////////////////////

bool KLAV_Sig_Text_Scanner::parse (const char *p, size_t len)
{
	m_pp = p;
	m_pe = m_pp + len;

	const char *sog = 0; // start-of-group
	const char *eog = 0; // end-of-group
	int in_group_type = 0;  // 0, '{', '('

	for (;;)
	{
		skip_blanks ();
		if (eof ()) break;

		// check for repeat instruction
		if (m_pe - m_pp > 1 && m_pp[0] == '[' && m_pp[1] >= '0' && m_pp[1] <= '9')
		{
			// parse repeat instruction
			unsigned int repcnt = 0;

			if (sog == 0)
			{
				set_errmsg ("invalid repeat instruction");
				return false;
			}

			if (in_group_type != 0)
			{
				set_errmsg ("cannot use repeat instructions inside groups");
				return false;
			}

			// eoe is nonzero if there was some group defined before
			if (eog == 0)
				eog = m_pp;
			++m_pp;

			for (;;)
			{
				if (m_pp == m_pe)
				{
					if (allow_incomplete ())
					{
						break;
					}
					else
					{
						set_errmsg ("invalid repeat instruction");
						return false;
					}
				}

				uint8_t c = *m_pp++;
				if (c == ']') break;

				if (c >= '0' && c <= '9')
				{
					repcnt = repcnt * 10 + (c - '0');
				}
				else
				{
					set_errmsg ("invalid repeat instruction");
					return false;
				}
			}

			// implement repeat instruction
			if (repcnt == 0)
			{
				set_errmsg ("invalid repeat count");
				return false;
			}

			--repcnt; // first pass was already performed
			for (unsigned int i = 0; i < repcnt; ++i)
			{
				KLAV_Sig_Text_Scanner sub_scanner (m_parser);
				if (! sub_scanner.parse (sog, (unsigned int)(eog - sog)))
					return false;
			}

			sog = 0;
			eog = 0;

			continue;
		}

		if (in_group_type == 0)
		{
			sog = m_pp;
			eog = 0;
		}

		uint8_t ch = getchar ();

		if (ch == '(')
		{
			if (in_group_type != 0)
			{
				set_errmsg ("cannot nest groups");
				return false;
			}
			sog = m_pp++;
			in_group_type = ch;
		}
		else if (ch == ')')
		{
			if (in_group_type != '(')
			{
				set_errmsg ("missing starting group character");
				return false;
			}
			eog = m_pp++;
			in_group_type = 0;
		}
		else if (ch == 'i')
		{
			next ();
			if (eof () || getchar () != '\'')
			{
				set_errmsg ("string literal expected");
				return false;
			}
			if (! scan_string (true))
			{
				set_errmsg ("invalid string literal");
				return false;
			}
		}
		else if (ch == '\'')
		{
			if (! scan_string ((m_parser.m_flags & KLAV_SIG_PARSE_CASE_INSENSITIVE) != 0 ? true : false))
			{
				set_errmsg ("invalid string literal");
				return false;
			}
		}
		else if (ch == '[')
		{
			if (! scan_special ())
			{
				set_errmsg ("invalid byte operation specification");
				return false;
			}
		}
		else if (ch == '*')
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_STAR))
				return false;
			next ();
		}
		else if (ANY_HB(ch) || h2i (ch) >= 0)
		{
			if (! scan_hex_bytes ())
			{
				set_errmsg ("invalid hexadecimal characters");
				return false;
			}
		}
		else
		{
			set_errmsg ("invalid character");
			return false;
		}
	}

	return true;
}

bool KLAV_Sig_Text_Scanner::scan_string (bool case_insensitive)
{
	// we are positioned at separator character
	uint8_t term = *m_pp++;

	while (m_pp < m_pe)
	{
		uint32_t uc = 0;
		int uclen = utf8_decode_char (m_pp, &uc);
		if (uclen == 0 || m_pe - m_pp < uclen)
		{
			set_errmsg ("invalid UTF-8 character");
			return false;
		}

		m_pp += uclen;	

		if (uc == term)
			return true;

		if (uc == '\\')
		{
			if (m_pp == m_pe)
				break;
			uc = *m_pp++;

			switch (uc)
			{
			case 't':
				uc = '\t';
				break;
			case 'r':
				uc = '\r';
				break;
			case 'n':
				uc = '\n';
				break;
			case '0':
				uc = '\0';
				break;
			case '\'':
			case '\"':
			case '\\':
			case '*':
			case '?':
				break;
			case 'x':
			case 'X':
				{
					int hh = -1;
					int hl = -1;
					if (m_pe - m_pp >= 2)
					{
						hh = h2i (m_pp[0]);
						hl = h2i (m_pp[1]);
						m_pp += 2;
					}
					if (hl < 0 || hh < 0)
					{
						set_errmsg ("invalid HEX character escape");
						return false;
					}
					uc = (uint8_t)((hh << 4) | hl);
				} break;
			default:
				set_errmsg ("invalid character escape");
				return false;
			}
		}
		else if (uc == '*')
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_STAR))
				return false;
			continue;
		}
		else if (uc == '?')
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_ANYBYTES))
				return false;
			continue;
		}

		if (uc <= 0x7F)
		{
			// ASCII
			if (case_insensitive && ((uc >= 'A' && uc <= 'Z') || (uc >= 'a' && uc <= 'z')))
			{
				uc |= 0x20;
				if (! m_parser.output_elt (KLAV_SIG_ELT_IBYTES, uc))
					return false;
			}
			else
			{
				if (! m_parser.output_elt (KLAV_SIG_ELT_BYTES, uc))
					return false;
			}
		}
		else
		{
			unsigned int enc = (m_parser.m_flags & KLAV_SIG_PARSE_ENCODING_MASK);
			if (enc == KLAV_SIG_PARSE_ENCODING_UTF8)
			{
				char ucc [10];
				int len = utf8_encode_char (uc, ucc);
				for (int i = 0; i < len; ++i)
				{
					if (! m_parser.output_elt (KLAV_SIG_ELT_BYTES, (unsigned char) ucc [i]))
						return false;
				}
			}
			else if (enc == KLAV_SIG_PARSE_ENCODING_CP1251)
			{
				unsigned int c = cp1251_rmap (uc);
				if (c == 0)
				{
					set_errmsg ("invalid character for CP-1251 encoding");
					return false;
				}
				if (case_insensitive && c >= 0xC0)
				{
					c |= 0x20;
					if (! m_parser.output_elt (KLAV_SIG_ELT_IBYTES, c))
						return false;
				}
				else
				{
					if (! m_parser.output_elt (KLAV_SIG_ELT_BYTES, c))
						return false;
				}
			}
			else
			{
				set_errmsg ("non-ASCII character, encoding not specified");
				return false;
			}
		}
	}

	if (! allow_incomplete ())
	{
		set_errmsg ("non-terminated string");
		return false;
	}
	return true;
}

bool KLAV_Sig_Text_Scanner::scan_hex_bytes ()
{
	for (;;)
	{
		skip_blanks ();
		if (m_pp == m_pe)
			return true;

		int ch1 = *m_pp;
		int hh = h2i (ch1);
		if (! (ANY_HB(ch1) || hh >= 0))
			break;

		m_pp++;

		int ch2 = 0;
		int hl = -1;

		if (m_pp < m_pe)
		{
			ch2 = *m_pp++;
			hl = h2i (ch2);
		}
		else if (allow_incomplete ())
		{
			ch2 = '0';
			hl = 0;
		}

		if (! (ANY_HB(ch2) || hl >= 0))
		{
			set_errmsg ("invalid hex byte");
			return false;
		}

		if (ANY_HB(ch1) && ANY_HB(ch2))	// ??
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_ANYBYTES))
				return false;
		}
		else if (ANY_HB(ch1))	// ?X
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_BITMASK, 0x0F, (uint8_t)hl))
				return false;
		}
		else if (ANY_HB(ch2))	// X?
		{
			if (! m_parser.output_elt (KLAV_SIG_ELT_BITMASK, 0xF0, (uint8_t)(hh << 4)))
				return false;
		}
		else
		{
			uint8_t b = (uint8_t)((hh << 4) | hl);
			if (! m_parser.output_elt (KLAV_SIG_ELT_BYTES, b))
				return false;
		}
	}
	return true;
}

bool KLAV_Sig_Text_Scanner::scan_special ()
{
	// we are standing on '['
	m_pp++;
	if (eof ()) return false;
	uint8_t op = *m_pp++;

	if (! (op == '&' || op == '|' || op == 'R' || op == 'r'))
	{
		set_errmsg ("invalid byte operation code");
		return false;
	}

	// all operations share form: [oAAsBB]
	if (m_pe - m_pp < 6 || m_pp[5] != ']')
	{
		if (allow_incomplete ())
		{
			return true;
		}
		set_errmsg ("invalid byte operation format");
		return false;
	}

	int ah = h2i (m_pp[0]);
	int al = h2i (m_pp[1]);

	uint8_t delim = m_pp[2];

	uint8_t need_delim = '=';
	if (op == 'R' || op == 'r') need_delim = '-';

	int bh = h2i (m_pp[3]);
	int bl = h2i (m_pp[4]);

	if (ah < 0 || al < 0 || bh < 0 || bl < 0 || delim != need_delim)
	{
		set_errmsg ("invalid byte operation format");
		return false;
	}

	uint8_t a = (uint8_t)((ah << 4) | al);
	uint8_t b = (uint8_t)((bh << 4) | bl);

	unsigned int elt = KLAV_SIG_ELT_INVALID;

	if (op == '&')
	{
		if ((a & b) != b)
		{
			set_errmsg ("invalid mask operation - no byte can match");
			return false;
		}

		if (a == 0)
		{
			elt = KLAV_SIG_ELT_ANYBYTES;
		}
		else if (a == 0xFF)
		{
			elt = KLAV_SIG_ELT_BYTES;
			a = b;
		}
		else
		{
			elt = KLAV_SIG_ELT_BITMASK;
		}

		elt = (a == 0) ? KLAV_SIG_ELT_ANYBYTES : KLAV_SIG_ELT_BITMASK;
	}
	else if (op == '|')
	{
		if ((a & b) != a)
		{
			set_errmsg ("invalid mask operation - no byte can match");
			return false;
		}
		a = ~a;
		b &= a;

		if (a == 0)
		{
			elt = KLAV_SIG_ELT_ANYBYTES;
		}
		else if (a == 0xFF)
		{
			elt = KLAV_SIG_ELT_BYTES;
			a = b;
		}
		else
		{
			elt = KLAV_SIG_ELT_BITMASK;
		}
	}
	else if (op == 'r' || op == 'R')
	{
		if (a > b)
		{
			set_errmsg ("invalid range bounds - no byte can match");
			return false;
		}
		bool f = false;

		if (a == b)
		{
			elt = KLAV_SIG_ELT_BYTES;
		}
		else if (a == 0 && b == 0xFF)
		{
			elt = KLAV_SIG_ELT_ANYBYTES;
		}
		else
		{
			elt = KLAV_SIG_ELT_RANGE;
		}
	}

	if (! m_parser.output_elt (elt, a, b))
		return false;

	m_pp += 6;
	return true;
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
klav_bool_t KLAV_CALL KLAV_Sig_Parse (
			hKLAV_Alloc allocator, // allocator for internal allocations
			const char *text,      // signature text
			size_t text_len,       // length of signature text
			unsigned int flags,    // parse flags (KLAV_SIG_PARSE_XXX)
			hKLAV_Bin_Writer output,    // output buffer writer
			const char **errmsg
		)
{
	if (errmsg != 0)
		*errmsg = 0;

	if ((flags & KLAV_SIG_PARSE_ENCODING_MASK) == KLAV_SIG_PARSE_ENCODING_DEFAULT)
		flags |= KLAV_SIG_PARSE_ENCODING_CP1251;

	KLAV_Sig_Text_Parser parser (allocator, output);

	if (! parser.parse (text, text_len, flags))
	{
		if (errmsg != 0)
			*errmsg = parser.errmsg ();
		return false;
	}

	if ((flags & KLAV_SIG_PARSE_ALLOW_EMPTY) == 0)
	{
		if (parser.sig_data_empty ())
		{
			if (errmsg != 0)
				*errmsg = "signature is empty";
			return false;
		}
	}

	return true;
}

