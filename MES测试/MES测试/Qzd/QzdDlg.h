
// QzdDlg.h : 头文件
//

#pragma once
#include "MvCameraControl.h"
#include "..\DllMod\Singleton.h"

#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "mscomm1.h"


// CQzdDlg 对话框
class CQzdDlg : public CDialogEx
{
// 构造
public:
	CQzdDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_QZD_DIALOG };

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
	void Snap(CHWin& HW,int nTimes = 4);
	BOOL GetDLLFunc();
	HINSTANCE  m_hDll;
	CWinThread* m_pThr[CAM_NUM];
	int m_nCHK[CAM_NUM];                                     //是否收到检测指令
	int m_nThread;
	void SetupCallBack(unsigned char ** pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, int II);
	CHWin m_HWRun[CAM_NUM];                            //四个生产时候的相机
	void* m_Camhandle[CAM_NUM];
	int InitCam(int iNum);
	int CloseCam(int iNum);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnImg();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedBtnRun();
	CComboBox m_comboType;
	CSliderCtrl m_sldPar;
	double m_fSldCoef;
	afx_msg void OnBnClickedBtnReal();
	afx_msg void OnNMReleasedcaptureSliderPar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboType();
	void SetEnable(BOOL Ena = TRUE);
	void InitSlider();
	void SendResult(int nSTS, int nOK);
	void ClearResult(int nSTS);
	void SendFindOrder();	
	bool InitialCom(int iNO, int nBaud);
	afx_msg void OnBnClickedBtnTrg();
	CMscomm1 m_Com;
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
};
