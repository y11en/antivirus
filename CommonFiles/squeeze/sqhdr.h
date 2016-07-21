#ifndef __SQHDR_H__
#define __SQHDR_H__

typedef unsigned long DWORD;

namespace Squeeze 
{

    const DWORD dwSqueezeHeaderSignature = 'EZQS';

    struct CSqueezeHeader
    {
	    DWORD dwSignature;
	    DWORD dwVersion;
	    DWORD dwHeaderSize;
	    DWORD dwOriginalSize;
	    DWORD dwCompressedSize;
	    DWORD dwCrc;
    };

    const DWORD dwFileSignature = 'ELIF';

    struct CSqueezeFile
    {
	    DWORD dwSignature;
	    DWORD dwFileSize;
	    DWORD dwFilenameSize;
    };

};

#endif //__SQHDR_H__