/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	xor_encoding.hpp
*		\brief	‘ункции кодировани€ и декодировани€ использу€ XOR алгоритмы
*		
*		\author ¬ладислав ќвчарик
*		\date	29.12.2003 19:20:41
*		
*		
*/
#pragma once
///////////////////////////////////////////////////////////////////////////////
namespace crypt
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \brief	XOR кодирование при условии что длина буфера меньше или равна
 *			фрейму ключа. »спользуетс€ оптимизированный цикл Duff's device
 *
 */
inline unsigned char* xor_encode(unsigned char *buffer
								 , unsigned char const *frame
								 , size_t size)
{
	switch(size & 7)
	{
	case 0:
		while(size)
		{
			*buffer++ ^= *frame++; --size;
	case 7: *buffer++ ^= *frame++; --size;
	case 6: *buffer++ ^= *frame++; --size;
	case 5:	*buffer++ ^= *frame++; --size;
	case 4:	*buffer++ ^= *frame++; --size;
	case 3:	*buffer++ ^= *frame++; --size;
	case 2:	*buffer++ ^= *frame++; --size;
	case 1:	*buffer++ ^= *frame++; --size;
		}
	}

	return buffer;
}
///////////////////////////////////////////////////////////////////////////////
//! ќптимизированна€ дл€ скал€рных типов
template<typename T>
inline unsigned char* xor_encode(unsigned char *buffer, T frame)
{
	*reinterpret_cast<T*>(buffer) ^= frame;
	return buffer + sizeof(T);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
/**
 * \brief		XOR кодирование буфера фреймом произвольной длинны
 */
inline unsigned char* xor_encode(unsigned char *bufferStart
								, unsigned char *bufferEnd
								, unsigned char const *frameStart
								, unsigned char const *frameEnd)
{
	size_t frame_size = frameEnd - frameStart;
	size_t last = (bufferEnd - bufferStart) % frame_size;
	for(unsigned char *p = bufferStart
		; p != (bufferEnd - last)
		; p = detail::xor_encode(p, frameStart, frame_size));
	return last > 0
		? detail::xor_encode(bufferEnd - last, frameStart, last), bufferStart
		: bufferStart;
}
///////////////////////////////////////////////////////////////////////////////
/**
 * \brief	XOR кодирование буфера фреймом определенного размера
 *			ќбобщенна€ функци€ дл€ скал€рных типов
 *			“ребует что бы была определена операци€ ^=		
 */
template<typename T>
inline char* xor_encode(void *bufferStart, void *bufferEnd, T frame)
{
	size_t frame_size = sizeof(T);
	unsigned char *begin = reinterpret_cast<unsigned char *>(bufferStart);
	unsigned char *end = reinterpret_cast<unsigned char *>(bufferEnd);
	size_t last = (end - begin) % sizeof(T);
	for(unsigned char *p = begin
		; p != (end - last)
		; p = detail::xor_encode(p, frame));
	void* result =
		last > 0
			? detail::xor_encode(end - last
							, reinterpret_cast<unsigned char const*>(&frame)
							, last), begin
			: begin;
	return static_cast<char*>(result);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace crypt