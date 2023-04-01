// VAL.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDll.h"
#include "VAL.h"
#include "afxdialogex.h"


// VAL 对话框

IMPLEMENT_DYNAMIC(VAL, CDialogEx)

VAL::VAL(CWnd* pParent /*=NULL*/)
	: CDialogEx(VAL::IDD, pParent)
{

}

VAL::~VAL()
{
}

void VAL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VAL, CDialogEx)
	ON_BN_CLICKED(IDOK, &VAL::OnBnClickedOk)
END_MESSAGE_MAP()


// VAL 消息处理程序


BOOL VAL::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	CString strVAL[13];
	strVAL[0] = pSng->GetCfgString("VAL补偿", "工站1", "0.000");
	strVAL[1] = pSng->GetCfgString("VAL补偿", "工站2", "0.000");
	strVAL[2] = pSng->GetCfgString("VAL补偿", "工站3", "0.000");
	strVAL[3] = pSng->GetCfgString(pSng->strLiaohao, "注水量上限", "2.000");
	strVAL[4] = pSng->GetCfgString(pSng->strLiaohao, "注水量下限", "1.000");
	strVAL[5] = pSng->GetCfgString(pSng->strLiaohao, "抽出量上限", "2.000");
	strVAL[6] = pSng->GetCfgString(pSng->strLiaohao, "抽出量下限", "1.000");
	strVAL[7] = pSng->GetCfgString(pSng->strLiaohao, "封存量上限", "2.000");
	strVAL[8] = pSng->GetCfgString(pSng->strLiaohao, "封存量下限", "1.000");
	strVAL[9] = pSng->GetCfgString("参数", "扫码时长", "6000");
	strVAL[10] = pSng->GetCfgString("参数", "清零时长", "3000");
	strVAL[11] = pSng->GetCfgString("参数", "清零上限", "0.01");
	strVAL[12] = pSng->GetCfgString("参数", "清零下限", "-0.01");
	SetDlgItemText(IDC_EDIT1, strVAL[0]);
	SetDlgItemText(IDC_EDIT2, strVAL[1]);
	SetDlgItemText(IDC_EDIT3, strVAL[2]);
	SetDlgItemText(IDC_EDIT4, strVAL[3]);
	SetDlgItemText(IDC_EDIT5, strVAL[4]);
	SetDlgItemText(IDC_EDIT6, strVAL[5]);
	SetDlgItemText(IDC_EDIT7, strVAL[6]);
	SetDlgItemText(IDC_EDIT8, strVAL[7]);
	SetDlgItemText(IDC_EDIT9, strVAL[8]);
	SetDlgItemText(IDC_EDIT10, strVAL[9]);
	SetDlgItemText(IDC_EDIT11, strVAL[10]);
	SetDlgItemText(IDC_EDIT12, strVAL[11]);
	SetDlgItemText(IDC_EDIT13, strVAL[12]);
	return TRUE;  // return TRUE unless you set the focus to a control

}

void VAL::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CSingleton* pSng = CSingleton::GetInstance();
	CString strVALNew[13], strVAL[11];
	GetDlgItemText(IDC_EDIT1, strVALNew[0]);
	GetDlgItemText(IDC_EDIT2, strVALNew[1]);
	GetDlgItemText(IDC_EDIT3, strVALNew[2]);
	GetDlgItemText(IDC_EDIT4, strVALNew[3]);
	GetDlgItemText(IDC_EDIT5, strVALNew[4]);
	GetDlgItemText(IDC_EDIT6, strVALNew[5]);
	GetDlgItemText(IDC_EDIT7, strVALNew[6]);
	GetDlgItemText(IDC_EDIT8, strVALNew[7]);
	GetDlgItemText(IDC_EDIT9, strVALNew[8]);
	GetDlgItemText(IDC_EDIT10, strVALNew[9]);
	GetDlgItemText(IDC_EDIT11, strVALNew[10]);
	GetDlgItemText(IDC_EDIT12, strVALNew[11]);
	GetDlgItemText(IDC_EDIT13, strVALNew[12]);
	double fVal[11];
	fVal[0] = atof(strVALNew[0]);
	fVal[1] = atof(strVALNew[1]);
	fVal[2] = atof(strVALNew[2]);
	fVal[3] = atof(strVALNew[3]);
	fVal[4] = atof(strVALNew[4]);
	fVal[5] = atof(strVALNew[5]);
	fVal[6] = atof(strVALNew[6]);
	fVal[7] = atof(strVALNew[7]);
	fVal[8] = atof(strVALNew[8]);
	fVal[9] = atof(strVALNew[11]);
	fVal[10] = atof(strVALNew[12]);
	if (fVal[3]<fVal[4])
	{
		fVal[3] = fVal[4];
	}
	if (fVal[5] < fVal[6])
	{
		fVal[5] = fVal[6];
	}
	if (fVal[7] < fVal[8])
	{
		fVal[7] = fVal[8];
	}
	if (fVal[9] < fVal[10])
	{
		fVal[9] = fVal[10];
	}
	strVAL[0].Format("%.3f", fVal[0]);
	strVAL[1].Format("%.3f", fVal[1]);
	strVAL[2].Format("%.3f", fVal[2]);
	strVAL[3].Format("%.3f", fVal[3]);
	strVAL[4].Format("%.3f", fVal[4]);
	strVAL[5].Format("%.3f", fVal[5]);
	strVAL[6].Format("%.3f", fVal[6]);
	strVAL[7].Format("%.3f", fVal[7]);
	strVAL[8].Format("%.3f", fVal[8]);
	strVAL[9].Format("%.3f", fVal[9]);
	strVAL[10].Format("%.3f", fVal[10]);
	pSng->SetCfgString("VAL补偿", "工站1", strVAL[0]);
	pSng->SetCfgString("VAL补偿", "工站2", strVAL[1]);
	pSng->SetCfgString("VAL补偿", "工站3", strVAL[2]);
	pSng->SetCfgString(pSng->strLiaohao, "注水量上限", strVAL[3]);
	pSng->SetCfgString(pSng->strLiaohao, "注水量下限", strVAL[4]);
	pSng->SetCfgString(pSng->strLiaohao, "抽出量上限", strVAL[5]);
	pSng->SetCfgString(pSng->strLiaohao, "抽出量下限", strVAL[6]);
	pSng->SetCfgString(pSng->strLiaohao, "封存量上限", strVAL[7]);
	pSng->SetCfgString(pSng->strLiaohao, "封存量下限", strVAL[8]);
	pSng->SetCfgString("参数", "扫码时长", strVALNew[9]);
	pSng->SetCfgString("参数", "清零时长", strVALNew[10]);
	pSng->SetCfgString("参数", "清零上限", strVAL[9]);
	pSng->SetCfgString("参数", "清零下限", strVAL[10]);
	CDialogEx::OnOK();
}
