#include "get_qqdetail.h"
#include <tlib_all.h>
#include <net.h>


void QQDetailGetter::Close()
{    
    if(m_stSendInfo.iSockfd >= 0)
    {
        struct sockaddr_in	 m_addr;
		socklen_t iLen = sizeof(m_addr);
		if(getsockname(m_stSendInfo.iSockfd, (struct sockaddr*)&m_addr, &iLen) == 0)
		{
		    BOSS_INFO("FD = %d, Port = %d", m_stSendInfo.iSockfd, ntohs(m_addr.sin_port));
		}
		else
			BOSS_ERROR("getsockname(%d): %s", m_stSendInfo.iSockfd, strerror(errno));
		
        close(m_stSendInfo.iSockfd);
	    m_stSendInfo.iSockfd = -1;
    }

}

QQDetailGetter::~QQDetailGetter()
{
    Close();
}

int QQDetailGetter::Open(const string &sDestIP, const int iDestPort, const string &sLocalAddr)
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

int QQDetailGetter::MakeGetDetailReqPkg(const u_int32_t uiDestUin)
{
    static bool bReqPkgInit = false;

	if(!bReqPkgInit)
	{
	    memset(&m_stReq, 0, sizeof(m_stReq));

		m_stReq.cStx = 0x02;
		m_stReq.cEtx = 0x03;

		m_stReq.stHead.sLength = htons(sizeof(TransReqPkg));
		m_stReq.stHead.sVersion = htons(5);
		m_stReq.stHead.sCommand = htons(0x68);

		snprintf(m_stReq.stHead.stAssistInfo.sUserName, sizeof(m_stReq.stHead.stAssistInfo.sUserName), "%s", "ivanqiu");
		snprintf(m_stReq.stHead.stAssistInfo.sServiceName, sizeof(m_stReq.stHead.stAssistInfo.sServiceName), "%s", "C2C");

	    m_stReq.stHead.stAssistInfo.sServiceIP.s_addr = inet_addr(m_sLocalAddr.c_str());
	    m_stReq.stHead.stAssistInfo.sClientIP.s_addr = inet_addr(m_sLocalAddr.c_str());

		bReqPkgInit = true;
	}

	m_stReq.stHead.sUin = htonl(uiDestUin);

    return 0;
}

