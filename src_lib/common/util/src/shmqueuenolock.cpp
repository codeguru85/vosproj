#include	"shmqueuenolock.h"

QueueOperNoLock::QueueOperNoLock()
{
    m_tQueueKey = 0;
    m_pQueueCtl = NULL;
    m_pPkgRoot = NULL;
    m_iSemLockID = -1;
    bzero(m_sErrMsg,sizeof(m_sErrMsg)); 
}

QueueOperNoLock::~QueueOperNoLock()
{
    if ( m_pQueueCtl ) {
        shmdt(m_pQueueCtl);
        m_pQueueCtl = NULL;
        m_pPkgRoot = NULL;      
    }
}

int     QueueOperNoLock::CreateShm(key_t tShmKey,int iShmSize,int iFlag)
{
    int     iShmId;

    if ( iFlag == SHM_CREATE ) {
        iShmId = shmget(tShmKey,iShmSize,IPC_CREAT|IPC_EXCL|0666);
        return	iShmId;
    } else {
        iShmId = shmget(tShmKey,0,0666);
        return	iShmId;
    }   
}

int     QueueOperNoLock::CreateQueue(key_t tQueueKey,int iQueueSize)
{
    int     iQueueShmId;
    char    *pQueueShm = NULL;

    this->~QueueOperNoLock();

    if ( iQueueSize <= 0 )
        iQueueSize = DEFAULT_QUEUE_SIZE;

    m_tQueueKey = tQueueKey;
    if ( (iQueueShmId = CreateShm(m_tQueueKey,iQueueSize + sizeof(QueueCtl),SHM_CREATE)) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create queue fail:%s",strerror(errno));
        return	-1;
    }
    pQueueShm = (char *)shmat(iQueueShmId,NULL,0);
    if ( pQueueShm == NULL || (int)pQueueShm == -1 || SemLockInit(m_tQueueKey,SHM_CREATE) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Create queue fail:%s",strerror(errno));
        shmctl(iQueueShmId,IPC_RMID,(struct shmid_ds *)NULL);
        return	-1;
    }
    m_pQueueCtl = (QueueCtl *)pQueueShm;
    m_pPkgRoot = (char *)(pQueueShm + sizeof(QueueCtl));

    m_pQueueCtl->cValidFlag = 1;
    m_pQueueCtl->iSpaceSize = iQueueSize;
    m_pQueueCtl->iQueueHeadPos = 0;
    m_pQueueCtl->iQueueTailPos = 0;

    return	0;
}

int     QueueOperNoLock::AttachQueue(key_t tQueueKey)
{
    int         iQueueShmId;
    char        *pQueueShm = NULL;

    this->~QueueOperNoLock();

    m_tQueueKey = tQueueKey;
    if ( (iQueueShmId = CreateShm(m_tQueueKey,0,SHM_ATTACH)) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach queue fail:%s",strerror(errno));
        return	-1;
    }
    pQueueShm = (char *)shmat(iQueueShmId,NULL,0);
    if ( pQueueShm == NULL || (int)pQueueShm == -1 || SemLockInit(m_tQueueKey,SHM_ATTACH) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Attach queue fail:%s",strerror(errno));
        return	-1;
    }
    m_pQueueCtl = (QueueCtl *)pQueueShm;
    m_pPkgRoot = (char *)(pQueueShm + sizeof(QueueCtl));

    return	0;
}


int     QueueOperNoLock::PushDataIntoQueueBack(char *sValue,int iValueLen,char cLocked)
{
    int         iPkgLenWord;
    int         iResSpaceLen = 0;
    int         iCopyBeginPos = 0;
    char        sHeader[sizeof(int) + 1] = {0};

    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Not init locally");
        return	-3;
    }
    if ( m_pQueueCtl->cValidFlag <= 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Shm deleted");
        this->~QueueOperNoLock();
        return	-4;
    }
    if ( sValue == NULL || iValueLen <= 0 || iValueLen >= MAX_STOREBLK_LEN ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Data null or too long");
        return	-2;
    }
    iPkgLenWord = sizeof(int) + iValueLen;

    if ( cLocked == 1 )
        SemLock();

    if ( m_pQueueCtl->iQueueTailPos >= m_pQueueCtl->iQueueHeadPos )
        iResSpaceLen = m_pQueueCtl->iSpaceSize - m_pQueueCtl->iQueueTailPos + m_pQueueCtl->iQueueHeadPos - 1;
    else
        iResSpaceLen = m_pQueueCtl->iQueueHeadPos - m_pQueueCtl->iQueueTailPos - 1;

    if ( iPkgLenWord > iResSpaceLen ) {
        if ( cLocked == 1 )
            SemUnLock();

        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Space full");
        return	-1;
    }
    memcpy(sHeader,&iPkgLenWord,sizeof(int));
    iCopyBeginPos = StoreData(sHeader,sizeof(int),m_pQueueCtl->iQueueTailPos);
    m_pQueueCtl->iQueueTailPos = StoreData(sValue,iValueLen,iCopyBeginPos);

    if ( cLocked == 1 )
        SemUnLock();

    return	0;
}

