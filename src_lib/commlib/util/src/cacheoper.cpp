#include	"cacheoper.h"

CacheBase::CacheBase()
{
	m_tHashShmKey = 0;				// Hash-shm key
	m_tInfoShmKey = 0;				// 数据shm key

	m_pCacheCtl = NULL;				// CacheCtl节点地址
	m_pHashRoot = NULL;				// Hash节点根地址
	m_pInfoRoot = NULL;				// 数据节点根地址
	
	m_pKeyToHashCodeFunc = NULL;	// Hash算法函数指针
	m_pKeyCompFunc = NULL;			// Key compare算法函数指针
	
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
}

CacheBase::~CacheBase()
{
	if (m_pCacheCtl)
	{
		shmdt(m_pCacheCtl);
		m_pCacheCtl = NULL;
		m_pHashRoot = NULL;		
	}
	if (m_pInfoRoot)
	{
		shmdt(m_pInfoRoot);
		m_pInfoRoot = NULL;
	}	
}

int		CacheBase::CreateShm(key_t tShmKey,int iShmSize,int iFlag)
{
	int		iShmId;

	if (iFlag == SHM_CREATE)
	{
		iShmId = shmget(tShmKey,iShmSize,IPC_CREAT|IPC_EXCL|0666);
		return	iShmId;
	}
	else
	{
		iShmId = shmget(tShmKey,0,0666);
		return	iShmId;
	}
}

int		CacheBase::CreateCache(key_t tCacheKey,int iKeySize,int iValueSize,int iNodeNum)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;
	
	this->~CacheBase();
	if (iKeySize <= 0 || iValueSize <= 0 || iNodeNum <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:Err input params");
		return	-1;
	}
	if (!(tCacheKey & 0x00000001))
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:Pls create cache with odd key");
		return	-1;
	}
	m_tHashShmKey = tCacheKey;
	m_tInfoShmKey = tCacheKey + (key_t)0x00000001;

	if ((iHashShmId = CreateShm(m_tHashShmKey,sizeof(CacheCtl) + sizeof(HashPoint) * iNodeNum,SHM_CREATE)) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:%s",strerror(errno));
		return	-2;
	}
	if ((iInfoShmId = CreateShm(m_tInfoShmKey,(sizeof(int) + iKeySize + iValueSize) * iNodeNum,SHM_CREATE)) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:%s",strerror(errno));
		shmctl(iHashShmId,IPC_RMID,(struct shmid_ds *)NULL);
		return	-2;
	}
	pHashShm = (char *)shmat(iHashShmId,NULL,0);
	m_pInfoRoot = (char *)shmat(iInfoShmId,NULL,0);
	if (pHashShm == NULL || (int)pHashShm == -1 || m_pInfoRoot == NULL || (int)m_pInfoRoot == -1 ||
			SemLockInit(m_tHashShmKey,SHM_CREATE) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:%s",strerror(errno));
		shmctl(iHashShmId,IPC_RMID,(struct shmid_ds *)NULL);
		shmctl(iInfoShmId,IPC_RMID,(struct shmid_ds *)NULL);
		this->~CacheBase();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoot = (HashPoint *)(pHashShm + sizeof(CacheCtl));

	m_pCacheCtl->iKeySize = iKeySize;
	m_pCacheCtl->iInfoSize = iValueSize;
	m_pCacheCtl->iTotalKey_InfoLen = m_pCacheCtl->iKeySize + m_pCacheCtl->iInfoSize + sizeof(int);
	m_pCacheCtl->iMaxInfoNum = iNodeNum;
	m_pCacheCtl->cValidFlag = 1;

	bzero(m_pHashRoot,sizeof(HashPoint) * iNodeNum);
	bzero(m_pInfoRoot,(sizeof(int) + iKeySize + iValueSize) * iNodeNum);

	ResetInfoList();

	return	0;
}

