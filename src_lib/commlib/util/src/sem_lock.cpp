#include "sem_lock.h"

using namespace boss::util;

SemLock::SemLock()
{
    m_iSemID = -1;
	m_iSemKey = -1;
}

int SemLock::Open(int iSemKey)
{
    assert(iSemKey >= 0);

    m_iSemKey = iSemKey;
	
	bool bIsNew = true;
	if(-1 == (m_iSemID = semget(m_iSemKey, 1, IPC_CREAT|IPC_EXCL|0664)))
	{
	    if(-1 == (m_iSemID = semget(m_iSemKey, 1, 0664)))
		{
			BOSS_ERROR("semget %d Error: %s", m_iSemKey, strerror(errno));
			m_iSemID = m_iSemKey = -1;
			return -1;
        }
	    bIsNew = false;
	}

	BOSS_INFO("semget(%d) = %d OK", m_iSemKey, m_iSemID);
	
	if(bIsNew)
	{
	    union semun arg;
		ushort sem_array[1] = {1};
		arg.array = sem_array;

		if(semctl(m_iSemID, 0, SETALL, arg) == -1)
		{
		    BOSS_ERROR("semctl %d, SETALL Error: %s", m_iSemKey, strerror(errno));
			m_iSemID = m_iSemKey = -1;
			return -1;
		}

		BOSS_INFO("semctl(%d) INIT", m_iSemKey);
	}

	return 0;
}

void SemLock::Status()
{
    assert(m_iSemID >= 0);
	
    BOSS_INFO("Status: %d = %d", m_iSemKey, semctl(m_iSemID, 0, GETVAL, 0));
}

int SemLock::Lock()
{
    struct sembuf stAcquire = {0, -1, SEM_UNDO};

	if(semop(m_iSemID, &stAcquire, 1) == -1)
	{
	    BOSS_ERROR("semop(%d) Error: %s", m_iSemKey, strerror(errno));
		return -1;
	}

	return 0;
}

int SemLock::Unlock()
{
    struct sembuf stRelease = {0, 1, SEM_UNDO};

	if(semop(m_iSemID, &stRelease, 1) == -1)
	{
	    BOSS_ERROR("semop(%d) Error: %s", m_iSemKey, strerror(errno));
		return -1;
	}

	return 0;
}