int     QueueOperNoLock::ReadDataFromQueueHead(char* &ptoStoreSpace,char cLocked)
{
    int         iPkgLenWord;
    int         iCopyBeginPos = 0;
    char        sHeader[sizeof(int) + 1] = {0};

    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Not init locally");
        return	-3;
    }
    if ( m_pQueueCtl->cValidFlag <= 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Shm deleted");
        this->~QueueOperNoLock();
        return	-4;
    }
    if ( cLocked == 1 )
        SemLock();

    if ( m_pQueueCtl->iQueueHeadPos == m_pQueueCtl->iQueueTailPos ) {
        if ( cLocked == 1 )
            SemUnLock();

        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Empty queue");
        return	-1;
    }
    iCopyBeginPos = ReadData(sHeader,sizeof(int),m_pQueueCtl->iQueueHeadPos);
    memcpy(&iPkgLenWord,sHeader,sizeof(int));
    iPkgLenWord -= sizeof(int);
    try {
        ptoStoreSpace = new char[iPkgLenWord + 1];
    } catch ( ... ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:allocate space err");
        return	-2;
    }
    ptoStoreSpace[iPkgLenWord] = 0;
    m_pQueueCtl->iQueueHeadPos = ReadData(ptoStoreSpace,iPkgLenWord,iCopyBeginPos);

    if ( cLocked == 1 )
        SemUnLock();

    return	iPkgLenWord;    
}

int     QueueOperNoLock::ReadDataFromQueueHead(char *pToStoreSpace,int iSpaceLen,char cLocked)
{
    int         iPkgLenWord;
    int         iCopyBeginPos = 0;
    char        sHeader[sizeof(int) + 1] = {0};
    char        sBuffTmp[MAX_STOREBLK_LEN + 1];

    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Not init locally");
        return	-3;
    }
    if ( m_pQueueCtl->cValidFlag <= 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Shm deleted");
        this->~QueueOperNoLock();
        return	-4;
    }
    if ( pToStoreSpace == NULL || iSpaceLen <= 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Input param not valid");
        return	-2;
    }
    if ( cLocked == 1 )
        SemLock();

    if ( m_pQueueCtl->iQueueHeadPos == m_pQueueCtl->iQueueTailPos ) {
        if ( cLocked == 1 )
            SemUnLock();

        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Read data fail:Empty queue");
        return	-1;
    }
    iCopyBeginPos = ReadData(sHeader,sizeof(int),m_pQueueCtl->iQueueHeadPos);
    memcpy(&iPkgLenWord,sHeader,sizeof(int));
    m_pQueueCtl->iQueueHeadPos = ReadData(sBuffTmp,iPkgLenWord - sizeof(int),iCopyBeginPos);

    if ( cLocked == 1 )
        SemUnLock();

    iPkgLenWord -= sizeof(int);
    sBuffTmp[iPkgLenWord] = 0;
    if ( iPkgLenWord <= iSpaceLen )
        memcpy(pToStoreSpace,sBuffTmp,iPkgLenWord);
    else
        memcpy(pToStoreSpace,sBuffTmp,iSpaceLen);

    return	iPkgLenWord;
}

int     QueueOperNoLock::StoreData(char *sData,int iDataLen,int iQueueTailPos)
{
    int         iCopyLen;

    if ( (iQueueTailPos + iDataLen) > m_pQueueCtl->iSpaceSize )
        iCopyLen = m_pQueueCtl->iSpaceSize - iQueueTailPos;
    else
        iCopyLen = iDataLen;

    memcpy(m_pPkgRoot + iQueueTailPos,sData,iCopyLen);
    if ( iCopyLen < iDataLen ) {
        memcpy(m_pPkgRoot,sData + iCopyLen,iDataLen - iCopyLen);
    }
    iQueueTailPos += iDataLen;
    iQueueTailPos %= m_pQueueCtl->iSpaceSize;

    return	iQueueTailPos;
}

