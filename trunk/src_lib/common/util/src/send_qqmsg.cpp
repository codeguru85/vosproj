#include "send_qqmsg.h"
#include "log.h"

#pragma pack(1)
typedef struct
{
    char      	m_cSTX;
    u_int16_t 	m_usLength;
	char        m_cCommand;
	u_int16_t   m_usServerID;
    char        m_sClientIP[16];
	char        m_sClientPort[6];
	u_int16_t   m_usType;
	u_int16_t   m_usSeqNo;
    char        m_sFromUin[10];
	char        m_sToUin[10];
    char        m_cFlag;
	char        m_sMsgID[10];
	char        m_sTemplateID[10];
	char        m_sVerifyCode[4];
	char        m_sMsgContent[MAX_PAYLOAD_LEN];
	char        m_cETX;
}STMsgPkg;

typedef struct
{
    char      	m_cSTX;
    u_int16_t 	m_usLength;
	char        m_cCommand;
	u_int16_t   m_usServerID;
    char        m_sClientIP[16];
	char        m_sClientPort[6];
	u_int16_t   m_usType;
	u_int16_t   m_usSeqNo;
    char        m_sFromUin[10];
	char        m_sToUin[10];
    char        m_cMainType;
	char        m_cSubType;
	u_int16_t   m_usStructLen;
	u_int32_t   m_uiMiniPortalID;
	u_int32_t   m_uiFlashTime;
	char        m_cETX;
}STPaiFlashPkg;

#pragma pack()

MsgVariable::MsgVariable()
{
    m_sName = m_sValue = "";
}

MsgVariable::MsgVariable(const string &sName, const string &sValue)
{
    m_sName = sName;
	m_sValue = sValue;
}

string MsgVariable::GetName() const
{
    return string(m_sName);
}

void MsgVariable::SetName(const string &sName)
{
    m_sName = sName;
}

string MsgVariable::GetValue() const
{
    return string(m_sValue);
}

void MsgVariable::SetValue(const string &sValue)
{
    m_sValue = sValue;
}


MsgVariables::MsgVariables()
{
    m_uiDataLen = 0;

	memset(m_sData, 0, sizeof(m_sData));
}

void MsgVariables::Clear()
{
    m_uiDataLen = 0;

	memset(m_sData, 0, sizeof(m_sData));
}

u_int16_t MsgVariables::GetPayloadLen() const
{
    return ntohs(m_uiDataLen);
}

int MsgVariables::AddVariable(const string &sName, const string &sValue)
{
    u_int16_t uiDataLen = ntohs(m_uiDataLen);

    if(uiDataLen >= sizeof(m_sData))
    {
        //BOSS_ERROR("无剩余存放空间!");
		return -1;
    }

	static char sVariable[1024];
	snprintf(sVariable, sizeof(sVariable), "%s=%s", sName.c_str(), sValue.c_str());

    u_int16_t uiNewVariableLen = strlen(sVariable);
    if(uiNewVariableLen > 255)
    {
        //BOSS_ERROR("变量[%s]长度超过255个字节", sVariable);
		return -1;
    }

	u_int16_t uiLeftDataLen = sizeof(m_sData) - uiDataLen;

    if((uiNewVariableLen + 1) > uiLeftDataLen)
    {
        //BOSS_ERROR("剩余空间不足以存放变量[%s]", sVariable);
		return -1;
    }

	m_sData[uiDataLen] = (u_int8_t)uiNewVariableLen;
	uiDataLen++;

    memcpy(m_sData + uiDataLen, sVariable, uiNewVariableLen);
	uiDataLen += uiNewVariableLen;

    m_uiDataLen = htons(uiDataLen);

	return 0;
}

int MsgVariables::AddVariable(const MsgVariable &cMsgVariable)
{
    return AddVariable(cMsgVariable.GetName(), cMsgVariable.GetValue());
}

