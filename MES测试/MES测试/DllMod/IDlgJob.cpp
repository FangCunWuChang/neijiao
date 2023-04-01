// IDlgJob.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "IDlgJob.h"
#include "afxdialogex.h"

extern void LogFile(CString log);
extern void LogFile(const char *fmt, ...);
// CIDlgJob 对话框

IMPLEMENT_DYNAMIC(CIDlgJob, CDialogEx)

CIDlgJob::CIDlgJob(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIDlgJob::IDD, pParent)
{
	m_strCam.Empty();
}

CIDlgJob::~CIDlgJob()
{
}

void CIDlgJob::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_JOB, m_tree);
	DDX_Control(pDX, IDC_COMBO_THREAD, m_comboThread);
}


BEGIN_MESSAGE_MAP(CIDlgJob, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &CIDlgJob::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_BTN_ADD_KNOT, &CIDlgJob::OnBnClickedBtnAddKnot)
	ON_BN_CLICKED(IDC_RADIO2, &CIDlgJob::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CIDlgJob::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_BTN_SAV_THREAD, &CIDlgJob::OnBnClickedBtnSavThread)
	ON_BN_CLICKED(IDOK, &CIDlgJob::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_THREAD, &CIDlgJob::OnCbnSelchangeComboThread)
END_MESSAGE_MAP()


// CIDlgJob 消息处理程序

extern "C" __declspec(dllexport) CIDlgJob* SetThread(CString strCam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSingleton* pSng = CSingleton::GetInstance();
	CIDlgJob *ph = new CIDlgJob;
	ph->m_strCam.Format("%s", strCam);
	ph->Create(CIDlgJob::IDD);
	ph->ShowWindow(SW_SHOW);
	pSng->_pJob = ph;
	return ph;
}


BOOL CIDlgJob::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSingleton* pSng = CSingleton::GetInstance();
	if (m_strCam.GetLength() > 0)
	{
		m_hRoot = m_tree.InsertItem( m_strCam + "线程", 1, 0, TVI_ROOT);              //添加一级结点
		m_comboThread.EnableWindow(FALSE);
	}
	else
	{
		m_hRoot = m_tree.InsertItem("监控线程", 1, 0, TVI_ROOT);              //添加一级结点
	}
	m_tree.SetTextColor(RGB(0, 0, 255));                                                   //设置文本颜色

	CString strWinTxt;
	GetWindowText(strWinTxt);
	SetWindowText(m_strCam + " " + strWinTxt);
	OnBnClickedRadio1();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO1);
	pBtn->SetCheck(BST_CHECKED);

	if (m_strCam.GetLength() == 0)
	{
		m_comboThread.AddString("监控线程");
		m_comboThread.SetCurSel(0);
	}
	
	pSng->_pJob = this;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CIDlgJob::OnBnClickedBtnAddKnot()
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nID[20] = { IDC_RADIO1, IDC_RADIO2, IDC_RADIO3 };
	CString strText;
	GetDlgItemText(nID[m_nRadio],strText);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	HTREEITEM hChild = m_tree.InsertItem(strText, m_hRoot);                                 //添加三级结点
	m_tree.Expand(m_hRoot, TVE_EXPAND);
	CStringArray sKeys;
	pSng->GetSecKeys(strText, sKeys);
	for (int i = 0; i < sKeys.GetCount(); i++)
	{
		CString strVal = pSng->GetCfgString(strText, sKeys[i], NULL);
		CStringArray strs;
		pSng->SplitString(strVal, ",", strs);
		if (strs.GetCount() > 1)
		{
			m_tree.InsertItem(sKeys[i] + "=" + strs[0], hChild);   //默认第一个方法
		}
		else
		{
			m_tree.InsertItem(sKeys[i] + "=" + strVal, hChild);
		}
	}
	m_tree.Expand(hChild, TVE_EXPAND);
	m_tree.Invalidate();
}

void CIDlgJob::OnBnClickedRadio1()
{
	m_nRadio = 0;
}


void CIDlgJob::OnBnClickedRadio2()
{
	m_nRadio = 1;
}


void CIDlgJob::OnBnClickedRadio3()
{
	m_nRadio = 2;
}


void CIDlgJob::OnBnClickedBtnSavThread()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CIDlgJob::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDialogEx::OnOK();
	delete pSng->_pJob;
	pSng->_pJob = NULL;
	TRACE("\n DLL工作流程窗口退出完毕！");
}


void CIDlgJob::OnCbnSelchangeComboThread()
{
	m_tree.DeleteAllItems();
	if (m_strCam.GetLength() > 0)
	{
		m_hRoot = m_tree.InsertItem(m_strCam + "线程", 1, 0, TVI_ROOT);              //添加一级结点
		m_comboThread.EnableWindow(FALSE);
	}
	else
	{
		CString strThread;
		m_comboThread.GetLBText(m_comboThread.GetCurSel(), strThread);
		m_hRoot = m_tree.InsertItem(strThread, 1, 0, TVI_ROOT);              //添加一级结点
	}
}
