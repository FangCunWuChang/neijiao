// DlgOffset.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDll.h"
#include "DlgOffset.h"
#include "afxdialogex.h"
#include "..\DllMod\Singleton.h"


// CDlgOffset �Ի���

IMPLEMENT_DYNAMIC(CDlgOffset, CDialogEx)

CDlgOffset::CDlgOffset(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOffset::IDD, pParent)
{
	m_nPos = 5;
}

CDlgOffset::~CDlgOffset()
{
}

void CDlgOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_POS, m_comboPos);
}


BEGIN_MESSAGE_MAP(CDlgOffset, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_POS, &CDlgOffset::OnCbnSelchangeComboPos)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgOffset::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgOffset::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BTN_SAV, &CDlgOffset::OnBnClickedBtnSav)
END_MESSAGE_MAP()


// CDlgOffset ��Ϣ�������

void CDlgOffset::SetRadioText(CString strText,int nID)
{
	if (nID != IDC_RADIO1 || nID != IDC_RADIO2)
		return;
	SetDlgItemText(nID, strText);
}


BOOL CDlgOffset::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	for (int i = 0; i < m_nPos;i++)
	{
		CString strPos;
		if (m_nPos > 2)
			strPos.Format("ȡ��λ%d", i + 1);
		else
		{
			if (i == 0)
				strPos.Format("��λ");
			else
				strPos.Format("�ҹ�λ");
		}
		m_comboPos.AddString(strPos);
	}
	m_comboPos.SetCurSel(0);
	OnBnClickedRadio1();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO1);
	pBtn->SetCheck(BST_CHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CDlgOffset::OnCbnSelchangeComboPos()
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nSel = m_comboPos.GetCurSel();
	CString strSec;
	strSec.Format("R%d%d����", nSel + 1, m_nCam + 1);
	CString strX = pSng->GetCfgString(strSec, "X", "");
	SetDlgItemText(IDC_EDIT_X, strX);
	CString strY = pSng->GetCfgString(strSec, "Y", "");
	SetDlgItemText(IDC_EDIT_Y, strY);
	CString strA = pSng->GetCfgString(strSec, "A", "");
	SetDlgItemText(IDC_EDIT_A, strA);
}


void CDlgOffset::OnBnClickedRadio1()
{
	m_nCam = 1;
	strRot.Format("��ת180���ϲ����޸�");
	OnCbnSelchangeComboPos();
}


void CDlgOffset::OnBnClickedRadio2()
{
	m_nCam = 0;
	strRot.Format("����ת�ϲ����޸�");
	OnCbnSelchangeComboPos();
}


void CDlgOffset::OnBnClickedBtnSav()
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nSel = m_comboPos.GetCurSel();
	CString strSec,strPos;
	strSec.Format("R%d%d����", nSel + 1, m_nCam + 1);
	CString strX, strY, strA;
	GetDlgItemText(IDC_EDIT_X, strX);
	GetDlgItemText(IDC_EDIT_Y, strY);
	GetDlgItemText(IDC_EDIT_A, strA);
	strPos.Format("ȡ�Ϲ�λ%d ", nSel + 1);
	if (IDYES == MessageBox(strPos +  strRot, strSec, MB_YESNO))
	{
		pSng->SetCfgString(strSec, "X", strX);
		pSng->SetCfgString(strSec, "Y", strY);
		pSng->SetCfgString(strSec, "A", strA);
		MessageBox(strSec + " ����ɹ���");
	}
}
