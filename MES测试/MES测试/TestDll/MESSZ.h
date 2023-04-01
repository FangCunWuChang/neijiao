#pragma once
#include "..\DllMod\Singleton.h"


// MESSZ 对话框

class MESSZ : public CDialogEx
{
	DECLARE_DYNAMIC(MESSZ)

public:
	MESSZ(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MESSZ();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MESSZ };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

};
