
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GINA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GINA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef GINA_EXPORTS
#define GINA_API __declspec(dllexport)
#else
#define GINA_API __declspec(dllimport)
#endif

// This class is exported from the gina.dll
class GINA_API CGina {
public:
	CGina(void);
	// TODO: add your methods here.
};

extern GINA_API int nGina;

GINA_API int fnGina(void);

