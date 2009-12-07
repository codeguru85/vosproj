
#ifndef _platform_win32_h
#define _platform_win32_h

#ifdef ENABLE_PLATFORM_WIN32

#include <Windows.h>

//#undef DEFAULT_XP_FILE_EXIST

#undef SD_ACCESS_READ
#define SD_ACCESS_READ(a)	_access(a,04)

#undef FILE_SEP
#define FILE_SEP "\\"

#define strncasecmp		strnicmp
#define strcasecmp		stricmp


#define snprintf		_snprintf		
#define unlink			_unlink
#define stricmp			_stricmp
#define strnicmp		_strnicmp
#define strdup			_strdup
#define alloca			_alloca


#if (_MSC_VER < 1500)
# define vsnprintf		_vsnprintf
#endif 


#define _access(f,num)	0

#define sleep(x)		Sleep(x*1000)

#endif //ENABLE_PLATFORM_WIN32

#endif //_platform_wince_h

