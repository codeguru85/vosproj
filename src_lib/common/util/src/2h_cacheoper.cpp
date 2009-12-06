#include	"2h_cacheoper.h"

TwoIndexCacheBase::TwoIndexCacheBase()
{
	m_tHashShmKey = 0;				// Hash-shm key
	m_tInfoShmKey = 0;				// 数据shm key

	m_pCacheCtl = NULL;				// CacheCtl节点地址
	m_pHashRoots[0] = NULL;				// Hash节点根地址
	m_pHashRoots[1] = NULL;
	m_pInfoRoot = NULL;				// 数据节点根地址
	m_sErrMsg[0] = 0;

	m_pPriKeyToHashIntFunc = NULL;		// Hash算法函数指针
	m_p2ndKeyToHashIntFunc = NULL;

	m_pPriKeyCompFunc = NULL;			// Key compare算法函数指针
	m_p2ndKeyCompFunc = NULL;
}

TwoIndexCacheBase::~TwoIndexCacheBase()
{
	if (m_pCacheCtl)
	{
		shmdt(m_pCacheCtl);
		m_pCacheCtl = NULL;
		m_pHashRoots[0] = NULL;		
		m_pHashRoots[1] = NULL;
	}
	if (m_pInfoRoot)
	{
		shmdt(m_pInfoRoot);
		m_pInfoRoot = NULL;
	}
}

int		TwoIndexCacheBase::CreateShm(key_t tShmKey,int iShmSize,int iFlag)
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

int		TwoIndexCacheBase::CreateCache(key_t tCacheKey,int iPriKeySize,int i2ndKeySize,int iValueSize,int iNodeNum)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;

	this->~TwoIndexCacheBase();

	if (iPriKeySize <= 0 || i2ndKeySize <= 0 || iValueSize <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:Err input params");
		return	-1;
	}
	if (!(tCacheKey & 0x00000001))
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:Pls create cache with odd key");
		return	-1;
	}
	if (iNodeNum <= 0)
		iNodeNum = DEFAULT_NODE_NUM;

	m_tHashShmKey = tCacheKey;
	m_tInfoShmKey = tCacheKey + (key_t)0x00000001;

	if ((iHashShmId = CreateShm(m_tHashShmKey,sizeof(CacheCtl) + sizeof(HashPoint) * iNodeNum * 2,SHM_CREATE)) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create cache fail:%s",strerror(errno));
		return	-2;
	}
	if ((iInfoShmId = CreateShm(m_tInfoShmKey,(sizeof(int) * 2 + iPriKeySize + i2ndKeySize + iValueSize) * iNodeNum,SHM_CREATE)) < 0)
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
		this->~TwoIndexCacheBase();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoots[0] = (HashPoint *)(pHashShm + sizeof(CacheCtl));
	m_pHashRoots[1] = (HashPoint *)(pHashShm + sizeof(CacheCtl) + sizeof(HashPoint) * iNodeNum);

	SemLock();
	m_pCacheCtl->cValidFlag = 1;
	m_pCacheCtl->iKeySizes[0] = iPriKeySize;
	m_pCacheCtl->iKeySizes[1] = i2ndKeySize;
	m_pCacheCtl->iInfoSize = iValueSize;
	m_pCacheCtl->iTotalKey_InfoLen = sizeof(int) * 2 + iPriKeySize + i2ndKeySize + iValueSize;
	m_pCacheCtl->iMaxInfoNum = iNodeNum;

	bzero(m_pHashRoots[0],sizeof(HashPoint) * iNodeNum);
	bzero(m_pHashRoots[1],sizeof(HashPoint) * iNodeNum);
	bzero(m_pInfoRoot,(m_pCacheCtl->iTotalKey_InfoLen) * iNodeNum);

	ResetInfoList();

	SemUnLock();

	return	0;
}

