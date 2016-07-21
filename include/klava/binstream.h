// binstream.h
//

#ifndef binstream_h_INCLUDED
#define binstream_h_INCLUDED

#include <klava/klavdef.h>

// binary writer interface
struct KLAV_Bin_Writer;
typedef struct KLAV_Bin_Writer * hKLAV_Bin_Writer;

#ifdef __cplusplus

struct KL_NO_VTABLE KLAV_Bin_Writer
{
public:
	virtual klav_bool_t KLAV_CALL write (const void *data, size_t size) = 0;
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL (KLAV_Bin_Writer)
{
    klav_bool_t (KLAV_CALL * write) (struct KLAV_Bin_Writer *, size_t size);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Bin_Writer);

#endif // __cplusplus


#ifdef __cplusplus

// common buffer implementation working with STL buffers
template <class BUF>
class KLAV_Buffer_Writer : public KLAV_Bin_Writer
{
public:
	KLAV_Buffer_Writer (BUF& buf) : m_buf (buf) {}

	typedef typename BUF::value_type value_type;

	BUF& buffer ()
		{ return m_buf; }

	const BUF& buffer () const
		{ return m_buf; }

	klav_bool_t KL_CDECL write (const void *data, size_t size)
		{ size_t oldsz = m_buf.size ();
		  m_buf.insert (m_buf.end (), (const value_type *)data, (const value_type *)data + size);
		  return m_buf.size () == oldsz + size;
		}

private:
	BUF& m_buf;
};

#endif // __cplusplus

#endif //binstream_h_INCLUDED

