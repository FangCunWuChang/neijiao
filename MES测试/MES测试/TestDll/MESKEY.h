#pragma once
#include "..\DllMod\Singleton.h"


// MESKEY �Ի���

class MESKEY : public CDialogEx
{
	DECLARE_DYNAMIC(MESKEY)

public:
	MESKEY(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MESKEY();

// �Ի�������
	enum { IDD = IDD_DIALOG_MESKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	bool bMesKey;
};
