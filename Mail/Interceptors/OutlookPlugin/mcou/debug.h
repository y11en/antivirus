#ifndef __DEBUG_H__
#define __DEBUG_H__

namespace MCOU
{
	void HandleException (LPCTSTR szFileName, DWORD dwLine, LPEXCEPTION_POINTERS lpExceptPointers = NULL);
	void TranslFunc (unsigned int, LPEXCEPTION_POINTERS lpExceptPointers);
}

#ifdef _DEBUG

	#define MCOU_TRY \
		{

	#define MCOU_END_TRY \
		}

	#define MCOU_END_TRY_CATCH \
		} if(0) {

	#define MCOU_END_CATCH \
		}

	#define MCOU_END_TRY_RETHROW \
		}

#else

	#define MCOU_TRY \
		__try \
		{

	#define MCOU_END_TRY \
		} \
		__except(MCOU::TranslFunc(GetExceptionCode(), GetExceptionInformation()), 1) \
		{ \
			MCOU::HandleException (__FILE__, __LINE__); \
		}

	#define MCOU_END_TRY_CATCH \
		} \
		__except(MCOU::TranslFunc(GetExceptionCode(), GetExceptionInformation()), 1) \
		{ \
			MCOU::HandleException (__FILE__, __LINE__); 

	#define MCOU_END_CATCH \
		}

	#define MCOU_END_TRY_RETHROW \
		} \
		__except(MCOU::TranslFunc(GetExceptionCode(), GetExceptionInformation()), 1) \
		{ \
			MCOU::HandleException (__FILE__, __LINE__); \
			throw; \
		}

#endif // _DEBUG

#endif // __DEBUG_H__