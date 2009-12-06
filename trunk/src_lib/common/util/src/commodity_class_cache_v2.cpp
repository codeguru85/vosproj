#include "commodity_class_cache_v2.h"
#include <algorithm>

#include <string>

using namespace std;

int CommodityClassFullPathV2::IsRoot()
{
    if(1 == m_uiClassLevel)
		return 1;
	else
		return 0;
}


int CommodityClassFullPathV2::IsLeaf()
{
    if(m_uiLeafID > 0)
		return 1;
	else
		return 0;
}


CommodityClassDBOperatorV2::CommodityClassDBOperatorV2():m_cDBTrans()
{
}

CommodityClassDBOperatorV2::~CommodityClassDBOperatorV2()
{
    m_cDBTrans.Close();
}

int CommodityClassDBOperatorV2::Init(const std::string &sDBProxyAddr, const int iDBProxyPort)
{
    if(m_cDBTrans.Init(sDBProxyAddr.c_str(), iDBProxyPort) < 0)
	{
	    BOSS_ERROR("�������ݿ�������: %s",m_cDBTrans.GetErrMsg());
		return -1;
	}

	return 0;
}

int CommodityClassDBOperatorV2::GetClassNameMap(std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName)
{
    char sSql[1024];

	snprintf(sSql, sizeof(sSql), "SELECT Parentid, Classid, ClassName FROM BossCommodityClass");

	try
	{
	    m_cDBTrans.ExecSQL(sSql, "centerdb_oracle");

        int iRowNum = m_cDBTrans.GetRowNum();

		for(int i = 0; i < iRowNum; i++)
		{
		    DBRow  stRow;

			stRow=m_cDBTrans.FetchRow();
            if("" == stRow.stDBFields[0].sFieldValue)
			    continue;

            u_int32_t uiParentID = strtoul(stRow.stDBFields[0].sFieldValue.c_str(), NULL, 10);
		    u_int32_t uiClassID = strtoul(stRow.stDBFields[1].sFieldValue.c_str(), NULL, 10);

			mapClassName[uiParentID][uiClassID] = stRow.stDBFields[2].sFieldValue;
		}
	}
	catch(CDBTransact::CDbException)
	{
	    BOSS_ERROR("��ѯ��Ŀ����:%s", m_cDBTrans.GetErrMsg());
		return -1;
	}

	return 0;
}

int CommodityClassDBOperatorV2::GetClassNameMap_SEARCH(std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName)
{
    char sSql[1024];

	snprintf(sSql, sizeof(sSql), "SELECT Parentid, Classid, ClassName FROM BossCommodityClass_search");

	try
	{
	    m_cDBTrans.ExecSQL(sSql, "centerdb_oracle");

        int iRowNum = m_cDBTrans.GetRowNum();

		for(int i = 0; i < iRowNum; i++)
		{
		    DBRow  stRow;

			stRow=m_cDBTrans.FetchRow();
            if("" == stRow.stDBFields[0].sFieldValue)
			    continue;

            u_int32_t uiParentID = strtoul(stRow.stDBFields[0].sFieldValue.c_str(), NULL, 10);
		    u_int32_t uiClassID = strtoul(stRow.stDBFields[1].sFieldValue.c_str(), NULL, 10);

			mapClassName[uiParentID][uiClassID] = stRow.stDBFields[2].sFieldValue;
		}
	}
	catch(CDBTransact::CDbException)
	{
	    BOSS_ERROR("��ѯ��Ŀ����:%s", m_cDBTrans.GetErrMsg());
		return -1;
	}

	return 0;
}
int CommodityClassMgrV2::Open(const std::string &sDBProxyAddr, const int iDBProxyPort)
{	
    if(m_cLocator.Open() != 0)
		return -1;
	
    if(m_cDBOptr.Init(sDBProxyAddr, iDBProxyPort) != 0)
		return -1;

	return 0;
}

