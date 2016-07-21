#include "windows.h"
#include "stdio.h"
#include "cio.h"

/* Standard error macro for reporting API errors */ 

#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

void _cls()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the cursor */ 
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;	/* to get buffer info */ 
	DWORD dwConSize;					/* number of character cells in the current buffer */ 
										
	/* get the number of character cells in the current buffer */ 
	
	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	PERR( bSuccess, "GetConsoleScreenBufferInfo" );
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	
	/* fill the entire screen with blanks */ 
	
	bSuccess = FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten );
	PERR( bSuccess, "FillConsoleOutputCharacter" );
	
	/* get the current text attribute */ 
	
	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	PERR( bSuccess, "ConsoleScreenBufferInfo" );
	
	/* now set the buffer's attributes accordingly */ 
	
	bSuccess = FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten );
	PERR( bSuccess, "FillConsoleOutputAttribute" );
	
	/* put the cursor at (0, 0) */ 
	
	bSuccess = SetConsoleCursorPosition( hConsole, coordScreen );
	PERR( bSuccess, "SetConsoleCursorPosition" );
	return;
} 

char* _gets(char* str)
{
	return gets(str);
}

int _printf(const char* format, ...)
{
	int nRes;
	va_list marker;
	va_start(marker, format);     // Initialize variable arguments. 
	nRes = vprintf(format, marker);
	va_end(marker);  
	return nRes;
}

unsigned short GetConsoleTextAttr()
{
	HANDLE hStdout;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (hStdout == INVALID_HANDLE_VALUE) 
		return 0;
	
	if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
		return 0; 
	
	return csbiInfo.wAttributes; 
}

int SetConsoleTextAttr(unsigned short wAttr)
{
	HANDLE hStdout;
	
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (hStdout == INVALID_HANDLE_VALUE) 
		return FALSE;
	
	return SetConsoleTextAttribute(hStdout, wAttr);
}

