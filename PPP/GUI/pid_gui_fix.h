#pragma once

#ifndef __PID_GUI_FIX_H__
#define __PID_GUI_FIX_H__

#if defined (ADMINGUI_WKS)
	#undef PID_GUI
	#define PID_GUI 777
#endif

#if defined (ADMINGUI_FS)
	#undef PID_GUI
	#define PID_GUI 778
#endif

#if defined (ADMINGUI_SOS)
	#undef PID_GUI
	#define PID_GUI 779
#endif

#endif //__PID_GUI_FIX_H__
