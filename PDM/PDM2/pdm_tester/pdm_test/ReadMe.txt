/////////////////////////////////////////////////////////////////////////////
//    CONSOLE APPLICATION : pdm_test Project Overview
//
//    PURPOSE : testing of heuristic functionality
//
//	  AUTHOR : Martynenko Vladislav
//
//	  DATE : 2007.04
/////////////////////////////////////////////////////////////////////////////
------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//Command line description:
/////////////////////////////////////////////////////////////////////////////
------------------------------------------------------------------------
pdm_test.exe [[[BY_CHILD] [COMMAND_ID_DESC | INI_FILE]] [NOT_TO_CLEAR]]
------------------------------------------------------------------------
BY_CHILD		- "prefix" indicate that action must be processing in self child process
COMMAND_ID_DESC	- string description of unic primitive function
				  it`s identical function ID
				  (full list of COMMAND_ID_DESC in "all_func_def.h")
INI_FILE		- name of ini-file (file must contain sequence of commands)
NOT_TO_CLEAR	- "postfix" indicate that action must not be rollback,
				  by default all action rollbacking
------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//Ini file description:
/////////////////////////////////////////////////////////////////////////////
------------------------------------------------------------------------
Ini file file must contain one or more commands.
Each command must be start from the new line.
Format of one command is equal to command line:
[[[BY_CHILD] [COMMAND_ID_DESC | INI_FILE]] [NOT_TO_CLEAR]].
So, this file can contain COMMAND_ID_DESC and INI_FILE with "prefix" and "postfix".
------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//List of self extracted const const
/////////////////////////////////////////////////////////////////////////////
------------------------------------------------------------------------
@SELF_PATH@			- full file path to current process
@SELF_DIR@			- folder name to current process
@SYSTEM_FOLDER@		- path to system folder (c:\windows\system32)
@APPDATA_FOLDER@	- path to system folder (C:\Documents and Settings\username\Application Data)
@PROGRAM_FILES@		- (C:\Program Files)
@USER_PROFILE@		- (C:\Documents and Settings\username)
@WINDIR@			- (c:\windows)
@USERNAME@			= (username)
------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//List of command line samples
/////////////////////////////////////////////////////////////////////////////
========================================================================
1.	Manual mode.
	Each action choosed manualy, by typing command number from list.
------------------------------------------------------------------------
pdm_test.exe 
========================================================================
2.	Direct command specified.
	Coping of self copy to %LocalDestFolder%.
	After that make rollback routine and finished.
------------------------------------------------------------------------
pdm_test.exe SC_TO_DF
------------------------------------------------------------------------
SC_TO_DF		- command ID description
========================================================================
3.	Ini-file specified.
	Processing sequence of commands in ini-file.
	After that make rollback routine and finished.
------------------------------------------------------------------------
pdm_test.exe SelfCopyToDestAndAutoReg1.txt
------------------------------------------------------------------------
SelfCopyToDestAndAutoReg1.txt - ini-file with sequence of commands
========================================================================
4.	Direct command specified with "postfix".
	Coping of self copy to %LocalDestFolder% and finished.
------------------------------------------------------------------------
pdm_test.exe SC_TO_DF NOT_TO_CLEAR
------------------------------------------------------------------------
SC_TO_DF		- command ID description
NOT_TO_CLEAR	- "postfix" indicate that action must not be rollback
========================================================================
5.	Direct command specified with "prefix"
	Process lunch self copy from current folder.
	Child process coping self copy to %LocalDestFolder%,
	after that make rollback routine and finished.
------------------------------------------------------------------------
pdm_test.exe BY_CHILD SC_TO_DF
------------------------------------------------------------------------
BY_CHILD		- "prefix" indicate that action must be processing in self child process
SC_TO_DF		- command ID description 
========================================================================
6.	Direct command specified with "prefix"
	Process lunch self copy from current folder.
	Child process coping self copy to %LocalDestFolder% and finished.
------------------------------------------------------------------------
pdm_test.exe BY_CHILD SC_TO_DF NOT_TO_CLEAR
------------------------------------------------------------------------
BY_CHILD		- "prefix" indicate that action must be processing in self child process
SC_TO_DF		- command ID description 
NOT_TO_CLEAR	- "postfix" indicate that action must not be rollback
========================================================================
7.	Self copy to specified folder with specified file name
------------------------------------------------------------------------
EX_COPY_FILE %SELF_PATH% c:\test\heurtest 1.exe
------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
