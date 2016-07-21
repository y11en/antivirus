////////////////////////////////////////////////////////////////////
//
//  TOMsgBox.h
//  Message box with timeout definition
//  AVP general purposess stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2002
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>

#define IDCLOSED_BY_TIMEOUT	  50

/*
Follow function implements standard Windows Message Box with disapearing timeout.

The first 4 parameters of funstion are the same as in MessageBox. 
The message box will be closed in uElapseTime milliseconds unless there is any activity from the user. 
If the user does not close the message box, but either presses a key when it is in the foreground, 
or does some mouse action over its client or non-client area, the message box will wait 
for another uElapseTime milliseconds. But if the bStopWaitingByUserActivity=TRUE, waiting will stopped
and message box will work as usual.

uBlinkingTime milliseconds before closing, 
the message box caption will start blinking to attract user attention.

To be able to close the message box MsgBoxWithTimeout function spawns another thread. 
This new thread sleeps for a while to give the message box time to appear and then tries 
to find its window handle. To this end it uses EnumThreadWindows API function looking for 
the window with "#32770" class name. Then the function waits either for timeout, 
or the user to close the message box using WaitForSingleObject API. 
On timeout the function calls EndDialog API to close the message box.
	
Parameters :
hWnd, lpText, lpCaption, uType - see MessageBox description
uElapseTime - the timeout to close message box automatically
              the "INFINITE" value means that waiting won't be performed and 
							message box will work as usual
uBlinkingTime - the timeout to blinking message box caption
bStopWaitingByUserActivity - if "TRUE", the function will stop waiting in case of any user activity
                             if "FALSE", the function will wait another uElapseTime
iResByElapsed - default result of MessageBox in case it is stopped by uElapseTime
                the "0" value means that default message box result will be used 
								(result that will be got if user presses "Enter" button)
*/

int MsgBoxWithTimeout( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, 
											 UINT uElapseTime=30000, UINT uBlinkingTime=5000, 
											 BOOL bStopWaitingByUserActivity=TRUE, int iResByElapsed=0 );
