// DlgCom.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DllMod.h"
#include "DlgCom.h"
#include "afxdialogex.h"


// CDlgCom �Ի���

IMPLEMENT_DYNAMIC(CDlgCom, CDialogEx)

CDlgCom::CDlgCom(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCom::IDD, pParent)
{

}

CDlgCom::~CDlgCom()
{
}

void CDlgCom::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PAR, m_comboPar);
}


BEGIN_MESSAGE_MAP(CDlgCom, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgCom::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BTN_SAV, &CDlgCom::OnBnClickedBtnSav)
	ON_CBN_SELCHANGE(IDC_COMBO_PAR, &CDlgCom::OnCbnSelchangeComboPar)
	ON_BN_CLICKED(IDOK, &CDlgCom::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCom ��Ϣ�������


BOOL CDlgCom::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_comboPar.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_SAV)->ShowWindow(SW_HIDE);
	for (int i = 0; i < (int)m_comboVals.size();i++)
	{	
		m_comboPar.AddString(m_comboVals[i].strNick);
		if (i == (int)m_comboVals.size() - 1)
		{
			m_comboPar.ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_SAV)->ShowWindow(SW_SHOW);
			m_comboPar.SetCurSel(0);
			OnCbnSelchangeComboPar();
		}
	}
	SetWindowText(strWin);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CDlgCom::OnEnChangeEdit1()
{
	GetDlgItemText(IDC_EDIT1, strEdit);
}


void CDlgCom::OnBnClickedBtnSav()
{
	int nSel = m_comboPar.GetCurSel();
	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	if (m_comboVals[nSel].strNick == "�ع�")
	{
		m_comboVals[nSel].x = max(100.0,atof(str));
	}
	else if (m_comboVals[nSel].strNick == "����")
	{
		m_comboVals[nSel].x = min(20.0,max(1.0, atof(str)));
	}
	else if (m_comboVals[nSel].strNick == "٤��")
	{
		m_comboVals[nSel].x = min(3.0, max(0.2, atof(str)));
	}
}


void CDlgCom::OnCbnSelchangeComboPar()
{
	int nSel = m_comboPar.GetCurSel();
	CString strText = m_comboVals[nSel].strNick;
	strText.Format("%.1f", m_comboVals[nSel].x);
	SetDlgItemText(IDC_EDIT1, strText);
}


void CDlgCom::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
