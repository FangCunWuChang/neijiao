// TCPSocket.cpp: implementation of the CTCPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCPSocket.h"
#include "Singleton.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void DoEvent();
extern int g_nExt;
void LogFile(CString log);
void LogFile(const char *fmt, ...);

CTCPSocket::CTCPSocket(int nType)
{
	m_nType=nType;
	m_sSocket=NULL;
	m_bAvailable=Initwinsock();
	m_bCreated=FALSE;
	m_bAuto=FALSE;
	m_dwUserData=0;
	error=0;

	m_nPort=-1;
	m_hServerThread=NULL;
	for(int i=0;i<MAX_CONNECTION;i++)
	{
		m_hServerDataThread[i]=NULL;
		m_sServer[i]=NULL;
		ZeroMemory(m_cIp[i],16);
		m_bConnected[i]=FALSE;
	}
	m_nConnections=0;
	m_nCurrent=0;
	m_lpServerStatusProc=NULL;
	m_lpServerDataArriveProc=NULL;

	m_hClientThread=NULL;
	m_lpClientDataArriveProc=NULL;
	m_lpClientStatusProc=NULL;

	ZeroMemory(m_nLnk, sizeof(m_nLnk));// = m_nLnk[1] = 0;
}

CTCPSocket::~CTCPSocket()
{
	Close();
}

int CTCPSocket::GetError()
{
	return error;
}

SOCKET CTCPSocket::GetSocket()
{
	return m_sSocket;
}

int CTCPSocket::GetType()
{
	return m_nType;
}

BOOL CTCPSocket::IsConnected(SOCKET s)
{
	int nRet=0;
	struct fd_set Fd_Recv;
	struct timeval Time_Recv;

	memset(&Fd_Recv,0,sizeof(struct fd_set));
	FD_CLR(s,&Fd_Recv);
	FD_SET(s,&Fd_Recv);
	Time_Recv.tv_sec=0;
	Time_Recv.tv_usec=0;

	nRet=select(s,&Fd_Recv,NULL,NULL,&Time_Recv);

	return (nRet==0);
}

BOOL CTCPSocket::CreateServer(int nPort,int backlog)
{
	m_bAvailable=Initwinsock();
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType!=TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(m_bCreated)
	{
		return FALSE;
	}

	struct sockaddr_in local;

	m_sSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	if(m_sSocket==SOCKET_ERROR)
	{
		error=WSAGetLastError();
		MessageBox(NULL,"服务器 错误1！","",MB_ICONERROR);
		return FALSE;
	}
	
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_family=AF_INET;
	local.sin_port=htons(nPort);

	if(bind(m_sSocket,(struct sockaddr*)&local,sizeof(local))==SOCKET_ERROR)
	{
		error=WSAGetLastError();
		closesocket(m_sSocket);
		MessageBox(NULL, "服务器 错误2！", "", MB_ICONERROR);
		return FALSE;
	}

	if(listen(m_sSocket,backlog)!=0)
	{
		error=WSAGetLastError();
		closesocket(m_sSocket);
		MessageBox(NULL, "服务器 错误3！", "", MB_ICONERROR);
		return FALSE;
	}
	m_nPort=nPort;
	m_bCreated=TRUE;
	return TRUE;
}

