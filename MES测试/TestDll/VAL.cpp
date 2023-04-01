// VAL.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDll.h"
#include "VAL.h"
#include "afxdialogex.h"


// VAL �Ի���

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


// VAL ��Ϣ�������


BOOL VAL::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	CString strVAL[13];
	strVAL[0] = pSng->GetCfgString("VAL����", "��վ1", "0.000");
	strVAL[1] = pSng->GetCfgString("VAL����", "��վ2", "0.000");
	strVAL[2] = pSng->GetCfgString("VAL����", "��վ3", "0.000");
	strVAL[3] = pSng->GetCfgString(pSng->strLiaohao, "עˮ������", "2.000");
	strVAL[4] = pSng->GetCfgString(pSng->strLiaohao, "עˮ������", "1.000");
	strVAL[5] = pSng->GetCfgString(pSng->strLiaohao, "���������", "2.000");
	strVAL[6] = pSng->GetCfgString(pSng->strLiaohao, "���������", "1.000");
	strVAL[7] = pSng->GetCfgString(pSng->strLiaohao, "���������", "2.000");
	strVAL[8] = pSng->GetCfgString(pSng->strLiaohao, "���������", "1.000");
	strVAL[9] = pSng->GetCfgString("����", "ɨ��ʱ��", "6000");
	strVAL[10] = pSng->GetCfgString("����", "����ʱ��", "3000");
	strVAL[11] = pSng->GetCfgString("����", "��������", "0.01");
	strVAL[12] = pSng->GetCfgString("����", "��������", "-0.01");
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	pSng->SetCfgString("VAL����", "��վ1", strVAL[0]);
	pSng->SetCfgString("VAL����", "��վ2", strVAL[1]);
	pSng->SetCfgString("VAL����", "��վ3", strVAL[2]);
	pSng->SetCfgString(pSng->strLiaohao, "עˮ������", strVAL[3]);
	pSng->SetCfgString(pSng->strLiaohao, "עˮ������", strVAL[4]);
	pSng->SetCfgString(pSng->strLiaohao, "���������", strVAL[5]);
	pSng->SetCfgString(pSng->strLiaohao, "���������", strVAL[6]);
	pSng->SetCfgString(pSng->strLiaohao, "���������", strVAL[7]);
	pSng->SetCfgString(pSng->strLiaohao, "���������", strVAL[8]);
	pSng->SetCfgString("����", "ɨ��ʱ��", strVALNew[9]);
	pSng->SetCfgString("����", "����ʱ��", strVALNew[10]);
	pSng->SetCfgString("����", "��������", strVAL[9]);
	pSng->SetCfgString("����", "��������", strVAL[10]);
	CDialogEx::OnOK();
}
