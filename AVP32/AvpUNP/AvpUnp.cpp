/////////////////////////////////////////////////////////////////////////////
//
// AvpUnp.cpp
// Archives unpacker
// Ilya Zverkov [zverkov@kaspersky.com], Kaspersky Labs. 2002
// Copyright (c) Kaspersky Labs.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <errno.h>

#include "ScanAPI/scanobj.h"
#include "ScanAPI/avp_dll.h"
#include "LoadKeys/key.h"
#include "Sign/sign_lib.h"
#include "Sign/ct_funcs.h"
#include "ScanAPI/ScanObj.h"
#include "ScanAPI/avp_dll.h"
#include "ScanAPI/avp_msg.h"
#include "Bases/sizes.h"
#include "../basework/workarea.h"
#include "Bases/Retcode3.h"
#include "IOCache/TFileData.h"

/////////////////////////////////////////////////////////////////////////////
// main() return values:
#define RETVAL_PARAMS_ERROR					60
#define RETVAL_BASES_ERROR					60
#define RETVAL_PROGRAM_INTERNAL_ERROR		50
#define RETVAL_IO_ERROR						40
#define RETVAL_CREATE_DIRECTORY_ERROR		40
#define RETVAL_CREATE_FILE_ERROR			40
#define RETVAL_ENGINE_ERROR					30
#define RETVAL_VIRUS_DETECTED				20
#define RETVAL_ALL_ARCHIVES_EXTRACTED		10
#define RETVAL_NO_ARCHIVES					 0

#define RETVAL_INCORRECT_SOURCE_PATH		 1
#define DEFAULT_AVP_BASE_FILE "avp.set"
#define DEFAULT_LOG_FILE "avpunp.log"

/////////////////////////////////////////////////////////////////////////////
// report detalization flags:
#define RDF_NO_ARCHIVE			0x00000001
#define RDF_ARCHIVE_DETECTED	0x00000002
#define RDF_FILE_SAVED			0x00000004
#define RDF_FILE_ERROR			0x00000008
#define RDF_LOAD_BASE_PROGRESS	0x00000010
#define RDF_CHECK_DIR			0x00000020
#define RDF_STATISTICS			0x00000040
#define RDF_ERROR				0x00000080
#define RDF_INFO				0x00000100

/////////////////////////////////////////////////////////////////////////////
// list of (char *)
typedef struct t_StrList {
	char *p_str;
	t_StrList *p_next;
} t_StrList;

/////////////////////////////////////////////////////////////////////////////
// callback function
LONG __stdcall OnAVPMessage(UINT uParam, LONG lParam);
  void ShowUsage();
   int UnpackObject(char *p_sIn, char *p_sOut);
char * If_MakeFullName(char* tempb, char* VirusName);
void PromptWhenDone();
void ShowHowToGetHelp();
void LogString(char *ansi_str);
void MakeUniqueFileName(char *filename);

/////////////////////////////////////////////////////////////////////////////
DWORD MFlags = MF_MASK_ALL |
			   MF_ARCHIVED |
			   MF_SFX_ARCHIVED |
			   MF_PACKED |
			   MF_MAILBASES |
			   MF_MAILPLAIN |
			   MF_ALLENTRY |
			   MF_CALLBACK_SAVE |
			   MF_D_DENYACCESS;
// log messages flags:
DWORD RDFlags = // RDF_NO_ARCHIVE |			// file is not an archive
				RDF_ARCHIVE_DETECTED |	// archive file or packed file detected
				RDF_FILE_SAVED |			// file saving message
				RDF_FILE_ERROR |			// file error message
				// RDF_LOAD_BASE_PROGRESS |	// base loading progress
				// RDF_CHECK_DIR |			// directory detected
				RDF_STATISTICS |			// statistics
				RDF_ERROR					// internal errors messages
				// RDF_INFO					// information messages
				;

int flagBaseFileIsOpened;
int flagOutputDirExists=0;
int flagSTDCheckForViruses=0;
char *p_sSTDFullArchivePath;
char *p_sSTDOutputDir, *p_sSTDOutputSubDir, *p_sSTDInput, *p_sSTDBaseFile, *p_sSTDLogFile;
int nSTDFiles=0, nSTDDirectories=0, nSTDArchivedFiles=0, nSTDPackedFiles=0, nSTDExtractedFiles=0,
	nSTDDetected=0, nSTDDisinfected=0, nSTDDisinfectionFailed=0;
