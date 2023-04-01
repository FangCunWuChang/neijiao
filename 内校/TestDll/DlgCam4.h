#pragma once
#include "afxwin.h"
#include "..\DllMod\HSCommPort.h"
#include "..\DllMod\Label.h"
#include "..\DllMod\Singleton.h"

// CDlgCam4 对话框

class CLabel;

class CDlgCam4 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCam4)

public:
	CDlgCam4(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCam4();

// 对话框数据
	enum { IDD = IDD_DIALOG_CAM4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSignal m_sg[4];
	CHWin m_HW[4];
	CLabel m_lbIP[2];
	CHSCommPort _PLC;
	int m_nThread,m_nCALI;
	CComboBox m_comboCam;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedBtnImage();
	afx_msg void OnBnClickedBtnCalib();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetEnable(BOOL bEna);
	afx_msg void OnBnClickedBtnTrig();
	afx_msg void OnCbnSelchangeComboCam();
	afx_msg void OnBnClickedBtnCam();
	afx_msg void OnBnClickedBtnRun();
//	afx_msg void OnBnClickedBtnJob();
	afx_msg void OnBnClickedBtnPos();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	CComboBox m_comboPos;
	afx_msg void OnBnClickedBtnOffset();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
