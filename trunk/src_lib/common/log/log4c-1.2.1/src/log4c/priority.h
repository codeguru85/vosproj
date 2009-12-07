/* $Id$
 *
 * priority.h
 * 
 * Copyright 2001-2003, Meiosys (www.meiosys.com). All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef log4c_priority_h
#define log4c_priority_h

/**
 * @file priority.h
 *
 * @brief The priority class provides importance levels with which one can
 * categorize log messages.
 **/

#include <log4c/defs.h>

__LOG4C_BEGIN_DECLS



/**
 * @param a_priority a numeric value of the priority.
 * @returns the given priority string name.
 **/
LOG4C_API const char* log4c_priority_to_string(int a_priority);

/**
 * @param a_priority_name a priority string name.
 * @returns the given numeric value of the priority.
 **/
LOG4C_API int log4c_priority_to_int(const char* a_priority_name);

__LOG4C_END_DECLS

#endif
