// HSCommPort.cpp: implementation of the CHSCommPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HSCommPort.h"
#include <fstream>
using namespace std;
//#include <sys/timeb.h>
//#include <crtdbg.h>
//#include <windows.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSCommPort::CHSCommPort()
{
	m_nSendTimeout=300;//ms
	m_nRecvTimeout=300;//ms
	m_nConnectTimeout=3;//second
	m_nRise=2;			//us
	m_nFall=2;			//us
	m_nDue=6;			//us
	m_nVol=95;			//V
	m_bInit=FALSE;
}

CHSCommPort::~CHSCommPort()
{
	closesocket(m_socket);
	WSACleanup();
}

void  CHSCommPort::GetHostIP(CString& strIP)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
	char szHostName[128];
	if (gethostname(szHostName, 128) == 0)
	{
		//m_HostName.SetWindowText(szHostName);
		hostent * ent = gethostbyname(szHostName); 
		for (int i = 0; ent!= NULL && ent->h_addr_list[i]!= NULL; i++ ) 
		{ 
			LPCTSTR lpAddr = inet_ntoa(*(in_addr *)ent->h_addr_list[i]);
			strIP.Format(_T("%s"),lpAddr);
		}
	}
	WSACleanup();
}

BOOL CHSCommPort::InitSocket()
{
	WSADATA wsd;
	if(WSAStartup(MAKEWORD(2,0),&wsd))
		return FALSE;

	m_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_socket==INVALID_SOCKET)
		return FALSE;
	
	//设置发送超值
	if(::setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&m_nSendTimeout,sizeof(m_nSendTimeout))==SOCKET_ERROR)
		return FALSE;
	
	if(::setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&m_nRecvTimeout,sizeof(m_nRecvTimeout))==SOCKET_ERROR)
		return FALSE;
//	int nZero=0;
//	::setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&nZero,sizeof(int));
//	::setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char*)&nZero,sizeof(int));
	int rcvbuf;  
	int rcvbufsize=sizeof(int);
	
	if(getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&rcvbuf,&rcvbufsize)!=SOCKET_ERROR)
	{
		if(rcvbuf<65536)
			rcvbuf=65536;
		setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&rcvbuf,rcvbufsize);
	}
	
	if(getsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char*)&rcvbuf,&rcvbufsize)!=SOCKET_ERROR)
	{
		if(rcvbuf<65536)
			rcvbuf=65536;
		setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char*)&rcvbuf,rcvbufsize);
	}

	return TRUE;
}

void CHSCommPort::delayus(int us)
{
    LARGE_INTEGER litmp;
       LONGLONG QPart1,QPart2;
       double dfMinus, dfFreq, dfTim;
       QueryPerformanceFrequency(&litmp);
       dfFreq = (double)litmp.QuadPart;// 获得计数器的时钟频率
       QueryPerformanceCounter(&litmp);
       QPart1 = litmp.QuadPart;// 获得初始值
       do
       {
          QueryPerformanceCounter(&litmp);
          QPart2 = litmp.QuadPart;//获得中止值
          dfMinus = (double)(QPart2-QPart1);
          dfTim = dfMinus / dfFreq;// 获得对应的时间值，单位为秒
       }while(dfTim<0.000001*us);
}

BOOL CHSCommPort::ConnectPort(CString IPAddress,UINT PortNum)
{
	if(!InitSocket())
		return FALSE;
	//设置为非阻塞方式
	int ret;
	ULONG ul=1;
	ret=ioctlsocket(m_socket,FIONBIO,(unsigned long*)&ul);
	if(ret==SOCKET_ERROR)
		return FALSE;

	SOCKADDR_IN addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PortNum);
	addr.sin_addr.S_un.S_addr=inet_addr(IPAddress);
	
	connect(m_socket,(const struct sockaddr*)&addr,sizeof(addr));

	struct timeval timeout;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket,&r);
	timeout.tv_sec=m_nConnectTimeout;
	timeout.tv_usec=0;
	ret=select(0,0,&r,0,&timeout);
	if(ret<=0)
	{
		//MessageBox(NULL,"Connect timeout!","warning",MB_OK);
		closesocket(m_socket);
		return FALSE;//连接超时
	}

	//设置回阻塞模式
	ULONG ul1=0;
	ret=ioctlsocket(m_socket,FIONBIO,(unsigned long*)&ul1);
	if(ret==SOCKET_ERROR)
		return FALSE;
	return TRUE;
}


