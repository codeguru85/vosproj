/***************************************************************************
  Copyright    : 
  Program ID   : httpfun.c
  Description  : HTTP包发送接收 底层接口函数
  Modification Log:
       DATE         AUTHOR          DESCRIPTION
---------------------------------------------------------------------------

***************************************************************************/
extern "C"
{
#include <tcplib.h>
}
#include "httpfun.h"
#include	"tlib_all.h"

#define MacDefaultContentType "text/xml"
#define MacDefaultBossContentType "application/x-www-form-urlencoded"

static unsigned char hexchars[] = "0123456789ABCDEF";
char *sUrlEncode(char *s, int len, int *new_length)
{
	register int x, y;
	static unsigned char str[BUFSIZ];

	for (x = 0, y = 0; len-- && y+2<BUFSIZ ; x++, y++)
    {
		str[y] = (unsigned char) s[x];

/*
		if (str[y] == ' ')
        {
			str[y] = '+';

		}
        else if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
				   (str[y] < 'A' && str[y] > '9') ||
				   (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
				   (str[y] > 'z'))
	    {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
		}
*/
        if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
				   (str[y] < 'A' && str[y] > '9') ||
				   (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
				   (str[y] > 'z') ||
				   str[y] == ' ')
	    {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
		}
	}
	str[y] = '\0';
	if (new_length) {
		*new_length = y;
	}
	return ((char *) str);
}

/***************************************************************************
 * 描  述： HTTP打包并发送
 * 输  入： nSock
            sBuffer 必须含字符串终止符'\0'
            sHost   
            sRestUrl
            sContentType:  为空时 缺省"text/xml"
 * 输  出： 
 * 返回值： 0:成功; !0:失败
*****************************************************************************/
int nHttpSendBuffer( int nSock, const char* sBuffer, 
                     char *sHost, char *sRestUrl, char* sContentType, char* sSoapAction,int nMemHandle)
{
    int     nRet = HTTP_FAIL;
    int     nTotalSize, nHeadSize, nBodySize;    
    char    sType[40], sActionStr[80];
    char    sHTTPBuffer[MacHttpSendBufLen]; 

    if( sBuffer==NULL || sHost==NULL || sRestUrl==NULL )
    {
        TLib_Log_LogMsg("%s:%d---nHttpSendBuffer Invalid parameters\n", __FILE__,__LINE__);
        return HTTP_FAIL;
    }

    memset(sActionStr, 0, sizeof(sActionStr));
    memset( sType, 0, sizeof(sType));
    if ( sContentType == NULL )
        strcpy( sType, MacDefaultContentType);
    else
        strncpy( sType, sContentType, sizeof(sType)-1);

    if ( (sSoapAction!=NULL) && strlen(sSoapAction) )
    {
        sprintf( sActionStr, "SOAPAction: \"%s\"\r\n", sSoapAction );
    }

    
    memset(sHTTPBuffer,0,sizeof(sHTTPBuffer));
    nBodySize = strlen( sBuffer );    
    
   // memset( sHTTPBuffer, 0, MacHttpSendBufLen );
    sprintf( sHTTPBuffer, "POST %s HTTP/1.1\r\n"
                          "HOST: %s\r\n"
                          "Content-Type: %s; charset=gbk\r\n"
                          "Content-Length: %d\r\n"
                          "%s\r\n", 
                          sRestUrl, sHost, sType, nBodySize, sActionStr);

    
      
    TLib_Log_LogMsg(
                "%s:%d---sHTTPHead=\n-----\n%s\n-----\n", 
                __FILE__, __LINE__, sHTTPBuffer);

    nHeadSize  = strlen(sHTTPBuffer);
    nTotalSize =  nHeadSize + nBodySize ;
    
    if ( nTotalSize+1 > MacHttpSendBufLen )
    {
        
        TLib_Log_LogMsg( 
                   "%s:%d---nTotalSize+1=[%d] is larger than MacBufferLength=[%d]\n",
                   __FILE__, __LINE__, nTotalSize+1,MacHttpSendBufLen); 
        
        return HTTP_BUF_FAIL;
    }
    
    memcpy( sHTTPBuffer+nHeadSize, sBuffer, nBodySize );
    nRet=nTcpWriten( nSock, sHTTPBuffer, nTotalSize );
    if( nRet < 0 )
    {
        TLib_Log_LogMsg(
                    "%s:%d---HttpSend nTcpWriten error=%d\n",__FILE__,__LINE__, nRet); 
        return HTTP_NET_FAIL;
    }
   TLib_Log_LogMsg("%s:%d---HttpSend =\n%s\n",
                __FILE__,__LINE__, sHTTPBuffer);
    return HTTP_SUCC;
}


