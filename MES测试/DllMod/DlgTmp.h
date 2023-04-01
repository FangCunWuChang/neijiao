#pragma once
#include "afxwin.h"
#include "Singleton.h"
#include "afxcmn.h"


// CDlgTmp 对话框

class CDlgTmp : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTmp)

public:
	CDlgTmp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTmp();

// 对话框数据
	enum { IDD = IDD_DIALOG_TMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nBlack;
	HObject hoTMP,hoXLD;
	HTuple m_WndID;
	double m_fDelta[2];
	CString strTmpName,strOffset;
	int m_nTmpTYP;
	virtual BOOL OnInitDialog();
	CComboBox m_comboMod;
	afx_msg void OnBnClickedBtnOffset();
	afx_msg void OnEnChangeEditName();
	CSliderCtrl m_sldBinVal;
	afx_msg void OnCbnSelchangeComboMod();
	afx_msg void OnNMReleasedcaptureSliderBin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