int     QueueOperNoLock::ReadData(char *sReadTo,int iDataLen,int iQueueHeadPos)
{
    int         iCopyLen;

    if ( (iQueueHeadPos + iDataLen) > m_pQueueCtl->iSpaceSize )
        iCopyLen = m_pQueueCtl->iSpaceSize - iQueueHeadPos;
    else
        iCopyLen = iDataLen;
    memcpy(sReadTo,m_pPkgRoot + iQueueHeadPos,iCopyLen);
    if ( iCopyLen < iDataLen ) {
        memcpy(sReadTo + iCopyLen,m_pPkgRoot,iDataLen - iCopyLen);
    }
    iQueueHeadPos += iDataLen;
    iQueueHeadPos %= m_pQueueCtl->iSpaceSize;

    return	iQueueHeadPos;
}

int     QueueOperNoLock::ClearQueue()
{
    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Clear queue fail:Not init locally");
        return	-1; 
    }
    m_pQueueCtl->iQueueTailPos = m_pQueueCtl->iQueueHeadPos;
    return	0;  
}

int     QueueOperNoLock::DestroyQueue()
{
    int     iQueueShmId;

    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Destory queue fail:Not init locally");
        return	-1;     
    }
    m_pQueueCtl->cValidFlag = 0;
    this->~QueueOperNoLock();
    iQueueShmId = CreateShm(m_tQueueKey,0,SHM_ATTACH);
    shmctl(iQueueShmId,IPC_RMID,(struct shmid_ds *)NULL);
    SemLockRm();

    return	0;  
}

int     QueueOperNoLock::IsQueueEmpty()
{
    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Get queue empty stat fail:Not init locally");
        return	-1; 
    }
    if ( m_pQueueCtl->iQueueTailPos == m_pQueueCtl->iQueueHeadPos )
        return	1;

    return	0;
}

int     QueueOperNoLock::SemLockInit(key_t tSemKey,int iFlag)
{
    if ( iFlag == SHM_CREATE ) {
        m_iSemLockID = semget(tSemKey,1,IPC_CREAT|0666);
        if ( m_iSemLockID < 0 ) {
            snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
            return	-1;
        }
        if ( semctl(m_iSemLockID,0,SETVAL,(int)1) < 0 ) {
            snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
            SemLockRm();
            return	-2;
        }
        return	m_iSemLockID;
    }
    m_iSemLockID = semget(tSemKey,1,0666);
    if ( m_iSemLockID < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem init fail:%s",strerror(errno));
        return	-1;
    }
    return	m_iSemLockID;
}

void    QueueOperNoLock::SemLockRm()
{
    if ( m_iSemLockID < 0 )
        return;

    semctl(m_iSemLockID,IPC_RMID,0);
    m_iSemLockID = -1;  
}

int     QueueOperNoLock::SemLock()
{
    sembuf  stSemBuf;

    if ( m_iSemLockID < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem lock fail:Not init");
        return	-1;
    }
    stSemBuf.sem_num = 0;

    stSemBuf.sem_op = -1;

    stSemBuf.sem_flg = SEM_UNDO;



    if ( semop(m_iSemLockID,&stSemBuf,1) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem lock fail:%s",strerror(errno));

        return	-1;
    }
    return	0;
}

int     QueueOperNoLock::SemUnLock()
{
    sembuf  stSemBuf;


    if ( m_iSemLockID < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem unlock fail:Not init");
        return	-1;
    }

    stSemBuf.sem_num = 0;

    stSemBuf.sem_op = 1;

    stSemBuf.sem_flg = SEM_UNDO;



    if ( semop(m_iSemLockID,&stSemBuf,1) < 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Sem unlock fail:%s",strerror(errno));
        return	-1;
    }
    return	0;  
}


char    *QueueOperNoLock::GetErrMsg()
{
    return		m_sErrMsg;
}


int     QueueOperNoLock::GetAvailableSize()
{
    if ( !m_pQueueCtl ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Not init locally");
        return	-1;
    }

    if ( m_pQueueCtl->cValidFlag <= 0 ) {
        snprintf(m_sErrMsg,sizeof(m_sErrMsg),"Store data fail:Shm deleted");
        return	-1;
    }

    //获取到的空间 -1 个字节，空位，用于是队列满时，首尾不相等，- sizeof(int)  存进来时会额外加上数据长度
    int iAvail = 0;
    if ( m_pQueueCtl->iQueueTailPos >= m_pQueueCtl->iQueueHeadPos ) {
        iAvail =  m_pQueueCtl->iSpaceSize - m_pQueueCtl->iQueueTailPos + m_pQueueCtl->iQueueHeadPos - 1 - sizeof(int);
    } else {
        iAvail =  m_pQueueCtl->iQueueHeadPos - m_pQueueCtl->iQueueTailPos - 1 - sizeof(int);
    }
    return iAvail<0?0:iAvail;
}
