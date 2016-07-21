#ifndef __DBF_DEFS_H
#define __DBF_DEFS_H

#define		SIZE_VIRUS_NAME		16


#define		_TSR_			0x0001

#define		_MBR_			0x0002
#define		_BOOT_			0x0004

#define		_COM_			0x0008
#define		_EXE_			0x0010
#define		_SYS_			0x0020

#define		_JMP_			0x0040

#define		_EMPTY_			0x0080

#define		_WIN_			0x0200
#define		_CA_			0x1000

#define		_SECTOR_		(_MBR_|_BOOT_)
#define		_FILE_			(_COM_|_EXE_|_SYS_|_WIN_)


#define		_HEADER			0
#define		_PAGE_A			1
#define		_PAGE_B			2
#define		_PAGE_C			3
#define		_DUMMY			4

#define		_CURE_			0
#define		_TEST_			2

// Methods of find and remove viruses

#define		_FILE_MOV			0
#define		_FILE_LEH			1
#define		_FILE_JER			2
#define		_FILE_STA			3
#define		_FILE_EXE_CISS		4
#define		_FILE_EXE_CISS_10	5
#define		_FILE_EXE_CIS		6
#define		_FILE_EXE_CIS_10		7
#define		_FILE_EXE_CI			8
#define		_FILE_EXE_CI_10		9
#define		_FILE_SYS_SI			10
#define		_FILE_SYS_I			11
#define		_FILE_DELETE			12
#define		_FILE_SPC			13
#define		_FILE_FAIL			14

#define		_SECTOR_ADDRESS		0
#define		_SECTOR_ABSOLUTE		1
#define		_SECTOR_LOGICAL		2
#define		_SECTOR_DELETE		3
#define		_SECTOR_SPC			4
#define		_SECTOR_FAIL			5

#define		_JMP_OFFSET			0
#define		_JMP_ADDRESS			1
#define		_JMP_DATA			2
#define		_JMP_EXE				3
#define		_JMP_SYS				4
#define		_JMP_SPC				5

#define		_TSR_ADDRESS			0
#define		_TSR_CUT				1
#define		_TSR_MCB				2
#define		_TSR_TRACE			3
#define		_TSR_SCAN			4
#define		_TSR_FULL_SCAN		5
#define		_TSR_SPC				6


#endif // __DBF_DEFS_H
