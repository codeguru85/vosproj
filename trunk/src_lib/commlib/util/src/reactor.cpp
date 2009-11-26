#include "reactor.h"

boss::SimpTimerScheduler::SimpTimerScheduler()
{
}

int boss::SimpTimerScheduler::GetTimerSeq()
{
    static int iSeq = 0;

	return (++iSeq);
}

int boss::SimpTimerScheduler::RegisterTimerHandler(u_int32_t uiSeconds, u_int32_t uiMilliSeconds, TimerHandler *pcTimerHandler)
{
    assert(pcTimerHandler && (uiSeconds > 0 || uiMilliSeconds > 0));
	
    if(m_mapHandlerToSeq.size() >= MAX_TIMER_HANDLER)
    {
        BOSS_ERROR("已经达到支持的TimerHandler个数上限[%d]", MAX_TIMER_HANDLER);
		return -1;
    }

	if(m_mapHandlerToSeq.find(pcTimerHandler) != m_mapHandlerToSeq.end())
	{
	    BOSS_ERROR("当前添加的TimerHandler已经使用[%p]", pcTimerHandler);
		return -1;
	}

	int iSeq = GetTimerSeq();

    m_mapHandlerToSeq[pcTimerHandler] = iSeq;
	m_mapSeqToHandler[iSeq] = pcTimerHandler;

	ExpireTime cExpireTime;
	cExpireTime.m_uiSeconds = uiSeconds;
	cExpireTime.m_uiMilliSeconds = uiMilliSeconds;

	struct timeval stCurrentTime;
	gettimeofday(&stCurrentTime, NULL);

	cExpireTime.m_uiExpireTime = (u_int64_t)stCurrentTime.tv_sec * 1000
		                         + (u_int64_t)stCurrentTime.tv_usec / 1000
		                         + (u_int64_t)uiSeconds * 1000
		                         + (u_int64_t)uiMilliSeconds;

	cExpireTime.m_iTimerSeq = iSeq;

	m_setExpireTime.insert(cExpireTime);

	return 0;
}


int boss::SimpTimerScheduler::UnRegisterTimerHandler(TimerHandler *pcTimerHandler)
{
    assert(pcTimerHandler);

	if(m_mapHandlerToSeq.find(pcTimerHandler) == m_mapHandlerToSeq.end())
	{
		return 0;
	}

	int iSeq = m_mapHandlerToSeq[pcTimerHandler];

	m_mapHandlerToSeq.erase(pcTimerHandler);
	m_mapSeqToHandler.erase(iSeq);

	for(set<ExpireTime, LessExpireTime>::iterator i = m_setExpireTime.begin();
	                              i != m_setExpireTime.end();
								  i++)
	{
	    if(i->m_iTimerSeq == iSeq)
	    {
			m_setExpireTime.erase(i);
			break;
	    }
	}
	
	return 0;
}

u_int32_t boss::SimpTimerScheduler::GetMinExpireTime()
{
    if(m_setExpireTime.size() == 0)
		return 0;
	
    struct timeval stCurrentTime;
	gettimeofday(&stCurrentTime, NULL);
    u_int64_t uiCurrentMilliSeconds =
		        (u_int64_t)stCurrentTime.tv_sec * 1000
		                         + (u_int64_t)stCurrentTime.tv_usec / 1000;
		
	set<ExpireTime, LessExpireTime>::iterator i = m_setExpireTime.begin();

    if(i->m_uiExpireTime <= uiCurrentMilliSeconds)
		return 10;
	else
	    return (u_int32_t)(i->m_uiExpireTime - uiCurrentMilliSeconds);
}

