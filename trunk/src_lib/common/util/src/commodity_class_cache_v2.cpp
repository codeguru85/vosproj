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
	    BOSS_ERROR("连接数据库代理错误: %s",m_cDBTrans.GetErrMsg());
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
	    BOSS_ERROR("查询类目错误:%s", m_cDBTrans.GetErrMsg());
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
	    BOSS_ERROR("查询类目错误:%s", m_cDBTrans.GetErrMsg());
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
    // 1. 获取要读取的共享内存Key
    int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("待读取类目Cache的ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
        BOSS_ERROR("待读取类目Cache的ShmKey < 0, 需进行初始化");
		return -1;
	}

    // 2. 打开共享内存
    int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
    }

	// 3. 链接共享内存
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. 检验版本号
	int *p1 = (int*)p;
	if(p1[0] < 1)
	{
	    BOSS_ERROR("类目Cache Version = %u < 1, 无法完成查找类目节点操作, 需要升级数据", 
            p1[0]
 		    );
        shmdt(p);
		return -1;
	}

    // 5. 获取并检查类目节点个数
	int iCnt = p1[1];
	//BOSS_INFO("类目节点个数 = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("类目节点个数 %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. 根据类目ID搜索类目节点, 使用二分查找
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
	char *pszTemp = TLib_Str_Trim(szTemp);	//去掉前后空格
	std::string strKeyWord  = pszTemp;

	if(strKeyWord.length() == 0)
	{
	    BOSS_ERROR("关键字不能为空");
	    return -1;
	}

	if(strstr(strKeyWord.c_str(), "%") !=NULL)
	{
		BOSS_ERROR("关键字中不能带百分号");
	    	return -1;	
	}
		

	// 1. 获取要读取的共享内存Key
	int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("待读取类目Cache的ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
	    BOSS_ERROR("待读取类目Cache的ShmKey < 0, 需进行初始化");
		return -1;
	}

	// 2. 打开共享内存
	int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
	}

	// 3. 链接共享内存
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. 检验版本号
	int *p1 = (int*)p;
	if(p1[0] < 2)
	{
	    BOSS_ERROR("类目Cache Version = %u < 2, 无法完成字符串查找操作, 需要升级数据", 
	        p1[0]
			    );

		shmdt(p);
		return -1;
	}

	// 5. 获取并检查类目节点个数
	int iCnt = p1[1];
	//BOSS_INFO("类目节点个数 = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("类目节点个数 %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. 根据关键字遍历搜索
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

		// 先用bitmap判断查找字符串的第一二个字符都在类目名称中
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
        BOSS_ERROR("未找到ID为 %u 的类目路径", uiClassID);
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
    // 1. 从数据库中获取商品类目信息
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap(mapClassName) != 0)
		return -1;

    // 2. 使用递归算法获取每个节点类目路径
    CommodityClassFullPathV2 cCommodityClassFullPath;
    std::map<u_int32_t, CommodityClassFullPathV2> mapClass;
    if(DoReload(0, 1, cCommodityClassFullPath, mapClassName, mapClass) == 0)
    {
        // 3. 获取要写入的共享内存Key
        int iShmKey = m_cLocator.GetShmKey4Write();
		BOSS_INFO("待写入类目Cache的ShmKey = %d", iShmKey);
		if(iShmKey < 0)
		{
		    BOSS_ERROR("待写入类目Cache的ShmKey < 0");
			return -1;
		}

		// 4. 检测Key是否正被使用,如果是则不进行处理
		int iShmID;
		u_int32_t uiShmSize = sizeof(int) 
			                  + sizeof(int) 
			                  + sizeof(CommodityClassFullPathV2) * mapClass.size()
			                  + sizeof(ClassNameBitMap) * mapClass.size();
	    if(-1 == (iShmID = shmget(iShmKey, uiShmSize, IPC_CREAT|IPC_EXCL|0664)))
	    {
	        BOSS_ERROR("ShmKey = %d 已经生成, 不能被写入", iShmKey);
	        return -1;
	    }

        // 5. 链接共享内存
        char *p;
	    if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	    {
	        BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		    shmctl(iShmID, IPC_RMID, NULL);
		    return -1;
	    }

        // 6.清空共享内存
        memset(p, 0, uiShmSize);

		// 7.写入共享内存
		int *p1 = (int*)p;
        // 版本号
		p1[0] = COMMODITY_CLASS_CACHE_VERSION;
		// 结点个数
		p1[1] = mapClass.size();

        BOSS_INFO("节点个数 = %d", p1[1]);
		
        // 类目ID由小至大写入类目节点
		CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++)
		{
		    *p2 = i->second;
			p2++;
		}

        // 写入类目名称BitMap
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
				
        // 8. 反转共享内存Key
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
			    BOSS_ERROR("类目级别最多为五级");
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
			    BOSS_ERROR("类目级别最多为五级");
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
    // 1. 获取要读取的共享内存Key
    int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("待读取类目Cache的ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
        BOSS_ERROR("待读取类目Cache的ShmKey < 0, 需进行初始化");
		return -1;
	}

    // 2. 打开共享内存
    int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
    }

	// 3. 链接共享内存
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. 检验版本号
	int *p1 = (int*)p;
	if(p1[0] < 1)
	{
	    BOSS_ERROR("类目Cache Version = %u < 1, 无法完成查找类目节点操作, 需要升级数据", 
            p1[0]
 		    );
        shmdt(p);
		return -1;
	}

    // 5. 获取并检查类目节点个数
	int iCnt = p1[1];
	//BOSS_INFO("类目节点个数 = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("类目节点个数 %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. 根据类目ID搜索类目节点, 使用二分查找
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
	char *pszTemp = TLib_Str_Trim(szTemp);	//去掉前后空格
	std::string strKeyWord  = pszTemp;

	if(strKeyWord.length() == 0)
	{
	    BOSS_ERROR("关键字不能为空");
	    return -1;
	}

	if(strstr(strKeyWord.c_str(), "%") !=NULL)
	{
		BOSS_ERROR("关键字中不能带百分号");
	    	return -1;	
	}
		

	// 1. 获取要读取的共享内存Key
	int iShmKey = m_cLocator.GetShmKey4Read();
	//BOSS_INFO("待读取类目Cache的ShmKey = %d", iShmKey);
	if(iShmKey < 0)
	{
	    BOSS_ERROR("待读取类目Cache的ShmKey < 0, 需进行初始化");
		return -1;
	}

	// 2. 打开共享内存
	int iShmID;
	if((iShmID = shmget(iShmKey, 0, 0664)) == -1)
	{
		BOSS_ERROR("shmget %d Error: %s", iShmKey, strerror(errno));
		return -1;
	}

	// 3. 链接共享内存
	char *p;
	if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	{
	    BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		return -1;
	}

	// 4. 检验版本号
	int *p1 = (int*)p;
	if(p1[0] < 2)
	{
	    BOSS_ERROR("类目Cache Version = %u < 2, 无法完成字符串查找操作, 需要升级数据", 
	        p1[0]
			    );

		shmdt(p);
		return -1;
	}

	// 5. 获取并检查类目节点个数
	int iCnt = p1[1];
	//BOSS_INFO("类目节点个数 = %d", iCnt);
	if(iCnt <= 0)
	{
	    BOSS_ERROR("类目节点个数 %d <= 0", iCnt);
		shmdt(p);
		return -1;
	}

	// 6. 根据关键字遍历搜索
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

		// 先用bitmap判断查找字符串的第一二个字符都在类目名称中
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
        BOSS_ERROR("未找到ID为 %u 的类目路径", uiClassID);
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
    // 1. 从数据库中获取商品类目信息
    std::map<u_int32_t, std::map<u_int32_t, std::string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap_SEARCH(mapClassName) != 0)
		return -1;

    // 2. 使用递归算法获取每个节点类目路径
    CommodityClassFullPathV2 cCommodityClassFullPath;
    std::map<u_int32_t, CommodityClassFullPathV2> mapClass;
    if(DoReload(0, 1, cCommodityClassFullPath, mapClassName, mapClass) == 0)
    {
        // 3. 获取要写入的共享内存Key
        int iShmKey = m_cLocator.GetShmKey4Write();
		BOSS_INFO("待写入类目Cache的ShmKey = %d", iShmKey);
		if(iShmKey < 0)
		{
		    BOSS_ERROR("待写入类目Cache的ShmKey < 0");
			return -1;
		}

		// 4. 检测Key是否正被使用,如果是则不进行处理
		int iShmID;
		u_int32_t uiShmSize = sizeof(int) 
			                  + sizeof(int) 
			                  + sizeof(CommodityClassFullPathV2) * mapClass.size()
			                  + sizeof(ClassNameBitMap) * mapClass.size();
	    if(-1 == (iShmID = shmget(iShmKey, uiShmSize, IPC_CREAT|IPC_EXCL|0664)))
	    {
	        BOSS_ERROR("ShmKey = %d 已经生成, 不能被写入", iShmKey);
	        return -1;
	    }

        // 5. 链接共享内存
        char *p;
	    if(-1 == (int)(p = (char*)shmat(iShmID, NULL, 0)))
	    {
	        BOSS_ERROR("shmat %d Error: %s", iShmID, strerror(errno));
		    shmctl(iShmID, IPC_RMID, NULL);
		    return -1;
	    }

        // 6.清空共享内存
        memset(p, 0, uiShmSize);

		// 7.写入共享内存
		int *p1 = (int*)p;
        // 版本号
		p1[0] = COMMODITY_CLASS_CACHE_VERSION;
		// 结点个数
		p1[1] = mapClass.size();

        BOSS_INFO("节点个数 = %d", p1[1]);
		
        // 类目ID由小至大写入类目节点
		CommodityClassFullPathV2 *p2 = (CommodityClassFullPathV2*)(p + sizeof(int) + sizeof(int));
		for(std::map<u_int32_t, CommodityClassFullPathV2>::iterator i = mapClass.begin();
		        i != mapClass.end(); i++)
		{
		    *p2 = i->second;
			p2++;
		}

        // 写入类目名称BitMap
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
				
        // 8. 反转共享内存Key
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
			    BOSS_ERROR("类目级别最多为五级");
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
			    BOSS_ERROR("类目级别最多为五级");
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