int CHSCommPort::SendCommonData(char* data,int nLen,char* buf)
{
	int nRet = send(m_socket,data,nLen,0);
	int iPos = 0;
	//for (;;)
	//{
	//	Sleep(50);
	//	nRet = recv(m_socket, buf + iPos, 4096, 0);
	//	if (nRet <= 0)
	//		break;
	//	iPos += nRet;
	//}
	return iPos + nRet;
}
/*
	发送最大数据1024字节
*/
int CHSCommPort::SendData(char* pSendBuffer,UINT nSizeOfBuffer)
{
	char* temp=pSendBuffer;
	int ret;
	int retval;
	char ACK[8];
		
	UINT Mo=nSizeOfBuffer/100;
	UINT Re=nSizeOfBuffer%100;
////////////////////////////////////////////////////////////////////////////////////////////////////
	retval=StartWriteRam();						//发送"写RAM命令"
	while(retval)
	{
		StopWriteRam();
		retval = StartWriteRam();
	}
////////////////////////////////////////////////////////////////////////////////////////////////////
	if(Mo>0)
	{
		for(UINT i=0;i<Mo;i++)
		{

			memset(ACK,0x00,8);
			ret = send(m_socket,temp,100,0);		//发送100字节
			if((ret!=100)||ret==SOCKET_ERROR)
			{
				StopWriteRam();
				return COMM_FAILED;
			}
			
			recv(m_socket,ACK,8,0);	
			temp+=100;
		}		
	}
	if(Re>0)
	{
		memset(ACK,0x00,8);
		send(m_socket,temp,Re,0);			//发送剩余字节数
		recv(m_socket,ACK,8,0);
	}
	retval=StopWriteRam();						//发送"写RAM命令"
	while(retval)
	{
		retval = StopWriteRam();	
	}

	return COMM_OK;
}
/*
	每次接收   2048 BYTE
	有效字节数 nSizeBuffer
*/
int CHSCommPort::ReceiveData(char* pRecvBuffer,UINT nSizeOfBuffer)
{

	char* temp=pRecvBuffer;
	char RecvTemp[2048];
	int ret;
	int retVal;
	memset(RecvTemp,0x55,2048);
	int sum;

	ULONG addr=0;
	UINT Mo=nSizeOfBuffer/2046;
	UINT Re=nSizeOfBuffer%2046;
	if(Mo>0)
	{
		for(UINT i=0;i<Mo;i++)
		{

			retVal=StartReadRam(addr);			//SendCommand
			
			sum=2048;
			ret=0;
			while(sum)//收完2048字节,
			{
				Sleep(1);
				ret=recv(m_socket,RecvTemp+ret,2048,0);
				if(retVal!=COMM_OK)
					return retVal;
				if((ret==SOCKET_ERROR)||(ret==0))
				{
					ret=0;
					sum =2048;
					StartReadRam(addr);
					Sleep(1);
					ret=recv(m_socket,RecvTemp+ret,2048,0);
					if((ret==SOCKET_ERROR)||(ret==0))
					{
						 int err=WSAGetLastError();
       					 return COMM_FAILED;
					}

				}

				sum-=ret;
			}

			memcpy(temp,RecvTemp+2,2046);
			temp+=2046;
			addr+=2046;
		}
	}

	if(Re>0)
	{

		retVal=StartReadRam(addr);
		
		sum=2048;
		ret=0;
		while(sum)//收完2048字节,
		{
			Sleep(1);
			ret=recv(m_socket,RecvTemp+ret,2048,0);
			if(retVal!=COMM_OK)
			return retVal;
			if((ret==SOCKET_ERROR)||(ret==0))
			{
				ret=0;
				sum =2048;
				StartReadRam(addr);
				Sleep(1);
				ret=recv(m_socket,RecvTemp+ret,2048,0);
				if((ret==SOCKET_ERROR)||(ret==0))
				{
					 int err=WSAGetLastError();
       				 return COMM_FAILED;
				}

			}

			sum-=ret;

		}
		memcpy(temp,RecvTemp+2,Re);
	}

	return COMM_OK;
}

