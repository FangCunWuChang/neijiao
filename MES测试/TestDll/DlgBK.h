#pragma once
#include "..\DllMod\HSCommPort.h"
#include "..\DllMod\Label.h"
#include "..\DllMod\Singleton.h"
#include "afxwin.h"
#include "mscomm1.h"

// CDlgBK �Ի���

class CDlgBK : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBK)

public:
	CDlgBK(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgBK();

// �Ի�������
	enum { IDD = IDD_DIALOG_BK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSignal m_sg[9];
	CHWin m_HW[4];
	int m_nThread, m_nCALI,m_nRUNs;  //����һ���߳̿���RUN�����������������m_nThread һ�¿����Զ�����
	CComboBox m_comboCam;
	virtual BOOL OnInitDialog();
	CLabel m_lbPLC,m_lbRBT[2];
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnCam();
	afx_msg void OnBnClickedBtnImg();
	CMscomm1 m_Com[2];
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	void OnCommMscomm2();
	void SendLaserOrder(CMscomm1& m_Com);
	bool InitialCom(int iNO[2], int nBaud = 9600);
	void GetLaserVal(CMscomm1& m_Com, int nLsr);
public:
	void UnLoadXML(int nCam, int nSumXML);
	afx_msg void OnBnClickedBtnOffset();
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetEnable(BOOL bEna /* = TRUE */);
	afx_msg void OnBnClickedBtnTrig();
	afx_msg void OnBnClickedCheckXml();
};
