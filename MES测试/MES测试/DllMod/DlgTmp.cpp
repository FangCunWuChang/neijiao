// DlgTmp.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "DlgTmp.h"
#include "afxdialogex.h"
#include "Singleton.h"


// CDlgTmp 对话框

IMPLEMENT_DYNAMIC(CDlgTmp, CDialogEx)

CDlgTmp::CDlgTmp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTmp::IDD, pParent)
{

}

CDlgTmp::~CDlgTmp()
{
}

void CDlgTmp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MOD, m_comboMod);
	DDX_Control(pDX, IDC_SLIDER_BIN, m_sldBinVal);
}


BEGIN_MESSAGE_MAP(CDlgTmp, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OFFSET, &CDlgTmp::OnBnClickedBtnOffset)
	//ON_EN_CHANGE(IDC_EDIT_SCORE, &CDlgTmp::OnEnChangeEditScore)
	ON_EN_CHANGE(IDC_EDIT_NAME, &CDlgTmp::OnEnChangeEditName)
	ON_CBN_SELCHANGE(IDC_COMBO_MOD, &CDlgTmp::OnCbnSelchangeComboMod)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BIN, &CDlgTmp::OnNMReleasedcaptureSliderBin)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgTmp::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgTmp::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CDlgTmp 消息处理程序


BOOL CDlgTmp::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemInt(IDC_EDIT_SCORE, 60);
	m_comboMod.AddString("NCC模板");
	m_comboMod.AddString("Shape模板");
	m_comboMod.SetCurSel(0);
	m_nTmpTYP = TMP_NCC;
	m_sldBinVal.SetRange(1, 254);
	m_sldBinVal.SetPos(128);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO1);
	pBtn->SetCheck(BST_CHECKED);
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_FILL);
	pBtn->SetCheck(BST_CHECKED);
	OnCbnSelchangeComboMod();

	SetDlgItemInt(IDC_STATIC_DX, 0);
	SetDlgItemInt(IDC_STATIC_DY, 0);
	strOffset.Format("0,0");
	ZeroMemory(m_fDelta, sizeof(m_fDelta));

	HTuple hvW, hvH;
	GetImageSize(hoTMP, &hvW, &hvH);                                                   //得到图像的宽度和高度
	CRect   rc_Pic;
	GetDlgItem(IDC_STATIC_IMG)->GetClientRect(&rc_Pic);                //ID1为第一个图像控件的ID号，第二个窗口类似处理。  
	HWND hImgWnd = GetDlgItem(IDC_STATIC_IMG)->m_hWnd;
	Hlong MainWndID = (Hlong)hImgWnd;
	HTuple m_htWindow;
	//HDevWindowStack::Push(m_WndID);
	double fWCoe = hvW.D() / rc_Pic.Width();
	double fHCoe = hvH.D() / rc_Pic.Height();
	if (fWCoe > fHCoe)
	{
		OpenWindow(rc_Pic.left, rc_Pic.top, rc_Pic.Width(), rc_Pic.Height()*fHCoe / fWCoe, MainWndID, "", "", &m_WndID);
	}
	else
	{
		OpenWindow(rc_Pic.left, rc_Pic.top, rc_Pic.Width()*fWCoe / fHCoe, rc_Pic.Height(), MainWndID, "", "", &m_WndID);
	}
	SetPart(m_WndID, 0, 0, hvH, hvW);
	SetLineWidth(m_WndID, 2);
	DispObj(hoTMP, m_WndID);
	SetColor(m_WndID, "gold");
	//DispCross(m_WndID, 0.5*hvW, hvH*0.5, 96, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDlgTmp::OnBnClickedBtnOffset()
{
	HTuple hvW, hvH;
	GetImageSize(hoTMP, &hvW, &hvH);                                                   //得到图像的宽度和高度
	HTuple hvR, hvC;
	DrawPoint(m_WndID, &hvR, &hvC);
	double fDX = hvC.D() - 0.5*hvW.D();
	double fDY = hvR.D() - 0.5*hvH.D();
	strOffset.Format("%.1f,%.1f",fDX / ZOOM,fDY / ZOOM);
	DispCross(m_WndID, hvR, hvC, 60, 0);
}

void CDlgTmp::OnEnChangeEditName()
{
	GetDlgItemText(IDC_EDIT_NAME, strTmpName);
}

void CDlgTmp::OnCbnSelchangeComboMod()
{
	int nSel = m_comboMod.GetCurSel();
	if (nSel == TMP_NCC)
	{
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_BIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AREA)->EnableWindow(FALSE);
	}
	else if (nSel == TMP_SHM)
	{
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_BIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FILL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AREA)->EnableWindow(TRUE);
	}
	m_nTmpTYP = nSel;
}


void CDlgTmp::OnNMReleasedcaptureSliderBin(NMHDR *pNMHDR, LRESULT *pResult)
{
	HObject hoBin, hoCons,hoRegs,hoFill;
	int nPos = m_sldBinVal.GetPos();
	if (m_nBlack)
	{	
		Threshold(hoTMP, &hoBin, 0,m_nBlack);
	}
	else
	{
		Threshold(hoTMP, &hoBin, m_nBlack,255);
	}
	CButton* pCHK = (CButton*)GetDlgItem(IDC_CHECK_FILL);
	if (pCHK->GetCheck() == BST_CHECKED)
	{
		FillUp(hoBin, &hoFill);
		HalconCpp::Connection(hoFill, &hoCons);
	}
	else
		HalconCpp::Connection(hoBin, &hoCons);
	CString strArea;
	GetDlgItemText(IDC_EDIT_AREA, strArea);
	double fMinArea = max(10.0, atof(strArea));
	SelectShape(hoCons, &hoRegs, "area", "and", fMinArea, 99999999);  //筛选得到最大最小轮廓
	GenContourRegionXld(hoRegs,&hoXLD,"border");
	DispObj(hoXLD, m_WndID);
	*pResult = 0;
}

void CDlgTmp::OnBnClickedRadio1()
{
	m_nBlack = 1;
}

void CDlgTmp::OnBnClickedRadio2()
{
	m_nBlack = 0;
}
