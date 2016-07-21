// SerializableView.cpp: implementation of the CSerializableView class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/Interpret.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// operators

void cOperatorUnary::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	m_args[0]->Exec(result, ctx);
	switch(m_type)
	{
	case ntNeg:      result = -result.ToInt(); break;
	case ntBitNeg:   result = ~result.ToQWORD(); break;
	case ntLogicNeg: result = !result; break;
	}
}

void cOperatorBinary::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	if( m_type == ntEQ )
	{
		m_args[1]->Exec(result, ctx);
		m_args[0]->Exec(result, ctx, false);
		return;
	}
	
	cVariant op;
	m_args[1]->Exec(op, ctx);
	m_args[0]->Exec(result, ctx);
	
	switch(m_type)
	{
	case ntAdd: result += op; break;
	case ntSub: result -= op; break;
	case ntMul: result *= op; break;
	case ntDiv: result /= op; break;
	case ntRem: result %= op; break;
	case ntCmpEQ: result = result == op; break;
	case ntCmpNE: result = result != op; break;
	case ntCmpLT: result = result < op;  break;
	case ntCmpGT: result = result > op;  break;
	case ntCmpLE: result = result <= op; break;
	case ntCmpGE: result = result >= op; break;
	case ntBitAnd: result &= op; break;
	case ntBitXor: result ^= op; break;
	case ntBitOr:  result |= op; break;
	case ntShiftLeft: result <<= op; break;
	case ntShiftRight: result >>= op; break;
	case ntLogicAnd: result = result && op; break;
	case ntLogicOr:  result = result || op; break;
	}
}

void cOperatorTernary::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	switch(m_type)
	{
	case ntIF:
		cVariant cond;
		m_args[0]->Exec(cond, ctx);
		if( cond )
			m_args[1]->Exec(result, ctx);
		else
			if( m_args[2] )
				m_args[2]->Exec(result, ctx);
			else
				result.Clear();
		break;
	}
}

void cOperatorSequence::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	cVariant var;
	m_args[0]->Exec(var, ctx, get);
	m_args[1]->Exec(value, ctx, get);
}

void cLikeNode::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
}


////////////////////////////////////////////////////////////////
// cParser

#define YYSTYPE cNode *

#include "Grammar.h"

#include <stdexcept>

class cParser
{
public:
	typedef std::vector<cNode *> cNodePtrs;

public:
	cParser(cNode::cNodeFactory *nodeFactory = NULL, cAlloc *nodeAllocator = NULL);
	~cParser();

	void    parse(const char *text, size_t text_len);
	void    clear();
	cNode * detach();

protected:
	void    push(cNode *node);
	void    pop(cNode *node, bool destroy = false);

private:
	cNode * m_expression;

	// scanner variables
	const unsigned char * m_pp; // parse pointer
	const unsigned char * m_pe; // text end

	const unsigned char * m_ptb; // start-of-token
	const unsigned char * m_pte; // end-of-token

	int m_lineno;

	int64_t m_last_nval;

	////////////////////////////////////////////////////////
	// lexical scanner methods

	void reset_scanner();
	void init_scanner(const void *p, size_t n);
	int  scan_token();

	void skip_space();
	void skip_inline_comment();
	void skip_block_comment();
	void scan_quoted_string();
	void scan_number(int64_t *pval);
	void scan_ident();
	void scan_string_value(std::string& buf);

	int check_keyword(const char *s, int len);

	////////////////////////////////////////////////////////
	// bison callbacks

	void    make_expression(cNode *expr);
	cNode * make_sequence(cNode *left, cNode *right);
	
	cNode * make_unary_op(cNode::type_t type, cNode *a);
	cNode * make_binary_op(cNode::type_t type, cNode *a, cNode *b);
	cNode * make_ternary_op(cNode::type_t type, cNode *a, cNode *b, cNode *c);
	
	cNode * make_func_op(const char *funcName, cNode *a, cNode *b, cNode *c, cNode *d, cNode *e, cNode *f, cNode *g);
	cNode * make_func_op(cNode *funcName, cNode *a = NULL, cNode *b = NULL, cNode *c = NULL, cNode *d = NULL, cNode *e = NULL, cNode *f = NULL, cNode *g = NULL);
	cNode * make_index_op(cNode *container, cNode *index);
	cNode * make_var_op(cNode *container, cNode *varName);

