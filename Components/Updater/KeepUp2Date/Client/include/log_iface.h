#ifndef LOG_IFACE_H_INCLUDED_CBCF2977_6AF6_4ff0_A9C6_DDCF4901903A
#define LOG_IFACE_H_INCLUDED_CBCF2977_6AF6_4ff0_A9C6_DDCF4901903A

#if (defined _WIN32) || (defined WIN32)
    // warning C4275: non dll-interface class used as base for dll-interface class
    #pragma warning( disable : 4275 )
#endif

#include "../helper/comdefs.h"

namespace KLUPD {

// logger interface to deal with trace messages
class KAVUPDCORE_API Log
{
public:
    enum Formatting
    {
        // Note: do not delete the noTimeStampNoTrailingNewLine mode which
         // is used for File Server Enterprise Edition application
        noTimeStampNoTrailingNewLine,
        noTimeStamp,
        withTimeStamp,
    };

    virtual ~Log();

    // pring log message with format string similar to printf()
    // format [in] - message's format string
    // ... [in] pointers to data to be printed, if any
    virtual void print(const char *format, ...) = 0;
    

    // helper function to make string from ellipsis parameters
    //  decorationPrefix [in] - append specified string in the begin of result
    static void makeTraceMessageFromEllipsis(std::string &result, const char *format,
        const Formatting &, const std::string &decorationPrefix, va_list);
};

}   // namespace KLUPD

#define TRACE_MESSAGE(p1) \
  { if (pLog) pLog->print(p1); }
#define TRACE_MESSAGE2(p1,p2) \
  { if (pLog) pLog->print(p1, p2); }
#define TRACE_MESSAGE3(p1,p2,p3) \
  { if (pLog) pLog->print(p1, p2, p3); }
#define TRACE_MESSAGE4(p1,p2,p3,p4) \
  { if (pLog) pLog->print(p1, p2, p3, p4); }
#define TRACE_MESSAGE5(p1,p2,p3,p4,p5) \
  { if (pLog) pLog->print(p1, p2, p3, p4, p5); }
#define TRACE_MESSAGE6(p1,p2,p3,p4,p5,p6) \
  { if (pLog) pLog->print(p1, p2, p3, p4, p5, p6); }


#endif  // #ifndef LOG_IFACE_H_INCLUDED_CBCF2977_6AF6_4ff0_A9C6_DDCF4901903A