int boss::SimpTimerScheduler::RunTimerLoop()
{
    if(m_mapHandlerToSeq.empty())
		return 0;
	
	set<ExpireTime, LessExpireTime> setExpiretime = m_setExpireTime;

	for(set<ExpireTime, LessExpireTime>::iterator i = setExpiretime.begin();
	                              i != setExpiretime.end();
								  i++)
	{
	    struct timeval stCurrentTime;

	    gettimeofday(&stCurrentTime, NULL);
        u_int64_t uiCurrentMilliSeconds =
		        (u_int64_t)stCurrentTime.tv_sec * 1000
		                         + (u_int64_t)stCurrentTime.tv_usec / 1000;
	
	    if(i->m_uiExpireTime > uiCurrentMilliSeconds)
	    {			
			break;
	    }

	    if(m_mapSeqToHandler.find(i->m_iTimerSeq) == m_mapSeqToHandler.end())
			continue;

		TimerHandler *pcTimerHandler = m_mapSeqToHandler[i->m_iTimerSeq];

	    UnRegisterTimerHandler(pcTimerHandler);
		if(pcTimerHandler->HandleTimeout(uiCurrentMilliSeconds / 1000, uiCurrentMilliSeconds % 1000) == 0)
		{
		    RegisterTimerHandler(i->m_uiSeconds, i->m_uiMilliSeconds, pcTimerHandler);
		}
	}

	return 0;
}

bool boss::Reactor::ms_bStop = false;

static void SignalHandler(int)
{
    BOSS_INFO("接收到中断信号,进程退出执行");

    boss::Reactor::Stop();
}

boss::Reactor::Reactor(u_int32_t uiMaxHandlersNum)
{
    m_uiMaxHandlersNum = uiMaxHandlersNum;
	
	signal(SIGTERM, SignalHandler);
    signal(SIGKILL, SignalHandler);
    signal(SIGINT, SignalHandler);
}

void boss::Reactor::RegisterHandler(EventHandler *pcEventHandler, int iMask)
{
    assert(pcEventHandler);

    m_mEventHandlers[pcEventHandler->GetHandler()] = pcEventHandler;
	m_mMasks[pcEventHandler->GetHandler()] = iMask;
	m_setHandlers.insert(pcEventHandler->GetHandler());
}

void boss::Reactor::UnRegisterHandler(EventHandler *pcEventHandler)
{
    if(pcEventHandler)
    {
        m_setHandlers.erase(pcEventHandler->GetHandler());
        m_mEventHandlers[pcEventHandler->GetHandler()] = NULL;
		m_mMasks[pcEventHandler->GetHandler()] = 0;
    }
}

u_int32_t boss::Reactor::GetSleepInterval()
{
    u_int32_t uiMilliSeconds = GetMinExpireTime();
	if(0 == uiMilliSeconds)
		uiMilliSeconds = 3 * 1000;
	if(uiMilliSeconds > 3 * 1000)
		uiMilliSeconds = 3 * 1000;

	return uiMilliSeconds;
}

int boss::Reactor::DoHandleReadyEvents(list<int> &listReadyFDs)
{
	for(list<int>::iterator i = listReadyFDs.begin(); i != listReadyFDs.end(); i++)
	{
	    int iHandler = *i;


        EventHandler *pEventHandler = m_mEventHandlers[iHandler];
			
        if(pEventHandler &&
			           (MASK_ACCEPT == m_mMasks[iHandler] || MASK_INPUT == m_mMasks[iHandler])
				                                               )
        {
            int iRst = 0;
            if(MASK_ACCEPT == m_mMasks[iHandler])
            {
                if((0 == m_uiMaxHandlersNum) || (m_setHandlers.size() < m_uiMaxHandlersNum))
                          iRst = pEventHandler->HandleInput(iHandler);
            }
			else
			{
				iRst = pEventHandler->HandleInput(iHandler);
			}

			if(0 != iRst)
			{
				UnRegisterHandler(pEventHandler);
						
			    pEventHandler->HandleClose();
   			}
        }
		else
		{
		    UnRegisterHandler(pEventHandler);
		}
	}

	return 0;
}

void boss::Reactor::CleanAndClear()
{
	for(set<int>::iterator i = m_setHandlers.begin(); i != m_setHandlers.end(); i++)
	{
	    int iHandler = *i;

	    if(m_mEventHandlers[iHandler])
	    {
	        m_mEventHandlers[iHandler]->HandleClose();
			m_mEventHandlers[iHandler] = NULL;
	    }

		m_mMasks[iHandler] = 0;
	}

	m_setHandlers.clear();
}

int boss::Reactor::DoWaitForEvents(u_int32_t uiSleepInterval, list<int> &listReadyFDs)
{
    listReadyFDs.clear();

    usleep(uiSleepInterval * 1000);
	
    return 0;
}

