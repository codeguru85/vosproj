#include "oidb_client.h"

OIDBClient::OIDBClient()
{
    m_bInit = false;
	m_bReqPkgInit = false;
	
    memset(&m_stSendInfo, 0, sizeof(m_stSendInfo));
	m_stSendInfo.iSockfd = -1;

	memset(&m_stReq, 0, sizeof(m_stReq));
	memset(&m_stResp, 0, sizeof(m_stResp));

    
	srandom(time(NULL));
	m_uiSeq = time(NULL) + random();

	m_iRecvLen = 0;	

	memset(&m_sErrMsg, 0, sizeof(m_sErrMsg));
}

int OIDBClient::Open(const string &sDestIP, 
		                  const int iDestPort, 
		                  const string &sLocalAddr, 
		                  u_int32_t uiTimeout)
{
    if(m_bInit)
		return 0;
	
    m_sDestAddr = sDestIP;
	m_iDestPort = iDestPort;
	m_sLocalAddr = sLocalAddr;
	m_uiTimeout = uiTimeout;
	
	m_stSendInfo.stToAddress.sin_family = AF_INET;
	m_stSendInfo.stToAddress.sin_port = htons(m_iDestPort);
	if(inet_pton(AF_INET, m_sDestAddr.c_str(), &(m_stSendInfo.stToAddress.sin_addr))<0)
	{
		snprintf(m_sErrMsg, sizeof(m_sErrMsg), "Wrong DestIP addresss:%s", m_sDestAddr.c_str());
		return -1;
	}

    m_bInit = true;	

	return 0;	
}


int OIDBClient::MakeReqPkg(u_int16_t uiCmd, 
		                         char cServiceType,
		                         const string &sUserName,		                   
		                         u_int32_t uiUserID, 
		                         const char *sData, 
		                         u_int16_t uiDataLen)
{
	if(!m_bReqPkgInit)
	{
	    memset(&m_stReq, 0, sizeof(m_stReq));

		m_stReq.cStx = 0x02;

		m_stReq.stHead.sVersion = htons(5);
		snprintf(m_stReq.stHead.stAssistInfo.sServiceName, sizeof(m_stReq.stHead.stAssistInfo.sServiceName), "%s", "C2C");

	    m_stReq.stHead.stAssistInfo.sServiceIP.s_addr = inet_addr(m_sLocalAddr.c_str());
	    m_stReq.stHead.stAssistInfo.sClientIP.s_addr = inet_addr(m_sLocalAddr.c_str());

		m_bReqPkgInit = true;
	}

	// 1.设置命令字
    m_stReq.stHead.sCommand = htons(uiCmd);

	// 2.设置服务类型
    m_stReq.stHead.stAssistInfo.cServiceType = cServiceType;

	// 3.设置用户名
	snprintf(m_stReq.stHead.stAssistInfo.sUserName, sizeof(m_stReq.stHead.stAssistInfo.sUserName), "%s", sUserName.c_str());
		
    // 4.设置qq号码
	m_stReq.stHead.sUin = htonl(uiUserID);

    // 5.设置包体并计算包长
    u_int16_t uiLength = sizeof(m_stReq) - sizeof(m_stReq.sBody);
	if(sData)
	{
        memcpy(m_stReq.sBody, sData, uiDataLen);
		uiLength += uiDataLen;
	}

    // 6.设置尾部标志
	((char*)&m_stReq)[uiLength - 1] = 0x03;

    // 7.设置包长
	m_stReq.stHead.sLength = htons(uiLength);

	// 8.设置序列号
	m_stReq.stHead.stAssistInfo.sServiceSeq = htonl(++m_uiSeq);

	m_iRecvLen = 0;
	
    return 0;
}

int OIDBClient::SendAndRecv()
{
    int iSockFD = -1;
	fd_set stFDSet;
	struct timeval  stTimeVal;

	iSockFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSockFD < 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "socket Error:%s", strerror(errno));
	    return -1 ;
	}

	SetNBlock(iSockFD);

    /*设置超时时间*/
	stTimeVal.tv_sec = m_uiTimeout;
	stTimeVal.tv_usec = 0;

	int iSendLen = sendto(iSockFD, &m_stReq, ntohs(m_stReq.stHead.sLength), 0, (struct sockaddr *)&m_stSendInfo.stToAddress, sizeof(m_stSendInfo.stToAddress));
	if(iSendLen != ntohs(m_stReq.stHead.sLength))
	{
		snprintf(m_sErrMsg, sizeof(m_sErrMsg), "sendto Error:%s", strerror(errno));
		close(iSockFD);
		iSockFD = -1;
		return -1;
	}

	FD_ZERO(&stFDSet);
	FD_SET(iSockFD, &stFDSet);
	if(select(iSockFD + 1, &stFDSet, NULL, NULL, &stTimeVal) <= 0)
	{
		snprintf(m_sErrMsg, sizeof(m_sErrMsg), "等待服务器响应超时[%s:%d]", m_sDestAddr.c_str(), m_iDestPort);
		close(iSockFD);
		iSockFD = -1;
		return -1;
	}

	if(FD_ISSET(iSockFD, &(stFDSet)))
	{
	    memset(&m_stResp, 0, sizeof(m_stResp));
		if((m_iRecvLen = recvfrom(iSockFD, &m_stResp, sizeof(m_stResp), 0, NULL, NULL)) <= 0)
		{
			snprintf(m_sErrMsg, sizeof(m_sErrMsg), "recvfrom Error:%s", strerror(errno));
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
	return 0;
}

int OIDBClient::GetRespPkg(char &cResult, char *sData, u_int16_t &uiDatalen)
{
    if(m_iRecvLen <= 0)
    {
        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "no recv data 4 parse");
		return -1;
    }

	if(m_iRecvLen < (int)(sizeof(m_stResp) - sizeof(m_stResp.sBody)))
    {
		snprintf(m_sErrMsg, sizeof(m_sErrMsg), "recv data len not right[%u]", m_iRecvLen);
		return -1;
	}

	if(m_iRecvLen != ntohs(m_stResp.stHead.sLength))
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "recv data len not right[%u]", m_iRecvLen);
		return -1;
	}

	if(0x02 != m_stResp.cStx)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "start flag not right");
		return -1;
	}

	if(0x03 != ((char*)&m_stResp)[m_iRecvLen - 1])
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "end flag not right");
		return -1;
	}

	if(m_stReq.stHead.sCommand != m_stResp.stHead.sCommand)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "cmd not right");
		return -1;
	}

	if(m_stReq.stHead.sUin != m_stResp.stHead.sUin)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "uin not right");
		return -1;
	}

	if(m_stReq.stHead.stAssistInfo.sServiceSeq != m_stResp.stHead.stAssistInfo.sServiceSeq)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "seq not right");
		return -1;
	}

	if(m_stReq.stHead.stAssistInfo.cServiceType!= m_stResp.stHead.stAssistInfo.cServiceType)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "ServiceType not right");
		return -1;
	}

	cResult = m_stResp.stHead.cResult;

    if(uiDatalen < m_iRecvLen - (sizeof(m_stResp) - sizeof(m_stResp.sBody)))
    {
        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "buffer not large enough");
		return -1;
    }
	
	uiDatalen = m_iRecvLen - (sizeof(m_stResp) - sizeof(m_stResp.sBody));
	if(0 != uiDatalen && sData)
		memcpy(sData, m_stResp.sBody, uiDatalen);

	return 0;
}

const char* OIDBClient::GetErrMsg()
{
    return m_sErrMsg;
}