int		TwoIndexCacheBase::AttachCache(key_t tCacheKey)
{
	int			iHashShmId;
	int			iInfoShmId;
	char		*pHashShm = NULL;

	this->~TwoIndexCacheBase();

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
		this->~TwoIndexCacheBase();
		return	-2;
	}
	m_pCacheCtl = (CacheCtl *)pHashShm;
	m_pHashRoots[0] = (HashPoint *)(pHashShm + sizeof(CacheCtl));
	m_pHashRoots[1] = (HashPoint *)(pHashShm + sizeof(CacheCtl) + sizeof(HashPoint) * m_pCacheCtl->iMaxInfoNum);

	return	0;
}

void	TwoIndexCacheBase::InitFuncRels(int (*pPriKeyToHashInt)(char *),int (*pPriKeyComp)(char *,char *),
											int (*p2ndKeyToHashInt)(char *),int (*p2ndKeyComp)(char *,char *))
{
	m_pPriKeyToHashIntFunc = pPriKeyToHashInt;
	m_p2ndKeyToHashIntFunc = p2ndKeyToHashInt;

	m_pPriKeyCompFunc = pPriKeyComp;
	m_p2ndKeyCompFunc = p2ndKeyComp;
}

int		TwoIndexCacheBase::AddCacheBlk(char *sPriKey,int iPriKeyLen,char *s2ndKey,int i2ndKeyLen,char *sValue,int iValueLen)
{
	int				iRet;
	int				iPriHashCode;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Not init locally yet");
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Cache deleted");
		this->~TwoIndexCacheBase();
		return	-5;
	}
	if (!m_pPriKeyToHashIntFunc || !m_p2ndKeyToHashIntFunc || !m_pPriKeyCompFunc || !m_p2ndKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Not init local funcs");
		return	-4;
	}
	if (sPriKey == NULL || iPriKeyLen <= 0 || iPriKeyLen > m_pCacheCtl->iKeySizes[0])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Key null or too long");
		return	-2;
	}
	if (s2ndKey == NULL || i2ndKeyLen <= 0 || i2ndKeyLen > m_pCacheCtl->iKeySizes[1])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Key null or too long");
		return	-2;
	}
	if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk fail:Value null or too long");
		return	-2;
	}

	iPriHashCode = abs((*m_pPriKeyToHashIntFunc)(sPriKey));
	iPriHashCode %= m_pCacheCtl->iMaxInfoNum;

	iRet = StoreInfoInCache(sPriKey,iPriKeyLen,s2ndKey,i2ndKeyLen,sValue,iValueLen,iPriHashCode);

	return	iRet;
}