int nSTDMainRetVal=0, flagSTDRetvalIsDefined = 0;
int flagSTDParameterOptionError = 0, flagSTDParameterPathError = 0;
int nSTDInvalidParameterOption, nSTDInvalidParameterPath;
int flagSTDOEM = 0;       // 0 - ANSI, 1 - OEM
char *p_sSTDBuffer;           // 64Kb buffer for output strings
LOAD_PROGRESS_STRUCT STDLoadProgressStruct;

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	DWORD dwTickCountStart = GetTickCount();
	p_sSTDBuffer = new char [64 * 1024];
	sprintf(p_sSTDBuffer, "AVPUNP (c) Kaspersky Labs. 2002\n\n");
	LogString(p_sSTDBuffer);
	// if command line format is needed
	if(argc < 2 || (argc == 2 && (!strcmp(argv[1], "/?") || !stricmp(argv[1], "/h") ))) {
		ShowUsage();
		return RETVAL_PARAMS_ERROR;
	}
	int flagPauseWhenDone = 0;
	p_sSTDFullArchivePath = NULL;
	AVP_RegisterCallbackPtr(OnAVPMessage);
	p_sSTDBaseFile = new char[strlen(DEFAULT_AVP_BASE_FILE) + 1];
	strcpy(p_sSTDBaseFile, DEFAULT_AVP_BASE_FILE);
	p_sSTDLogFile = new char[strlen(DEFAULT_LOG_FILE) + 1];
	strcpy(p_sSTDLogFile, DEFAULT_LOG_FILE);
	p_sSTDOutputDir = NULL;
	p_sSTDOutputSubDir = NULL;

	t_StrList *pInListHead = NULL; // subdirectories list
	t_StrList *pInListIterator = NULL; // subdirectories list iterator
	t_StrList *pInListLast = NULL; // pointer to last list element was inserted

	for(int i=1; i<argc; i++) {
		if(strlen(argv[i])==2 && !stricmp(argv[i], "-M")) {
			sprintf(p_sSTDBuffer, "Skipping mailbases (-M option)\n");
			LogString(p_sSTDBuffer);
			MFlags &= ~MF_MAILBASES;
			MFlags &= ~MF_MAILPLAIN;
		}else if(!stricmp(argv[i], "-P")) {
			sprintf(p_sSTDBuffer, "Skipping packed files (-P option)\n");
			LogString(p_sSTDBuffer);
			MFlags &= ~MF_PACKED;
		}else if(!stricmp(argv[i], "-A")) {
			sprintf(p_sSTDBuffer, "Skipping archived and SFX-archived files (-A option)\n");
			LogString(p_sSTDBuffer);
			MFlags &= ~MF_ARCHIVED;
			MFlags &= ~MF_SFX_ARCHIVED;
		//}else if(!stricmp(argv[i], "-S")) {
		//	sprintf(p_sSTDBuffer, "Skipping SFX-archived files (-S option)\n");
		//	MFlags &= ~MF_SFX_ARCHIVED;
		}else if(strlen(argv[i])>3 && !strnicmp(argv[i], "/B=", 3)) {
			if(p_sSTDBaseFile) delete p_sSTDBaseFile;
			p_sSTDBaseFile = new char[strlen(argv[i])-3+1];
			strcpy(p_sSTDBaseFile, argv[i]+3);
		}else if(strlen(argv[i])>3 && !strnicmp(argv[i], "/O=", 3)) {
			p_sSTDOutputDir = new char[strlen(argv[i])-3+1];
			strcpy(p_sSTDOutputDir, argv[i]+3);
			if(p_sSTDOutputDir[strlen(p_sSTDOutputDir)-1] == '\\') {
				p_sSTDOutputDir[strlen(p_sSTDOutputDir)-1] = '\x0';
			}
			flagOutputDirExists = 1;
		}else if(!stricmp(argv[i], "/V")) {
			flagSTDCheckForViruses = 1;
		}else if(!stricmp(argv[i], "/W")) {
			flagPauseWhenDone = 1;
		}else if(!stricmp(argv[i], "/OEM")) {
			flagSTDOEM = 1;
		}else if(!stricmp(argv[i], "/ANSI")) {
			flagSTDOEM = 0;
		}else if(argv[i][0] == '/' || argv[i][0] == '-') {
			if(!flagSTDParameterOptionError) {
				flagSTDParameterOptionError = 1;
				nSTDInvalidParameterOption = i;
			}
		}else {		
			// check whether the object exists
			char *p_sPathToCheck = new char[strlen(argv[i]) + 2];
			strcpy(p_sPathToCheck, argv[i]);
			// remove \ at the end of the pathname if exists
			if(p_sPathToCheck[strlen(p_sPathToCheck)-1] == '\\') {
				p_sPathToCheck[strlen(p_sPathToCheck)-1] = '\x0';
			}
			if(-1==_access(p_sPathToCheck, 0)) {
				if(!flagSTDParameterPathError) {
					flagSTDParameterPathError = 1;
					nSTDInvalidParameterPath = i;
				}
			}
			// write it's full pathname to a list element
			pInListIterator = new t_StrList;
			if(!pInListHead) {
				pInListHead = pInListIterator;
				pInListLast = pInListIterator;
			}else {
				pInListLast->p_next = pInListIterator;
			}
			pInListIterator->p_str = new char[strlen(p_sPathToCheck) + 1];
			strcpy(pInListIterator->p_str, p_sPathToCheck);
			pInListIterator->p_next = NULL;
			pInListLast = pInListIterator;
		}
	}
	if(flagSTDParameterPathError) {
		sprintf(p_sSTDBuffer, "Parameters error: can't find file or directory \"%s\"\n", argv[nSTDInvalidParameterPath]);
		LogString(p_sSTDBuffer);
		ShowHowToGetHelp();
		if(flagPauseWhenDone) PromptWhenDone();
		return RETVAL_PARAMS_ERROR;
	}
	if(flagSTDParameterOptionError) {
		sprintf(p_sSTDBuffer, "Parameters error: option \"%s\" is illegal\n", argv[nSTDInvalidParameterOption]);
		LogString(p_sSTDBuffer);
		ShowHowToGetHelp();
		if(flagPauseWhenDone) PromptWhenDone();
		return RETVAL_PARAMS_ERROR;
	}
	if(!_access(p_sSTDBaseFile, 0)) {
		sprintf(p_sSTDBuffer, "Using base file: %s\n", p_sSTDBaseFile);
		LogString(p_sSTDBuffer);
	}else {
		sprintf(p_sSTDBuffer, "Unable to use base file: %s\n", p_sSTDBaseFile);
		LogString(p_sSTDBuffer);
		ShowHowToGetHelp();
		if(flagPauseWhenDone) PromptWhenDone();
		return RETVAL_PARAMS_ERROR;
	}
	if(!pInListHead) {
		sprintf(p_sSTDBuffer, "Parameters error: no objects to unpack\n");
		LogString(p_sSTDBuffer);
		ShowHowToGetHelp();
		if(flagPauseWhenDone) PromptWhenDone();
		return RETVAL_PARAMS_ERROR;
	}
	if(!flagOutputDirExists) {
		sprintf(p_sSTDBuffer, "Parameters error: no output directory\n");
		LogString(p_sSTDBuffer);
		ShowHowToGetHelp();
		if(flagPauseWhenDone) PromptWhenDone();
		return RETVAL_PARAMS_ERROR;
	}else if(_access(p_sSTDOutputDir, 0)) {
			sprintf(p_sSTDBuffer, "Invalid output path: %s\n", p_sSTDOutputDir);
			LogString(p_sSTDBuffer);
			if(flagPauseWhenDone) PromptWhenDone();
			return RETVAL_PARAMS_ERROR;
	}else {
		sprintf(p_sSTDBuffer, "Output directory is: %s\n", p_sSTDOutputDir);
		LogString(p_sSTDBuffer);
	}

	// processing all objects defined in command line
	pInListIterator = pInListHead;
	while(pInListIterator) {
		sprintf(p_sSTDBuffer, "\nProcessing %s\n", pInListIterator->p_str);
		LogString(p_sSTDBuffer);
		if(p_sSTDInput) delete p_sSTDInput;
		p_sSTDInput = new char[strlen(pInListIterator->p_str) + 1];
		strcpy(p_sSTDInput, pInListIterator->p_str);
		char *p_sInputModified = new char[strlen(p_sSTDInput) + 1];
		strcpy(p_sInputModified, p_sSTDInput);
		// replacing : and \ by _
		for(unsigned i=0; i<strlen(p_sInputModified); i++) {
			if(p_sInputModified[i] == ':' || p_sInputModified[i] == '\\' || p_sInputModified[i] == '/') {
				p_sInputModified[i] = '_';
			}
		}
		p_sSTDOutputSubDir = new char[strlen(p_sSTDOutputDir) + 1 + strlen(p_sInputModified) + 1];
		strcpy(p_sSTDOutputSubDir, p_sSTDOutputDir);
		strcat(p_sSTDOutputSubDir, "\\");
		strcat(p_sSTDOutputSubDir, p_sInputModified);

		if(!flagBaseFileIsOpened) {
			LONG res = AVP_LoadBase(p_sSTDBaseFile, 0);
			if(STDLoadProgressStruct.recCount <= 100) {
				// if quantity of loaded records is too small
				sprintf(p_sSTDBuffer, "Bases loading error, records loaded: %d\n", STDLoadProgressStruct.recCount);
				LogString(p_sSTDBuffer);
				if(flagPauseWhenDone) PromptWhenDone();
				return RETVAL_BASES_ERROR;
			}
			flagBaseFileIsOpened = 1;
		}
 		UnpackObject(p_sSTDInput, p_sSTDOutputSubDir);
		delete p_sSTDOutputSubDir;
		delete p_sInputModified;

		t_StrList *pInListTemp = pInListIterator->p_next;
		delete pInListIterator->p_str;
		delete pInListIterator;
		pInListIterator = pInListTemp;
	}
	
	if(p_sSTDOutputDir) delete p_sSTDOutputDir;
	if(p_sSTDInput) delete p_sSTDInput;
	if(p_sSTDBaseFile) delete p_sSTDBaseFile;
	if(p_sSTDLogFile) delete p_sSTDLogFile;

	// stopping stopwatch
	DWORD dwTicks = GetTickCount() - dwTickCountStart;
	int nHours = (int)dwTicks / (60*60*1000);
	int nMinutes = ((int)dwTicks - nHours * (60*60*1000)) / (60*1000);
	int nSeconds = (int)dwTicks % (60*1000) / 1000 + 1;

	// statistics if needed
	if(RDFlags & RDF_STATISTICS) {
		sprintf(p_sSTDBuffer, "\n");
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Virus detection: %s\n", flagSTDCheckForViruses?"ON":"OFF");
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Records loaded: %d\n", STDLoadProgressStruct.recCount);
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Files: %d\n", nSTDFiles);
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Directories: %d\n", nSTDDirectories);
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Archived files: %d\n", nSTDArchivedFiles);
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Packed files: %d\n", nSTDPackedFiles);
		LogString(p_sSTDBuffer);
		sprintf(p_sSTDBuffer, "Extracted files: %d\n", nSTDExtractedFiles);
		LogString(p_sSTDBuffer);
		if(flagSTDCheckForViruses) {
			sprintf(p_sSTDBuffer, "Infected: %d\n", nSTDDetected);
			LogString(p_sSTDBuffer);
		}
		sprintf(p_sSTDBuffer, "Process time: %02d:%02d:%02d\n", nHours, nMinutes, nSeconds);
		LogString(p_sSTDBuffer);
	}
	delete p_sSTDBuffer;
	if(flagPauseWhenDone) PromptWhenDone();
	if(!flagSTDRetvalIsDefined) {
		if(nSTDExtractedFiles)	{
			// there was at least one archive, or packed file, or mailbase/mailplain file
			nSTDMainRetVal = RETVAL_ALL_ARCHIVES_EXTRACTED;
		}else {
			// no archives and packed files were found
			nSTDMainRetVal = RETVAL_NO_ARCHIVES;
		}
	}
	return nSTDMainRetVal;
}