int CommodityClassMgrV2::Open()
{
    if(m_cLocator.Open() != 0)
		return -1;

	return 0;
}

int CommodityClassMgrV2::GetCommodityClassFullPath(const u_int32_t uiClassID, CommodityClassFullPathV2 &cCommodityClassFullPath)
{
    // 1. ��ȡҪ��ȡ�Ĺ����ڴ�Key
    int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("����ȡ��ĿCache��ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
        BOSS_ERROR("����ȡ��ĿCache��ShmKey < 0, ����г�ʼ��");
		return -1;
	}

    // 2. �򿪹����ڴ�
    int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
    }

	// 3. ���ӹ����ڴ�
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. ����汾��
	int *p1 = (int*)p;
	if(p1[0] < 1)
	{
	    BOSS_ERROR("��ĿCache Version = %u < 1, �޷���ɲ�����Ŀ�ڵ����, ��Ҫ��������", 
            p1[0]
 		    );
        shmdt(p);
		return -1;
	}

    // 5. ��ȡ�������Ŀ�ڵ����
	int iCnt = p1[1];
	//BOSS_INFO("��Ŀ�ڵ���� = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("��Ŀ�ڵ���� %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. ������ĿID������Ŀ�ڵ�, ʹ�ö��ֲ���
	CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
	int iLow = 0;
	int iHigh = iCnt - 1;

	while(iLow <= iHigh)
	{ 
	    int iMiddle = (iLow + iHigh) / 2;

		if(p2[iMiddle].m_uiClassID == uiClassID)
		{
			cCommodityClassFullPath = p2[iMiddle];
			shmdt(p);
			return 0;
		}
		else if(p2[iMiddle].m_uiClassID < uiClassID)
			iLow = iMiddle + 1;
		else
			iHigh = iMiddle - 1;
	}

    shmdt(p);
	return -1;
}

int CommodityClassMgrV2::GetCommodityClassList(const std::string &sKeyword, std::list<CommodityClassFullPathV2> &listClass)
{
	 listClass.clear();

	char szTemp[1024];
	memset(szTemp, 0, sizeof(szTemp));
	strncpy(szTemp, sKeyword.c_str(), sKeyword.length());	
	char *pszTemp = TLib_Str_Trim(szTemp);	//ȥ��ǰ��ո�
	std::string strKeyWord  = pszTemp;

	if(strKeyWord.length() == 0)
	{
	    BOSS_ERROR("�ؼ��ֲ���Ϊ��");
	    return -1;
	}

	if(strstr(strKeyWord.c_str(), "%") !=NULL)
	{
		BOSS_ERROR("�ؼ����в��ܴ��ٷֺ�");
	    	return -1;	
	}
		

	// 1. ��ȡҪ��ȡ�Ĺ����ڴ�Key
	int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("����ȡ��ĿCache��ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
	    BOSS_ERROR("����ȡ��ĿCache��ShmKey < 0, ����г�ʼ��");
		return -1;
	}

	// 2. �򿪹����ڴ�
	int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
	}

	// 3. ���ӹ����ڴ�
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. ����汾��
	int *p1 = (int*)p;
	if(p1[0] < 2)
	{
	    BOSS_ERROR("��ĿCache Version = %u < 2, �޷�����ַ������Ҳ���, ��Ҫ��������", 
	        p1[0]
			    );

		shmdt(p);
		return -1;
	}

	// 5. ��ȡ�������Ŀ�ڵ����
	int iCnt = p1[1];
	//BOSS_INFO("��Ŀ�ڵ���� = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("��Ŀ�ڵ���� %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. ���ݹؼ��ֱ�������
	CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
	ClassNameBitMap *p3 = (ClassNameBitMap*)(p + sizeof(int) + sizeof(int) + sizeof(CommodityClassFullPathV2) * iCnt);

	for(int i = 0; i < iCnt; i++, p2++, p3++)
	{
	    u_int8_t c = (u_int8_t)strKeyWord.c_str()[0];
		if(c >= 97 && c <= 122)
			c -= 32;
		
	    u_int8_t c1 = (u_int8_t)strKeyWord.c_str()[1];
		if(c1 >= 97 && c1 <= 122)
			c1 -= 32;

		// ����bitmap�жϲ����ַ����ĵ�һ�����ַ�������Ŀ������
		if((p3->m_sData[c / 8] & (0X1 << ((c) % 8)))
			&& (0 == c1 || (p3->m_sData[c1 / 8] & (0X1 << ((c1) % 8))))
			)
		{
		    string s1 = p2->m_sClassName;
			transform(s1.begin(), s1.end(), s1.begin(), toupper);

			string s2 = strKeyWord;
			transform(s2.begin(), s2.end(), s2.begin(), toupper);

		    if(strstr(s1.c_str(), s2.c_str()) != NULL)
		    {
		        listClass.push_back(*p2);
		    }
		}
	}

	shmdt(p);
	return 0;
}

