#if !defined (__AVPCOMMONIDS_H__)
#define __AVPCOMMONIDS_H__

// Main DataTree files magic
#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#define AVP_SW_MAGIC									(0x77734c4b)//('wsLK')
#else
#define AVP_SW_MAGIC									('wsLK')
#endif

// Serialization Wrapper's IDs
#define AVP_SW_PID_RAWDATA						(0)
#define AVP_SW_PID_PACKER							(1)
#define AVP_SW_PID_CRC_CHECKER				(2)
#define AVP_SW_PID_DATA_TREE					(3)
#define AVP_SW_PID_ENCRYPTER					(4)

#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
# if defined( __cplusplus )
#pragma pack(1)
# endif
# define PACKSTRUCT __attribute ((packed))
#else
#pragma pack(push,1)
# define PACKSTRUCT 
#endif

// ---		         
typedef struct {
        unsigned int	magic;	// Tim: unsigned long could be 64bit under hpux
        unsigned short	version;
        unsigned short	descriptionLength;
//      char description[];
} PACKSTRUCT AVP_KL_Header;

// ---
typedef struct {
        unsigned int	magic;	// Tim: unsigned long could be 64bit under hpux
        unsigned short	id;
} PACKSTRUCT AVP_SW_Header;

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
# if defined( __cplusplus )
#pragma pack()
# endif
#endif


// DataClass IDs
#define AVP_DC_KEY					(0x1)
#define AVP_DC_CONFIG_PATTERN						(0x2)
#define AVP_DC_CONFIG_IMPLEMENTATION		(0x3)
#define AVP_DC_VENDOR_LIST							(0x4)
#define AVP_DC_OPTIONS_PATTERN					(0x5)
#define AVP_DC_OPTIONS_IMPLEMENTATION		(0x6)
#define AVP_DC_CUSTOMIZE_PATTERN				(0x7)
#define AVP_DC_CUSTOMIZE_IMPLEMENTATION (0x8)

// Main AppIDs
#define AVP_APID_GLOBAL		      (0x8)
#define AVP_APID_KERNEL		      (0x9)


// Utilities AppIDs
#define PP_APP_ID											(0x1)  // Property Pattern generator
#define NS_APP_ID											(0x2)	 // Work Area Setup control
#define RP_APP_ID											(0x3)	 // Report Viewer control
#define ST_APP_ID											(0x4)	 // Statistic Viewer control
#define AB_APP_ID											(0x5)	 // About Viewer control

