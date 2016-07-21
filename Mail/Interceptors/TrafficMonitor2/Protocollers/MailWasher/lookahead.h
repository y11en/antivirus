#ifndef _LOOKAHEAD_H_
#define _LOOKAHEAD_H_

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

////////////////////////////////////////////////////////////////
// lookahead character stream

class	lookahead_stream {
public:
	lookahead_stream( int sz=10 ) : is(NULL),buf(NULL),pos(0),size(0),pos_in_io(0L),buf_size(0)
	{ 
		buf = new unsigned char[sz+1]; 
		if ( buf )
		{
			memset(buf, 0, sz+1); size=sz; 
		}
	}
	~lookahead_stream()
		{ clear(); if(buf!=NULL) { delete []buf; buf=NULL; } }
		
	void	open( hIO s )		{ clear(); is=s; }
	void	close()				{ clear(); }
	void	clear()				{ pos=0; is=NULL; pos_in_io=0L; buf_size=0;  }
	tQWORD	get_size()              
	{
		tQWORD qwSize = 0;
		if ( PR_SUCC(is->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT)) )
			return qwSize;
		else
			return 0;
	}
	tBOOL seek(tQWORD new_pos)
	{ 
		if ( get_size() < new_pos )
			return cFALSE;
		//TODO: ”лучшить, использу€ get/unget!
		pos=0; 
		pos_in_io=new_pos; 
		memset(buf, 0, size+1);
		return cTRUE;
	}
	tBOOL compare(const tCHAR* cmp_buf, tDWORD cmp_size)
	{
		if ( !get_size() )
			return cFALSE;
		tBOOL bCompared = cTRUE;
		tDWORD i = 0;
		for ( i = 0; i < cmp_size; i++ )
		{
			if ( cmp_buf[i] != get() )
			{
				bCompared = cFALSE;
				break;
			}
		}
		while ( i-- > 0 ) 
			unget();
		if ( cmp_size )
			unget();
		return bCompared;
	}

	int	get()	
	{ 
		if ( !buf )
			return -1;

		if( !buf[0] || pos >= buf_size )
		{
			tDWORD dwRead = 0;
			cERROR err = errOK;
			if ( PR_SUCC(err= is->SeekRead(&dwRead, pos_in_io, buf, size) ) && dwRead )
			{
				pos = 0;
				buf_size = dwRead;
				pos_in_io += buf_size;
			}
			else
				return -1;
		}
		return buf[pos++]; 
	}

	void unget()
	{ 
		if ( !buf )
			return;

		if ( !pos )
		{
			// “ут нам придетс€ откатитьс€ на один буфер назад.
			// ќткатимс€-ка мы на пол-буфера, чтобы в будущем не выходить за пределы нового буфера
			tDWORD dwRead = 0;
			cERROR err = errOK;
			int _size = buf_size/2;
			tQWORD pos_in_io_new = pos_in_io - buf_size - _size;
			if ( pos_in_io_new < 0 )
				pos_in_io_new = 0L;
			if ( PR_SUCC(err= is->SeekRead(&dwRead, pos_in_io_new, buf, size) ) && dwRead )
			{
				pos = _size;
				buf_size = dwRead;
				pos_in_io = pos_in_io_new + buf_size;
			}
		}

		if ( pos )
			pos--;
	}

protected:
	hIO is;
	unsigned char *buf;
	int	pos;
	int	buf_size;
	int	size;
	tQWORD pos_in_io;
};

#endif//_LOOKAHEAD_H_