/*
 * $Id: byteorder.h,v 1.16 2006/05/06 12:21:01 santucco Exp $
 */

#ifndef BYTEORDER_H
#define BYTEORDER_H

#include "AVP_data.h"

#if (defined (__sparc__) || defined (__hpux__) || defined (__ppc__)) && !defined (MACHINE_IS_BIG_ENDIAN)
	#error MACHINE_IS_BIG_ENDIAN should be defined on Sparc & HP-UX
#endif

// all platforms are responsible to add own define
// to make sense when size_t is 64 bit and when is 32
#if defined(_M_X64) || defined(_M_IA64) || defined(_WIN64) || defined(__x86_64__) || defined(__ia64__) || defined(__LP64__)
	#define ReadSize_tPtr(ptr)       ReadQWordPtr(ptr)
	#define WriteSize_tPtr(ptr,data) WriteQWordPtr(ptr,data)
#else
	#define ReadSize_tPtr(ptr)       ReadDWordPtr(ptr)
	#define WriteSize_tPtr(ptr,data) WriteDWordPtr(ptr,data)
#endif

/* Intel and other LITTLE-endians */
#if !defined (MACHINE_IS_BIG_ENDIAN)
	#ifdef __cplusplus
		inline AVP_word ReadWordPtr (AVP_word * ptr)    				{ 	return *ptr; 	}
		inline AVP_dword ReadDWordPtr (AVP_dword * ptr) 				{ 	return *ptr;	}
		inline AVP_qword ReadQWordPtr (AVP_qword * ptr) 				{ 	return *ptr;	}
		inline void * ReadUnalignedPtr (void ** ptr)					{	return *ptr;	}
		inline void WriteWordPtr (AVP_word * ptr, AVP_word data) 		{	*ptr = data;	}
		inline void WriteDWordPtr (AVP_dword * ptr, AVP_dword data)		{	*ptr = data;	}
		inline void WriteQWordPtr (AVP_qword * ptr, AVP_qword data)		{	*ptr = data;	}		
		inline void WriteUnalignedPtr (void * ptr, void * data)			{	*((AVP_dword *)ptr) = (AVP_dword) data;	}
	#else
		#define ReadWordPtr(ptr)		(*(ptr))
		#define ReadDWordPtr(ptr)		(*(ptr))
		#define ReadQWordPtr(ptr)		(*(ptr))
		#define ReadUnalignedPtr(ptr)	(*(ptr))		
		#define WriteWordPtr(a,b)		((*(a)) = (b))
		#define WriteDWordPtr(a,b)		((*(a)) = (b))
		#define WriteQWordPtr(a,b)		((*(a)) = (b))
		#define WriteUnalignedPtr(a,b)	((*(a)) = (b))				
	#endif /* __cplusplus */

	#define LittleEndianToMachine(value) (value)
	#define MachineToLittleEndian(value) (value)

#else

	#if defined (__cplusplus)
		// Sparc and other BIG-endians 
		template <class T> inline T LittleEndianToMachine (T value)
		{
			unsigned char * b_order = (unsigned char *) &value;

			for ( register unsigned i = 0; i < (sizeof(T) / 2); i++ )
			{
				unsigned char tmpval = b_order[i];
				b_order[i] = b_order[sizeof(T)-i-1];
				b_order[sizeof(T)-i-1] = tmpval;
			}
		    return value;
		}

		template <class T> inline T MachineToLittleEndian (T value)
		{
			unsigned char * b_order = (unsigned char *) &value;

			for ( register unsigned i = 0; i < (sizeof(T) / 2); i++ )
			{
				unsigned char tmpval = b_order[i];
				b_order[i] = b_order[sizeof(T)-i-1];
				b_order[sizeof(T)-i-1] = tmpval;
			}
		    return value;
		}
	#endif /* defined (__cplusplus) */

	static inline AVP_word ReadWordPtr (AVP_word * ptr)
	{
		register AVP_word value = 0;

		value |= ((unsigned char *) ptr)[1];
		value <<= 8;
		value |= ((unsigned char *) ptr)[0];
		return value;
	}

	static inline AVP_dword ReadDWordPtr (AVP_dword * ptr)
	{
		register AVP_dword value = 0;
		value |= ((unsigned char *) ptr)[3];
		value <<= 8;
		value |= ((unsigned char *) ptr)[2];
		value <<= 8;
		value |= ((unsigned char *) ptr)[1];
		value <<= 8;
		value |= ((unsigned char *) ptr)[0];
		return value;
	}

	static inline AVP_qword ReadQWordPtr (AVP_qword * ptr)
	{
		register AVP_qword value = 0;
		value |= ((unsigned char *) ptr)[7];
		value <<= 8;
		value |= ((unsigned char *) ptr)[6];
		value <<= 8;
		value |= ((unsigned char *) ptr)[5];
		value <<= 8;
		value |= ((unsigned char *) ptr)[4];
		value <<= 8;		
		value |= ((unsigned char *) ptr)[3];
		value <<= 8;
		value |= ((unsigned char *) ptr)[2];
		value <<= 8;
		value |= ((unsigned char *) ptr)[1];
		value <<= 8;
		value |= ((unsigned char *) ptr)[0];
		return value;
	}

	static inline void WriteWordPtr (AVP_word * ptr, AVP_word data)
	{
		((unsigned char *) ptr)[0] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[1] = (unsigned char) data;
	}


	static inline void WriteDWordPtr (AVP_dword * ptr, AVP_dword data)
	{
		((unsigned char *) ptr)[0] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[1] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[2] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[3] = (unsigned char) data;
	}

	static inline void WriteQWordPtr (AVP_qword * ptr, AVP_qword data)
	{
		((unsigned char *) ptr)[0] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[1] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[2] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[3] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[4] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[5] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[6] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[7] = (unsigned char) data;
	}

	static inline void * ReadUnalignedPtr (void * ptr)
	{
		register AVP_dword value = 0;
		value |= ((unsigned char *) ptr)[0];
		value <<= 8;
		value |= ((unsigned char *) ptr)[1];
		value <<= 8;
		value |= ((unsigned char *) ptr)[2];
		value <<= 8;
		value |= ((unsigned char *) ptr)[3];
		return (void *) value;
	}
	
	static inline void WriteUnalignedPtr (void * ptr, void * dt)
	{
		register AVP_dword data = (AVP_dword) dt;
		((unsigned char *) ptr)[3] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[2] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[1] = (unsigned char) data;
		data >>= 8;
		((unsigned char *) ptr)[0] = (unsigned char) data;
	}
	

#endif /* byte order */

#endif /* BYTEORDER_H */


