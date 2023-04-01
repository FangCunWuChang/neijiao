// DlgCam4.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDll.h"
#include "DlgCam4.h"
#include "afxdialogex.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "..\DllMod\IDlgCamCfg.h"
#include "..\DllMod\TCPSocket.h"
#include "DlgOffset.h"
#include "ImgDLL.h"

extern void DoEvent();
extern int g_nExt;

extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);

// CDlgCam4 �Ի���
UINT ThreadCalib(LPVOID p);
UINT ThreadCam(LPVOID p);
//UINT ThreadWatch(LPVOID p);

IMPLEMENT_DYNAMIC(CDlgCam4, CDialogEx)

CDlgCam4::CDlgCam4(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCam4::IDD, pParent)
{
}

CDlgCam4::~CDlgCam4()
{
	
}

void CDlgCam4::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM, m_comboCam);
	DDX_Control(pDX, IDC_STATIC_PLC, m_lbIP[0]);
	DDX_Control(pDX, IDC_STATIC_RBT, m_lbIP[1]);
	DDX_Control(pDX, IDC_COMBO_POS, m_comboPos);
}


BEGIN_MESSAGE_MAP(CDlgCam4, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgCam4::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_LOG, &CDlgCam4::OnBnClickedBtnLog)
	ON_BN_CLICKED(IDC_BTN_IMAGE, &CDlgCam4::OnBnClickedBtnImage)
	ON_BN_CLICKED(IDC_BTN_CALIB, &CDlgCam4::OnBnClickedBtnCalib)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_TRIG, &CDlgCam4::OnBnClickedBtnTrig)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM, &CDlgCam4::OnCbnSelchangeComboCam)
	ON_BN_CLICKED(IDC_BTN_CAM, &CDlgCam4::OnBnClickedBtnCam)
	ON_BN_CLICKED(IDC_BTN_RUN, &CDlgCam4::OnBnClickedBtnRun)
ON_BN_CLICKED(IDC_BTN_POS, &CDlgCam4::OnBnClickedBtnPos)
ON_WM_SYSCOMMAND()
ON_BN_CLICKED(IDC_BTN_OFFSET, &CDlgCam4::OnBnClickedBtnOffset)
ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgCam4 ��Ϣ�������


