#include	"cacheoperV2.h"

using namespace iori_cache_v2;

CacheBaseV2::CacheBaseV2()
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

CacheBaseV2::~CacheBaseV2()
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

int		CacheBaseV2::CreateShm(key_t tShmKey,int iShmSize,int iFlag)
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

int		CacheBaseV2::CreateCache(key_t tCacheKey,int iKeySize,int iValueSize,int iNodeNum)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;

	this->~CacheBaseV2();
	if (iKeySize <= 0 || iValueSize < 0 || iNodeNum <= 0)
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
	if ((iInfoShmId = CreateShm(m_tInfoShmKey,(sizeof(int) * 3 + iKeySize + iValueSize) * iNodeNum,SHM_CREATE)) < 0)
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
		this->~CacheBaseV2();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoot = (HashPoint *)(pHashShm + sizeof(CacheCtl));

	SemLock();
	m_pCacheCtl->iKeySize = iKeySize;
	m_pCacheCtl->iInfoSize = iValueSize;
	m_pCacheCtl->iTotalKey_InfoLen = m_pCacheCtl->iKeySize + m_pCacheCtl->iInfoSize + sizeof(int) * 3;

	m_pCacheCtl->iMaxInfoNum = iNodeNum;

	memset(m_pHashRoot,-1,sizeof(HashPoint) * iNodeNum);
	//bzero(m_pInfoRoot,m_pCacheCtl->iTotalKey_InfoLen * iNodeNum);

	ResetInfoList();

	m_pCacheCtl->cValidFlag = 1;

	SemUnLock();

	return	0;
}

int		CacheBaseV2::AttachCache(key_t tCacheKey)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;

	this->~CacheBaseV2();
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
		this->~CacheBaseV2();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoot = (HashPoint *)(pHashShm + sizeof(CacheCtl));

	return	0;
}

void	CacheBaseV2::InitFuncRels(int (*pKeyToHashFunc)(char *),int (*pKeyCompFunc)(char *,char *))
{
	m_pKeyToHashCodeFunc = pKeyToHashFunc;
	m_pKeyCompFunc = pKeyCompFunc;	
}

int		CacheBaseV2::AddCacheBlk_DirectToHashHead(char *sKey,int iKeyLen,char *sValue,int iValueLen,char cReplaceWhenFull)
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
		this->~CacheBaseV2();
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
	if (m_pCacheCtl->iInfoSize > 0)		// 支持value=0的cache类型
	{
		if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Value null or too long");
			return	-2;
		}
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);
	iHashCode %= m_pCacheCtl->iMaxInfoNum;

	SemLock();
	iRet = StoreInfoInCache(sKey,iKeyLen,sValue,iValueLen,iHashCode,cReplaceWhenFull,1);
	SemUnLock();
	
	return	iRet;	
}

int		CacheBaseV2::AddCacheBlk(char *sKey,int iKeyLen,char *sValue,int iValueLen,char cReplaceWhenFull)
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
		this->~CacheBaseV2();
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
	if (m_pCacheCtl->iInfoSize > 0)		// 支持value=0的cache类型
	{
		if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Value null or too long");
			return	-2;
		}
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);
	iHashCode %= m_pCacheCtl->iMaxInfoNum;

	SemLock();
	iRet = StoreInfoInCache(sKey,iKeyLen,sValue,iValueLen,iHashCode,cReplaceWhenFull,0);
	SemUnLock();
	
	return	iRet;
}

