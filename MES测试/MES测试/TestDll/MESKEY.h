#pragma once
#include "..\DllMod\Singleton.h"


// MESKEY 对话框

class MESKEY : public CDialogEx
{
	DECLARE_DYNAMIC(MESKEY)

public:
	MESKEY(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MESKEY();

// 对话框数据
	enum { IDD = IDD_DIALOG_MESKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	bool bMesKey;
};