int CommodityClassMgrV2::GetCommodityClassFullName(const u_int32_t uiClassID, std::string &cCommodityClassFullName)
{
    CommodityClassFullPathV2 cCommodityClassFullPath;

    if(GetCommodityClassFullPath(uiClassID, cCommodityClassFullPath) != 0)
    {
        BOSS_ERROR("δ�ҵ�IDΪ %u ����Ŀ·��", uiClassID);
		return -1;
    }
	else
	{
	    cCommodityClassFullName = cCommodityClassFullPath.m_sFullClassName;
		return 0;
	}
}

int CommodityClassMgrV2::Reload()
{
    // 1. �����ݿ��л�ȡ��Ʒ��Ŀ��Ϣ
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap(mapClassName) != 0)
		return -1;

    // 2. ʹ�õݹ��㷨��ȡÿ���ڵ���Ŀ·��
    CommodityClassFullPathV2 cCommodityClassFullPath;
    std::map<u_int32_t, CommodityClassFullPathV2> mapClass;
    if(DoReload(0, 1, cCommodityClassFullPath, mapClassName, mapClass) == 0)
    {
        // 3. ��ȡҪд��Ĺ����ڴ�Key
        int iShmKey = m_cLocator.GetShmKey4Write();
		BOSS_INFO("��д����ĿCache��ShmKey = %d", iShmKey);
		if(iShmKey < 0)
		{
		    BOSS_ERROR("��д����ĿCache��ShmKey < 0");
			return -1;
		}

		// 4. ���Key�Ƿ�����ʹ��,������򲻽��д���
		int iShmID;
		u_int32_t uiShmSize = sizeof(int) 
			                  + sizeof(int) 
			                  + sizeof(CommodityClassFullPathV2) * mapClass.size()
			                  + sizeof(ClassNameBitMap) * mapClass.size();
	    if(-1 == (iShmID = shmget(iShmKey, uiShmSize, IPC_CREAT|IPC_EXCL|0664)))
	    {
	        BOSS_ERROR("ShmKey = %d �Ѿ�����, ���ܱ�д��", iShmKey);
	        return -1;
	    }

        // 5. ���ӹ����ڴ�
        char *p;
	    if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	    {
	        BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		    shmctl(iShmID, IPC_RMID, NULL);
		    return -1;
	    }

        // 6.��չ����ڴ�
        memset(p, 0, uiShmSize);

		// 7.д�빲���ڴ�
		int *p1 = (int*)p;
        // �汾��
		p1[0] = COMMODITY_CLASS_CACHE_VERSION;
		// ������
		p1[1] = mapClass.size();

        BOSS_INFO("�ڵ���� = %d", p1[1]);
		
        // ��ĿID��С����д����Ŀ�ڵ�
		CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++)
		{
		    *p2 = i->second;
			p2++;
		}

        // д����Ŀ����BitMap
		ClassNameBitMap *p3 = (ClassNameBitMap*)(p + sizeof(int) + sizeof(int) + sizeof(CommodityClassFullPathV2) * mapClass.size());
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++, p3++)
		{
		    CommodityClassFullPathV2 &tmp = i->second;

			u_int8_t *p4 = (u_int8_t *)tmp.m_sClassName;
			while(*p4 != '\0')
			{
			    u_int8_t uiTmp = *p4;
				if(uiTmp >= 97 && uiTmp <= 122)
					uiTmp -= 32;
				
			    p3->m_sData[(uiTmp) / 8] |= 0X1 << ((uiTmp) % 8);

			    p4++;
			}
		}
				
        // 8. ��ת�����ڴ�Key
		m_cLocator.Reverse();
		
		return 0;
    }
	else
	    return -1;
}

