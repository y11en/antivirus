
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"
{
#include "fpiapi.h"
#include "fpirun.h"
}


#define N_BINS 5

struct statistics
{
    int     arclevel;   // 0 for non-archived
    char    ext[20];     // empty for "others"
    
    long    nFilesTotal;
    DWORD   nTicksTotal;
    
    long    nFilesClass[N_BINS];
    DWORD   nTicksClass[N_BINS];
};

statistics* st = NULL;
int         st_size = 0;
int         st_maxsize = 0;


void extract_extension(const char* name, char* ext, int maxlen);
int find_or_add_type(int arclevel, const char* ext);
int add_type(int arclevel, const char* ext);


void add_statistics(const char* fullname, int arclevel, int bin, int nTicks)
{
    int i;

    if (bin < 0 || bin >= N_BINS)
    {
        printboth("%s: invalid bin number (%d)\n", fullname, bin);
        return;
    }

    char ext[20];
    extract_extension(fullname, ext, 3);
    strupr(ext);

    if (strcmp(ext,"EXE")==0
        || strcmp(ext,"COM")==0
        || strcmp(ext,"DLL")==0
        || strcmp(ext,"SYS")==0
        )
        strcpy(ext,"EXE/COM/DLL/SYS");
    else if (strncmp(ext,"DO",2)==0
        || strncmp(ext,"XL",2)==0
        )
        strcpy(ext,"DO?/XL?");
    else
        strcpy(ext,"Other");

    i = find_or_add_type(arclevel, ext);
    if (i < 0)
        return;

    st[i].nFilesTotal++;
    st[i].nTicksTotal+=nTicks;

    st[i].nFilesClass[bin]++;
    st[i].nTicksClass[bin]+=nTicks;
}


void extract_extension(const char* name, char* ext, int maxlen)
{
    const char* lastdot = strrchr(name, '.');

    if (lastdot == NULL
        || strlen(lastdot+1) > (unsigned)maxlen)
    {
        memset(ext, '#', maxlen);
        ext[maxlen] = '\0';
    }
    else
    {
        strcpy(ext, lastdot+1);
    }
}

int find_or_add_type(int arclevel, const char* ext)
{
    if (st == NULL)
        return add_type(arclevel, ext);

    for (int i=0; i<st_size; i++)
    {
        if (st[i].arclevel == arclevel
            && strcmp(st[i].ext, ext) == 0)
        {
            return i;
        }
    }

    return add_type(arclevel, ext);
}

int add_type(int arclevel, const char* ext)
{
    if (st_size >= st_maxsize)
    {
        // need more space
        void* p = realloc(st, (st_maxsize+20)*sizeof(statistics));

        if (p == NULL)
        {
            printboth("cannot realloc more file types\n");
            return -1;
        }

        st_maxsize += 20;
        st = (statistics*)p;
    }

    st[st_size].arclevel = arclevel;
    strcpy(st[st_size].ext, ext);
    st[st_size].nFilesTotal = 0;
    st[st_size].nTicksTotal = 0;
    for (int i=0; i<N_BINS; i++)
    {
        st[st_size].nFilesClass[i] = 0;
        st[st_size].nTicksClass[i] = 0;
    }

    st_size++;
    return st_size-1;
}

void print_statistics()
{
    int i,j;

    fprintf(statfile, "ArcLev\tExt\tN\ttime");
    for (j=0; j<N_BINS; j++)
    {
        fprintf(statfile, "\tN%d\ttime%d", j, j);
    }
    fprintf(statfile, "\n");

    for (i=0; i<st_size; i++)
    {
        fprintf(statfile, "%d\t%s\t%ld\t%.3f",
            st[i].arclevel,
            st[i].ext,
            st[i].nFilesTotal,
            st[i].nTicksTotal/1000.0);

        for (j=0; j<N_BINS; j++)
        {
            fprintf(statfile, "\t%ld\t%.3f",
                st[i].nFilesClass[j],
                st[i].nTicksClass[j]/1000.0);
        }

        fprintf(statfile, "\n");
    }
}