/***************************************************************************
 * 描  述： HTTP包接收
 * 输  入： nSock
            nBuffSize
            nTimeOut
 * 输  出： sRecvBuff
 * 返回值： >0:返回应答内容长度; <0:失败
*****************************************************************************/
int nHttpReceive( int nSock, char *sRecvBuff, int nBuffSize, int nTimeOut  )
{
    int     nRet = HTTP_FAIL;
    int     nContentSize, nHeadSize, nRecvSize, nLeftToRead;
    char    sHttpHeader[MacHttpPreReadLen+1];
    char    *pFoundIt   = NULL;
    char    *pHeadEnd   = NULL;
    const char* sHeadEnd    = "\r\n\r\n"; 
    const char* sLengthTag  = "Content-Length:";
    const char* slengthTag  = "Content-length:";

    if ( sRecvBuff == NULL || nBuffSize < 0 || nSock < 0 )
    {
        return HTTP_FAIL;
    }
    memset( sHttpHeader, 0, sizeof(sHttpHeader) );  

     //预读1024字节
    //nRecvSize = Readn( nSock, sHttpHeader, MacHttpPreReadLen );
    nRecvSize = nTcpReadn(nSock, sHttpHeader, MacHttpPreReadLen, nTimeOut); 
    if( nRecvSize <= 0  )
    {
        TLib_Log_LogMsg( 
                    "%s:%d---Readn Error=%d\n",__FILE__, __LINE__, nRecvSize);
        return HTTP_NET_FAIL;
    }
    else
    {
        TLib_Log_LogMsg("%s:%d---Readn Head=%d\n %s\n",__FILE__, __LINE__, nRecvSize, sHttpHeader);
    }
    //查找http head结束标记
    pFoundIt = strstr( sHttpHeader, sHeadEnd );
    if( pFoundIt == NULL )   // not found it
    {
        TLib_Log_LogMsg("%s:%d---Can't found sHttpHeadEnd\n%s\n",
            __FILE__, __LINE__, sHttpHeader);
        return HTTP_FAIL;
    }

    pHeadEnd = pFoundIt + strlen(sHeadEnd);
    nHeadSize= pHeadEnd - sHttpHeader; 
    if( nHeadSize > nRecvSize )
    {
        //头部长度超过预读缓冲区长度
        TLib_Log_LogMsg("%s:%d---Http Head too long=%d > bufsize=%d\n",
            __FILE__, __LINE__, nHeadSize, nRecvSize);
        return HTTP_BUF_FAIL;    
    }
    
    pFoundIt = strstr( sHttpHeader, sLengthTag );
    if ( pFoundIt == NULL )
    {
        pFoundIt = strstr( sHttpHeader, slengthTag );
    }
    
    if ( pFoundIt == NULL )        
    {   //没有内容长度字段
       TLib_Log_LogMsg(
                    "%s:%d---Http Content Length not found. Head Buffer=\n%s\n",
                    __FILE__, __LINE__, sHttpHeader);
        if( nRecvSize < MacHttpPreReadLen )
            nContentSize = nRecvSize - nHeadSize;
        else
            nContentSize = nBuffSize;
    }
    else 
    {
        if ( pFoundIt - sHttpHeader > nHeadSize )
        {
            TLib_Log_LogMsg(
                "%s:%d---Http Content Length not found, nHead=\n%s\n",
                __FILE__, __LINE__, sHttpHeader);
            return HTTP_FAIL;
        }
        nContentSize = atoi(pFoundIt+strlen(sLengthTag));
    }

    if( nContentSize > nBuffSize )
    {
        TLib_Log_LogMsg(
            "%s:%d---sRecvBuff space[%d] not enough, Http Package body size=%d\n", 
            __FILE__, __LINE__, nBuffSize, nContentSize );
        return HTTP_FAIL;
    }

    memcpy( sRecvBuff, sHttpHeader+nHeadSize, nRecvSize-nHeadSize );

    nLeftToRead = nContentSize-(nRecvSize-nHeadSize);
    if ( nLeftToRead > 0 )
    {
        //nRet = Readn(nSock, sRecvBuff+(nRecvSize-nHeadSize), nLeftToRead); 
        nRet = nTcpReadn(nSock, sRecvBuff+(nRecvSize-nHeadSize), nLeftToRead, nTimeOut); 
        if( nRet <= 0  )
        {
           TLib_Log_LogMsg("%s:%d---Readn:: Error=%d\n", __FILE__, __LINE__, nRet);
            return HTTP_NET_FAIL;
        }
        if ( nContentSize == nBuffSize && nRet < nLeftToRead )
        {   //没有内容长度字段
            nContentSize = nContentSize - nLeftToRead + nRet;
        }
    }
    sHttpHeader[nHeadSize]  = 0;
    sRecvBuff[nContentSize] = 0;
    TLib_Log_LogMsg("%s:%d---Http Recv=%d, head len=%d,content length=%d\n" 
                "Head=\n%s\nBody=\n%s\n",
                __FILE__, __LINE__, nRecvSize, nHeadSize, nContentSize,
                sHttpHeader, sRecvBuff);    
    return nContentSize;
}  


