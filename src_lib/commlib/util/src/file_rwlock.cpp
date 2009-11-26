#include "file_rwlock.h"

FileRWLock::FileRWLock()
{
    m_iFD = -1;
	m_uiLockSize = 0;
}

FileRWLock::~FileRWLock()
{
    Close();
}

void FileRWLock::Close()
{
    if(m_iFD >= 0)
    {
        close(m_iFD);
		m_iFD = -1;
    }
}

int FileRWLock::Open(const string &sLockFileName, u_int32_t uiLockSize)
{
    if(sLockFileName.length() == 0)
		return -1;

	m_sFileName = sLockFileName;
	
    Close();

	bool bIsNew = true;
	int iFD = open(m_sFileName.c_str(), O_RDWR|O_CREAT|O_EXCL, 0600);
	// 如果创建新文件失败
	if(-1 == iFD)
	{
	    // 如果错误不是文件已经存在,则输出错误信息,返回错误
	    if(EEXIST != errno)
	    {
	        BOSS_ERROR("open %s error %s", m_sFileName.c_str(), strerror(errno));
	        return -1;
	    }

		// 如果是文件已经存在,则打开文件
		iFD = open(m_sFileName.c_str(), O_RDWR, 0600);
		if(-1 == iFD)
		{
		    BOSS_ERROR("open %s error %s", m_sFileName.c_str(), strerror(errno));
	        return -1;
		}
		bIsNew = false;
	}
    m_iFD = iFD;
	
	// 如果是新创建文件,则根据参数初始化文件的大小
    if(bIsNew)
    {
        if(0 == uiLockSize || ftruncate(m_iFD, uiLockSize + 1) < 0)
        {
            BOSS_ERROR("ftruncate %s to %u error %s", m_sFileName.c_str(), uiLockSize, strerror(errno));
            Close();
			return -1;
        }
		else
			m_uiLockSize = uiLockSize;
			
    }
	// 如果是老文件,则读取文件的大小
    else
	{
	    struct stat stStat;
		if(fstat(m_iFD, &stStat) != 0)
		{
		    BOSS_ERROR("fstat %s error %s", m_sFileName.c_str(), strerror(errno));
			Close();
			return -1;
		}

		if(0 == stStat.st_size)
		{
		    BOSS_ERROR("fstat %s is empty", m_sFileName.c_str());
			Close();
			return -1;
		}
		
		m_uiLockSize = (u_int32_t)stStat.st_size - 1;
	}

	BOSS_INFO("Open Lock %s LockSize %u", m_sFileName.c_str(), m_uiLockSize);
	
	return 0;
}

int FileRWLock::RLock(u_int32_t uiIdx)
{
    if(m_iFD < 0)
		return -1;

	if(uiIdx > m_uiLockSize)
		return -1;
	
    struct flock stLock;    
	stLock.l_type = F_RDLCK;    
	stLock.l_whence = SEEK_SET;    
	stLock.l_start = uiIdx;    
	stLock.l_len = 1;    
	stLock.l_pid = -1;    
	while(fcntl(m_iFD, F_SETLKW, &stLock))    
    {        
        if(errno != EINTR)
        {
            BOSS_ERROR("RLock %s Idx %u Error %s", m_sFileName.c_str(), uiIdx, strerror(errno));
			return -1;
        }
    }    

	return 0;	
}

int FileRWLock::WLock(u_int32_t uiIdx)
{
    if(m_iFD < 0)
		return -1;

	if(uiIdx > m_uiLockSize)
		return -1;
	
    struct flock stLock;    
	stLock.l_type = F_WRLCK;    
	stLock.l_whence = SEEK_SET;    
	stLock.l_start = uiIdx;    
	stLock.l_len = 1;    
	stLock.l_pid = -1;    
	while(fcntl(m_iFD, F_SETLKW, &stLock))    
    {        
        if(errno != EINTR)
        {
            BOSS_ERROR("WLock %s Idx %u Error %s", m_sFileName.c_str(), uiIdx, strerror(errno));
			return -1;
        }
    }    	

	return 0;
}

int FileRWLock::UnLock(u_int32_t uiIdx)
{
    if(m_iFD < 0)
		return -1;

	if(uiIdx > m_uiLockSize)
		return -1;
	
    struct flock stLock;    
	stLock.l_type = F_UNLCK;    
	stLock.l_whence = SEEK_SET;    
	stLock.l_start = uiIdx;    
	stLock.l_len = 1;    
	stLock.l_pid = -1;    
	while(fcntl(m_iFD, F_SETLKW, &stLock))
		;
	return 0;
}


