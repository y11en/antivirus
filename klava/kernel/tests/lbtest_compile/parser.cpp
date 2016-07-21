// parser.cpp
//

#include "precompile.h"
#include "parser.h"
#include "compiler.h"
#include "lbt_keywords.cpp"

extern int lbt_debug;

void LbtScanner::skip_white()
{
	while (pp < pe)
	{
		int c = *pp;
		if (c == ' ' || c == '\t' || c == '\r')
		{
			++pp;
			continue;
		}
		if (c == '\n')
		{
			++pp;
			++m_line_no;
			continue;
		}
		break;
	}
}

void LbtScanner::skip_inline_comment()
{
	while (pp < pe)
	{
		int c = *pp++;
		if (c == '\n')
		{
			++m_line_no;
			break;
		}
	}
}

void LbtScanner::skip_block_comment()
{
	while (pp < pe)
	{
		int c = *pp++;
		if (c == '*' && pp < pe && *pp == '/')
		{
			++pp;
			break;
		}
		if (c == '\n')
			++m_line_no;
	}
}

int LbtScanner::scan_string()
{
	pt = pp;
	int term_ch = *pp++;

	while (pp < pe)
	{
		int c = *pp++;
		if (c == term_ch)
		{
			pte = pp;
			return TOK_STRING;
		}
		if (c == '\\')
		{
			if (pp == pe)
				break;
			++pp;
		}
		if (c < 0x20)
		{
			// invalid symbol
			return -2;
		}
	}

	return -2;	// invalid string
}

int	LbtScanner::scan_number()
{
	uint64_t val = 0;

	pt = pp;

	bool negative = false;
	if (pp[0] == '-')
	{
		negative = true;
		++pp;
	}

	char const* pt1 = pp;

	if (pp[0] == '0' && (pe - pp) >= 2 && (pp[1] == 'x' || pp[1] == 'X'))
	{
		// hex number
		for (pp += 2; pp < pe; ++pp) {
			int c = *pp;

			uint64_t v1 = val << 4;
			if (v1 >> 4 != val)
				return -3;
			if (c >= '0' && c <= '9')
				val = v1 | (c - '0');
			else if (c >= 'A' && c <= 'F')
				val = v1 | (c - 'A' + 10);
			else if (c >= 'a' && c <= 'f')
				val = v1 | (c - 'a' + 10);
			else
				break;
		}
		if (pp - pt1 < 3)
			return -1;
	} else
	{
		// decimal number
		for (; pp < pe; ++pp)
		{
			int c = *pp;
			uint64_t v1 = val * 10;
			if (c >= '0' && c <= '9')
				val = v1 + (c - '0');
			else
				break;
		}
		if (pp - pt1 < 1)
			return -1;
	}
	pte = pp;

	m_intValue = negative ? -int64_t(val) : int64_t(val);
	return TOK_NUM;
}

int	LbtScanner::scan_ident()
{
	for (; pp < pe; ++pp)
	{
		int c = *pp;
		if (!(c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))) break;
	}
	pte = pp;
	return TOK_IDENT;
}

void LbtScanner::skip_blanks ()
{
	while (true)
	{
		skip_white();
		pt = pp;
		if (pp == pe || *pp != '/') break;
		if ((pp+1) == pe) break;

		const int c2 = pp[1];
		if( c2=='/' ) { pp += 2; skip_inline_comment(); continue; }
		if( c2=='*' ) { pp += 2; skip_block_comment(); continue; }
		break;
	}
}

int	LbtScanner::scan_token()
{
	while (true)
	{
		skip_blanks();
		pt = pp;
		if (pp == pe) return 0;

		int c = *pp;
		int c2 = (pp+1 < pe) ? pp[1] : -1;

		if (c == '\"')
			return scan_string();

		// try to scan number...
		if (c >= '0' && c <= '9' || c == '-')
			return scan_number();

		// scan keyword
		if (c=='_' || (c>='A' && c<='Z') || (c>='a' && c<='z')) {
			pt = pp;
			int tok = scan_ident();
			if (tok != TOK_IDENT) return tok;

			LbtKeyword const* pk = LbtKeywordHash::in_word_set ((const char *) pt, (unsigned int) (pte - pt));
			return (pk == NULL) ? TOK_IDENT : pk->token;
		}

		// return delimiters
		++pp;
		return c;
	}
}

bool LbtParser::parse(const char *text, size_t textLen)
{
	m_scanner.reset(text, textLen);
	clearError();

	if (lbt_parse(this) != 0)
		setError("Syntax error");

	if (m_errorMsg)
		return false;

	return true;
}

int	LbtParser::yylex(YYSTYPE* plval, YYLTYPE* lineNo)
{
	m_scanner.skip_blanks();
	*lineNo = m_scanner.line_no();

	*plval = 0;

	int tok = m_scanner.scan_token();

	switch(tok)
	{
	case TOK_NUM:
		*plval = m_scanner.intValue();
		break;
	case TOK_STRING:
		*plval = m_compiler.getStringLiteralIdx(m_scanner.token_text() + 1, m_scanner.token_len() - 2);
		break;
	case TOK_IDENT:
		*plval = m_compiler.getIdentIdx(m_scanner.token_text(), m_scanner.token_len());
		break;
	case -3:
		setError("Numeric overflow");
		return 0;
	case -2:
		setError("Bad string");
		return 0;
	case -1:
		setError("Lexic error");
		return 0;
	}

	return tok;
}

////////////////////////////////////////////////////////////////
// bison interface

#ifdef _MSC_VER
# pragma warning(disable:4065) // switch statement contains 'default' but no 'case' labels
# pragma warning(disable:4060) // switch statement contains no 'case' or 'default' labels
#endif

#define YYPARSE_PARAM parser
#define lbt_lex   ((LbtParser*)parser)->yylex
#define lbt_error ((LbtParser*)parser)->yyerror

// force stack allocation using alloca() to make code exception-safe
#define YYSTACK_USE_ALLOCA 1

// set YYMAXDEPTH to a small value to guard against stack overflow when using alloca()
#define YYMAXDEPTH  5000

/* Bison manual states:

Because of semantical differences between C and C++,
the LALR(1) parsers in C produced by Bison by compiled as C++ cannot grow.
In this precise case (compiling a C parser as C++) you are suggested to grow YYINITDEPTH.

So, define YYINITDEPTH equal to YYMAXDEPTH
*/
#define YYINITDEPTH YYMAXDEPTH

#include "lbt_grammar.c"
