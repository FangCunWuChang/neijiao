#pragma once
#include "afxwin.h"


// CDlgOffset 对话框

class CDlgOffset : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOffset)

public:
	CDlgOffset(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOffset();

// 对话框数据
	enum { IDD = IDD_DIALOG_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:	
	void SetRadioText(CString strText, int nID = 0);
	int m_nPos;
	CString strRot;
	int m_nCam;
	CComboBox m_comboPos;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPos();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedBtnSav();

};
