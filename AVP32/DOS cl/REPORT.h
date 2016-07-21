#ifndef __REPORT_H
#define __REPORT_H

//#define report_buf_size 1024            // размер буфера рапорт файла

//extern unsigned char *report_buf;       // указатель на буфер рапорт файла
//extern int            report_hnd;       // handle рапорт файла

extern char szExtRep[0x200];
// Инициализировать рапорт файл
void reportInit(void);
// Записать строку в рапорт файл
void reportPutS(char *str);
void reportForEach(char* szCurName);
// Завершить работу с рапорт файлом
void reportDone(void);

#endif