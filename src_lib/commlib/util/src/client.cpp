#include "client.h"
#include "log.h"
#include "tlib_log.h"

using namespace std;

using namespace boss;
using namespace boss::net;


#define DBG_LOG(args...) printf(args);printf("\n");

Client::Client():m_iSvrPort(0),m_iSockFd(-1)
{
    memset( m_sSvrIP, 0, sizeof( m_sSvrIP ) );
}

int Client::init(const char *pSvrIP, int iSvrPort )
{
    if ( !pSvrIP || iSvrPort < 0 )
        return NET_PARA;
    strcpy( m_sSvrIP, pSvrIP );
    m_iSvrPort = iSvrPort;
    return 0;
}

int Client::conn()
{
    int iRet;
    int iSockFd;
    struct sockaddr_in stAddr;
    struct linger rLinger;

    clean();
    iSockFd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( iSockFd < 0 )
        return NET_CREATE_ERR;
    memset( &stAddr, 0, sizeof( stAddr ) );
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons( m_iSvrPort );
    stAddr.sin_addr.s_addr = inet_addr( m_sSvrIP );

    iRet = connect( iSockFd, ( struct sockaddr * ) & stAddr, sizeof( stAddr ) );
    if ( iRet < 0 )
        return NET_CONN_ERR;

    m_iSockFd = iSockFd;
    rLinger.l_onoff = 1;
    rLinger.l_linger = 0;
    setsockopt( m_iSockFd, SOL_SOCKET, SO_LINGER, ( char * ) & rLinger, sizeof( rLinger ) );
    return m_iSockFd;
}

int Client::send(const char *pBuff, int iSendLen, int iTimeOut )
{
    int iRet;
    int iAlreadyWriteLen = 0;
    int iThisTimeWriteLen;
    struct timeval tTimeOut;
    fd_set wSet;

    if ( pBuff == NULL || iSendLen <= 0 )
        return NET_PARA;
    tTimeOut.tv_sec = iTimeOut;
    tTimeOut.tv_usec = 0;

    while ( true ) {
        FD_ZERO( &wSet );
        FD_SET( m_iSockFd, &wSet );
        iThisTimeWriteLen = 0;
        iRet = select( m_iSockFd + 1, NULL, &wSet, NULL, &tTimeOut );
        if ( iRet < 0 ) {
            if ( errno == EINTR ) {
                continue;
            }

            return NET_SELECT_ERR;
        }
        if ( iRet == 0 )
            return NET_TIMEOUT;

        if ( FD_ISSET( m_iSockFd, &wSet ) ) {
            iThisTimeWriteLen = write( m_iSockFd, pBuff + iAlreadyWriteLen,
                                       iSendLen - iAlreadyWriteLen );
            if ( iThisTimeWriteLen < 0 )
                return NET_WRITE_ERR;

            iAlreadyWriteLen += iThisTimeWriteLen;
            if ( iAlreadyWriteLen >= iSendLen ) {
                break;
            } else {
                continue;
            }
        } else {
            return NET_SELECT_ERR;
        }
    }
    return iAlreadyWriteLen;
}

int Client::recv( char *pBuff, int iRecvLen, int iTimeOut )
{
    int iRet;
    int iAlreadyReadLen = 0;
    int iThisTimeReadLen;
    fd_set rSet;
    struct timeval tTimeOut;

    tTimeOut.tv_sec = iTimeOut;
    tTimeOut.tv_usec = 0;

    while ( true ) {
        FD_ZERO( &rSet );
        FD_SET( m_iSockFd, &rSet );

        iRet = select( m_iSockFd + 1, &rSet, NULL, NULL, &tTimeOut );
        if ( iRet < 0 ) {
            if ( errno == EINTR ) {
                continue;
            } else {
                return NET_SELECT_ERR;
            }
        }
        if ( iRet == 0 ) {
            return NET_TIMEOUT;
        }
        if ( FD_ISSET( m_iSockFd, &rSet ) ) {
            iThisTimeReadLen = read( m_iSockFd, pBuff + iAlreadyReadLen, iRecvLen - iAlreadyReadLen );
            if ( iThisTimeReadLen == 0 ) {//对方已关闭
                DBG_LOG("peer closed,read:%d,%d",m_iSockFd,iAlreadyReadLen);
                return NET_PEER_CLOSE;
            }
            if ( iThisTimeReadLen < 0 ) {
                DBG_LOG("read err,socket:%d,msg=%s[%d]",m_iSockFd,strerror(errno),errno);
                return NET_READ_ERR;
            }
            iAlreadyReadLen += iThisTimeReadLen;
            if ( iAlreadyReadLen >= iRecvLen ) {
                break;
            } else {
                continue;
            }
        } else {
            return NET_SELECT_ERR;
        }
    }
    return iAlreadyReadLen;
}

