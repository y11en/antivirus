#pragma once


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSI_SCM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSI_SCM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MSI_SCM_EXPORTS
#define MSI_SCM_API __declspec(dllexport)
#else
#define MSI_SCM_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
// CustomActionData :	Name=<service name>; 
//						Path=<binary path>; 
//						[DoNotQuotePath=yes|no|1|0;]
//						[DisplayName=<display name>;]
//						[Description=<description>;]
//						[StartOnInstall=yes|no|1|0;]
//						ServiceType=kernelDriver|systemDriver|ownProcess|shareProcess;
//						StartType=auto|boot|system|demand|disabled;
//						Dependencies=[<service1>[,<service2>[,...,<serviceN>]]];
//						[ErrorControl=ignore|normal|critical|severe;]
//						[FailureCountResetPeriod=<integer value in seconds>;]
//						[FailureAction1=restart|none[,<delay in seconds>];]
//						[FailureAction2=restart|none[,<delay in seconds>];]
//						[FailureAction3=restart|none[,<delay in seconds>];]
//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CARegisterService(MSIHANDLE hInstall);
//////////////////////////////////////////////////////////////////////////
// CustomActionData :	Name=<service name>
//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAUnregisterService(MSIHANDLE hInstall);
//////////////////////////////////////////////////////////////////////////
// CustomActionData :	Name=<service name>
//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAStartService(MSIHANDLE hInstall);
//////////////////////////////////////////////////////////////////////////
// CustomActionData :	Name=<service name>
//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAStopService(MSIHANDLE hInstall);