BOOL CDlgCam4::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSingleton* pSng = CSingleton::GetInstance();
	m_nThread = 0;
	m_nCALI = 0;
	if (CImgDLL::Init() == FALSE)
	{
		MessageBox("����DLLʧ��!");
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	pSng->CreateDir("D:\\INI");
	pSng->CreateDir("D:\\TMP");
	pSng->CreateDir("D:\\CALI");
	pSng->CreateDir("D:\\Image");
	pSng->CreateDir("D:\\XML");
	vector<int> nIDs;
	nIDs.push_back(IDC_STATIC1);
	nIDs.push_back(IDC_STATIC2);
	nIDs.push_back(IDC_STATIC3);
	nIDs.push_back(IDC_STATIC4);
	for (int i = 0; i < 5;i++)
	{
		CString strPos;
		strPos.Format("ȡ��λ%d", i + 1);
		m_comboPos.AddString(strPos);
	}
	m_comboPos.SetCurSel(0);
	CImgDLL::WriteLog("������Ի����ʼ���ɹ���");
	//AfxBeginThread(ThreadWatch, this);
	for (int i = 0; i < 2;i++)
	{
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		m_comboCam.AddString(strCam);
		m_HW[i].psRD.x = 5472;
		m_HW[i].psRD.y = 3648;
		m_HW[i].InitWnd(GetDlgItem(nIDs[i]));
		SetColor(m_HW[i].WndID, "green");
		SetLineWidth(m_HW[i].WndID, 1);
		int nPosTxt[2] = { 0 };
		m_HW[i].ShowImage(nPosTxt,strCam);
		//continue;   //�ǵ�ȡ��
		if (i != 1)
		{
			bool bOK = CImgDLL::InitCam(i);
			if (bOK == false)
			{
				CImgDLL::WriteLog(_T("���%d��ʼ��ʧ��"), i + 1);
				MessageBox("�����ʼ��ʧ�ܣ�");
				SendMessage(WM_CLOSE);
				return FALSE;
			}
			bOK = CImgDLL::SetHeartTime(i, 2000);
		}
		AfxBeginThread(ThreadCam, this,THREAD_PRIORITY_HIGHEST);
		while (i == m_nThread)
		{
			DoEvent();
		}
		if (i < 2)
		{
			AfxBeginThread(ThreadCalib, this);
			while (i == m_nCALI)
			{
				DoEvent();
			}
		}
	}
	m_comboCam.SetCurSel(0);
	if (CImgDLL::SvrLink(pSng->_iSvrPort) == false)
	{
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	for (int i = 0; i < 2; i++)
	{
		m_lbIP[i].SetBkColor(RGB(0, 0, 0));
		m_lbIP[i].SetFontName("Arial");
		m_lbIP[i].SetFontSize(22);
		m_lbIP[i].SetTextColor(RGB(0, 0, 0));
		//m_lbIP[i].SetTextColor(RGB(255, 255, 0));
	}
	SetTimer(TIM_LNK, 1000, NULL);
	//CString ss("---");
	//CImgDLL::WriteLog("PLC:IP=%s,Port = %d", ss, 60);
	return TRUE;
	BOOL b = pSng->_PLC.InitSocket();
	if (b == false)
	{
		MessageBox(_T("��ʼ��Socketʧ��"), _T("ϵͳ��ʾ"), MB_ICONERROR);
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	int nPort = pSng->GetCfgInt("����", "PLC�˿�", 8501);
	CString strIPPLC = pSng->GetCfgString("����", "IP_PLC", "192.168.0.125");
	b = pSng->_PLC.ConnectPort(pSng->_strIP, nPort);
	if (b == FALSE)
	{
		MessageBox(_T("PLC ����ʧ�ܣ�"), _T("������ʾ"), MB_ICONERROR);
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	CImgDLL::WriteLog("PLC:IP=%s,Port = %d", strIPPLC, nPort);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CDlgCam4::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	g_nExt = 1;
	TRACE("\n��������˳����ƣ�");
	if (pSng->_pLog)
	{
		pSng->_pLog->ShowWindow(SW_HIDE);
	}
	Sleep(60);
	for (int i = 0; i < m_nThread; i++)
	{
		if (i == 1)
			continue;
		CImgDLL::CloseCam(i);
	}
	CImgDLL::SvrStop();
	Sleep(800);
	pSng->DeleteInstance();
	CDialogEx::OnOK();
}

void CDlgCam4::OnBnClickedBtnLog()
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pLog = (CIDlgInfo*)CImgDLL::LogDlg();
	//TRACE("\nLog���ڵ�ַ��%d", *pSng->_pLog);
}

void CDlgCam4::OnBnClickedBtnImage()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam;
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);
	IHpDllWin* pImageWin = (IHpDllWin*)CImgDLL::ImgDlg(strCam, NULL);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CALI);   //�Ƿ��ñ����ͼƬ
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		pSng->CreateDir("D:\\CALI\\" + strCam);
		pImageWin->m_bCali = true;
	}
	else
		pSng->CreateDir("D:\\XML\\" + strCam);
	////////��������ɼ�ͼƬ����ò��ñ���ͼƬ����Gain��Exposure��Gamma �����ļ���//////
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMG);   //�Ƿ��ñ����ͼƬ
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		strCam.Format("D:\\PIC\\%d.jpg", m_comboCam.GetCurSel() + 1);
		ReadImage(&pImageWin->m_HW.hoImage, strCam.GetBuffer());
		strCam.ReleaseBuffer();
		pImageWin->m_HW.bLoad = true;
		pImageWin->m_HW.bCam = false;
	}
	if (m_nThread > 0)
	{
		int nCam = m_comboCam.GetCurSel();
		if (nCam == 1)
		{
			CImgDLL::SafeSnap(pImageWin->m_HW, 0, 4);
			pImageWin->m_HW.bCam = false;
			return;
		}
		CImgDLL::Snap(pImageWin->m_HW, nCam, 4);
		pImageWin->m_HW.bCam = true;
	}
}

