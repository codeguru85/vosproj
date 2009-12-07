#include <stdio.h>
#include <tchar.h>
//#define WITH_LOG4C
//#include "log4c.h"

#define LOG_CATEGORY_NAME "com.luonjtu.log"
#include "log4c-wrapper.h"



int _tmain(int argc, TCHAR** argv){
	int rc = 0;
	
	char sz[100];
	int i;
	for(i=0;i<100;i++)
		sz[i]='a'; 
	sz[99] =0;

	LOGINIT();
	LOGMSG(LOG_PRIORITY_ERROR, "HELLO");
	LOGMSG(LOG_PRIORITY_ERROR, sz);
	LOGFINI();

	/*
	if (LOGINIT()){
		printf("mylog_init() failed");
		rc = 1;  
	}else{

		LOGMSG(LOG_PRIORITY_ERROR, "HELLO");


		if ( LOGFINI()){
			printf("mylog_fini() failed");
		}
	}
	*/
	return rc;
}
