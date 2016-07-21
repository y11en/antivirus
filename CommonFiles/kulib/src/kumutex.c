

#include "kumutex.h"
#include "kualloc.h"

#if defined( WIN32 ) || defined( _WIN32 )

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOUSER
#define NOSERVICE
#define NOIME
#define NOMCX
#include <windows.h>


int kumutex_init( kumutex_t* mutex )
{
	*mutex = ku_malloc( sizeof(CRITICAL_SECTION) );
	if( *mutex == NULL )
	{
    	return KU_MEMORY_ALLOCATION_ERROR;
	}

	InitializeCriticalSection( (LPCRITICAL_SECTION) *mutex );

	return KU_OK;
}


int kumutex_destroy( kumutex_t* mutex )
{
	DeleteCriticalSection( (LPCRITICAL_SECTION) *mutex );
	ku_free( *mutex );

	return KU_OK;
}

int kumutex_lock( kumutex_t* mutex )
{
	EnterCriticalSection( (LPCRITICAL_SECTION) *mutex );
	return KU_OK;
}


int kumutex_unlock( kumutex_t* mutex )
{
	LeaveCriticalSection( (LPCRITICAL_SECTION) *mutex );
	return KU_OK;
}


#elif /* WIN32 */

#include <pthread.h>

int kumutex_init( kumutex_t* mutex )
{
	*mutex = ku_malloc( sizeof(pthread_mutex_t) );
	if( *mutex == NULL )
	{
    	return KU_MEMORY_ALLOCATION_ERROR;
	}

	pthread_mutex_init( (pthread_mutex_t*) *mutex, NULL );

	return KU_OK;
}


int kumutex_destroy( kumutex_t* mutex )
{
	pthread_mutex_destroy( (pthread_mutex_t*) *mutex );
	ku_free( *mutex );

	return KU_OK;
}

int kumutex_lock( kumutex_t* mutex )
{
	pthread_mutex_lock( (pthread_mutex_t*) *mutex );
	return KU_OK;
}


int kumutex_unlock( kumutex_t* mutex )
{
	pthread_mutex_unlock( (pthread_mutex_t*) *mutex );
	return KU_OK;
}

#endif