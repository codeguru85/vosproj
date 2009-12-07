

#include <xplatform/xplatform.h>

#ifdef ENABLE_PLATFORM_WIN32

#include <Windows.h>
#include <assert.h>
#include <xplatform/platform-win32.h>

#if !defined(DEFAULT_XP_FILE_EXIST)
int xp_file_exist(const char* name)
{
	wchar_t	wszBuffer[1024];
	int		nPathLen;
	int		nWidecharCount;

	nPathLen = strlen(name);
	assert(1024 > nPathLen + 2);	
	nWidecharCount = mbstowcs(wszBuffer, name, nPathLen);

	assert(nWidecharCount >= 0);
	wszBuffer[nWidecharCount] = '\0';

	return GetFileAttributes(wszBuffer);
}
#endif //DEFAULT_XP_FILE_EXIST

#endif //ENABLE_PLATFORM_WIN32
