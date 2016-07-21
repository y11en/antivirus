
#ifndef __RELEASETRY_H__
#define __RELEASETRY_H__

#ifndef _DEBUG

    #define RELEASE_TRY   \
        try {

    #define RELEASE_CATCH  \
        } catch (...) {

    #define RELEASE_ENDTRY \
        }

#else

    #define RELEASE_TRY

    #define RELEASE_CATCH

    #define RELEASE_ENDTRY

#endif // _DEBUG

#endif // __RELEASETRY_H__