int		CacheBase::AttachCache(key_t tCacheKey)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;

	this->~CacheBase();
	if (!(tCacheKey & 0x00000001))
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach cache fail:Pls create cache with odd key");
		return	-1;
	}
	m_tHashShmKey = tCacheKey;
	m_tInfoShmKey = tCacheKey + (key_t)0x00000001;	

	if ((iHashShmId = CreateShm(m_tHashShmKey,0,SHM_ATTACH)) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach cache fail:%s",strerror(errno));
		return	-2;
	}
	if ((iInfoShmId = CreateShm(m_tInfoShmKey,0,SHM_ATTACH)) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach cache fail:%s",strerror(errno));
		return	-2;
	}
	pHashShm = (char *)shmat(iHashShmId,NULL,0);
	m_pInfoRoot = (char *)shmat(iInfoShmId,NULL,0);

	if (pHashShm == NULL || (int)pHashShm == -1 || m_pInfoRoot == NULL || (int)m_pInfoRoot == -1 ||
			SemLockInit(m_tHashShmKey,SHM_ATTACH) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach cache fail:%s",strerror(errno));
		this->~CacheBase();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoot = (HashPoint *)(pHashShm + sizeof(CacheCtl));

	return	0;
}

void	CacheBase::InitFuncRels(int (*pKeyToHashFunc)(char *),int (*pKeyCompFunc)(char *,char *))
{
	m_pKeyToHashCodeFunc = pKeyToHashFunc;
	m_pKeyCompFunc = pKeyCompFunc;	
}

int		CacheBase::AddCacheBlk(char *sKey,int iKeyLen,char *sValue,int iValueLen)
{
	int				iRet;
	int				iHashCode;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Not init locally yet");
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache deleted");
		this->~CacheBase();
		return	-5;
	}
	if (!m_pKeyToHashCodeFunc || !m_pKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Not init local funcs");
		return	-4;
	}
	if (sKey == NULL || iKeyLen <= 0 || iKeyLen > m_pCacheCtl->iKeySize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Key null or too long");
		return	-2;
	}
	if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Value null or too long");
		return	-2;
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	iRet = StoreInfoInCache(sKey,iKeyLen,sValue,iValueLen,iHashCode);
	return	iRet;
}

int		CacheBase::StoreInfoInCache(char *sKey,int iKeyLen,char *sValue,int iValueLen,int iHashCode)
{
	int				iNextOffSet;
	int				iFreeListHead;
	int				iSizeCnt = 0;
	const int		iMinus = -1;
	char			*pInfoToStore = NULL;
	char			*pInfoList = NULL;
	HashPoint		*pHashPoint = NULL;

	pHashPoint = m_pHashRoot + iHashCode;
	SemLock();
	if (pHashPoint->cValidFlag <= 0)
	{
		if (m_pCacheCtl->iUseInfoNum >= m_pCacheCtl->iMaxInfoNum || m_pCacheCtl->iFreeListHead < 0)
		{
			SemUnLock();
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache space full");
			return	-1;
		}
		iFreeListHead = m_pCacheCtl->iFreeListHead;
		pInfoToStore = m_pInfoRoot + iFreeListHead * m_pCacheCtl->iTotalKey_InfoLen;
		memcpy(&iNextOffSet,pInfoToStore,sizeof(int));
		m_pCacheCtl->iFreeListHead = iNextOffSet;
		if (m_pCacheCtl->iFreeListHead < 0)
			m_pCacheCtl->iFreeListTail = -1;

		bzero(pInfoToStore,m_pCacheCtl->iTotalKey_InfoLen);
		memcpy(pInfoToStore,&iMinus,sizeof(int));
		memcpy(pInfoToStore + sizeof(int),sKey,iKeyLen);
		memcpy(pInfoToStore + sizeof(int) + m_pCacheCtl->iKeySize,sValue,iValueLen);
		pHashPoint->iInfoListHead = iFreeListHead;
		pHashPoint->cValidFlag = 1;

		m_pCacheCtl->iUseInfoNum++;
		SemUnLock();
		return	0;
	}
	if ((iNextOffSet = pHashPoint->iInfoListHead) < 0)
	{
		SemUnLock();
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk err:Exception.Space might be lost");
		return	-1;	
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int)) == COMP_EQUAL)
		{
			bzero(pInfoList + sizeof(int) + m_pCacheCtl->iKeySize,m_pCacheCtl->iInfoSize);
			memcpy(pInfoList + sizeof(int) + m_pCacheCtl->iKeySize,sValue,iValueLen);
			SemUnLock();
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
		if (iSizeCnt++ >= MAX_LIST_SIZE)
		{
			SemUnLock();
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Too many info on hashcode:%d",iHashCode);
			return	-1;
		}
	}
	if (m_pCacheCtl->iUseInfoNum >= m_pCacheCtl->iMaxInfoNum || m_pCacheCtl->iFreeListHead < 0)
	{
		SemUnLock();
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache space full");
		return	-1;
	}
	iFreeListHead = m_pCacheCtl->iFreeListHead;
	pInfoToStore = m_pInfoRoot + iFreeListHead * m_pCacheCtl->iTotalKey_InfoLen;
	memcpy(&iNextOffSet,pInfoToStore,sizeof(int));
	m_pCacheCtl->iFreeListHead = iNextOffSet;
	if (m_pCacheCtl->iFreeListHead < 0)
		m_pCacheCtl->iFreeListTail = -1;

	bzero(pInfoToStore,m_pCacheCtl->iTotalKey_InfoLen);
	memcpy(pInfoToStore,&iMinus,sizeof(int));
	memcpy(pInfoToStore + sizeof(int),sKey,iKeyLen);
	memcpy(pInfoToStore + sizeof(int) + m_pCacheCtl->iKeySize,sValue,iValueLen);
	memcpy(pInfoList,&iFreeListHead,sizeof(int));

	m_pCacheCtl->iUseInfoNum++;
	SemUnLock();

	return	0;
}

