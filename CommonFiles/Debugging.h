#ifndef DEBUGGING_H
#define DEBUGGING_H

#ifdef _DEBUG
	#define AB_TRY			
	#define AB_CATCH_ALL	if (FALSE)
#else
	#ifdef AB_NOCATCH
		#define AB_TRY			
		#define AB_CATCH_ALL	if (FALSE)
	#else
		#define AB_TRY			try
		#define AB_CATCH_ALL	catch(...)
	#endif
#endif

#define ENTER		AB_TRY
#define LEAVE		AB_CATCH_ALL{}
#define LEAVE_PROC	AB_CATCH_ALL

#ifdef AB_LOGGING
	#define ODS(sMessage)	OutputDebugString(sMessage)
#else
	#define ODS(sMessage)
#endif

#endif