void CDlgCam4::SetEnable(BOOL bEna /* = TRUE */)
{
	GetDlgItem(IDC_BTN_CALIB)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_CAM)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_POS)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_IMAGE)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_TRIG)->EnableWindow(bEna);
	m_comboPos.EnableWindow(bEna);
	m_comboCam.EnableWindow(bEna);
}


void CDlgCam4::OnBnClickedBtnCalib()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText;
	GetDlgItemText(IDC_BTN_CALIB, strText);
	int nCam = 0;
	if (strText == "�����궨")
	{
		pSng->_nRun = CALIB_LOAD;
		while (pSng->_nRun == CALIB_LOAD)
		{
			DoEvent();
		}
		if (pSng->_nRun == RUN_NULL)
			return;
		SetDlgItemText(IDC_BTN_CALIB, "�رձ궨");
		int nCam = m_comboCam.GetCurSel();
		m_sg[nCam].iStatus = SIG_NULL;
		SetEnable(FALSE);
		GetDlgItem(IDC_BTN_CALIB)->EnableWindow(TRUE);
	}
	else
	{		
		pSng->_nRun = CALIB_UNLOAD;
		while (pSng->_nRun == CALIB_UNLOAD)
		{
			DoEvent();
		}
		SetDlgItemText(IDC_BTN_CALIB, "�����궨");
		SetEnable(TRUE);
	}
}