BOOL CTCPSocket::StartServer(LPStatusProc proc1,LPDataArriveProc proc2,DWORD userdata)
{
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType!=TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(!m_bCreated)
	{
		return FALSE;
	}
	if(m_bAuto)
	{
		return FALSE;
	}

	m_lpServerStatusProc=proc1;
	m_lpServerDataArriveProc=proc2;
	m_dwUserData=userdata;

	DWORD dwThreadId;

	m_bAuto = TRUE;
	m_hServerThread = CreateThread(NULL,0,ServerThread,this,0,&dwThreadId);

	if(m_hServerThread==NULL)
	{
		m_bAuto=FALSE;
		error=WSAGetLastError();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CTCPSocket::StopServer()
{
	m_bAuto=FALSE;
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType!=TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(!m_bCreated)
	{
		return FALSE;
	}
	if(!m_bAuto)
	{
		return FALSE;
	}
	DWORD exitcode;
	//停止监听线程
	WaitForSingleObject(m_hServerThread,500);
	if(!GetExitCodeThread(m_hServerThread,&exitcode))
	{
		TerminateThread(m_hServerThread,exitcode);
	}
	CloseHandle(m_hServerThread);
	m_hServerThread=NULL;
	shutdown(m_sSocket,SD_RECEIVE);
	closesocket(m_sSocket);
	m_sSocket=NULL;

	//停止所有收发数据线程
	for(int i = 0; i < MAX_CONNECTION;i++)
	{
		if(m_bConnected[i])
		{
			m_bConnected[i]=FALSE;
			WaitForSingleObject(m_hServerDataThread[i],50);
			if(!GetExitCodeThread(m_hServerDataThread[i],&exitcode))
			{
				TerminateThread(m_hServerDataThread[i],exitcode);
			}
			shutdown(m_sServer[i],SD_RECEIVE);
			closesocket(m_sServer[i]);
			m_sServer[i]=NULL;
			CloseHandle(m_hServerDataThread[i]);
			m_hServerDataThread[i]=NULL;
		}
	}
	m_nConnections = 0;
	return TRUE;
}

SOCKET CTCPSocket::Listen(char* ClientIP)
{
	if(!m_bAvailable)
	{
		return -1;
	}
	if(m_nType!=TCP_SOCKET_SERVER)
	{
		return -1;
	}
	if(!m_bCreated)
	{
		return -1;
	}

	SOCKET sClient;
	int iAddrSize;
	struct sockaddr_in addr;

	iAddrSize=sizeof(addr);

	sClient=accept(m_sSocket,(struct sockaddr*)&addr,&iAddrSize);

	if(sClient==SOCKET_ERROR)
	{
		error=WSAGetLastError();
		closesocket(sClient);
		return SOCKET_ERROR;
	}

	if(ClientIP!=NULL)
	{
		sprintf(ClientIP,"%3d.%3d.%3d.%3d",addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4);
	}

	return sClient;
}

int CTCPSocket::ReceiveServer(int nNo,char* data,int length,int timeout)
{
	if(!m_bConnected[nNo])
	{
		return -2;
	}

	HANDLE hThread;
	DWORD dwThreadId;

	TimeOutParameter TimeOut;

	TimeOut.bExit=FALSE;
	TimeOut.bFinished=FALSE;
	TimeOut.EndTime=timeout;
	TimeOut.nNo=nNo;
	TimeOut.pbConnected=&(m_bConnected[nNo]);
	TimeOut.phDataThread=&(m_hServerDataThread[nNo]);
	TimeOut.pnConnections=&m_nConnections;
	TimeOut.s=m_sServer[nNo];

	hThread=CreateThread(NULL,0,TimeOutControl,(LPVOID)&TimeOut,0,&dwThreadId);

	if(hThread==NULL)
	{
		return -3;
	}

	int nRet=recv(m_sServer[nNo],data,length,0);
	if(nRet==SOCKET_ERROR)
	{
		error=WSAGetLastError();
	}
	TimeOut.bFinished=TRUE;

	while(!TimeOut.bExit)
	{
		Sleep(1);
	}

	return nRet;
}

int CTCPSocket::SendServer(int nNo,char *data,int length)
{
	if(!m_bConnected[nNo])
	{
		return -2;
	}
	
	int nRet=send(m_sServer[nNo],data,length,0);
	if(nRet==SOCKET_ERROR)
	{
		error=WSAGetLastError();
	}

	return nRet;
}

void CTCPSocket::Disconnect(int nNo)
{
	if(!m_bConnected[nNo])
	{
		return;
	}

	//断开服务器上第nNo个连接
	DWORD exitcode;
	m_bConnected[nNo]=FALSE;
	WaitForSingleObject(m_hServerDataThread[nNo],50);
	if(!GetExitCodeThread(m_hServerDataThread[nNo],&exitcode))
	{
		TerminateThread(m_hServerDataThread[nNo],exitcode);
	}
	shutdown(m_sServer[nNo],SD_RECEIVE);
	closesocket(m_sServer[nNo]);
	m_sServer[nNo]=NULL;
	CloseHandle(m_hServerDataThread[nNo]);
	m_hServerDataThread[nNo]=NULL;
	m_nConnections--;

}

BOOL CTCPSocket::Connect(LPCTSTR pstrHost,int nPort)
{
	m_bAvailable=Initwinsock();
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType==TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(m_bCreated)
	{
		return FALSE;
	}

	LPHOSTENT lpHost;
	struct sockaddr_in server;

	//查找主机
	lpHost=gethostbyname(pstrHost);
	if(lpHost==NULL)
	{
		return FALSE;
	}

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=*((u_long FAR*)(lpHost->h_addr));
	server.sin_port=htons(nPort);

	m_sSocket=socket(AF_INET,SOCK_STREAM,0);

	if(m_sSocket<=0)
	{
		error=WSAGetLastError();
		return FALSE;
	}
	
	if(connect(m_sSocket,(LPSOCKADDR)&server,sizeof(SOCKADDR))==SOCKET_ERROR) 
	{
		error=WSAGetLastError();
		closesocket(m_sSocket);
		m_sSocket=NULL;
		return FALSE;
	}

	m_bCreated=TRUE;

	return TRUE;
}

BOOL CTCPSocket::StartReceiving(LPStatusProc proc1,LPDataArriveProc proc2,DWORD userdata)
{
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType==TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(!m_bCreated)
	{
		return FALSE;
	}
	if(m_bAuto)
	{
		return FALSE;
	}

	//开始自动接收
	m_lpClientStatusProc=proc1;
	m_lpClientDataArriveProc=proc2;
	m_dwUserData=userdata;
	m_bAuto=TRUE;

	DWORD dwThreadId;

	m_hServerThread=CreateThread(NULL,0,ClientThread,this,0,&dwThreadId);

	if(m_hServerThread==NULL)
	{
		m_bAuto=FALSE;
		error=WSAGetLastError();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CTCPSocket::StopReceiving()
{
	if(!m_bAvailable)
	{
		return FALSE;
	}
	if(m_nType==TCP_SOCKET_SERVER)
	{
		return FALSE;
	}
	if(!m_bCreated)
	{
		return FALSE;
	}
	if(!m_bAuto)
	{
		return FALSE;
	}

	DWORD exitcode;
	m_bAuto=FALSE;
	//停止接收线程
	WaitForSingleObject(m_hClientThread,500);
	if(!GetExitCodeThread(m_hClientThread,&exitcode))
	{
		TerminateThread(m_hClientThread,exitcode);
	}
	CloseHandle(m_hClientThread);
	m_hClientThread=NULL;

	return TRUE;
}

int CTCPSocket::ReceiveClient(char* data, int length,int timeout)
{
	if(m_nType==TCP_SOCKET_SERVER)
	{
		return -3;
	}
	if(!m_bCreated)
	{
		return -2;
	}

	HANDLE hThread;
	DWORD dwThreadId;

	TimeOutParameter TimeOut;

	TimeOut.bExit=FALSE;
	TimeOut.bFinished=FALSE;
	TimeOut.EndTime=timeout;
	TimeOut.nNo=-1;
	TimeOut.pbConnected=&(m_bAuto);
	TimeOut.phDataThread=&(m_hClientThread);
	TimeOut.pnConnections=&(m_bCreated);
	TimeOut.s=m_sSocket;

	hThread=CreateThread(NULL,0,TimeOutControl,(LPVOID)&TimeOut,0,&dwThreadId);

	if(hThread==NULL)
	{
		return -3;
	}

	int nRet=recv(m_sSocket,data,length,0);
	if(nRet==SOCKET_ERROR)
	{
		error=WSAGetLastError();
	}
	TimeOut.bFinished=TRUE;

	while(!TimeOut.bExit)
	{
		Sleep(1);
	}

	return nRet;
}

int CTCPSocket::SendClient(char* data,int length)
{
	if(m_nType==TCP_SOCKET_SERVER)
	{
		return -3;
	}
	if(!m_bCreated)
	{
		return -2;
	}

	int nRet=send(m_sSocket,data,length,0);
	if(nRet==SOCKET_ERROR)
	{
		error=WSAGetLastError();
	}

	return nRet;
}

void CTCPSocket::Close()
{
	if(m_nType==TCP_SOCKET_SERVER)
	{
		StopServer();
		m_bCreated=FALSE;
	}
	else
	{
		StopReceiving();
		shutdown(m_sSocket,SD_RECEIVE);
		closesocket(m_sSocket);
		m_sSocket=NULL;
		m_bCreated=FALSE;
	}
	WSACleanup();
}

/////////////////////////////////////////////////////////////////////////////
//Protected Functions

BOOL CTCPSocket::Initwinsock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	
	wVersionRequested=MAKEWORD(2,2);
	
	if(WSAStartup(wVersionRequested,&wsaData)==0)
	{
		return TRUE;
	}
	else
	{
		WSACleanup();
		return FALSE;
	}
}

BOOL CTCPSocket::NewConnect(int nNo)
{
	//建立一个接收数据的线程
	m_bConnected[nNo] = TRUE;
	m_hServerDataThread[nNo] = CreateThread(NULL,0,DataThread,this,0,NULL);
	if(m_hServerDataThread==NULL)
	{
		m_bConnected[nNo]=FALSE;
		closesocket(m_sServer[nNo]);
		return FALSE;
	}
	m_nConnections++;
	return TRUE;
}

DWORD WINAPI CTCPSocket::DataThread(LPVOID lpParameter)
{
	CTCPSocket* m_pTCP = (CTCPSocket*)lpParameter;
	const int II = m_pTCP->m_nCurrent;
	m_pTCP->m_nLnk[II] = 1;
	int nRet;
	char buf[512];
	timeval tv = {0,5000};
	fd_set fs;
	//TRACE("机器人线程%d 已连接，连接状态：%d,上线%d个客户端", II + 1, m_pTCP->m_bConnected[II], m_pTCP->m_nConnections);
	LogFile("线程%d 已连接,上线%d个客户端", II + 1,  m_pTCP->m_nConnections);
	while(m_pTCP->m_bConnected[II])
	{
		FD_ZERO(&fs);
		FD_SET(m_pTCP->m_sServer[II],&fs);
		if(select(1,&fs,NULL,NULL,&tv)==1)
		{
			nRet = recv(m_pTCP->m_sServer[II],buf,512,0);
			if(nRet  ==  SOCKET_ERROR)
			{
				//出错断开(例如客户端执行了超时操作导致自己断开)
				m_pTCP->error=WSAGetLastError();
				closesocket(m_pTCP->m_sServer[II]);
				m_pTCP->m_bConnected[II]=FALSE;
				m_pTCP->m_nConnections--;
				TRACE("%s出错断开! 序号%d\n",m_pTCP->m_cIp[II],II);
				//回调处理
				if(m_pTCP->m_lpServerStatusProc!=NULL)
				{
					char* inf;
					inf=new char[22];
					sprintf(inf,"S%sD%3d",m_pTCP->m_cIp[II],II);
					m_pTCP->m_lpServerStatusProc(inf,22,m_pTCP->m_dwUserData);
					delete inf;
				}
				break;
			}
			if(nRet > 0)
			{
				m_pTCP->m_strSvrRev[II].Empty();
				TRACE("收到%s数据%d字节! IP线程%d\n",m_pTCP->m_cIp[II],nRet,II + 1);
				CString strCH;
				for (int i = 0; i < nRet;i++)
				{
					//if (buf[i] >= 'A' && buf[i] <= 'Z' ||
					//	buf[i] >= 'a' && buf[i] <= 'z' ||
					//	buf[i] >= '0' && buf[i] <= '9')
					{
						//TRACE("\n %c--", buf[i]);
						//if (buf[i] == 'T')
						//	continue;
						strCH.AppendChar(buf[i]);
					}
				}
				m_pTCP->m_strSvrRev[II].Format("%s", strCH);
				//TRACE("收到 %d 线程数据:%s",II + 1,strCH);
				if(m_pTCP->m_lpServerDataArriveProc!=NULL)
				{
					char* inf;
					inf=new char[nRet+21];
					sprintf(inf,"S%s%3d",m_pTCP->m_cIp[II],II);
					memcpy(inf+21,buf,nRet);
					m_pTCP->m_lpServerDataArriveProc(inf,nRet+21,m_pTCP->m_dwUserData);
					delete inf;
				}
				continue;
			}
			if(nRet == 0)
			{
				TRACE("%s正常断开! 序号%d\n",m_pTCP->m_cIp[II],II);
				if(m_pTCP->m_lpServerStatusProc!=NULL)
				{
					char* inf = new char[22];
					sprintf(inf,"S%sD%3d",m_pTCP->m_cIp[II],II);
					m_pTCP->m_lpServerStatusProc(inf,22,m_pTCP->m_dwUserData);
					delete inf;
				}
				closesocket(m_pTCP->m_sServer[II]);
				m_pTCP->m_bConnected[II] = FALSE;
				m_pTCP->m_nConnections--;
				break;
			}
		}
	}
	LogFile("线程%d 已断开,上线%d个客户端", II + 1,m_pTCP->m_nConnections);
	m_pTCP->m_nLnk[II] = 0;
	return 0;
}

DWORD WINAPI CTCPSocket::ServerThread(LPVOID lpParameter)
{
	CTCPSocket* m_pTCP = (CTCPSocket*)lpParameter;
	CSingleton* pSng = CSingleton::GetInstance();
	SOCKET sClient;
	int iAddrSize;
	struct sockaddr_in addr;
	int i;
	iAddrSize = sizeof(addr);
	LogFile("服务器创建完毕！");
	while(m_pTCP->m_bAuto)
	{
		sClient = accept(m_pTCP->m_sSocket,(struct sockaddr*)&addr,&iAddrSize);
		if(sClient == SOCKET_ERROR)
		{
			continue;
		}
		//判断是否达到最大连接数
		if(m_pTCP->m_nConnections >= MAX_CONNECTION)
		{
			closesocket(sClient);
			continue;
		}
		//没有则开始一个线程处理这个的收发
		for(i = 0;i < MAX_CONNECTION;i++)
		{
			if(!(m_pTCP->m_bConnected[i]))
			{
				break;
			}
		}
		LogFile("收到新的连接！");
		CString strIP,strKey;
		strKey.Format("IP%d",i + 1);
		CStringArray strsName;
		pSng->GetSecKeys(_T("ADDR"), strsName);         //得到所有的IP地址名称	
		int nSvr = 3;	
		for (int i = 0; i < strsName.GetCount(); i++)
		{
			strIP = pSng->GetCfgString("ADDR", strsName[i],"192.168.0.20");	
			LogFile(strIP);
			CStringArray strs;
			pSng->SplitString(strIP, ".", strs);
			if (addr.sin_addr.S_un.S_un_b.s_b4 == atoi(strs[strs.GetCount() - 1]))
			{
				nSvr = atoi(strsName[i].Right(1)) - 1;
			}
		}	
		if (nSvr == 3)
		{
			LogFile("无效IP地址：%d.%d.%d.%d", addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2, addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4);
			closesocket(sClient);
			TRACE("\n无效IP地址：%d.%d.%d.%d  ", addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2, addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4);
			continue;
		}
		sprintf(m_pTCP->m_cIp[nSvr],"%3d.%3d.%3d.%3d",
			addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4);
		TRACE("\n%s已经连接! 这是机器人%d\n",m_pTCP->m_cIp[nSvr],nSvr + 1);
		if(m_pTCP->m_lpServerStatusProc!=NULL)
		{
			char* inf = new char[22];
			sprintf(inf,"S%sC%3d",m_pTCP->m_cIp[nSvr],nSvr);
			m_pTCP->m_lpServerStatusProc(inf,22,m_pTCP->m_dwUserData);
			delete inf;
		}
		m_pTCP->m_nLnk[nSvr] = 0;                       //创建连接线程前是0
		m_pTCP->m_sServer[nSvr] = sClient;
		m_pTCP->m_nCurrent = nSvr;
		m_pTCP->NewConnect(nSvr);
		while(m_pTCP->m_nLnk[nSvr] != 1)            //这里要等 DataThread 创建完毕
		{
			DoEvent();
		}
	}
	return 0;
}


DWORD WINAPI CTCPSocket::ClientThread(LPVOID lpParameter)
{
	CTCPSocket* m_pTCP=(CTCPSocket*)lpParameter;
	CSingleton* pInfo = CSingleton::GetInstance();
	const int nID = m_pTCP->m_nID;
	int nRet;
	char buf[4096*2];

	timeval tv = {0,5000};
	fd_set fs;                                      //不断接收服务器发来数据
	while(m_pTCP->m_bAuto)
	{
		FD_ZERO(&fs);
		FD_SET(m_pTCP->m_sSocket,&fs);
		if(select(1,&fs,NULL,NULL,&tv) == 1)
		{		
			nRet = recv(m_pTCP->m_sSocket,buf,4096*2,0);
			if(nRet == SOCKET_ERROR)
			{			
				m_pTCP->error = WSAGetLastError();
				PostQuitMessage(0);                                                  //出错断开(例如服务器关闭)
				closesocket(m_pTCP->m_sSocket);
				m_pTCP->m_bAuto=FALSE;
				TRACE("客户端出错断开! %d\n",m_pTCP->error);
				if(m_pTCP->m_lpClientStatusProc!=NULL)
				{
					char* inf;
					inf=new char[20000];					
					inf[0]='C';
					inf[1]='D';
					//m_pTCP->m_lpClientStatusProc(inf,20000,m_pTCP->m_dwUserData);
					delete inf;
				}
				break;
			}
			if(nRet > 0)
			{
				CSingleton* pSng = CSingleton::GetInstance();
				//收到新的数据
				TRACE("\n客户端收到数据%d字节! ", nRet);//收到新的数据
				CString strBuf;
				for (int i = 0; i < nRet; i++)
				{
					strBuf.AppendChar(buf[i]);
				}
				pSng->_csIP.Lock();
				pSng->_strCliRobot[nID].Format("%s", strBuf);
				pSng->_csIP.Unlock();
				if (m_pTCP->m_lpClientDataArriveProc != NULL)
				{
					char* inf;
					inf = new char[nRet + 1];
					inf[0] = 'C';
					memcpy(inf + 1, buf, nRet);
					//m_pTCP->m_lpClientDataArriveProc(inf,nRet+1,m_pTCP->m_dwUserData);
					delete inf;
				}
				continue;
			}
			if(nRet == 0)
			{
				//服务器正常断开
				TRACE("客户端正常断开! \n");
				//回调处理
				if(m_pTCP->m_lpClientStatusProc!=NULL)
				{
					char* inf;
					inf=new char[22000];					
					inf[0]='C';
					inf[1]='D';				
					m_pTCP->m_lpClientStatusProc(inf,22000,m_pTCP->m_dwUserData);
					delete inf;
				}
				closesocket(m_pTCP->m_sSocket);
				m_pTCP->m_bAuto=FALSE;

				break;
			}
		}
	}
	return 0;
}

DWORD WINAPI CTCPSocket::TimeOutControl(LPVOID lpParameter)
{
	TimeOutParameter* m_pTimeOut=(TimeOutParameter*)lpParameter;

	time_t starttime,endtime;
	BOOL bTimeOut=FALSE;

	starttime=time(NULL);
	while(!bTimeOut)
	{
		if(m_pTimeOut->bFinished)
		{
			m_pTimeOut->bExit=TRUE;
			return 1;
		}
		Sleep(1);
		endtime=time(NULL);
		if((endtime-starttime)>m_pTimeOut->EndTime)
		{
			//超时
			bTimeOut=TRUE;
		}
	}

	//断开对应连接
	DWORD exitcode;
	if(m_pTimeOut->bFinished)
	{
		return 1;
	}
	if(m_pTimeOut->s!=NULL)
	{
		if(m_pTimeOut->nNo>=0)
		{
			//服务器socket
			//停止该接收线程
			*(m_pTimeOut->pbConnected)=FALSE;
			WaitForSingleObject(*(m_pTimeOut->phDataThread),50);
			if(!GetExitCodeThread(*(m_pTimeOut->phDataThread),&exitcode))
			{
				TerminateThread(*(m_pTimeOut->phDataThread),exitcode);
			}
			shutdown(m_pTimeOut->s,SD_RECEIVE);
			closesocket(m_pTimeOut->s);
			m_pTimeOut->s=NULL;
			CloseHandle(*(m_pTimeOut->phDataThread));
			*(m_pTimeOut->phDataThread)=NULL;
			*(m_pTimeOut->pnConnections)--;
			
		}
		else
		{
			//停止客户端接收线程
			if(*(m_pTimeOut->pbConnected))
			{
				*(m_pTimeOut->pbConnected)=FALSE;
				WaitForSingleObject(*(m_pTimeOut->phDataThread),50);
				if(!GetExitCodeThread(*(m_pTimeOut->phDataThread),&exitcode))
				{
					TerminateThread(*(m_pTimeOut->phDataThread),exitcode);
				}
			}
			shutdown(m_pTimeOut->s,SD_RECEIVE);
			closesocket(m_pTimeOut->s);
			m_pTimeOut->s=NULL;
			CloseHandle(*(m_pTimeOut->phDataThread));
			*(m_pTimeOut->phDataThread)=NULL;
			*(m_pTimeOut->pnConnections)=FALSE;
		}
	}

	m_pTimeOut->bExit=TRUE;
	return 0;
}
