

#include <xplatform/xplatform.h>

#ifdef ENABLE_PLATFORM_WINCE

#include <Windows.h>
#include <assert.h>
#include <xplatform/platform-wince.h>

int AnsiToUnicode(const char* in, wchar_t** out)
{
	int nOutLen;
	int nInLen = strlen(in);
	*out = malloc(sizeof(wchar_t) * nInLen + 2);
	nOutLen = mbstowcs(*out, in, nInLen);
	*out[nOutLen] = '\0';
	if(nOutLen == 0)
		free(*out);
	return nOutLen;
}

int xp_file_exist(const char* name)
{
	/*
	wchar_t	wszBuffer[1024];
	int		nPathLen;
	int		nWidecharCount;

	nPathLen = strlen(name);
	assert(1024 > nPathLen + 2);	
	nWidecharCount = mbstowcs(wszBuffer, name, nPathLen);

	assert(nWidecharCount >= 0);
	wszBuffer[nWidecharCount] = '\0';

	return GetFileAttributes(wszBuffer) != INVALID_FILE_ATTRIBUTES;
	*/
	FILE *fp;
	fp = fopen(name,"r");
	if(fp)
	{
		fclose(fp);
	}
	return fp!=NULL;
}

void xp_gettimeofday(log4c_common_time_t* p,void* reserve)
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	p->tm_milli	= time.wMilliseconds;
	p->tm_sec	= time.wSecond;
	p->tm_min	= time.wMinute;
	p->tm_hour	= time.wHour;
	p->tm_mday	= time.wDay;
	p->tm_mon	= time.wMonth;
	p->tm_wday	= time.wDayOfWeek;
	p->tm_year	= time.wYear;
}

int rename(const char *oldfile, const char *newfile)
{
	int res;    
	size_t lenold;
	size_t lennew;
	wchar_t *wsold;
	wchar_t *wsnew;

	/* Covert filename buffer to Unicode. */

	/* Old filename */
	lenold = MultiByteToWideChar (CP_ACP, 0, oldfile, -1, NULL, 0) ;
	wsold = (wchar_t*)malloc(sizeof(wchar_t) * lenold);
	MultiByteToWideChar( CP_ACP, 0, oldfile, -1, wsold, lenold);

	/* New filename */
	lennew = MultiByteToWideChar (CP_ACP, 0, newfile, -1, NULL, 0) ;
	wsnew = (wchar_t*)malloc(sizeof(wchar_t) * lennew);
	MultiByteToWideChar(CP_ACP, 0, newfile, -1, wsnew, lennew);

	/* Delete file using Win32 CE API call */
	res = MoveFile(wsold, wsnew);

	/* Free wide-char string */
	free(wsold);
	free(wsnew);

	if (res)
		return 0; /* success */
	else
		return -1;
}

int unlink(const char *filename)
{
	wchar_t* name;
	int nLen;
	nLen = AnsiToUnicode(filename,&name);
	return DeleteFile(name)!=0 ? 0 : -1;
}

#endif //ENABLE_PLATFORM_WINCE