int CommodityClassMgrV2::DoReload(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPathV2 cCommodityClassFullPath, std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName, std::map<u_int32_t, CommodityClassFullPathV2> &mapClass)
{
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(std::map<u_int32_t, std::string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		std::string    sClassName = i->second;


		CommodityClassFullPathV2 cCurrentPath = cCommodityClassFullPath;
        cCurrentPath.m_uiClassLevel = uiClassLevel;

		switch(uiClassLevel)
		{
		    case 1:
				cCurrentPath.m_uiClassIDL1 = uiClassID;
				break;
			case 2:
				cCurrentPath.m_uiClassIDL2 = uiClassID;
				break;
			case 3:
				cCurrentPath.m_uiClassIDL3 = uiClassID;
				break;
			case 4:
				cCurrentPath.m_uiClassIDL4 = uiClassID;
				break;
			case 5:
				cCurrentPath.m_uiClassIDL5 = uiClassID;
				break;
			default:
			    BOSS_ERROR("��Ŀ�������Ϊ�弶");
				return -1;
		}

        snprintf(cCurrentPath.m_sClassName, sizeof(cCurrentPath.m_sClassName), "%s", sClassName.c_str());
		
		if('\0' == cCurrentPath.m_sFullClassName[0])
		{
		    snprintf(cCurrentPath.m_sFullClassName, sizeof(cCurrentPath.m_sFullClassName), "%s", sClassName.c_str());
		}
		else
		{
            strncat(cCurrentPath.m_sFullClassName, ">>", sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
			strncat(cCurrentPath.m_sFullClassName, sClassName.c_str(), sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
		}

        if(mapClassName.find(uiClassID) == mapClassName.end())
        {
            cCurrentPath.m_uiLeafID = uiClassID;
        }

		cCurrentPath.m_uiClassID = uiClassID;
		mapClass[uiClassID] = cCurrentPath;

		if(DoReload(uiClassID, uiClassLevel + 1, cCurrentPath, mapClassName, mapClass) != 0)
			return -1;

	}

	return 0;
}

int CommodityClassMgrV2::Export(const std::string &sOutputFile)
{
    FILE *fp = fopen(sOutputFile.c_str(), "w");
	if(NULL == fp)
	{
	    BOSS_ERROR("open %s error %s", sOutputFile.c_str(), strerror(errno));
		return -1;
	}
	
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap(mapClassName) != 0)
		return -1;

    CommodityClassFullPathV2 cCommodityClassFullPath;

    int iRet = DoExport(0, 1, cCommodityClassFullPath, mapClassName, fp);
	fclose(fp);
	return iRet;
}

int CommodityClassMgrV2::DoExport(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPathV2 cCommodityClassFullPath, std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName, FILE *fp)
{
    assert(fp);
	
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(std::map<u_int32_t, std::string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		std::string    sClassName = i->second;


		CommodityClassFullPathV2 cCurrentPath = cCommodityClassFullPath;
        cCurrentPath.m_uiClassLevel = uiClassLevel;

		switch(uiClassLevel)
		{
		    case 1:
				cCurrentPath.m_uiClassIDL1 = uiClassID;
				break;
			case 2:
				cCurrentPath.m_uiClassIDL2 = uiClassID;
				break;
			case 3:
				cCurrentPath.m_uiClassIDL3 = uiClassID;
				break;
			case 4:
				cCurrentPath.m_uiClassIDL4 = uiClassID;
				break;
			case 5:
				cCurrentPath.m_uiClassIDL5 = uiClassID;
				break;
			default:
			    BOSS_ERROR("��Ŀ�������Ϊ�弶");
				return -1;
		}

		if('\0' == cCurrentPath.m_sFullClassName[0])
		{
		    snprintf(cCurrentPath.m_sFullClassName, sizeof(cCurrentPath.m_sFullClassName), "%s", sClassName.c_str());
		}
		else
		{
            strncat(cCurrentPath.m_sFullClassName, ">>", sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
			strncat(cCurrentPath.m_sFullClassName, sClassName.c_str(), sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
		}

        if(mapClassName.find(uiClassID) == mapClassName.end())
        {
            cCurrentPath.m_uiLeafID = uiClassID;
        }
		
		fprintf(fp, "%u,%u,%u,%u,%u,%u,%u\n", uiClassID,
			                                  cCurrentPath.m_uiClassIDL1,
			                                  cCurrentPath.m_uiClassIDL2,
			                                  cCurrentPath.m_uiClassIDL3,
			                                  cCurrentPath.m_uiClassIDL4,
			                                  cCurrentPath.m_uiClassIDL5,
			                                  cCurrentPath.m_uiClassLevel);

		if(DoExport(uiClassID, uiClassLevel + 1, cCurrentPath, mapClassName, fp) != 0)
			return -1;

	}

	return 0;
}

int CommoditySearchClassMgrV2::Open(const std::string &sDBProxyAddr, const int iDBProxyPort)
{	
    if(m_cLocator.Open() != 0)
		return -1;
	
    if(m_cDBOptr.Init(sDBProxyAddr, iDBProxyPort) != 0)
		return -1;

	return 0;
}

int CommoditySearchClassMgrV2::Open()
{
    if(m_cLocator.Open() != 0)
		return -1;

	return 0;
}

int CommoditySearchClassMgrV2::GetCommodityClassFullPath(const u_int32_t uiClassID, CommodityClassFullPathV2 &cCommodityClassFullPath)
{
    // 1. ��ȡҪ��ȡ�Ĺ����ڴ�Key
    int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("����ȡ��ĿCache��ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
        BOSS_ERROR("����ȡ��ĿCache��ShmKey < 0, ����г�ʼ��");
		return -1;
	}

    // 2. �򿪹����ڴ�
    int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
    }

	// 3. ���ӹ����ڴ�
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. ����汾��
	int *p1 = (int*)p;
	if(p1[0] < 1)
	{
	    BOSS_ERROR("��ĿCache Version = %u < 1, �޷���ɲ�����Ŀ�ڵ����, ��Ҫ��������", 
            p1[0]
 		    );
        shmdt(p);
		return -1;
	}

    // 5. ��ȡ�������Ŀ�ڵ����
	int iCnt = p1[1];
	//BOSS_INFO("��Ŀ�ڵ���� = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("��Ŀ�ڵ���� %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. ������ĿID������Ŀ�ڵ�, ʹ�ö��ֲ���
	CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
	int iLow = 0;
	int iHigh = iCnt - 1;

	while(iLow <= iHigh)
	{ 
	    int iMiddle = (iLow + iHigh) / 2;

		if(p2[iMiddle].m_uiClassID == uiClassID)
		{
			cCommodityClassFullPath = p2[iMiddle];
			shmdt(p);
			return 0;
		}
		else if(p2[iMiddle].m_uiClassID < uiClassID)
			iLow = iMiddle + 1;
		else
			iHigh = iMiddle - 1;
	}

    shmdt(p);
	return -1;
}

int CommoditySearchClassMgrV2::GetCommodityClassList(const std::string &sKeyword, std::list<CommodityClassFullPathV2> &listClass)
{
	 listClass.clear();

	char szTemp[1024];
	memset(szTemp, 0, sizeof(szTemp));
	strncpy(szTemp, sKeyword.c_str(), sKeyword.length());	
	char *pszTemp = TLib_Str_Trim(szTemp);	//ȥ��ǰ��ո�
	std::string strKeyWord  = pszTemp;

	if(strKeyWord.length() == 0)
	{
	    BOSS_ERROR("�ؼ��ֲ���Ϊ��");
	    return -1;
	}

	if(strstr(strKeyWord.c_str(), "%") !=NULL)
	{
		BOSS_ERROR("�ؼ����в��ܴ��ٷֺ�");
	    	return -1;	
	}
		

	// 1. ��ȡҪ��ȡ�Ĺ����ڴ�Key
	int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("����ȡ��ĿCache��ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
	    BOSS_ERROR("����ȡ��ĿCache��ShmKey < 0, ����г�ʼ��");
		return -1;
	}

	// 2. �򿪹����ڴ�
	int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
	}

	// 3. ���ӹ����ڴ�
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. ����汾��
	int *p1 = (int*)p;
	if(p1[0] < 2)
	{
	    BOSS_ERROR("��ĿCache Version = %u < 2, �޷�����ַ������Ҳ���, ��Ҫ��������", 
	        p1[0]
			    );

		shmdt(p);
		return -1;
	}

	// 5. ��ȡ�������Ŀ�ڵ����
	int iCnt = p1[1];
	//BOSS_INFO("��Ŀ�ڵ���� = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("��Ŀ�ڵ���� %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. ���ݹؼ��ֱ�������
	CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
	ClassNameBitMap *p3 = (ClassNameBitMap*)(p + sizeof(int) + sizeof(int) + sizeof(CommodityClassFullPathV2) * iCnt);

	for(int i = 0; i < iCnt; i++, p2++, p3++)
	{
	    u_int8_t c = (u_int8_t)strKeyWord.c_str()[0];
		if(c >= 97 && c <= 122)
			c -= 32;
		
	    u_int8_t c1 = (u_int8_t)strKeyWord.c_str()[1];
		if(c1 >= 97 && c1 <= 122)
			c1 -= 32;

		// ����bitmap�жϲ����ַ����ĵ�һ�����ַ�������Ŀ������
		if((p3->m_sData[c / 8] & (0X1 << ((c) % 8)))
			&& (0 == c1 || (p3->m_sData[c1 / 8] & (0X1 << ((c1) % 8))))
			)
		{
		    string s1 = p2->m_sClassName;
			transform(s1.begin(), s1.end(), s1.begin(), toupper);

			string s2 = strKeyWord;
			transform(s2.begin(), s2.end(), s2.begin(), toupper);

		    if(strstr(s1.c_str(), s2.c_str()) != NULL)
		    {
		        listClass.push_back(*p2);
		    }
		}
	}

	shmdt(p);
	return 0;
}

int CommoditySearchClassMgrV2::GetCommodityClassFullName(const u_int32_t uiClassID, std::string &cCommodityClassFullName)
{
    CommodityClassFullPathV2 cCommodityClassFullPath;

    if(GetCommodityClassFullPath(uiClassID, cCommodityClassFullPath) != 0)
    {
        BOSS_ERROR("δ�ҵ�IDΪ %u ����Ŀ·��", uiClassID);
		return -1;
    }
	else
	{
	    cCommodityClassFullName = cCommodityClassFullPath.m_sFullClassName;
		return 0;
	}
}

int CommoditySearchClassMgrV2::Reload()
{
    // 1. �����ݿ��л�ȡ��Ʒ��Ŀ��Ϣ
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap_SEARCH(mapClassName) != 0)
		return -1;

    // 2. ʹ�õݹ��㷨��ȡÿ���ڵ���Ŀ·��
    CommodityClassFullPathV2 cCommodityClassFullPath;
    std::map<u_int32_t, CommodityClassFullPathV2> mapClass;
    if(DoReload(0, 1, cCommodityClassFullPath, mapClassName, mapClass) == 0)
    {
        // 3. ��ȡҪд��Ĺ����ڴ�Key
        int iShmKey = m_cLocator.GetShmKey4Write();
		BOSS_INFO("��д����ĿCache��ShmKey = %d", iShmKey);
		if(iShmKey < 0)
		{
		    BOSS_ERROR("��д����ĿCache��ShmKey < 0");
			return -1;
		}

		// 4. ���Key�Ƿ�����ʹ��,������򲻽��д���
		int iShmID;
		u_int32_t uiShmSize = sizeof(int) 
			                  + sizeof(int) 
			                  + sizeof(CommodityClassFullPathV2) * mapClass.size()
			                  + sizeof(ClassNameBitMap) * mapClass.size();
	    if(-1 == (iShmID = shmget(iShmKey, uiShmSize, IPC_CREAT|IPC_EXCL|0664)))
	    {
	        BOSS_ERROR("ShmKey = %d �Ѿ�����, ���ܱ�д��", iShmKey);
	        return -1;
	    }

        // 5. ���ӹ����ڴ�
        char *p;
	    if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	    {
	        BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		    shmctl(iShmID, IPC_RMID, NULL);
		    return -1;
	    }

        // 6.��չ����ڴ�
        memset(p, 0, uiShmSize);

		// 7.д�빲���ڴ�
		int *p1 = (int*)p;
        // �汾��
		p1[0] = COMMODITY_CLASS_CACHE_VERSION;
		// ������
		p1[1] = mapClass.size();

        BOSS_INFO("�ڵ���� = %d", p1[1]);
		
        // ��ĿID��С����д����Ŀ�ڵ�
		CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++)
		{
		    *p2 = i->second;
			p2++;
		}

        // д����Ŀ����BitMap
		ClassNameBitMap *p3 = (ClassNameBitMap*)(p + sizeof(int) + sizeof(int) + sizeof(CommodityClassFullPathV2) * mapClass.size());
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++, p3++)
		{
		    CommodityClassFullPathV2 &tmp = i->second;

			u_int8_t *p4 = (u_int8_t *)tmp.m_sClassName;
			while(*p4 != '\0')
			{
			    u_int8_t uiTmp = *p4;
				if(uiTmp >= 97 && uiTmp <= 122)
					uiTmp -= 32;
				
			    p3->m_sData[(uiTmp) / 8] |= 0X1 << ((uiTmp) % 8);

			    p4++;
			}
		}
				
        // 8. ��ת�����ڴ�Key
		m_cLocator.Reverse();
		
		return 0;
    }
	else
	    return -1;
}