/////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
	sprintf(p_sSTDBuffer, 	"\nDecompresses archived files, SFX-archived files, packed files, mailbases\n"
			"and mailplain files.\n\n"
			"AVPUNP <files> <directories> /O=<output_directory> [/B=<base_file>]\n"
			"[-M] [-P] [-A] [/V] [/W] [/ANSI | /OEM]\n\n"
			"<files and directories>  Files and directories to decompress. All directories\n"
			"                         are decompressed with subdirectories.\n\n"
			"/O=<output_directory>    Top level directory decompression location.\n"
			"                         (For example /O=D:\\Output)\n\n"
			"/B=<base_file>           Causes the program to use <base_file> with a set of\n"
			"                         AVP bases (for example /B=C:\\AVP\\avp.set),\n"
			"                         by default the program uses file AVP.SET from current\n"
			"                         directory. Please, be sure you are using the latest\n"
			"                         versions of anti-virus bases. You can get the latest\n"
			"                         versions at ftp://ftp.kaspersky.com/bases.\n\n"
			"-M   Disable decompression of mailbases and mailplain files\n"
			"-P   Disable decompression of packed files\n"
			"-A   Disable decompression of archived files, including SFX-archives\n"
			"/V   Enable virus detection (extra anti-virus bases from <base_file> will be\n"
			"         loaded)\n"
			"/W   Pause with prompt to press any key when done\n\n"
			"/ANSI   Write log using ANSI codepage (by default)\n"
			"/OEM    Write log using OEM codepage\n\n"
			"The program returns distinct error levels for success, failure, crash:\n"
			"ERRORLEVEL    DESCRIPTION\n"
			"         0    Everything is ok, no compressed files were detected\n"
			"        10    Everything is ok, all compressed files were decompressed\n"
			"        20    Virus detected\n"
			"        30    Engine error (compressed file was detected, but could not be\n"
			"                  decompressed)\n"
			"        40    File or directory creation error, other I/O errors\n"
			"        50    Program internal error\n"
			"        60    Incorrect command line, incorrect or corrupted AVP bases\n\n"
			"The program generates ASCII log messages and sends them to the standard output\n"
			"    stream.\n"
			"The program does not leave files locked if it errors out.\n"
			"The program does not modify existing environment variables.\n"
			"The program does not perform any functions outside of decompression,\n"
			"    i.e. deleting files, terminating processes (except it's own), etc.\n"
			"The program runs under Windows 2000 and Windows XP.\n"
		);
	LogString(p_sSTDBuffer);
}

