/*------------------------------------------------------------*/
/* filename -       tfillist.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TFileList member functions                */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TVMemMgr
#define Uses_MsgBox
#define Uses_TFileList
#define Uses_TRect
#define Uses_TSearchRec
#define Uses_TEvent
#define Uses_TGroup
#include "include\tv.h"
#include "include\tvdir.h"

#include <Errno.h>
#include <IO.h>
#include <Stdio.h>
#include <ctype.h>
#include <Assert.h>
#include <Dos.h>
#include <String.h>

TFileList::TFileList( const TRect& bounds,
                      TScrollBar *aScrollBar) :
    TSortedListBox( bounds, 2, aScrollBar )
{
}

TFileList::~TFileList()
{
   if ( list() )
      destroy ( list() );
}

void TFileList::focusItem( short item )
{
    TSortedListBox::focusItem( item );
    message( owner, evBroadcast, cmFileFocused, list()->at(item) );
}

void TFileList::getData( void * )
{
}

void TFileList::setData( void * )
{
}

size_t TFileList::dataSize()
{
    return 0;
}

void* TFileList::getKey( const char *s )
{
static TSearchRec sR;

    if( (getShiftState() & 0x03) != 0 || *s == '.' )
        sR.attr = FA_DIREC;
    else
        sR.attr = 0;
    strcpy( sR.name, s );
    strupr( sR.name );
    return &sR;
}

void TFileList::getText( char *dest, short item, short maxChars )
{
        TSearchRec *f = (TSearchRec *)(list()->at(item));

        strncpy( dest, f->name, maxChars );
        dest[maxChars] = '\0';
    if( f->attr & FA_DIREC )
        strcat( dest, "\\" );
}

void TFileList::handleEvent( TEvent & event )
{
    if( event.what == evMouseDown && event.mouse.doubleClick )
        {
        event.what = evCommand;
        event.message.command = cmOK;
        putEvent( event );
        clearEvent( event );
        }
    else
        TSortedListBox::handleEvent( event );
}

void TFileList::readDirectory( const char *dir, const char *wildCard )
{
    char path[MAXPATH];
    strcpy( path, dir );
    strcat( path, wildCard );
    readDirectory( path );
}

struct DirSearchRec : public TSearchRec
{

    void *operator new( size_t );

};

void *DirSearchRec::operator new( size_t sz )
{
    void *temp = ::operator new( sz );
    if( TVMemMgr::safetyPoolExhausted() )
        {
        delete temp;
        temp = 0;
        }
    return temp;
}

void TFileList::readDirectory( const char *aWildCard )
{
ffblk s;

char path[MAXPATH];
char drive[MAXDRIVE];
char dir[MAXDIR];
char file[MAXFILE];
char ext[MAXEXT];
int res;
DirSearchRec *p;

const unsigned findAttr = FA_RDONLY | FA_ARCH;

    TFileCollection *fileList = new TFileCollection( 5, 5 );

    strcpy( path, aWildCard );
//printf("path=%s\n",path);
    fexpand( path );
    fnsplit( path, drive, dir, file, ext );
//printf("split: %s,%s,%s,%s\n",drive,dir,file,ext);

    res = findfirst( aWildCard, &s, findAttr );
    p = (DirSearchRec *)&p;
    while( p != 0 && res == 0 )
        {
        if( (s.ff_attrib & FA_DIREC) == 0 )
            {
            p = new DirSearchRec;
            if( p != 0 )
                {
                p->attr = s.ff_attrib;
                p->time = s.ff_ftime + (long(s.ff_fdate) << 16);
                p->size = s.ff_fsize;
                strcpy(p->name,s.ff_name);
                fileList->insert( p );
                }
            }
        res = findnext( &s );
        }
    findclose( &s );

    fnmerge( path, drive, dir, "*", ".*" );

    int  upattr = FA_DIREC;
    long uptime = 0x210000uL;
    long upsize = 0;

    res = findfirst( path, &s, FA_DIREC );
    while( p != 0 && res == 0 )
        {
        if ( (s.ff_attrib & FA_DIREC) != 0 ) {
          if ( s.ff_name[0] != '.' ) {
            p = new DirSearchRec;
            if ( p != 0 ) {
              p->attr = s.ff_attrib;
              p->time = s.ff_ftime + (long(s.ff_fdate) << 16);
              p->size = s.ff_fsize;
              strcpy(p->name,s.ff_name);
              fileList->insert( p );
            }
          } else if ( strcmp(s.ff_name,"..") == 0 ) {
            upattr = s.ff_attrib;
            uptime = s.ff_ftime + (long(s.ff_fdate) << 16);
            upsize = s.ff_fsize;
          }
        }
        res = findnext( &s );
        }
    findclose( &s );
    if ( strlen( dir ) > 4 ) {
      p = new DirSearchRec;
      if ( p != 0 ) {
        p->attr = upattr;
        p->time = uptime;
        p->size = upsize;
        strcpy( p->name, ".." );
        fileList->insert( p );
      }
    }
    if( p == 0 )
        messageBox( tooManyFiles, mfOKButton | mfWarning );
    newList(fileList);
    if( list()->getCount() > 0 )
        message( owner, evBroadcast, cmFileFocused, list()->at(0) );
    else
        {
        static DirSearchRec noFile;
        message( owner, evBroadcast, cmFileFocused, &noFile );
        }
}

