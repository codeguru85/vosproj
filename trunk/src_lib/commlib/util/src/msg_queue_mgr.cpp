#include "msg_queue_mgr.h"


namespace boss
{
namespace task
{

MsgQueueMgr::MsgQueueMgr()
{
    m_iMsgQID = -1;
}

MsgQueueMgr::~MsgQueueMgr()
{
    Close();
}

void MsgQueueMgr::Close()
{
    if(m_iMsgQID >= 0)
    {        
        m_iMsgQID = -1;
    }
}

void MsgQueueMgr::Dump()
{
    assert(m_iMsgQID >= 0);

	struct msqid_ds stMsgStat;

	if(msgctl(m_iMsgQID, IPC_STAT, &stMsgStat) != 0)
	{
	    BOSS_ERROR("msgctl %d Error %s", m_iMsgQKey, strerror(errno));
		return;
	}

	BOSS_INFO("MsgQ[%d] Bytes: %u", m_iMsgQKey, stMsgStat.msg_qbytes);
	BOSS_INFO("MsgQ[%d] MsgNum: %u", m_iMsgQKey, stMsgStat.msg_qnum);
	BOSS_INFO("MsgQ[%d] CBytes: %u", m_iMsgQKey, stMsgStat.msg_cbytes);
}

int  MsgQueueMgr::Open(int iMsgQKey, u_int32_t uiQBytes = 0)
{
    assert(iMsgQKey > 0);
	
    Close();

    bool bIsNew = true;
	
	int iMsgID = msgget(iMsgQKey, IPC_CREAT | IPC_EXCL | 0664);
	if(-1 == iMsgID)
	{
	    if((iMsgID = msgget(iMsgQKey, 0664)) == -1)
		{
			BOSS_ERROR("msgget %d Error: %s", iMsgQKey, strerror(errno));
			return -1;
        }
	    bIsNew = false;
	}
	
	m_iMsgQID = iMsgID;
	m_iMsgQKey = iMsgQKey;
	
	BOSS_INFO("打开消息队列[%d -> %d]", m_iMsgQKey, m_iMsgQID);

	if(bIsNew && (uiQBytes > 0))
	{
	    struct msqid_ds stMsgStat;

		if(msgctl(m_iMsgQID, IPC_STAT, &stMsgStat) != 0)
	    {
	        BOSS_ERROR("msgctl %d Error %s", m_iMsgQKey, strerror(errno));
		    return -1;
	    }

		stMsgStat.msg_qbytes = uiQBytes;

		if(msgctl(m_iMsgQID, IPC_SET, &stMsgStat) != 0)
	    {
	        BOSS_ERROR("msgctl %d Error %s", m_iMsgQKey, strerror(errno));
		    return -1;
	    }

		BOSS_INFO("设置消息队列的最大字节数为 %u", uiQBytes);
	}

	Dump();	

	return 0;
}

int MsgQueueMgr::Put(const MsgBuf  *pcMsgBuf)
{
    assert(pcMsgBuf);

	size_t msgsz = sizeof(*pcMsgBuf) + pcMsgBuf->GetPayloadLen();

	if(msgsnd(m_iMsgQID, (void*)pcMsgBuf, msgsz, IPC_NOWAIT) == -1)
	{
	    if(EAGAIN == errno)
	    {
	        BOSS_ERROR("管道已满[%d]", m_iMsgQKey);
			return -1;
	    }
		else
		{
	        BOSS_ERROR("msgsnd[%d] Error: %s", m_iMsgQKey, strerror(errno));
		    return -1;
		}
	}

	return 0;
}

int MsgQueueMgr::Get(MsgBuf  *pcMsgBuf, int iType)
{
    assert(pcMsgBuf);

	int iReadLen = msgrcv(m_iMsgQID, (void*)pcMsgBuf, sizeof(*pcMsgBuf) + pcMsgBuf->GetPayloadLen(),
		              iType, IPC_NOWAIT);
    
	if(-1 == iReadLen)
	{
	    if(ENOMSG == errno)
			return -1;
		else
		{
	        BOSS_ERROR("msgrcv[%d - type %d] Error: %s", m_iMsgQKey, iType, strerror(errno));
		    return -1;
		}
	}

	if((u_int32_t)iReadLen < sizeof(*pcMsgBuf))
	{
	    BOSS_ERROR("readlen %d < %u", iReadLen, sizeof(*pcMsgBuf));
		return -1;
	}
	
    pcMsgBuf->SetPayloadLen(iReadLen - sizeof(*pcMsgBuf));
	
	return 0;
}

Task::Task(const string &sTaskName, MsgQueueMgr *pcIn, MsgQueueMgr *pcOut)
{
    assert((sTaskName.length() > 0) && pcIn);

	m_pcIn = pcIn;
	m_pcOut = pcOut;

	m_uiTaskIdx = 1;

	m_sTaskName = sTaskName;
}

Task::~Task()
{
}

int Task::Start(u_int32_t uiProcessNum, u_int32_t uiRate, bool bByTaskIdx)
{
	signal(SIGCHLD, SIG_IGN);
	
    if(0 == uiProcessNum)
		uiProcessNum = 1;

	if(uiRate > 0)
		m_cFMgr.Open(uiRate);

	m_uiRate = uiRate;
	
	for(unsigned int i = 1; i <= uiProcessNum; i++)
	{
	    int iPid = fork();
		if(0 == iPid)
		{
		    InitDaemon();
			
		    DoStart(i, bByTaskIdx);
			
		    exit(0);
		}
		else if(iPid < 0)
		{
		    BOSS_ERROR("fork %d error: %s", i, strerror(errno));
		}
	}

	return 0;
}

u_int32_t Task::GetTaskIdx()
{
    return m_uiTaskIdx;
}

bool Task::m_bRunning = true;

static void SigHandle(int iSigNo)
{
    BOSS_INFO("接收到程序终止信号!");
	
    Task::m_bRunning = false;
}

int Task::Init()
{
    return 0;
}

int Task::Handle(u_int32_t uiType, u_int32_t uiCmd, 
		                char *sData, u_int32_t uiDataLen)
{
    return 0;
}

void Task::DoStart(u_int32_t uiTaskIdx, bool bByTaskIdx)
{
    m_uiTaskIdx = uiTaskIdx;

    // 0. 初始化信号
    signal(SIGTERM, boss::task::SigHandle);
	
    // 1. 初始化日志
    SetLogName(m_sTaskName.c_str());

    CCfg cCfg;
	char szErrMsg[1024];
	if(iInitLog(LOG_CFG_FILE_PATH, const_cast<char*>(cCfg.GetLogFilePath()), "main", szErrMsg) < 0)
    {
        printf("iInitLog Error:%s\n", szErrMsg);
		return;
    }
	
	// 2. 调用子类的初始化函数
	if(this->Init() != 0)
		return;
	
    // 3. 初始化接收缓冲区
    char sData[sizeof(MsgBuf) + MAX_PAYLOAD_SIZE];
	MsgBuf *pcMsgBuf = new(sData)MsgBuf;
	
	// 4. 循环处理消息管道
	int iType = 0;
	if(bByTaskIdx)
		iType = m_uiTaskIdx;
	
    while(Task::m_bRunning)
    {
        u_int32_t uiQuota = 0;
		if(m_uiRate > 0)
			uiQuota = m_cFMgr.GetQuota();

		u_int32_t uiCount = 0;
		
		while(1)
		{
		    if((uiQuota > 0) && (uiCount >= uiQuota))
				break;

			pcMsgBuf->SetPayloadLen(MAX_PAYLOAD_SIZE);
			
			if(m_pcIn->Get(pcMsgBuf, iType) != 0)
				break;			

			this->Handle(pcMsgBuf->GetType(), pcMsgBuf->GetCmd(), pcMsgBuf->GetData(), pcMsgBuf->GetPayloadLen());

			uiCount++;
		}
			
		usleep(50 * 1000);
    }	
}

int Task::Output(u_int32_t uiType, u_int32_t uiCmd, 
		                char *sData, u_int32_t uiDataLen)
{
    assert(m_pcOut && sData);

	static char sBuf[sizeof(MsgBuf) + MAX_PAYLOAD_SIZE];

    if(uiDataLen > MAX_PAYLOAD_SIZE)
    {
        BOSS_ERROR("载荷大小超过限制[%u > %u]", uiDataLen, MAX_PAYLOAD_SIZE);

		return -1;
    }

	MsgBuf *pcMsgBuf = new(sBuf)MsgBuf;

	pcMsgBuf->SetType(uiType);
	pcMsgBuf->SetCmd(uiCmd);

	memcpy(pcMsgBuf->GetData(), sData, uiDataLen);
	pcMsgBuf->SetPayloadLen(uiDataLen);
	
    return m_pcOut->Put(pcMsgBuf);	
}

};
};