/////////////////////////////////////////////////////////////////////////////
int UnpackObject(char *p_sIn, char *p_sOut)
{
	// saving parameters
	char *p_sInput = new char [strlen(p_sIn) + 1];
	strcpy(p_sInput, p_sIn);
	char *p_sOutputDir = new char [strlen(p_sOut) + 1];
	strcpy(p_sOutputDir, p_sOut);

	char *p_sInputDir = NULL;
	char *p_sFullName = NULL;
	t_StrList *pDirListHead = NULL; // list of subdirectories
	t_StrList *pDirListIterator = NULL;
	t_StrList *pDirListLast = NULL; // last filled list element
	
	// removing backslash
	if(p_sInput[strlen(p_sInput)-1] == '\\') {
		p_sInput[strlen(p_sInput)-1] = '\x0';
	}
	AVPScanObject ScanObject;
    WIN32_FIND_DATA fd;     
	// fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY |FILE_ATTRIBUTE_HIDDEN;

	HANDLE H = FindFirstFile(p_sInput, &fd);
	
	//if(H == INVALID_HANDLE_VALUE) return RETVAL_INCORRECT_SOURCE_PATH;
	//if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	if(H == INVALID_HANDLE_VALUE || fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		// if object is a directory or root directory
		FindClose(H);
		nSTDDirectories ++;

		p_sInputDir = new char [strlen(p_sInput) + strlen("\\*.*") + 1];
		strcpy(p_sInputDir, p_sInput);
		strcat(p_sInputDir, "\\*.*");
		H = FindFirstFile(p_sInputDir, &fd);
		if(H == INVALID_HANDLE_VALUE) return RETVAL_INCORRECT_SOURCE_PATH;
		do{
			if(p_sFullName) delete p_sFullName;
			p_sFullName = new char[strlen(p_sInput) + 1 + strlen(fd.cFileName) + 1];
			strcpy(p_sFullName, p_sInput);
			strcat(p_sFullName, "\\");
			strcat(p_sFullName, fd.cFileName);

			if(!strlen(fd.cFileName)||(DWORD)-1==GetFileAttributes(p_sFullName))
			{
				if(p_sFullName) delete p_sFullName;
				p_sFullName = new char[strlen(p_sInput) + 1 + strlen(fd.cAlternateFileName) + 1];
				strcpy(p_sFullName, p_sInput);
				strcat(p_sFullName, "\\");
				strcat(p_sFullName, fd.cAlternateFileName);

				if(!strlen(fd.cAlternateFileName)||(DWORD)-1==GetFileAttributes(p_sFullName))
					continue;
			}
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// if object is a directory
				if(!strcmp(fd.cFileName,".")) continue;
				if(!strcmp(fd.cFileName,"..")) continue;

				pDirListIterator = new t_StrList;
				if(!pDirListHead) {
					pDirListHead = pDirListIterator;
					pDirListLast = pDirListIterator;
				}else {
					pDirListLast->p_next = pDirListIterator;
				}
				pDirListIterator->p_str = new char[strlen(p_sFullName) + 1];
				strcpy(pDirListIterator->p_str, p_sFullName);
				pDirListIterator->p_next = NULL;
				pDirListLast = pDirListIterator;
			}else {
				// if object is a file
				UnpackObject(p_sFullName, p_sOutputDir);
			}
		}while(FindNextFile(H, &fd));
		
		FindClose(H);

		// processing subdirectories
		pDirListIterator = pDirListHead;
		while(pDirListIterator) {
			UnpackObject(pDirListIterator->p_str, p_sOutputDir);
			t_StrList *pDirListTemp = pDirListIterator->p_next;
			delete pDirListIterator->p_str;
			delete pDirListIterator;
			pDirListIterator = pDirListTemp;
		}
	}else {
		// if object is a file (not directory)
		nSTDFiles ++;
		memset(&ScanObject, 0, sizeof(ScanObject));
		strcpy(ScanObject.Name, p_sInput);
		ScanObject.Type = OT_FILE;
		ScanObject.MFlags = MFlags | MF_SCAN_STREAMS;
		ScanObject.Size = fd.nFileSizeLow;

		// processing file
		LONG result=AVP_ProcessObject(&ScanObject);
		if(ScanObject.RFlags & RF_ERROR) {
			sprintf(p_sSTDBuffer, "Engine error: can't process file \"%s\"\n", p_sInput);
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_ENGINE_ERROR) {
				nSTDMainRetVal = RETVAL_ENGINE_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
		}
		FindClose(H);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// callback function
LONG __stdcall OnAVPMessage(UINT uParam, LONG lParam)
{
	static LOAD_PROGRESS_STRUCT lps;
	LONG ret=0;
	char *p_sObjName = NULL;
	char tempb[0x200];
	AVPScanObject* ScanObject=(AVPScanObject*)lParam;
	char *p_sName;

	switch(uParam){
	case AVP_CALLBACK: 
		break;
		
	case AVP_CALLBACK_MB_OK:
		return 1;

	case AVP_CALLBACK_MB_OKCANCEL:
		return 1;

	case AVP_CALLBACK_MB_YESNO:
		return 1;

	case AVP_CALLBACK_PUT_STRING:
		// PutLogString((char*)lParam, "");
		break;

	case AVP_CALLBACK_ROTATE:
	case AVP_CALLBACK_ROTATE_OFF:
		break;

	case AVP_CALLBACK_LOAD_BASE:
		memcpy(&lps, (LOAD_PROGRESS_STRUCT*)lParam, sizeof(LOAD_PROGRESS_STRUCT));
		STDLoadProgressStruct = lps;
		if(!flagSTDCheckForViruses) {
			int i;
			for(i=strlen(lps.curName)-1; i>=0 && lps.curName[i]!='\\'; i--);
			char *p_sAVBaseFileShortName = new char[strlen(lps.curName) - i];
			strcpy(p_sAVBaseFileShortName, lps.curName + i + 1);
			if(!strnicmp(p_sAVBaseFileShortName, "backdoor", 8)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "avp", 3)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "ca", 2)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "daily", 5)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "macro", 5)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "script", 6)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "trojan", 6)) {
				return 1;	// do not load the base
			}else if(!strnicmp(p_sAVBaseFileShortName, "eicar", 5)) {
				return 1;	// do not load the base
			}else {
				return 0;	// load the base
			}
		}else { // if checking for viruses
			return 0;	// load all the bases
		}
	
	case AVP_CALLBACK_LOAD_PROGRESS:
		{
			if(RDFlags & RDF_LOAD_BASE_PROGRESS) {
				memcpy(&lps, (LOAD_PROGRESS_STRUCT*)lParam, sizeof(LOAD_PROGRESS_STRUCT));
				if(lps.curLength) {
					sprintf(p_sSTDBuffer, "Load base %s %d%%\n", lps.curName, lps.curPos*100/lps.curLength);
					LogString(p_sSTDBuffer);
				}
			}
		}
		break;

	case AVP_INTERNAL_MESSAGE:
		{
			//OMS* oms=(OMS*)lParam;
			//PutLogString(oms->Name,oms->Message);
			//delete oms;
		}
		break;

	case AVP_CALLBACK_OBJ_NAME:
		if(p_sSTDFullArchivePath) delete p_sSTDFullArchivePath;
		p_sSTDFullArchivePath = new char[strlen((char *)lParam) + 1];
		strcpy(p_sSTDFullArchivePath, (char *)lParam);
		if((MFlags & MF_PACKED) || (MFlags & MF_ARCHIVED))
		{
			if(strlen(p_sSTDFullArchivePath) > 4 &&
			   !stricmp(p_sSTDFullArchivePath + strlen(p_sSTDFullArchivePath) - 4, ".tmp")) {
				ret = 17;
				break;
			}
		}
		break;
	case AVP_INTERNAL_CHECK_DIR:
		if(RDFlags & RDF_CHECK_DIR) {
			p_sName = new char[strlen((char *)lParam) + 1];
			strcpy(p_sName, (char *)lParam);
			sprintf(p_sSTDBuffer, "%s\n", p_sName);
			LogString(p_sSTDBuffer);
			delete p_sName;
		}
		break;
	
	case AVP_CALLBACK_ASK_DELETE:
		return 2;

	case AVP_CALLBACK_ASK_CURE:
		return 1; 
	case AVP_CALLBACK_OBJECT_DONE:
		if(ScanObject->RFlags & RF_INTERFACE_FAULT){
			// sprintf(p_sSTDBuffer, "interface fault.\n");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_KERNEL_FAULT){
			sprintf(p_sSTDBuffer, "kernel fault.\n");
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_ENGINE_ERROR) {
				nSTDMainRetVal = RETVAL_ENGINE_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
			ScanObject->RFlags|=RF_REPORT_DONE;
		}

		if(ScanObject->RFlags & RF_NOT_A_PROGRAMM) break;
		if(ScanObject->RFlags & RF_NOT_OPEN) return 0; // return KillScan;

		if(ScanObject->RFlags & RF_REPORT_DONE) return 0; // return KillScan;
		
		if(ScanObject->RFlags & RF_DISK_OUT_OF_SPACE){
			sprintf(p_sSTDBuffer, "disk out of space.\n");
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_IO_ERROR) {
				nSTDMainRetVal = RETVAL_IO_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_CORRUPT){
			sprintf(p_sSTDBuffer, "corrupted.\n");
			LogString(p_sSTDBuffer);
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_IO_ERROR){
			sprintf(p_sSTDBuffer, "I/O error.\n");
			if(nSTDMainRetVal < RETVAL_IO_ERROR) {
				nSTDMainRetVal = RETVAL_IO_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
			LogString(p_sSTDBuffer);
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_CURE_FAIL){
			// s.Format("disinfection failed: %s",If_MakeFullName(tempb,ScanObject->VirusName));
			// ObjectMessage(ScanObject,s);	
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_PROTECTED){
			sprintf(p_sSTDBuffer, "password protected.\n");
			LogString(p_sSTDBuffer);
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(!(ScanObject->RFlags & RF_REPORT_DONE))
		{
			if( !(ScanObject->RFlags & ~(RF_CURE|RF_ARCHIVE|RF_PACKED)) ){
				if(RDFlags & RDF_NO_ARCHIVE) {
					sprintf(p_sSTDBuffer, "File %s, no archive\n");
					LogString(p_sSTDBuffer);
				}
			}
			else{
				sprintf(p_sSTDBuffer, "RFlags: %08X\n", ScanObject->RFlags);
				LogString(p_sSTDBuffer);
			}
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		return 0;

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
		nSTDDisinfectionFailed ++;
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "disinfection failed: %s\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_OBJECT_CURE:
		nSTDDisinfected ++;
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "disinfected: %s\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;

	case AVP_CALLBACK_OBJECT_DELETE:
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "deleted: %s\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;

	case AVP_CALLBACK_OBJECT_DETECT:
		nSTDDetected ++;
		if(nSTDMainRetVal < RETVAL_VIRUS_DETECTED) {
			nSTDMainRetVal = RETVAL_VIRUS_DETECTED;
			flagSTDRetvalIsDefined = 1;
		}
		//if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "infected: %s\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		//}
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;
	
	case AVP_CALLBACK_OBJECT_PACKED:
		nSTDPackedFiles ++;
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "packed: %s\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_OBJECT_ARCHIVE:
		nSTDArchivedFiles ++;
		if(RDFlags & RDF_ARCHIVE_DETECTED) {
			sprintf(p_sSTDBuffer, "file %s, archive: %s\n", p_sSTDFullArchivePath, If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		break;
	
	case AVP_CALLBACK_OBJECT_WARNING:
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "warning: %s\n" ,If_MakeFullName(tempb,ScanObject->WarningName));
			LogString(p_sSTDBuffer);
		}
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;

	case AVP_CALLBACK_OBJECT_SUSP:
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "suspicion: %s\n" ,If_MakeFullName(tempb,ScanObject->SuspicionName));
			LogString(p_sSTDBuffer);
		}
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;

	case AVP_CALLBACK_OBJECT_UNKNOWN:
		ScanObject->RFlags|=RF_REPORT_DONE;
		if(nSTDMainRetVal < RETVAL_ENGINE_ERROR) {
			nSTDMainRetVal = RETVAL_ENGINE_ERROR;
			flagSTDRetvalIsDefined = 1;
		}
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "%s: unknown format\n", If_MakeFullName(tempb,ScanObject->VirusName));
			LogString(p_sSTDBuffer);
		}
		break;
	
	case AVP_CALLBACK_OBJECT_MESS:
		if(RDFlags & RDF_INFO) {
			sprintf(p_sSTDBuffer, "%s\n", (LPCTSTR)ScanObject->VirusName);
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_LINK_NAME:
		if(p_sObjName) delete p_sObjName;
		p_sObjName = new char[strlen((char *)lParam) + 1];
		strcpy(p_sObjName, (char *)lParam);
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
		if(RDFlags & RDF_FILE_ERROR) {
			sprintf(p_sSTDBuffer, "Can't open file: %s\n", (char*)lParam);
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_IO_ERROR) {
				nSTDMainRetVal = RETVAL_IO_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
		}
		return 1;

	case AVP_CALLBACK_ERROR_SET:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Wrong set file: %s\n", (char*)lParam);
			LogString(p_sSTDBuffer);
		}
		return 1;

	case AVP_CALLBACK_ERROR_BASE:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Old or corrupted antiviral base\n");
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_BASES_ERROR) {
				nSTDMainRetVal = RETVAL_BASES_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
		}
		break;

	case AVP_CALLBACK_ERROR_KERNEL_PROC:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Base kernel proc absent\n");
			LogString(p_sSTDBuffer);
			if(nSTDMainRetVal < RETVAL_ENGINE_ERROR) {
				nSTDMainRetVal = RETVAL_ENGINE_ERROR;
				flagSTDRetvalIsDefined = 1;
			}
		}
		break;

	case AVP_CALLBACK_ERROR_LINK:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Object code not linked (%s)\n", lParam ? (char*)lParam : "");
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_ERROR_SYMBOL_DUP:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Duplicate symbol %s (%s)\n", (char*)lParam, (const char*)p_sObjName);
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_ERROR_SYMBOL:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Symbol %s not defined (%s)\n", (char*)lParam, (const char*)p_sObjName);
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_ERROR_FIXUPP:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Symbol %s fixupp error (%s)\n", (char*)lParam, (const char*)p_sObjName);
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
		if(RDFlags & RDF_ERROR) {
			sprintf(p_sSTDBuffer, "ERROR: Authentication failed.\n");
			LogString(p_sSTDBuffer);
		}
		break;

	case AVP_CALLBACK_SAVE_PACKED:
