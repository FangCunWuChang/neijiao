#pragma once
#include "afxwin.h"


// CIDlgInfo �Ի���

class CIDlgInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CIDlgInfo)

public:
	CIDlgInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIDlgInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_list;
	afx_msg void OnBnClickedCheckLast();
	afx_msg void OnBnClickedOk();
};
