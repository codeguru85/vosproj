#include <sys/socket.h>
#include <netinet/in_systm.h>	/* required for ip.h */
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>
#include "lib_boss_sockstack.h"
#include "lib_boss_net.h"


#include "net.h"
#include "tlib_all.h"

int UdpSendAndRecv(char *sAddress,int iPort,int iListenPort,char *sSendBuf, uint32_t  iSendLen,int iTimeOutLimit,char *sRevbuf,uint32_t  *iRecvLen,  char * sErrMsg)
{
	int sockfd;
	//int iLen = 0 ;
	fd_set readfds ;
	struct sockaddr_in addr, peer_addr;
	struct sockaddr_in sockname;
	struct timeval  		stTimeVal;
	socklen_t peer_addrLen = sizeof(peer_addr);
	int revlen = 0 ;


	errno = 0 ;
	if(!sSendBuf) { sprintf(sErrMsg,"%s","发送缓冲区为空") ; return -20 ;}
	if(!sRevbuf) {sprintf(sErrMsg,"%s","接收缓冲区为空") ; return -21 ;}
	if(!sAddress){ sprintf(sErrMsg,"%s","发送IP地址为空") ; return -22 ;}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);    
	if ( sockfd <0)				
		return -1 ; 	

	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(sAddress);;
	addr.sin_port = htons(iPort); 

	TLib_Log_LogMsg("IP:%s port:%d,%d",sAddress,iPort,iListenPort);
	sockname.sin_addr.s_addr=INADDR_ANY;
	sockname.sin_family=AF_INET;
	/*防止CGI绑定同一个端口*/
	for(;;)
	{
		sockname.sin_port=htons(iListenPort+rand()%1000);
		if ( (bind(sockfd,(struct sockaddr *)&sockname,sizeof(sockname))) == -1 ) 
		{
			//close(sockfd);
			sprintf(sErrMsg,"绑定端口失败\n");
			continue;
		}
		break;
	}	
	SetNBlock(sockfd);

	/*设置超时时间*/
	stTimeVal.tv_sec = iTimeOutLimit ;
	stTimeVal.tv_usec = 0;         

	//TC2Ccgi_LogHex("sSendBuf.psBuffer:",sSendBuf,iSendLen);
	if (sendto(sockfd, sSendBuf, iSendLen, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	{
		sprintf(sErrMsg,"%s%s","发送数据包失败",strerror(errno));
		close(sockfd);
		return -1;
	}

	FD_ZERO(&readfds);
	FD_SET(sockfd,&readfds);
	 if (select(sockfd +1, &readfds, NULL, NULL, &stTimeVal) <= 0)
	{
		strcpy(sErrMsg," 服务器超时!");
		close(sockfd);
		return -2;
	}

	memset(&peer_addr, 0, sizeof( peer_addr));
	if (FD_ISSET(sockfd, &(readfds)))
	{
		if((revlen = recvfrom(sockfd, sRevbuf, *iRecvLen, 0, (struct sockaddr *) &(peer_addr), (socklen_t*)&peer_addrLen)) <= 0)
		{
			TLib_Log_LogMsg("ip is [%s],[%d]",inet_ntoa(peer_addr.sin_addr),*iRecvLen);
			if (errno == 11)//EAGAIN
			{
				TLib_Log_LogMsg("[Warning] EAGAIN error on recvfrom \n");
				close(sockfd);
				return -3;
			}
			else if(revlen == 0)
			{
				TLib_Log_LogMsg("[Warning] recvfrom len = 0 \n");
				close(sockfd);
				return -4;
			}
			else
			{
				TLib_Log_LogMsg("[Err] Recv udp packet error. Errno = %d, errstr = %s. quitting revlen: %d \n",errno, strerror(errno),revlen);
				return -5;
			}
		}
		*iRecvLen = revlen ;
	}
	
	close(sockfd);

	return 0 ;
}


/*********************************************************************************/
char* Inet_ntoa(u_int32_t lIPAddr)
{
	struct in_addr in;
	static char pIPAddr[20];
	char *p;
	
	in.s_addr = lIPAddr;
	p = inet_ntoa(in);
	if (p == NULL)
		return NULL;
	strncpy( pIPAddr, p , 20);
	return pIPAddr;
}


/**************************************************************
// 函数名： TCP_Server()
// 编写者： Hunter@Tencent
// 参考资料：
// 功 能：  建立本地Tcp Server
// 输入参数：pszHost -- 本地地址，NULL则监听所有地址；nPort -- 监听端口
// 输出参数：return > 0 is ok , -1 is error
// 备 注：
***************************************************************/
int	TCP_Server(const char* pszHost, u_int16_t nPort, int32_t iBacklog, int32_t iSockBuf)
{
	const int on = 1;
	int sock;
	struct hostent * hostinfo;
	struct in_addr * addp;
	struct sockaddr_in sockname;

	memset ((char *)&sockname,0,sizeof(sockname));

	if (pszHost == NULL) {
		hostinfo = NULL;
	} else if ( (hostinfo=gethostbyname(pszHost)) == NULL ) {
        //err_msg("Cannot find %s - %s\n",pszHost,strerror(errno));
        return -1;
    }
	
	if( (sock=socket(AF_INET,SOCK_STREAM,0)) == -1 ) {
		//err_msg("Error opening socket - %s\n",strerror(errno));
		return -1;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (hostinfo != NULL) {
		addp=(struct in_addr *)*(hostinfo->h_addr_list);
		sockname.sin_addr=*addp;
	} else {
		sockname.sin_addr.s_addr=INADDR_ANY;
	}
	sockname.sin_family=AF_INET;
	sockname.sin_port=htons(nPort);

	if ( (bind(sock,(struct sockaddr *)&sockname,sizeof(sockname))) == -1 ) 
	{
		close (sock);
		//err_msg("Cannot bind port %i at %s -%s\n",nPort,pszHost,strerror(errno));
		return -1;
	}
	setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(void*)&iSockBuf,sizeof(iSockBuf));
	setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(void*)&iSockBuf,sizeof(iSockBuf));
	if (listen(sock, iBacklog) < 0)
	{
		close (sock);
		//err_msg("Cannot listen:%s \n",strerror(errno));
		return -1;
	}
	return (sock);
}

