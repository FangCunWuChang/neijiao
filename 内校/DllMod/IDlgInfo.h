#pragma once
#include "afxwin.h"


// CIDlgInfo 对话框

class CIDlgInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CIDlgInfo)

public:
	CIDlgInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIDlgInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_list;
	afx_msg void OnBnClickedCheckLast();
	afx_msg void OnBnClickedOk();
};
