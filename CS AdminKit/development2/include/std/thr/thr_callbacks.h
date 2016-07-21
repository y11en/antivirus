#ifndef __KL_THR_CALLBACKS_H__
#define __KL_THR_CALLBACKS_H__


namespace KLSTD
{
#ifdef _WIN32	
    typedef void (__cdecl *terminate_handler_t)();
#else
	typedef void (*terminate_handler_t)();
#endif
};


/*!
  \brief	Sets terminate handler for threads. Must be called BEFORE KLSTD_Initialize call. 
            KLSTD_Deinitialize calls KLSTD_SetTerminateCallback(NULL);

  \param	pHandler IN handler
*/
KLCSC_DECL void KLSTD_SetTerminateCallback(KLSTD::terminate_handler_t pHandler);


/*!
  \brief	Returns pointer set by KLSTD_SetTerminateCallback

  \return	pointer to handler
*/
KLCSC_DECL KLSTD::terminate_handler_t KLSTD_GetTerminateCallback();

#endif //__KL_THR_CALLBACKS_H__
