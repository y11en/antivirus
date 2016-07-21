#if !defined(AFX_SHELLHELPER_H__E2700970_C4D1_4A1D_AD1F_6AFE5C7090BB__INCLUDED_)
#define AFX_SHELLHELPER_H__E2700970_C4D1_4A1D_AD1F_6AFE5C7090BB__INCLUDED_

namespace ShellHelper
{
	enum SYSTEM_IMAGES
	{
		SI_FIRST = 0,
		SI_UNKNOWN = 0,			// unknown file type
		SI_DEF_DOCUMENT = 1,	// default document
		SI_DEF_APPLICATION = 2, // default application
		SI_FOLDER_CLOSED = 3,	// Closed folder 
		SI_FOLDER_OPEN = 4,		// Open folder 
		SI_FLOPPY_514 = 5,		// 5 1/4 floppy 
		SI_FLOPPY_35 = 6,		// 3 1/2 floppy 
		SI_REMOVABLE = 7,		// Removable drive 
		SI_HDD = 8,				// Hard disk drive 
		SI_NETWORKDRIVE = 9,	// Network drive 
		SI_NETWORKDRIVE_DISCONNECTED = 10, // network drive offline 
		SI_CDROM = 11,			// CD drive 
		SI_RAMDISK = 12,		// RAM disk 
		SI_NETWORK = 13,		// Entire network 
		SI_RESERVED1 = 14,		// ? 
		SI_MYCOMPUTER = 15,		// My Computer 
		SI_PRINTMANAGER = 16,	// Printer Manager 
		SI_NETWORK_NEIGHBORHOOD = 17,	// Network Neighborhood 
		SI_NETWORK_WORKGROUP = 18,	// Network Workgroup 
		SI_STARTMENU_PROGRAMS = 19,	// Start Menu Programs 
		SI_STARTMENU_DOCUMENTS = 20, // Start Menu Documents 
		SI_STARTMENU_SETTINGS = 21, // Start Menu Settings 
		SI_STARTMENU_FIND = 22, // Start Menu Find 
		SI_STARTMENU_HELP = 23, // Start Menu Help 
		SI_STARTMENU_RUN = 24, // Start Menu Run 
		SI_STARTMENU_SUSPEND = 25, // Start Menu Suspend 
		SI_STARTMENU_DOCKING = 26, // Start Menu Docking 
		SI_STARTMENU_SHUTDOWN = 27, // Start Menu Shutdown 
		SI_SHARE = 28,			// Sharing overlay (hand) 
		SI_SHORTCUT = 29,		// Shortcut overlay (small arrow) 
		SI_PRINTER_DEFAULT = 30, // Default printer overlay (small tick) 
		SI_RECYCLEBIN_EMPTY = 31, // Recycle bin empty 
		SI_RECYCLEBIN_FULL = 32, // Recycle bin full 
		SI_DUN = 33,			// Dial-up Network Folder 
		SI_DESKTOP = 34,		// Desktop 
		SI_CONTROLPANEL = 35,	// Control Panel 
		SI_PROGRAMGROUPS = 36,	// Program Group 
		SI_PRINTER = 37,		// Printer 
		SI_FONT = 38,			// Font Folder 
		SI_TASKBAR = 39,		// Taskbar 
		SI_AUDIO_CD = 40,		// Audio CD 
		SI_RESERVED2 = 41,		// ? 
		SI_RESERVED3 = 42,		// ? 
		SI_FAVORITES = 43,		// IE favorites 
		SI_LOGOFF = 44,			// Start Menu Logoff 
		SI_RESERVED4 = 45,		// ? 
		SI_RESERVED5 = 46,		// ? 
		SI_LOCK = 47,			// Lock 
		SI_HIBERNATE = 48,		// Hibernate 
		SI_LAST = 48			
	};

	// don't forget to DestroyIcon when you no longer need it
	HICON COMMONEXPORT GetShellIcon(SYSTEM_IMAGES nImageIndex);

} // namespace ShellHelper

#endif // !defined(AFX_SHELLHELPER_H__E2700970_C4D1_4A1D_AD1F_6AFE5C7090BB__INCLUDED_)
