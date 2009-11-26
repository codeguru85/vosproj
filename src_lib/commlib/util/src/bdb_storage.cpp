#include "bdb_storage.h"

int BDBStoragePlus::Close()
{
    if(m_bHaveInit)
    {
        try
		{
		    m_pcDB->close(0);
			delete m_pcDB;
			m_pcDB = NULL;
		}
		catch (DbException& e)
		{
		    BOSS_ERROR("Close DB %s error: %s", m_sDBFileName.c_str(), e.what());
			return -1;
		}
		catch (std::exception& e)
		{
		    BOSS_ERROR("Close DB %s error: %s", m_sDBFileName.c_str(), e.what());
			return -1;
		}

		BOSS_INFO("Close DB %s OK", m_sDBFileName.c_str());

		m_sDBFileName = "";

    	m_bHaveInit = false;
    }

	return 0;
}

BDBStoragePlus::~BDBStoragePlus()
{
    Close();
}

int BDBStoragePlus::Open(const string& sDBFileName)
{
    if(m_bHaveInit)
    {
        Close();
    }

    if("" == sDBFileName)
    {
        BOSS_ERROR("Open DB Miss DBFileName");
		return -1;
    }

    m_pcDB = new Db(NULL, 0);

	m_sDBFileName = sDBFileName;

	try
	{
		m_pcDB->set_error_stream(&std::cerr);
		m_pcDB->set_errpfx("BDB_ERROR:");

		m_pcDB->set_pagesize(1024 * 4);
		//m_pcDB->set_cachesize(0, 1024 * 1024 * 2, 0);

		m_pcDB->open(NULL, m_sDBFileName.c_str(), NULL, DB_BTREE, DB_CREATE, 0664);

	}
	catch(DbException& e)
	{
		BOSS_ERROR("Open DB %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Open DB %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

	BOSS_INFO("Open DB %s OK", m_sDBFileName.c_str());

    m_bHaveInit = true;
	return 0;
}

int  BDBStoragePlus::Insert(u_int32_t uiKey, char *pcData, const u_int32_t uiDataLen)
{
    assert(pcData);

	if(0 == uiDataLen)
		return 0;
	
    try
	{
        Dbt cKey(&uiKey, sizeof(uiKey));
        Dbt cData(pcData, uiDataLen);

		int iRet = m_pcDB->put(NULL, &cKey, &cData, DB_NOOVERWRITE);
		if(DB_KEYEXIST == iRet)
		{
		    BOSS_ERROR("Key %u Duplcated", uiKey);
			return -1;
		}
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Insert Into %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Insert Into %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	
	Sync();

	return 0;
}

int  BDBStoragePlus::Update(u_int32_t uiKey, char *pcData, const u_int32_t uiDataLen)
{
    assert(pcData);

	if(0 == uiDataLen)
		return 0;
	
    try
	{
        Dbt cKey(&uiKey, sizeof(uiKey));
        Dbt cData(pcData, uiDataLen);

		m_pcDB->put(NULL, &cKey, &cData, 0);
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Update %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Update %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

	Sync();

	return 0;
}

int  BDBStoragePlus::Delete(u_int32_t uiKey)
{
    try
	{
        Dbt cKey(&uiKey, sizeof(uiKey));

		m_pcDB->del(NULL, &cKey, 0);
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Delete From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Delete From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

    Sync();

	return 0;
}

int BDBStoragePlus::Get(u_int32_t uiKey, char *pcData, u_int32_t &uiDataLen)
{
    assert(pcData && uiDataLen > 0);
	
    try
	{
        Dbt cKey(&uiKey, sizeof(uiKey));
		Dbt cData;

		if(DB_NOTFOUND == m_pcDB->get(NULL, &cKey, &cData, 0))
		{
		    uiDataLen = 0;
			
			return 0;
		}

		if(cData.get_size() > uiDataLen)
		{
		    BOSS_ERROR("数据缓冲区[%u] 不足以容纳 %u 的 %u 字节数据", uiDataLen, uiKey, cData.get_size());
			return -1;
		}

		uiDataLen = cData.get_size();
		memcpy(pcData, cData.get_data(), uiDataLen);		
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Get %u From %s error: %s", uiKey, m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Get %u From %s error: %s", uiKey, m_sDBFileName.c_str(), e.what());
		return -1;
	}

	return 0;
}

int  BDBStoragePlus::Insert(char *pcKey, const u_int32_t uiKeyLen, char *pcData, const u_int32_t uiDataLen)
{
    assert(pcData && pcKey);

	if(0 == uiDataLen)
		return 0;
	
    try
	{
        Dbt cKey(pcKey, uiKeyLen);
        Dbt cData(pcData, uiDataLen);

		int iRet = m_pcDB->put(NULL, &cKey, &cData, DB_NOOVERWRITE);
		if(DB_KEYEXIST == iRet)
		{
		    BOSS_ERROR("Key Duplcated");
			return -1;
		}
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Insert Into %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Insert Into %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	
	Sync();

	return 0;
}

int  BDBStoragePlus::Update(char *pcKey, const u_int32_t uiKeyLen, char *pcData, const u_int32_t uiDataLen)
{
    assert(pcData && pcKey);

	if(0 == uiDataLen)
		return 0;
	
    try
	{
        Dbt cKey(pcKey, uiKeyLen);
        Dbt cData(pcData, uiDataLen);

		m_pcDB->put(NULL, &cKey, &cData, 0);
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Update %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Update %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

	Sync();

	return 0;
}

int  BDBStoragePlus::Delete(char *pcKey, const u_int32_t uiKeyLen)
{
    assert(pcKey);
	
    try
	{
        Dbt cKey(pcKey, uiKeyLen);

		m_pcDB->del(NULL, &cKey, 0);
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Delete From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Delete From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

    Sync();

	return 0;
}

int BDBStoragePlus::Get(char *pcKey, const u_int32_t uiKeyLen, char *pcData, u_int32_t &uiDataLen)
{
    assert(pcData && uiDataLen > 0 && pcKey);
	
    try
	{
        Dbt cKey(pcKey, uiKeyLen);
		Dbt cData;

		if(DB_NOTFOUND == m_pcDB->get(NULL, &cKey, &cData, 0))
		{
		    uiDataLen = 0;
			
			return 0;
		}

		if(cData.get_size() > uiDataLen)
		{
		    BOSS_ERROR("数据缓冲区[%u] 不足以容纳%u 字节数据", uiDataLen, cData.get_size());
			return -1;
		}

		uiDataLen = cData.get_size();
		memcpy(pcData, cData.get_data(), uiDataLen);		
	}
	catch(DbException& e)
	{
		BOSS_ERROR("Get From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}
	catch (std::exception& e)
	{
		BOSS_ERROR("Get From %s error: %s", m_sDBFileName.c_str(), e.what());
		return -1;
	}

	return 0;
}


void BDBStoragePlus::Sync()
{
    static u_int32_t uiUpdateCount = 0;

	uiUpdateCount++;

	if(uiUpdateCount >= 10000)
	{
	    uiUpdateCount = 0;

	    try
	    {
	        m_pcDB->sync(0);
	    }
		catch (DbException& e)
	    {
	        BOSS_ERROR("Sync DB %s error: %s", m_sDBFileName.c_str(), e.what());
		    return;
	    }
	    catch (std::exception& e)
	    {
	        BOSS_ERROR("Sync DB %s error: %s", m_sDBFileName.c_str(), e.what());
		    return;
	    }
	}
}

int BDBStorageGroupPlus::Open(const string& sDBFileName, const u_int32_t uiBaseNum)
{
    assert(uiBaseNum > 0);

    Close();

	m_uiBaseNum = uiBaseNum;

    m_pcBDBStorages = new BDBStoragePlus[m_uiBaseNum];
	if(NULL == m_pcBDBStorages)
	{
	    BOSS_ERROR("new BDBStoragePlus array error");
		return -1;
	}

    int iRst = 0;
    for(unsigned int i = 0; i < m_uiBaseNum; i++)
    {
        stringstream sRealDBFileName;
		sRealDBFileName  <<sDBFileName <<"_" <<i <<".dat";

		if(m_pcBDBStorages[i].Open(sRealDBFileName.str()) != 0)
		{
		    BOSS_ERROR("打开 %s BDB数据库发生错误", sRealDBFileName.str().c_str());
			iRst++;
		}
    }

	return iRst;
}

void BDBStorageGroupPlus::Close()
{
    if(m_pcBDBStorages)
    {
	    delete []m_pcBDBStorages;
		m_pcBDBStorages = NULL;
		m_uiBaseNum = 0;
    }
}


int  BDBStorageGroupPlus::Insert(u_int32_t uiKey, char *pcData, const u_int32_t uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[uiKey % m_uiBaseNum].Insert(uiKey, pcData, uiDataLen);
}

int  BDBStorageGroupPlus::Update(u_int32_t uiKey, char *pcData, const u_int32_t uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[uiKey % m_uiBaseNum].Update(uiKey, pcData, uiDataLen);
}

int  BDBStorageGroupPlus::Delete(u_int32_t uiKey)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[uiKey % m_uiBaseNum].Delete(uiKey);
}

int BDBStorageGroupPlus::Get(u_int32_t uiKey, char *pcData, u_int32_t &uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[uiKey % m_uiBaseNum].Get(uiKey, pcData, uiDataLen);
}

int  BDBStorageGroupPlus::Insert(char *pcKey, const u_int32_t uiKeyLen, char *pcData, const u_int32_t uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[Hash(pcKey, uiKeyLen) % m_uiBaseNum].Insert(pcKey, uiKeyLen, pcData, uiDataLen);
}

int  BDBStorageGroupPlus::Update(char *pcKey, const u_int32_t uiKeyLen, char *pcData, const u_int32_t uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[Hash(pcKey, uiKeyLen) % m_uiBaseNum].Update(pcKey, uiKeyLen, pcData, uiDataLen);
}

int  BDBStorageGroupPlus::Delete(char *pcKey, const u_int32_t uiKeyLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[Hash(pcKey, uiKeyLen) % m_uiBaseNum].Delete(pcKey, uiKeyLen);
}

int BDBStorageGroupPlus::Get(char *pcKey, const u_int32_t uiKeyLen, char *pcData, u_int32_t &uiDataLen)
{
    assert(m_pcBDBStorages);
	
    return m_pcBDBStorages[Hash(pcKey, uiKeyLen) % m_uiBaseNum].Get(pcKey, uiKeyLen, pcData, uiDataLen);
}

u_int32_t BDBStorageGroupPlus::Hash(char *pcKey, u_int32_t uiKeyLen)
{
    assert(pcKey);

	u_int32_t uiHash = 0;

	char *pEnd = pcKey + uiKeyLen - 1;
	
    for(; ((u_int32_t)pcKey % 4) != 0 && pcKey <= pEnd; pcKey++, uiKeyLen--)
    {
        uiHash = (uiHash << 1) | *pcKey;
    }

	if(pcKey > pEnd)
		return uiHash;

	u_int32_t uiIntNum = uiKeyLen / sizeof(u_int32_t);
	while(uiIntNum > 0)
	{
        uiHash = uiHash ^ *((u_int32_t*)pcKey);
		pcKey += sizeof(u_int32_t);
	    uiIntNum--;
		uiKeyLen -= sizeof(u_int32_t);
	}

	while(uiKeyLen > 0)
	{
	    uiHash = uiHash | (*pcKey << uiKeyLen);
		uiKeyLen--;
	}
	
	return uiHash;
}