int TCP_Nodelay(int iSockfd)
{
	int i;
	if(( i = fcntl(iSockfd, F_GETFL, 0)) == -1)
		return (-1);
	else 
		if (fcntl(iSockfd, F_SETFL, i | FNDELAY) == -1)
		return (-1);
	return 0;
}

/**************************************************************
// 函数名： UDP_Send()
// 编写者： Hunter@Tencent
// 参考资料：
// 功 能：  建立一个UDP Socket
// 输入参数：pszHost -- 服务器IP地址
// 输出参数：return > 0 is ok , -1 is error
// 备 注：  
***************************************************************/
ssize_t UDP_Send(int iSockfd , const void *ptr , ssize_t nbytes , int32_t flags , const char *psIpAddress , u_int16_t unPort)
{
	struct sockaddr_in	servaddr;
	int n ,tmperror;
	
	memset( &servaddr, 0, sizeof(struct sockaddr_in));
	
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(psIpAddress);
    servaddr.sin_port = htons(unPort);

	if( (n = sendto(iSockfd , ptr,nbytes,flags,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in))) != nbytes)
	{
		tmperror = errno;
		errno = tmperror;
        
		if (n < 0 )
			return -1;
	}
	return n;
}

/**************************************************************
// 函数名： UDP_Server()
// 编写者： Hunter@Tencent
// 参考资料：
// 功 能：  建立一个UDP Socket
// 输入参数：pszHost -- 服务器IP地址和端口
// 输出参数：return > 0 is ok , -1 is error
// 备 注：  
***************************************************************/
int UDP_Server(const char* pszHost, u_int16_t nPort)
{
	int udpsock ;
	struct hostent * hostinfo;
	struct in_addr * addp;
	struct sockaddr_in sockname;

	memset ((char *)&sockname,0,sizeof(sockname));
	if (pszHost == NULL) {
		hostinfo = NULL;
	} else if ( (hostinfo=gethostbyname(pszHost)) == NULL ) {
        //err_msg("Cannot find %s - %s\n",pszHost,strerror(errno));
        return -1;
    }
	
	udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpsock < 0 )
	{
		//err_msg("Error opening socket - %s\n",strerror(errno));
		return -1;
	}
	//setsockopt(udpsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (hostinfo != NULL) {
		addp=(struct in_addr *)*(hostinfo->h_addr_list);
		sockname.sin_addr=*addp;
	} else {
		sockname.sin_addr.s_addr=INADDR_ANY;
	}
	sockname.sin_family=AF_INET;
	sockname.sin_port=htons(nPort);

	if ( (bind(udpsock,(struct sockaddr *)&sockname,sizeof(sockname))) == -1 ) 
	{
		close(udpsock);
		//err_msg("Cannot bind port %i at %s -%s\n",nPort,pszHost,strerror(errno));
		return -1;
	}

	return (udpsock);
}	