	////////////////////////////////////////////////////////
	// bison hooks
	friend int my_parse(void *parser);

	int  yylex(cNode **plval);
	void yyerror(const char *msg);

	void raise_error(const char *msg, const char *ctx = 0);

private:
	cNode::cNodeFactory * m_nodeFactory;
	cAlloc              * m_nodeAllocator;
	cNodePtrs             m_nodes;
};


cNode *cNode::Parse(const char *expression, cNodeFactory *nodeFactory, cAlloc *nodeAllocator, tString *error)
{
	try
	{
		if( error )
			error->clear();

		if( expression && *expression )
		{
			cParser parser(nodeFactory, nodeAllocator);
			parser.parse(expression, strlen(expression));
			if( nodeFactory->FinalInit() )
				return parser.detach();
		}
	}
	catch(const std::exception& ex)
	{
		const char * strErr = ex.what();
		if( error )
			*error = strErr;

		nodeFactory->ParseError(expression, strErr, -1);
	}
	
	return NULL;
}

////////////////////////////////////////////////////////////////
// cParser implementation

bool katran_string_parse(const char *kt_str, unsigned int kt_str_len, std::string& str, const char **errmsg);

cParser::cParser(cNode::cNodeFactory *nodeFactory, cAlloc *nodeAllocator) :
	m_nodeFactory(nodeFactory),
	m_nodeAllocator(nodeAllocator),
	m_expression(NULL)
{
	m_nodes.reserve(16);
	reset_scanner();
}

cParser::~cParser ()
{
	clear();
}

void cParser::clear()
{
	m_expression = 0;

	for(cNodePtrs::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		(*i)->Destroy();
	m_nodes.clear();

	reset_scanner();
}

cNode * cParser::detach()
{
	cNode * result = m_expression;
	if( result )
		m_nodes.clear();
	clear();
	return result;
}

void cParser::parse(const char *text, size_t text_len)
{
	clear ();
	init_scanner (text, text_len);

	if (my_parse (this) != 0)
	{
		raise_error ("syntax error");
	}
}

int	cParser::yylex(cNode **plval)
{
	*plval = 0;

	int tok = scan_token();
	switch(tok)
	{
	case TOK_NUMBER:
		{
			cNodeInt * node = new cNodeInt();
			node->m_val = m_last_nval;
			*plval = node;
		}
		break;
	case TOK_IDENT:
		{
			cNodeStr * node = new cNodeIdent();
			scan_string_value(node->m_str);
			*plval = node;
		}
		break;
	case TOK_STRING:
		{
			cNodeStr * node = new cNodeStr();
			scan_string_value(node->m_str);
			*plval = node;
		}
		break;
	}

	push(*plval);
	return tok;
}

void cParser::yyerror(const char *msg)
{
	raise_error("syntax error");
}

void cParser::raise_error(const char *msg, const char *ctx)
{
	char nbuf[20] = {0};
	std::string text = "line ";
	sprintf ( nbuf, "%d", m_lineno );
	text += nbuf;

	text += ':';
	text += msg;
	if (ctx != 0)
	{
		text += " (";
		text += ctx;
		text += ")";
	}

	throw std::runtime_error (text);
}

void cParser::make_expression(cNode *expr)
{
	m_expression = expr;
}

cNode * cParser::make_sequence(cNode *left, cNode *right)
{
	pop(left);
	pop(right);
	cNode *args[] = { left, right, NULL };
	cNode *seq = new cOperatorSequence(args);
	push(seq);
	return seq;
}

cNode * cParser::make_unary_op(cNode::type_t type, cNode *a)
{
	cOperatorUnary *node = new cOperatorUnary(type);
	node->m_args[0] = a;
	
	pop(a);
	push(node);
	return node;
}

cNode * cParser::make_binary_op(cNode::type_t type, cNode *a, cNode *b)
{
	cOperatorBinary *node = new cOperatorBinary(type);
	node->m_args[0] = a;
	node->m_args[1] = b;
	
	pop(a);
	pop(b);
	push(node);
	return node;
}

cNode * cParser::make_ternary_op(cNode::type_t type, cNode *a, cNode *b, cNode *c)
{
	cOperatorTernary *node = new cOperatorTernary(type);
	node->m_args[0] = a;
	node->m_args[1] = b;
	node->m_args[2] = c;
	
	pop(a);
	pop(b);
	pop(c);
	push(node);
	return node;
}

cNode * cParser::make_func_op(const char *funcName, cNode *a, cNode *b, cNode *c, cNode *d, cNode *e, cNode *f, cNode *g)
{
	cNode *args[NODE_MAX_ARGS + 1] = {a, b, c, d, e, f, g, NULL, };
	cNode *args_node[countof(args)]; memcpy(args_node, args, sizeof(args));
	cNode *op = m_nodeFactory->CreateOperator(funcName, args_node, m_nodeAllocator);

	for(size_t i = 0; i < countof(args); i++)
		if( cNode *node = args[i] )
			pop(node, !!args_node[i]);

	push(op);
	
	if( !op )
		raise_error("unknown function", funcName);
	return op;
}

cNode * cParser::make_func_op(cNode *funcName, cNode *a, cNode *b, cNode *c, cNode *d, cNode *e, cNode *f, cNode *g)
{
	cNode *op = make_func_op(((cNodeStr *)funcName)->m_str.c_str(), a, b, c, d, e, f, g);
	pop(funcName, true);
	return op;
}

cNode * cParser::make_index_op(cNode *container, cNode *index)
{
	return make_func_op("internalIndexOperator", container, index, NULL, NULL, NULL, NULL, NULL);
}

cNode * cParser::make_var_op(cNode *container, cNode *varName)
{
	const tString& name = ((cNodeStr *)varName)->m_str;
	
	cNode *op = m_nodeFactory->CreateOperand(name.c_str(), container, m_nodeAllocator);
	
	if( !op )
		raise_error("unknown operand", name.c_str());

	pop(varName, true);

	if( container )
	{
		if( container == op )
			return container;
		pop(container, !op);
	}
	
	push(op);
	return op;
}

void cParser::push(cNode *node)
{
	if( node )
		m_nodes.push_back(node);
}

void cParser::pop(cNode *node, bool destroy)
{
	if( node )
	{
		m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), node));
		if( destroy )
			node->Destroy();
	}
}


