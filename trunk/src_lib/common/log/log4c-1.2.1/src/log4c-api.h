/* $Id$
 *
 * log4c.h
 *
 * Copyright 2001-2002, Meiosys (www.meiosys.com). All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */


#ifndef _log4c_api_h
#define _log4c_api_h

#include <stdarg.h>
#include <log4c/defs.h>

__LOG4C_BEGIN_DECLS


/**
* Predefined Levels of priorities. These correspond to the priority levels
* used by syslog(3).
**/
typedef enum {
	/** fatal */	LOG4C_PRIORITY_FATAL	= 000, 
	/** alert */	LOG4C_PRIORITY_ALERT	= 100, 
	/** crit */	    LOG4C_PRIORITY_CRIT		= 200, 
	/** error */	LOG4C_PRIORITY_ERROR	= 300, 
	/** warn */	    LOG4C_PRIORITY_WARN		= 400, 
	/** notice */	LOG4C_PRIORITY_NOTICE	= 500, 
	/** info */	    LOG4C_PRIORITY_INFO		= 600, 
	/** debug */	LOG4C_PRIORITY_DEBUG	= 700,
	/** trace */	LOG4C_PRIORITY_TRACE	= 800,
	/** notset */	LOG4C_PRIORITY_NOTSET	= 900,
	/** unknown */	LOG4C_PRIORITY_UNKNOWN	= 1000
} log4c_priority_level_t;


LOG4C_API int log4c_init(void);

LOG4C_API int log4c_fini(void);

LOG4C_API void log4c_category_log_name(const char* category_name,
								  int a_priority,
								  const char* a_format,
								  ...);

LOG4C_API void log4c_category_vlog_name(const char* category_name,
									   int a_priority,
									   const char* a_format,
									   va_list a_args);
__LOG4C_END_DECLS

#endif //_log4c_api_h


