
// TwoCamDlg.h : 头文件
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

// CTwoCamDlg 对话框
class CTwoCamDlg : public CDialogEx
{
// 构造
public:
	CTwoCamDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TWOCAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	int m_nCHK[CAM_NUM];                                     //是否收到检测指令
	CHWin m_HWRun[CAM_NUM];                            //四个生产时候的相机
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
	//////////////////////////////////找条码///////////////////////////////////////////////
	int checkMESSN(CString cstrSN, CString cstrStationNumber);
	//////////////////////////////////发数据///////////////////////////////////////////////
	int sendMESData(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestData);
	//////////////////////////////////发结果///////////////////////////////////////////////
	int sendMESResult(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestResult);

	afx_msg void OnBnClickedBtnSend();
};
