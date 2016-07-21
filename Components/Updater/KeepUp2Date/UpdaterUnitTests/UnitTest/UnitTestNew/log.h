#ifndef LOG_H
#define LOG_H

#include "include/log_iface.h"

class TestLog: public KLUPD::Log
{
public:
	
	virtual void print(const char *format, ...);

private:

	void reallyPrint (const char *format, va_list ap);

};





#endif