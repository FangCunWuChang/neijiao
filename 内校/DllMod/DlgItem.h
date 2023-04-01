#pragma once
#include "ListCtrlCl.h"
#include "Singleton.h"
#include "afxwin.h"

// CDlgItem �Ի���

class CDlgItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgItem)

public:
	CDlgItem(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgItem();

// �Ի�������
	enum { IDD = IDD_DIALOG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrlCl m_listNode;
	CString m_strNick;
	vector<CPos> psLeaf;
	vector<CListStyle> sts;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkListNode(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	CComboBox m_comboTol;
	afx_msg void OnCbnSelchangeComboTol();
	afx_msg void OnBnClickedBtnDelTol();
	afx_msg void OnBnClickedBtnAddTol();
//	afx_msg void OnBnClickedBtnSavTol();
	afx_msg void OnBnClickedBtnSavTol();
};
