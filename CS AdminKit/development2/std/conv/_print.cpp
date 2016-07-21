
#include "_print.h"

#define BOOST_NO_LOCALE_ISIDIGIT
#include <boost/format.hpp>

using namespace boost;
using namespace std;

void _safe_printer( wostream& s, const wchar_t *format, va_list args )
{
  try {
    wstring _sformat( format );
    string _argfmt;
    wstring::size_type pp = _sformat.find( '%' );
    wstring::size_type ppf;
    // std::wcerr << "****** XFormat: '";
    while ( pp != wstring::npos ) {
      ppf = _sformat.find_first_of( L"diouxXeEfFgGaAcsCSp%", pp + 1 );
      if ( ppf != wstring::npos ) {
        switch ( _sformat.at( ppf ) ) {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            _argfmt += 'I';
            break;
          case 's':
            // _sformat.substr( pp + 1, ppf - pp + 1 ).find_first_of( L"lh" ) != wstring::npos
            // hmm, not process size format modifier "%6ls"?
            if ( _sformat.substr( pp + 1, ppf - pp - 1 ).find( L'l' ) != wstring::npos ) { // "%ls"
              _argfmt += 'S';
            } else { // "%s" "%hs"
              _argfmt += 's';
            }
            break;
          case 'S':
            _argfmt += 'S';
            break;
          case 'c':
            // _sformat.substr( pp + 1, ppf - pp + 1 ).find_first_of( L"lh" ) != wstring::npos
            if ( _sformat.substr( pp + 1, ppf - pp - 1 ).find( L'l' ) != wstring::npos ) { // "%lc"
              _argfmt += 'C';
            } else { // "%c" "%hc"
              _argfmt += 'c';
            }
            break;
          case 'C':
            _argfmt += 'C';
            break;
          case 'e':
          case 'E':
          case 'f':
          case 'F':
          case 'g':
          case 'G':
          case 'a':
          case 'A':
            _argfmt += 'D';
            break;
          case 'p':
            _argfmt += 'P';
            break;
        }
      } else {
        break;
      }
      // std::wcerr << _sformat.at( pp + 1 );
      pp = _sformat.find( '%', ppf + 1 );
    }
    // std::cerr << _argfmt << "'" << std::endl;
    wformat fmt( /* format */ _sformat );
    // std::wcerr << s_out.str();
    // std::wcerr << "****** Format: '" << _sformat << "'" << std::endl;
    string::iterator ii = _argfmt.begin();
    while ( ii != _argfmt.end() ) {
      switch ( *ii ) {
        case 'I':
          fmt % va_arg( args, int );
          break;
        case 's':
        {
          const char *c = va_arg( args, const char * );
          fmt % (c != 0 ? c : "(null)");
        }
        break;
        case 'S':
        {
          const wchar_t *wc = va_arg( args, const wchar_t * );
          fmt % (wc != 0 ? wc : L"(null)");
        }
        break;
        case 'c':
          fmt % (char)va_arg( args, int );
          break;
        case 'C':
          fmt % (wchar_t)va_arg( args, int );
          break;
        case 'D':
          fmt % va_arg( args, double );
          break;
        case 'P':
          fmt % va_arg( args, void * );
          break;
      }
      ++ii;
    }
    s << fmt;
  }                
  catch ( std::exception& err ) {
    s << err.what();
  }
}