/***************************************************************************
 * 描  述： BOSS HTTP打包并发送
 * 输  入： nSock
            sBuffer 必须含字符串终止符'\0'
            sHost   
            sRestUrl
            sContentType:  为空时 缺省"text/xml"
 * 输  出： 
 * 返回值： 0:成功; !0:失败
*****************************************************************************/
int nBossHttpSendBuffer( int nSock, const char* sBuffer, 
                     char *sHost, char *sRestUrl, char* sContentType, char* sSoapAction,char * sMethod)
{
    int     nRet = HTTP_FAIL;
    int     nTotalSize, nHeadSize, nBodySize;    
    char    sType[40], sActionStr[80];
    char    sTmpStr[MacHttpSendBufLen];
    static char    *sHTTPBuffer=NULL; 

    if( sBuffer==NULL || sHost==NULL || sRestUrl==NULL )
    {
        TLib_Log_LogMsg(
            "%s:%d---nHttpSendBuffer Invalid parameters\n", __FILE__,__LINE__);
        return HTTP_FAIL;
    }

    memset(sActionStr, 0, sizeof(sActionStr));
    memset( sType, 0, sizeof(sType));
    if ( sContentType == NULL )
        strcpy( sType, MacDefaultBossContentType);
    else
        strncpy( sType, sContentType, sizeof(sType)-1);

    if ( (sSoapAction!=NULL) && strlen(sSoapAction) )
    {
        sprintf( sActionStr, "SOAPAction: \"%s\"\r\n", sSoapAction );
    }

    if ( sHTTPBuffer == NULL )
    {
        sHTTPBuffer = (char*)malloc(MacHttpSendBufLen);
    }
    nBodySize = strlen( sBuffer );    
    
    memset( sHTTPBuffer, 0, MacHttpSendBufLen );
//    strcpy(sHTTPBuffer,"POST ");
    sprintf(sHTTPBuffer,"%s ",sMethod);
    strncat(sHTTPBuffer,sRestUrl,MacHttpSendBufLen);
    
    sprintf( sTmpStr, " HTTP/1.0\r\n"
                          "HOST: %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %d\r\n"
                          "%s\r\n", 
                           sHost, sType, nBodySize, sActionStr);
    strncat(sHTTPBuffer,sTmpStr,MacHttpSendBufLen);


    nHeadSize  = strlen(sHTTPBuffer);
    nTotalSize =  nHeadSize + nBodySize ;
    
    if ( nTotalSize+1 > MacHttpSendBufLen )
    {
        
       TLib_Log_LogMsg( 
                   "%s:%d---nTotalSize+1=[%d] is larger than MacBufferLength=[%d]\n",
                   __FILE__, __LINE__, nTotalSize+1,MacHttpSendBufLen); 
        
        return HTTP_BUF_FAIL;
    }
    
    memcpy( sHTTPBuffer+nHeadSize, sBuffer, nBodySize );
    nRet=nTcpWriten( nSock, sHTTPBuffer, nTotalSize );
    if( nRet < 0 )
    {
        TLib_Log_LogMsg( 
                    "%s:%d---HttpSend nTcpWriten error=%d\n",__FILE__,__LINE__, nRet); 
        return HTTP_NET_FAIL;
    }

    TLib_Log_LogMsg(
                "%s:%d---Http Send=%s\n",
                __FILE__, __LINE__, sHTTPBuffer);    

    return HTTP_SUCC;
}


