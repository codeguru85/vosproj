#include "get_qqfriends.h"
#include <net.h>

QQFriendsGetter::~QQFriendsGetter()
{
    if(m_stSendInfo.iSockfd >= 0)
    {
        close(m_stSendInfo.iSockfd);
	    m_stSendInfo.iSockfd = -1;
    }
}

int QQFriendsGetter::Open(const string &sDestIP, const int iDestPort, const string &sLocalAddr)
{
    if(m_bInit)
		return 0;

    m_sLocalAddr = sLocalAddr;
    m_sDestAddr = sDestIP;
	m_iDestPort = iDestPort;

	/*配置接收socket 的信息*/
	m_stSendInfo.stToAddress.sin_family = AF_INET;
	m_stSendInfo.stToAddress.sin_port = htons(iDestPort);
	if(inet_pton(AF_INET, sDestIP.c_str(), &(m_stSendInfo.stToAddress.sin_addr))<0)
	{
		BOSS_ERROR("Wrong DestIP addresss:%s", m_sDestAddr.c_str());
		return -1;
	}

    m_bInit = true;

	return 0 ;
}

int QQFriendsGetter::MakeGetFriendsReqPkg(const u_int32_t uiDestUin)
{
    static bool bReqPkgInit = false;

	if(!bReqPkgInit)
	{
	    memset(&m_stReq, 0, sizeof(m_stReq));

		m_stReq.cStx = 0x02;
		m_stReq.cEtx = 0x03;

		m_stReq.stHead.sLength = htons(sizeof(TransReqPkg));
		m_stReq.stHead.sVersion = htons(5);
		m_stReq.stHead.sCommand = htons(0x61);

		snprintf(m_stReq.stHead.stAssistInfo.sUserName, sizeof(m_stReq.stHead.stAssistInfo.sUserName), "%s", "ivanqiu");
		snprintf(m_stReq.stHead.stAssistInfo.sServiceName, sizeof(m_stReq.stHead.stAssistInfo.sServiceName), "%s", "C2C");

	    m_stReq.stHead.stAssistInfo.sServiceIP.s_addr = inet_addr(m_sLocalAddr.c_str());
	    m_stReq.stHead.stAssistInfo.sClientIP.s_addr = inet_addr(m_sLocalAddr.c_str());

		bReqPkgInit = true;
	}

	m_stReq.stHead.sUin = htonl(uiDestUin);

    return 0;
}

int QQFriendsGetter::SendAndRecv(int iTimeOut)
{
    int iSockFD = -1;
	fd_set stFDSet;
	struct timeval  stTimeVal;
	int iRecvLen = 0 ;


 	iSockFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSockFD < 0)
	{
	    BOSS_ERROR("socket Error:%s", strerror(errno));
	    return -1 ;
	}

	SetNBlock(iSockFD);


    /*设置超时时间*/
	stTimeVal.tv_sec = iTimeOut;
	stTimeVal.tv_usec = 0;

	int iSendLen = sendto(iSockFD, &m_stReq, sizeof(m_stReq), 0, (struct sockaddr *)&m_stSendInfo.stToAddress, sizeof(m_stSendInfo.stToAddress));
	if(iSendLen != sizeof(m_stReq))
	{
		BOSS_ERROR("sendto Error:%s", strerror(errno));
		close(iSockFD);
		iSockFD = -1;
		return -1;
	}

	FD_ZERO(&stFDSet);
	FD_SET(iSockFD, &stFDSet);
	if(select(iSockFD + 1, &stFDSet, NULL, NULL, &stTimeVal) <= 0)
	{
		BOSS_ERROR("等待服务器响应超时[%s:%d]", m_sDestAddr.c_str(), m_iDestPort);
		close(iSockFD);
		iSockFD = -1;
		return -1;
	}

	if(FD_ISSET(iSockFD, &(stFDSet)))
	{
	    memset(&m_stResp, 0, sizeof(m_stResp));

	    iRecvLen = sizeof(m_stResp);
		if((iRecvLen = recvfrom(iSockFD, &m_stResp, iRecvLen, 0, NULL, NULL)) <= 0)
		{
			BOSS_ERROR("recvfrom Error:%s", strerror(errno));
            close(iSockFD);
			iSockFD = -1;
			return -1;
		}
	}
	else
	{
	    close(iSockFD);
		iSockFD = -1;
		return -1;
	}

	close(iSockFD);
	iSockFD = -1;
	return 0 ;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQFriendsGetter::ParseGetFriendsRespPkg(const u_int32_t uiDestUin, list<u_int32_t> &listFriends)
{
	if(0x02 != m_stResp.cStx)
 	{
 	    BOSS_ERROR("包头错误");
		return -1;
 	}

	if(uiDestUin != ntohl(m_stResp.stHead.sUin))
	{
	    BOSS_ERROR("号码不匹配");
		return -1;
	}

	if(5 != ntohs(m_stResp.stHead.sVersion))
	{
	    BOSS_ERROR("版本号不匹配");
		return -1;
	}

	if(0x61 != ntohs(m_stResp.stHead.sCommand))
	{
	    BOSS_ERROR("命令字不匹配");
		return -1;
	}

	m_stResp.sTotal = ntohs(m_stResp.sTotal);
	if(m_stResp.sTotal > 1000)
	{
	    BOSS_ERROR("好友个数超1000");
		return -1;
	}

	unsigned short usPkgLen = sizeof(TransRespPkg) - (2000 - m_stResp.sTotal) * sizeof(unsigned long);
	if(usPkgLen != ntohs(m_stResp.stHead.sLength))
	{
	    //BOSS_ERROR("数据包长度属性错误");
		return -1;
	}

	if(((char*)&m_stResp)[usPkgLen - 1] != 0x03)
	{
	    BOSS_ERROR("包尾错误");
		return -1;
	}

	//停止好友服务
	if(0x64 == m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]停止好友服务", uiDestUin);
		return -1;
	}

	for(unsigned short i = 0; i < m_stResp.sTotal; i++)
	{
	    listFriends.push_back(ntohl(m_stResp.lUin[i]));
	}

	return 0;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQFriendsGetter::GetFriends(const u_int32_t uiDestUin, list<u_int32_t> &listFriends)
{
    assert(m_bInit);

	listFriends.clear();

    MakeGetFriendsReqPkg(uiDestUin);

    if(SendAndRecv(2) != 0)
		return -1;

	return ParseGetFriendsRespPkg(uiDestUin, listFriends);
}