int		CacheBaseV2::StoreInfoInCache(char *sKey,int iKeyLen,char *sValue,int iValueLen,int iHashCode,
							char cReplaceWhenFull,char cDirectToHead)
{
	int				iNextOffSet;
	int				iFreeListHead;
	int				iSizeCnt = 0;
	const int		iMinus = -1;
	char			*pInfoToStore = NULL;
	char			*pInfoList = NULL;
	HashPoint		*pHashPoint = NULL;

	pHashPoint = m_pHashRoot + iHashCode;
	if (pHashPoint->iInfoListHead < 0)
	{
		// space full?
		if (m_pCacheCtl->iUseInfoNum >= m_pCacheCtl->iMaxInfoNum || m_pCacheCtl->iFreeListHead < 0)
		{
			if (!cReplaceWhenFull)
			{
				snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache space full");
				return	-1;
			}
			// 空间满,自动放弃最老的数据
			if (TakeOverTheOldestData(m_pInfoRoot+m_pCacheCtl->iTotalKey_InfoLen*m_pCacheCtl->iTimeListHead + sizeof(int) * 3,
									m_pCacheCtl->iKeySize,m_pCacheCtl->iTimeListHead) < 0)
			{
				return	-1;
			}
		}
		iFreeListHead = m_pCacheCtl->iFreeListHead;
		pInfoToStore = m_pInfoRoot + iFreeListHead * m_pCacheCtl->iTotalKey_InfoLen;
		memcpy(&iNextOffSet,pInfoToStore,sizeof(int));
		m_pCacheCtl->iFreeListHead = iNextOffSet;
		if (m_pCacheCtl->iFreeListHead < 0)
			m_pCacheCtl->iFreeListTail = -1;

		bzero(pInfoToStore,m_pCacheCtl->iTotalKey_InfoLen);
		memcpy(pInfoToStore,&iMinus,sizeof(int));
		memcpy(pInfoToStore + sizeof(int),&iMinus,sizeof(int));
		memcpy(pInfoToStore + sizeof(int) * 2,&iMinus,sizeof(int));
		memcpy(pInfoToStore + sizeof(int) * 3,sKey,iKeyLen);
		if (m_pCacheCtl->iInfoSize > 0)
			memcpy(pInfoToStore + sizeof(int) * 3 + m_pCacheCtl->iKeySize,sValue,iValueLen);
		pHashPoint->iInfoListHead = iFreeListHead;
		this->PushIntoTimeList(iFreeListHead);
		m_pCacheCtl->iUseInfoNum++;

		return	0;
	}

	iNextOffSet = pHashPoint->iInfoListHead;

	if (!cDirectToHead)
	{
		while (iNextOffSet >= 0)
		{
			pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
			// 当Key存在"重合",保证Key的唯一性
			if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int) * 3) == COMP_EQUAL)
			{
				if (m_pCacheCtl->iInfoSize > 0)
				{
					bzero(pInfoList + sizeof(int) * 3 + m_pCacheCtl->iKeySize,m_pCacheCtl->iInfoSize);
					memcpy(pInfoList + sizeof(int) * 3 + m_pCacheCtl->iKeySize,sValue,iValueLen);
				}
				if (iNextOffSet != m_pCacheCtl->iTimeListTail)
				{
					this->EraseInTimeList(iNextOffSet);
					this->PushIntoTimeList(iNextOffSet);
				}

				return	0;
			}
			memcpy(&iNextOffSet,pInfoList,sizeof(int));
			if (iSizeCnt++ >= MAX_LIST_SIZE)
			{
				snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Too many info on hashcode:%d",iHashCode);
				return	-1;
			}
		}
	}
	if (m_pCacheCtl->iUseInfoNum >= m_pCacheCtl->iMaxInfoNum || m_pCacheCtl->iFreeListHead < 0)
	{
		if (!cReplaceWhenFull)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache space full");
			return	-1;
		}
		if (TakeOverTheOldestData(m_pInfoRoot+m_pCacheCtl->iTotalKey_InfoLen*m_pCacheCtl->iTimeListHead + sizeof(int) * 3,
								m_pCacheCtl->iKeySize,m_pCacheCtl->iTimeListHead) < 0)
		{
			return	-1;
		}
		return	this->StoreInfoInCache(sKey,iKeyLen,sValue,iValueLen,iHashCode,cReplaceWhenFull,cDirectToHead);
	}

	iFreeListHead = m_pCacheCtl->iFreeListHead;
	pInfoToStore = m_pInfoRoot + iFreeListHead * m_pCacheCtl->iTotalKey_InfoLen;
	memcpy(&iNextOffSet,pInfoToStore,sizeof(int));
	m_pCacheCtl->iFreeListHead = iNextOffSet;
	if (m_pCacheCtl->iFreeListHead < 0)
		m_pCacheCtl->iFreeListTail = -1;

	bzero(pInfoToStore,m_pCacheCtl->iTotalKey_InfoLen);

	if (!cDirectToHead)
	{
		memcpy(pInfoToStore,&iMinus,sizeof(int));
		memcpy(pInfoList,&iFreeListHead,sizeof(int));
	}
	else
	{
		memcpy(pInfoToStore,&(pHashPoint->iInfoListHead),sizeof(int));
		pHashPoint->iInfoListHead = iFreeListHead;	
	}

	memcpy(pInfoToStore + sizeof(int),&iMinus,sizeof(int));
	memcpy(pInfoToStore + sizeof(int) * 2,&iMinus,sizeof(int));
	memcpy(pInfoToStore + sizeof(int) * 3,sKey,iKeyLen);
	if (m_pCacheCtl->iInfoSize > 0)
		memcpy(pInfoToStore + sizeof(int) * 3 + m_pCacheCtl->iKeySize,sValue,iValueLen);
	this->PushIntoTimeList(iFreeListHead);

	m_pCacheCtl->iUseInfoNum++;

	return	0;
}