//发送8字节命令
BOOL CHSCommPort::SendCommand(char* pCommand)
{
	int ret=send(m_socket,pCommand,8,0);
	if(ret!=8)
	{
		return FALSE;
	}
	return TRUE;
}

//接收8字节命令返回
BOOL CHSCommPort::RecvACK(char* pACK)
{
//	Sleep(1);
	int ret=recv(m_socket,pACK,8,0);
	if((ret == SOCKET_ERROR)||(ret == 0))
	{
		return FALSE;
	}
	return TRUE;
}


void CHSCommPort::CloseSocket()
{
	closesocket(m_socket);
}


void CHSCommPort::SetSendTimeout(int timeout)
{
	if(timeout<=0)
		return;
	m_nSendTimeout=timeout;
}
void CHSCommPort::SetRecvTimeout(int timeout)
{
	if(timeout<=0)
		return;
		m_nRecvTimeout=timeout;
}
void CHSCommPort::SetConnectTimeout(int timeout)
{
	if(timeout<=0)
		return;
	m_nConnectTimeout=timeout;
}

//开关LED
int CHSCommPort::TurnOnLED(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_LED_CTL;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

//开始写RAM命令
int CHSCommPort::StartWriteRam()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	memset(ACK,0x01,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_START_WRITE_RAM;
	if(!SendCommand(Command))
	{
		return COMM_FAILED;	
	}
	if(!RecvACK(ACK))
	{
		return COMM_FAILED;
	}
	if(memcmp(Command,ACK,8))
	{
		return COMM_ERR;	
	}
	return COMM_OK;
}


int CHSCommPort::StartPrint()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	memset(ACK,0x01,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_START_PRINT;
	Command[5]=(char)0x55;
	Command[6]=(char)0x55;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
	{
		return COMM_ERR;
	}
	return COMM_OK;
}


int CHSCommPort::StopPrint()
{
	char Command[8];
	char ACK[8];
	memset(ACK,0x01,8);
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_STOP_PRINT;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
	{		
		return COMM_ERR;
	}
	return COMM_OK;
}

int CHSCommPort::PausePrint()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_PAUSE_PRINT;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;

}

