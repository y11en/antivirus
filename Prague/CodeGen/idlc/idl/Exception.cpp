#include "Exception.hpp"

#ifdef WIN32
#include <windows.h>
#endif

namespace idl {

Exception::Exception(const char* filename, int line, const std::string& format) : _format(format) {
    _context = (boost::format("[%1%,%2%]") % filename % line).str();
    retrieveSystemError();
}

Exception::Exception(const std::string& format) : _format(format) {
    retrieveSystemError();
}

Exception::~Exception() throw() {
}

void Exception::retrieveSystemError() {
#ifdef WIN32
    DWORD error = GetLastError();

    if (error == ERROR_SUCCESS) {
        _systemError.clear();
        return;
    }

    LPVOID lpMsgBuf = NULL;
    if(!FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            0x0409, //MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL)) {
       _systemError = (boost::format("unable to get message for system error %1%") % error).str();
       return;
    }

    std::wstring message = (const wchar_t*)lpMsgBuf;
    int maxBytesNumber = wcstombs(NULL, message.c_str(), 0);
    
    char* data = new char[maxBytesNumber + 1];
    wcstombs(data, message.c_str(), maxBytesNumber + 1);
    
    _systemError = (boost::format("system error %1%: %2%") % error % data).str();
    
    delete[] data; data = NULL;
    
    // Free the buffer.
    LocalFree(lpMsgBuf);
#else
    _systemError.clear();
#endif
}

const char* Exception::what() const throw() {
    _output = _context + ": " + _format.str();
    if (!_systemError.empty()) {
        _output += "; see also " + _systemError;
    }
    return _output.c_str();
}
} //namespace idl
