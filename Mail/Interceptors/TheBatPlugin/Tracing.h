#pragma once

#include "Tracer.h"

//#define AVTRACE(msg)							\
//	do {										\
//	CTracer::Trace1((msg));						\
//	} while(0);
//
//
//#define AVTRACE_EX(msg)							\
//	do {										\
//	CTracer::Trace2((msg), __FILE__, __LINE__);	\
//	} while(0);

#define AVTRACE(msg)	cPrTracer::Trace1((tSTRING)(msg))

#define AVTRACE_EX(msg)	cPrTracer::Trace2((tSTRING)(msg), __FILE__, __LINE__)