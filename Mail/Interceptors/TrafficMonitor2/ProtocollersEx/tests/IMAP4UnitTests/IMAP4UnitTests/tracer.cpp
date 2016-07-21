#include "tracer.h"
#include <iostream>
#include <algorithm>
#include <fstream>

bool Tracer::bLoggingEnabled = true;

Tracer& Tracer::GetTracer()
{
	static Tracer tracer;
	return tracer;
};

void Tracer::Out (std::string to_out)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::cout<<to_out;
}

void Tracer::Out (std::wstring to_out)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::wcout<<to_out;
}

void Tracer::Out (std::string to_out1, std::string to_out2)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::cout<<to_out1<<to_out2;
}

void Tracer::Out (std::wstring to_out1, std::wstring to_out2)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::wcout<<to_out1<<to_out2;
}

void Tracer::Out (std::string to_out1, std::string to_out2, std::string to_out3)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::cout<<to_out1<<to_out2<<to_out3;
}

void Tracer::Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::wcout<<to_out1<<to_out2<<to_out3;
}

void Tracer::Out (std::string to_out1, std::string to_out2, std::string to_out3, std::string to_out4)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::cout<<to_out1<<to_out2<<to_out3<<to_out4;
}

void Tracer::Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3, std::wstring to_out4)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::wcout<<to_out1<<to_out2<<to_out3<<to_out4;
}

void Tracer::Out (std::string to_out1, std::string to_out2, std::string to_out3, std::string to_out4, std::string to_out5)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::cout<<to_out1<<to_out2<<to_out3<<to_out4<<to_out5;
}

void Tracer::Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3, std::wstring to_out4, std::wstring to_out5)
{
	sync::local_mutex::scoped_lock lock(m_guard);
	std::wcout<<to_out1<<to_out2<<to_out3<<to_out4<<to_out5;
}