void Client::clean()
{
    if ( m_iSockFd >= 0 ) {
        close( m_iSockFd );
        m_iSockFd = -1;
    }
}

Client::~Client()
{
    clean();
}

int Client::recvsome(char*  pvRecvBuf,
                     size_t nMaxBytesToRead,
                     int    nTimeout)
{
    int nRetSel = 0;
    int nRead = 0;

    nRetSel = myselect(m_iSockFd, nTimeout);
    if ( nRetSel == 0 ) {
        return 0;
    }
    if ( nRetSel <0 ) {
        return -1;
    }
    if ( (nRead = read(m_iSockFd, pvRecvBuf, nMaxBytesToRead)) < 0 ) {
        if ( errno == EINTR ) {
            return 0;
        } else {
            return NET_READ_ERR;
        }
    }
    if ( nRead == 0 ) {
        /*select >0 , read=0, means network error*/
        return NET_PEER_CLOSE;
    }

    return nRead;
}

int Client::myselect(int  nSockfd, int  nTimeout)
{
    int     nRet;
    fd_set  fds;

    struct timeval timeout;
    struct timeval *prTimeout;

    if ( nSockfd < 0 )  return -1;

    prTimeout = &timeout;
    FD_ZERO(&fds);
    FD_SET((unsigned int)nSockfd, &fds);

    if ( nTimeout < 0 ) {
        prTimeout = NULL; 
    } else {
        prTimeout->tv_sec = nTimeout;
        prTimeout->tv_usec = 0;
    }

    nRet =select(nSockfd+1, &fds, NULL, NULL, prTimeout);
    return nRet;
}


//财富通专用发送接口函数,财富通收发的内容一定是可打印文本串
int Client::sndrcv_cft(const std::string & req, std::string & resp)
{   
    int iReqLen = req.length();
    char * buf = NULL;
    /*iReqLen=htonl(iReqLen);财富通没转*/
    int iRet = 0;

    do {
        if ( this->send((char *)&iReqLen,sizeof(iReqLen),10) < 0 ||
             this->send(req.c_str(),req.length(),10) < 0 ) {
            iRet =  NET_WRITE_ERR;
            break;
        }

        //收回包
        int iRespLen = 0;
        if ( this->recv((char*)&iRespLen,sizeof(iRespLen),10) < 0 ) {
            iRet =   NET_READ_ERR;
            break;
        }

        /*iRespLen = ntohl(iRespLen); 财富通没转*/
        if(iRespLen > 10*1024) {//太大了，可能是个错误
            buf = new char[1024 + 1];
            memset(buf,0x0,1024 + 1);
            if ( this->recvsome(buf,1024,10) < 0 ) {
                iRet = NET_READ_ERR;
                break;
            }
        }else{
            buf = new char[iRespLen + 1];
            memset(buf,0x0,iRespLen + 1);
            if ( this->recv(buf,iRespLen,10) < 0 ) {
                iRet = NET_READ_ERR;
                break;
            }
        }
    } while ( 0 );
     
    if ( buf ) {
        resp = buf;
        delete [] buf;
        buf = NULL;
    }else{
        resp = "";
    }

    return iRet;
}