////////////////////////////////////////////////////////////////
// Scanner

void cParser::reset_scanner ()
{
	m_pp = 0;
	m_pe = 0;
	m_ptb = 0;
	m_pte = 0;
	m_lineno = 0;
	m_last_nval = 0;
}

void cParser::init_scanner (const void *p, size_t n)
{
	reset_scanner ();

	m_pp = (const unsigned char *)p;
	m_pe = m_pp + n;
	m_lineno = 1;
}

void cParser::skip_space ()
{
	while (m_pp < m_pe)
	{
		unsigned int c = *m_pp;
		if (c==' ' || c=='\t' || c=='\r') { ++m_pp; continue; }
		if (c=='\n') { ++m_pp; ++m_lineno; continue; }
		break;
	}
}

void cParser::skip_inline_comment ()
{
	while (m_pp < m_pe)
	{
		unsigned int c = *m_pp++;
		if (c == '\n') { ++m_lineno; break; }
	}
}

void cParser::skip_block_comment ()
{
	while (m_pp < m_pe)
	{
		unsigned int c = *m_pp++;
		if (c == '*' && m_pp < m_pe && *m_pp == '/') { ++m_pp; break; }
		if (c == '\n') { ++m_lineno; }
	}
}

void cParser::scan_quoted_string ()
{
	int term_ch = *m_pp++;
	m_ptb = m_pp;
	for (; m_pp < m_pe; ++m_pp)
	{
		int c = *m_pp;
		if (c == term_ch)
		{
			m_pte = m_pp++;
			return;
		}
		if (c == '\\')
		{
			if (++m_pp == m_pe) break;
		}
		if (c < 0x20 && c != '\r' && c != '\n' && c != '\t') break;
	}
	raise_error ("invalid string literal");
}

