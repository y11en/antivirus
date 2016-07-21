#ifndef LICENSINGTYPES_H
#define LICENSINGTYPES_H

/** 
 * Kaspersky Licensing namespace. 
 */
namespace KasperskyLicensing
{

#ifdef UNICODE
	#include <wchar.h>
	typedef wchar_t char_t;
	#define TXT(x)	L##x
	#define tstrcmp wcscmp
#else
	typedef char char_t;
	#define TXT(x)	x
	#define tstrcmp strcmp
#endif

/**
 * Platform type.
 */
enum PlatformType
{
	/**
	 * Unknown (future) platforms. 
	 */
	PT_UNKNOWN,

	/**
	 * All Novell platforms. 
	 */
	PT_NOVELL,

	/**
	 * Unix-like platforms. 
	 */
	PT_UNIX_FAMILY,

	/**
	 * Personal platforms: Windows 9x/Me, Windows XP Home Edition. 
	 */
	PT_WIN_PERSONAL,

	/**
	 * Windows workstations: Windows NT for WorkStation,
	 * Windows XP Professional, Windows 2000 Professional. 
	 */
	PT_WIN_WORKSTATION,

	/**
	 * Windows small business platforms: Windows Server 2003 Standard Edition, 
	 * Windows 2000 server, Microsoft Small Business Server, Windows NT Server. 
	 */
	PT_WIN_SERVER_SMB,

	/**
	 * Windows enterprise platforms: Windows Server 2003 Enterprise Edition 
	 * (Data Center Web Edition Storage Edition), Windows 2000 Advanced Server(Data center), 
	 * Windows NT 4.0 Enterprise Edition. 
	 */
	PT_WIN_SERVER_ENTERPRISE
};
	
/**
 * License keys control mode.
 */
enum ControlMode
{
	/**
	 * Control against black list. 
	 */
	CTRL_BLIST_ONLY,

	/**
	 * Check against black list and anti-virus database. 
	 */
	CTRL_BLIST_AND_BASES
};

} // namespace KasperskyLicensing


#endif // LICENSINGTYPES_H