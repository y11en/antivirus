// parser.h
//

#ifndef parser_h_INCLUDED
#define parser_h_INCLUDED

#define YYSTYPE int64_t
#define YYLTYPE unsigned

# define YYLLOC_DEFAULT(Current, Rhs, N) \
	do \
	{ \
		(Current) = ((Rhs)[N]); \
	} while (0)

#include "lbt_grammar.h"

class LbtCompiler;

////////////////////////////////////////////////////////////////
// scanner

class LbtScanner
{
public:
	LbtScanner()
		{ reset(0, 0); }

	void reset(const char *text, size_t len)
		{ pp = pb = text; pe = pb + len; pt = pte = 0; m_line_no = 1; }

	void skip_blanks();
	int  scan_token();

	unsigned line_no() const
		{ return m_line_no; }

	char const* token_text() const
		{ return pt; }

	size_t token_len() const
		{ return pte - pt; }

	YYSTYPE intValue() const
		{ return m_intValue; }

private:
	char const* pb;
	char const* pp;
	char const* pe;

	char const* pt;
	char const* pte;

	YYSTYPE m_intValue;
	unsigned m_line_no;

	void skip_white();
	void skip_inline_comment();
	void skip_block_comment();
	int scan_string();
	int scan_number();
	int scan_ident();
};

////////////////////////////////////////////////////////////////
// parser

class LbtParser
{
public:
	LbtParser(LbtCompiler& compiler) :
		m_compiler(compiler)
		{}

	bool parse(const char *text, size_t textLen);

	LbtCompiler& compiler() const
		{ return m_compiler; }

	char const* errorMsg()
		{ return m_errorMsg; }
	unsigned lastLineNo()
		{ return m_scanner.line_no(); }

private:
	LbtCompiler& m_compiler;
	LbtScanner m_scanner;

	char const* m_errorMsg;

	friend int lbt_parse(void *parser);

	int yylex(YYSTYPE* plval, YYLTYPE* lineNo);
	void yyerror(const char *msg)
		{ setError(msg); }

	void clearError()
		{ m_errorMsg = 0; }
	void setError(const char* errorMsg)
		{ m_errorMsg = errorMsg; }
};

#endif // parser_h_INCLUDED
