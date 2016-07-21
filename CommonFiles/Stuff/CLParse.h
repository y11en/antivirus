////////////////////////////////////////////////////////////////////
//
//  CLParse.h
//  CommadLine parsing utility class definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __CLPARSE_H__
#define __CLPARSE_H__

#include <windows.h>

//
// class CCmdLineParser
// ~~~~~ ~~~~~~~~
// Command line argument processing class, processes in the form:
//
//  <Name> | {-/}<Option>[{:=}<Value>] ...
//
class CCmdLineParser {
  public:
    enum CKind {
      Start,   // No tokens have been parsed yet
      Name,    // Name type token, has no leading / or -
      Option,  // Option type token. Leading / or - skipped by m_pToken
      Value,   // Value for name or option. Leading : or = skipped by m_pToken
      Done     // No more tokens
    };

    CCmdLineParser( const TCHAR *cmdLine );
   ~CCmdLineParser();

		BOOL GetExecutableName( LPTSTR pszExeName, DWORD dwExeNameLength );
    CKind NextToken( BOOL removeCurrent = FALSE );
    const TCHAR* GetLine() const {return m_pBuffer;}
    void Reset();

    CKind  m_eKind;       // m_eKind of current token
    TCHAR* m_pToken;      // Ptr to current token. (Not 0-terminated, use m_iTokenLen)
    int    m_iTokenLen;   // Length of current token

  private:
    TCHAR* m_pBuffer;     // Command line buffer
    TCHAR* m_pTokenStart; // Actual start of current token
};

#endif  // __CLPARSE_H__
