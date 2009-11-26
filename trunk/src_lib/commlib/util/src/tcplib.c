/***************************************************************************
  Copyright    : 2002, 
  Program ID   : tcplib.c
  Description  : socket 实现简单源文件
  Version      : 
  Modification Log:

***************************************************************************/
#include <unistd.h>
#include "tcplib.h"

/**************************************************************************
 Function Name 
 Description: this function will construct a tcp connection to destinated
        server
 
 Parameters:  char* sHost(I) - the hostname or IP address of server to be 
                    connected
                int nServ(I) - the service name or port number 
 
 Return Value:

 *************************************************************************/
int nGetLocalIP(char* sIP)
{
    char    buf[100];
    char    **ppsTmp;
    struct  hostent* prHost = NULL;

    if ( sIP == NULL )
    {
        return -1;
    }
    
    memset(buf, 0, 100);
    gethostname(buf, 100);

    prHost = gethostbyname( buf);
    if(prHost == NULL)
    {
        //printf("gethostent error\n");
        return -1;
    }

    ppsTmp = prHost->h_addr_list;
    memcpy(sIP,(char*)inet_ntoa(*((struct in_addr*)prHost->h_addr)), 16 );
    
    return 0;
}

/**************************************************************************
 Function Name 
 Description: 
            this function will construct a tcp connection to destinated
            server
 
 Parameters:  char* sHost(I) - the hostname or IP address of server to be 
                    connected
                int nServ(I) - the service name or port number 
 
 Return Value: int
                >0 success
                TCPLIB_FAIL error
 
 Return Value  :
 *************************************************************************/
int isIPAddr(const char *sValue)
{
        if (NULL == sValue)
                return -1;

        while(*sValue != '\0')
        {
                if ( (*sValue < '0' || *sValue > '9') && (*sValue!='.') )
                        return -1;
                sValue++;
        }
        return 0;
}

int nTcpConnect( const char* sHost, int nServ )
{
    int     nSockfd;
    int     nRetConn;
    struct  sockaddr_in rServaddr;
    struct  linger      rLinger;
    int     nRetSetOpt;
    struct hostent* h;

     memset(&rServaddr, 0, sizeof(rServaddr));
    if (isIPAddr(sHost) != 0)
    {
	    if(  !(h=gethostbyname(sHost)) ) 
	    {
	        return TCPLIB_FAIL;
	    }
		bzero( (char *)&rServaddr, sizeof(rServaddr));
		bcopy(h->h_addr,&rServaddr.sin_addr,sizeof(rServaddr.sin_addr));
    }
    else
    {
           rServaddr.sin_addr.s_addr   = inet_addr(sHost);
    }
   

    rServaddr.sin_family        = AF_INET;
    rServaddr.sin_port          = htons((unsigned short)nServ);
    
    nSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( nSockfd < 0)
    {
        return TCPLIB_FAIL;
    }

    rLinger.l_onoff  = 1;   // 打开linegr开关
    rLinger.l_linger = 0;   // 设置延迟时间为 0 秒, 注意 TCPIP立即关闭，但是有可能出现化身

    if( (nRetSetOpt = setsockopt(nSockfd, SOL_SOCKET, SO_LINGER, (char *)&rLinger, sizeof(rLinger))) != 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }

    
    if(0 > (nRetConn = connect(nSockfd, (struct sockaddr*)&rServaddr,
                sizeof(rServaddr))))
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }

    return nSockfd;
} 