int QQDetailGetter::SendAndRecv(int iTimeOutSeconds, int iTimeOutUSeconds)
{
	fd_set stFDSet;
	struct timeval  stTimeVal;
	int iRecvLen = 0 ;

    if(m_stSendInfo.iSockfd < 0)
    {
	    m_stSendInfo.iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	    if(m_stSendInfo.iSockfd < 0)
	    {
	        BOSS_ERROR("socket Error:%s", strerror(errno));
	        return -1 ;
	    }

	    SetNBlock(m_stSendInfo.iSockfd);
    }
	
    /*设置超时时间*/
	stTimeVal.tv_sec = iTimeOutSeconds;
	stTimeVal.tv_usec = iTimeOutUSeconds;

	int iSendLen = sendto(m_stSendInfo.iSockfd, &m_stReq, sizeof(m_stReq), 0, (struct sockaddr *)&m_stSendInfo.stToAddress, sizeof(m_stSendInfo.stToAddress));
	if(iSendLen != sizeof(m_stReq))
	{
		BOSS_ERROR("sendto Error:%s", strerror(errno));
		Close();
		return -1;
	}

	FD_ZERO(&stFDSet);
	FD_SET(m_stSendInfo.iSockfd, &stFDSet);

	struct timeval stStartTime, stEndTime;
	gettimeofday(&stStartTime, NULL);
	int iRet = select(m_stSendInfo.iSockfd + 1, &stFDSet, NULL, NULL, &stTimeVal);
	gettimeofday(&stEndTime, NULL);

	unsigned long long uiExpireTime = 
		(stEndTime.tv_usec + stEndTime.tv_sec * 1000000)
		-
		(stStartTime.tv_usec + stStartTime.tv_sec * 1000000);

	uiExpireTime = uiExpireTime / 1000;

	static unsigned long long uiTotalExpireTime = 0;
	static int iCount = 0;

	iCount++;
    uiTotalExpireTime += uiExpireTime;
	
	if(iCount >= 100)
	{
	    BOSS_INFO("TotalExpireTime = %lld ms, Count = %d, Avg = %lld ms", uiTotalExpireTime, iCount, uiTotalExpireTime/(unsigned long long)iCount);
	    iCount = 0;
		uiTotalExpireTime = 0;
	}
		
	if(iRet <= 0)
	{
		BOSS_ERROR("等待服务器响应超时[%s:%d]", m_sDestAddr.c_str(), m_iDestPort);
		Close();
		return -1;
	}

	if(FD_ISSET(m_stSendInfo.iSockfd, &(stFDSet)))
	{
	    memset(&m_stResp, 0, sizeof(m_stResp));

	    iRecvLen = sizeof(m_stResp);
		if((iRecvLen = recvfrom(m_stSendInfo.iSockfd, &m_stResp, iRecvLen, 0, NULL, NULL)) <= 0)
		{
			BOSS_ERROR("recvfrom Error:%s", strerror(errno));
            Close();
			return -1;
		}
	}
	else
	{
	    Close();
		return -1;
	}

	return 0;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQDetailGetter::ParseGetDetailRespPkg(const u_int32_t uiDestUin, string &sNickName)
{
	if(0x02 != m_stResp.cStx)
 	{
 	    BOSS_ERROR("包头错误");
        Close();
		return -1;
 	}

	if(uiDestUin != ntohl(m_stResp.stHead.sUin))
	{
	    BOSS_ERROR("号码不匹配[%u]", uiDestUin);
        Close();

		return -1;
	}

	if(5 != ntohs(m_stResp.stHead.sVersion))
	{
	    BOSS_ERROR("版本号不匹配");
        Close();

		return -1;
	}

	if(0x68 != ntohs(m_stResp.stHead.sCommand))
	{
	    BOSS_ERROR("命令字不匹配");
        Close();

		return -1;
	}

	if(0x11 == m_stResp.stHead.cResult || 0x12 == m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]取用户资料失败,原因是号码不存在或者系统停止服务", uiDestUin);

		return -1;
	}
	else if(0x20 == m_stResp.stHead.cResult || 0x70 == m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]账号冻结", uiDestUin);
		return -1;
	}
	else if(0x21 == m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]账号在黑名单中", uiDestUin);
		return -1;
	}
	else if(0x24 == m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]email账号未允许uin登陆", uiDestUin);
		return -1;
	}
	else if(0x00 != m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]获取用户资料失败[0X%X]", uiDestUin, (u_int8_t)m_stResp.stHead.cResult);
        Close();
		return -1;
	}

    //wFace + cAge + cGender + cNickLength + strNick + dwFlag＋cEmalLen + strEmail

    char *sData = m_stResp.sData;

	u_int16_t wFace;
	memcpy(&wFace, sData, sizeof(wFace));
	wFace = ntohs(wFace);
	sData += sizeof(wFace);

	u_int8_t cAge = *(sData++);

	u_int8_t cGender = *(sData++);

	u_int8_t cNickLen = *(sData++);
    if(cNickLen > 32)
    {
        BOSS_ERROR("昵称超过32字节");
        Close();

		return -1;
    }

	char sNick[33];
	memset(sNick, 0, sizeof(sNick));
	memcpy(sNick, sData, cNickLen);
	sData += cNickLen;

	u_int32_t dwFlag;
	memcpy(&dwFlag, sData, sizeof(dwFlag));
	dwFlag = ntohl(dwFlag);
	sData += sizeof(dwFlag);


	u_int8_t cEmailLen = *(sData++);

	char sEmail[200];
	memset(sEmail, 0, sizeof(sEmail));
	memcpy(sEmail, sData, cEmailLen);
	sData += cEmailLen;

	unsigned short uiDataLen = sData - m_stResp.sData;

	unsigned short usPkgLen = sizeof(TransRespPkg) - (sizeof(m_stResp.sData) - uiDataLen);
	if(usPkgLen != ntohs(m_stResp.stHead.sLength))
	{
	    BOSS_ERROR("数据包长度属性错误");
        Close();

		return -1;
	}

	if(((char*)&m_stResp)[usPkgLen - 1] != 0x03)
	{
	    BOSS_ERROR("包尾错误");
        Close();

		return -1;
	}

    sNickName = sNick;

	return 0;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQDetailGetter::GetQQNickName(const u_int32_t uiDestUin, string &sNickName)
{
    assert(m_bInit);

	MakeGetDetailReqPkg(uiDestUin);

    if(SendAndRecv(0, 200 * 1000) != 0)
		return -1;

	return ParseGetDetailRespPkg(uiDestUin, sNickName);
}

