/* $Id$
 *
 * sd_xplatform.h
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef __sd_xplatform_h
#define __sd_xplatform_h

#include <time.h>

#ifdef HAVE_PTHREAD_H
# include <pthread.h>
#endif

#include <sd/defs.h>
#include <log4c/defs.h>
#include <log4c/common_time.h>

#define YY_NO_UNISTD_H

//YY_INPUT used errno. define here to avoid it
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	while ( (result = fread( (char *) buf,1, max_size ,yyin)) < 0 ) \
	{ \
	clearerr(yyin); \
	}
#endif //YY_INPUT

#define FILE_SEP "/"


typedef  long long XP_UINT64;

#define SD_ACCESS_READ(a) access(a,4) /*temp disabe it*/

#define DEFAULT_XP_FILE_EXIST
int xp_file_exist(const char* name);

#define DEFAULT_XP_GETTIMEOFDAY
void xp_gettimeofday(log4c_common_time_t* p,void* reserve);


#if defined(_WIN32) && !defined(WINCE)
#define ENABLE_PLATFORM_WIN32
#include <xplatform/platform-win32.h>
#endif

#if defined(WINCE)
#define ENABLE_PLATFORM_WINCE
#include <xplatform/platform-wince.h>
#endif


/*extern int sd_optind; */
LOG4C_DATA int sd_optind;

extern void getopt_reset(void);

extern int sd_getopt(int argc, char *const *argv, const char *opts);

#ifdef _WIN32
#define SD_GETOPT(a,b,c) sd_getopt(a,b,c)
#define SD_OPTIND sd_optind
#else
#define SD_GETOPT(a,b,c) getopt(a,b,c)
#define SD_OPTIND optind
#endif




int sd_stat_ctime(const char* path, time_t* time);
#define SD_STAT_CTIME(path, time) sd_stat_ctime(path, time)


/* Maybe should be using this for to mean
* MS compiler #if defined(_MSC_VER)
*/
#ifdef _WIN32
#define pthread_t HANDLE
#define pthread_mutex_t HANDLE
#define pthread_attr_t DWORD
#define THREAD_FUNCTION DWORD (WINAPI *)(void *)

/*
* This one not obvious: you would have naturally thought of mapping to
* CreateThread()--turns out that to be safe using CRT functions
* you need to use _begintheadex().
* cf. http://msdn2.microsoft.com/en-us/library/7t9ha0zh.aspx
*  http://groups.google.com/group/comp.os.ms-windows.programmer.win32/browse_thread/thread/86d8624e7ee38c5d/f947ac76cd10f397?lnk=st&q=when+to+use+_beginthreadex&rnum=1#f947ac76cd10f397
*
*/
#define pthread_create(thhandle,attr,thfunc,tharg) \
  (int)((*thhandle=(HANDLE)_beginthreadex(NULL,0,(THREAD_FUNCTION)thfunc,tharg,0,NULL))==NULL)
#define pthread_join(thread, result) \
  ((WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))
#define pthread_exit() _endthreadex(0)
#define pthread_cancel(thread) TerminateThread(thread,0)

#define pthread_mutex_init(pobject,pattr) (*pobject=CreateMutex(NULL,FALSE,NULL))
#define pthread_mutex_lock(pobject) WaitForSingleObject(*pobject,INFINITE)
#define pthread_mutex_unlock(pobject) ReleaseMutex(*pobject)

#define pthread_mutex_destroy(pobject) CloseHandle(*pobject)

#endif


#ifdef __HP_cc
#define inline __inline
#endif

#endif //__sd_xplatform_h
