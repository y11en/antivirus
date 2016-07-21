
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AVPFPI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AVPFPI_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AVPFPI_EXPORTS
#define AVPFPI_API __declspec(dllexport)
#else
#define AVPFPI_API __declspec(dllimport)
#endif
/*
// This class is exported from the AVPFPI.dll
class AVPFPI_API CAVPFPI {
public:
	CAVPFPI(void);
	// TODO: add your methods here.
};

extern AVPFPI_API int nAVPFPI;

AVPFPI_API int fnAVPFPI(void);

*/