int		CacheBaseV2::TakeOverTheOldestData(char *sKey,int iKeyLen,int iNodePos)
{
	int			iHashCode;
	int			iNextOffSet;
	int			*pForeInfoNode;
	const int	iMinus = -1;
	char		*pInfoList;	
	HashPoint	*pHashPoint;

	if (sKey == NULL || iKeyLen <= 0 || iKeyLen > m_pCacheCtl->iKeySize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Drop oldest data fail:key null or too long");
		return	-2;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Drop oldest data fail:in-use info num == 0");
		return	-1;
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	pHashPoint = m_pHashRoot + iHashCode;

	if (pHashPoint->iInfoListHead < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Drop oldest data fail:Oldest data not found<1>");
		return	-1;
	}
	iNextOffSet = pHashPoint->iInfoListHead;
	pForeInfoNode = &(pHashPoint->iInfoListHead);
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if (iNextOffSet == iNodePos)
		{
			memcpy(pForeInfoNode,pInfoList,sizeof(int));
			memcpy(pInfoList,&iMinus,sizeof(int));

			this->EraseInTimeList(iNextOffSet);

			if (m_pCacheCtl->iFreeListTail < 0)
			{
				m_pCacheCtl->iFreeListHead = m_pCacheCtl->iFreeListTail = iNextOffSet;
				m_pCacheCtl->iUseInfoNum--;

				return	0;
			}
			memcpy(m_pInfoRoot + m_pCacheCtl->iFreeListTail * m_pCacheCtl->iTotalKey_InfoLen,&iNextOffSet,sizeof(int));
			m_pCacheCtl->iFreeListTail = iNextOffSet;
			m_pCacheCtl->iUseInfoNum--;

			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
		pForeInfoNode = (int *)pInfoList;
	}
	snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Drop oldest data fail:Oldest data not found<2>");
	return	-1;	
}

int		CacheBaseV2::SearchCacheBlk(char *sKey,int iKeyLen,char *sValue,int iValueLen)
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
		this->~CacheBaseV2();
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
	if (m_pCacheCtl->iInfoSize > 0)
	{
		if (sValue == NULL || iValueLen < m_pCacheCtl->iInfoSize)
		{
			snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Output space null or too short");
			return	-2;
		}
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	pHashPoint = m_pHashRoot + iHashCode;

	if ((iNextOffSet = pHashPoint->iInfoListHead) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<1>");
		return	-1;
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int) * 3) == COMP_EQUAL)
		{
			if (m_pCacheCtl->iInfoSize > 0)
				memcpy(sValue,pInfoList + sizeof(int) * 3 + m_pCacheCtl->iKeySize,m_pCacheCtl->iInfoSize);

			if (iNextOffSet != m_pCacheCtl->iTimeListTail)
			{
				SemLock();
				this->EraseInTimeList(iNextOffSet);
				this->PushIntoTimeList(iNextOffSet);
				SemUnLock();
			}
			
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
	}
	snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<3>");
	return	-1;
}

