#include "send_qqmsg.h"
#include "send_qqmsg_v2.h"
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

#pragma pack()

DnsMsgSender::~DnsMsgSender()
{
}

int DnsMsgSender::Open(const string &sDnsIP1, 
		                  const int iDnsPort1, 
		                  const string &sDnsIP2, 
		                  const int iDnsPort2, 
		                  const string &sDefaultSrvIP, 
		                  const int iDefaultSrvPort)
{
    if(m_bInit)
		return 0;

	m_cSrv.SetDnsAddress(sDnsIP1.c_str(), iDnsPort1, sDnsIP2.c_str(), iDnsPort2, 3);
    m_cSrv.SetDefaultAddress(sDefaultSrvIP.c_str(), iDefaultSrvPort);
    BOSS_INFO("sDnsIP1=%s, iDnsPort1=%d, sDnsIP2=%s, iDnsPort2=%d, sDefaultSrvIP=%s, iDefaultSrvPort=%d", 
		       sDnsIP1.c_str(), 
		       iDnsPort1, 
		       sDnsIP2.c_str(), 
		       iDnsPort2,
		       sDefaultSrvIP.c_str(), 
		       iDefaultSrvPort);

    m_bInit = true;

	return 0 ;
}

void DnsMsgSender::SendMsg(const u_int16_t uiMsgType, const u_int32_t uiDestUin, const u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables, char cCommand)
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

    if(m_cSrv.SendMsg((char*) &stMsgPkg, usLength) != 0)
		BOSS_ERROR("SendMsg failed: %s\n", m_cSrv.GetErrMsg());

    return;
}

void DnsFuncMsgSender2::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    DnsMsgSender::SendMsg(0x4C, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x01);
}

void DnsAioMsgSender::SendMsg(const u_int32_t uiDestUin, u_int32_t uiMsgID, const char *sVerifyCode, const MsgVariables  &cMsgVariables)
{
    DnsMsgSender::SendMsg(0xAA, uiDestUin, uiMsgID, sVerifyCode, cMsgVariables, 0x1);
}