// Application IDs
#define AVP_APID_FIRST								(0x10)
#define AVP_APID_MONITOR_WIN_95	      (0x10) // 16
#define AVP_APID_MONITOR_WIN_NTWS     (0x11) // 17
#define AVP_APID_SCANNER_DOS_LITE     (0x12) // 18
#define AVP_APID_SCANNER_DOS_32	      (0x13) // 19
#define AVP_APID_SCANNER_DOS_16				(0x14) // 20
#define AVP_APID_SCANNER_WIN_95				(0x15) // 21
#define AVP_APID_SCANNER_WIN_NTWS			(0x16) // 22
#define AVP_APID_CONTROLCENTER	      (0x17) //	23
#define AVP_APID_INSPECTOR						(0x18) //	24
#define AVP_APID_UPDATOR							(0x20) //	32
#define AVP_APID_NOVELL								(0x21) //	33
#define AVP_APID_NT_SERVER						(0x22) //	34
#define AVP_APID_OS_2									(0x23) //	35
#define AVP_APID_WEBINSPECTOR					(0x24) //	36
#define AVP_APID_EXCHANGE							(0x25) //	37
#define AVP_APID_LINUX								(0x26) //	38
#define AVP_APID_NCC_SERVER						(0x27) //	39
#define AVP_APID_NCC_CONSOLE					(0x28) //	40
#define AVP_APID_OFFICEGUARD					(0x29) //	41
#define AVP_APID_OFFICEMONITOR	      (0x2a) //	42
#define AVP_APID_OFFICEMAILCHECKER    (0x2b) // 43
#define AVP_APID_FIREWALL							(0x2c) //	44
#define AVP_APID_FREEBSD							(0x2d) //	45
#define AVP_APID_BSDIUNIX							(0x2e) //	46
#define AVP_APID_INVISIMAIL						(0x2f) //	47
#define AVP_APID_LINUXSERVER					(0x30) //	48
#define AVP_APID_UNIX_SCANNER					(0x31) //	49
#define AVP_APID_UNIX_DAEMON					(0x32) //	50
#define AVP_APID_SCRIPT_CHECKER	      (0x33) // 51
#define AVP_APID_RESCUE_DISK					(0x34) //	52
#define AVP_APID_SOLARIS							(0x35) //	53
#define AVP_APID_KAVEXTERNALAPI	      (0x36) // 54
#define AVP_APID_SCANNER_DEERFIELD    (0x37) // 55
#define AVP_APID_REPORT_VIEWER	      (0x38) //	56
#define AVP_APID_DEPLOYMENT_TOOL      (0x39) // 57
#define AVP_APID_SCANNER_LITE					(0x3a) //	58
#define AVP_APID_MONITOR_LITE					(0x3b) //	59
#define AVP_APID_UNIX_MAILSERVER      (0x3c) // 60
#define AVP_APID_UNIX_MAILSERVER_MAILBOX		  (0x3d) // 61
#define AVP_APID_UNIX_MAILSERVER_TRAFIC			  (0x3e) // 62
#define AVP_APID_UNIX_GATEWAY									(0x3f) // 63
#define AVP_APID_UNIX_GATEWAY_TRAFIC					(0x40) // 64
#define AVP_APID_GUARD												(0x41) // 65
#define AVP_APID_EXCHANGE2000_MMC_SNAPIN			(0x42) // 66
#define AVP_APID_EXCHANGE_SERVER							(0x43) //	67
#define AVP_APID_CONTENT_FILTER 							(0x44) //	68
#define AVP_APID_EXCHANGE_2000								(0x45) //	69
#define AVP_APID_EXCHANGE_SERVER_2000					(0x46) //	70
#define AVP_APID_PERSONAL_FIREWALL						(0x47) //	71
#define AVP_APID_UNIX_FILESERVER							(0x48) //	72
#define AVP_APID_GATEWAY_MAILBOX							(0x49) //	73
#define AVP_APID_LINUX_PERSONAL								(0x4a) // 74
#define AVP_APID_UNIX_MAILSERVER_MAILBOX_ADV	(0x4b) // 75
#define AVP_APID_UNIX_MAILSERVER_TRAFFIC_ADV 	(0x4c) // 76
#define AVP_APID_UNIX_FILESERVER_ADV 					(0x4d) // 77
#define AVP_APID_CORE_COMPONENTS     					(0x4e) // 78
#define AVP_APID_PALM_CONDUIT									(0x4f) // 79
#define AVP_APID_MT_KAVEXTERNALAPI						(0x50) // 80
#define AVP_APID_KAVDAEMON_UNIX_LOCAL					(0x51) // 81
#define AVP_APID_KAVDAEMON_UNIX_NETWORK			  (0x52) // 82
#define AVP_APID_UNIX_SPAMFILTER_MAILBOX	    (0x53) // 83
#define AVP_APID_UNIX_SPAMFILTER_TRAFIC       (0x54) // 84
#define AVP_APID_CS_ADMKIT_LOCAL_AGENT        (0x55) // 85
#define AVP_APID_CS_ADMKIT_NETWORK_AGENT      (0x56) // 86
#define AVP_APID_CS_ADMKIT_ADMINISTRATION_SERVER   (0x57) // 87
#define AVP_APID_CS_ADMKIT_ADMINISTRATION_CONSOLE  (0x58) // 88
#define AVP_APID_KAV_FOR_BORDERWARE_MXTREME_MX     (0x59) // 89
#define AVP_APID_KAV_FOR_LOTUS                (0x68) // 104
#define AVP_APID_API_FOR_UNIX_LOCAL           (0x69) // 105
#define AVP_APID_API_FOR_UNIX_NETWORK         (0x6a) // 106
#define AVP_APID_ANTI_SPAM_ISP_MAIL_ADDRESS   (0x6b) // 107
#define AVP_APID_ANTI_SPAM_ISP_TRAFFIC        (0x6c) // 108
#define AVP_APID_ANTI_SPAM_ISP_CPU            (0x6d) // 109
#define AVP_APID_KAV_FOR_INNOMINATE_MGUARD		(0x6e) // 110
#define AVP_APID_ANTI_HACKER_NET					(0x6f) //	111
#define AVP_APID_KAV_FOR_SONICWALL            (0x70) // 112
#define AVP_APID_PERSONAL_HOME_EDITION					(0x71) //	113
#define AVP_APID_PERSONAL_FIREWALL_STEGANOS				(0x72) //	114
#define AVP_APID_SDK_UNPACKER					(0x73) //	115
#define AVP_APID_SDK_UPDATER_MIRROR_SERVERS			(0x74) //	116
#define AVP_APID_ANTI_FISHING			(0x75) //	117
#define AVP_APID_ANTI_BLOCKER			(0x76) //	118
#define AVP_APID_REGISTRY_MONITOR			(0x77) //	119

#endif