int		TwoIndexCacheBase::StoreInfoInCache(char *sPriKey,int iPriKeyLen,char *s2ndKey,int i2ndKeyLen,
												char *sValue,int iValueLen,int iPriHashCode)
{
	int				iTmpInt;
	int				iNextOffSet;
	int				iFreeListHead;
	int				i2ndHashCode;
	const int		iMinus = -1;
	char			*pInfoToStore = NULL;
	char			*pInfoList = NULL;
	HashPoint		*pPriHashPoint = NULL;
	HashPoint		*p2ndHashPoint = NULL;

	pPriHashPoint = m_pHashRoots[0] + iPriHashCode;

	i2ndHashCode = abs((*m_p2ndKeyToHashIntFunc)(s2ndKey));
	i2ndHashCode %= m_pCacheCtl->iMaxInfoNum;
	p2ndHashPoint = m_pHashRoots[1] + i2ndHashCode;	

	SemLock();
	if (pPriHashPoint->cValidFlag <= 0)
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
		memcpy(pInfoToStore + sizeof(int),&iMinus,sizeof(int));

		memcpy(pInfoToStore + sizeof(int) * 2,sPriKey,iPriKeyLen);
		memcpy(pInfoToStore + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0],s2ndKey,i2ndKeyLen);
		memcpy(pInfoToStore + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],sValue,iValueLen);
		pPriHashPoint->iInfoListHead = iFreeListHead;
		pPriHashPoint->cValidFlag = 1;
		m_pCacheCtl->iUseInfoNum++;

		Add2ndIndex(p2ndHashPoint,iFreeListHead);

		SemUnLock();
		return	0;
	}
	if ((iNextOffSet = pPriHashPoint->iInfoListHead) < 0)
	{
		SemUnLock();
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Add cache blk err:Exception.Space might be lost");
		return	-1;	
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pPriKeyCompFunc)(sPriKey,pInfoList + sizeof(int) * 2) == COMP_EQUAL)
		{
			bzero(pInfoList + sizeof(int)*2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],m_pCacheCtl->iInfoSize);
			memcpy(pInfoList + sizeof(int)*2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],sValue,iValueLen);

			if ((*m_p2ndKeyCompFunc)(s2ndKey,pInfoList + sizeof(int)*2 + m_pCacheCtl->iKeySizes[0]) != COMP_EQUAL)
			{
				iTmpInt = abs((*m_p2ndKeyToHashIntFunc)(pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0]));
				iTmpInt %= m_pCacheCtl->iMaxInfoNum;

				Del2ndIndex(m_pHashRoots[1] + iTmpInt,iNextOffSet);
				memcpy(pInfoList + sizeof(int),&iMinus,sizeof(int));
				bzero(pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0],m_pCacheCtl->iKeySizes[1]);
				memcpy(pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0],s2ndKey,i2ndKeyLen);
				Add2ndIndex(p2ndHashPoint,iNextOffSet);
			}

			SemUnLock();
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
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
	memcpy(pInfoToStore + sizeof(int),&iMinus,sizeof(int));

	memcpy(pInfoToStore + sizeof(int) * 2,sPriKey,iPriKeyLen);
	memcpy(pInfoToStore + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0],s2ndKey,i2ndKeyLen);
	memcpy(pInfoToStore + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],sValue,iValueLen);

	memcpy(pInfoList,&iFreeListHead,sizeof(int));
	m_pCacheCtl->iUseInfoNum++;

	Add2ndIndex(p2ndHashPoint,iFreeListHead);

	SemUnLock();

	return	0;
}

int		TwoIndexCacheBase::Add2ndIndex(HashPoint *p2ndHashPoint,int iNodeOffSet)
{
	int		iTmpInt;
	char	*pInfoList = NULL;

	if (p2ndHashPoint->cValidFlag <= 0)
	{
		p2ndHashPoint->cValidFlag = 1;
		p2ndHashPoint->iInfoListHead = iNodeOffSet;
	}
	else
	{
		iTmpInt = p2ndHashPoint->iInfoListHead;
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iTmpInt;

		while (true)
		{
			memcpy(&iTmpInt,pInfoList + sizeof(int),sizeof(int));
			if (iTmpInt >= 0)
				pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iTmpInt;
			else
				break;
		}
		memcpy(pInfoList + sizeof(int),&iNodeOffSet,sizeof(int));
	}
	return	0;
}

int		TwoIndexCacheBase::Del2ndIndex(HashPoint *p2ndHashPoint,int iNodeOffSet)
{
	int		iTmpInt;
	char	*pInfoList = NULL;

	if (p2ndHashPoint->cValidFlag <= 0)
		return	0;

	if (p2ndHashPoint->iInfoListHead == iNodeOffSet)
	{
		memcpy(&p2ndHashPoint->iInfoListHead,m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNodeOffSet + sizeof(int),sizeof(int));

		if (p2ndHashPoint->iInfoListHead < 0)
			p2ndHashPoint->cValidFlag = 0;

		return	0;
	}

	iTmpInt = p2ndHashPoint->iInfoListHead;
	pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iTmpInt;

	while (true)
	{
		memcpy(&iTmpInt,pInfoList + sizeof(int),sizeof(int));

		if (iTmpInt < 0)
			return	0;

		if (iTmpInt == iNodeOffSet)
		{
			memcpy(pInfoList + sizeof(int),m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNodeOffSet + sizeof(int),sizeof(int));
			return	0;
		}
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iTmpInt;
	}
}

