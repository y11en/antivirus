#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/diff.h"

// #define DG_DEBUG

// ----------------------------------------------------------------------------

#ifdef DG_DEBUG

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

  printf("Usage: %s <old_file.bin> <diff_file.bin> <new_file.bin> \n", PName);
}

#endif // DG_DEBUG

// ----------------------------------------------------------------------------

#ifdef _unix__
int main(int argc, char** argv)
#else
int __cdecl main(int argc, char** argv)
#endif
{
  tINT Result;

#ifdef DG_DEBUG
  tCHAR* MArg[4];
  argc = 4;
  argv = (char**)(&MArg);
  MArg[0] = "diffmake.exe";
  MArg[1] = "old.avc";
  MArg[2] = "diff.bin";
  MArg[3] = "result.bin";
#endif

#ifdef DG_DEBUG
  printf("Diff patcher for AVP bases\n");
#endif

  if ( 4 != argc )
  {
#ifdef DG_DEBUG
    PrintUsage(argc, argv);
#endif
    return(DIFF_ERR_PARAM);
  }

#ifdef DG_DEBUG
  printf("Converting: %s + %s == %s\n", argv[1], argv[2], argv[3]);
#endif
  if ( DIFF_ERR_OK != (Result = ApplyDiff(argv[1], argv[2], argv[3])) )
  {
    printf("\nProcessing error: %d\n", Result);
    return(Result); // error here
  }

#ifdef DG_DEBUG
  printf("\nAll done.\n");
#endif
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------




