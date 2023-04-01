// MESKEY.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDll.h"
#include "MESKEY.h"
#include "afxdialogex.h"


// MESKEY �Ի���

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


// MESKEY ��Ϣ�������


void MESKEY::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