void QQDetailGetterV2::Close()
{    
    if(m_stSendInfo.iSockfd >= 0)
    {
        struct sockaddr_in	 m_addr;
		socklen_t iLen = sizeof(m_addr);
		if(getsockname(m_stSendInfo.iSockfd, (struct sockaddr*)&m_addr, &iLen) == 0)
		{
		    BOSS_INFO("FD = %d, Port = %d", m_stSendInfo.iSockfd, ntohs(m_addr.sin_port));
		}
		else
			BOSS_ERROR("getsockname(%d): %s", m_stSendInfo.iSockfd, strerror(errno));
		
        close(m_stSendInfo.iSockfd);
	    m_stSendInfo.iSockfd = -1;
    }

}

QQDetailGetterV2::~QQDetailGetterV2()
{
    Close();
}

int QQDetailGetterV2::Open(const string &sDestIP, const int iDestPort, const string &sLocalAddr, const string &sInvoker)
{
    if(m_bInit)
		return 0;

    m_sInvoker = sInvoker;
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

int QQDetailGetterV2::MakeGetDetailReqPkg(const u_int32_t uiDestUin)
{
    static bool bReqPkgInit = false;

	if(!bReqPkgInit)
	{
	    memset(&m_stReq, 0, sizeof(m_stReq));

		m_stReq.cStx = 0x02;
		m_stReq.cEtx = 0x03;

		m_stReq.stHead.sLength = htons(sizeof(TransReqPkg));
		m_stReq.stHead.sVersion = htons(5);
		m_stReq.stHead.sCommand = htons(0x47);

		snprintf(m_stReq.stHead.stAssistInfo.sUserName, sizeof(m_stReq.stHead.stAssistInfo.sUserName), "%s", m_sInvoker.c_str());
		snprintf(m_stReq.stHead.stAssistInfo.sServiceName, sizeof(m_stReq.stHead.stAssistInfo.sServiceName), "%s", "C2C");

	    m_stReq.stHead.stAssistInfo.sServiceIP.s_addr = inet_addr(m_sLocalAddr.c_str());
	    m_stReq.stHead.stAssistInfo.sClientIP.s_addr = inet_addr(m_sLocalAddr.c_str());

		bReqPkgInit = true;
	}

	m_stReq.stHead.sUin = htonl(uiDestUin);

    return 0;
}

int QQDetailGetterV2::SendAndRecv(int iTimeOutSeconds, int iTimeOutUSeconds)
{
	fd_set stFDSet;
	struct timeval  stTimeVal;
	int iRecvLen = 0 ;

    if(m_stSendInfo.iSockfd < 0)
    {
	    m_stSendInfo.iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	    if(m_stSendInfo.iSockfd < 0)
	    {
	        BOSS_ERROR("socket Error:%s", strerror(errno));
	        return -1 ;
	    }

	    SetNBlock(m_stSendInfo.iSockfd);
    }
	
    /*设置超时时间*/
	stTimeVal.tv_sec = iTimeOutSeconds;
	stTimeVal.tv_usec = iTimeOutUSeconds;

	int iSendLen = sendto(m_stSendInfo.iSockfd, &m_stReq, sizeof(m_stReq), 0, (struct sockaddr *)&m_stSendInfo.stToAddress, sizeof(m_stSendInfo.stToAddress));
	if(iSendLen != sizeof(m_stReq))
	{
		BOSS_ERROR("sendto Error:%s", strerror(errno));
		Close();
		return -1;
	}

	FD_ZERO(&stFDSet);
	FD_SET(m_stSendInfo.iSockfd, &stFDSet);

	struct timeval stStartTime, stEndTime;
	gettimeofday(&stStartTime, NULL);
	int iRet = select(m_stSendInfo.iSockfd + 1, &stFDSet, NULL, NULL, &stTimeVal);
	gettimeofday(&stEndTime, NULL);

	unsigned long long uiExpireTime = 
		(stEndTime.tv_usec + stEndTime.tv_sec * 1000000)
		-
		(stStartTime.tv_usec + stStartTime.tv_sec * 1000000);

	uiExpireTime = uiExpireTime / 1000;

	static unsigned long long uiTotalExpireTime = 0;
	static int iCount = 0;

	iCount++;
    uiTotalExpireTime += uiExpireTime;
	
	if(iCount >= 100)
	{
	    BOSS_INFO("TotalExpireTime = %lld ms, Count = %d, Avg = %lld ms", uiTotalExpireTime, iCount, uiTotalExpireTime/(unsigned long long)iCount);
	    iCount = 0;
		uiTotalExpireTime = 0;
	}
		
	if(iRet <= 0)
	{
		BOSS_ERROR("等待服务器响应超时[%s:%d]", m_sDestAddr.c_str(), m_iDestPort);
		Close();
		return -1;
	}

	if(FD_ISSET(m_stSendInfo.iSockfd, &(stFDSet)))
	{
	    memset(&m_stResp, 0, sizeof(m_stResp));

	    iRecvLen = sizeof(m_stResp);
		if((iRecvLen = recvfrom(m_stSendInfo.iSockfd, &m_stResp, iRecvLen, 0, NULL, NULL)) <= 0)
		{
			BOSS_ERROR("recvfrom Error:%s", strerror(errno));
            Close();
			return -1;
		}
	}
	else
	{
	    Close();
		return -1;
	}

	return 0;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQDetailGetterV2::ParseGetDetailRespPkg(const u_int32_t uiDestUin, string &sNickName)
{
	if(0x02 != m_stResp.cStx)
 	{
 	    BOSS_ERROR("包头错误");
        Close();
		return -1;
 	}

	if(uiDestUin != ntohl(m_stResp.stHead.sUin))
	{
	    BOSS_ERROR("号码不匹配[%u]", uiDestUin);
        Close();

		return -1;
	}

	if(5 != ntohs(m_stResp.stHead.sVersion))
	{
	    BOSS_ERROR("版本号不匹配");
        Close();

		return -1;
	}

	if(0x47 != ntohs(m_stResp.stHead.sCommand))
	{
	    BOSS_ERROR("命令字不匹配");
        Close();

		return -1;
	}

	if(0x00 != m_stResp.stHead.cResult)
	{
	    BOSS_ERROR("[%u]获取用户资料失败[0X%X]", uiDestUin, (u_int8_t)m_stResp.stHead.cResult);
        Close();
		return -1;
	}

    //dwUin + wFace + cAge + cGender + cNickLength + strNick

    char *sData = m_stResp.sData;

    u_int32_t uiUin;
	memcpy(&uiUin, sData, sizeof(uiUin));
	uiUin = ntohl(uiUin);
	sData += sizeof(uiUin);
	
	u_int16_t wFace;
	memcpy(&wFace, sData, sizeof(wFace));
	wFace = ntohs(wFace);
	sData += sizeof(wFace);

	u_int8_t cAge = *(sData++);

	u_int8_t cGender = *(sData++);

	u_int8_t cNickLen = *(sData++);
    if(cNickLen > 32)
    {
        BOSS_ERROR("昵称超过32字节");
        Close();

		return -1;
    }

	char sNick[33];
	memset(sNick, 0, sizeof(sNick));
	memcpy(sNick, sData, cNickLen);
	sData += cNickLen;

	unsigned short uiDataLen = sData - m_stResp.sData;

	unsigned short usPkgLen = sizeof(TransRespPkg) - (sizeof(m_stResp.sData) - uiDataLen);
	if(usPkgLen != ntohs(m_stResp.stHead.sLength))
	{
	    BOSS_ERROR("数据包长度属性错误");
        Close();

		return -1;
	}

	if(((char*)&m_stResp)[usPkgLen - 1] != 0x03)
	{
	    BOSS_ERROR("包尾错误");
        Close();

		return -1;
	}

    sNickName = sNick;

	return 0;
}

/*
返回0表示取好友列表成功
返回-1表示取好友列表暂时失败
返回1表示放弃取好友列表
*/
int QQDetailGetterV2::GetQQNickName(const u_int32_t uiDestUin, string &sNickName,int iTimeOut)
{
    assert(m_bInit);

	MakeGetDetailReqPkg(uiDestUin);

    if(SendAndRecv(0, iTimeOut * 1000) != 0)
		return -1;

	return ParseGetDetailRespPkg(uiDestUin, sNickName);
}

void QQBizBitmapGetter::Close()
{
    if(m_stSendInfo.iSockfd >= 0)
    {
        struct sockaddr_in	 m_addr;
		socklen_t iLen = sizeof(m_addr);
		if(getsockname(m_stSendInfo.iSockfd, (struct sockaddr*)&m_addr, &iLen) == 0)
		{
		    TLib_Log_LogMsg("FD = %d, Port = %d", m_stSendInfo.iSockfd, ntohs(m_addr.sin_port));
		}
		else
			TLib_Log_LogMsg("getsockname(%d): %s", m_stSendInfo.iSockfd, strerror(errno));
		
        close(m_stSendInfo.iSockfd);
	    m_stSendInfo.iSockfd = -1;
    }
}

QQBizBitmapGetter::~QQBizBitmapGetter()
{
    Close();
}

int QQBizBitmapGetter::Open(const string &sDestIP, const int iDestPort, const string &sLocalAddr, const string &sInvoker)
{
    if(m_bInit)
		return 0;

    m_sInvoker = sInvoker;
    m_sLocalAddr = sLocalAddr;
    m_sDestAddr = sDestIP;
	m_iDestPort = iDestPort;

	/*配置接收socket 的信息*/
	m_stSendInfo.stToAddress.sin_family = AF_INET;
	m_stSendInfo.stToAddress.sin_port = htons(iDestPort);
	if(inet_pton(AF_INET, sDestIP.c_str(), &(m_stSendInfo.stToAddress.sin_addr))<0)
	{
		TLib_Log_LogMsg("Wrong DestIP addresss:%s", m_sDestAddr.c_str());
		return -1;
	}

    m_bInit = true;

	return 0 ;
}

int QQBizBitmapGetter::GetQQBizBitmap(const u_int32_t uiDestUin, char* szFlag, int iTimeOut)
{
    assert(m_bInit);

	MakeGetDetailReqPkg(uiDestUin);

    if(SendAndRecv(0, iTimeOut * 1000) != 0)
		return -1;

	return ParseGetDetailRespPkg(uiDestUin, szFlag);
	
}

int QQBizBitmapGetter::MakeGetDetailReqPkg(const u_int32_t uiDestUin)
{
	
    memset(&m_stReq, 0, sizeof(m_stReq));

	m_stReq.cStx = 0x02;
	m_stReq.cEtx = 0x03;

	m_stReq.stHead.sLength = htons(sizeof(m_stReq));
	m_stReq.stHead.sVersion = htons(5);
	m_stReq.stHead.sCommand = htons(0x99);

	m_stReq.stHead.stAssistInfo.cServiceType = 1;

	snprintf(m_stReq.stHead.stAssistInfo.sUserName, sizeof(m_stReq.stHead.stAssistInfo.sUserName), "%s", m_sInvoker.c_str());
	snprintf(m_stReq.stHead.stAssistInfo.sServiceName, sizeof(m_stReq.stHead.stAssistInfo.sServiceName), "%s", "C2C");

    m_stReq.stHead.stAssistInfo.sServiceIP.s_addr 	= inet_addr(m_sLocalAddr.c_str());
    m_stReq.stHead.stAssistInfo.sClientIP.s_addr 	= inet_addr(m_sLocalAddr.c_str());

	m_stReq.cCount 	= 1;		
	
	m_stReq.dwUin   	= htonl(uiDestUin);
	m_stReq.stHead.sUin = htonl(uiDestUin);

    return 0;
}


int QQBizBitmapGetter::SendAndRecv(int iTimeOutSeconds, int iTimeOutUSeconds)
{	
	fd_set stFDSet;
	struct timeval  stTimeVal;
	int iRecvLen = 0 ;

    if(m_stSendInfo.iSockfd < 0)
    {
	    m_stSendInfo.iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	    if(m_stSendInfo.iSockfd < 0)
	    {
	        TLib_Log_LogMsg("socket Error:%s", strerror(errno));
	        return -1 ;
	    }
	    SetNBlock(m_stSendInfo.iSockfd);
    }
	
    /*设置超时时间*/
	stTimeVal.tv_sec 	= iTimeOutSeconds;
	stTimeVal.tv_usec 	= iTimeOutUSeconds;

	int iSendLen = sendto(m_stSendInfo.iSockfd, &m_stReq, sizeof(m_stReq), 0, (struct sockaddr *)&m_stSendInfo.stToAddress, sizeof(m_stSendInfo.stToAddress));
	if(iSendLen != sizeof(m_stReq))
	{
		TLib_Log_LogMsg("sendto Error:%s", strerror(errno));
		Close();
		return -1;
	}

	FD_ZERO(&stFDSet);
	FD_SET(m_stSendInfo.iSockfd, &stFDSet);

	struct timeval stStartTime, stEndTime;
	gettimeofday(&stStartTime, NULL);
	int iRet = select(m_stSendInfo.iSockfd + 1, &stFDSet, NULL, NULL, &stTimeVal);
	gettimeofday(&stEndTime, NULL);

	unsigned long long uiExpireTime = 	(stEndTime.tv_usec + stEndTime.tv_sec * 1000000)	-
										(stStartTime.tv_usec + stStartTime.tv_sec * 1000000);

	uiExpireTime = uiExpireTime / 1000;

	static unsigned long long uiTotalExpireTime = 0;
	static int iCount = 0;

	iCount++;
    uiTotalExpireTime += uiExpireTime;
	
	if(iCount >= 100)
	{
	    TLib_Log_LogMsg("TotalExpireTime = %lld ms, Count = %d, Avg = %lld ms", uiTotalExpireTime, iCount, uiTotalExpireTime/(unsigned long long)iCount);
	    iCount = 0;
		uiTotalExpireTime = 0;
	}
		
	if(iRet <= 0)
	{
		TLib_Log_LogMsg("等待服务器响应超时[%s:%d], ret = %d", m_sDestAddr.c_str(), m_iDestPort, iRet);
		Close();
		return -1;
	}

	if(FD_ISSET(m_stSendInfo.iSockfd, &(stFDSet)))
	{
	    memset(&m_stResp, 0, sizeof(m_stResp));

	    iRecvLen = sizeof(m_stResp);
		if((iRecvLen = recvfrom(m_stSendInfo.iSockfd, &m_stResp, iRecvLen, 0, NULL, NULL)) <= 0)
		{
			TLib_Log_LogMsg("recvfrom Error:%s", strerror(errno));
            Close();
			return -1;
		}
	}
	else
	{
	    Close();
		return -1;
	}

	return 0;
}

int QQBizBitmapGetter::ParseGetDetailRespPkg(const u_int32_t uiDestUin, char* szFlag)
{
	if(0x02 != m_stResp.cStx)
 	{
 	    TLib_Log_LogMsg("包头错误");
        Close();
		return -1;
 	}

	if(uiDestUin != ntohl(m_stResp.stHead.sUin))
	{
	    TLib_Log_LogMsg("号码不匹配[%u]", uiDestUin);
        Close();

		return -1;
	}

	if(5 != ntohs(m_stResp.stHead.sVersion))
	{
	    TLib_Log_LogMsg("版本号不匹配");
        Close();

		return -1;
	}

	if(0x99 != ntohs(m_stResp.stHead.sCommand))
	{
	    TLib_Log_LogMsg("命令字不匹配");
        Close();

		return -1;
	}

	if(0x00 != m_stResp.stHead.cResult)
	{
	    TLib_Log_LogMsg("[%u]获取用户资料失败[0X%X]", uiDestUin, (u_int8_t)m_stResp.stHead.cResult);
        Close();
		return -1;
	}

	if(sizeof(m_stResp) != ntohs(m_stResp.stHead.sLength))
	{
	    TLib_Log_LogMsg("包体尺寸不匹配");
        Close();
		return -1;	
	}
	
	//char			cDoCount		= m_stResp.cDoCount;	
	//unsigned long   dwNextBeginUin	= m_stResp.dwNextBeginUin;	
	//unsigned long   sUin			= m_stResp.sUin;            //用户号码 unsigned long	
	
	TLib_Log_LogMsg("[%u][获取用户资料]: \t%4x%4x%4x", uiDestUin, 													  
													  m_stResp.sData[8],
													  m_stResp.sData[9],
													  m_stResp.sData[10]);
	//
	memcpy(szFlag, m_stResp.sData, sizeof(m_stResp.sData));
	
	return 0;

}

