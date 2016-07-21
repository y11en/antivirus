#pragma pack (push, pack_records)
#pragma pack (1)

#define far 

struct	DB_EMPTY_RECORD		{
	WORD	Type;

				};

struct	DB_FILE_RECORD		{			// FILE Data Base definition

	WORD	Type;				//
	BYTE	Name[SIZE_VIRUS_NAME];	// имя вируса

	BYTE	Page_1;				// страница где подсчитывается первая сумма
	WORD	Off_1;				// с какого места 1
	BYTE	Len_1;				// сколько байт 1
	DWORD	Sum_1;				// контрольная сумма 1
	BYTE	Page_2;				// где лежит проверяемый блок 2
	WORD	Off_2;				// с какого смещения 2
	BYTE	Len_2;				// сколько байт 2
	DWORD	Sum_2;				// контрольная сумма 2

	DWORD		Check;		// program CHECK,
	DWORD		Cure;		// program CURE

	BYTE	Cure_Method;		// метод лечения
	BYTE	Cure_Page;			// страница откуда брать данные для лечения
										// если DUMM, то от точки входа
	WORD	Cure_Data_1;			// данные для лечения
	WORD	Cure_Data_2;			// данные для лечения
	WORD	Cure_Data_3;			// данные для лечения
	WORD	Cure_Data_4;			// данные для лечения

	WORD	Cure_Cut;			// на сколько укорачивать
	WORD	Tail;				// длина "хвоста"

							};

struct	DB_SECTOR_RECORD	{			// SECTOR Data Base definition
	WORD	Type;				//
	BYTE	Name[SIZE_VIRUS_NAME];	// имя вируса

	WORD	Off_1;				// с какого места (1)
	BYTE	Len_1;				// сколько байт (1)
	DWORD	Sum_1;				// контрольная сумма (1)
	WORD	Off_2;				// с какого смещения
	BYTE	Len_2;				// сколько байт
	DWORD	Sum_2;				// контрольная сумма
	BYTE	Page_2;				// где лежит проверяемый блок

	DWORD		Check;		// program CHECK,
	DWORD		Cure;		// program CURE

	BYTE	Cure_Method;		// метод лечения, если SPC -> выполняется программа
	BYTE	Cure_Page;			// страница откуда

	WORD	Cure_Addr_A;		// Addr1  - адр. или offs где лежит адр.
	WORD	Cure_Addr_B;		// Addr2  - DH
	WORD	Cure_Offset;		// добавочные к адресу (CX)

				};

struct	DB_JMP_RECORD		{			// JMP Data Base definition

	WORD	Type;				//
	BYTE	Name[SIZE_VIRUS_NAME];	//

	BYTE	Len_1;				// сколько байт (1)
	DWORD	Sum_1;				// контрольная сумма (1)
	WORD	Off_2;				// с какого места (2)
	BYTE	Len_2;				// сколько байт (2)
	DWORD	Sum_2;				// контрольная сумма (2)

	DWORD		Jmp;		// spec. program JMP

	BYTE	Jmp_Method;			// как делать JMP
	WORD	Jmp_Data;			// данные для JMP

							};

struct	DB_TSR_RECORD		{			// TSR Data Base definition
	WORD	Type;				//
	BYTE	Name[SIZE_VIRUS_NAME];	// имя вируса

	DWORD		Find;               // program поиска in memory

	BYTE	Find_Method;		// метод поиска in memory
	WORD	Segment;			// сегмент для ADDR

	WORD	Off_1;				// оффсет
	BYTE	Len_1;				// сколько байт
	DWORD	Sum_1;				// контрольная сумма
	BYTE	Control_Byte;		// контрольный байт

	WORD	Off_2;				// оффсет для лечения
	BYTE	Len_2;				// сколько байт заменять (не более 5) byte
	BYTE	Replace_Bytes[5];	// байты для замены


				};

#pragma pack (pop, pack_records)
