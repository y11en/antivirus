#include <windows.h>
extern "C" {
#include "unzip.h"
#include "windll/structs.h"
#include "windll/decs.h"
}
#include <stdio.h>

int (WINAPI dprint) (LPSTR, unsigned long){return 0;}
int (WINAPI dpassword) (LPSTR, int, LPCSTR, LPCSTR){return 0;}
int (WINAPI dservice) (LPCSTR, unsigned long){return 0;}
void (WINAPI dsound) (void){}
int (WINAPI dreplace)(LPSTR){return 0;}
void (WINAPI dmessage)(unsigned long, unsigned long, unsigned,
   unsigned, unsigned, unsigned, unsigned, unsigned,
   char, LPSTR, LPSTR, unsigned long, char){}


int UnZip (const char *zipfile, const char*outdir)
{
    DCL dcl;

    dcl.ExtractOnlyNewer = 0;
    dcl.SpaceToUnderscore = 0;
    dcl.PromptToOverwrite = 0;
    dcl.fQuiet = 2;
    dcl.ncflag = 0;
    dcl.ntflag = 0;
    dcl.nvflag = 0;
    dcl.nfflag = 0;
    dcl.nzflag = 0;
    dcl.ndflag = 1;
    dcl.ndflag = 1;
    dcl.noflag = 1;
    dcl.naflag = 0;
    dcl.nZIflag = 0;
    dcl.C_flag = 1;
    dcl.fPrivilege = 0;
    dcl.lpszZipFN = (char*)zipfile;
    dcl.lpszExtractDir = (char*)outdir;

    USERFUNCTIONS uf;

    uf.print = dprint;
    uf.sound = dsound;
    uf.replace = dreplace;
    uf.password = dpassword;
    uf.SendApplicationMessage = dmessage;
    uf.ServCallBk = dservice;
    uf.TotalSizeComp = 0;
    uf.TotalSize = 0;
    uf.CompFactor = 0;
    uf.NumMembers = 0;
    uf.cchComment = 0;

    return Wiz_SingleEntryUnzip(0, 0, 0, 0, &dcl, &uf);
}


int UnZipToMemory(const char *zipfile, const char *file, const char *& ptr, int& size )
{
    USERFUNCTIONS uf;

    uf.print = dprint;
    uf.sound = dsound;
    uf.replace = dreplace;
    uf.password = dpassword;
    uf.SendApplicationMessage = dmessage;
    uf.ServCallBk = dservice;
    uf.TotalSizeComp = 0;
    uf.TotalSize = 0;
    uf.CompFactor = 0;
    uf.NumMembers = 0;
    uf.cchComment = 0;

    UzpBuffer buffer;
    int ret = Wiz_UnzipToMemory((char*)zipfile, (char*)file, &uf, &buffer);

    ptr = buffer.strptr;
    size = buffer.strlength;

    return ret;
}

void UnZipFreeMemory(const char *ptr)
{
    UzpBuffer buffer;
    buffer.strptr = (char*)ptr;
    buffer.strlength = 0;
    UzpFreeMemBuffer(&buffer);
}

#define LOCAL_HDR_SIG             0x04034b50

#pragma pack(1)
typedef struct local_file_header {                 
   unsigned long sig;
   unsigned char version_needed_to_extract[2];
   unsigned short general_purpose_bit_flag;
   unsigned short compression_method;
   unsigned long last_mod_dos_datetime;
   unsigned long crc32;
   unsigned long csize;
   unsigned long ucsize;
   unsigned short filename_length;
   unsigned short extra_field_length;
} local_file_hdr;
#pragma pack()

bool UnZipIsFile (const char *zipfile, const char *filename)
{
    bool ret = false;

    FILE * fz = fopen(zipfile,"rb");
    if (!fz) return ret;

    local_file_header lfh;
    long offs = 0;

    while (true)
    {
        if (fseek(fz,offs,SEEK_SET) != 0) break;
        if (fread(&lfh, sizeof(lfh), 1, fz) != 1) break;
        if (lfh.sig != LOCAL_HDR_SIG) break;

        char *name = new char[lfh.filename_length + 1];
        if (!name)
        {
            break;
        }

        if (fread(name, lfh.filename_length, 1, fz) != 1) 
        {
            delete [] name;
            break;
        }

        name[lfh.filename_length] = 0;

        if (_stricmp(name, filename) == 0)
        {
            ret = true;
            delete [] name;
            break;
        }
        delete [] name;

        offs += sizeof(lfh) + lfh.csize + lfh.filename_length+ lfh.extra_field_length;
    }

    fclose(fz);

    return ret;
}

/*
int UnZipGetFileList(const char *zipfile, char **filenames, int files)
{
    FILE * fz = fopen(zipfile);
    if (!fz) return 0;

    for (int i = 0; i < filesmax; ++i)
    {
        files[i] = 0;
    }

    local_file_header lfh;
    long offs = 0;
    int file = 0;

    while (file < files)
    {
        if (fseek(fz,offs,SEEK_SET) != 0) break;
        if (fread(&lfh, sizeof(lfh), 1, fz) != 1) break;
        if (lfh.sig != LOCAL_HDR_SIG) break;

        name = new char[lfh.filename_length + 1];

        if (fread(name, lfh.filename_length, 1, fz) != 1) 
        {
            delete [] name;
            break;
        }

        name[lfh.filename_length] = 0;

        filenames[file++] = name;

        offs += sizeof(lfh) + lfh.compressed_size + lfh.filename_length+ lfh.extra_field_length;
    }

    fclose(fz);

    return file;
}

void UnZipFreeFileList(const char **filenames, int files)
{
    for (int i = 0; i < files; ++i)
    {
        delete [] filenames[i];
    }
}
*/