int MsgVariables::GetVariables(list<MsgVariable> &listMsgVariables)
{
    static char sVariable[1024];

    listMsgVariables.clear();

    u_int16_t uiDataLen = ntohs(m_uiDataLen);

    u_int16_t uiIndex = 0;

	while(uiIndex < uiDataLen)
	{
        u_int8_t uiVariableLen = m_sData[uiIndex];

		uiIndex++;

		memcpy(sVariable, m_sData + uiIndex, uiVariableLen);
		sVariable[uiVariableLen] = '\0';

		uiIndex += uiVariableLen;

		if(uiIndex > uiDataLen)
		{
		    //BOSS_ERROR("解析得到数据长度[%u] > 标识的长度[%u]", uiIndex, uiDataLen);
			return -1;
		}

        char *sSep = strstr(sVariable, "=");
		if(NULL == sSep)
        {
            //BOSS_ERROR("变量[%s]不符合Name=Value格式", sVariable);
			return -1;
        }

		char *sName = sVariable;
		char *sValue = sSep + 1;
		*sSep = '\0';

		MsgVariable cMsgVariable(sName, sValue);

		listMsgVariables.push_back(cMsgVariable);
	}

	return 0;
}

MsgSender::~MsgSender()
{
    if(m_stSendInfo.iSockfd >= 0)
    {
        close(m_stSendInfo.iSockfd);
	    m_stSendInfo.iSockfd = -1;
    }
}

int MsgSender::Open(const string &sIP, const int iPort)
{
    if(m_bInit)
		return 0;
//
//BOSS_ERROR("%s,%d",sIP.c_str(),iPort);
//
    m_stSendInfo.iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_stSendInfo.iSockfd < 0)
	{
		//BOSS_ERROR("**create Sendsocket faile,Sockfd =%d**",m_stSendInfo.iSockfd);
		return -1;
	}

	/*配置接收socket 的信息*/
	m_stSendInfo.stToAddress.sin_family = AF_INET;
	m_stSendInfo.stToAddress.sin_port = htons(iPort);
	if(inet_pton(AF_INET, sIP.c_str(), &(m_stSendInfo.stToAddress.sin_addr))<0)
	{
		//BOSS_ERROR("wrong acceptIP addresss");
		return -1;
	}

    m_bInit = true;

	return 0 ;
}

void MsgSender::SendMsg(const u_int16_t uiMsgType, const u_int32_t uiDestUin, const u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables, char cCommand)
{
    assert(m_bInit && sVerifyCode);

   	static STMsgPkg     stMsgPkg;
	static u_int16_t    usSeqNo = time(NULL) % 60000;
    static u_int16_t    usLength = 0;

    memset(&stMsgPkg, 0, sizeof(stMsgPkg));

    stMsgPkg.m_cSTX = 0x02;
	stMsgPkg.m_cCommand = cCommand;
	stMsgPkg.m_usType = htons(uiMsgType);

    stMsgPkg.m_usSeqNo = htons(usSeqNo++);

    sprintf(stMsgPkg.m_sFromUin, "%.10u", 518810000);
	sprintf(stMsgPkg.m_sToUin, "%.10u", uiDestUin);

    sprintf(stMsgPkg.m_sMsgID, "%.10u", uiMsgID);
	sprintf(stMsgPkg.m_sTemplateID, "%.10u", uiMsgID);

    sprintf(stMsgPkg.m_sVerifyCode, "%.4s", sVerifyCode);

	if(0 == cMsgVariables.GetPayloadLen())
	{
		stMsgPkg.m_cFlag = '0';
		stMsgPkg.m_sMsgContent[0] = 0x03;

        usLength = sizeof(stMsgPkg) - MAX_PAYLOAD_LEN;

		stMsgPkg.m_usLength = htons(usLength);
	}
	else
	{
	    stMsgPkg.m_cFlag = '1';

	    u_int16_t uiIndex = 0;
		u_int8_t *sDst = (u_int8_t*)stMsgPkg.m_sMsgContent;

        u_int16_t uiBodyLen = htons(2 + cMsgVariables.GetPayloadLen());
		memcpy(sDst + uiIndex, &uiBodyLen, 2);
		uiIndex += 2;

		memcpy(sDst + uiIndex, cMsgVariables.GetPayloadAddr(), cMsgVariables.GetPayloadLen());
		uiIndex += cMsgVariables.GetPayloadLen();

		stMsgPkg.m_sMsgContent[uiIndex] = 0x03;

		usLength = sizeof(stMsgPkg) - MAX_PAYLOAD_LEN + uiIndex;

		stMsgPkg.m_usLength = htons(usLength);
	}
//
//    char stmp[4000] ={0};
//   memcpy(stmp,&stMsgPkg,usLength);
//   BOSS_ERROR("upd:%d",usLength);
//   BOSS_ERROR("sizeof(stMsgPkg):%d",sizeof(stMsgPkg));
//
    sendto(m_stSendInfo.iSockfd, (char*) &stMsgPkg, usLength, 0,
			(struct sockaddr *)&(m_stSendInfo.stToAddress), sizeof(struct sockaddr_in));

    return;
}

