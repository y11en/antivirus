#ifdef __cplusplus
#      define C extern "C"
#else
#      define C
#endif

C int     DLL_GetMethodCount(int Object);
C char**  DLL_GetMethodText(int Object,int No);
// Object:      0 - Jmp, 1 - File, 2 - Sector, 3 - Mem
// No:          Method No

#define MEM_SPC				0
#define MEM_ADDRESS			1
#define MEM_CUT				2
#define MEM_MCB				3
#define MEM_TRACE			4
#define MEM_SCAN			5
#define MEM_FULL_SCAN		6

#define MEM_W95_ADDR		7	// Win95
#define MEM_W95_IFS_API		8	// Win95
#define MEM_W32_API			9	// Win95, NT

#define JMP_SPC				0
#define JMP_OFFSET			1
#define JMP_ADDRESS			2
#define JMP_DATA			3
#define JMP_FAR_A			4
#define JMP_FAR_B			5
#define JMP_FAR_AB			6

#define FILE_SPC			0
#define FILE_FAIL			1
#define FILE_DELETE			2

#define FILE_MOVE			3
#define FILE_LEHIGH			4
#define FILE_JERU			5
#define FILE_START			6
#define FILE_SYS_SI			7

#define FILE_COM_3BYTES		8
#define FILE_COM_4BYTES		9
#define FILE_COM_JMP		10

#define FILE_E_CISS			11
#define FILE_E_CIS			12
#define FILE_E_CI			13

#define FILE_E_CISS_10		14
#define FILE_E_CIS_10		15
#define FILE_E_CI_10		16

#define FILE_E_CISS_MIN		17
#define FILE_E_CIS_MIN		18
#define FILE_E_CI_MIN		19
#define FILE_E_CISS_PLS		20
#define FILE_E_CIS_PLS		21
#define FILE_E_CI_PLS		22
#define FILE_E_CISS_FIX		23
#define FILE_E_CIS_FIX		24
#define FILE_E_CI_FIX		25

#define FILE_E_HDR_2EXE		26
#define FILE_E_HDR_CS_IP	27
#define FILE_E_HDR_CS_IP2	28

#define FILE_OLE2			29
#define FILE_SCRIPT			37	// NOTE - this is last No

#define FILE_PE_FIX_STRUCT	30	// just fixes PE structure // after cure()
#define FILE_PE_CUT_SCT		31	// moves EIP and erases last section  - Boza like
#define FILE_PE_CUT_SCT_SUB	32	// moves -EIP and erases last section
#define FILE_PE_LST_SCT		33	// moves EIP and cuts last section - Anxiety like
#define FILE_PE_LST_SCT_SUB	34	// moves -EIP and cuts last section
#define FILE_PE_OFFS_SCTCUT	35	// moves EIP by OFFS and cuts last section
#define FILE_PE_OFFS		36	// moves EIP by OFFS

#define SECT_SPC			0
#define SECT_FAIL			1
#define SECT_OVERWRITE		2

#define SECT_ADDRESS		3
#define SECT_ABSOLUTE		4
#define SECT_ABSOLUTE_CL	5
#define SECT_LOGICAL		6
#define SECT_ACTIVE_BOOT	7

#define SECT_CONSTANT		8			// floppy disks