int		CacheBaseV2::SearchCacheBlkList(char *sKey,int iKeyLen,std::list<const char *> &DataRootAddrList)
{
	int			iHashCode;
	int			iNextOffSet;
	char		*pInfoList;
	HashPoint	*pHashPoint;
	const char	*pDataRoot = NULL;

	DataRootAddrList.clear();

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Not init locally");	
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Cache deleted");
		this->~CacheBaseV2();
		return	-5;
	}
	if (!m_pKeyToHashCodeFunc || !m_pKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Not init local funcs");
		return	-4;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Empty cache");
		return	-1;
	}
	if (sKey == NULL || iKeyLen <= 0 || iKeyLen > m_pCacheCtl->iKeySize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Key null or too long");
		return	-2;
	}
	iHashCode = (*m_pKeyToHashCodeFunc)(sKey);
	if (iHashCode < 0)
		iHashCode = iHashCode * (-1);

	iHashCode %= m_pCacheCtl->iMaxInfoNum;
	pHashPoint = m_pHashRoot + iHashCode;

	if ((iNextOffSet = pHashPoint->iInfoListHead) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk list fail:Not found<1>");
		return	-1;
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int) * 3) == COMP_EQUAL)
		{
			pDataRoot = pInfoList + sizeof(int) * 3;
			DataRootAddrList.push_back(pDataRoot);

			if (iNextOffSet != m_pCacheCtl->iTimeListTail)
			{
				SemLock();
				this->EraseInTimeList(iNextOffSet);
				this->PushIntoTimeList(iNextOffSet);
				SemUnLock();
			}
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
	}
	return	DataRootAddrList.size();	
}

int		CacheBaseV2::EraseCacheBlk(char *sKey,int iKeyLen)
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
		this->~CacheBaseV2();
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
	if (pHashPoint->iInfoListHead < 0)
	{
		SemUnLock();
		return	0;
	}
	iNextOffSet = pHashPoint->iInfoListHead;
	pForeInfoNode = &(pHashPoint->iInfoListHead);
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pKeyCompFunc)(sKey,pInfoList + sizeof(int) * 3) == COMP_EQUAL)
		{
			memcpy(pForeInfoNode,pInfoList,sizeof(int));
			memcpy(pInfoList,&iMinus,sizeof(int));
			this->EraseInTimeList(iNextOffSet);

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

int		CacheBaseV2::ClearCache()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Clear cache fail:Not init locally");
		return	-1;	
	}
	SemLock();
	memset(m_pHashRoot,-1,sizeof(HashPoint) * m_pCacheCtl->iMaxInfoNum);
//	bzero(m_pInfoRoot,m_pCacheCtl->iTotalKey_InfoLen * m_pCacheCtl->iMaxInfoNum);
	ResetInfoList();
	SemUnLock();

	return	0;	
}

int		CacheBaseV2::DestroyCache()
{
	int		iHashShmId;
	int		iInfoShmId;
	
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Destory cache fail:Not init locally");
		return	-1;		
	}
	m_pCacheCtl->cValidFlag = 0;
	this->~CacheBaseV2();
	iHashShmId = CreateShm(m_tHashShmKey,0,SHM_ATTACH);
	iInfoShmId = CreateShm(m_tInfoShmKey,0,SHM_ATTACH);
	shmctl(iHashShmId,IPC_RMID,(struct shmid_ds *)NULL);
	shmctl(iInfoShmId,IPC_RMID,(struct shmid_ds *)NULL);
	SemLockRm();
	
	return	0;
}

int		CacheBaseV2::ResetInfoList()
{
	int			iNextOffSet;
	const	int	iMinus = -1;

	for (int i = 0;i < m_pCacheCtl->iMaxInfoNum;i++)
	{
		iNextOffSet = i + 1;
		if (i == m_pCacheCtl->iMaxInfoNum - 1)
			iNextOffSet = -1;

		memcpy(m_pInfoRoot + i * m_pCacheCtl->iTotalKey_InfoLen,&iNextOffSet,sizeof(int));
		memcpy(m_pInfoRoot + i * m_pCacheCtl->iTotalKey_InfoLen + sizeof(int),&iMinus,sizeof(int));
		memcpy(m_pInfoRoot + i * m_pCacheCtl->iTotalKey_InfoLen + sizeof(int) * 2,&iMinus,sizeof(int));
	}
	m_pCacheCtl->iUseInfoNum = 0;

	m_pCacheCtl->iFreeListHead = 0;
	m_pCacheCtl->iFreeListTail = m_pCacheCtl->iMaxInfoNum - 1;

	m_pCacheCtl->iTimeListHead = -1;
	m_pCacheCtl->iTimeListTail = -1;

	return	0;
}

int		CacheBaseV2::SemLockInit(key_t tSemKey,int iFlag)
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

