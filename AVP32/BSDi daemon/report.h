#ifndef __REPORT_H
#define __REPORT_H

#pragma pack (1)

extern char szExtRep[0x400];
// Инициализировать рапорт файл
void reportInit(ULONG flag);
void reportReinit(void);
// Записать строку в рапорт файл
void reportPutS(char *str);
void reportForEach(char* szCurName);
// Завершить работу с рапорт файлом
void reportDone(ULONG flag);

#endif