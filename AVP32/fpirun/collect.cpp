// collect.cpp -- code for copying virus samples to a collection directory

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

extern "C"
{
#include "fpiapi.h"
#include "fpirun.h"
}

#ifdef USE_FM
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack(8)
#else
#pragma pack(push,8)
#endif
#include "fmngm.h"
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack()
#else
#pragma pack(pop)
#endif
#endif

#ifndef min
#define min(x,y) ((x)<(y) ? (x) : (y))
#endif

void collect_sample(const char* target, FPICONTAINERHANDLE chandle, const char* fullname, const char* name, FPIFILEHANDLE handle)
{
#ifndef FSAV

    FPIFILEHANDLE   readhandle      = FPIFILEHANDLE_ERROR;
#ifdef USE_FM 
    HFMFILE         writehandle     = -1;
#else
    FILE*           writehandle     = NULL;
#endif
    const char*     filename;
    int             namepos;
    char            outname[200]    = "";
    DWORD           size;
    BYTE*           buffer          = NULL;
    DWORD           pos             = 0;
    DWORD           bytestocopy;

    if (!options.collect)
        return;

    if (handle == FPIFILEHANDLE_ERROR)
    {
        readhandle = FN_OPEN(chandle, name, FPIFILEIO_READ, 0, 0, 0);
        if (readhandle == FPIFILEHANDLE_ERROR)
        {
            printboth("\n\tCannot collect from %s (FN_OPEN)\n", fullname);
            goto out;
        }
    }
    else
    {
        readhandle = handle;
    }

    size = FN_GETSIZE(readhandle);
    if (size==FPIFILESIZE_ERROR || size==0)
        goto out;

    // Choose a nice name for it
    nCollected++;
    namepos = strlen(name);
    while (namepos>0
        && name[namepos-1] != '\\'
        && name[namepos-1] != ' '
        && name[namepos-1] != '/'
        && name[namepos-1] != ']'
        && name[namepos-1] != ':')
        namepos--;
    filename = name+namepos;
    sprintf(outname, "%s\\%s%03ld_%.50s", collectdirname, target, nCollected, filename);

#ifdef USE_FM
    FMOFSTRUCT openstruct;
    FMFILEINFO fileinfo;
    strcpy(openstruct.pszPassword, "");
    if (!fmOpenFile(&writehandle, 0, outname, FM_CREATE_ACCESS | FM_WRITEONLY_ACCESS, FALSE, FM_SHARE_DENYRW, SKIP_ALL_TYPECHECKS,
        &openstruct, &fileinfo))
    {
        writehandle = -1;
        printboth("\n\tCannot collect to %s (fmOpenFile)\n", outname);
        goto out;
    }
#else
    writehandle = fopen(outname, "wb");
    if (writehandle == NULL)
    {
        printboth("\n\tCannot collect to %s (fopen)\n", outname);
        goto out;
    }
#endif

    // Copy it
#define BUFSIZE 32768U
    buffer = (BYTE*)malloc(BUFSIZE);
    if (buffer == NULL)
    {
        printboth("\n\tCannot collect (malloc)\n");
        goto out;
    }

    if (FN_SETPOS(readhandle, 0, FPIFILEIO_BEGIN) == FPIFILESIZE_ERROR)
    {
        printboth("\n\tCannot collect (FN_SETPOS)\n");
        goto out;
    }

    while (pos < size)
    {
        bytestocopy = min(BUFSIZE, size-pos);
        DWORD bytescopied;

        if (!FN_READ(readhandle, buffer, bytestocopy, &bytescopied)
            || bytescopied < bytestocopy)
        {
            printboth("\n\tCannot collect (FN_READ)\n");
            goto out;
        }

#ifdef USE_FM
        if (!fmWriteFile(writehandle, buffer, bytestocopy, &bytescopied)
            || bytescopied < bytestocopy)
        {
            ulong error = fmGetLastError();
            printboth("\n\tCannot collect (fmWriteFile, %lu)\n", error);
            goto out;
        }
#else
        if (fwrite(buffer, 1, bytestocopy, writehandle) < bytestocopy)
        {
            printboth("\n\tCannot collect (fwrite)\n");
            goto out;
        }
#endif

        pos += bytescopied;
    }

    printlog("\t=> %s\n", outname);

out:
    if (buffer != NULL)
        free(buffer);
#ifdef USE_FM
    if (writehandle != -1)
        fmCloseFile(writehandle);
#else
    if (writehandle != NULL)
        fclose(writehandle);
#endif
    if (readhandle != FPIFILEHANDLE_ERROR)
        FN_CLOSE(readhandle);

#endif // !defined FSAV
}