//		if(!(MFlags & MF_PACKED)) break;
		// sprintf(p_sSTDBuffer, ".%d.unp\n", ((CWorkArea*)lParam)->ScanObject->InPack+1);
		goto sav;
		break;

	case AVP_CALLBACK_SAVE_ARCHIVED:
//		if(!(MFlags & MF_ARCHIVED)) break;
		// sprintf(p_sSTDBuffer, ".%s\n",((CWorkArea*)lParam)->ZipName);
sav:
		{
			CWorkArea* WorkArea = (CWorkArea*)lParam;
			HANDLE Handle = WorkArea->Handle;	

			DWORD size = GetFileSize(Handle,0);
			if(size == -1) {
				Handle = ((TFileData*)(WorkArea->Handle))->Handle;
				size = ((TFileData*)(WorkArea->Handle))->CurrLen;
			}
			if(size == -1) break;

			char *p_sFullArchivePath = new char[strlen(p_sSTDFullArchivePath) + 1];
			strcpy(p_sFullArchivePath, p_sSTDFullArchivePath);
/*DEBUG*/	printf("\nENTRY: %s\n", p_sFullArchivePath);

			unsigned i, k, iter, z;
			
			// generating short filename
			for(k=strlen(p_sFullArchivePath);
					p_sFullArchivePath[k] != '/' &&
					p_sFullArchivePath[k] != '\\';
				k--);
			char *p_sShortFileName = new char[strlen(p_sFullArchivePath) - k];
			strcpy(p_sShortFileName, p_sFullArchivePath + k + 1);

			// replacing / to \ 
			for(iter=0; iter < strlen(p_sFullArchivePath); iter++) {
				if(p_sFullArchivePath[iter] == '/')
					p_sFullArchivePath[iter] = '\\';
			}
			i = strlen(p_sSTDInput);
			iter = i;

			char *p_sSubTree = NULL;
			char *p_sNewDir = NULL;
			int flagSaveInRoot = 0;

			while(iter < strlen(p_sFullArchivePath)) {
				while(iter < strlen(p_sFullArchivePath) && p_sFullArchivePath[iter] != '\\') {
					iter ++;
				}
				if(	p_sFullArchivePath[iter] == '\\') {
					if(p_sSubTree) delete p_sSubTree;
					p_sSubTree = new char [strlen(p_sSTDOutputDir) + iter - i + 2];
					strncpy(p_sSubTree, p_sFullArchivePath + i, iter - i + 1);
					for(unsigned z=0; z<iter-i+1; z++) {
						if(	p_sSubTree[z] == '*'  || p_sSubTree[z] == '?' ||
							p_sSubTree[z] == ':'  || p_sSubTree[z] == '.' ||
							p_sSubTree[z] == '|'  || p_sSubTree[z] == ' ' ||
							p_sSubTree[z] == '<'  || p_sSubTree[z] == '>' ||
							p_sSubTree[z] == '\"') {
							p_sSubTree[z] = '_';
						}
					}
					p_sSubTree[iter - i] = '\x0';
					if(p_sNewDir) delete p_sNewDir;
					p_sNewDir = new char [strlen(p_sSTDOutputSubDir) + strlen(p_sSubTree) + 1];
					strcpy(p_sNewDir, p_sSTDOutputSubDir);
					strcat(p_sNewDir, p_sSubTree);

					if(_access(p_sNewDir, 0) == -1) {
						// if object does not exist
						if(!CreateDirectory(p_sNewDir, NULL)) {
							flagSaveInRoot = 1;
							break;
						}else {
/*DEBUG*/					//printf("CREATED DIR: %s\n", p_sNewDir);
						}
					}else {
						// if object exists
						WIN32_FIND_DATA fd;     
						HANDLE H = FindFirstFile(p_sNewDir, &fd);
						FindClose(H);
						if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
							flagSaveInRoot = 1;
							break;
						}
					}
				}
				iter ++;
			}
			for(z=0; z<strlen(p_sShortFileName); z++) {
				if(	p_sShortFileName[z] == '*'  || p_sShortFileName[z] == '?'  ||
					p_sShortFileName[z] == ':'  || p_sShortFileName[z] == '\\' ||
					p_sShortFileName[z] == '/'  || p_sShortFileName[z] == '|'  ||
					p_sShortFileName[z] == '<'  || p_sShortFileName[z] == '>'  ||
					p_sShortFileName[z] == '\"') {
					p_sShortFileName[z] = '_';
				}
			}

			//int nInPack = ((CWorkArea*)lParam)->ScanObject->InPack+1;
			//char sInPack[20];
			//itoa(nInPack, sInPack, 10);

			char p_sFullPathName[64*1024];
			if(flagSaveInRoot) {
				// p_sFullPathName = new char[strlen(p_sSTDOutputDir) + 1 + strlen(p_sFullArchivePath) + 1];
				strcpy(p_sFullPathName, p_sSTDOutputDir);
				strcat(p_sFullPathName, "\\");
				int nLastDot = strlen(p_sFullArchivePath) - 1;
				while(nLastDot > 0 && p_sFullArchivePath[nLastDot] != '.')
					nLastDot--;
				// nLastDot points to a last dot in full archive path
				for(unsigned int i=0; i<strlen(p_sFullArchivePath); i++) {
					if(	p_sFullArchivePath[i] == '*'  || p_sFullArchivePath[i] == '?'  ||
						p_sFullArchivePath[i] == ':'  || p_sFullArchivePath[i] == '.'  ||
						p_sFullArchivePath[i] == '|'  || p_sFullArchivePath[i] == ' '  ||
						p_sFullArchivePath[i] == '<'  || p_sFullArchivePath[i] == '>'  ||
						p_sFullArchivePath[i] == '\"' || p_sFullArchivePath[i] == '\\' ||
						p_sFullArchivePath[i] == '/') {
						p_sFullArchivePath[i] = '_';
					}
				}
				// restore original last dot if it was in full archive path
				if(nLastDot > 0) {
					p_sFullArchivePath[nLastDot] = '.';
				}
				strcat(p_sFullPathName, p_sFullArchivePath);
			}else {
				// p_sFullPathName = new char[strlen(p_sNewDir) + 1 + strlen(p_sShortFileName) + 1];
				strcpy(p_sFullPathName, p_sNewDir);
				strcat(p_sFullPathName, "\\");
				strcat(p_sFullPathName, p_sShortFileName);
			}
			// add suffix [1] or [2] or [3]... if file with the pathname already exists
			MakeUniqueFileName(p_sFullPathName);
			// saving file
			HANDLE f=CreateFile(p_sFullPathName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
			if(f!=INVALID_HANDLE_VALUE) {
				BOOL del=0;
				char* buf=new char[size];
				if(buf) {
					ULONG count;
					SetFilePointer(Handle,0,0,0);
					if(ReadFile(Handle,buf, size, &count, NULL))
						if(WriteFile(f,buf, size, &count, NULL))
							del=0;
					delete buf;
				}
				CloseHandle(f);
				nSTDExtractedFiles ++;
				if(del){
					DeleteFile(p_sFullPathName);
				}else
				{
					if(RDFlags & RDF_FILE_SAVED) {
						char *message = new char[strlen("Archive ") + strlen(p_sSTDFullArchivePath) + strlen(" entry saved as ") + strlen(p_sFullPathName) + 2];
						sprintf(message, "%s %s entry saved as %s\n", uParam == AVP_CALLBACK_SAVE_ARCHIVED ? "Archive" : "Pack", p_sSTDFullArchivePath, p_sFullPathName);
						LogString(message);
						int nInPack = ((CWorkArea*)lParam)->ScanObject->InPack+1;
/*DEBUG*/				sprintf(message, "InPack: %d\n", nInPack);
/*DEBUG*/				LogString(message);
						//delete message;
					}
				}
			}else {
				// creating file error
				sprintf(p_sSTDBuffer, "Error: unable to create file \"%s\"\n", p_sFullPathName);
				LogString(p_sSTDBuffer);
				if(nSTDMainRetVal < RETVAL_CREATE_FILE_ERROR) {
					nSTDMainRetVal = RETVAL_CREATE_FILE_ERROR;
				}
				flagSTDRetvalIsDefined = 1;
			}
			//delete p_sFullPathName;
			//delete p_sNewDir;
			delete p_sSubTree;
			//delete p_sFullArchivePath;
			//delete p_sShortFileName;
		}
		break;

	default:
		break;
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
char* If_MakeFullName(char* tempb, char* VirusName)
{
	AVP_MakeFullName(tempb, VirusName);
	strcat(tempb, " (");
	strcat(tempb, VirusName);
	strcat(tempb, ")");
	return tempb;
}

