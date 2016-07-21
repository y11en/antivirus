#pragma once

#include "Tracer.h"

//#define ASTRACE(msg)								\
//	do {											\
//	CTracer::Trace1((msg));						\
//	} while(0);
//
//#define ASTRACE_EX(msg)								\
//	do {											\
//	CTracer::Trace2((msg), __FILE__, __LINE__);	\
//	} while(0);

#define ASTRACE(msg)	cPrTracer::Trace1((tSTRING)(msg))

#define ASTRACE_EX(msg)	cPrTracer::Trace2((tSTRING)(msg), __FILE__, __LINE__)

