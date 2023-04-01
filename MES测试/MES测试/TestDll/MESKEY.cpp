// MESKEY.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDll.h"
#include "MESKEY.h"
#include "afxdialogex.h"


// MESKEY 对话框

IMPLEMENT_DYNAMIC(MESKEY, CDialogEx)

MESKEY::MESKEY(CWnd* pParent /*=NULL*/)
	: CDialogEx(MESKEY::IDD, pParent)
{

}

MESKEY::~MESKEY()
{
}

void MESKEY::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MESKEY, CDialogEx)
	ON_BN_CLICKED(IDOK, &MESKEY::OnBnClickedOk)
END_MESSAGE_MAP()


// MESKEY 消息处理程序


void MESKEY::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CSingleton* pSng = CSingleton::GetInstance();
	CString strMESKEY,strMesReal;
	GetDlgItemText(IDC_EDIT_KEY, strMESKEY);
	strMesReal =pSng->GetCfgString("MES", "KEY", "123123");
	bMesKey = false;
	if (strMesReal == strMESKEY)
	{
		bMesKey = true;
	}
	else
	{
		bMesKey = false;
	}
	CDialogEx::OnOK();
}