int CommoditySearchClassMgrV2::DoReload(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPathV2 cCommodityClassFullPath, std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName, std::map<u_int32_t, CommodityClassFullPathV2> &mapClass)
{
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(std::map<u_int32_t, std::string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		std::string    sClassName = i->second;


		CommodityClassFullPathV2 cCurrentPath = cCommodityClassFullPath;
        cCurrentPath.m_uiClassLevel = uiClassLevel;

		switch(uiClassLevel)
		{
		    case 1:
				cCurrentPath.m_uiClassIDL1 = uiClassID;
				break;
			case 2:
				cCurrentPath.m_uiClassIDL2 = uiClassID;
				break;
			case 3:
				cCurrentPath.m_uiClassIDL3 = uiClassID;
				break;
			case 4:
				cCurrentPath.m_uiClassIDL4 = uiClassID;
				break;
			case 5:
				cCurrentPath.m_uiClassIDL5 = uiClassID;
				break;
			default:
			    BOSS_ERROR("��Ŀ�������Ϊ�弶");
				return -1;
		}

        snprintf(cCurrentPath.m_sClassName, sizeof(cCurrentPath.m_sClassName), "%s", sClassName.c_str());
		
		if('\0' == cCurrentPath.m_sFullClassName[0])
		{
		    snprintf(cCurrentPath.m_sFullClassName, sizeof(cCurrentPath.m_sFullClassName), "%s", sClassName.c_str());
		}
		else
		{
            strncat(cCurrentPath.m_sFullClassName, ">>", sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
			strncat(cCurrentPath.m_sFullClassName, sClassName.c_str(), sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
		}

        if(mapClassName.find(uiClassID) == mapClassName.end())
        {
            cCurrentPath.m_uiLeafID = uiClassID;
        }

		cCurrentPath.m_uiClassID = uiClassID;
		mapClass[uiClassID] = cCurrentPath;

		if(DoReload(uiClassID, uiClassLevel + 1, cCurrentPath, mapClassName, mapClass) != 0)
			return -1;

	}

	return 0;
}

int CommoditySearchClassMgrV2::Export(const std::string &sOutputFile)
{
    FILE *fp = fopen(sOutputFile.c_str(), "w");
	if(NULL == fp)
	{
	    BOSS_ERROR("open %s error %s", sOutputFile.c_str(), strerror(errno));
		return -1;
	}
	
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap_SEARCH(mapClassName) != 0)
		return -1;

    CommodityClassFullPathV2 cCommodityClassFullPath;

    int iRet = DoExport(0, 1, cCommodityClassFullPath, mapClassName, fp);
	fclose(fp);
	return iRet;
}

int CommoditySearchClassMgrV2::DoExport(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPathV2 cCommodityClassFullPath, std::map<u_int32_t, std::map<u_int32_t, std::string> > &mapClassName, FILE *fp)
{
    assert(fp);
	
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(std::map<u_int32_t, std::string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		std::string    sClassName = i->second;


		CommodityClassFullPathV2 cCurrentPath = cCommodityClassFullPath;
        cCurrentPath.m_uiClassLevel = uiClassLevel;

		switch(uiClassLevel)
		{
		    case 1:
				cCurrentPath.m_uiClassIDL1 = uiClassID;
				break;
			case 2:
				cCurrentPath.m_uiClassIDL2 = uiClassID;
				break;
			case 3:
				cCurrentPath.m_uiClassIDL3 = uiClassID;
				break;
			case 4:
				cCurrentPath.m_uiClassIDL4 = uiClassID;
				break;
			case 5:
				cCurrentPath.m_uiClassIDL5 = uiClassID;
				break;
			default:
			    BOSS_ERROR("��Ŀ�������Ϊ�弶");
				return -1;
		}

		if('\0' == cCurrentPath.m_sFullClassName[0])
		{
		    snprintf(cCurrentPath.m_sFullClassName, sizeof(cCurrentPath.m_sFullClassName), "%s", sClassName.c_str());
		}
		else
		{
            strncat(cCurrentPath.m_sFullClassName, ">>", sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
			strncat(cCurrentPath.m_sFullClassName, sClassName.c_str(), sizeof(cCurrentPath.m_sFullClassName) - strlen(cCurrentPath.m_sFullClassName) - 1);
		}

        if(mapClassName.find(uiClassID) == mapClassName.end())
        {
            cCurrentPath.m_uiLeafID = uiClassID;
        }
		
		fprintf(fp, "%u,%u,%u,%u,%u,%u,%u\n", uiClassID,
			                                  cCurrentPath.m_uiClassIDL1,
			                                  cCurrentPath.m_uiClassIDL2,
			                                  cCurrentPath.m_uiClassIDL3,
			                                  cCurrentPath.m_uiClassIDL4,
			                                  cCurrentPath.m_uiClassIDL5,
			                                  cCurrentPath.m_uiClassLevel);

		if(DoExport(uiClassID, uiClassLevel + 1, cCurrentPath, mapClassName, fp) != 0)
			return -1;

	}

	return 0;
}


