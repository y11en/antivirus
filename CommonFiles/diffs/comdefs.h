#ifndef COMDEFS_H_INCLUDED_C4294A59_1428_4b95_94FD_9C4263F4D4B3
#define COMDEFS_H_INCLUDED_C4294A59_1428_4b95_94FD_9C4263F4D4B3

#ifndef KLAVPACK_API

    #if defined(LIBS_STATIC_LINKING) || !defined(WIN32)
        #define KLAVPACK_API
    #else
        #ifdef KLAVPACK_EXPORTS
            #define KLAVPACK_API __declspec(dllexport)
        #else
            #define KLAVPACK_API __declspec(dllimport)
        #endif // KLAVPACK
    #endif

#endif // KLAVPACK_API



struct Diff_Buffer 
{
	const unsigned char * m_data;
	size_t                m_size;
	void *                m_pimpl;
#ifdef __cplusplus
	Diff_Buffer ();
	~Diff_Buffer ();

private:
	Diff_Buffer (const Diff_Buffer&);
	Diff_Buffer& operator= (const Diff_Buffer&);
#endif // __cplusplus
};


#ifdef __cplusplus
extern "C"
#else
extern
#endif
void KLAVPACK_API Diff_Buffer_Free (struct Diff_Buffer *buf);

#ifdef __cplusplus
inline Diff_Buffer::Diff_Buffer ()
	: m_data (0), m_size (0), m_pimpl (0)
{
}
inline Diff_Buffer::~Diff_Buffer ()
{
	Diff_Buffer_Free (this);
}
#endif // __cplusplus
#endif  // COMDEFS_H_INCLUDED_C4294A59_1428_4b95_94FD_9C4263F4D4B3
