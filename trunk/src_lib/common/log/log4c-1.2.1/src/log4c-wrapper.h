

#pragma once

//#define WITH_LOG4C

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef WITH_LOG4C
	#include <log4c-api.h>
	#include <stdarg.h>

	#ifndef LOG_CATEGORY_NAME_DEFAULT
		#define LOG_CATEGORY_NAME_DEFAULT "root"
	#endif

	#ifdef LOG_CATEGORY_NAME
		#define LOG_CATEGORY_NAME_REAL  LOG_CATEGORY_NAME
	#else
		#define LOG_CATEGORY_NAME_REAL  LOG_CATEGORY_NAME_DEFAULT
	#endif

	#define LOG_PRIORITY_ERROR	LOG4C_PRIORITY_ERROR
	#define LOG_PRIORITY_WARN	LOG4C_PRIORITY_WARN
	#define LOG_PRIORITY_NOTICE	LOG4C_PRIORITY_NOTICE
	#define LOG_PRIORITY_DEBUG	LOG4C_PRIORITY_DEBUG
	#define LOG_PRIORITY_TRACE	LOG4C_PRIORITY_TRACE

	static void log_proxy(int a_priority, const char* a_format, ...)
	{
		va_list va;
		va_start(va,a_format);
		log4c_category_vlog_name(LOG_CATEGORY_NAME_REAL,a_priority,a_format,va);
		va_end(va);
	}

	#define LOGINIT	log4c_init
	#define LOGFINI	log4c_fini
	#define LOGMSG	log_proxy
//#else
//	#define LOGINIT()	1
//	#define LOGFINI()	1
//	#define LOGMSG(...)	1
//#endif //WITH_LOG4C

#ifdef __cplusplus
}; //extern "C"
#endif