/*
    fexpand:    reimplementation of pascal's FExpand routine.  Takes a
                relative DOS path and makes an absolute path of the form

                    drive:\[subdir\ ...]filename.ext

                works with '/' or '\' as the subdir separator on input;
                changes all to '\' on output.

*/

static void near squeeze( char *path )
{
    char *
    dest = path;
    char *
    src = path;
    while( *src != 0 )
        {
        if (*src == '.')
          if (src[1] == '.') {
            src += 2;
            if (dest > path) {
              dest--;
              while ((*--dest != '\\')&&(dest > path)) // back up to the previous '\'
                ;
              dest++;         // move to the next position
              }
            }
          else if (src[1] == '\\')
            src++;
          else
            *dest++ = *src++;
        else
          *dest++ = *src++;
        }
    *dest = EOS;                // zero terminator
    dest = path;
    src = path;
    while( *src != 0 )
        {
        if ((*src == '\\')&&(src[1] == '\\'))
          src++;
        else
          *dest++ = *src++;
        }
    *dest = EOS;                // zero terminator
}

void fexpand( char *rpath )
{
char path[MAXPATH];
char drive[MAXDRIVE];
char dir[MAXDIR];
char file[MAXFILE];
char ext[MAXEXT];

    int flags = fnsplit( rpath, drive, dir, file, ext );
    if( (flags & DRIVE) == 0 )
        {
        drive[0] = getdisk() + 'A';
        drive[1] = ':';
        drive[2] = '\0';
        }
    drive[0] = toupper(drive[0]);
    if( (flags & DIRECTORY) == 0 || (dir[0] != '\\' && dir[0] != '/') )
        {
        char curdir[MAXDIR];
        getcurdir( drive[0] - 'A' + 1, curdir );
        // ++ V.Timonin : better more than nothing
        int
        len = strlen(curdir);
        if (curdir[len - 1] != '\\') {
          curdir[len] = '\\';
          curdir[len + 1] = EOS;
          }
        // -- V.Timonin
        strcat( curdir, dir );
        if( *curdir != '\\' && *curdir != '/' )
            {
            *dir = '\\';
            strcpy( dir+1, curdir );
            }
        else
            strcpy( dir, curdir );
        }

    //++ V.Timonin - squeeze must be after '/' --> '\\'
    char *p = dir;
    while( (p = strchr( p, '/' )) != 0 )
        *p = '\\';
    squeeze( dir );
    //-- V.Timonin
    fnmerge( path, drive, dir, file, ext );
//printf("fnmerge %s,%s,%s,%s: %s\n",drive,dir,file,ext,path);
#ifdef __MSDOS__
    strupr( path );
#endif
    strcpy( rpath, path );
}


#ifndef NO_TV_STREAMS
TStreamable *TFileList::build()
{
    return new TFileList( streamableInit );
}
#endif  // ifndef NO_TV_STREAMS