int CHSCommPort::GetStatus(char* status)
{
	char Command[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_GET_STAUS;
	if(!SendCommand(Command))
		return COMM_FAILED;
	int ret=recv(m_socket,status,10,0);
	if((ret==SOCKET_ERROR)||(ret==0)||(ret!=10))
	{
		return COMM_FAILED;
	}
	if((status[0]!=CMD_HEAD)||(status[9]!=CMD_TAIL))
	{
		return COMM_ERR;
	}
	return COMM_OK;
}

int CHSCommPort::EncoderReset()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	memset(ACK,0x01,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_ENCODER_RESET;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
	{
		return COMM_ERR;
	}
	return COMM_OK;
}

int CHSCommPort::GasControl(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_GAS_ALLCTL;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

int CHSCommPort::VacummControl(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_GAS_VCMMCTL;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

int	CHSCommPort::HeadClear(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,0);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_HEAD_CLEAR;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

int CHSCommPort::InkPumpControl(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,0);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_INK_CTL;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

//前门使能控制
int CHSCommPort::DoorEnable(BOOL bTurnOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,0);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_DOOR_ENABLE;
	if(bTurnOn)
		Command[6]=SWITCH_ON;
	else
		Command[6]=SWITCH_OFF;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}

int CHSCommPort::StopWriteRam()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	memset(ACK,0x01,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_STOP_WRITE_RAM;
	if(!SendCommand(Command))
	{	
		return COMM_FAILED;	
	}
	if(!RecvACK(ACK))
	{	
		return COMM_FAILED;	
	}
	if(memcmp(Command,ACK,8))
	{
		return COMM_ERR;	
	}
	return COMM_OK;
}


int CHSCommPort::StartReadRam(UINT fromAddr)
{
	char Command[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_READ_RAM;
	Command[4]=(char)((fromAddr&0xff0000)>>16);
	Command[5]=(char)((fromAddr&0x00ff00)>>8);
	Command[6]=(char)(fromAddr&0x0000ff);
	Command[7]=CMD_TAIL;
	if(!SendCommand(Command))
	{
		return COMM_FAILED;
	}

	return COMM_OK;
}



int CHSCommPort::SetChannelAddr(BYTE nChannelAddr)
{

	char Param[8];
	int ret;
	memset(Param,0x00,8);
	Param[0]='M';
	Param[1]='E';
	Param[2]=0x00;
	Param[3]=0x02;
	Param[4]=0x0a;
	switch(nChannelAddr)
	{
	case 1:
		Param[5]=0x10;
		Param[6]=0x01;
		break;
	case 2:
		Param[5]=0x20;
		Param[6]=0x02;
		break;
	case 3:
		Param[5]=0x30;
		Param[6]=0x03;
		break;
	case 4:
		Param[5]=0x40;
		Param[6]=0x04;
		break;
	}
	for(int i=0;i<7;i++)
		Param[7]+=Param[i];

	for(int j=0;j<8;j++)
	{
		ret=SetPowerParam(Param[j]);
		if(ret!=COMM_OK)
			return ret;
	}
	return COMM_OK;
}

int CHSCommPort::PowerSwitch(BOOL bTurnOn,BYTE nChannelAddr)
{
	if(nChannelAddr!=0x10&&nChannelAddr!=0x20&&nChannelAddr!=30&&nChannelAddr!=40)
		return COMM_FAILED;
	char Param[7];
	int ret;
	memset(Param,0x00,7);
	Param[0]='M';
	Param[1]='E';
	Param[2]=(char)nChannelAddr;
	Param[3]=0x01;
	Param[4]=0x01;
	if(bTurnOn)
		Param[5]=0x01;
	else
		Param[5]=0x00;

	int i;
	for(i=0;i<6;i++)
	{
		Param[6]+=Param[i];
	}

	for(int j=0;i<7;i++)
	{
		ret=SetPowerParam(Param[j]);
		if(ret!=COMM_OK)
			return ret;		
	}
	return COMM_OK;
}



int CHSCommPort::SetPowerParam(char param)//发送高压电源的子函数
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_SET_VOL;
	Command[6]=param;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}


void CHSCommPort::SetFuncParam(BYTE Rise,BYTE Fall,BYTE Due,BYTE Vol)
{
	m_nRise=Rise;
	m_nFall=Fall;
	m_nDue=Due;
	m_nVol=Vol;
}


BYTE CHSCommPort::WaveFormFunc(float input)
{
	BYTE total=m_nRise+m_nFall+m_nDue;
	if(input<0)
		return 0;
	float output;
	if(input<=m_nRise)
		output=(float) (m_nVol*255*input)/(180*m_nRise);
	else if(input<m_nRise+m_nDue)
		output=(float)(m_nVol*255/180);
	else
		output=(float)((total-input)*255*m_nVol)/(180*m_nFall);
	return (BYTE)output;
}


int CHSCommPort::SendWaveFormData(BYTE nChannelAddr)//发送所有波形数据函数
{
	if(nChannelAddr!=0x10&&nChannelAddr!=0x20&&nChannelAddr!=0x30&&nChannelAddr!=0x40)
		return COMM_ERR;
	int ret=0;
	BYTE total=m_nRise+m_nDue+m_nFall;
	float temp=(float)(total*8.25);
	BYTE sampleNum=(BYTE)temp+1;
	BYTE* WaveFormData=new BYTE[sampleNum];
	memset(WaveFormData,0x00,sampleNum);
	for(int i=0;i<sampleNum;i++)
	{
		float input=(float)(i/8.25);
		WaveFormData[i]=WaveFormFunc(input);
	}
	BYTE SendData[256];
	memset(SendData,0x00,256);
	memcpy(SendData,WaveFormData,sampleNum);
	BYTE startAddr=0x00;

	char tempData[16];
	for(int j=0;j<16;j++)
	{
		memcpy(tempData,SendData+startAddr,16);
		ret=SendWaveFormCmd(nChannelAddr,startAddr,(BYTE*)tempData,16);
		Sleep(1);
		if(ret!=COMM_OK)
		{
			delete[] WaveFormData;
			return ret;
		}
		startAddr+=16;
	}

	delete[] WaveFormData;
	return COMM_OK;
}

/*
	nLength----波形采样点的数据长度，字节为单位
*/
int CHSCommPort::SendWaveFormCmd(BYTE nChannelAddr,BYTE nStartAddr,BYTE* data,BYTE nlength)//发送一次波形数据函数
{

	int ret=0;
	BYTE CommandLength=7+nlength;
	BYTE* pCommand=new BYTE[CommandLength];
	memset(pCommand,0x00,CommandLength);
	
	pCommand[0]='M';
	pCommand[1]='E';
	pCommand[2]=nChannelAddr;
	pCommand[3]=nlength+2;
	pCommand[4]=0x04;
	pCommand[5]=nStartAddr;
	
	memcpy(pCommand+6,data,nlength);
	for(int i=0;i<CommandLength-1;i++)
	{
		pCommand[CommandLength-1]+=pCommand[i];
	}


	for(int j=0;j<CommandLength;j++)
	{

		ret=SetPowerParam((char)pCommand[j]);
		usDelay(500);
		if(ret!=COMM_OK)
			return ret;
	}
	return COMM_OK;
}



int CHSCommPort::SetRowsNum(BYTE nNums)
{

	if(nNums!=0x01&&nNums!=0x02&&nNums!=0x03&&nNums!=0x04)
		return COMM_FAILED;
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_CHANGE_ROW;
	Command[6]=8*nNums;

	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}



int CHSCommPort::QueryChannelAddrAndNums(BYTE nChannelAddr,BYTE nStartAddr)
{
//	if(nChannelAddr!=0x10||nChannelAddr!=0x20||nChannelAddr!=0x30||nChannelAddr!=0x40)
//		return COMM_FAILED;
	int ret=0;
	char pCommand[7];
	memset(pCommand,0x00,7);
	pCommand[0]='M';
	pCommand[1]='E';
	pCommand[2]=nChannelAddr;
	pCommand[3]=(char)0x01;
	pCommand[4]=(char)0x03;
	pCommand[5]=(char)nStartAddr;
	for(int i=0;i<6;i++)
		pCommand[6]+=pCommand[i];
	for(int j=0;j<7;j++)
	{
		ret=SetPowerParam(pCommand[j]);
		if(ret!=COMM_OK)
			return ret;
	}
	return COMM_OK;
}

/*
int CHSCommPort::Query16ByteData(BYTE nStartAddr)
{
	int ret=0;
	BYTE pCommand[7];
	memset(pCommand,0x00,7);
	pCommand[0]='M';
	pCommand[1]='E';
	pCommand[2]=nStartAddr;
	pCommand[3]=0x01;
	pCommand[4]=0x03;
	pCommand[5]=nStartAddr;
	for(int i=0;i<6;i++)
	{
		ret=SetPowerParam((char)pCommand);
		if(ret!=COMM_OK)
			return ret;
	}
	return COMM_OK;
}
*/

int CHSCommPort::EnableChannelAddr(BYTE nStartAddr)
{
	char Command[7];
	int ret;
	memset(Command,0x00,6);
	Command[0]='M';
	Command[1]='E';
	Command[2]=(char)nStartAddr;
	Command[3]=(char)0x00;
	Command[4]=(char)0x0b;
	Command[5]=(char)0x00;
	for(int i=0;i<6;i++)
	{
		Command[6]+=Command[i];
	}
	for(int j=0;j<7;j++)
	{
		ret=SetPowerParam(Command[j]);
		if(ret!=COMM_OK)
			return ret;
	}
	return COMM_OK;
}

int CHSCommPort::AD9832EnableFreq()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_9832_CTL;
	Command[5]=(char)0xC0;
	Command[6]=(char)0x00;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}
int CHSCommPort::AD9832EnablePhase()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_9832_CTL;
	Command[5]=(char)0xb0;
	Command[6]=(char)0x00;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
	
}
int CHSCommPort::AD9832Reset()
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_9832_CTL;
	Command[5]=(char)0xf8;
	Command[6]=(char)0x00;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}
