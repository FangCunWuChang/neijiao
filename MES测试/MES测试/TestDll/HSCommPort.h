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

	
	int		SendData(char* pSendBuffer,UINT nSizeOfBuffer);		//���ʹ��ͼ�����ݵ�RAM��������դ����Ȳ���
	int		ReceiveData(char* pRecvBuffer,UINT nSizeOfBuffer);	//��RAM���մ��ͼ������

	
	BOOL	SendCommand(char* pCommand);					//ͨ�÷��͵�8�ֽ�����
	BOOL	RecvACK(char* pACK);							//ͨ�ý���8�ֽڷ���

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

	int    SendCommonData(char* data,int nLen,char* buf);     //����char����ָ��,���յ�ָ������4096
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
	//�趨��ʱ
	void	SetSendTimeout(int timeout);				//��λms
	void	SetRecvTimeout(int timeout);				//��λms
	void	SetConnectTimeout(int timeout);				//��λs


	
	int		TurnOnLED(BOOL bTurnOn);					//���ʹ򿪶�λLED����
	int		StartWriteRam();							//���Ϳ�ʼдRAM����
	int		StartPrint();								//���Ϳ�ʼ��ӡ����		
	int		StopPrint();								//����ֹͣ��ӡ����
	int		PausePrint();								//������ͣ��ӡ����

	
	int		GetStatus(char* status);					//���ͻ�ȡ״̬�������:10�ֽ�����ָ��
	int		EncoderReset();								//��դ��λ����

	
	int		GasControl(BOOL bTurnOn);					//����������
	int		VacummControl(BOOL bTurnOn);				//��տ���
	int		InkPumpControl(BOOL bTurnOn);				//ī�бÿ���
	int		HeadClear(BOOL bTurnOn);					//��ͷ��ϴ  
	int		DoorEnable(BOOL bTurnOn);					//ǰ��ʹ�ܿ���
	
	int		StopWriteRam();								//����ֹͣдRAM����
	int		StartReadRam(UINT fromAddr);				//���Ϳ�ʼ��RAM����


	
	int		SetPowerParam(char param);						//��Դ�����Ӻ��� *
	int		SetChannelAddr(BYTE nChannelAddr);				//����ͨ���׵�ַ������ÿ��ͨ����ַ�ڽ�(0x10,0x20,0x30,0x40), call *
	int		PowerSwitch(BOOL bTurnOn,BYTE nChannelAddr);	//��Դ��ͨ������	

	
	void	SetFuncParam(BYTE Rise,BYTE Fall,BYTE Due,BYTE Vol);//���ò��β���
	BYTE	WaveFormFunc(float input);							//�������ߺ���						
	int		SendWaveFormData(BYTE nChannelAddr);				//�������в������� call SendWaveFormCmd
	int		SendWaveFormCmd(BYTE nChannelAddr,BYTE nStartAddr,BYTE* data,BYTE nlength);//����һ�β�������(���16�ֽ�),
	int		SetRowsNum(BYTE nNums);							//������ͷ���� Ŀǰ�Ϸ�ֵ:1 2 3 4

/***********************��ǰ�Ѿ���ʹ��*******************************************/
	int		QueryChannelAddrAndNums(BYTE nChannelAddr,BYTE nStartAddr);
	int		EnableChannelAddr(BYTE nStartAddr);
	int		AD9832EnableFreq();						//Ƶ��ʹ�ܣ�ͬʱ�򿪹۲�LED						
	int		AD9832EnablePhase();					//��λʹ��
	int		AD9832Reset();							//��λ							
	int		AD9832SetFreq(ULONG freq);				//����Ƶ�ʺ���λ					
	int		AD9832SetPhase(WORD delay);						
/*******************************************************************************/



	int		NozzleTestControl(BOOL bTest);					//��ͷ���Կ���
	int		SetNozzleFreq(ULONG freq);						//������ͷƵ��
	int		SetNozzleData(UINT Lower,UINT Upper,int rows);	//������ͷ����(64)

/*****************�������޸�**************************/
	int		TuneLEDlum(UINT lum);					//������λ
	int		TuneLEDCurrent(WORD wValue);			//LED����
	int		TuneDelaySpeed(BYTE speed);				//����ͷ�ٶ�
	int		SlowMode(BOOL bOn);						//����ͷ����
/*****************************************************/

	void	usDelay(UINT us);
	
	int		RepeatPrintCtl(BOOL bOn);                   //��ȷī�������ӡ��ʼ
	int		SetRepeatTimes(int times);					//���ô�ӡ����
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
