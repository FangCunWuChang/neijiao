// IDlgInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "IDlgInfo.h"
#include "afxdialogex.h"
#include "Singleton.h"


// CIDlgInfo 对话框
extern void LogFile(CString log);
extern void LogFile(const char *fmt, ...);

IMPLEMENT_DYNAMIC(CIDlgInfo, CDialogEx)

CIDlgInfo::CIDlgInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIDlgInfo::IDD, pParent)
{

}

CIDlgInfo::~CIDlgInfo()
{
}

void CIDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CIDlgInfo, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_LAST, &CIDlgInfo::OnBnClickedCheckLast)
	ON_BN_CLICKED(IDOK, &CIDlgInfo::OnBnClickedOk)
END_MESSAGE_MAP()


// CIDlgInfo 消息处理程序

extern "C" __declspec(dllexport) CIDlgInfo* GetLogInfo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_pLog == NULL)
	{
		CIDlgInfo *ph = new CIDlgInfo;
		ph->Create(CIDlgInfo::IDD);
		ph->ShowWindow(SW_SHOW);
		pSng->_pLog = ph;
		LogFile("日志对话框初始化成功！");
		return ph;
	}
	if (pSng->_pLog->IsWindowVisible())
		pSng->_pLog->ShowWindow(SW_HIDE);
	else
		pSng->_pLog->ShowWindow(SW_SHOW);
	return (CIDlgInfo*)pSng->_pLog;
}

BOOL CIDlgInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_LAST);
	pBtn->SetCheck(BST_CHECKED);
	pSng->_pLog = this;
	SetTimer(1, 900, NULL);
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CIDlgInfo::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_csInfo.Lock();
	int nC = (int)pSng->_Info.size();
	if (nC > m_list.GetCount())
	{
		int n = pSng->_Info.size() - m_list.GetCount();
		for (int i = n - 1; i >= 0; i--)
		{
			m_list.AddString(pSng->_Info[pSng->_Info.size() - i - 1]);
		}
	}
	/////////////////////////////增加以下代码///////////////////////////
	for (; pSng->_Info.size() >= 2000;)
	{
		pSng->_Info.erase(pSng->_Info.begin());
	}
	for (; m_list.GetCount() >= 2000;)
	{
		m_list.DeleteString(0);                                 //删除第一列
	}
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_LAST);
	if (pBtn->GetCheck() == BST_CHECKED && m_list.GetCount() > 20)
		m_list.SetCurSel(m_list.GetCount() - 1);
	/////////////////////// 超过1000行删除原来的老的数据//////////////////////////
	pSng->_csInfo.Unlock();
	CDialogEx::OnTimer(nIDEvent);
}


void CIDlgInfo::OnBnClickedCheckLast()
{
	//CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_LAST);
	//if (pBtn->GetCheck() == BST_UNCHECKED)
	//{
	//	KillTimer(1);
	//}
	//else
	//{
	//	SetTimer(1, 900, NULL);
	//}
}


void CIDlgInfo::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
	//CDialogEx::OnOK();
}