int nTcpAsynConnect( char* sHost, int nServ , int nTimeout)
{
    int     nRet,nFlags,nError;
    socklen_t nLen;
    int     nSockfd;
    struct  sockaddr_in rServaddr;

    struct hostent* h;
    fd_set rset,wset;
    struct timeval tval;
	
    memset(&rServaddr, 0, sizeof(rServaddr));
    if (isIPAddr(sHost) != 0)
    {
	    if(  !(h=gethostbyname(sHost)) ) 
	    {
	        return TCPLIB_FAIL;
	    }
		bzero( (char *)&rServaddr, sizeof(rServaddr));
		bcopy(h->h_addr,&rServaddr.sin_addr,sizeof(rServaddr.sin_addr));
    }
    else
    {
           rServaddr.sin_addr.s_addr   = inet_addr(sHost);
    }

    rServaddr.sin_family        = AF_INET;
    rServaddr.sin_port          = htons((unsigned short)nServ);
    
    nSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( nSockfd < 0)
    {
        return TCPLIB_FAIL;
    }
   
    nFlags = fcntl(nSockfd, F_GETFL, 0);	
    nRet = fcntl(nSockfd, F_SETFL,  nFlags | O_NONBLOCK);	
    if (nRet < 0)
    {
	   nTcpClose(nSockfd);
          return TCPLIB_FAIL;
    }

    nError = 0;
    nRet = connect(nSockfd, (struct sockaddr*)&rServaddr,  sizeof(rServaddr));
    if (nRet == 0)
    {
         nRet = fcntl(nSockfd, F_SETFL,  nFlags);	
	  if (nRet < 0)
	  {
		nTcpClose(nSockfd);
	       return TCPLIB_FAIL;
	  }
	   return nSockfd;         
    }
	
    if (nRet < 0)
    {
        if ( errno != EINPROGRESS )
	 {
	        nTcpClose(nSockfd);
	        return TCPLIB_FAIL;
	 }
    }

    //对于connect()函数返回-1，且errno为EINPROGRESS的情况，通过select()函数,
    //等待最多nTimeout秒，来确认端口是否真正连接成功:

    FD_ZERO(&rset);
    FD_SET(nSockfd, &rset);
    wset = rset;
    tval.tv_sec = nTimeout;
    tval.tv_usec = 0;

    nRet = select(nSockfd + 1, &rset, &wset, NULL, &tval);
    if (nRet == 0)
    {
        nTcpClose(nSockfd);
	 return TCPLIB_FAIL;
    }

    if (FD_ISSET(nSockfd, &rset) || FD_ISSET(nSockfd, &wset) )
    {
        nLen = sizeof(nError);
        nRet = getsockopt(nSockfd, SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen);
	 if (nRet < 0)
	 {
	     nTcpClose(nSockfd);
	     return TCPLIB_FAIL;
	 }
    }
    else
    {
         nTcpClose(nSockfd);
	  return TCPLIB_FAIL;
    }

    nRet = fcntl(nSockfd, F_SETFL,  nFlags);	
    if (nRet < 0)
    {
	nTcpClose(nSockfd);
	return TCPLIB_FAIL;
     }

     if (nError)
     {
         nTcpClose(nSockfd);
	  return TCPLIB_FAIL;
      }   

      return nSockfd;    
} 

#define  MacListenQueue 1024 * 10

/**************************************************************************
 Function Name 
 Description  : the call of this function will construct a tcp server on desig
                nated IP and PORT
 Parameters    : const char* sHost(I) - the IP address or hostname of server
                 if NULL,the IP address decided by system
                  int nServ(I) - the port on which server will listen
 
 Return Value : int
                   TCPLIB_OK success
                   TCPLIB_FAIL error
 *************************************************************************/
