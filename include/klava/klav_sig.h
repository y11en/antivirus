// klav_sig.h
//
//

#ifndef klav_sig_h_INCLUDED
#define klav_sig_h_INCLUDED

#include <klava/klavdef.h>
#include <klava/binstream.h>
#include <klava/klavstl/buffer.h>

////////////////////////////////////////////////////////////////
// Signature binary format

// maximum signature size in bytes
#define KLAV_SIG_MAX_BYTES 0x4000

// flags (plain signature should have NONE of these flags)
enum
{
	KLAV_SIG_F_HAS_IBYTES    = 0x0001,  // signature has case-insensitive bytes
	KLAV_SIG_F_HAS_WILDCARDS = 0x0010,  // signature has wildcards (including bitmasks/ranges)
	KLAV_SIG_F_HAS_ANYBYTES  = 0x0020,  // signature has arbitrary bytes ('??')
	KLAV_SIG_F_HAS_BITMASKS  = 0x0040,  // signature has arbitrary bitmasks (&XX=YY) and half-byte '?'
	KLAV_SIG_F_HAS_RANGES    = 0x0080,  // signature has ranges (RXX-YY)
	KLAV_SIG_F_HAS_STARS     = 0x0100,  // signature has 'stars' (*)
	KLAV_SIG_F_INVALID       = 0x0800,  // invalid signature data
};

// signature element types
enum
{
	KLAV_SIG_ELT_INVALID = 0, // undefined
	KLAV_SIG_ELT_BYTES,       // raw bytes, arg[0] - byte count, arg[1] - position of first byte 
	KLAV_SIG_ELT_IBYTES,      // case-insensitive chars, arg[0] - byte count, arg[1] - position of first byte
	KLAV_SIG_ELT_ANYBYTES,    // '??' wildcard, arg[0]-count
	KLAV_SIG_ELT_BITMASK,     // &XX=YY construct, arg[0]-mask, arg[1]-value
	KLAV_SIG_ELT_RANGE,       // RXX-YY construct, arg[0]-lbound, arg[1]-hbound
	KLAV_SIG_ELT_STAR,        // '*' construct, arg[0]-lbound, arg[1]-hbound
};

// returns element size, 0 on error
KLAV_EXTERN_C
size_t KLAV_CALL KLAV_Sig_Element_Scan (
		const uint8_t * data,
		size_t data_size,
		struct KLAV_Sig_Element *elt
	);

// returns mask of element flags for signature data
KLAV_EXTERN_C
unsigned int KLAV_CALL KLAV_Sig_Element_Flags (
		const uint8_t * data,
		size_t data_size
	);

// signature data element
struct KLAV_Sig_Element
{
	unsigned int m_type;     // element type
	unsigned int m_size;     // element size
	unsigned int m_args[2];  // arguments

#ifdef __cplusplus
	KLAV_Sig_Element ()
	{
		clear ();
	}

	void clear ()
	{
		m_type = KLAV_SIG_ELT_INVALID;
		m_size = 0;
		m_args[0] = 0;
		m_args[1] = 0;
	}

	// returns element size, 0 on error or EOF
	size_t scan (const uint8_t *ptr, size_t len)
		{ return KLAV_Sig_Element_Scan (ptr, len, this); }

	unsigned int type () const
		{ return m_type; }

	// helpers for BYTES element
	unsigned int get_data_offset () const
		{ return m_args[1]; }

	unsigned int get_data_size () const
		{ return m_args[0]; }

	// helpers for ANYBYTES element
	unsigned int get_anybyte_count () const
		{ return (unsigned int)(((m_args[1]) << 8) | m_args[0]); }

	// helpers for BITMASK element
	uint8_t get_bitmask_mask () const
		{ return (uint8_t) m_args[0]; }

	uint8_t get_bitmask_value () const
		{ return (uint8_t) m_args[1]; }

	// helpers for RANGE element
	uint8_t get_range_lbound () const
		{ return (uint8_t) m_args[0]; }

	uint8_t get_range_hbound () const
		{ return (uint8_t) m_args[1]; }

	// helpers for STAR element
	uint8_t get_star_count () const
		{ return (unsigned int)(((m_args[1]) << 8) | m_args[0]); }

#endif // __cplusplus
};

////////////////////////////////////////////////////////////////
// Signature data parser

// signature parser flags
enum
{
	KLAV_SIG_PARSE_CASE_INSENSITIVE = 0x0001,  // by default, consider text case-insensitive
	KLAV_SIG_PARSE_ALLOW_EMPTY      = 0x0002,  // do not treat empty signature as error
	KLAV_SIG_PARSE_ALLOW_INCOMPLETE = 0x0004,  // do not return error if entry is incomplete (for editor)

	KLAV_SIG_PARSE_ENCODING_MASK    = 0xFF00,  // string encoding mask
};

// string encodings
enum
{
	KLAV_SIG_PARSE_ENCODING_ASCII        = 0x0100,  // default: disallow non-ascii chars
	KLAV_SIG_PARSE_ENCODING_UTF8         = 0x0200,  // use UTF-8 encoding
	KLAV_SIG_PARSE_ENCODING_CP1251       = 0x0300,  // use CP1251 encoding

	KLAV_SIG_PARSE_ENCODING_DEFAULT      = 0x0000,  // default: use default behavior (ASCII?)
};


KLAV_EXTERN_C
klav_bool_t KLAV_CALL KLAV_Sig_Parse (
			hKLAV_Alloc alloc,
			const char *text,         // signature text
			size_t text_len,          // length of signature text
			unsigned int flags,       // parse flags (KLAV_SIG_PARSE_XXX)
			hKLAV_Bin_Writer output,  // output buffer writer
			const char **errmsg = 0
		);

// conversion to search tree format
KLAV_EXTERN_C
klav_bool_t KLAV_CALL KLAV_Sig_Conv_Tree (
			const void * sig_data,
			size_t sig_data_len,
			klavstl::buffer <klav_allocator>& tree_sig,
			const char **errmsg
		);

////////////////////////////////////////////////////////////////

#ifdef KLAV_SIG_CONV_TREE_IMPL
# ifndef KLAV_SIG_DATA_IMPL
#  define KLAV_SIG_DATA_IMPL
# endif
# include "../../klava/kernel/klavcorelib/sig_conv_tree.cpp"
#endif// KLAV_SIG_CONV_TREE_IMPL

#ifdef KLAV_SIG_DATA_IMPL
# include "../../klava/kernel/klavcorelib/sig_data.cpp"
#endif// KLAV_SIG_DATA_IMPL

#ifdef KLAV_SIG_PARSER_IMPL
# include "../../klava/kernel/klavcorelib/sig_parser.cpp"
# include "../../klava/kernel/klavcorelib/cp1251r.cpp"
#endif // KLAV_SIG_PARSER_IMPL

#endif // klav_sig_h_INCLUDED

