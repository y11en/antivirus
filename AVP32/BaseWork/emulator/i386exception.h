/*
 *  i386exception.h,v 1.2 2002/10/17 09:55:03 tim 
 */

#ifndef I386EXCEPTION_H
#define I386EXCEPTION_H

class CPUexception
{
	public:
        CPUexception (int number);

	private:
		int number;
};



#endif