int nTcpListen(const char* sHost, int nServ)
{
    int     nSockfd;
    int     nRetSetOpt;
    int     nRetBnd;
    int     nOne;

    struct  sockaddr_in rServaddr;
    struct  linger      rLinger;

    
    memset(&rServaddr, 0, sizeof(rServaddr));

    rServaddr.sin_family = AF_INET;
    rServaddr.sin_port   = htons((unsigned short)nServ);
    if(sHost == NULL)
    {
        rServaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        rServaddr.sin_addr.s_addr = inet_addr(sHost);
    }

    
    if( (nSockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0 )
    {
        return TCPLIB_FAIL;
    }

    
    nOne = 1;
    if( (nRetSetOpt = setsockopt(nSockfd, SOL_SOCKET, SO_REUSEADDR,(char *)&nOne, sizeof(nOne))) != 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }
/*
    nOne = 1;
    if( (nRetSetOpt = setsockopt(nSockfd, SOL_SOCKET, SO_REUSEPORT, &nOne, sizeof(nOne))) != 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }
*/
    rLinger.l_onoff  = 1;   // 打开linegr开关
    rLinger.l_linger = 0;   // 设置延迟时间为 0 秒, 注意 TCPIP立即关闭，但是有可能出现化身

    if( (nRetSetOpt = setsockopt(nSockfd, SOL_SOCKET, SO_LINGER, (char *)&rLinger, sizeof(rLinger))) != 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }

    
    if( (nRetBnd = bind(nSockfd, (struct sockaddr*)&rServaddr, sizeof(rServaddr))) < 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }    

    
    if( listen(nSockfd, MacListenQueue) < 0 )
    {
        nTcpClose(nSockfd);
        return TCPLIB_FAIL;
    }

    return nSockfd;
}

/**************************************************************************
 Function Name 
 Description : this function will block until a client construct a connetion 
                     to the server and the function will return a socket file 
                    descriptor representing the construted connnection
 
 Parameters : int nListenfd(I) - the socket descriptor returned by nTcpListen
                char* sIPClient(O) - the IP address of client who constructed 
                                        the connection
 
 Return Value : int 
                  > 0  success
                  TCPLIB_FAIL error
 Return Value  :
 *************************************************************************/
int nTcpAccept(int nListenfd, char* sIPClient)
{
    int     nSockfd;
    socklen_t     nLenCliAddr;
    
    struct sockaddr_in rCliAddr;

    nLenCliAddr = sizeof(rCliAddr);
    memset(&rCliAddr, 0, sizeof(rCliAddr));
    
    for(;;)
    {
        nSockfd = accept(nListenfd, (struct sockaddr*)&rCliAddr, &nLenCliAddr); 
        if( nSockfd < 0 )
        {
            if( EINTR == errno || ENOBUFS == errno ) 
                continue;
            else
                return TCPLIB_FAIL;
        }
        break;
    }

    if(sIPClient != NULL)
    {
        strcpy(sIPClient, (const char*)inet_ntoa(rCliAddr.sin_addr));
    }

    return nSockfd;
} 

/**************************************************************************
 Function Name 
 Description   : this function will read n bytes from designated socket 
                    descriptor or return less than n bytes when timeout
 
 Parameters    : int nSockfd(I) - socket fd from which data will be read
                                         the fd will be returned by nTcpConnect
                                            and nTcpAccept
                   void* pvRecvBuf(IO) - the buf to receive data
                   size_t nBytesToRead(I) - how many bytes the caller want to
                                                read 
                   int nTimeout(IO) - the timeout time ,unit is second,if NULL,
                   will block...,if *pnTimeout == -1,time out
 
 Return Value : int
                 > 0 - the bytes read
                 = 0 - socket fd is nTcpClosed
                 < 0 - error occured
 Return Value  :
 *************************************************************************/
int nTcpReadn(int    nSockfd,
              char*  pvRecvBuf,
              size_t nBytesToRead,
              int    nTimeout)
{
    int     nLeft;
    int     nRead;
    int     nRet;
    int     nRetSel;
    char*   pvTmp;

    pvTmp = pvRecvBuf;
    nLeft = nBytesToRead;
    nRet  = 0;
    while( nLeft > 0 )
    {   
        nRetSel = nTcpSelect(nSockfd, nTimeout);
        if(nRetSel == 0)
        {            
            nRet = 0; 
            break;
        }
        if(nRetSel <0)
        {
            nRet = -1;
            break;
        }

#ifdef WIN32        
        if((nRead = recv(nSockfd, pvTmp, nLeft,0)) <= 0)
        {               
            nRet = -2;
            break;
        }

#else
        if((nRead = read(nSockfd, pvTmp, nLeft)) < 0)
        {
            if( errno == EINTR)
            {
                nRead = 0;
                continue;
            }
            else
            {
                nRet = -2;
                break;
            }
        }
        if(nRead == 0)
        {
            /*select >0 , read=0, means network error*/
            nRet = -3;
            break;
        }
#endif

        nLeft-=nRead;
        pvTmp+=nRead;
    }

    if ( pvTmp > pvRecvBuf )  
        return pvTmp-pvRecvBuf;
    else 
        return nRet;
    
}


/**************************************************************************
 Function Name 
 Description   : this function will write n bytes data to designates socket fd
 Parameters    : int nSockfd(I) - socket fd to write data to
                 const void* pvSendBuf(I) - the buffer which contain the data
                                                 to write 
                 size_t nBytesToWrite(I) - how many bytes to write
 
 Return Value  : int
                    > 0 - bytes that have been written
                    -1 - error occured
 Parameters    :
 Return Value  :
 *************************************************************************/
int nTcpWriten(int          nSockfd,
               const char*  pvSendBuf,
               size_t       nBytesToWrite)
{
    int         nLeft;
    int         nWritten;
    const char* pcPtr;

    pcPtr = pvSendBuf;
    nLeft = nBytesToWrite;

#ifdef WIN32
    while( nLeft > 0 )
    {
        if( 0 >= (nWritten = send(nSockfd, pcPtr, nLeft,0)) )
        {   
            return -1;
        }
        nLeft-=nWritten;
        pcPtr+=nWritten;
    }

#else
    while( nLeft > 0 )
    {
        if( 0 >= (nWritten = write(nSockfd, pcPtr, nLeft)) )
        {
            if( EINTR == errno )
                nWritten = 0;
            else
                {
                return -1;
                }
        }
        nLeft-=nWritten;
        pcPtr+=nWritten;
    }

#endif

    return nBytesToWrite;
}


int nTcpSelect(int  nSockfd, int  nTimeout)
{
    int     nRet;
    fd_set  fds;

    struct timeval timeout;
    struct timeval *prTimeout;

    if ( nSockfd < 0 )  return -1;
    
    prTimeout = &timeout;
    FD_ZERO(&fds);
    FD_SET((unsigned int)nSockfd, &fds);

    if ( nTimeout < 0 )
    {
        prTimeout = NULL; 
    }
    else
    {
        prTimeout->tv_sec = nTimeout;
        prTimeout->tv_usec = 0;
    }

    nRet =select(nSockfd+1, &fds, NULL, NULL, prTimeout);
    return nRet;
}


int nTcpClose( int nSockfd )
{

#ifdef WIN32
    return closesocket(nSockfd);
#else
    return close(nSockfd);
#endif

}

/***************************************************************************
 * 描  述： 从socket接收单个包 包头4个字节为长度
 *
 * 输  入： nSock   : socket句柄
 *          pBuf    : 接收缓冲，调用者负责分配空间
 *          nBufLen : 接收缓冲大小
 *          nTimeout: 超时
 * 输  出：
 * 返回值： >0 表示接收包的大小, <0 表示失败
 * add by sunjun
*****************************************************************************/
int nMsgRecv( int nSock, char* pBuf, int nBufLen, int nTimeout )
{
    int     nRet;
    int     nTotalLen;
    int     nTmp;
    if ( pBuf == NULL || nSock < 0 || nBufLen < MacHeadLen )
    {
        return -1;
    }
    /* ------ 读包头长度部分 ------- */    
    nRet =  nTcpReadn( nSock, pBuf, MacHeadLen, nTimeout );
    if ( nRet < 0 )
    {
        return nRet;
    }
    if ( nRet == 0 )
    {
        return 0;
    }
    
    memcpy(&nTmp, pBuf, MacHeadLen);
    nTotalLen = ntohl(nTmp);
    if ( nTotalLen < MacHeadLen || nTotalLen > nBufLen )
    {
        /*
        nDebugLog(0, LOG_PRIORITY_ALERT, 0, 0, 0, 
            "Receive Length not correct len = %d\n", nTotalLen );
        */
        return -2;
    }
    /* ------ 读包体 ------- */        
    nRet = nTcpReadn( nSock, pBuf+MacHeadLen, nTotalLen-MacHeadLen, nTimeout );
    if (nRet < 0)
    {
        /*
        nDebugLog(0, LOG_PRIORITY_ALERT, 0, 0, 0, 
            "%s\n", "nTcpRecv Msg Body error");
        */
        return nRet;
    }

    if ( nRet < nTotalLen-MacHeadLen )
    {
        /*未收到预期包大小，丢弃*/
        /*
        nDebugLog(0, LOG_PRIORITY_ALERT, 0, 0, 0, 
            "%s\n", "nTcpRecv Msg Body timeout");
        */
        return 0;
    }
    return nTotalLen;
}

/*
int GetLocalIP(char * szLocalIP, int iLen)
{   
	  	const char* pszIP = "192.168.0.1" ;
        int iClientSockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(iClientSockfd < 0 )
        {
            printf("Client, Create socket failed");
            return -1;
        }
        
        struct sockaddr_in stINETAddr;
        stINETAddr.sin_addr.s_addr = inet_addr(pszIP);
        stINETAddr.sin_family = AF_INET;
        stINETAddr.sin_port = htons(8888);

        int iCurrentFlag = fcntl(iClientSockfd, F_GETFL, 0);
        fcntl(iClientSockfd, F_SETFL, iCurrentFlag | FNDELAY);

        connect(iClientSockfd, (struct sockaddr *)&stINETAddr, sizeof(struct sockaddr));

        struct sockaddr_in stINETAddrLocal;
        socklen_t iAddrLenLocal = sizeof(stINETAddrLocal);
        getsockname(iClientSockfd, (struct sockaddr *)&stINETAddrLocal, &iAddrLenLocal);
        inet_ntop(AF_INET,&stINETAddrLocal.sin_addr,szLocalIP,iLen);
        return 0;
}
*/
