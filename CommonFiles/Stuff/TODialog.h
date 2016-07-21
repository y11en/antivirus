////////////////////////////////////////////////////////////////////
//
//  TODialog.h
//  Dialog with timeout definition
//  AVP general purposess stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2002
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>

#define IDCLOSED_BY_TIMEOUT	  50

/*
Follow functions implement fuctionality for DialogBox with disapearing timeout.

The DialogBox will be closed in uElapseTime milliseconds unless there is any activity from the user. 
If the user does not close the DialogBox, but either presses a key when it is in the foreground, 
or does some mouse action over its client or non-client area, the DialogBox will wait 
for another uElapseTime milliseconds. But if the bStopWaitingByUserActivity=TRUE, waiting will stopped
and DialogBox will work as usual.

uBlinkingTime milliseconds before closing, 
the DialogBox caption will start blinking to attract user attention.

To be able to close the DialogBox DialogWithTimeoutStart function spawns another thread. 
This new thread sleeps for a while to give the DialogBox time to appear. 
Then the function waits either for timeout, or the user to close the DialogBox using 
WaitForSingleObject API. 
On timeout the function sends WM_COMMAND(MAKEWPARAM(IDOK, BN_CLICKED), LPARAM(::GetDlgItem(hDialogWnd, IDOK))) 
message to close the DialogBox.
	
Parameters :
uElapseTime - the timeout to close DialogBox automatically
              the "INFINITE" value means that waiting won't be performed and 
							DialogBox will work as usual
uBlinkingTime - the timeout to blinking DialogBox caption
bStopWaitingByUserActivity - if "TRUE", the function will stop waiting in case of any user activity
                             if "FALSE", the function will wait another uElapseTime

Result :
The cookie value which should be used as parameter of DialogWithTimeoutStop function.

The best plase to call DialogWithTimeoutStop is DestroyWindow method.
*/

DWORD DialogWithTimeoutStart( HWND hWnd, UINT uElapseTime=30000, UINT uBlinkingTime=5000, 
															BOOL bStopWaitingByUserActivity=TRUE );


void DialogWithTimeoutStop( DWORD dwCookie );
