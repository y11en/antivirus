#ifndef __KLMEM_H__
#define __KLMEM_H__

#include <std/base/kldefs.h>

/*!
  \brief	 Allocate memory for temporary object.

  \param	size_t size
  \return	void*  
*/
KLCSC_DECL void*   KLSTD_AllocTmp(size_t size);

/*!
  \brief	 Deallocate memory allocated with KLSTD_AllocTmp

  \param	p
*/
KLCSC_DECL void    KLSTD_FreeTmp(void* p);

/*!
  \brief	 Allocate large (64K and more) chunk of memory

  \param	size_t size
  \return	void*  
*/
KLCSC_DECL void*   KLSTD_AllocBuffer(size_t size);


/*!
  \brief	  Deallocate memory allocated with KLSTD_AllocBuffer

  \param	void* p
  \return	void  
*/
KLCSC_DECL void    KLSTD_FreeBuffer(void* p);

namespace KLSTD
{
    void initTmpHeap();
    void deinitTmpHeap();
};

#endif //__KLMEM_H__