int		CacheBase::SearchCacheBlk(char *sKey,int iKeyLen,char *sValue,int iValueLen)
{
	int			iHashCode;
	int			iNextOffSet;
	char		*pInfoList;
	HashPoint	*pHashPoint;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init locally");	
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Cache deleted");
		this->~CacheBase();
		return	-5;
	}
	if (!m_pKeyToHashCodeFunc || !m_pKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init local funcs");
		return	-4;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Empty cache");
		return	-1;	
	}
	if (sKey == NULL || iKeyLen <= 0 || iKeyLen > m_pCacheCtl->iKeySize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Key null or too long");
		return	-2;
	}
	if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Output space null or too long");
		return	-2;
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	pHashPoint = m_pHashRoot + iHashCode;
	if (pHashPoint->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<1>");
		return	-1;
	}
	if ((iNextOffSet = pHashPoint->iInfoListHead) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<2>");
		return	-1;
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int)) == COMP_EQUAL)
		{
			memcpy(sValue,pInfoList + sizeof(int) + m_pCacheCtl->iKeySize,iValueLen);
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
	}
	snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<3>");
	return	-1;
}

int		CacheBase::EraseCacheBlk(char *sKey,int iKeyLen)
{
	int			iHashCode;
	int			iNextOffSet;
	int			*pForeInfoNode;
	const int	iMinus = -1;
	char		*pInfoList;
	HashPoint	*pHashPoint;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Not init locally");
		return	-2;		
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Cache deleted");
		this->~CacheBase();
		return	-4;
	}
	if (!m_pKeyToHashCodeFunc || !m_pKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Not init local funcs");
		return	-3;	
	}
	if (sKey == NULL || iKeyLen <= 0 || iKeyLen > m_pCacheCtl->iKeySize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:key null or too long");
		return	-1;
	}	
	if (m_pCacheCtl->iUseInfoNum == 0)
		return	0;

	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	pHashPoint = m_pHashRoot + iHashCode;

	SemLock();
	if (pHashPoint->cValidFlag <= 0)
	{
		SemUnLock();
		return	0;
	}
	if ((iNextOffSet = pHashPoint->iInfoListHead) < 0)
	{
		SemUnLock();
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Exception.Space might be lost");
		return	-1;
	}
	pForeInfoNode = &(pHashPoint->iInfoListHead);
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int)) == COMP_EQUAL)
		{
			memcpy(pForeInfoNode,pInfoList,sizeof(int));
			memcpy(pInfoList,&iMinus,sizeof(int));
			if (pHashPoint->iInfoListHead < 0)
				pHashPoint->cValidFlag = 0;
			if (m_pCacheCtl->iFreeListTail < 0)
			{
				m_pCacheCtl->iFreeListHead = m_pCacheCtl->iFreeListTail = iNextOffSet;
				m_pCacheCtl->iUseInfoNum--;
				SemUnLock();
				return	0;
			}
			memcpy(m_pInfoRoot + m_pCacheCtl->iFreeListTail * m_pCacheCtl->iTotalKey_InfoLen,&iNextOffSet,sizeof(int));
			m_pCacheCtl->iFreeListTail = iNextOffSet;			
			m_pCacheCtl->iUseInfoNum--;
			SemUnLock();
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
		pForeInfoNode = (int *)pInfoList;
	}
	SemUnLock();
	return	0;
}

