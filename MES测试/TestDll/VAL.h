#pragma once
#include "..\DllMod\Singleton.h"


// VAL 对话框

class VAL : public CDialogEx
{
	DECLARE_DYNAMIC(VAL)

public:
	VAL(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~VAL();

// 对话框数据
	enum { IDD = IDD_DIALOG_VAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
