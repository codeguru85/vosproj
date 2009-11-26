#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "lib_boss_sockstack.h"

////////////////////////////////////////////////////////////////////////////
int PushBuf(char *psDBuffer,short *pshOffset, const int iMaxBufLen,const char *pszSBuffer,int iLength)
{
	//assert(*pshOffset + iLength <= iMaxBufLen);
	if(*pshOffset + iLength >= iMaxBufLen)
		return -1;
	memcpy(psDBuffer+(*pshOffset),pszSBuffer,iLength);
	*pshOffset += iLength;
	return iLength;
}

int Push(SocketContext *pstSocketContext,const char *pszBuffer,int iLength,int type)
{
	if(type)
		return PushBuf(pstSocketContext->sSendBuffer,&(pstSocketContext->shBytesToSend),
		MAXSENDBUF,pszBuffer,iLength);
	return PushBuf(pstSocketContext->sRecvBuffer,&(pstSocketContext->shBytesReceived),
		MAXRECVBUF,pszBuffer,iLength);
}

int PopBuf(char *psSBuffer,short *pshOffset, char *pszDBuffer,int iLength)
{
	assert(*pshOffset - iLength >= 0);
//	if(*pshOffset - iLength < 0)
//		return -1;
	if(pszDBuffer)
		memcpy(pszDBuffer,psSBuffer,iLength);
	memmove(psSBuffer,psSBuffer+iLength,*pshOffset-iLength);
	*pshOffset -= iLength;
	return 0;
}

int Pop(SocketContext *pstSocketContext,char *pszBuffer,int iLength,int type)
{
	if(type)
		return PopBuf(pstSocketContext->sSendBuffer,&(pstSocketContext->shBytesToSend),pszBuffer,iLength);
	return PopBuf(pstSocketContext->sRecvBuffer,&(pstSocketContext->shBytesReceived),pszBuffer,iLength);
}

int PopToNull(SocketContext *pstSocketContext,int iLength,int type)
{
	if(type)
		return PopBuf(pstSocketContext->sSendBuffer,&(pstSocketContext->shBytesToSend),NULL,iLength);
	return PopBuf(pstSocketContext->sRecvBuffer,&(pstSocketContext->shBytesReceived),NULL,iLength);
}

///////////////////////////////////////////////////////////////////////////////////////