///////////////////////////////////////////////////////////////////////////////////
void PromptWhenDone()
{
	cprintf("Press any key to continue");
	getch();
	cprintf("\n");
}

///////////////////////////////////////////////////////////////////////////////////
void ShowHowToGetHelp()
{
	printf("\nType AVPUNP /? or AVPUNP /h to get help\n");
}

///////////////////////////////////////////////////////////////////////////////////
void LogString(char *ansi_str)
{
	if(flagSTDOEM) {
		char *oem_str = new char[strlen(ansi_str)+1];
		CharToOem(ansi_str, oem_str);
		printf(oem_str);
		delete oem_str;
	}else {
		printf(ansi_str);
	}
}

///////////////////////////////////////////////////////////////////////////////////
void MakeUniqueFileName(char *filename)
{
	char *p_sNewFileName = new char[strlen(filename) + 1 + 16 + 1 + 1];
	//                                                "[""1234""]""\n"
	strcpy(p_sNewFileName, filename);
	int nLastDot = strlen(filename);
	for(unsigned int i=0; i<strlen(filename); i++) {
		if(filename[i] == '.') {
			nLastDot = i;
		}
	}
	int nSuffix = 0;
	char *p_sSuffix = new char[1 + 10 + 1 + 1];
	while(_access(p_sNewFileName, 0) != -1) {
		// while object with the filename exists
		nSuffix ++;
		strcpy(p_sSuffix, "[");
		itoa(nSuffix, p_sSuffix + 1, 10);
		strcat(p_sSuffix, "]");
		strcpy(p_sNewFileName + nLastDot, p_sSuffix);
		strcat(p_sNewFileName, filename + nLastDot);
	}
	strcpy(filename, p_sNewFileName);
	// delete p_sNewFileName;
	// delete p_sSuffix;
}
