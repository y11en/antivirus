#pragma pack(1)
struct tagMESSAGEINFO {
	DWORD u_Unk_00;
	DWORD u_Unk_04;
	DWORD u_Unk_08;
	DWORD dwRecordID;				// 10
	DWORD dwFlags;				// 9
	FILETIME ftDateTime1;		// 0
	DWORD u_Unk_1C;
	DWORD dwStreamOffset;		// 6
	char* u_Unk_24;
	char* szSubject1;
	FILETIME ftDateTime2;		// 0
	char* szMsgID;
	char* szSubject2;
	char* u_Unk_3C;
	char* u_Unk_40;
	char* u_Unk_44;
	char* szToHost;
	char* szFromName;
	char* szFromAddr;
	WORD  u_Unk_54;				// 5
	WORD  u_Unk_56;				// 5
	DWORD u_Unk_58; 
	FILETIME ftDateTime3;
	char* szToName;
	char* szToAddr;
	DWORD u_Unk_6C; 
	char* u_Unk_70;
	char* u_Unk_74;
	char* szHost;
	char* szID;	
	char* szAccountName;
	char* szAccountID;
	char* u_Unk_88;
	double u_Unk_8C;
	DWORD u_Unk_94;				// 5
	char* u_Unk_98;
	DWORD u_Unk_9C;
	DWORD u_Unk_A0;
	DWORD u_Unk_A4;
	double u_Unk_A8;
	DWORD u_Unk_B0;				// 3
	char* u_Unk_B4;
};
#pragma pack()
