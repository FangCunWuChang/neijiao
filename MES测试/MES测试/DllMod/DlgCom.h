#pragma once
#include "afxwin.h"
#include "Singleton.h"

// CDlgCom �Ի���

class CDlgCom : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCom)

public:
	CDlgCom(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCom();

// �Ի�������
	enum { IDD = IDD_DIALOG_COM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString strEdit,strWin;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
	CComboBox m_comboPar;
	vector<CPos> m_comboVals;
	afx_msg void OnBnClickedBtnSav();
	afx_msg void OnCbnSelchangeComboPar();
	afx_msg void OnBnClickedOk();
};