void boss::Reactor::RunEventLoop()
{

    BOSS_INFO("Reactor ready......");

    while(!Reactor::ms_bStop)
    {

		u_int32_t uiSleepInterval = GetSleepInterval();

        list<int> listReadyFDs;
			
		if(DoWaitForEvents(uiSleepInterval, listReadyFDs) != 0)
			break;

		RunTimerLoop();

		if(listReadyFDs.empty())
			continue;

		if(DoHandleReadyEvents(listReadyFDs) != 0)
			break;
    }

    CleanAndClear();
}

boss::SelectReactor::SelectReactor(u_int32_t uiMaxHandlersNum):Reactor(uiMaxHandlersNum)
{
}


void boss::SelectReactor::RegisterHandler(EventHandler *pcEventHandler, int iMask)
{
    assert(pcEventHandler);

    Reactor::RegisterHandler(pcEventHandler, iMask);
	
	m_setHandlers.insert(pcEventHandler->GetHandler());
}

void boss::SelectReactor::UnRegisterHandler(EventHandler *pcEventHandler)
{
    if(pcEventHandler)
    {
        Reactor::UnRegisterHandler(pcEventHandler);
		
        m_setHandlers.erase(pcEventHandler->GetHandler());
    }
}

int boss::SelectReactor::DoWaitForEvents(u_int32_t uiSleepInterval, list<int> &listReadyFDs)
{
    struct timeval stTimeOut;

    fd_set stReadSet;
	int    iMaxHandler = -1;
    int    iRet;

		
    stTimeOut.tv_sec = uiSleepInterval / 1000;
    stTimeOut.tv_usec = (uiSleepInterval % 1000) * 1000;

    FD_ZERO(&stReadSet);

    iMaxHandler = -1;
	for(set<int>::iterator i = m_setHandlers.begin(); i != m_setHandlers.end(); i++)
	{
	    FD_SET(*i, &stReadSet);
		if(*i > iMaxHandler)
			iMaxHandler = *i;
	}

	if(-1 == iMaxHandler)
	    iRet = select(0, NULL, NULL, NULL, &stTimeOut);
	else
		iRet = select(iMaxHandler + 1, &stReadSet, NULL, NULL, &stTimeOut);

	listReadyFDs.clear();
	
	if(0 == iRet)
		return 0;

    if(iRet < 0)
    {
        if(EINTR == errno)
	    {
	        BOSS_INFO("Encounter Signal");
			return 0;
	    }

        BOSS_ERROR("Select Error: %s", strerror(errno));
        return -1;
    }

	for(set<int>::iterator i = m_setHandlers.begin(); i != m_setHandlers.end(); i++)
	{
	    int iHandler = *i;

	    if(FD_ISSET(iHandler, &stReadSet))
	    {
	        listReadyFDs.push_back(iHandler);
	    }
    }

    return 0;
}