/**************************************************************
// 函数名： UDP_Recv()
// 编写者： Hunter@Tencent
// 参考资料：
// 功 能：  读取UDP数据
// 输入参数：pszHost -- 服务器IP地址
// 输出参数：return > 0 is ok , -1 is error
// 备 注：  
***************************************************************/
ssize_t UDP_Recv(int iSockfd , char* pszBuf , size_t iLength , int32_t flags ,  char pszHost[] , uint16_t *punPort)
{
	struct	sockaddr_in	ServAddr;
	int		iRecvLen;
	socklen_t iAddrLen = sizeof(struct	sockaddr_in);
	
	*punPort = 0;
	iRecvLen = recvfrom(iSockfd, (void*)pszBuf, iLength, flags, (struct	sockaddr *) &ServAddr, &iAddrLen);
	if ( iRecvLen <= 0 )
	{
		pszHost[0] = '\000';
		return -1;;
	}
	sprintf(pszHost, "%s", inet_ntoa( ServAddr.sin_addr ));
	if (strcmp(pszHost,"0.0.0.0") == 0)
	{
		pszHost[0] = 0;
		return -2;
	}
	*punPort = ntohs( ServAddr.sin_port);
	return iRecvLen;
}

int SendToConns(SocketContext *pstSocketContext)
{
    int             iBytesSended = 0;

    time(&pstSocketContext->lLastHelloTime);

	iBytesSended = send(pstSocketContext->iSocket,pstSocketContext->sSendBuffer,pstSocketContext->shBytesToSend,0);
    if (iBytesSended<0)
    {
		if (errno==EINTR||errno==EAGAIN)
		{
			return 0;
		}
		return -1;
    }
	PopToNull(pstSocketContext,iBytesSended,1);
    return iBytesSended;
}

#define Min(a,b) (((a)<(b))?(a):(b))
int ReadFromConns(SocketContext	*pstSocketContext)
{
	char		szRecvBuffer[5120];
	int			iRecvLen, iRecvBufLen;

    time(&pstSocketContext->lLastHelloTime);

    iRecvBufLen = Min(5120, sizeof(pstSocketContext->sRecvBuffer)-pstSocketContext->shBytesReceived);
	
    // if recv buffer is full, iRecvBufLen will eq 0
	if (iRecvBufLen==0)
		return 0;
	iRecvLen = recv(pstSocketContext->iSocket,szRecvBuffer,iRecvBufLen,0);
	if (iRecvLen<=0)
    {
        if (iRecvLen<0 && (errno==EINTR||errno==EAGAIN))
        {
            return 0;
        }
        return -1;
    }
	Push(pstSocketContext,szRecvBuffer,iRecvLen,0);
	
    return iRecvLen;
}

int connect_to_server(SocketContext *pstClient, u_int32_t uiIpAddress, u_int16_t unPort, char *psErrMsg)
{
	struct sockaddr_in remote;
	int		n;
	
	pstClient->iSocket = -1;
	pstClient->iSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (pstClient->iSocket < 0 )
	{
		sprintf(psErrMsg,"Can't Create Socket Handle");
		goto ret_err;
	}
	
	if (TCP_Nodelay(pstClient->iSocket)==-1)
	{
		sprintf(psErrMsg, "cann't may socket non block");
		goto ret_err;
	}

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = uiIpAddress;
	remote.sin_port = htons(unPort);
	errno = 0;
	if ((n = connect(pstClient->iSocket,(struct sockaddr *)&remote,sizeof(remote)) ) < 0 )
	{
		if (errno != EINPROGRESS)
		{
			sprintf(psErrMsg, "Can't Connect To DirServer:%s",strerror(errno));
			goto ret_err;
		}
		pstClient->iStatus = CS_CONNECTTING;
		return 0;
	}
	pstClient->iStatus = CS_CONNECT;
	return 0;
ret_err:
	if (pstClient->iSocket>=0)
		close(pstClient->iSocket);
	pstClient->iSocket = -1;
	return -1;
}