int		TwoIndexCacheBase::SearchCacheBlkByPriKey(char *sPriKey,int iPriKeyLen,char *s2ndKey,
													int i2ndKeyLen,char *sValue,int iValueLen)
{
	int			iPriHashCode;
	int			iNextOffSet;
	char		*pInfoList;
	HashPoint	*pPriHashPoint;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init locally");	
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Cache deleted");
		this->~TwoIndexCacheBase();
		return	-5;
	}
	if (!m_pPriKeyToHashIntFunc || !m_pPriKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init local funcs");
		return	-4;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Empty cache");
		return	-1;
	}
	if (sPriKey == NULL || iPriKeyLen <= 0 || iPriKeyLen > m_pCacheCtl->iKeySizes[0])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Key null or too long");
		return	-2;
	}
	if (s2ndKey == NULL || i2ndKeyLen <= 0 || i2ndKeyLen > m_pCacheCtl->iKeySizes[1])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Output space null or too long");
		return	-2;
	}	
	if (sValue == NULL || iValueLen <= 0 || iValueLen > m_pCacheCtl->iInfoSize)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Output space null or too long");
		return	-2;
	}
	iPriHashCode = abs((*m_pPriKeyToHashIntFunc)(sPriKey));
	iPriHashCode %= m_pCacheCtl->iMaxInfoNum;
	pPriHashPoint = m_pHashRoots[0] + iPriHashCode;

	if (pPriHashPoint->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<1>");
		return	-1;
	}
	if ((iNextOffSet = pPriHashPoint->iInfoListHead) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<2>");
		return	-1;
	}
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pPriKeyCompFunc)(sPriKey,pInfoList + sizeof(int)*2) == COMP_EQUAL)
		{
			memcpy(s2ndKey,pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0],i2ndKeyLen);
			memcpy(sValue,pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],iValueLen);
			return	0;
		}
		memcpy(&iNextOffSet,pInfoList,sizeof(int));
	}
	snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<3>");
	return	-1;
}

int		TwoIndexCacheBase::RansackBy2ndKey(char *s2ndKey,int i2ndKeyLen,std::list<RansackNode>	&RanResults)
{
	int			i2ndHashCode;
	int			iNextOffSet;
	char		*pInfoList;
	HashPoint	*p2ndHashPoint;
	RansackNode	stRanNode;

	RanResults.clear();
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init locally");	
		return	-3;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Cache deleted");
		this->~TwoIndexCacheBase();
		return	-5;
	}
	if (!m_p2ndKeyToHashIntFunc || !m_p2ndKeyCompFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not init local funcs");
		return	-4;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Empty cache");
		return	0;
	}
	if (s2ndKey == NULL || i2ndKeyLen <= 0 || i2ndKeyLen > m_pCacheCtl->iKeySizes[1])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Key null or too long");
		return	-2;
	}
	i2ndHashCode = abs((*m_p2ndKeyToHashIntFunc)(s2ndKey));
	i2ndHashCode %= m_pCacheCtl->iMaxInfoNum;
	p2ndHashPoint = m_pHashRoots[1] + i2ndHashCode;

	if (p2ndHashPoint->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<1>");
		return	0;
	}
	if ((iNextOffSet = p2ndHashPoint->iInfoListHead) < 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Not found<2>");
		return	0;
	}

