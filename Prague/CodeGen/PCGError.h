#ifndef __PCGError_h__
#define __PCGError_h__

#include <winerror.h>

#define FACILITY_PRAGUE_CG 0x800

#define CG_E_IFACE_NAME                 MAKE_HRESULT(11,FACILITY_PRAGUE_CG,1)
#define CG_E_IFACE_TYPE                 MAKE_HRESULT(11,FACILITY_PRAGUE_CG,2)
#define CG_E_IFACE_DEF                  MAKE_HRESULT(11,FACILITY_PRAGUE_CG,3)
#define CG_E_OBJECT_NAME                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,4)
#define CG_E_METHODS                    MAKE_HRESULT(11,FACILITY_PRAGUE_CG,5)
#define CG_E_METHOD_NAME                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,6)
#define CG_E_METHOD_RETURN_TYPE         MAKE_HRESULT(11,FACILITY_PRAGUE_CG,7)
#define CG_E_PARAM_TYPE                 MAKE_HRESULT(11,FACILITY_PRAGUE_CG,8)
#define CG_E_PROP_DEFINITION            MAKE_HRESULT(11,FACILITY_PRAGUE_CG,9)
#define CG_E_PROP_NAME                  MAKE_HRESULT(11,FACILITY_PRAGUE_CG,10)
#define CG_E_PROP_ID                    MAKE_HRESULT(11,FACILITY_PRAGUE_CG,11)
#define CG_E_PROP_TYPE                  MAKE_HRESULT(11,FACILITY_PRAGUE_CG,12)
#define CG_E_STRUCT_NAME                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,13)
#define CG_E_MEMBER_TYPE                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,14)
#define CG_E_MEMBER_NAME                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,15)
#define CG_E_MEMBER_PROP                MAKE_HRESULT(11,FACILITY_PRAGUE_CG,16)
#define CG_E_MEMBER_PROP_FN_RW_COINC    MAKE_HRESULT(11,FACILITY_PRAGUE_CG,17) // сопадение имен ф-ий R & W
#define CG_E_MEMBER_METHOD_TABLE_EMPTY  MAKE_HRESULT(11,FACILITY_PRAGUE_CG,18) // пустая таблица методов 
#define CG_E_INT_METHOD_TABLE_EMPTY     MAKE_HRESULT(11,FACILITY_PRAGUE_CG,19) // пустая таблица системных методов 
#define CG_E_SERIALIZE_PARAMETER        MAKE_HRESULT(11,FACILITY_PRAGUE_CG,20) // (де)сериализация параметров завершилась ошибкой
#define CG_E_DATA_MATCH                 MAKE_HRESULT(11,FACILITY_PRAGUE_CG,21) // данные не подходят для операции
#define CG_E_PROP_WITHOUT_VALUE         MAKE_HRESULT(11,FACILITY_PRAGUE_CG,22) // не задано ни имя поля данных, ни финкции обработки для проперти
#define CG_E_BACKUP_ERROR               MAKE_HRESULT(11,FACILITY_PRAGUE_CG,23)
#define CG_E_RDONLY                     MAKE_HRESULT(11,FACILITY_PRAGUE_CG,24)
#define CG_E_LOAD_ERROR                 MAKE_HRESULT(11,FACILITY_PRAGUE_CG,25)
#define CG_E_PLUGIN_UNDEFINED           MAKE_HRESULT(11,FACILITY_PRAGUE_CG,26)
#define CG_FILE_NAME                    MAKE_HRESULT(11,FACILITY_PRAGUE_CG,27)

// CCodeGen::GetErrorString

#endif // __PCGError_h__