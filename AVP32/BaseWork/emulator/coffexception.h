/*
 *  coffexception.h,v 1.2 2002/10/17 09:55:01 tim 
 */

#ifndef CMD_COFFEXCEPTION_H
#define CMD_COFFEXCEPTION_H


class CoffException
{
	public:
        CoffException (const char * exception, ...);
        const char * GetErr();

	private:
		char text[256];
};


#endif
