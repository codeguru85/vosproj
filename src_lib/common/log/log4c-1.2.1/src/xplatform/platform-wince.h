
#ifndef _platform_wince_h
#define _platform_wince_h

#ifdef ENABLE_PLATFORM_WINCE

#include <Windows.h>

#undef DEFAULT_XP_FILE_EXIST
#undef DEFAULT_XP_GETTIMEOFDAY

#undef SD_ACCESS_READ
#define SD_ACCESS_READ(a)	_access(a,04)

#undef FILE_SEP
#define FILE_SEP "\\"

#define strncasecmp		strnicmp
#define strcasecmp		stricmp

#define vsnprintf		_vsnprintf
#define snprintf		_snprintf	
#define stricmp			_stricmp
#define strnicmp		_strnicmp
#define strdup			_strdup

#define setbuf(f,buf)	setvbuf((f),(buf), 0x0004 ,0)
#define getenv(var)		0
#define _access(f,num)	0

int rename(const char *oldfile, const char *newfile);
int unlink(const char *filename);

#endif //ENABLE_PLATFORM_WINCE

#endif //_platform_wince_h
