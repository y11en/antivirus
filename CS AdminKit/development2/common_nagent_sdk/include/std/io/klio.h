#ifndef KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5
#define KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5

#ifdef __unix__
#include <stdio.h>
#endif

#include "avp/avp_data.h"

#include "std/base/kldefs.h"
#include "std/base/klbase.h"
#include "std/thr/sync.h"


namespace KLSTD{

#ifdef _WIN32
	const wchar_t c_szwPathDelimiters[]=L"\\/";
#else
 #ifdef N_PLAT_NLM
	const wchar_t c_szwPathDelimiters[]=L"\\/";
 #else
	const wchar_t c_szwPathDelimiters[]=L"/";
 #endif
#endif


	typedef enum{
		CF_OPEN_EXISTING=1,									//opens only if exists else fail
		CF_CREATE_NEW=2,									//creates only if not exists else fail
		CF_OPEN_ALWAYS=CF_OPEN_EXISTING|CF_CREATE_NEW,		//opens if exists and creates if not exists
		CF_CLEAR=4,
		CF_TRUNCATE_EXISTING=CF_OPEN_EXISTING|CF_CLEAR,		//truncate 
		CF_CREATE_ALWAYS=CF_OPEN_ALWAYS|CF_CLEAR			//creates if not exists and opens and clears if exists
	}CREATION_FLAGS;

	typedef enum{
		AF_READ=1,		// Read access
		AF_WRITE=2		// Write access
	}ACCESS_FLAGS;

	typedef enum{
		SF_READ=1,      // Sharing read
		SF_WRITE=2,     // Sharing write
	}SHARE_FLAGS;

	typedef enum{
		EF_RANDOM_ACCESS=1,             //Caching hint: file will be accessed randomly
		EF_SEQUENTIAL_SCAN=2,           //Caching hint: file will be accessed sequentially
		EF_TEMPORARY=4,                 //Caching hint: file is temporary and it will be deleted as soon as possible
        EF_DONT_WORRY_ABOUT_CLOSE=8,
        EF_DELETE_ON_CLOSE = 16,
        EF_WRITE_THROUGH = 32           //Caching hint: file will be written to disk directly
	}EXTRA_FLAGS;

	enum SEEK_TYPE{
		ST_SET,     //seek from beginning
		ST_CUR,     //seek from current position
		ST_END      //seek from the end of file
	};

};


#endif //KL_IO_H_68F05DB1_D23C_4d36_B9D6_69613A9352E5
