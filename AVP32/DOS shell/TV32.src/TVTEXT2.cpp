/*------------------------------------------------------------*/
/* filename -       tvtext2.cpp                               */
/*                                                            */
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

#define Uses_TEditWindow
#define Uses_TFileList
#define Uses_TProgram
#define Uses_MsgBox
#define Uses_TChDirDialog
#define Uses_TFileDialog
#define Uses_TFileInfoPane
#define Uses_TSystemError
#define Uses_TDeskTop
#define Uses_TKeys
#include "include\tv.h"

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

static const char altCodes1[] =
    "QWERTYUIOP\0\0\0\0ASDFGHJKL\0\0\0\0\0ZXCVBNM";
static const char altCodes2[] = "1234567890-=";

char getAltChar(ushort keyCode)
{
    if ((keyCode & 0xff) == 0)
        {
        ushort tmp = (keyCode >> 8);

        if( tmp == (kbAltSpace>>8) )
            return '\xF0';      // special case to handle alt-Space

        else if( tmp >= 0x10 && tmp <= 0x32 )
            return altCodes1[tmp-0x10];     // alt-letter

        else if( tmp >= 0x78 && tmp <= 0x83 )
            return altCodes2[tmp - 0x78];   // alt-number

        }
    return 0;
}

ushort getAltCode(char c)
{
    if( c == 0 )
        return 0;

    c = toupper(c);

    if( (unsigned char)c == 0xF0 )
        return kbAltSpace;       // special case to handle alt-Space

    int i;
    for( i = 0; i < sizeof( altCodes1 ); i++)
       if( altCodes1[i] == c )
        return (i+0x10) << 8;

    for( i = 0; i < sizeof( altCodes2); i++)
        if (altCodes2[i] == c)
            return (i+0x78) << 8;

    return 0;
}

const char * near TEditWindow::clipboardTitle = "Clipboard";
const char * near TEditWindow::untitled = "Untitled";

const char * near TFileList::tooManyFiles = "Too many files.";

const char * near TProgram::exitText = "~Alt-X~ Exit";

const char * near MsgBoxText::yesText = "~Y~es";
const char * near MsgBoxText::noText = "~N~o";
const char * near MsgBoxText::okText = "O~K~";
const char * near MsgBoxText::cancelText = "Cancel";
const char * near MsgBoxText::warningText = "Warning";
const char * near MsgBoxText::errorText = "Error";
const char * near MsgBoxText::informationText = "Information";
const char * near MsgBoxText::confirmText = "Confirm";

const char * near TChDirDialog::changeDirTitle = "Change Directory";
const char * near TChDirDialog::dirNameText = "Directory ~n~ame";
const char * near TChDirDialog::dirTreeText = "Directory ~t~ree";
const char * near TChDirDialog::okText = "O~K~";
const char * near TChDirDialog::chdirText = "~C~hdir";
const char * near TChDirDialog::revertText = "~R~evert";
const char * near TChDirDialog::helpText = "Help";
const char * near TChDirDialog::drivesText = "Drives";
const char * near TChDirDialog::invalidText = "Invalid directory";

const char * near TFileDialog::filesText = "~F~iles";
const char * near TFileDialog::openText = "~O~pen";
const char * near TFileDialog::okText = "O~K~";
const char * near TFileDialog::replaceText = "~R~eplace";
const char * near TFileDialog::clearText = "~C~lear";
const char * near TFileDialog::cancelText = "Cancel";
const char * near TFileDialog::helpText = "~H~elp";
const char * near TFileDialog::invalidDriveText = "Invalid drive or directory";
const char * near TFileDialog::invalidFileText = "Invalid file name.";

const char * near TFileInfoPane::pmText = "p";
const char * near TFileInfoPane::amText = "a";
const char * const near TFileInfoPane::months[] =
    {
    "","Jan","Feb","Mar","Apr","May","Jun",
    "Jul","Aug","Sep","Oct","Nov","Dec"
    };

const char * const near TSystemError::errorString[] =
{
    "Disk is write-protected in drive %c",      // 0
    "Critical disk error on drive %c",          // 1
    "Disk is not ready in drive %c",            // 2
    "Unknown command (drive %c)",               // 3
    "Data error (CRC) on drive %c",             // 4
    "Bad request structure length (drive %c)",  // 5
    "Seek error on drive %c",                   // 6
    "Unknown media type in drive %c",           // 7
    "Sector not found on drive %c",             // 8
    "Printer out of paper",                     // 9
    "Write fault on drive %c",                  // 10
    "Read fault on drive %c",                   // 11
    "General failure on drive %c",              // 12
    "Sharing violation on drive %c",            // 13
    "Lock violation on drive %c",               // 14
    "Invalid disk change on drive %c",          // 15
    "FCB unavailable on drive %c",              // 16
    "Sharing buffer overflow on drive %c",      // 17
    "Code page mispatch",                       // 18
    "Out of input",                             // 19
    "Insufficient disk space on drive %c",      // 20

    "Device access error",                      // 21
    "Bad memory image of FAT detected",         // 22
    "Insert diskette in drive %c"               // 23
};

const char * near TSystemError::sRetryOrCancel = "~Enter~ Retry  ~Esc~ Cancel";

const char near TDeskTop::defaultBkgrnd = '\xB0';
