#pragma once
#include "..\DllMod\Singleton.h"


// VAL �Ի���

class VAL : public CDialogEx
{
	DECLARE_DYNAMIC(VAL)

public:
	VAL(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~VAL();

// �Ի�������
	enum { IDD = IDD_DIALOG_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