void cParser::scan_number (int64_t *pval)
{
	uint64_t val = 0;

	m_ptb = m_pp;
	if (m_pp[0] == '0'
		&& (m_pe - m_pp) >= 2 && (m_pp[1] == 'x' || m_pp[1] == 'X'))
	{
		// hex number

		for (m_pp += 2; m_pp < m_pe; ++m_pp)
		{
			unsigned int c = *m_pp;
			if (c >= '0' && c <= '9')
			{
				val = (val << 4) | (c - '0');
			}
			else if (c >= 'A' && c <= 'F')
			{
				val = (val << 4) | (c - 'A' + 10);
			}
			else if (c >= 'a' && c <= 'f')
			{
				val = (val << 4) | (c - 'a' + 10);
			}
			else break;
		}
		if (m_pp - m_ptb < 3)
		{
			raise_error ("invalid hexadecimal number");
		}
	}
	else
	{
		// decimal number
		for (; m_pp < m_pe; ++m_pp)
		{
			unsigned int c = *m_pp;
			if (c >= '0' && c <= '9')
			{
				val = val * 10 + (c - '0');
			}
			else
				break;
		}
	}
	m_pte = m_pp;

	if (pval != 0)
		*pval = val;
}

void cParser::scan_ident ()
{
	for (; m_pp < m_pe; ++m_pp)
	{
		unsigned int c = *m_pp;
		if (! (c=='@' || c=='_' || (c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9')) ) break;
	}
	m_pte = m_pp;
}

int cParser::check_keyword (const char *s, int len)
{
	if (len == 2 && strncmp (s, "if", 2) == 0)
		return TOK_IF;
	if (len == 3 && strncmp (s, "var", 3) == 0)
		return TOK_VAR;
	if (len == 4 && strncmp (s, "else", 4) == 0)
		return TOK_ELSE;
	return 0;
}

void cParser::scan_string_value (std::string& buf)
{
	buf.resize (0);

	const char *str = (const char *) m_ptb;
	unsigned int len = m_pte - m_ptb;

	const char *errmsg = 0;
	if (! katran_string_parse (str, len, buf, &errmsg))
	{
		raise_error ("error in string literal", errmsg);
	}
}

int cParser::scan_token ()
{
	for (;;)
	{
		skip_space ();

		m_ptb = m_pp;
		if (m_pp == m_pe) return 0;

		unsigned int c = *m_pp;
		unsigned int c2 = (m_pp + 1 < m_pe) ? m_pp[1] : -1;

		if (c == '/')
		{
			if (c2 == '/') { m_pp += 2; skip_inline_comment (); continue; }
			if (c2 == '*') { m_pp += 2; skip_block_comment (); continue; }
		}
		if (c == '\"')
		{
			scan_quoted_string ();
			return TOK_STRING;
		}

		// try to scan number...
		if (c >= '0' && c <= '9')
		{
			scan_number (&m_last_nval);
			return TOK_NUMBER;
		}

		// scan ident or keyword
		if (c == '@' || c == '_'
			|| (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z'))
		{
			m_ptb = m_pp;
			scan_ident ();

			int tok = check_keyword ((const char *)m_ptb, m_pte - m_ptb);
			if (tok == 0)
				return TOK_IDENT;

			return tok;
		}

		// return delimiters
		++m_pp;
		switch(c)
		{
		case '<':
			if (c2 == '=') { ++m_pp; return TOK_LE; }
			if (c2 == '<') { ++m_pp; return TOK_LSHIFT; }
			break;
		case '>':
			if (c2 == '=') { ++m_pp; return TOK_GE; }
			if (c2 == '>') { ++m_pp; return TOK_RSHIFT; }
			break;
		case '!':
			if (c2 == '=') { ++m_pp; return TOK_NE; }
			break;
		case '=':
			if (c2 == '=') { ++m_pp; return TOK_EQ; }
			break;
		case '&':
			if (c2 == '&') { ++m_pp; return TOK_LOGIC_AND; }
			break;
		case '|':
			if (c2 == '|') { ++m_pp; return TOK_LOGIC_OR; }
			break;
		}

		return c;
	}
}

//////////////////////////////////////////////////////////////////////////
// some parser stuff

#include "utf8.h"

static bool set_errmsg (const char *msg, const char **errmsg)
{
	if (errmsg != 0) *errmsg = msg;
	return false;
}

static bool invalid_utf8 (const char **errmsg)
{
	return set_errmsg ("invalid UTF-8 character", errmsg);
}

static bool invalid_escape (const char **errmsg)
{
	return set_errmsg ("invalid character escape", errmsg);
}

static bool append_unicode_char (std::string& str, unsigned int uc, const char **errmsg)
{
	if (uc == 0) return set_errmsg ("invalid Unicode character", errmsg);

	char utf8_buf[UTF8_MAX_CHAR_SIZE + 1];
	int utf8_len = utf8_encode_char (uc, utf8_buf);
	if (utf8_len == 0) return set_errmsg ("invalid Unicode character", errmsg);

	str.append (utf8_buf, utf8_len);
	return true;
}

inline int a2h (unsigned char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	c |= 0x20;
	if (c >= 'a' && c <= 'z') return c - 'a' + 10;
	return -1;
}

inline int h2a (unsigned int h)
{
	return h + ((h < 10) ? '0' : 'A'-10);
}

bool katran_string_parse (const char *kt_str, unsigned int kt_str_len, std::string& str, const char **errmsg)
{
	if (errmsg != 0) *errmsg = 0;

	const char *pp = kt_str;
	const char *pe = pp + kt_str_len;

	while (pp < pe)
	{
		unsigned int len = utf8_char_validate (pp, pe);
		if (len == 0) return invalid_utf8 (errmsg);

		const unsigned char c = *pp;

		if (len > 1)
		{
			str.append (pp, len);
			pp += len;
			continue;
		}

		if (c < 0x20 && c != '\r' && c != '\n' && c != '\t')
			return set_errmsg ("invalid character(s)", errmsg);

		++pp;

		if (c != '\\')
		{
			str += c;
			continue;
		}

		// escape processing
		if (pe - pp < 1)
			return invalid_escape (errmsg);

		char esc = *pp++;

		if (esc == 'x' || esc == 'X')
		{
			// TODO:
			if (pe - pp < 2)
				return invalid_escape (errmsg);

			int h = a2h (pp[0]);
			int l = a2h (pp[1]);
			if (h < 0 || l < 0) return invalid_escape (errmsg);

			unsigned int uc = (h << 4) | l;
			if (! append_unicode_char (str, uc, errmsg)) return false;

			pp += 2;
		}
		else if (esc == 'u')
		{
			if (pe - pp < 4)
				return invalid_escape (errmsg);

			unsigned int uc = 0;
			for (int i = 0; i< 4; ++i)
			{
				int n = a2h (pp[i]);
				if (n < 0) return invalid_escape (errmsg);
				uc = (uc << 4) | n;
			}

			if (! append_unicode_char (str, uc, errmsg)) return false;

			pp += 4;
		}
		else
		{
			switch (esc)
			{
			case 'n':
				esc = '\n';
				break;
			case 'r':
				esc = '\r';
				break;
			case 't':
				esc = '\t';
				break;
			case '\"':
			case '\\':
			case '\'':
				break;
			default:
				return invalid_escape (errmsg);
			}
			str += esc;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////
// bison interface

#ifdef _MSC_VER
# pragma warning(disable:4065) // switch statement contains 'default' but no 'case' labels
# pragma warning(disable:4060) // switch statement contains no 'case' or 'default' labels
#endif

#define YYPARSE_PARAM parser
#define my_lex ((cParser *)parser)->yylex
#define my_error ((cParser *)parser)->yyerror

// TODO: use yyoverflow();

// force stack allocation using alloca() to make code exception-safe
#define YYSTACK_USE_ALLOCA 1

// set YYMAXDEPTH to a small value to guard against stack overflow when using alloca()
#define YYMAXDEPTH  2000

/* Bison manual states:

Because of semantical differences between C and C++,
the LALR(1) parsers in C produced by Bison by compiled as C++ cannot grow.
In this precise case (compiling a C parser as C++) you are suggested to grow YYINITDEPTH.

So, define YYINITDEPTH equal to YYMAXDEPTH
*/
#define YYINITDEPTH YYMAXDEPTH

#include "Grammar.c"