/***************************************************************************
 * 描  述： BOSS HTTP包接收
 * 输  入： nSock
            nBuffSize
            nTimeOut
 * 输  出： sRecvBuff
 * 返回值： >0:返回应答内容长度; <0:失败
*****************************************************************************/
int nBossHttpReceive( int nSock, char *sRecvBuff, int nBuffSize, int nTimeOut  )
{
    int     nRet = HTTP_FAIL;
    int     nContentSize, nHeadSize, nRecvSize, nLeftToRead;
    char    sHttpHeader[MacHttpPreReadLen+1];
    char    *pFoundIt   = NULL;
    char    *pHeadEnd   = NULL;
    const char* sHeadEnd    = "\r\n\r\n"; 
    const char* sLengthTag  = "Content-Length:";
    const char* slengthTag  = "Content-length:";

    if ( sRecvBuff == NULL || nBuffSize < 0 || nSock < 0 )
    {
        return HTTP_FAIL;
    }
    memset( sHttpHeader, 0, sizeof(sHttpHeader) );  

    //预读1024字节
    //nRecvSize = Readn( nSock, sHttpHeader, MacHttpPreReadLen );
    nRecvSize = nTcpReadn(nSock, sHttpHeader, MacHttpPreReadLen, nTimeOut); 
    if( nRecvSize <= 0  )
    {
        TLib_Log_LogMsg(
                    "%s:%d---Readn Error=%d\n",__FILE__, __LINE__, nRecvSize);
        return HTTP_NET_FAIL;
    }
    else
    {
        /*
         TLib_Log_LogMsg(
                    "%s:%d---Readn Head=%d\n %s\n",__FILE__, __LINE__, nRecvSize, sHttpHeader);*/
    }
    //查找http head结束标记
    pFoundIt = strstr( sHttpHeader, sHeadEnd );
    if( pFoundIt == NULL )   // not found it
    {
        TLib_Log_LogMsg(
            "%s:%d---Can't found sHttpHeadEnd\n%s\n",
            __FILE__, __LINE__, sHttpHeader);
        return HTTP_FAIL;
    }

    pHeadEnd = pFoundIt + strlen(sHeadEnd);
    nHeadSize= pHeadEnd - sHttpHeader; 
    if( nHeadSize > nRecvSize )
    {
        //头部长度超过预读缓冲区长度
        TLib_Log_LogMsg(
            "%s:%d---Http Head too long=%d > bufsize=%d\n",
            __FILE__, __LINE__, nHeadSize, nRecvSize);
        return HTTP_BUF_FAIL;    
    }
    
    pFoundIt = strstr( sHttpHeader, sLengthTag );
    if ( pFoundIt == NULL )
    {
        pFoundIt = strstr( sHttpHeader, slengthTag );
    }
    
    if ( pFoundIt == NULL )        
    {   //没有内容长度字段
        /*TLib_Log_LogMsg(
                    "%s:%d---Http Content Length not found. Head Buffer=\n%s\n",
                    __FILE__, __LINE__, sHttpHeader);
         */           
        if( nRecvSize < MacHttpPreReadLen )
            nContentSize = nRecvSize - nHeadSize;
        else
            nContentSize = nBuffSize;
    }
    else 
    {
        if ( pFoundIt - sHttpHeader > nHeadSize )
        {
           TLib_Log_LogMsg(
                "%s:%d---Http Content Length not found, nHead=\n%s\n",
                __FILE__, __LINE__, sHttpHeader);
            return HTTP_FAIL;
        }
        nContentSize = atoi(pFoundIt+strlen(sLengthTag));
    }

    if( nContentSize > nBuffSize )
    {
        TLib_Log_LogMsg(
            "%s:%d---sRecvBuff space[%d] not enough, Http Package body size=%d\n", 
            __FILE__, __LINE__, nBuffSize, nContentSize );
        return HTTP_FAIL;
    }

    memcpy( sRecvBuff, sHttpHeader+nHeadSize, nRecvSize-nHeadSize );

    nLeftToRead = nContentSize-(nRecvSize-nHeadSize);
    if ( nLeftToRead > 0 )
    {
        //nRet = Readn(nSock, sRecvBuff+(nRecvSize-nHeadSize), nLeftToRead); 
        nRet = nTcpReadn(nSock, sRecvBuff+(nRecvSize-nHeadSize), nLeftToRead, nTimeOut); 
        if( nRet <= 0  )
        {
            TLib_Log_LogMsg(
                "%s:%d---Readn:: Error=%d\n", __FILE__, __LINE__, nRet);
            return HTTP_NET_FAIL;
        }
        if ( nContentSize == nBuffSize && nRet < nLeftToRead )
        {   //没有内容长度字段
            nContentSize = nContentSize - nLeftToRead + nRet;
        }
    }
    sHttpHeader[nHeadSize]  = 0;
    sRecvBuff[nContentSize] = 0;
    TLib_Log_LogMsg(
                "%s:%d---Http Recv=%d, head len=%d,content length=%d\n" 
                "Head=\n%s\nBody=\n%s\n",
                __FILE__, __LINE__, nRecvSize, nHeadSize, nContentSize,
                sHttpHeader, sRecvBuff);    
    return nContentSize;
}  