int		CacheBase::ClearCache()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Clear cache fail:Not init locally");
		return	-1;	
	}
	SemLock();
	bzero(m_pHashRoot,sizeof(HashPoint) * m_pCacheCtl->iMaxInfoNum);
	bzero(m_pInfoRoot,(sizeof(int) + m_pCacheCtl->iKeySize + m_pCacheCtl->iInfoSize) * m_pCacheCtl->iMaxInfoNum);
	ResetInfoList();
	SemUnLock();

	return	0;	
}

int		CacheBase::DestroyCache()
{
	int		iHashShmId;
	int		iInfoShmId;
	
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Destory cache fail:Not init locally");
		return	-1;		
	}
	m_pCacheCtl->cValidFlag = 0;
	this->~CacheBase();
	iHashShmId = CreateShm(m_tHashShmKey,0,SHM_ATTACH);
	iInfoShmId = CreateShm(m_tInfoShmKey,0,SHM_ATTACH);
	shmctl(iHashShmId,IPC_RMID,(struct shmid_ds *)NULL);
	shmctl(iInfoShmId,IPC_RMID,(struct shmid_ds *)NULL);
	SemLockRm();
	
	return	0;
}

int		CacheBase::ResetInfoList()
{
	int			iNextOffSet;

	for (int i = 0;i < m_pCacheCtl->iMaxInfoNum;i++)
	{
		iNextOffSet = i + 1;
		if (i == m_pCacheCtl->iMaxInfoNum - 1)
			iNextOffSet = -1;
		memcpy(m_pInfoRoot + i * m_pCacheCtl->iTotalKey_InfoLen,&iNextOffSet,sizeof(int));
	}
	m_pCacheCtl->iUseInfoNum = 0;
	m_pCacheCtl->iFreeListHead = 0;
	m_pCacheCtl->iFreeListTail = m_pCacheCtl->iMaxInfoNum - 1;	

	return	0;
}

int		CacheBase::SemLockInit(key_t tSemKey,int iFlag)
{
	if (iFlag == SHM_CREATE)
	{
		m_iSemLockID = semget(tSemKey,1,IPC_CREAT|0666);
		if (m_iSemLockID < 0)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
			return	-1;
		}
		if (semctl(m_iSemLockID,0,SETVAL,(int)1) < 0)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
			SemLockRm();
			return	-2;
		}
		return	m_iSemLockID;
	}
	m_iSemLockID = semget(tSemKey,1,0666);
	if (m_iSemLockID < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
		return	-1;
	}
	return	m_iSemLockID;	
}

void	CacheBase::SemLockRm()
{
	if (m_iSemLockID < 0)
		return;

	semctl(m_iSemLockID,IPC_RMID,0);
	m_iSemLockID = -1;
}

int		CacheBase::SemLock()
{
	sembuf	stSemBuf;

	if (m_iSemLockID < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem lock fail:Not init");
		return	-1;
	}
	stSemBuf.sem_num = 0;
	stSemBuf.sem_op = -1;
	stSemBuf.sem_flg = SEM_UNDO;

	if (semop(m_iSemLockID,&stSemBuf,1) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem lock fail:%s",strerror(errno));
		return	-1;
	}
	return	0;
}

int		CacheBase::SemUnLock()
{
	sembuf	stSemBuf;

	if (m_iSemLockID < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem unlock fail:Not init");
		return	-1;
	}
	stSemBuf.sem_num = 0;
	stSemBuf.sem_op = 1;
	stSemBuf.sem_flg = SEM_UNDO;

	if (semop(m_iSemLockID,&stSemBuf,1) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem unlock fail:%s",strerror(errno));
		return	-1;
	}
	return	0;
}

int		CacheBase::GetCurCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;		
	}
	return	m_pCacheCtl->iUseInfoNum;	
}

int		CacheBase::GetMaxCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;			
	}	
	return	m_pCacheCtl->iMaxInfoNum;
}

int		CacheBase::GetKeySize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get key size fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iKeySize;
}

int		CacheBase::GetInfoSize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get info sizeo fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iInfoSize;
}

char	*CacheBase::GetErrMsg()
{
	return	m_sErrMsg;
}