void CDlgCam4::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (nIDEvent == TIM_LNK)
	{
		for (int i = 0; i < 2; i++)
		{
			if (CImgDLL::SvrLinkStatus(i) == 0)
			{
				m_lbIP[i].SetBkColor(RGB(255, 0, 0));
				m_lbIP[i].SetText("δ����");
			}
			else
			{
				m_lbIP[i].SetBkColor(RGB(0, 255, 0));
				m_lbIP[i].SetText("������");
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


UINT ThreadCalib(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgCam4* pDlg = (CDlgCam4*)p;
	int nCam = pDlg->m_nCALI++;
	int nID[4] = { IDC_STATIC1, IDC_STATIC2, 0, 0 };
	CString strThread, strDir, strLog;
	strThread.Format("CAM%d�궨", nCam + 1);
	strDir.Format("D:\\CALI\\CAM%d", nCam + 1);   //���������ļ�·��
	BYTE pValue[4] = { 0 };
	vector<CPos> psVal;
	int nExp, nCount = 0;
	float fGain, fGamma;
	CImgDLL::WriteLog("�궨�߳�%d��ʼ���ɹ���", nCam + 1);
	for (;g_nExt == 1;)
	{
		Sleep(60);
		if (pSng->_nRun == RUN_NULL)
			continue;
		else if (pSng->_nRun == CALIB_LOAD)
		{
			CStrs XMLFiles;
			pSng->FindAllFile(strDir, XMLFiles);
			if (XMLFiles.size() == 0)
			{
				pSng->_nRun = RUN_NULL;
				strLog.Format("û�м�⵽CAM%d �ı궨XML�ļ�", nCam + 1);
				CImgDLL::WriteLog(strLog);
				pDlg->MessageBox(strLog, "", MB_ICONERROR);
				continue;
			}
			//////////////////////////////////���빤������////////////////////////////////////////////////////
			bool bLoad = CImgDLL::LoadXML(nCam * 10, XMLFiles[0], nExp, fGain, fGamma);
			if (bLoad == false)
			{
				pSng->_nRun = RUN_NULL;
				continue;
			}
			//������ع⣬���棬������һ��
			CString strCam;
			strCam.Format("CAM%d", nCam + 1);
			if (CImgDLL::SetCamPar(nCam, (float)nExp, fGain, fGamma) == false)
			{
				pSng->_nRun = RUN_NULL;
				continue;
			}
			///////////////////////////��������ɼ�ͼ��ͼ��////////////////////////////////////////////
			pDlg->m_HW[nCam].RealSize("green");
			pSng->_nRun = CALIB_WORK;
			nCount = 0;
			continue;
		}
		else if (pSng->_nRun == RUN_UNLOAD)
		{
			CImgDLL::UnLoadXML(10 * nCam);
			pSng->_nRun = RUN_NULL;
			continue;
		}
		////////////////////////////�Զ��궨����/////////////////////
		if (pDlg->m_sg[nCam].iStatus != SIG_ON)
			continue;
		pDlg->m_sg[nCam].iStatus = SIG_CAL;
		if (CImgDLL::IsImageOK(pDlg->m_HW[nCam], 10 * nCam, false, psVal) == true)
		{
			CString strFile, strSec, strKey, strVal;
			strSec.Format("CAM%d�궨", nCam + 1);
			strKey.Format("P%d", nCount + 1);
			strFile.Format("D:\\Image\\CAM%dCALI%d.bmp", nCam + 1, nCount + 1);
			WriteImage(pDlg->m_HW[nCam].hoImage, "bmp", 0, strFile.GetBuffer());
			strFile.ReleaseBuffer();
			double fImgPos[2];
			fImgPos[0] = atof(psVal[0].sgOut[0].sVal);
			fImgPos[1] = atof(psVal[0].sgOut[1].sVal);
			strVal.Format("%.2f,%.2f,%.2f,%.2f", fImgPos[0], fImgPos[1], 0, 0);
			pSng->SetCfgString(strSec, strKey, strVal);
			LogFile("%s ---%s:%s", strSec, strKey, strVal);
			nCount++;
			pDlg->m_sg[nCam].iStatus = SIG_OK;
		}
		else
		{
			strLog.Format("CAM%d ��%d �ű궨ͼʧ�ܣ�", nCam + 1, nCount + 1);
			pDlg->MessageBox(strLog, "", MB_ICONWARNING);
			pDlg->m_sg[nCam].iStatus = SIG_NG;
		}
		Sleep(900);
		pDlg->m_sg[nCam].iStatus = SIG_NULL;
	}
	return 0;
}

DWORD dwRobot = 0;
//UINT ThreadWatch(LPVOID p)
//{
//	CSingleton* pSng = CSingleton::GetInstance();
//	CDlgCam4* pDlg = (CDlgCam4*)p;
//	DWORD dwCycle = 0;
//	CString strPos;
//	for (;g_nExt != 1;)
//	{
//		Sleep(1);		
//		if (pSng->_nRun == CALIB_WORK)
//		{
//			for (int i = 0; i < 2;i++)
//			{
//				if (pDlg->m_sg[i].iStatus == SIG_NG || pDlg->m_sg[i].iStatus == SIG_OK)
//				{
//					pDlg->m_sg[i].iStatus = SIG_NULL;
//				}
//			}					
//			Sleep(800);
//			continue;
//		}
//		if (pSng->_nRun != RUN_WORK)
//			continue;
//		int II = 0;
//		CString strSvrRev;
//		CImgDLL::SvrRev(II, strSvrRev);
//		if (strSvrRev.Left(2) == "TA" && pDlg->m_sg[0].iStatus == SIG_NULL/* && pDlg->m_sg[1].iStatus == SIG_NULL*/)
//		{
//			pDlg->m_sg[0].iStatus = SIG_ON;
//			strPos.Format("%s",strSvrRev.Right(1));
//			pDlg->m_sg[0].sVal.Format("%d", atoi(strSvrRev.Right(1)) - 1);
//			pDlg->m_sg[1].sVal.Format("%d", atoi(strSvrRev.Right(1)) - 1);
//			CImgDLL::ClearSvrRev(II);
//			dwRobot = GetTickCount();
//		}
//		else if (pSng->_strRevRobot[0] == "TB")
//		{
//			
//		}
//		else if (pSng->_strRevRobot[0] == "TC")
//		{
//			//pDlg->m_nCHK[2] = SIG_ON;
//		}
//		else if (pSng->_strRevRobot[0] == "TD")
//		{
//			//pDlg->m_nCHK[3] = SIG_ON;
//		}
//		if ((pDlg->m_sg[0].iStatus == SIG_OK || pDlg->m_sg[0].iStatus == SIG_NG) && (pDlg->m_sg[1].iStatus == SIG_OK || pDlg->m_sg[1].iStatus == SIG_NG))
//		{
//			CString strRes;
//			strRes.Format("%d%d11%s%s", pDlg->m_sg[1].iStatus == SIG_OK, pDlg->m_sg[0].iStatus == SIG_OK, pSng->_strSnd[1], pSng->_strSnd[0]);
//			int nRet = CImgDLL::SvrSnd(0, strRes.GetBuffer(0), strRes.GetLength());
//			strRes.ReleaseBuffer();
//			CImgDLL::WriteLog("ȡ��λ%s: %s--����ʱ:%d ms",strPos,strRes,GetTickCount() - dwRobot);
//			pDlg->m_sg[0].iStatus = SIG_NULL;
//			pDlg->m_sg[1].iStatus = SIG_NULL;				
//			DWORD dwCurr = GetTickCount();
//			if (dwCycle > 0)
//			{
//				CImgDLL::WriteLog("���ĺ�ʱ:%.2f ��",0.001*(dwCurr - dwCycle));	
//			}
//			dwCycle = dwCurr;
//		}
//	}
//	return 0;
//}

UINT ThreadCam(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgCam4* pDlg = (CDlgCam4*)p;
	int nCam = pDlg->m_nThread;
	CString strCam;
	strCam.Format("CAM%d",nCam + 1);
	pDlg->m_nThread++;
	int nXML = 0;
	int nID[4] = { IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4 };
	CString strThread, strDir, strLog;
	strThread.Format("CAM%d�߳�", nCam + 1);
	strDir.Format("D:\\XML\\CAM%d", nCam + 1);   //���������ļ�·��
	CImgDLL::WriteLog("����߳�%d�����ɹ���",nCam + 1);
	vector<CPos> psVal;
	int nExp[10],nSumXML = 0,nNextNO = 0;
	float fGain[10], fGamma[10];
	for (; g_nExt != 1;)
	{
		Sleep(1);
		if (pSng->_nRun == RUN_NULL)
			continue;
		else if (pSng->_nRun == RUN_LOAD)
		{
			CStrs XMLFiles;
			pSng->FindAllFile(strDir, XMLFiles);	
			nSumXML = (int)XMLFiles.size();
			if (nSumXML == 0)
			{
				pSng->_nRun = RUN_NULL;
				strLog.Format("û�м�⵽CAM%d �Ĺ�������XML�ļ�", nCam + 1);
				LogFile(strLog);
				pDlg->MessageBox(strLog, "", MB_ICONERROR);
				continue;
			}
			//////////////////////////////////���빤������//////////////////////////////
			CSortStringArray strsSort;
			for (int i = 0; i < nSumXML; i++)
				strsSort.Add(XMLFiles[i]);
			strsSort.Sort(true);
			bool bLoad = true;
			for (int i = 0; i < nSumXML && bLoad == true;i++)
			{
				CString strXMLSel = strsSort.GetAt(i);
				bLoad = CImgDLL::LoadXML(nCam * 10 + i,strXMLSel , nExp[i], fGain[i], fGamma[i]);
				if (bLoad)
				{
					CImgDLL::WriteLog("%s �ļ�����ɹ���",strXMLSel);
				}
				else
					CImgDLL::WriteLog("%s �ļ�����ʧ�ܣ�",strXMLSel);
			}
			if (bLoad == false)
			{
				pSng->_nRun = RUN_NULL;
			}
			else
			{
				//////////////��������ɼ�ͼ��ͼ��/////////////////////////////////////
				if (nCam != 1)
				{
					nNextNO = 0;
					if (CImgDLL::SetCamPar(nCam, (float)nExp[0], fGain[0], fGamma[0]) == false)
					{
						pSng->_nRun = RUN_NULL;
						continue;
					}
					CImgDLL::SafeSnap(pDlg->m_HW[nCam], nCam, 4);                      //��һ����Ƭ�����ߴ�
				}
				//CRect rc;
				//pDlg->GetDlgItem(nID[nCam])->GetClientRect(&rc);
				pDlg->m_HW[nCam].RealSize();
				pSng->_nRun = RUN_WORK;
			}
			continue;
		}
		else if (pSng->_nRun == RUN_UNLOAD)
		{	
			for (int i = 0; i < nSumXML;i++)
			{		
				CImgDLL::UnLoadXML(10 * nCam + i);
			}
			pSng->_nRun = RUN_NULL;
			continue;
		}
		if (pDlg->m_sg[nCam].iStatus != SIG_ON)                                                    //�յ����ָ��
			continue;
		pDlg->m_sg[nCam].iStatus = SIG_CAL;
		int nInx = atoi(pDlg->m_sg[nCam].sVal);
		if (nCam == 1)
		{
			CImgDLL::WriteLog("CAM2 ��ʼ���㣬��ʱ:%d ms", GetTickCount() - dwRobot);
		}
		else
		{
			if (nSumXML > 1/* && nNextNO != nInx*/)
			{
				nNextNO = nInx; 
				if (CImgDLL::SetCamPar(nCam, (float)nExp[nInx], fGain[nInx], fGamma[nInx]) == false)
				{
					CImgDLL::WriteLog("CAM %d ȡ��%d ����ع�����ʧ��", nCam + 1, nInx + 1);
				}
				//CImgDLL::WriteLog("���ǰ���ع⣡����");
			}
			bool bFailSnap = CImgDLL::SafeSnap(pDlg->m_HW[nCam], nCam, 3);
			if (bFailSnap)
			{
				pDlg->m_sg[nCam].iStatus = SIG_NG;
				continue;
			}
			if (nCam == 0)
			{
				CopyImage(pDlg->m_HW[0].hoImage, &pDlg->m_HW[1].hoImage);
				pDlg->m_HW[1].WinTexts.clear();
				pDlg->m_HW[1].ShowImage();
				CImgDLL::WriteLog("CAM1 ��ʼ���㣬�յ�ָ�������ɹ���ʱ:%d ms", GetTickCount() - dwRobot);
			}
		}			
		double fAng[2] = { -0.7*PI / 180, 180.26*PI / 180 };
		for (int k = 0; k < 2 && nCam == 0;k++)
		{
			pSng->_strSnd[k].Format("+0.000+0.000+0.000+0.000");
			psVal.clear();
			bool bOK = CImgDLL::IsImageOK(pDlg->m_HW[k], 10 * k + nInx , false, psVal);
			if (bOK == false)
			{
				pDlg->m_sg[k].iStatus = SIG_NG;
				CImgDLL::WriteLog("CAM%d ����NG����ʱ:%d ms", k + 1, GetTickCount() - dwRobot);
				continue;
			}
			double fImgPos[2];
			fImgPos[0] = atof(psVal[0].sgOut[0].sVal);                    //ͼ������X
			fImgPos[1] = atof(psVal[0].sgOut[1].sVal);                    //ͼ������Y
			double fPhi = atof(psVal[1].sgOut[0].sVal);                   //ͼ��Ƕ�
			double fDelta[3], fOffset[3] = { 0 };
			CString strSec;
			strSec.Format("R%d%d����", nInx + 1, 1 + k);
			fOffset[0] = atof(pSng->GetCfgString(strSec, "X", ""));
			fOffset[1] = atof(pSng->GetCfgString(strSec, "Y", ""));
			fOffset[2] = atof(pSng->GetCfgString(strSec, "A", ""));
			double fRot = (pSng->_PosST[nInx][k].fPos[2] - fPhi + fOffset[2])*PI / 180;   //��׼�Ƕȣ���������ĽǶȺͲ�����λ���ǽǶȣ�Ҫת���ɻ���
			HTuple HomMat2D, HomMat2DRot, hvTransY, hvTransX, hvMX, hvXST, hvYST;
			HomMat2dIdentity(&HomMat2D);
			HomMat2dRotate(HomMat2D, fAng[k], pSng->_RotAxs[nInx].y, pSng->_RotAxs[nInx].x, &HomMat2DRot);
			CPos ImgPts[9], AxisPts[9];
			for (int j = 0; j < 9; j++)
			{
				ImgPts[j].x = pSng->_psCali[0].fImg[j].x;
				ImgPts[j].y = pSng->_psCali[0].fImg[j].y;
				AxisPts[j].x = pSng->_psCali[0].fAxs[j].x;
				AxisPts[j].y = pSng->_psCali[0].fAxs[j].y;
			}
			pSng->NineDotCalib(ImgPts, AxisPts, hvMX);
			AffineTransPoint2d(HomMat2DRot, pSng->_PosST[nInx][k].fPos[1], pSng->_PosST[nInx][k].fPos[0], &hvYST, &hvXST);
			AffineTransPoint2d(hvMX, hvYST, hvXST, &hvTransY, &hvTransX);
			double fPosST[2];
			fPosST[0] = hvTransX.D();
			fPosST[1] = hvTransY.D();
			//CImgDLL::WriteLog("CAM%d ��׼��е����:(%.2f,%.2f)--ͼ������:(%.2f,%.2f)", nCam + 1, fPosST[0], fPosST[1], pSng->_PosST[nInx][nCam].fPos[0], pSng->_PosST[nInx][nCam].fPos[1]);
			HomMat2dIdentity(&HomMat2D);
			HomMat2dRotate(HomMat2D, fRot + fAng[k], pSng->_RotAxs[nInx].y, pSng->_RotAxs[nInx].x, &HomMat2DRot);
			AffineTransPoint2d(HomMat2DRot, fImgPos[1], fImgPos[0], &hvYST, &hvXST);
			AffineTransPoint2d(hvMX, hvYST, hvXST, &hvTransY, &hvTransX);
			double fCurr[2];
			fCurr[0] = hvTransX.D();
			fCurr[1] = hvTransY.D();
			//CImgDLL::WriteLog("CAM%d ��ǰ��е����:(%.2f,%.2f)--ͼ������:(%.2f,%.2f)--��ת:%.2f��", k + 1, fCurr[0], fCurr[1], fImgPos[0], fImgPos[1], -fRot * 180 / PI);
			fDelta[0] = (fPosST[0] - fCurr[0]) + fOffset[0];
			fDelta[1] = (fPosST[1] - fCurr[1]) + fOffset[1];
			fDelta[2] = fRot;
			CString strDelta[3];
			for (int i = 0; i < 3; i++)
			{
				if (fDelta[i] >= ESP)
				{
					strDelta[i].Format("+%.3f", fDelta[i]);
				}
				else
				{
					strDelta[i].Format("-%.3f", abs(fDelta[i]));
				}
			}
			pSng->_strSnd[k].Format("%s%s+0.000%s", strDelta[0], strDelta[1], strDelta[2]);
			pDlg->m_sg[k].iStatus = SIG_OK;
			CImgDLL::WriteLog("CAM%d ����OK����ʱ:%d ms", k + 1, GetTickCount() - dwRobot);
			//Sleep(10);
			//if (nSumXML > 1 && nCam == 0)
			//{
			//	nNextNO = (nInx + 1) % nSumXML;
			//	if (CImgDLL::SetCamPar(nCam, (float)nExp[nNextNO], fGain[nNextNO], fGamma[nNextNO]) == false)
			//	{
			//		CImgDLL::WriteLog("CAM %d ȡ��%d ����ع�����ʧ��", nCam + 1, nNextNO + 1);
			//	}
			//}
		}
		if (nCam == 0)
		{
			Sleep(100);
			CString strImg;
			pSng->CreateDir("D:\\PIC");
			strImg.Format("D:\\PIC\\%d-%d.jpg",nInx + 1,dwRobot);
			WriteImage(pDlg->m_HW[0].hoImage, "jpg", 0, strImg.GetBuffer(0));
			strImg.ReleaseBuffer();
		}
		if (nCam > 1)
		{
		}
		Sleep(600);
	}
	return 0;
}

void CDlgCam4::OnBnClickedBtnTrig()
{
	int nRot =  m_comboPos.GetCurSel();	
	m_sg[nRot].iStatus = SIG_ON;
	m_sg[nRot].sVal.Format("%d", nRot);
}


void CDlgCam4::OnCbnSelchangeComboCam()
{
	if (m_comboCam.GetCurSel() > 1)
	{
		GetDlgItem(IDC_BTN_CALIB)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_POS)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BTN_POS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CALIB)->EnableWindow(TRUE);
	}
}

void CDlgCam4::OnBnClickedBtnCam()
{
	int nCam = m_comboCam.GetCurSel();
	CIDlgCamCfg* pDlgCam = (CIDlgCamCfg*)CImgDLL::GetCamCfgDlg(nCam);
	if (pDlgCam == NULL)
	{
		CImgDLL::WriteLog("���û�г�ʼ���ɹ���");
	}
}

void CDlgCam4::OnBnClickedBtnRun()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText;
	GetDlgItemText(IDC_BTN_RUN, strText);
	int nCam = 0;
	if (strText == "�Զ�����")
	{
		pSng->_nRun = RUN_LOAD;
		while (pSng->_nRun == RUN_LOAD)
		{
			DoEvent();
		}
		if (pSng->_nRun == RUN_NULL)
			return;
		for (int i = 0; i < 4;i++)
		{
			m_sg[i].iStatus = SIG_NULL;
		}
		SetEnable(FALSE);
		GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BTN_RUN, "ֹͣ����");
	}
	else
	{
		pSng->_nRun = RUN_UNLOAD;
		while (pSng->_nRun == RUN_UNLOAD)
		{
			DoEvent();
		}
		SetDlgItemText(IDC_BTN_RUN, "�Զ�����");
		SetEnable(TRUE);
	}
}

