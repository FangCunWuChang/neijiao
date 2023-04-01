// DlgItem.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "DlgItem.h"
#include "afxdialogex.h"

// CDlgItem 对话框

IMPLEMENT_DYNAMIC(CDlgItem, CDialogEx)

CDlgItem::CDlgItem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgItem::IDD, pParent)
{

}

CDlgItem::~CDlgItem()
{
}

void CDlgItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NODE, m_listNode);
	DDX_Control(pDX, IDC_COMBO_TOL, m_comboTol);
}


BEGIN_MESSAGE_MAP(CDlgItem, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_NODE, &CDlgItem::OnNMDblclkListNode)
	ON_BN_CLICKED(IDOK, &CDlgItem::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TOL, &CDlgItem::OnCbnSelchangeComboTol)
	ON_BN_CLICKED(IDC_BTN_DEL_TOL, &CDlgItem::OnBnClickedBtnDelTol)
	ON_BN_CLICKED(IDC_BTN_ADD_TOL, &CDlgItem::OnBnClickedBtnAddTol)
//	ON_BN_CLICKED(IDC_BTN_SAV_TOL, &CDlgItem::OnBnClickedBtnSavTol)
ON_BN_CLICKED(IDC_BTN_SAV_TOL, &CDlgItem::OnBnClickedBtnSavTol)
END_MESSAGE_MAP()


// CDlgItem 消息处理程序


BOOL CDlgItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSingleton* pSng = CSingleton::GetInstance();
	SetWindowText(m_strNick);

	pSng->InitList(m_listNode, sts);
	int u = 0;
	for (int i = 0; i < (int)psLeaf.size(); i++)
	{
		m_listNode.InsertItem(i, psLeaf[i].strNick);
		m_listNode.SetItemText(i, 1, psLeaf[i].strMN);
		CStrs ss;
		CStringArray sKeys;
		CString strSeg = pSng->GetCfgString(m_strNick, psLeaf[i].strNick, "");
		pSng->SplitString(strSeg, ";", ss);
		if (ss.size() == 1)
		{
			m_listNode.SetReWorkCol(1, 1, IDC_EDIT);
			continue;
		}
	}
	GetDlgItem(IDC_EDIT_MAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DEL_TOL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ADD_TOL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAV_TOL)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDlgItem::OnNMDblclkListNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


BOOL CDlgItem::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		//根据选择的x，y 决定是否弹出combox	
		CSingleton* pSng = CSingleton::GetInstance();
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		m_listNode.ScreenToClient(&point);
		LVHITTESTINFO lvinfo;
		lvinfo.pt = point;
		lvinfo.flags = LVHT_ABOVE;
		int nItem = m_listNode.SubItemHitTest(&lvinfo);
		if (nItem != -1)
		{
			CString strtemp;
			strtemp.Format(_T("单击的是第%d行第%d列"), lvinfo.iItem, lvinfo.iSubItem);
			CStrs ss;
			CString strNode =  m_listNode.GetItemText(lvinfo.iItem, 0);
			CString strSeg = pSng->GetCfgString(m_strNick, strNode, "");
			pSng->SplitString(strSeg, ";", ss);
			int nS = ss.size();
			GetDlgItem(IDC_EDIT_MAX)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_MIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_DEL_TOL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_ADD_TOL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_SAV_TOL)->EnableWindow(FALSE);
			if (nS == 1)
				return FALSE;
			if (strNode == "公差")
			{
				GetDlgItem(IDC_EDIT_MAX)->EnableWindow(TRUE);
				GetDlgItem(IDC_EDIT_MIN)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_DEL_TOL)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_ADD_TOL)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_SAV_TOL)->EnableWindow(TRUE);
				m_comboTol.ResetContent();
				for (int i = 0; i < nS; i++)
				{
					m_comboTol.AddString(ss[i]);
				}
				m_comboTol.SetCurSel(0);
				OnCbnSelchangeComboTol();
			}
			else
			{
				m_listNode.ClearCombo();
				for (int i = 0; i < nS; i++)
				{
					m_listNode.AddString(ss[i]);
				}
				m_listNode.SetReWorkCol(1, 1, IDC_COMBOX);
				GetDlgItem(IDC_EDIT_MAX)->EnableWindow(FALSE);
				GetDlgItem(IDC_EDIT_MIN)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_DEL_TOL)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_ADD_TOL)->EnableWindow(FALSE);
				m_comboTol.ResetContent();
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgItem::OnBnClickedOk()
{
	for (int i = 0; i < m_listNode.GetItemCount(); i++)
	{
		psLeaf[i].strMN.Format("%s",m_listNode.GetItemText(i, 1));
	}
	CDialogEx::OnOK();
}


void CDlgItem::OnCbnSelchangeComboTol()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	m_comboTol.GetLBText(m_comboTol.GetCurSel(), str);
	CStrs ss;
	pSng->SplitString(str, ",", ss);
	SetDlgItemText(IDC_EDIT_MIN, ss[0]);
	SetDlgItemText(IDC_EDIT_MAX, ss[1]);
}


void CDlgItem::OnBnClickedBtnDelTol()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	int nSel = m_comboTol.GetCurSel();
	CStrs ss;
	for (int i = 0; i < m_comboTol.GetCount();i++)
	{
		CString strVal;
		if (i == nSel)
		{
			ss.push_back("_,_");
			continue;
		}
		m_comboTol.GetLBText(i, strVal);
		ss.push_back(strVal);
	}
	m_comboTol.ResetContent();
	for (int i = 0; i < (int)ss.size();i++)
	{
		m_comboTol.AddString(ss[i]);
	}
	m_comboTol.SetCurSel(nSel);
	OnCbnSelchangeComboTol();
}


void CDlgItem::OnBnClickedBtnAddTol()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	int nSel = m_comboTol.GetCount();
	m_comboTol.AddString("0,10000");
	m_comboTol.SetCurSel(nSel);
	OnCbnSelchangeComboTol();
}


void CDlgItem::OnBnClickedBtnSavTol()
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nSel = m_comboTol.GetCurSel();
	CStrs ss;
	for (int i = 0; i < m_comboTol.GetCount(); i++)
	{
		CString strVal;
		if (i == nSel)
		{
			CString strMin, strMax;
			GetDlgItemText(IDC_EDIT_MIN, strMin);
			GetDlgItemText(IDC_EDIT_MAX, strMax);
			ss.push_back(strMin + "," + strMax);
			continue;
		}
		m_comboTol.GetLBText(i, strVal);
		ss.push_back(strVal);
	}
	m_comboTol.ResetContent();
	for (int i = 0; i < (int)ss.size(); i++)
	{
		m_comboTol.AddString(ss[i]);
	}
	m_comboTol.SetCurSel(nSel);
	OnCbnSelchangeComboTol();
}
