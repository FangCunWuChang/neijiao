// IDlgCamCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "IDlgCamCfg.h"
#include "afxdialogex.h"

// CIDlgCamCfg 对话框

IMPLEMENT_DYNAMIC(CIDlgCamCfg, CDialogEx)

CIDlgCamCfg::CIDlgCamCfg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIDlgCamCfg::IDD, pParent)
{

}

CIDlgCamCfg::~CIDlgCamCfg()
{

}

void CIDlgCamCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);
	DDX_Control(pDX, IDC_COMBO_CAM_SEL, m_comboCam);
	DDX_Control(pDX, IDC_SLIDER_PAR, m_sldPar);
}


BEGIN_MESSAGE_MAP(CIDlgCamCfg, CDialogEx)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PAR, &CIDlgCamCfg::OnNMReleasedcaptureSliderPar)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CIDlgCamCfg::OnCbnSelchangeComboType)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_SEL, &CIDlgCamCfg::OnCbnSelchangeComboCamSel)
	ON_BN_CLICKED(IDOK, &CIDlgCamCfg::OnBnClickedOk)
END_MESSAGE_MAP()


// CIDlgCamCfg 消息处理程序

extern "C" __declspec(dllexport) CIDlgCamCfg* GetDlgCamCfg(int nCam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_bINICam == true)
	{
		CIDlgCamCfg *ph = new CIDlgCamCfg;
		ph->Create(CIDlgCamCfg::IDD);
		ph->ShowWindow(SW_SHOW);
		ph->m_comboCam.SetCurSel(nCam);
		ph->OnCbnSelchangeComboCamSel();
		pSng->_pCamCfg = ph;
		return ph;
	}
	return NULL;
}

extern "C" __declspec(dllexport) CIDlgCamCfg* GetCamCfgPtr()
{
	CSingleton* pSng = CSingleton::GetInstance();
	return (CIDlgCamCfg*)pSng->_pCamCfg;
}

BOOL CIDlgCamCfg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	m_comboType.AddString("曝光");
	m_comboType.AddString("增益");
	m_comboType.AddString("伽马");
	m_comboType.SetCurSel(0);
	for (int i = 0; i < pSng->_nCamThread; i++)
	{
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		m_comboCam.AddString(strCam);
		m_comboCam.SetCurSel(0);
		pSng->_iReal[i] = 1;
	}
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO4);
	pBtn->SetCheck(BST_CHECKED);
	m_HW.InitWnd(GetDlgItem(IDC_STATIC_IMG));
	OnCbnSelchangeComboType();
	SetTimer(TIM_REAL, 900, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CIDlgCamCfg::InitSlider()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strType, strCam, strVal;
	m_comboType.GetLBText(m_comboType.GetCurSel(), strType);
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);
	if (m_comboType.GetCurSel() == 0)
	{
		m_sldPar.SetRange(100, 20000);
		m_fSldCoef = 1.0;
		strVal = pSng->GetCfgString(strType, strCam, "5000");
	}
	else if (m_comboType.GetCurSel() == 1)
	{
		m_sldPar.SetRange(10, 200);
		m_fSldCoef = 0.1;
		strVal = pSng->GetCfgString(strType,strCam,  "5");
	}
	else if (m_comboType.GetCurSel() == 2)
	{
		m_sldPar.SetRange(2, 30);
		m_fSldCoef = 0.1;
		strVal = pSng->GetCfgString(strType,strCam, "1");
	}
	double fVal = atof(strVal) / m_fSldCoef;
	m_sldPar.SetPos((int)fVal);
	SetDlgItemText(IDC_STATIC_VAL, strVal);
}

void CIDlgCamCfg::OnCbnSelchangeComboType()
{
	InitSlider();
}

void CIDlgCamCfg::OnNMReleasedcaptureSliderPar(NMHDR *pNMHDR, LRESULT *pResult)
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nCam = m_comboCam.GetCurSel(), nRet;
	CString strType, strVal, strCam;
	m_comboType.GetLBText(m_comboType.GetCurSel(), strType);
	float fVal = (float)(m_sldPar.GetPos()*m_fSldCoef);
	strVal.Format("%.1f", fVal);
	if (m_comboType.GetCurSel() == 0)
	{
		strVal.Format("%d", m_sldPar.GetPos());
		nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "ExposureTime", fVal);
	}
	else if (m_comboType.GetCurSel() == 1)
	{
		nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gain", fVal);
	}
	else if (m_comboType.GetCurSel() == 2)
	{
		nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gamma", fVal);
	}
	m_comboCam.GetLBText(nCam, strCam);
	pSng->SetCfgString(strType, strCam, strVal);
	SetDlgItemText(IDC_STATIC_VAL, strVal);
	*pResult = 0;
}


void CIDlgCamCfg::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (nIDEvent == TIM_REAL)
	{
		KillTimer(TIM_REAL);
		int nCam = m_comboCam.GetCurSel();
		if (pSng->_iReal[nCam] > 1)
		{
			pSng->_csCam[nCam].Lock();
			//CopyImage(pSng->_hoImg[nCam], &m_HW.hoImage);
			//WriteImage(m_HW.hoImage, "jpg", 0, "D:\\Test")
			m_HW.ShowImage(pSng->_hoImg[nCam],true);;
			m_HW.RealSize();
			SetColor(m_HW.WndID, "blue");
			DispCross(m_HW.WndID, m_HW.psRD.y / 2, m_HW.psRD.x / 2, 6000, 0);
			pSng->_csCam[nCam].Unlock();
		}
		SetTimer(TIM_REAL, 300, NULL);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CIDlgCamCfg::OnOK()
{

}


void CIDlgCamCfg::OnCbnSelchangeComboCamSel()
{
	//要载入相机的尺寸
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam,strSize;
	strCam.Format("CAM%d", m_comboCam.GetCurSel() + 1);
	pSng->GetCfgString("相机尺寸", strCam, strSize);
	CStrs XYs;
	pSng->SplitString(strSize, ",",XYs);

	InitSlider();
}


void CIDlgCamCfg::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	KillTimer(TIM_REAL);
	for (int i = 0; i < pSng->_nCamThread; i++)
	{
		pSng->_iReal[i] = 0;
	}
	Sleep(500);
	CDialogEx::OnOK();
	delete pSng->_pCamCfg;
	pSng->_pCamCfg = NULL;
}
