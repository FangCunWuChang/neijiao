// HSCommPort.h: interface for the CHSCommPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSCOMMPORT_H__941AE9B4_83B1_460C_AC7E_863249E66A69__INCLUDED_)
#define AFX_HSCOMMPORT_H__941AE9B4_83B1_460C_AC7E_863249E66A69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HS_def.h"

class CHSCommPort  
{
public:

	CHSCommPort();
	virtual ~CHSCommPort();
	void    GetHostIP(CString& strIP);
	BOOL	InitSocket();
	BOOL	ConnectPort(CString IPAddress,UINT PortNum);
	void	CloseSocket();

	
	int		SendData(char* pSendBuffer,UINT nSizeOfBuffer);		//发送大的图像数据到RAM，包含光栅间隔等参数
	int		ReceiveData(char* pRecvBuffer,UINT nSizeOfBuffer);	//从RAM接收大的图像数据

	
	BOOL	SendCommand(char* pCommand);					//通用发送的8字节命令
	BOOL	RecvACK(char* pACK);							//通用接收8字节返回

	BOOL    RecvACK(char* pACK,int nLen)
	{
		//	Sleep(1);
		int ret = recv(m_socket,pACK,nLen,0);
		if((ret == SOCKET_ERROR)||(ret == 0))
		{
			return FALSE;
		}
		return TRUE;
	}

	int    SendCommonData(char* data,int nLen,char* buf);     //发送char数组指令,接收的指令少于4096
	////////////////////////////////////Lee//////////////////////////////////////
	int MySend(char* data,int nLen)
	{
		int nRet = send(m_socket,data,nLen,0);
		int iPos = 0;
		//for (;;)
		//{
		//	Sleep(50);
		//	nRet = recv(m_socket,buf + iPos,4096,0);
		//	if (nRet <= 0)
		//		break;
		//	iPos += nRet;
		//}
		return iPos + nRet;
	}
	BOOL    MyRecv(char* buff,int nLen)
	{
		//	Sleep(1);
		memset(buff,0,nLen);
		int ret = recv(m_socket,buff,nLen,0);
		if((ret == SOCKET_ERROR)||(ret == 0))
		{
			return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	//设定超时
	void	SetSendTimeout(int timeout);				//单位ms
	void	SetRecvTimeout(int timeout);				//单位ms
	void	SetConnectTimeout(int timeout);				//单位s


	
	int		TurnOnLED(BOOL bTurnOn);					//发送打开定位LED命令
	int		StartWriteRam();							//发送开始写RAM命令
	int		StartPrint();								//发送开始打印命令		
	int		StopPrint();								//发送停止打印命令
	int		PausePrint();								//发送暂停打印命令

	
	int		GetStatus(char* status);					//发送获取状态命令，参数:10字节数组指针
	int		EncoderReset();								//光栅复位命令

	
	int		GasControl(BOOL bTurnOn);					//总气阀开关
	int		VacummControl(BOOL bTurnOn);				//真空开关
	int		InkPumpControl(BOOL bTurnOn);				//墨盒泵开关
	int		HeadClear(BOOL bTurnOn);					//喷头清洗  
	int		DoorEnable(BOOL bTurnOn);					//前门使能开关
	
	int		StopWriteRam();								//发送停止写RAM命令
	int		StartReadRam(UINT fromAddr);				//发送开始读RAM命令


	
	int		SetPowerParam(char param);						//电源设置子函数 *
	int		SetChannelAddr(BYTE nChannelAddr);				//设置通道首地址函数，每个通道地址内建(0x10,0x20,0x30,0x40), call *
	int		PowerSwitch(BOOL bTurnOn,BYTE nChannelAddr);	//电源个通道开关	

	
	void	SetFuncParam(BYTE Rise,BYTE Fall,BYTE Due,BYTE Vol);//设置波形参数
	BYTE	WaveFormFunc(float input);							//波形曲线函数						
	int		SendWaveFormData(BYTE nChannelAddr);				//发送所有波形数据 call SendWaveFormCmd
	int		SendWaveFormCmd(BYTE nChannelAddr,BYTE nStartAddr,BYTE* data,BYTE nlength);//发送一次波形数据(最大16字节),
	int		SetRowsNum(BYTE nNums);							//设置喷头排数 目前合法值:1 2 3 4

/***********************当前已经不使用*******************************************/
	int		QueryChannelAddrAndNums(BYTE nChannelAddr,BYTE nStartAddr);
	int		EnableChannelAddr(BYTE nStartAddr);
	int		AD9832EnableFreq();						//频率使能，同时打开观测LED						
	int		AD9832EnablePhase();					//相位使能
	int		AD9832Reset();							//复位							
	int		AD9832SetFreq(ULONG freq);				//设置频率和相位					
	int		AD9832SetPhase(WORD delay);						
/*******************************************************************************/



	int		NozzleTestControl(BOOL bTest);					//喷头测试开关
	int		SetNozzleFreq(ULONG freq);						//设置喷头频率
	int		SetNozzleData(UINT Lower,UINT Upper,int rows);	//设置喷头数据(64)

/*****************新增和修改**************************/
	int		TuneLEDlum(UINT lum);					//调节相位
	int		TuneLEDCurrent(WORD wValue);			//LED电流
	int		TuneDelaySpeed(BYTE speed);				//慢镜头速度
	int		SlowMode(BOOL bOn);						//慢镜头开关
/*****************************************************/

	void	usDelay(UINT us);
	
	int		RepeatPrintCtl(BOOL bOn);                   //精确墨点个数打印开始
	int		SetRepeatTimes(int times);					//设置打印次数
	void    delayus(int us);


	BOOL m_bInit;
	SOCKET m_socket;
private:

	int m_nSendTimeout;
	int m_nRecvTimeout;
	int m_nConnectTimeout;
	BYTE m_nRise;
	BYTE m_nFall;
	BYTE m_nDue;
	BYTE m_nVol;

};

#endif // !defined(AFX_HSCOMMPORT_H__941AE9B4_83B1_460C_AC7E_863249E66A69__INCLUDED_)
