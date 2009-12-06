#include "textlink_mgr.h"

TextLinkMgr::TextLinkMgr()
{
    memset(m_sErrMsg, 0, sizeof(m_sErrMsg));
	m_iTotalNum = 0;
}

const char* TextLinkMgr::GetErrMsg()
{
    return m_sErrMsg;
}

int TextLinkMgr::AddTextLink(int iClickRemainFlag, const string &sText, const string &sTip, const string &sUrl)
{
    if(sText.length() == 0)
    {
        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "Text不能为空");
		return -1;
    }
	
    int iFlag = 0;
	if(iClickRemainFlag > 0)
		iFlag |= FLAG_ONCLICK_PRESERVE;

    int iLen = 0;
    char sTextUtf8[MAXTEXT + 1];
	iLen = sizeof(sTextUtf8) - 1;
	if(tconv_gbk2utf8(sText.c_str(), sText.length(), sTextUtf8, (size_t*)&iLen) != 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "Text转换为UTF8编码失败, 允许转换后最大长度为%d", MAXTEXT);
	    return -1;
	}

	
    char sTipUTF8[MAXTIP + 1];
	iLen = sizeof(sTipUTF8) - 1;
	if(tconv_gbk2utf8(sTip.c_str(), sTip.length(), sTipUTF8, (size_t*)&iLen) != 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "Tip转换为UTF8编码失败, 允许转换后最大长度为%d", MAXTIP);
	    return -1;
	}

    char sUrlUTF8[MAXURL + 1];
	iLen = sizeof(sUrlUTF8) - 1;
	if(tconv_gbk2utf8(sUrl.c_str(), sUrl.length(), sUrlUTF8, (size_t*)&iLen) != 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "Url转换为UTF8编码失败, 允许转换后最大长度为%d", MAXURL);
	    return -1;
	}

    int iTextLinkId = AddText(iFlag, PAIPAI_SRVID, sTextUtf8, sTipUTF8, sUrlUTF8);
	BOSS_INFO("iTextLinkId = %d", iTextLinkId);	
	if(iTextLinkId < 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "AddText 调用错误 %s", GetErrorString());
		return -1;
	}

	return iTextLinkId;
}


int TextLinkMgr::DelTextLink(int iTextLinkID)
{
    if(iTextLinkID < 0)
    {
        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "待删除TextLinkID[%d]必须 >= 0", iTextLinkID);
		return -1;
    }

	if(DelText(iTextLinkID) < 0)
	{
	    snprintf(m_sErrMsg, sizeof(m_sErrMsg), "DelText 调用错误 %s", GetErrorString());
	    return -1;
	}

	return 0;
}

int TextLinkMgr::SendGroup(int iTextLinkID, u_int32_t uiGroupID, int iSleepTime, bool bFlush)
{
    static int s_aiGroupID[MAXGROUPNUM + 1];
	static int s_iNum = 0;
	static int s_iTextLinkID = -1;

	if(iTextLinkID < 0)
    {
        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "待发送TextLinkID[%d] 必须 >= 0", iTextLinkID);
		return -1;
    }

	if(iTextLinkID != s_iTextLinkID && s_iNum > 0 && s_iTextLinkID > 0)
	{
	    int iRet = AddGroup(s_iTextLinkID, s_iNum, s_aiGroupID);
	    if(iRet < 0)
	    {
	        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "发送GroupID 错误 %s", GetErrorString());
	        return iRet;
	    }
        m_iTotalNum += s_iNum;
		s_iNum = 0;
	}

    s_iTextLinkID = iTextLinkID;
    s_aiGroupID[s_iNum++] = uiGroupID;

/*
	BOSS_INFO("Num = %d", s_iNum);
	if(bFlush)
	{
	    BOSS_INFO("Last");
	}
*/	
	if(s_iNum >= MAXGROUPNUM || bFlush)
	{
	    //BOSS_INFO("Actually Send To JT");
		
	    int iRet = AddGroup(s_iTextLinkID, s_iNum, s_aiGroupID);
		usleep(iSleepTime * 1000);
	    if(iRet < 0)
	    {
	        s_iNum = 0;
	        snprintf(m_sErrMsg, sizeof(m_sErrMsg), "发送GroupID 错误 %s", GetErrorString());
	        return iRet;
	    }

		for(int i = 0; i < s_iNum; i++)
		{
		    if(iRet < 0)
				BOSS_ERROR("FAIL SEND TO %u", s_aiGroupID[i]);
			else
				BOSS_INFO("SUCC SEND TO %u", s_aiGroupID[i]);
		}

		m_iTotalNum += s_iNum;
		s_iNum = 0;
	}
	
	return 0;
}

int TextLinkMgr::GetTotalNum()
{
    return m_iTotalNum;
}

