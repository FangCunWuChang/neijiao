#pragma once
#include "Singleton.h"

#include <stdio.h>
#include <string.h>
#include <vector>
using namespace std;
#include "../tinyxml/tinyxml.h"
#include "afxcmn.h"
#include "afxwin.h"

// CIDlgJob �Ի���

class CIDlgJob : public CDialogEx
{
	DECLARE_DYNAMIC(CIDlgJob)

public:
	CIDlgJob(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIDlgJob();

// �Ի�������
	enum { IDD = IDD_DIALOG_IJOB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nRadio;
	HTREEITEM m_hRoot;
	CTreeCtrl m_tree;
	CString m_strCam;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedBtnAddKnot();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedBtnSavThread();
	CComboBox m_comboThread;
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboThread();
};