if (m_pCacheCtl->iKeySizes[0] > MAX_PRIKEY_LEN || m_pCacheCtl->iInfoSize > MAX_VALUE_LEN)
{
	snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Cant ransack the cache:PriKeySize<%d> ValueSize<%d> in cache:RansackMaxSpace<%d> <%d>",
					m_pCacheCtl->iKeySizes[0],m_pCacheCtl->iInfoSize,MAX_PRIKEY_LEN,MAX_VALUE_LEN);
	return	-2;
}

	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;

		if ((*m_p2ndKeyCompFunc)(s2ndKey,pInfoList + sizeof(int)*2 + m_pCacheCtl->iKeySizes[0]) == COMP_EQUAL)
		{
/*			stRanNode.sPriKey = new char[m_pCacheCtl->iKeySizes[0] + 1];
			stRanNode.sValue = new char[m_pCacheCtl->iInfoSize + 1];

			if (stRanNode.sPriKey == NULL || stRanNode.sValue == NULL)
			{
				snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Search cache blk fail:Allocate space err");
				return	-1;
			}
*/
bzero(&stRanNode,sizeof(stRanNode));
			memcpy(stRanNode.sPriKey,pInfoList + sizeof(int)*2,m_pCacheCtl->iKeySizes[0]);
			memcpy(stRanNode.sValue,pInfoList + sizeof(int)*2 + m_pCacheCtl->iKeySizes[0] + m_pCacheCtl->iKeySizes[1],m_pCacheCtl->iInfoSize);

			RanResults.push_back(stRanNode);

/*			stRanNode.sPriKey = NULL;
			stRanNode.sValue = NULL;
*/
		}

		memcpy(&iNextOffSet,pInfoList + sizeof(int),sizeof(int));
	}
	return	RanResults.size();	
}

int		TwoIndexCacheBase::EraseCacheBlk(char *sPriKey,int iPriKeyLen)
{
	int			iPriHashCode;
	int			i2ndHashCode;
	int			iNextOffSet;
	int			*pForeInfoNode;
	const int	iMinus = -1;
	char		*pInfoList;
	HashPoint	*pPriHashPoint;
	HashPoint	*p2ndHashPoint;

	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Not init locally");
		return	-2;
	}
	if (m_pCacheCtl->cValidFlag <= 0)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Cache deleted");
		this->~TwoIndexCacheBase();
		return	-4;
	}
	if (!m_pPriKeyToHashIntFunc || !m_pPriKeyCompFunc || !m_p2ndKeyToHashIntFunc)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Not init local funcs");
		return	-3;	
	}
	if (sPriKey == NULL || iPriKeyLen <= 0 || iPriKeyLen > m_pCacheCtl->iKeySizes[0])
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:key null or too long");
		return	-1;
	}
	if (m_pCacheCtl->iUseInfoNum == 0)
		return	0;

	iPriHashCode = abs((*m_pPriKeyToHashIntFunc)(sPriKey));
	iPriHashCode %= m_pCacheCtl->iMaxInfoNum;
	pPriHashPoint = m_pHashRoots[0] + iPriHashCode;

	SemLock();
	if (pPriHashPoint->cValidFlag <= 0)
	{
		SemUnLock();
		return	0;
	}
	if ((iNextOffSet = pPriHashPoint->iInfoListHead) < 0)
	{
		SemUnLock();
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Erase cache blk fail:Exception.Space might be lost");
		return	-1;
	}
	pForeInfoNode = &(pPriHashPoint->iInfoListHead);
	while (iNextOffSet >= 0)
	{
		pInfoList = m_pInfoRoot + m_pCacheCtl->iTotalKey_InfoLen * iNextOffSet;
		if ((*m_pPriKeyCompFunc)(sPriKey,pInfoList + sizeof(int) * 2) == COMP_EQUAL)
		{
			i2ndHashCode = abs((*m_p2ndKeyToHashIntFunc)(pInfoList + sizeof(int) * 2 + m_pCacheCtl->iKeySizes[0]));
			i2ndHashCode %= m_pCacheCtl->iMaxInfoNum;
			p2ndHashPoint = m_pHashRoots[1] + i2ndHashCode;
			Del2ndIndex(p2ndHashPoint,iNextOffSet);

			memcpy(pForeInfoNode,pInfoList,sizeof(int));
			memcpy(pInfoList,&iMinus,sizeof(int));
			memcpy(pInfoList + sizeof(int),&iMinus,sizeof(int));
			if (pPriHashPoint->iInfoListHead < 0)
				pPriHashPoint->cValidFlag = 0;
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

int		TwoIndexCacheBase::ClearCache()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Clear cache fail:Not init locally");
		return	-1;
	}
	SemLock();
	bzero(m_pHashRoots[0],sizeof(HashPoint) * m_pCacheCtl->iMaxInfoNum);
	bzero(m_pHashRoots[1],sizeof(HashPoint) * m_pCacheCtl->iMaxInfoNum);	
	bzero(m_pInfoRoot,(m_pCacheCtl->iTotalKey_InfoLen) * m_pCacheCtl->iMaxInfoNum);
	ResetInfoList();
	SemUnLock();

	return	0;
}