int CHSCommPort::AD9832SetFreq(ULONG freq)
{
	if(freq<=0)
		return COMM_FAILED;
	char Command[8];
	memset(Command,0x00,8);
	char ACK[8];

	ULONG data=freq;

	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_9832_CTL;








	Command[5]=0x33;
	Command[6]=(char)((data&0xff000000)>>24);/*25--32bit*/
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;


	Command[5]=0x22;
	Command[6]=(char)((data&0x00ff0000)>>16);/*17--24bit*/
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;

	Command[5]=0x31;
	Command[6]=(char)((data&0x0000ff00)>>8);/*9--16bit*/
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;

	Command[5]=0x20;
	Command[6]=(char)(data&0x000000ff);/*1--8bit*/
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;	

	return COMM_OK;
}
int CHSCommPort::AD9832SetPhase(WORD delay)
{
	if(delay>360)
		return COMM_FAILED;
	if(delay==360)
		delay=0;
	
	float ratio=(float)delay/360;
	WORD data=(WORD)(ratio*4096);

	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_9832_CTL;
	Command[5]=0x08;
	Command[6]=(char)(data&0x00ff);
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;

	Command[5]=0x19;
	Command[6]=(char)((data&0x0f00)>>8);
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;

	return COMM_OK;
}



int CHSCommPort::TuneLEDlum(UINT lum)
{
	char Command[8];
	char ACK[8];
//	char data=(char)0x40;
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_LED_LUM;
	Command[3]=(lum&0xff000000)>>24;
	Command[4]=(lum&0x00ff0000)>>16;
	Command[5]=(lum&0x0000ff00)>>8;

	Command[6]=(lum&0x000000ff);
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}


