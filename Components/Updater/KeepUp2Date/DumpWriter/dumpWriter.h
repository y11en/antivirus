#ifndef DUMPWRITER_H_INCLUDED_AE5B5802_BB6B_46f8_9B98_B80AA22BD741
#define DUMPWRITER_H_INCLUDED_AE5B5802_BB6B_46f8_9B98_B80AA22BD741

#pragma once


#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <windows.h>


/// call constructor of this class to have dump in case non-handled exceptions
/// Usage notes: write somewhere in your main()
///       MiniDumper::MiniDumper();
class MiniDumper
{
public:
    /// enables write dump in case unhandled exceptions
    static void enable();

    /// bool suppress error dialog box
    static void suppressErrorDialog();

private:
    // you do not need to create an instance of MiniDumper, just call enable()
    MiniDumper();

    /// implementation class
    class DbgHelpWrapper;

    /// writes dump on unhandled exception
    static LONG WINAPI topLevelFilter(_EXCEPTION_POINTERS *);
};


#endif  // #ifndef DUMPWRITER_H_INCLUDED_AE5B5802_BB6B_46f8_9B98_B80AA22BD741