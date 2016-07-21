#define OUTPUTN(s) {_printf s;}
#define OUTPUT(s) {OUTPUTN(s);OUTPUTN(("\n"));}

void _cls();
char* _gets(char*);
int _printf(const char* format, ...);
unsigned short GetConsoleTextAttr();
int SetConsoleTextAttr(unsigned short wAttr);
