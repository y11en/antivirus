#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/diff.h"

#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
# undef N
# include <windows.h>
#endif

// ----------------------------------------------------------------------------

tVOID PrintUsage(tINT ArgC, tCHAR** ArgV)
{
  tCHAR* PName;
  
  if ( 0 == ArgC || NULL == ArgV || NULL == ArgV[0] )
  {
    // predefined name
    PName = "diffmake.exe";
  }
  else
  {
    // get current name
    PName = ArgV[0] + strlen(ArgV[0]);
    while ( PName > ArgV[0] )
    {
      if ( *PName == '\\' || *PName == '/' )
      {
        PName++;
        break;
      }

      PName--;
    }
  }

  printf("Usage: %s <old_file.bin> <new_file.bin> <diff_file.bin> [params]\n",
    PName);
  printf("Params: -s : sign diff-file by tsigner.exe\n");
  printf("        -s\"path\" : sign diff-file by \"path\" file\n");
}

// ----------------------------------------------------------------------------

tBOOL SignDiff(tCHAR* Param, tCHAR* FName)
{
  tCHAR TPath[2048];
  tCHAR SArgs[2048];
  tUINT Cnt;

  if ( *Param != '\0' )
  {
    Cnt = 0;
    while( *Param != '\"' )
    {
      if ( *Param == '\0' )
        break;
      TPath[Cnt++] = *(Param++);
    }
    TPath[Cnt++] = '\0';
  }
  else
  {
    Cnt = 12;
    memcpy(&TPath[0], "tsigner.exe", Cnt);
  }

  memcpy(&SArgs[0], &TPath[0], Cnt);
  strcat(&SArgs[0], " ");
  strcat(&SArgs[0], FName);

#ifdef USE_WIN32_API
  {
    DWORD  EC;
    PROCESS_INFORMATION PI;
    STARTUPINFO SI;

    memset(&PI, 0, sizeof(PI));
    memset(&SI, 0, sizeof(SI));
    if ( cFALSE == CreateProcess(&TPath[0], &SArgs[0], NULL, NULL, FALSE,
      0, NULL, NULL, &SI, &PI) )
    {
      return(cFALSE);
    }

    EC = 0;
    do
    {
      if ( cFALSE == GetExitCodeProcess(PI.hProcess, &EC) )
        return(cFALSE);
      Sleep(500);
    }
    while( STILL_ACTIVE == EC );

    if ( EC != 0 )
      return(cFALSE);
  }
#else // USE_WIN32_API
  printf("Error: tsigner spawning not implemented for non win32 os.\n")
#endif // USE_WIN32_API

  return(cTRUE);
}

// ----------------------------------------------------------------------------

#ifdef _DEBUG
# define DG_DEBUG
#endif

// #define DG_DEBUG

int __cdecl main(int argc, char** argv)
{
  tINT Result;

#ifdef DG_DEBUG
  tCHAR* MArg[5];
  argc = 5;
  argv = (char**)(&MArg);
  MArg[0] = "diffmake.exe";
  MArg[1] = "old.avc";
  MArg[2] = "new.avc";
  MArg[3] = "diff.bin";
  MArg[4] = "-s\"\\\\VINTIK\\VirusLib\\Tools\\Scarabey\\tsigner.exe\"";
//  MArg[0] = "diffmake.exe";
//  MArg[1] = "\\\\VINTIK\\Test\\!CUMULAT\\old\\x-files.avc";
//  MArg[2] = "\\\\VINTIK\\Test\\!CUMULAT\\x-files.avc";
//  MArg[3] = "\\\\VINTIK\\Test\\X_DIMA\\diff.bin";
//  MArg[4] = "-s\"d:\\soft\\editor\\tsigner.exe\"";
//  MArg[4] = "-s\"\\\\VINTIK\\VirusLib\\Tools\\Scarabey\\tsigner.exe\"";
#endif

  printf("Diff creator for AVP bases\n");

  if ( argc < 4 || argc > 5 )
  {
    PrintUsage(argc, argv);
    return(DIFF_ERR_PARAM);
  }

  printf("Converting: %s -> %s == %s\n", argv[1], argv[2], argv[3]);
  if ( DIFF_ERR_OK != (Result = MakeDiff(argv[1], argv[2], argv[3])) )
  {
    printf("\nProcessing error: %d\n", Result);
    return(Result); // error here
  }

  // sign needed
  if ( argc == 5 )
  {
    tCHAR* LPtr;

    LPtr = argv[4];
    if ( *LPtr == '-' )
      LPtr++;
    switch(LPtr[0])
    {
    case 's':
    case 'S':
      printf("Signing diff-file... ");
      if ( cFALSE == SignDiff(&LPtr[1], argv[3]) )
      {
        printf("Failed\n");
        return(DIFF_ERR_SIGN);
      }
      printf("Ok\n");
      break;
    }
  }

  printf("\nAll done.\n");
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------




