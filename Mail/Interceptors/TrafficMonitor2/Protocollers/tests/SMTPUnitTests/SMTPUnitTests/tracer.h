#if !defined(__TRACER_H)
#define __TRACER_H


#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "utils/local_mutex.hpp"


class Tracer
{
public:
	static Tracer& GetTracer ();

	static bool bLoggingEnabled;

	void Out (std::wstring to_out);
	void Out (std::string to_out);
	void Out (std::wstring to_out1, std::wstring to_out2);
	void Out (std::string to_out1, std::string to_out2);
	void Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3);
	void Out (std::string to_out1, std::string to_out2, std::string to_out3);
	void Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3, std::wstring to_out4);
	void Out (std::string to_out1, std::string to_out2, std::string to_out3, std::string to_out4);
	void Out (std::wstring to_out1, std::wstring to_out2, std::wstring to_out3, std::wstring to_out4, std::wstring to_out5);
	void Out (std::string to_out1, std::string to_out2, std::string to_out3, std::string to_out4, std::string to_out5);

	sync::local_mutex& GetMutex ()
	{
		return m_guard;
	};

private:

	sync::local_mutex m_guard;
};

#define LOG_CALL(M1) Tracer::GetTracer().Out(M1);
#define LOG_CALL2(M1,M2) Tracer::GetTracer().Out(M1,M2);
#define LOG_CALL3(M1,M2,M3) Tracer::GetTracer().Out(M1,M2,M3);
#define LOG_CALL4(M1,M2,M3,M4) Tracer::GetTracer().Out(M1,M2,M3,M4);
#define LOG_CALL5(M1,M2,M3,M4,M5) Tracer::GetTracer().Out(M1,M2,M3,M4,M5);


#define DISABLE_LOGGING Tracer::bLoggingEnabled = false;
#define ENABLE_LOGGING Tracer::bLoggingEnabled = true;


#endif