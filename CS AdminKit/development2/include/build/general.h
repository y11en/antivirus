/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	general.h
 * \author	Andrew Kazachkov
 * \date	31.03.2003 13:15:39
 * \brief	
 * 
 */

#ifndef GENERAL_H_78E7E662_6CDC_4512_8498_9C6F65BFBEE1
#define GENERAL_H_78E7E662_6CDC_4512_8498_9C6F65BFBEE1

#ifdef _WIN32	

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


#if defined(UNICODE) || defined(_UNICODE)
	#define KLSTD_WINNT
#else
	#define KLSTD_WIN9X
#endif

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE         // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
#endif
#endif

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#if (defined(UNICODE) || defined(_UNICODE)) && defined(_MBCS)
#pragma message("Behavior is undefined if you define both _UNICODE and _MBCS !!!\n")
#endif

#endif

#endif // GENERAL_H_78E7E662_6CDC_4512_8498_9C6F65BFBEE1
