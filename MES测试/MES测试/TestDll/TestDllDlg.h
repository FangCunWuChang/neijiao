
// TestDllDlg.h : ͷ�ļ�
//

#pragma once
#include "MvCameraControl.h"
#include "afxwin.h"
#include "mscomm1.h"
#include "..\DllMod\Singleton.h"

// CTestDllDlg �Ի���
class CTestDllDlg : public CDialogEx
{
// ����
public:
	CTestDllDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTDLL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CHWin m_HWRun[CAM_THREAD];                            //�ĸ�����ʱ������
	CWinThread* m_pThr[CAM_THREAD];
	int m_nThread;
	HINSTANCE  m_hDll;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_comboCam;
	afx_msg void OnBnClickedBtnJob();
	CMscomm1 m_Com;
	void SendFindOrder();
	void SendResult(int nSTS, int nOK = 1);
	void ResetResult(int nSTS);
	int m_nCHK[CAM_THREAD];                                     //�Ƿ��յ����ָ��
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	afx_msg void OnBnClickedBtnRun();
	void SetEnable(BOOL bEna);
	afx_msg void OnBnClickedBtnManu();
	afx_msg void OnBnClickedBtnTrig();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedBtnCam();
	//BOOL CTestDllDlg::GetDLLFunc();
};