int boss::EpollReactor::AddToEPoll(int iEpollFd, int iSocketFd, unsigned int iEvents)
{
    struct epoll_event stEvent;

    stEvent.events  = iEvents;
    stEvent.data.fd = iSocketFd;

	if (epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iSocketFd, &stEvent) != 0)
    {
        BOSS_ERROR("epoll_ctl Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int boss::EpollReactor::ModInEPoll(int iEpollFd, int iSocketFd, unsigned int iEvents)
{
    struct epoll_event stEvent;

    stEvent.events  = iEvents;
    stEvent.data.fd = iSocketFd;

	if (epoll_ctl(iEpollFd, EPOLL_CTL_MOD, iSocketFd, &stEvent) != 0)
    {
        BOSS_ERROR("epoll_ctl Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int boss::EpollReactor::DelFromEPoll(int iEpollFd, int iSocketFd)
{
    struct epoll_event stEvent;

    stEvent.events  = 0;
    stEvent.data.fd = iSocketFd;

	if (epoll_ctl(iEpollFd, EPOLL_CTL_DEL, iSocketFd, &stEvent) != 0)
    {
        BOSS_ERROR("epoll_ctl Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

boss::EpollReactor::EpollReactor(u_int32_t uiMaxHandlersNum):Reactor(uiMaxHandlersNum)
{
    if(0 == m_uiMaxHandlersNum)
		m_uiMaxHandlersNum = 65535;
	
    m_pstEpollEvent = (struct epoll_event*)malloc(sizeof(struct epoll_event) * m_uiMaxHandlersNum);

	m_iEpollFd = epoll_create(m_uiMaxHandlersNum);
	if(m_iEpollFd < 0)
	{
	    BOSS_ERROR("epoll_create Error:%s\n", strerror(errno));
	}
}

boss::EpollReactor:: ~EpollReactor()
{
    if(NULL != m_pstEpollEvent)
    {
        free(m_pstEpollEvent);
		m_pstEpollEvent = NULL;
    }

	if(m_iEpollFd >= 0)
	{
	    close(m_iEpollFd);
		m_iEpollFd = -1;
	}
}
void boss::EpollReactor::RegisterHandler(EventHandler *pcEventHandler, int iMask)
{
    assert(pcEventHandler);

    Reactor::RegisterHandler(pcEventHandler, iMask);

	int iEvents = 0;
	if(0 != iMask)
		iEvents |= EPOLLIN;

	AddToEPoll(m_iEpollFd, pcEventHandler->GetHandler(), iEvents);
}

void boss::EpollReactor::UnRegisterHandler(EventHandler *pcEventHandler)
{
    if(pcEventHandler)
    {
        Reactor::UnRegisterHandler(pcEventHandler);

		DelFromEPoll(m_iEpollFd, pcEventHandler->GetHandler());
    }
}

int boss::EpollReactor::DoWaitForEvents(u_int32_t uiSleepInterval, list<int> &listReadyFDs)
{
	if(m_iEpollFd < 0)
    {
        BOSS_ERROR("epoll未初始化");
		return -1;
    }

    int    iReady;
		
    iReady = epoll_wait(m_iEpollFd, m_pstEpollEvent, m_uiMaxHandlersNum, uiSleepInterval);

    listReadyFDs.clear();
	
	if(0 == iReady)
		return 0;

    if(iReady < 0)
    {
        if(EINTR == errno)
        {
	        BOSS_INFO("Encounter Signal");
			return 0;
	    }

        BOSS_ERROR("epoll_wait Error: %s", strerror(errno));
        return -1;
    }

	for(int i = 0; i < iReady; i++)
	{
		int iCurrentHandler = m_pstEpollEvent[i].data.fd;

		listReadyFDs.push_back(iCurrentHandler);
    }

	return 0;
}

boss::UdpHandlerBase::UdpHandlerBase(Reactor *pcReactor):EventHandler(pcReactor)
{
    BOSS_INFO("UdpHandlerBase Constructor");
}

boss::UdpHandlerBase::~UdpHandlerBase()
{
    BOSS_INFO("UdpHandlerBase Destructor");
}

int boss::UdpHandlerBase::HandleInput(int iHandler)
{
    return 0;
}
	
int boss::UdpHandlerBase::Open(u_int16_t uiPort, const string &sAddr)
{
    struct  sockaddr_in stAddr;

    memset(&stAddr, 0, sizeof(stAddr));

    stAddr.sin_family = AF_INET;
    stAddr.sin_port   = htons((unsigned short)uiPort);
    if("" == sAddr)
    {
        stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        stAddr.sin_addr.s_addr = inet_addr(sAddr.c_str());
    }

    int iSocketFD;
    if((iSocketFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        BOSS_ERROR("socket Error: %s", strerror(errno));
        return -1;
    }

	u_int32_t uiRecvBufSize = 40 * 100000;
	uint32_t  dwLen = sizeof(uiRecvBufSize);
	if (setsockopt(iSocketFD, SOL_SOCKET, SO_RCVBUF, (int *)&uiRecvBufSize, dwLen) == -1)
	{
		BOSS_ERROR("setsockopt Error: %s", strerror(errno));
		close(iSocketFD);
		return -1;
	}

	int iFlags;

	iFlags = fcntl(iSocketFD, F_GETFL, 0);
	iFlags |= O_NONBLOCK;
	iFlags |= O_NDELAY;
	fcntl(iSocketFD, F_SETFL, iFlags);

    if(bind(iSocketFD, (struct sockaddr*)&stAddr, sizeof(stAddr)) < 0)
    {
        BOSS_ERROR("bind Error: %s", strerror(errno));

        close(iSocketFD);
        return -1;
    }

    SetHandler(iSocketFD);

    GetReactor()->RegisterHandler(this, MASK_INPUT);

    return 0;
}

int boss::UdpHandlerBase::HandleClose()
{
    if(GetHandler() != -1)
    {
        close(GetHandler());
        SetHandler(-1);
	}

	return 0;
}

boss::Socket::Socket()
:  _socket_fd(INVALID_SOCKET)
{
    _error_text[0] = '\0';

    reset_buf();
}

boss::Socket::~Socket()
{
    close();
}

int
boss::Socket::create(int socket_type)
{
    _socket_fd =::socket(AF_INET, socket_type, 0);
    if (_socket_fd < 0) {
        _socket_fd = INVALID_SOCKET;
        snprintf(_error_text,sizeof(_error_text)-1, "create: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return 0;
}

void
boss::Socket::get_peer_name(string & peer_address, uint16_t & peer_port)
{
    struct sockaddr name;
    struct sockaddr_in *p = (struct sockaddr_in *) &name;
    socklen_t namelen = sizeof(name);

    bzero(&name, sizeof(struct sockaddr));

    get_peer_name(name, namelen);

    char addr[INET_ADDRSTRLEN];
    peer_address = inet_ntop(AF_INET, &p->sin_addr, addr, sizeof(addr));
    peer_port = ntohs(p->sin_port);

    return;
}

void
boss::Socket::get_peer_name(struct sockaddr &name, socklen_t & namelen)
{
    if (::getpeername(_socket_fd, &name, &namelen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "get_peer_name: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

void
boss::Socket::get_sock_name(string & socket_address, uint16_t & socket_port)
{
    struct sockaddr name;
    struct sockaddr_in *p = (struct sockaddr_in *) &name;
    socklen_t namelen = sizeof(name);

    bzero(&name, sizeof(struct sockaddr));
    get_sock_name(name, namelen);

    char addr[INET_ADDRSTRLEN];
    socket_address = inet_ntop(AF_INET, &p->sin_addr, addr, sizeof(addr));
    socket_port = ntohs(p->sin_port);

    return;
}

void
boss::Socket::get_sock_name(struct sockaddr &name, socklen_t & namelen)
{
    if (::getsockname(_socket_fd, &name, &namelen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "get_sock_name: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

void
boss::Socket::set_sock_opt(int optname, const void *optval,
                     socklen_t optlen, int level /*= SOL_SOCKET*/ )
{
    if (setsockopt(_socket_fd, level, optname, optval, optlen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "set_sock_opt: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

void
boss::Socket::get_sock_opt(int optname, void *optval,
                     socklen_t * optlen, int level /*= SOL_SOCKET*/ )
{
    if (getsockopt(_socket_fd, level, optname, optval, optlen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "get_sock_opt: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

int
boss::Socket::accept(Socket & client_socket,
               struct sockaddr *addr /*= NULL*/ ,
               socklen_t * addrlen /*= NULL*/ )
{
    int client_fd;

    assert(client_socket._socket_fd == INVALID_SOCKET);

    if (addr != NULL) {
        bzero(addr, sizeof(struct sockaddr));
    }
    while ((client_fd =::accept(_socket_fd, addr, addrlen)) < 0
           && errno == EINTR);

    if (client_fd < 0) {
        client_socket._socket_fd = INVALID_SOCKET;
        snprintf(_error_text,sizeof(_error_text)-1, "accept: %s", strerror(errno));

        if (errno == EWOULDBLOCK || errno == ECONNABORTED
            || errno == EPROTO || errno == EINTR) {
            return errno;
        }
        else {
            throw SocketException(_error_text);
        }
    }

    client_socket._socket_fd = client_fd;
    client_socket._error_text[0] = '\0';

    return 0;
}

void
boss::Socket::get_in_addr(const char * address,
		struct in_addr& sinaddr,
		const char * func_name)
{
    	int ret = inet_pton(AF_INET, address, &sinaddr);
    	if (ret < 0){
	    snprintf(_error_text,sizeof(_error_text)-1, "%s: inet_pton(%s): %s",
                    func_name, address, strerror(errno));
            throw SocketException(_error_text);
    	}
        else if (ret == 0) {
        	struct hostent hostent;
        	struct hostent *hostent_ptr;
        	char buf[2048];
        	int herror;
        	gethostbyname_r(address,
        		&hostent,
        		buf,
        		sizeof(buf),
        		&hostent_ptr,
        		&herror);
	        if (hostent_ptr == NULL) {
	            snprintf(_error_text,sizeof(_error_text)-1, "%s: gethostbyname_r(%s): %s",
	                    func_name, address, hstrerror(herror));
	            throw SocketException(_error_text);
	        }
	        else {
	            sinaddr = *(struct in_addr *) hostent_ptr->h_addr;
	        }
        }

        return;
}

void
boss::Socket::bind(uint16_t port, const char *server_address /*= NULL*/ )
{
    struct sockaddr bind_addr;
    socklen_t addrlen = sizeof(bind_addr);
    struct sockaddr_in *p = (struct sockaddr_in *) &bind_addr;

    bzero(&bind_addr, sizeof(struct sockaddr));

    p->sin_family = AF_INET;
    p->sin_port = htons(port);
    if (server_address == NULL) {
        p->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else {
    	get_in_addr(server_address, p->sin_addr, "bind");
    }

    bind(bind_addr, addrlen);
}

void
boss::Socket::bind(const struct sockaddr &bind_addr, socklen_t addrlen)
{
    if (::bind(_socket_fd, &bind_addr, addrlen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "bind: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

void
boss::Socket::close(void)
{
    if (_socket_fd != INVALID_SOCKET) {
        ::close(_socket_fd);
        _socket_fd = INVALID_SOCKET;
    }
}

void
boss::Socket::connect(const char *address, uint16_t port)
{
    if (address == NULL) {
        snprintf(_error_text,sizeof(_error_text)-1, "connect: address == NULL");
        throw SocketException(_error_text);
    }

    struct sockaddr serveraddr;
    struct sockaddr_in *p = (struct sockaddr_in *) &serveraddr;

    bzero(&serveraddr, sizeof(serveraddr));

    p->sin_family = AF_INET;
    get_in_addr(address, p->sin_addr, "connect");
    p->sin_port = htons(port);

    connect(serveraddr, sizeof(serveraddr));
}

void
boss::Socket::connect(const struct sockaddr &serv_addr, socklen_t addrlen)
{
    if (::connect(_socket_fd, &serv_addr, addrlen) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "connect: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return;
}

void
boss::Socket::listen(int connection_baklog)
{
    if (::listen(_socket_fd, connection_baklog) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "listen: %s", strerror(errno));
        throw SocketException(_error_text);
    }
}

int
boss::Socket::receive(void *buf, size_t len, int flag)
{
    int bytes;

    if ((bytes =::recv(_socket_fd, buf, len, flag)) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "receive: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return bytes;
}

void boss::Socket::wait(u_int32_t uiTimeout)
{
    if(!socket_is_ok())
    {
        snprintf(_error_text,sizeof(_error_text), "socket is not ready");
        throw SocketException(_error_text);
    }

    fd_set  stFDSet;
    FD_ZERO(&stFDSet);
    FD_SET((unsigned int)_socket_fd, &stFDSet);
	
    struct timeval stTimeout;

    stTimeout.tv_sec = uiTimeout;
    stTimeout.tv_usec = 0;
    
    int iRet = select(_socket_fd + 1, &stFDSet, NULL, NULL, &stTimeout);
	if(iRet > 0)
		return;
	else if(0 == iRet)
	{
	    snprintf(_error_text,sizeof(_error_text), "select timeout[%u]", uiTimeout);
        throw SocketException(_error_text);
	}
	else
	{
	    snprintf(_error_text,sizeof(_error_text), "select error: %s", strerror(errno));
        throw SocketException(_error_text);
	}
}

int
boss::Socket::receive_from(void *buf, size_t len,
                     string & from_address, uint16_t & from_port, int flags)
{
    struct sockaddr from;
    socklen_t fromlen = sizeof(struct sockaddr);
    struct sockaddr_in *p = (struct sockaddr_in *) &from;

    bzero(&from, sizeof(struct sockaddr));

    int bytes = receive_from(buf, len, from, fromlen, flags);
    if (bytes < 0){
        throw SocketException(_error_text);
    }

    char addr[INET_ADDRSTRLEN];
    from_address = inet_ntop(AF_INET, &p->sin_addr, addr, sizeof(addr));
    from_port = ntohs(p->sin_port);

    return bytes;
}

int
boss::Socket::receive_from(void *buf, size_t len,
                     struct sockaddr &from, socklen_t & fromlen, int flags)
{
    int bytes =::recvfrom(_socket_fd, buf, len, flags, &from, &fromlen);

    if (bytes < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "receive_from: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return bytes;
}

int
boss::Socket::send(const void *buf, size_t len, int flag)
{
    int bytes;

    if ((bytes =::send(_socket_fd, buf, len, flag)) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "send: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return bytes;
}

int
boss::Socket::send_to(const void *msg, size_t len,
                uint16_t port, const char *host_address /*= NULL*/ ,
                int flags /* = 0 */ )
{
    struct sockaddr toaddr;
    struct sockaddr_in *p = (struct sockaddr_in *) &toaddr;

    bzero(&toaddr, sizeof(struct sockaddr));

    p->sin_family = AF_INET;

    if (host_address == NULL) {
        p->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }
    else {
        get_in_addr(host_address, p->sin_addr, "send_to");
    }

    p->sin_port = htons(port);

    return send_to(msg, len, toaddr, sizeof(toaddr), flags);
}

int
boss::Socket::send_to(const void *msg, size_t len,
                const struct sockaddr &to, socklen_t tolen,
                int flags /* = 0 */ )
{
    int bytes;

    if ((bytes =::sendto(_socket_fd, msg, len, flags, &to, tolen)) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "send_to: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    return bytes;
}

void
boss::Socket::shutdown(int how)
{
    if (::shutdown(_socket_fd, how) < 0) {
        snprintf(_error_text,sizeof(_error_text)-1, "send_to: %s", strerror(errno));
        throw SocketException(_error_text);
    }
}

void
boss::Socket::set_nonblock()
{
    int val;

    if ((val = fcntl(_socket_fd, F_GETFL, 0)) == -1) {
        snprintf(_error_text,sizeof(_error_text)-1, "set_nonblock: %s", strerror(errno));
        throw SocketException(_error_text);
    }

    if (fcntl(_socket_fd, F_SETFL, val | O_NONBLOCK) == -1) {
        snprintf(_error_text,sizeof(_error_text)-1, "set_nonblock: %s", strerror(errno));
        throw SocketException(_error_text);
    }
}

void
boss::Socket::reset_buf()
{
    _read_count = 0;
    _read_ptr = NULL;
    _read_buf[0] = '\0';
}

ssize_t boss::Socket::buf_read(char &ch)
{
    if (_read_count <= 0) {
      again:
        _read_count =::read(_socket_fd, _read_buf, sizeof(_read_buf));
        if (_read_count < 0) {
            if (errno == EINTR)
                goto again;
            else
                return -1;
        }
        else if (_read_count == 0) {
            return 0;
        }

        _read_ptr = _read_buf;
    }

    _read_count--;
    ch = *_read_ptr++;
    return 1;
}

int
boss::Socket::gets(char *str, size_t maxlen)
{
    size_t len;
    int rc;
    char ch;
    char *ptr;

    ptr = str;
    for (len = 1; len < maxlen; ++len) {
        if ((rc = buf_read(ch)) == 1) {
            if (ch == '\n')
                break;
            *ptr++ = ch;
        }
        else if (rc == 0) {
            return 0;
        }
        else {
            snprintf(_error_text,sizeof(_error_text)-1, "gets: %s", strerror(errno));
            throw SocketException(_error_text);
        }
    }
    *ptr = '\0';
    if (ptr > str)
        *(--ptr) = '\0';        // remove '\n'
    return len;
}


string InetNtoa(u_int32_t uiIP)
{
    struct in_addr in;
	in.s_addr = uiIP;

	return inet_ntoa(in);
}

u_int32_t InetAton(const string &sIP)
{
	return inet_addr(sIP.c_str());
}
