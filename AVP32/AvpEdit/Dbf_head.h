#define	  _ID_HEADER		0x562D
#define	  _ID_HEADER_S	0x532D
#define	  _VERSION		0x0100

#define   _ID_RECORD 	0x0A0D
#define   _ID_COMMENT 	0x010D
#define   _ID_LINK	 	0x020D
#define   _ID_CODE	 	0x040D

#pragma pack (push, pack_records)
#pragma pack (1)



struct DBF_HEADER {					// Заголовок файлов баз данных
	WORD	id;
	DWORD	First_Record;	// seek первой записи от начала файла
	WORD	version;
	DWORD		create_date;
	DWORD	  	modify_date;
	WORD	nJRec;
	WORD	nFRec;
	WORD	nSRec;
	WORD	nTRec;
				  };

struct	DBF_RCB	  {					// Data Base File Record Control Block
	WORD	id;				// Идентификатор record'a
	WORD	size;			// Длина записи без RCB
	WORD	key;			//
				  };

struct	DBF_RCBT  {					// Data Base File Record Control Block
	WORD	id;				// Идентификатор record'a
	WORD	size;			// Длина записи
	WORD	key;			//
	WORD	type;			// for acces ->!!
				  };

#pragma pack (pop, pack_records)

