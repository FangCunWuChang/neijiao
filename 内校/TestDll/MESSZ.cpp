// MESSZ.cpp: 实现文件
//

#include "stdafx.h"
#include "TestDll.h"
#include "MESSZ.h"
#include "afxdialogex.h"
#include "Define.h"

// MESSZ 对话框

IMPLEMENT_DYNAMIC(MESSZ, CDialogEx)

MESSZ::MESSZ(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MESSZ, pParent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->SetCfgString("MES", "deptID", DEPTID);
	pSng->SetCfgString("MES", "partID", PARTID);
	pSng->SetCfgString("MES", "mo_id", MO_ID);
	pSng->SetCfgString("MES", "lineID", LINEID);
	pSng->SetCfgString("MES", "test_Station", TEST_STATION);
	pSng->SetCfgString("MES", "testResult", TESTRESULT);
	pSng->SetCfgString("MES", "machineSN", MACHINESN);
	pSng->SetCfgString("MES", "testchannelID", TESTCHANNELID);
}

MESSZ::~MESSZ()
{
}

void MESSZ::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MESSZ, CDialogEx)
	ON_BN_CLICKED(IDOK, &MESSZ::OnBnClickedOk)
END_MESSAGE_MAP()


// MESSZ 消息处理程序
BOOL MESSZ::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	CString strdeptID = pSng->GetCfgString("MES", "deptID", DEPTID);
	CString strpartID = pSng->GetCfgString("MES", "partID", PARTID);
	CString strmo_id = pSng->GetCfgString("MES", "mo_id", MO_ID);
	CString strlineID = pSng->GetCfgString("MES", "lineID", LINEID);
	CString strtest_Station = pSng->GetCfgString("MES", "test_Station", TEST_STATION);
	CString strtestResult = pSng->GetCfgString("MES", "testResult", TESTRESULT);
	CString strmachineSN = pSng->GetCfgString("MES", "machineSN", MACHINESN);
	CString strtestchannelID = pSng->GetCfgString("MES", "testchannelID", TESTCHANNELID);
	SetDlgItemText(IDC_EDIT1, strdeptID);
	SetDlgItemText(IDC_EDIT2, strpartID);
	SetDlgItemText(IDC_EDIT3, strmo_id);
	SetDlgItemText(IDC_EDIT4, strlineID);
	SetDlgItemText(IDC_EDIT5, strtest_Station);
	SetDlgItemText(IDC_EDIT6, strtestResult);
	SetDlgItemText(IDC_EDIT7, strmachineSN);
	SetDlgItemText(IDC_EDIT8, strtestchannelID);
	return TRUE;  // return TRUE unless you set the focus to a control

}

void MESSZ::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strMES[8];
	GetDlgItemText(IDC_EDIT1, strMES[0]);
	GetDlgItemText(IDC_EDIT2, strMES[1]);
	GetDlgItemText(IDC_EDIT3, strMES[2]);
	GetDlgItemText(IDC_EDIT4, strMES[3]);
	GetDlgItemText(IDC_EDIT5, strMES[4]);
	GetDlgItemText(IDC_EDIT6, strMES[5]);
	GetDlgItemText(IDC_EDIT7, strMES[6]);
	GetDlgItemText(IDC_EDIT8, strMES[7]);
	pSng->SetCfgString("MES", "deptID", strMES[0]);
	pSng->SetCfgString("MES", "partID", strMES[1]);
	pSng->SetCfgString("MES", "mo_id", strMES[2]);
	pSng->SetCfgString("MES", "lineID", strMES[3]);
	pSng->SetCfgString("MES", "test_Station", strMES[4]);
	pSng->SetCfgString("MES", "testResult", strMES[5]);
	pSng->SetCfgString("MES", "machineSN", strMES[6]);
	pSng->SetCfgString("MES", "testchannelID", strMES[7]);

	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