void FuncMsgSender::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    MsgSender::SendMsg(0x18, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x01);
}

void SaleMsgSender::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    MsgSender::SendMsg(0x4C, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x01);
}

void FuncMsgSender2::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    MsgSender::SendMsg(0x4C, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x01);
}

void HolidayWishMsgSender::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    MsgSender::SendMsg(0x9A, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x17);
}

void PaiFlashMsgSender::SendMsg(const u_int32_t uiDestUin, const u_int32_t uiFlashTime)
{
    assert(m_bInit);

   	static STPaiFlashPkg     stMsgPkg;
	static u_int16_t    usSeqNo = time(NULL) % 60000;
    static u_int16_t    usLength = sizeof(stMsgPkg);

    memset(&stMsgPkg, 0, sizeof(stMsgPkg));

    stMsgPkg.m_cSTX = 0x02;
	stMsgPkg.m_cETX = 0x03;
	stMsgPkg.m_cCommand = 0x17;
	stMsgPkg.m_usType = htons(0x43);

    stMsgPkg.m_usSeqNo = htons(usSeqNo++);

    sprintf(stMsgPkg.m_sFromUin, "%.10u", 518810000);
	sprintf(stMsgPkg.m_sToUin, "%.10u", uiDestUin);
    stMsgPkg.m_cMainType = 0x01;
	stMsgPkg.m_cSubType =  0x01;
	stMsgPkg.m_uiMiniPortalID = htonl(10019);
	stMsgPkg.m_uiFlashTime = htonl(uiFlashTime);
	stMsgPkg.m_usStructLen = htons(8);
	stMsgPkg.m_usLength = htons(sizeof(stMsgPkg));

    sendto(m_stSendInfo.iSockfd, (char*) &stMsgPkg, usLength, 0,
			(struct sockaddr *)&(m_stSendInfo.stToAddress), sizeof(struct sockaddr_in));

    return;
}

void PaiFlashMsgSender::SendMsg(const u_int32_t uiDestUin, const u_int32_t uiPortalID, const u_int32_t uiFlashTime)
{
    assert(m_bInit);

   	static STPaiFlashPkg     stMsgPkg;
	static u_int16_t    usSeqNo = time(NULL) % 60000;
    static u_int16_t    usLength = sizeof(stMsgPkg);

    memset(&stMsgPkg, 0, sizeof(stMsgPkg));

    stMsgPkg.m_cSTX = 0x02;
	stMsgPkg.m_cETX = 0x03;
	stMsgPkg.m_cCommand = 0x17;
	stMsgPkg.m_usType = htons(0x43);

    stMsgPkg.m_usSeqNo = htons(usSeqNo++);

    sprintf(stMsgPkg.m_sFromUin, "%.10u", 518810000);
	sprintf(stMsgPkg.m_sToUin, "%.10u", uiDestUin);
    stMsgPkg.m_cMainType = 0x01;
	stMsgPkg.m_cSubType =  0x01;
	stMsgPkg.m_uiMiniPortalID = htonl(uiPortalID);
	stMsgPkg.m_uiFlashTime = htonl(uiFlashTime);
	stMsgPkg.m_usStructLen = htons(8);
	stMsgPkg.m_usLength = htons(sizeof(stMsgPkg));

    sendto(m_stSendInfo.iSockfd, (char*) &stMsgPkg, usLength, 0,
			(struct sockaddr *)&(m_stSendInfo.stToAddress), sizeof(struct sockaddr_in));

    return;
}

void AioMsgSender::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    MsgSender::SendMsg(0xAA, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x1);
}

