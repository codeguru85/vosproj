#include "bitmap_single.h"

int BitMapMgr::Close()
{
    if(m_pstState)
	{
		BOSS_INFO("munmap %s", m_sFileName.c_str());
	    munmap(m_pstState, sizeof(*m_pstState));
	    m_pstState = NULL;
		m_sFileName = "";
	}

	return 0;
}

void BitMapMgr::Init()
{
    if(NULL == m_pstState)
		return;

	memset(m_pstState, 0, sizeof(*m_pstState));
}

int BitMapMgr::Open(const string& sFileName)
{
    Close();

    m_sFileName = sFileName;

    bool bNew = false;
    int iFD = open(m_sFileName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0664);
 	if(iFD < 0)
	{
	    if(EEXIST == errno)
	    {
	        iFD = open(m_sFileName.c_str(), O_RDWR | O_CREAT, 0664);
 	        if(iFD < 0)
 	        {
 	            BOSS_ERROR("Open File %s Error: %s", m_sFileName.c_str(), strerror(errno));
                return -1;
 	        }
	    }
		else
		{
	        BOSS_ERROR("Open File %s Error: %s", m_sFileName.c_str(), strerror(errno));
            return -1;
		}
    }
    else
		bNew = true;

    lseek(iFD, sizeof(*m_pstState), SEEK_SET);
    write(iFD, "", 1);
	lseek(iFD, 0, SEEK_SET);

	m_pstState = (STBitMapState*)mmap(NULL, sizeof(*m_pstState), PROT_READ | PROT_WRITE, MAP_SHARED, iFD, 0);
    if((void*)MAP_FAILED == (void*)m_pstState)
    {
        BOSS_ERROR("mmap File %s Error: %s", m_sFileName.c_str(), strerror(errno));
		close(iFD);
		return -1;
    }

	close(iFD);

    if(bNew)
		Init();

	BOSS_INFO("mmap File %s Size %u OK", m_sFileName.c_str(), sizeof(*m_pstState));

	return 0;
}

void BitMapMgr::Set(const uint32_t dwDestUin)
{
    assert(m_pstState);
    
    // 当前时间
    uint32_t dwTimeStamp = time(NULL);
    // 索引为QQ号取mod
    uint32_t dwIndex = dwDestUin % MAX_QQ_NUM;
    // 存时间最老的索引
    uint32_t dwOldTimeIndex = 0;

    for(uint32_t i = 0; i < STEP_LENGTH; ++i)
    {    	
    	// 当前位置
    	dwIndex = dwIndex % MAX_QQ_NUM;
    	
    	if(0 == dwOldTimeIndex)
    		dwOldTimeIndex = dwIndex;
    	else if(m_pstState->m_stState[dwIndex].m_dwTimeStamp < m_pstState->m_stState[dwOldTimeIndex].m_dwTimeStamp)
    		dwOldTimeIndex = dwIndex;
    		
    	// 如果当前位置为0，把QQ号存入，退出
    	if(m_pstState->m_stState[dwIndex].m_dwUin == 0)
    	{
    		m_pstState->m_stState[dwIndex].m_dwUin = dwDestUin;
    		m_pstState->m_stState[dwIndex].m_dwTimeStamp = dwTimeStamp;
    		m_pstState->m_dwUniqueUserNum++;
    		return;
    	}
    	
    	dwIndex++;
    }

	// 步长内全部都有数据，则淘汰时间最早的
    m_pstState->m_stState[dwOldTimeIndex].m_dwUin = dwDestUin;
    m_pstState->m_stState[dwOldTimeIndex].m_dwTimeStamp = dwTimeStamp;	
}

uint32_t BitMapMgr::Get(const uint32_t dwDestUin)
{
    assert(m_pstState);
    
    uint32_t dwIndex = dwDestUin % MAX_QQ_NUM;
    for(uint32_t i = 0; i < STEP_LENGTH; ++i)
    {
    	dwIndex = dwIndex % MAX_QQ_NUM;
    	if(m_pstState->m_stState[dwIndex].m_dwUin == dwDestUin)
    		return 1;
    	dwIndex++;
    }
    
    return 0;
}

void BitMapMgr::Clear(const uint32_t dwDestUin)
{
    assert(m_pstState);
    
    uint32_t dwIndex = dwDestUin % MAX_QQ_NUM;
    for(uint32_t i = 0; i < STEP_LENGTH; ++i)
    {
    	dwIndex = dwIndex % MAX_QQ_NUM;
    	if(m_pstState->m_stState[dwIndex].m_dwUin == dwDestUin)
    	{
	    	m_pstState->m_stState[dwIndex].m_dwUin = 0;
	    	m_pstState->m_stState[dwIndex].m_dwTimeStamp = 0;
	    	m_pstState->m_dwUniqueUserNum--;
	    	break;
    	}
    	dwIndex++;
    }
}

uint32_t BitMapMgr::Size()
{
    assert(m_pstState);
	
    return m_pstState->m_dwUniqueUserNum;
}