int check_connection_status(SocketContext *pstClient, int iTimeOut, char *psErrMsg)
{
	struct timeval tval;
	fd_set rset,wset;
	int	   n , error = 0;
	socklen_t len;
	
	FD_ZERO(&rset);
	FD_SET(pstClient->iSocket,&rset);
	wset = rset;
	tval.tv_sec = iTimeOut;
	tval.tv_usec = 0;

	if ((n = select(pstClient->iSocket + 1, NULL , &wset , NULL , &tval)) == 0)
	{
		sprintf(psErrMsg,"Connect Time Out");
		goto ret_err;
	}

	if (FD_ISSET(pstClient->iSocket,&wset))
	{
		len = sizeof(error);
		if (getsockopt(pstClient->iSocket,SOL_SOCKET,SO_ERROR,&error,&len) < 0)
		{
			sprintf(psErrMsg, "Fatal Error:%s",strerror(errno));
			goto ret_err;
		}
		if (error)
		{
			sprintf(psErrMsg, "Connect Error:%s",strerror(error));
			goto ret_err;
		}
		pstClient->iStatus = CS_CONNECT;
		return 0;
	}
	
	sprintf(psErrMsg,"Select Error:socket not set");
ret_err:
	close(pstClient->iSocket);
	pstClient->iSocket = -1;
	pstClient->iStatus = CS_NOT_CONNECT;
	return -1;
}

int connect_with_timeout(SocketContext *pstClient,const char *pszIpAddress,u_int16_t unPort,int iTimeOut,char *psErrMsg)
{
	if (connect_to_server(pstClient,inet_addr(pszIpAddress),unPort,psErrMsg)==-1)
		return -1;
	return check_connection_status(pstClient,iTimeOut,psErrMsg);
}

//////////////////////////////////////////////////////////////////////////////////
/* Write "n" bytes to a descriptor. */
int send_with_timeout(int fd, const void *vptr, size_t n,int timeout)
{
        size_t          nleft;
        ssize_t         nwritten;
        const char      *ptr;
        time_t          begin = time(NULL),now;
        struct timeval  stTimeVal;
        fd_set          writefds;
        int                     ret;

        ptr = (char*)vptr;
        nleft = n;
        while (nleft > 0) {
                if (timeout!=-1)
                {
                        now = time(NULL);
                        errno = -ETIMEDOUT;
                        if (difftime(now,begin)>=timeout)
                                return -1;
                }
                FD_ZERO(&writefds);
                FD_SET(fd,&writefds);
                stTimeVal.tv_sec = 1;
                stTimeVal.tv_usec= 0;
                ret = select(fd+1,NULL,&writefds,NULL,&stTimeVal);
                if ( ret<=0 )
                        continue;
                if ( (nwritten = send(fd, (void*)ptr, nleft,0)) <= 0) {
                        if (errno == EINTR)
                                nwritten = 0;           /* and call write() again */
                        else
                                return(-1);                     /* error */
                }

                nleft -= nwritten;
                ptr   += nwritten;
        }
        return(n);
}

//////////////////////////////////////////////////////////////////////////////////
int read_with_timeout(int iSockfd, char *psBuffer, size_t iBufferSize, int iTimeOut)
{
	fd_set readfds;
	struct timeval tv;
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	tv.tv_sec = iTimeOut;
	tv.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(iSockfd, &readfds);
	
	ptr = (char*)psBuffer;
	nleft = iBufferSize;

	while (nleft > 0) 
	{
		if (select(iSockfd+1, &readfds, NULL, NULL, &tv) > 0)
		{
			if(FD_ISSET(iSockfd, &readfds))
			{
				nread = read(iSockfd, ptr, nleft);
				if (nread < 0)
				{
					if (errno == EINTR)
						nread = 0;		/* and call read() again */
					else
						return(-1);
				}
				if (nread == 0)
					break;
				nleft -= nread;
				ptr   += nread;
			}
		}
	}
	return(iBufferSize - nleft);
}

int WaitForEvent(int iReadSocket, int iWriteSocket, int iTimeOut)
{
	fd_set stReadFds, stWriteFds;
	struct timeval stTimeTv;
	int    iRetCode, iMaxSocket;

	if (iReadSocket < 0 && iWriteSocket < 0)		return -1;
	stTimeTv.tv_sec = iTimeOut;
	stTimeTv.tv_usec = 0;
	FD_ZERO(&stReadFds);
	FD_ZERO(&stWriteFds);
	
	if (iReadSocket  > -1)
		FD_SET(iReadSocket, &stReadFds);
	if (iWriteSocket > -1)
		FD_SET(iWriteSocket, &stWriteFds);
	iMaxSocket = (iReadSocket>iWriteSocket)?iReadSocket:iWriteSocket;
	while(1)
	{
		iRetCode = select(iMaxSocket+1, (iReadSocket>-1)?(&stReadFds):NULL, 									(iWriteSocket>-1)?(&stWriteFds):NULL, NULL, &stTimeTv);
		if (iRetCode < 0)
		{
			if (errno == EINTR)
				continue;
			else
				return -1;
		}
		return iRetCode;
	}
	return -1;
}