void CDlgCam4::OnBnClickedBtnPos()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strDir;
	int nCam = m_comboCam.GetCurSel();
	strDir.Format("D:\\XML\\CAM%d", nCam + 1);   //���������ļ�·��
	CStrs XMLFiles;
	pSng->FindAllFile(strDir, XMLFiles);
	if (XMLFiles.size() == 0)
	{
		MessageBox(strDir + " û��XML�ļ�", "��λʧ��", MB_ICONERROR);
		return;
	}	
	int nInx = m_comboPos.GetCurSel();
	int nExp;
	float fGain, fGamma;
	int nSumXML = XMLFiles.size();
	CSortStringArray strsSort;
	for (int i = 0; i < nSumXML; i++)
		strsSort.Add(XMLFiles[i]);
	strsSort.Sort(true);
	if (nInx >= strsSort.GetCount())
	{
		MessageBox(strDir + " û�и�������XML�ļ�", "�ļ�ȱʧ", MB_ICONERROR);
		return;
	}
	bool bLoad = CImgDLL::LoadXML(nCam * 10 + nInx, strsSort.GetAt(nInx), nExp, fGain, fGamma);
	if (bLoad == false)
	{
		MessageBox(strDir + " ����XMLʧ�ܣ�", "��λʧ��", MB_ICONERROR);		
		return;
	}
	if (CImgDLL::SetCamPar(0, (float)nExp, fGain, fGamma) == false)
	{
		MessageBox("�����������ʧ�ܣ�", "��λʧ��", MB_ICONERROR);
		return;
	}
	CImgDLL::SafeSnap(m_HW[nCam], 0, 4);      //��һ����Ƭ�����ߴ�
	vector<CPos> psVal;
	bool bOK = CImgDLL::IsImageOK(m_HW[nCam], 10 * nCam + nInx, false, psVal);
	if (bOK == false)
	{
		MessageBox("����NG��", "��λʧ��", MB_ICONERROR);
		return;
	}
	////���������̣����򵼳�X��Y���Ƕ�
	double fXYR[3] = { 2000,1500,0 };
	fXYR[0] = atof(psVal[0].sgOut[0].sVal);                     //ͼ������X
	fXYR[1] = atof(psVal[0].sgOut[1].sVal);                     //ͼ������Y
	fXYR[2] = atof(psVal[1].sgOut[0].sVal);                     //ͼ��Ƕȣ���λ��
	CString strCam,strVal;
	strCam.Format("CAM%d%d", m_comboCam.GetCurSel() + 1,nInx + 1);
	strVal.Format("%.2f,%.2f,%.2f", fXYR[0], fXYR[1], fXYR[2]);
	pSng->SetCfgString("��׼λ��", strCam, strVal);
	CImgDLL::UnLoadXML(10 * nCam + nInx);	
	CImgDLL::WriteLog("%s�ı�׼λ��%s����ɹ���",strCam,strVal);
}

void CDlgCam4::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		OnBnClickedOk();
		return;
	}
	CDialogEx::OnSysCommand(nID, lParam);
}


void CDlgCam4::OnBnClickedBtnOffset()
{
	CDlgOffset dlg;
	dlg.DoModal();
}


void CDlgCam4::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMButtonDown(nFlags, point);
}


BOOL CDlgCam4::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	//if (pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_MBUTTONDBLCLK)
	//{
	//	Sleep(1);
	//}

	return CDialogEx::PreTranslateMessage(pMsg);
}
