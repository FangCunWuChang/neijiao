
// TwoCamDlg.h : ͷ�ļ�
//

#pragma once
#include "MvCameraControl.h"
#include "..\DllMod\Singleton.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "..\DllMod\\IDlgCamCfg.h"
#include "..\DllMod\HSCommPort.h"

#include "afxwin.h"

// CTwoCamDlg �Ի���
class CTwoCamDlg : public CDialogEx
{
// ����
public:
	CTwoCamDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TWOCAM_DIALOG };

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
	CComboBox m_comboCam;
	int m_nThread;
	HINSTANCE  m_hDll;
	CWinThread* m_pThr[CAM_NUM];
	int m_nCHK[CAM_NUM];                                     //�Ƿ��յ����ָ��
	CHWin m_HWRun[CAM_NUM];                            //�ĸ�����ʱ������
	BOOL GetDLLFunc();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnBnClickedBtnReal();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetEnable(BOOL bEna);
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnTrg();
	CHSCommPort m_Lasr;
	CString m_strCode;
	int UpMESLoad(CString cstrStationNumber, CString cstrEmpNo);
	//////////////////////////////////������///////////////////////////////////////////////
	int checkMESSN(CString cstrSN, CString cstrStationNumber);
	//////////////////////////////////������///////////////////////////////////////////////
	int sendMESData(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestData);
	//////////////////////////////////�����///////////////////////////////////////////////
	int sendMESResult(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestResult);

	afx_msg void OnBnClickedBtnSend();
};