int CHSCommPort::NozzleTestControl(BOOL bTest)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_TEST_NOZZLE;
	if(bTest)
		Command[6]=(char)0x01;
	else
		Command[6]=(char)0x00;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}


int CHSCommPort::SetNozzleFreq(ULONG freq)
{
	char Command[8];
	char ACK[8];
	ULONG data=freq;

	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[7]=CMD_TAIL;
	Command[1]=CMD_NOZZLE_FREQ;

	Command[6]=(char)(data&0x000000ff);
	Command[5]=(char)((data&0x0000ff00)>>8);
	Command[4]=(char)((data&0x00ff0000)>>16);
	Command[3]=(char)((data&0xff000000)>>24);
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;

}
	
void CHSCommPort::usDelay(UINT us)
{
		LARGE_INTEGER litmp;
		LONGLONG QPart1,QPart2;
		double dfMinus, dfFreq, dfTim;
		dfTim=0.0;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;// 获得计数器的时钟频率
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;// 获得初始值
		
		while(dfTim<us)
		{
			QueryPerformanceCounter(&litmp);
			QPart2 = litmp.QuadPart;//获得中止值
			dfMinus = (double)(QPart2-QPart1);
			dfTim = dfMinus / dfFreq;// 获得对应的时间值，单位为秒
			dfTim*=1000000;
		}
}

int	CHSCommPort::TuneLEDCurrent(WORD wValue)			//LED电流
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_LED_CURRENT;		//0x1c
	Command[5]=(wValue&0xff00)>>8;
	Command[6]=(wValue&0xff);
	Command[7]=CMD_TAIL;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}
int	CHSCommPort::TuneDelaySpeed(BYTE speed)				//慢镜头速度
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_DELAY_SPEED;			//0x1e
	Command[7]=CMD_TAIL;
	Command[6]=(char)speed;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}
int	CHSCommPort::SlowMode(BOOL bOn)						//慢镜头开关
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_SLOW_MODE;	//0x1d
	if(bOn)
		Command[6]=(char)0x01;
	else
		Command[6]=(char)0x00;
	Command[7]=CMD_TAIL;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}


int CHSCommPort::RepeatPrintCtl(BOOL bOn)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_REPEAT_CTL;
	if(bOn)
		Command[6]=(char)0x01;
	else
		Command[6]=(char)0x00;
	Command[7]=CMD_TAIL;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;	
}
int CHSCommPort::SetRepeatTimes(int times)
{
	char Command[8];
	char ACK[8];
	memset(Command,0x00,8);
	Command[0]=CMD_HEAD;
	Command[1]=CMD_REPEAT_TIMES;
	//Command[2]=i+1;
	Command[3]=(times&0xff000000)>>24;
	Command[4]=(times&0x00ff0000)>>16;
	Command[5]=(times&0x0000ff00)>>8;
	Command[6]=(times&0x000000ff);
	Command[7]=CMD_TAIL;
	if(!SendCommand(Command))
		return COMM_FAILED;
	if(!RecvACK(ACK))
		return COMM_FAILED;
	if(memcmp(Command,ACK,8))
		return COMM_ERR;
	return COMM_OK;
}


