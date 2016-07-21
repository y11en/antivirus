#pragma pack(1)
struct tagFOLDERINFO {
	char* u_Unk_00;
	DWORD u_Unk_04;
	DWORD dwRecordID; // 0
	DWORD dwParentID; // 1
	char* szName; // 2
	char* szDBFile; // 3
	char* u_Unk_18; // 4
	char* u_Unk_1C; // 5
	char* u_Unk_20; // 0x14
	DWORD u_Unk_24; // 6
	DWORD dwMsgCount; // 7
	DWORD u_Unk_2C; // 8
	DWORD u_Unk_30; // 0x1C
	DWORD u_Unk_34; // 0x16
	DWORD u_Unk_38; // 0x17
	DWORD u_Unk_3C; // 0x1B
	char  u_Type; //u_Unk_40; // 9
	char  u_Protocol; //u_Unk_41; // 0x0A
	char  u_Unk_42; // 0x0B
	char  u_Unk_43;
	DWORD u_Unk_44; // 0x0C
	DWORD u_Unk_48; // 0x0D
	DWORD u_Unk_4C; // 0x0E
	DWORD u_Unk_50; // 0x0F
	DWORD u_Unk_54; // 0x10
	DWORD u_Unk_58; // 0x11
	DWORD u_Unk_5C; // 0x18
	DWORD u_Unk_60; // 0x19
	DWORD u_Unk_64; // 0x12
	FILETIME u_Unk_68; // 0x13
	FILETIME u_Unk_70; // 0x15
	DWORD u_Unk_78; // 0x1A
	char  u_Dummy[60];
};
#pragma pack()
