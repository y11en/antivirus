#ifndef __AVE_SQL
#define __AVE_SQL

#include "windows.h"

#define FIELD_TYPE_DWORD   0x00000000
#define FIELD_TYPE_STRING  0x10000000
#define FIELD_TYPE_BINARY  0x20000000
#define FIELD_TYPE_MASK    0xF0000000
#define FIELD_ID_MASK      0x0FFFFFFF

#ifdef AVE_SQL_EXPORTS
#define AVE_SQL_API __declspec(dllexport)
#else
#define AVE_SQL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// initialize library
	AVE_SQL_API BOOL InitDB(void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*));
	
	// shutdown library
	AVE_SQL_API void DoneDB(void);

	// delete all records in database	
	AVE_SQL_API BOOL ClearDatabase(void);
	
	//	
	AVE_SQL_API BOOL AddRecord(DWORD RecordIDH, DWORD RecordIDL);

	// this function add new field into record. 
	// if record is not exist function will create it, the same with field
	AVE_SQL_API BOOL SetField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD dwSize);

	AVE_SQL_API BOOL UpdateField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID pData, DWORD dwSize);
	
	AVE_SQL_API BOOL InsertField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID pData, DWORD dwSize);

	// retrive data from field 
	// memory is allocated by library, caller is responsible to free memory
	AVE_SQL_API BOOL GetField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD* pDataSize);
	AVE_SQL_API BOOL GetFieldCached(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID, PVOID* pData, DWORD* pDataSize, BOOL DoNotAlloc = FALSE);
	
	// delete record from database.
	AVE_SQL_API BOOL DeleteRecord(DWORD RecordIDH, DWORD RecordIDL);
	
	// filter recordset by specified conditions
	// FilterString format is equal to SQL WHERE statement format except fields specified as "FIELD_%X" 
	// example: "(FIELD_10000002='AAA' OR FIELD_10000002 LIKE 'BBB*') AND (FIELD_3<20 OR FIELD_3>40)"
	AVE_SQL_API BOOL SetFilter(CHAR* szFilterString);
	
	// enumerate records in current dataset
	// bFirst == TRUE -> function returns first record in dataset, otherwise (bFirst == FALSE) - next record
	// if result == 0 - false, otherwise DB's RecordID
	AVE_SQL_API DWORD EnumRecords(BOOL bFirst, DWORD* RecordIDH, DWORD* RecordIDL);
	
	// return non-empty fields count of record
	AVE_SQL_API BOOL GetFieldsCount(DWORD RecordIDH, DWORD RecordIDL, DWORD* pCount);
	
	// enumerate fields for specified record
	// bFirst == TRUE -> function returns first field, otherwise (bFirst == FALSE) - next field
	AVE_SQL_API BOOL EnumFields(DWORD RecordIDH, DWORD RecordIDL, BOOL bFirst, DWORD* pFieldID);
	
	// returns TRUE if record exists	
	AVE_SQL_API BOOL IsRecord(DWORD RecordIDH, DWORD RecordIDL);
	
	// returns TRUE if field exists	
	AVE_SQL_API BOOL IsField(DWORD RecordIDH, DWORD RecordIDL, DWORD dwFieldID);
		
#ifdef __cplusplus
}
#endif

#endif	//__AVE_SQL