void	CacheBaseV2::SemLockRm()
{
	if (m_iSemLockID < 0)
		return;

	semctl(m_iSemLockID,IPC_RMID,0);
	m_iSemLockID = -1;
}

int		CacheBaseV2::SemLock()
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

int		CacheBaseV2::SemUnLock()
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


// 将节点推入时间链表尾~
int		CacheBaseV2::PushIntoTimeList(int iNodePos)
{
	int		iTimeListTail;
	char	*pInfoForward = NULL;
	char	*pInfoToStore = NULL;
	
	if (m_pCacheCtl->iTimeListTail < 0)
	{
		m_pCacheCtl->iTimeListHead = m_pCacheCtl->iTimeListTail = iNodePos;		
		return	0;
	}
	iTimeListTail = m_pCacheCtl->iTimeListTail;
	pInfoForward = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iTimeListTail;
	pInfoToStore = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNodePos;

	memcpy(pInfoForward + sizeof(int) * 2,&iNodePos,sizeof(int));
	memcpy(pInfoToStore + sizeof(int),&iTimeListTail,sizeof(int));

	m_pCacheCtl->iTimeListTail = iNodePos;

	return	0;
}

// 将节点从时间链表中删除
int		CacheBaseV2::EraseInTimeList(int iNodePos)
{
	int			iForward,iBackward;
	const int 	iMinus = -1;
	char		*pInfoForward = NULL;
	char		*pInfoBackward = NULL;
	char		*pInfoToStore = NULL;
	
	pInfoToStore = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNodePos;
	memcpy(&iForward,pInfoToStore + sizeof(int),sizeof(int));
	memcpy(&iBackward,pInfoToStore + sizeof(int) * 2,sizeof(int));
	
	if (iForward >= 0)
	{
		pInfoForward = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iForward;
		memcpy(pInfoToStore + sizeof(int),&iMinus,sizeof(int));
	}
	if (iBackward >= 0)
	{
		pInfoBackward = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iBackward;
		memcpy(pInfoToStore + sizeof(int) * 2,&iMinus,sizeof(int));
	}
	if (pInfoForward)
		memcpy(pInfoForward + sizeof(int) * 2,&iBackward,sizeof(int));
	if (pInfoBackward)
		memcpy(pInfoBackward + sizeof(int),&iForward,sizeof(int));
	
	if (m_pCacheCtl->iTimeListTail == iNodePos)
		m_pCacheCtl->iTimeListTail = iForward;

	if (m_pCacheCtl->iTimeListHead == iNodePos)
		m_pCacheCtl->iTimeListHead = iBackward;
	
	if (m_pCacheCtl->iTimeListHead < 0 || m_pCacheCtl->iTimeListTail < 0)
	{
		m_pCacheCtl->iTimeListHead = -1;
		m_pCacheCtl->iTimeListTail = -1;
	}
	
	return	0;
}

int		CacheBaseV2::GetCurCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;		
	}
	return	m_pCacheCtl->iUseInfoNum;	
}

int		CacheBaseV2::GetMaxCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;			
	}	
	return	m_pCacheCtl->iMaxInfoNum;
}

int		CacheBaseV2::GetKeySize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get key size fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iKeySize;
}

int		CacheBaseV2::GetInfoSize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get info sizeo fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iInfoSize;
}

int		CacheBaseV2::GetAllInUseDataAddr(std::list<const char *> &DataRootAddrList)
{
	int			iInUseNode;
	const char	*pDataRoot = NULL;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get all in-use data addr fail:Not init locally");
		return	-1;
	}
	DataRootAddrList.clear();

	iInUseNode = m_pCacheCtl->iTimeListHead;

	while (iInUseNode >= 0)
	{
		pDataRoot = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iInUseNode + sizeof(int) * 3;

		DataRootAddrList.push_back(pDataRoot);

		memcpy(&iInUseNode,m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iInUseNode + sizeof(int) * 2,sizeof(int));
	}

	return	DataRootAddrList.size();
}

const CacheCtl *CacheBaseV2::GetCacheCtlRootAddr()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cache-ctl root addr fail:Not init locally");	
		return	(const CacheCtl *)NULL;
	}

	return	(const CacheCtl *)m_pCacheCtl;
}

char	*CacheBaseV2::GetErrMsg()
{
	return	m_sErrMsg;
}
