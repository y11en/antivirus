#define PR_IMPEX_DEF

#undef _USE_VTBL
#include <prague.h>
#include <pr_loadr.h>
#include <iface/i_root.h>
#include <iface/i_plugin.h>
#include <iface/i_loader.h>
#include <wchar.h>
#include <pr_cpp.h>
#include <stdio.h>

tERROR test () 
{
  cStringObj str ( L"test/TEST\\TeSt" );
  if ( str.compare ( L"TEST/TEST\\test", fSTRING_COMPARE_CASE_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( L"test\\TEST/TeSt", fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( L"TeSt\\test/tEsT", fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( L"test/TEST\\TeSt1" ) != cSTRING_COMP_LT ) 
    return errNOT_OK;
  if ( str.compare ( L"test/TEST\\TeS" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( L"TEST" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( L"test" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( L"" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( reinterpret_cast <const tWCHAR*> ( 0x0 ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
//****************************************************************
  if ( str.compare ( "TEST/TEST\\test", fSTRING_COMPARE_CASE_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( "test\\TEST/TeSt", fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( "TeSt\\test/tEsT", fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( "test/TEST\\TeSt1" ) != cSTRING_COMP_LT ) 
    return errNOT_OK;
  if ( str.compare ( "test/TEST\\TeS" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( "TEST" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( "test" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( "" ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( reinterpret_cast <const tCHAR*> ( 0x0 ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
//****************************************************************
  if ( str.compare ( cAutoString ( cStringObj ( L"TEST/TEST\\test" ) ), fSTRING_COMPARE_CASE_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"test\\TEST/TeSt" ) ), fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"TeSt\\test/tEsT" ) ), fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"test/TEST\\TeSt1" ) ) ) != cSTRING_COMP_LT ) 
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"test/TEST\\TeS" ) ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"TEST" ) ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"test" ) ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj ( L"" ) ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cAutoString ( cStringObj () ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( 1, 3, cAutoString ( cStringObj ( L"test/TEST" ) ), 1, 3, 0 ) != cSTRING_COMP_EQ )
    return errNOT_OK; 
  if ( str.compare ( 1, 3, cAutoString ( cStringObj ( L"TeSt/TEST" ) ), 1, 3, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK; 
//****************************************************************
  if ( str.compare ( cStringObj ( L"TEST/TEST\\test" ), fSTRING_COMPARE_CASE_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"test\\TEST/TeSt" ), fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"TeSt\\test/tEsT" ), fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"test/TEST\\TeSt1" ) ) != cSTRING_COMP_LT ) 
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"test/TEST\\TeS" ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"TEST" ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"test" ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cStringObj ( L"" ) ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( cStringObj () ) != cSTRING_COMP_GT ) 
    return errNOT_OK;
  if ( str.compare ( 1, 3, cStringObj ( L"test/TEST" ), 1, 3, 0 ) != cSTRING_COMP_EQ )
    return errNOT_OK; 
  if ( str.compare ( 1, 3, cStringObj ( L"TeSt/TEST" ), 1, 3, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE ) != cSTRING_COMP_EQ )
    return errNOT_OK; 
//****************************************************************
  if ( cStringObj () != L"" )
    return errNOT_OK;
  if ( cStringObj () != "" )
    return errNOT_OK;
  if ( cStringObj ( L"" ) != L"" )
    return errNOT_OK;
  if ( cStringObj () != "" )
    return errNOT_OK;
  if ( cStringObj () != cStringObj ( "" ) )
    return errNOT_OK;
  if ( str != str )
    return errNOT_OK;
//****************************************************************
  if ( str.compare ( 0, 2, cStringObj (), 0, 2 ) != cSTRING_COMP_UNDEF )
    return errNOT_OK;
  if ( str.compare ( 0, 2, cAutoString ( cStringObj () ), 0, 2 ) != cSTRING_COMP_UNDEF )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "compare is OK\n");
//****************************************************************
  if ( str.find ( L"TEST TEST TEST TEST TEST TEST TEST", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( L"TEST", 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( L"TEST", 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( L"test", 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( L"TeSt", 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( L"TEST", 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( L"\\TEST/", 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"/tESt\\", 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"\\tESt/", 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( L"test", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( L"\\tESt/", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( L"/tESt\\", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ().find ( L"", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ( L"").find ( L"", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find ( "TEST", 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( "TEST", 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( "test", 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( "TeSt", 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( "TEST", 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( "\\TEST/", 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "/tESt\\", 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "\\tESt/", 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( "test", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( "\\tESt/", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( "/tESt\\", 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ().find ( "", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ( L"").find ( "", 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find ( "TEST", cCP_ANSI, 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( "TEST", cCP_ANSI, 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( "test", cCP_ANSI, 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( "TeSt", cCP_ANSI, 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( "TEST", cCP_ANSI, 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( "\\TEST/", cCP_ANSI, 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "/tESt\\", cCP_ANSI, 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "\\tESt/", cCP_ANSI, 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( "test", cCP_ANSI, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( "\\tESt/", cCP_ANSI, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( "/tESt\\", cCP_ANSI, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( "", cCP_ANSI, 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find ( L"TEST", cCP_UNICODE, 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( L"TEST", cCP_UNICODE, 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( L"test", cCP_UNICODE, 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( L"TeSt", cCP_UNICODE, 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( L"TEST", cCP_UNICODE, 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( L"\\TEST/", cCP_UNICODE, 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"/tESt\\", cCP_UNICODE, 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"\\tESt/", cCP_UNICODE, 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( L"test", cCP_UNICODE, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( L"\\tESt/", cCP_UNICODE, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( L"/tESt\\", cCP_UNICODE, 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( L"", cCP_UNICODE, 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ().find ( L"", cCP_UNICODE, 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ( L"" ).find ( L"", cCP_UNICODE, 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find ( cAutoString ( cStringObj ( L"TEST" ) ).obj (), 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"TEST" ) ).obj (), 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"test" ) ).obj (), 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"TeSt" ) ).obj (), 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"TEST" ) ).obj (), 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"\\TEST/" ) ).obj (), 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"/tESt\\" ) ).obj (), 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"\\tESt/" ) ).obj (), 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"test" ) ).obj (), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"\\tESt/" ) ).obj (), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"/tESt\\" ) ).obj (), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cAutoString ( cStringObj ( L"" ) ).obj (), 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ().find ( cAutoString ( cStringObj ( L"" ) ).obj (), 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( cStringObj ( L"" ).find ( cAutoString ( cStringObj ( L"" ) ).obj (), 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find ( cStringObj ( L"TEST" ), 0, cStringObj::whole, 0 ) != 5 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"TEST" ), 0, cStringObj::whole, fSTRING_FIND_BACKWARD ) != 5 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"test" ), 0, cStringObj::whole, 0 ) != 0 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"TeSt" ), 0, cStringObj::whole, 0 ) != 10 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"TEST" ), 6, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"\\TEST/" ), 0, cStringObj::whole, fSTRING_COMPARE_SLASH_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"/tESt\\" ), 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"\\tESt/" ), 0, cStringObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"test" ), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE) != 10 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"\\tESt/" ), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) != 4 )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"/tESt\\" ), 0, cStringObj::whole, fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE ) != 4 )
    return errNOT_OK;
  if ( str.find ( cStringObj (), 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
  if ( str.find ( cStringObj ( L"" ), 0, cStringObj::whole, 0 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "find is OK\n"); 
//****************************************************************
  if ( str.find_first_not_of ( L"tT", 0, cStringObj::whole ) != 1 )
    return errNOT_OK;
  if ( str.find_first_not_of ( L"tT", 1, 13 ) != 1)
    return errNOT_OK;
  if ( str.find_last_not_of ( L"eE", 0, cStringObj::whole ) != 13 )
    return errNOT_OK;
  if ( str.find_last_not_of ( L"eE", 1, 11 ) != 10 )
    return errNOT_OK;
  if ( str.find_first_of ( L"sS", 0, cStringObj::whole) != 2 )
    return errNOT_OK;
  if ( str.find_first_of ( L"sS", 3, 11) != 7 )
    return errNOT_OK;
  if ( str.find_last_of ( L"\\/", 0, cStringObj::whole ) != 9 )
    return errNOT_OK;
  if ( str.find_last_of ( L"\\/", 1, 8 ) != 4 )
    return errNOT_OK;
  if ( str.find_last_of ( L"", 1, 8 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  if ( str.find_first_not_of ( "tT", 0, cStringObj::whole ) != 1 )
    return errNOT_OK;
  if ( str.find_first_not_of ( "tT", 1, 13 ) != 1)
    return errNOT_OK;
  if ( str.find_last_not_of ( "eE", 0, cStringObj::whole ) != 13 )
    return errNOT_OK;
  if ( str.find_last_not_of ( "eE", 1, 11 ) != 10 )
    return errNOT_OK;
  if ( str.find_first_of ( "sS", 0, cStringObj::whole) != 2 )
    return errNOT_OK;
  if ( str.find_first_of ( "sS", 3, 11) != 7 )
    return errNOT_OK;
  if ( str.find_last_of ( "\\/", 0, cStringObj::whole ) != 9 )
    return errNOT_OK;
  if ( str.find_last_of ( "\\/", 1, 8 ) != 4 )
    return errNOT_OK;
  if ( str.find_last_of ( "", 1, 8 ) != cStringObj::npos )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "find_* are OK\n"); 
//****************************************************************
  tERROR l_err = errOK;
  l_err = str.replace_one ( L"TEST", 0, L"123456" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/123456\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( L"123456", 0, L"TEST" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"t", fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, L"!" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"!es!/!ES!\\!eS!" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"!", 0, L"t" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/tESt\\teSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"teSt", 0, L"TeSt" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/tESt\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"tESt", 0, L"TEST" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( "TEST", 0, "123456" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/123456\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( "123456", 0, "TEST" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "t", fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, "!" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "!es!/!ES!\\!eS!" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "!", 0, "t" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/tESt\\teSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "teSt", 0, "TeSt" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/tESt\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "tESt", 0, "TEST" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"T", 0, L"TT" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TTESTT\\TTeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"TT", fSTRING_FIND_BACKWARD, L"T" );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = errOK;
  l_err = str.replace_one ( L"TEST", 0, L"123456", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/123456\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( L"123456", 0, L"TEST", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"t", fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, L"!", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"!es!/!ES!\\!eS!" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"!", 0, L"t", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/tESt\\teSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"teSt", 0, L"TeSt", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/tESt\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"tESt", 0, L"TEST", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"T", 0, L"TT", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TTESTT\\TTeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( L"TT", fSTRING_FIND_BACKWARD, L"T", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( "TEST", 0, "123456", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/123456\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( "123456", 0, "TEST", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "t", fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, "!", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "!es!/!ES!\\!eS!" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "!", 0, "t", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/tESt\\teSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "teSt", 0, "TeSt", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/tESt\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "tESt", 0, "TEST", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "T", 0, "TT", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TTESTT\\TTeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( "TT", fSTRING_FIND_BACKWARD, "T", cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( cAutoString ( cStringObj ( L"TEST" ) ), 0, cAutoString ( cStringObj ( L"123456" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/123456\\TeSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( cAutoString ( cStringObj ( L"123456" ) ), 0, cAutoString ( cStringObj ( L"TEST" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/TEST\\TeSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"t" ) ), fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, cAutoString ( cStringObj ( L"!" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"!es!/!ES!\\!eS!" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"!" ) ), 0, cAutoString ( cStringObj ( L"t" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/tESt\\teSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"teSt" ) ), 0, cAutoString ( cStringObj ( L"TeSt" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/tESt\\TeSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"tESt" ) ), 0, cAutoString ( cStringObj ( L"TEST" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/TEST\\TeSt" ) )  )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"T" ) ), 0, cAutoString ( cStringObj ( L"TT" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/TTESTT\\TTeSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cAutoString ( cStringObj ( L"TT" ) ), fSTRING_FIND_BACKWARD, cAutoString ( cStringObj ( L"T" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cAutoString ( cStringObj ( L"test/TEST\\TeSt" ) ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( cStringObj ( L"TEST" ), 0, cStringObj ( L"123456" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/123456\\TeSt" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_one ( cStringObj ( L"123456" ), 0, cStringObj ( L"TEST" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/TEST\\TeSt" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"t" ), fSTRING_FIND_BACKWARD | fSTRING_COMPARE_CASE_INSENSITIVE, cStringObj ( L"!" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"!es!/!ES!\\!eS!" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"!" ), 0, cStringObj ( L"t" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str !=  cStringObj ( L"test/tESt\\teSt" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"teSt" ), 0, cStringObj ( L"TeSt" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/tESt\\TeSt" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"tESt" ), 0, cStringObj ( L"TEST" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/TEST\\TeSt" )  )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"T" ), 0, cStringObj ( L"TT" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/TTESTT\\TTeSt" ) )
    return errNOT_OK;
//****************************************************************
  l_err = str.replace_all ( cStringObj ( L"TT" ), fSTRING_FIND_BACKWARD, cStringObj ( L"T" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != cStringObj ( L"test/TEST\\TeSt" ) )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "replace_one and replace_all are OK\n"); 
//****************************************************************
  cStringObj path;
  l_err = path.check_last_slash ();
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path == PATH_DELIMETER_WIDE )
    return errNOT_OK;
//****************************************************************
  l_err = path.set_path ( str );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE )
    return errNOT_OK;
//****************************************************************
  l_err = path.set_path ( cAutoString ( str ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE )
    return errNOT_OK;
//****************************************************************
  l_err = path.set_path ( str.data (), cCP_UNICODE, 0 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE )
    return errNOT_OK;
//****************************************************************
  l_err = path.add_path_sect ( str );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = path.add_path_sect ( cAutoString ( str ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  l_err = path.add_path_sect ( str.data (), cCP_UNICODE, 0 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" PATH_DELIMETER_WIDE "test/TEST\\TeSt" )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "check_last_slash, set_path and add_path_sect are OK\n"); 
//****************************************************************
  l_err = path.toupper ();
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"TEST/TEST\\TEST" PATH_DELIMETER_WIDE "TEST/TEST\\TEST" PATH_DELIMETER_WIDE "TEST/TEST\\TEST" PATH_DELIMETER_WIDE "TEST/TEST\\TEST" )
    return errNOT_OK;  
//****************************************************************
  l_err = path.tolower ();
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( path != L"test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" )
    return errNOT_OK;  
//****************************************************************
  fprintf ( stderr, "toupper and tolower are OK\n"); 
//****************************************************************
  cStringObj substr = path.substr ( 0, cStringObj::whole );
  if ( substr != L"test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" PATH_DELIMETER_WIDE "test/test\\test" )
    return errNOT_OK;  
//****************************************************************
  substr = path.substr ( 10, 20 );
  if ( substr != L"test" PATH_DELIMETER_WIDE "test/test\\test" PATH_DELIMETER_WIDE )
    return errNOT_OK; 
//****************************************************************
  fprintf ( stderr, "substr is OK\n"); 
//****************************************************************
  l_err = str.assign ( cStringObj( L"assign from cStringObj" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"assign from cStringObj" )
    return errNOT_OK;  
//****************************************************************
  l_err = str.assign ( cStringObj( L"assign from cStringObj" ), 7, 4 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"from" )
    return errNOT_OK;  
//****************************************************************
  l_err = str.assign ( cAutoString( cStringObj ( L"assign from cAutoString" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"assign from cAutoString" )
    return errNOT_OK; 
//****************************************************************
  l_err = str.assign ( cAutoString( cStringObj ( L"assign from cAutoString" ) ).obj (), 7, 4 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"from" )
    return errNOT_OK; 
//****************************************************************
  l_err = str.assign ( L"assign from tVOID", cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"assign from tVOID" )
    return errNOT_OK;  
//****************************************************************
  fprintf ( stderr, "assign is OK\n"); 
//****************************************************************
  str = L"copy test";
  cStringObj copy;
  l_err = str.copy ( copy );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( copy != L"copy test" )
    return errNOT_OK;  
//****************************************************************  
  l_err = str.copy ( copy, 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( copy != L"py te" )
    return errNOT_OK;
//****************************************************************
  l_err = str.copy ( cAutoString ( copy ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( copy != L"copy test" )
    return errNOT_OK;  
//****************************************************************
  l_err = str.copy (  static_cast <cString*> ( cAutoString ( copy ) ), 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( copy != L"py te" )
    return errNOT_OK;
//****************************************************************    
  tWCHAR w_copy [15];      
  l_err = str.copy ( w_copy, sizeof ( w_copy ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"copy test" ) != w_copy )
    return errNOT_OK;  
//**************************************************************** 
  l_err = str.copy ( w_copy, sizeof ( w_copy ), 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"py te" ) != w_copy )
    return errNOT_OK;
//****************************************************************
  tCHAR c_copy [20];      
  l_err = str.copy ( c_copy, sizeof ( c_copy ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"copy test" ) != c_copy )
    return errNOT_OK;  
//**************************************************************** 
  l_err = str.copy ( c_copy, sizeof ( c_copy ), 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"py te" ) != c_copy )
    return errNOT_OK;
//****************************************************************
  l_err = str.copy ( static_cast <tVOID*> ( w_copy ), sizeof ( w_copy ), cCP_UNICODE );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"copy test" ) != w_copy )
    return errNOT_OK;  
//****************************************************************  
  l_err = str.copy ( w_copy, sizeof ( w_copy ), cCP_UNICODE, 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"py te" ) != w_copy )
    return errNOT_OK;
//****************************************************************
  l_err = str.copy ( static_cast< tVOID*> ( c_copy ), sizeof ( c_copy ), cCP_ANSI );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"copy test" ) != c_copy )
    return errNOT_OK;  
//****************************************************************
  l_err = str.copy ( c_copy, sizeof ( c_copy ), cCP_ANSI, 2, 5 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( cStringObj ( L"py te" ) != c_copy )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "copy is OK\n"); 
//****************************************************************
  str = L"test";
  l_err = str.append( cStringObj ( L" cStringObj" ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj" ) 
    return errNOT_OK;
//****************************************************************
  l_err = str.append( ',' );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj," ) 
    return errNOT_OK;
//**************************************************************** 
  l_err = str.append ( L" tVOID,", cCP_UNICODE, 1000 );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj, tVOID," ) 
    return errNOT_OK;
//****************************************************************
  l_err = str.append( " tCHAR," );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj, tVOID, tCHAR," ) 
    return errNOT_OK;
//****************************************************************  
  l_err = str.append ( L" tWCHAR," );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj, tVOID, tCHAR, tWCHAR," ) 
    return errNOT_OK;
//****************************************************************    
  l_err = str.append( cAutoString ( cStringObj ( L" cString" ) ) );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( str != L"test cStringObj, tVOID, tCHAR, tWCHAR, cString" ) 
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "append is OK\n"); 
//****************************************************************
  if ( str.at ( 6, cCP_UNICODE, 0 ) != L'S' )
    return errNOT_OK;
  if ( str.at ( 6, cCP_ANSI, 0 ) != 'S' )
    return errNOT_OK;
  if ( str.at ( 5, cCP_UNICODE, cSTRING_TO_UPPER ) != L'C' )
    return errNOT_OK;
  if ( str.at ( 5, cCP_ANSI, cSTRING_TO_UPPER ) != 'C' )
    return errNOT_OK;
  if ( str.at ( 6, cCP_UNICODE, cSTRING_TO_LOWER ) != L's' )
    return errNOT_OK;
  if ( str.at ( 6, cCP_ANSI, cSTRING_TO_LOWER ) != 's' )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "at is OK\n"); 
//****************************************************************
  if ( str != (tWCHAR*)str.c_str ( cCP_UNICODE ).ptr () ) 
    return errNOT_OK;
  if ( str != (tCHAR*)str.c_str ( cCP_ANSI ).ptr () ) 
    return errNOT_OK;
  int i = 1000;
  str = "";
  while ( i-- ) 
    str.append ( L"*" );
  cStrBuff l_str_buff ;
  l_str_buff  = str.c_str (cCP_ANSI);
  if ( str != (tCHAR*)l_str_buff.ptr () )
    return errNOT_OK;    
  l_str_buff = str.c_str (cCP_UNICODE);
  if ( str != (tWCHAR*)l_str_buff.ptr () )
    return errNOT_OK;    
//****************************************************************
  fprintf ( stderr, "c_str is OK\n"); 
//****************************************************************
  cStringObj str1;
  str1.at ( 0 );
  str1.clear ();
  l_err = str1.assign ( L"1234567890", cCP_UNICODE ); 
  if ( PR_FAIL ( l_err ) )
    return l_err;
  str1.clean ( 3, 3 );
  if ( str1 != L"1237890" ) 
    return errNOT_OK;
  str1.clean ();  
  if ( !str1.empty () )
    return errNOT_OK;
//****************************************************************
  fprintf ( stderr, "clean is OK\n"); 
//****************************************************************
  return errOK;
}

cRoot* g_root = 0;

int main ()
{
  CPrague aPrague ( 0 );
  if ( !g_root ) {
    fprintf ( stderr, "Can't load Prague\n");
    return -1;
  }
  if ( PR_SUCC ( test () ) )
    fprintf ( stderr, "Test passed\n");
  else
    fprintf ( stderr, "Test failed\n"); 
  return 0;
}