int		TwoIndexCacheBase::DestroyCache()
{
	int		iHashShmId;
	int		iInfoShmId;
	
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Destory cache fail:Not init locally");
		return	-1;		
	}
	m_pCacheCtl->cValidFlag = 0;
	this->~TwoIndexCacheBase();
	iHashShmId = CreateShm(m_tHashShmKey,0,SHM_ATTACH);
	iInfoShmId = CreateShm(m_tInfoShmKey,0,SHM_ATTACH);
	shmctl(iHashShmId,IPC_RMID,(struct shmid_ds *)NULL);
	shmctl(iInfoShmId,IPC_RMID,(struct shmid_ds *)NULL);
	SemLockRm();

	return	0;
}

int		TwoIndexCacheBase::ResetInfoList()
{
	int			iNextOffSet;

	for (int i = 0;i < m_pCacheCtl->iMaxInfoNum;i++)
	{
		iNextOffSet = i + 1;

		if (iNextOffSet == m_pCacheCtl->iMaxInfoNum)
			iNextOffSet = -1;

		memcpy(m_pInfoRoot + i * m_pCacheCtl->iTotalKey_InfoLen,&iNextOffSet,sizeof(int));
	}
	m_pCacheCtl->iUseInfoNum = 0;
	m_pCacheCtl->iFreeListHead = 0;
	m_pCacheCtl->iFreeListTail = m_pCacheCtl->iMaxInfoNum - 1;	

	return	0;
}

int		TwoIndexCacheBase::SemLockInit(key_t tSemKey,int iFlag)
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

void	TwoIndexCacheBase::SemLockRm()
{
	if (m_iSemLockID < 0)
		return;

	semctl(m_iSemLockID,IPC_RMID,0);
	m_iSemLockID = -1;
}

int		TwoIndexCacheBase::SemLock()
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

int		TwoIndexCacheBase::SemUnLock()
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

int		TwoIndexCacheBase::GetCurCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;
	}
	return	m_pCacheCtl->iUseInfoNum;	
}

int		TwoIndexCacheBase::GetMaxCacheBlkNum()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get cur cache blk fail:Not init locally");
		return	-1;
	}	
	return	m_pCacheCtl->iMaxInfoNum;
}

int		TwoIndexCacheBase::GetPriKeySize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get Pri-key size fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iKeySizes[0];
}

int		TwoIndexCacheBase::Get2ndKeySize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get 2nd-key size fail:Not init locally");
		return	-1;	
	}	
	return	m_pCacheCtl->iKeySizes[1];
}

int		TwoIndexCacheBase::GetInfoSize()
{
	if (!m_pCacheCtl)
	{
		snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get info sizeo fail:Not init locally");
		return	-1;	
	}
	return	m_pCacheCtl->iInfoSize;
}

char	*TwoIndexCacheBase::GetErrMsg()
{
	return	m_sErrMsg;
}
