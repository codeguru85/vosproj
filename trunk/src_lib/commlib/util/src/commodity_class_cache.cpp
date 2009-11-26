#include "commodity_class_cache.h"


int CommodityClassFullPath::IsRoot()
{
    if(1 == m_uiClassLevel)
		return 1;
	else
		return 0;
}


int CommodityClassFullPath::IsLeaf()
{
    if(m_uiLeafID > 0)
		return 1;
	else
		return 0;
}


CommodityClassDBOperator::CommodityClassDBOperator():m_cDBTrans()
{
}

CommodityClassDBOperator::~CommodityClassDBOperator()
{
    m_cDBTrans.Close();
}

int CommodityClassDBOperator::Init(const string &sDBProxyAddr, const int iDBProxyPort)
{
    if(m_cDBTrans.Init(sDBProxyAddr.c_str(), iDBProxyPort) < 0)
	{
	    BOSS_ERROR("连接数据库代理错误: %s",m_cDBTrans.GetErrMsg());
		return -1;
	}

	return 0;
}
int CommodityClassMgr::Open(const string &sDBProxyAddr, const int iDBProxyPort, int iShmKey)
{
    if(0 == iShmKey)
		iShmKey = COMMODITY_CLASS_FULL_PATH_SHM_KEY;
	
    if(m_cLock.Open(iShmKey) != 0)
		return -1;
	
    if(m_cDBOptr.Init(sDBProxyAddr, iDBProxyPort) != 0)
		return -1;
	else
	{
	    return m_cHashTable.Open(iShmKey);
	}
}

int CommodityClassMgr::Open(int iShmKey)
{
    if(0 == iShmKey)
		iShmKey = COMMODITY_CLASS_FULL_PATH_SHM_KEY;
	
    if(m_cLock.Open(iShmKey) != 0)
		return -1;
	
    return m_cHashTable.Open(iShmKey);
}

int CommodityClassMgr::GetCommodityClassFullPath(const u_int32_t uiLeafID, CommodityClassFullPath &cCommodityClassFullPath)
{
    boss::util::LockGuard<boss::util::SemLock> cLockGurad(&m_cLock);
	
    if(m_cHashTable.Get(uiLeafID, &cCommodityClassFullPath) <= 0)
    {
        //BOSS_ERROR("未找到叶子节点为 %u 的类目路径", uiLeafID);
		return -1;
    }
	else
		return 0;
}

int CommodityClassMgr::SetCommodityClassFullPath(const u_int32_t uiLeafID, const CommodityClassFullPath &cCommodityClassFullPath)
{
    boss::util::LockGuard<boss::util::SemLock> cLockGurad(&m_cLock);

	if(m_cHashTable.Put(uiLeafID, &cCommodityClassFullPath) > 0)
		return 0;
	else
		return -1;
}

int CommodityClassMgr::GetCommodityClassFullName(const u_int32_t uiLeafID, string &cCommodityClassFullName)
{
    boss::util::LockGuard<boss::util::SemLock> cLockGurad(&m_cLock);
	
    CommodityClassFullPath cCommodityClassFullPath;

    if(m_cHashTable.Get(uiLeafID, &cCommodityClassFullPath) <= 0)
    {
        BOSS_ERROR("未找到叶子节点为 %u 的类目路径", uiLeafID);
		return -1;
    }
	else
	{
	    cCommodityClassFullName = cCommodityClassFullPath.m_sFullClassName;
		return 0;
	}
}

int CommodityClassMgr::Reload()
{
    map<u_int32_t, map<u_int32_t, string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap(mapClassName) != 0)
		return -1;

    CommodityClassFullPath cCommodityClassFullPath;

    return DoReload(0, 1, cCommodityClassFullPath, mapClassName);
}

int CommodityClassMgr::DoReload(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPath cCommodityClassFullPath, map<u_int32_t, map<u_int32_t, string> > &mapClassName)
{
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(map<u_int32_t, string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		string    sClassName = i->second;


		CommodityClassFullPath cCurrentPath = cCommodityClassFullPath;
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
		
		if(SetCommodityClassFullPath(uiClassID, cCurrentPath) != 0)
		{
		    BOSS_ERROR("保存类目ID为 %u的路径错误", uiClassID);
			return -1;
		}

		if(DoReload(uiClassID, uiClassLevel + 1, cCurrentPath, mapClassName) != 0)
			return -1;

	}

	return 0;
}

int CommodityClassMgr::Export(const string &sOutputFile)
{
    FILE *fp = fopen(sOutputFile.c_str(), "w");
	if(NULL == fp)
	{
	    BOSS_ERROR("open %s error %s", sOutputFile.c_str(), strerror(errno));
		return -1;
	}
	
    map<u_int32_t, map<u_int32_t, string> > mapClassName;
	if(m_cDBOptr.GetClassNameMap(mapClassName) != 0)
		return -1;

    CommodityClassFullPath cCommodityClassFullPath;

    int iRet = DoExport(0, 1, cCommodityClassFullPath, mapClassName, fp);
	fclose(fp);
	return iRet;
}

int CommodityClassMgr::DoExport(u_int32_t uiParentID, u_int32_t uiClassLevel, CommodityClassFullPath cCommodityClassFullPath, map<u_int32_t, map<u_int32_t, string> > &mapClassName, FILE *fp)
{
    assert(fp);
	
    if(mapClassName[uiParentID].size() == 0)
		return 0;

	for(map<u_int32_t, string>::iterator i = mapClassName[uiParentID].begin();
	                                      i != mapClassName[uiParentID].end();
										  i++)
	{
	    u_int32_t uiClassID = i->first;
		string    sClassName = i->second;


		CommodityClassFullPath cCurrentPath = cCommodityClassFullPath;
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

int CommodityClassDBOperator::GetClassNameMap(map<u_int32_t, map<u_int32_t, string> > &mapClassName)
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
