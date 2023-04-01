// DlgBK.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDll.h"
#include "DlgBK.h"
#include "afxdialogex.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "..\DllMod\IDlgCamCfg.h"
#include "..\DllMod\TCPSocket.h"
#include "DlgOffset.h"
#include "ImgDLL.h"
#include "DlgOffset.h"

#define SIG_LLSR   0
#define SIG_RLSR  1
#define SIG_BK        2      //料盘内来料的位置
#define SIG_LASM   3     //左装配相机
#define SIG_RASM   4    //右装配相机
#define SIG_LSCAN  5   //左扫码
#define SIG_RSCAN  6  //右扫码

// CDlgBK 对话框
extern void DoEvent();
extern int g_nExt;

extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);

UINT ThreadSnap(LPVOID p);
UINT ThreadOrder(LPVOID p);

IMPLEMENT_DYNAMIC(CDlgBK, CDialogEx)

CDlgBK::CDlgBK(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgBK::IDD, pParent)
{

}

CDlgBK::~CDlgBK()
{
}

void CDlgBK::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM, m_comboCam);
	DDX_Control(pDX, IDC_STATIC_PLC, m_lbPLC);
	DDX_Control(pDX, IDC_STATIC_RBT1, m_lbRBT[0]);
	DDX_Control(pDX, IDC_STATIC_RBT2, m_lbRBT[1]);
	DDX_Control(pDX, IDC_MSCOMM1, m_Com[0]);
	DDX_Control(pDX, IDC_MSCOMM2, m_Com[1]);
}


BEGIN_MESSAGE_MAP(CDlgBK, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgBK::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CAM, &CDlgBK::OnBnClickedBtnCam)
	ON_BN_CLICKED(IDC_BTN_IMG, &CDlgBK::OnBnClickedBtnImg)
	ON_BN_CLICKED(IDC_BTN_OFFSET, &CDlgBK::OnBnClickedBtnOffset)
	ON_BN_CLICKED(IDC_BTN_RUN, &CDlgBK::OnBnClickedBtnRun)
//	ON_BN_CLICKED(IDC_BTN_POS, &CDlgBK::OnBnClickedBtnPos)
	ON_BN_CLICKED(IDC_BTN_LOG, &CDlgBK::OnBnClickedBtnLog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_TRIG, &CDlgBK::OnBnClickedBtnTrig)
	ON_BN_CLICKED(IDC_CHECK_XML, &CDlgBK::OnBnClickedCheckXml)
END_MESSAGE_MAP()


// CDlgBK 消息处理程序

void CDlgBK::UnLoadXML(int nCam, int nSumXML)
{
	for (int i = 0; i < nSumXML; i++)
	{
		CImgDLL::UnLoadXML(10 * nCam + i);
	}
}


BOOL CDlgBK::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSingleton* pSng = CSingleton::GetInstance();
	if (CImgDLL::Init() == FALSE)
	{
		MessageBox("载入图像库DLL失败!");
		SendMessage(WM_CLOSE);
		return FALSE;
	}

	CString strHex, strRes;
	BYTE c[4] = { 0xFF, 0xFF, 0xEC, 0x78 };
	//BYTE c[4] = { 0x0, 0x0, 0x13, 0x88 };
	strHex.Format("%02X%02X%02X%02X", c[0], c[1], c[2], c[3]);
	strRes = strHex.Right(8);
	long lRes = strtoul(strRes, NULL, 16);

	pSng->CreateDir("D:\\INI");
	pSng->CreateDir("D:\\TMP");
	pSng->CreateDir("D:\\CALI");
	pSng->CreateDir("D:\\Image");
	pSng->CreateDir("D:\\XML");
	vector<int> nIDs;
	nIDs.push_back(IDC_STATIC1);
	nIDs.push_back(IDC_STATIC2);
	nIDs.push_back(IDC_STATIC3);
	//nIDs.push_back(IDC_STATIC4);
	CImgDLL::WriteLog("软件主对话框初始化成功！");
	bool bTest = true;
	for (int i = 0; i < (int)nIDs.size(); i++)
	{
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		m_comboCam.AddString(strCam);
		m_HW[i].psRD.x = 4024;
		m_HW[i].psRD.y = 3036;
		m_HW[i].InitWnd(GetDlgItem(nIDs[i]));
		SetColor(m_HW[i].WndID, "green");
		SetLineWidth(m_HW[i].WndID, 1);
		int nPosTxt[2] = { 0 };
		m_HW[i].ShowImage(nPosTxt, strCam);
		if (bTest)
			continue;   //111111111111111111111111记得取消
		bool bOK = CImgDLL::InitCam(i);
		if (bOK == false)
		{
			CImgDLL::WriteLog(_T("相机%d初始化失败"), i + 1);
			MessageBox("相机初始化失败！");
			SendMessage(WM_CLOSE);
			return FALSE;
		}
		bOK = CImgDLL::SetHeartTime(i, 2000);
		AfxBeginThread(ThreadSnap, this, THREAD_PRIORITY_HIGHEST);
		while (i == m_nThread)
		{
			DoEvent();
		}
		//if (i < 2)
		//{
		//	AfxBeginThread(ThreadCalib, this);
		//	while (i == m_nCALI)
		//	{
		//		DoEvent();
		//	}
		//}
	}
	m_comboCam.SetCurSel(0);
	m_lbPLC.SetFontName("Arial");
	m_lbPLC.SetFontSize(16);
	m_lbPLC.SetTextColor(RGB(0, 0, 0));
	m_lbPLC.SetBkColor(RGB(255, 0, 0));
	m_lbPLC.SetText("未连接");
	if (bTest)
	{
		return FALSE;
	}
	AfxBeginThread(ThreadOrder, this);
	int iNO[2] = { 1, 2 };
	bool bRet = InitialCom(iNO);
	if (bRet == false)
	{
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	if (CImgDLL::SvrLink(pSng->_iSvrPort) == false)
	{
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	for (int i = 0; i < 2; i++)
	{
		m_lbRBT[i].SetBkColor(RGB(0, 0, 0));
		m_lbRBT[i].SetFontName("Arial");
		m_lbRBT[i].SetFontBold(TRUE);
		m_lbRBT[i].SetFontSize(16);
		m_lbRBT[i].SetTextColor(RGB(0, 0, 0));
	}
	SetTimer(TIM_LNK, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


UINT ThreadOrder(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgBK* pDlg = (CDlgBK*)p;
	BOOL b = pSng->_PLC.InitSocket();
	if (b == false)
	{
		pDlg->MessageBox(_T("初始化Socket失败"), _T("系统提示"), MB_ICONERROR);
		pDlg->SendMessage(WM_CLOSE);
		return FALSE;
	}
	int nPort = pSng->GetCfgInt("参数", "PLC端口", 8501);
	CString strIPPLC = pSng->GetCfgString("参数", "PLC", "192.168.0.10");
	b = pSng->_PLC.ConnectPort(strIPPLC, nPort);
	if (b == FALSE)
	{
		pDlg->MessageBox(_T("PLC 连接失败！"), _T("错误提示"), MB_ICONERROR);
		pDlg->SendMessage(WM_CLOSE);
		return FALSE;
	}
	pDlg->m_lbPLC.SetBkColor(RGB(0, 255, 0));
	pDlg->m_lbPLC.SetText("PLC已连接");
	//CImgDLL::WriteLog("PLC:IP=%s,Port = %d", strIPPLC, nPort);
	DWORD dwPLC = 0;
	DWORD dwCycle = 0;
	CString strPos;
	for (; g_nExt != 1;)
	{
		Sleep(1);
		//if (pSng->_nRun == CALIB_WORK)
		//{
		//	for (int i = 0; i < 3; i++)
		//	{
		//		if (pDlg->m_sg[i].iStatus == SIG_NG || pDlg->m_sg[i].iStatus == SIG_OK)
		//		{
		//			pDlg->m_sg[i].iStatus = SIG_NULL;
		//		}
		//	}
		//	Sleep(800);
		//	continue;
		//}
		if (pSng->_nRun != RUN_WORK)
			continue;
		for (int i = 0; i < 2; i++)
		{
			CString strSvrRev;
			CImgDLL::SvrRev(i, strSvrRev);
			if (strSvrRev.Left(2) == "TA")
			{
				pDlg->m_sg[i + 3].sVal.Format("%d", 0);
				pDlg->m_sg[i + 3].iStatus = SIG_ON;
				pDlg->m_sg[i + 3].dwTime = GetTickCount();
				CImgDLL::ClearSvrRev(i);
			}		
			else if (strSvrRev.Left(2) == "TB")
			{
				pDlg->m_sg[i + 3].sVal.Format("%d", 1);
				pDlg->m_sg[i + 3].iStatus = SIG_ON;
				pDlg->m_sg[i + 3].dwTime = GetTickCount();
				CImgDLL::ClearSvrRev(i);
				//CImgDLL::WriteLog("CAM%d 收到拍照1",i + 2);
			}
		}
		for (int i = 0; i < 2;i++)
		{
			CString strRes;
			if (pDlg->m_sg[i + 3].iStatus == SIG_NG)
			{
				strRes.Format("0,0,0,0,0,0");
				CImgDLL::SvrSnd(i, strRes.GetBuffer(), strRes.GetLength());
				pDlg->m_sg[i + 3].iStatus = SIG_NULL;
				pDlg->m_sg[i + 3].sRet.Empty();
				//CImgDLL::WriteLog("CAM%d NG发送%s",i + 2,strRes);
			}
			else if (pDlg->m_sg[i + 3].iStatus == SIG_OK)
			{
				if (pDlg->m_sg[i + 3].sVal == "1")
				{
					CImgDLL::SvrSnd(i, pDlg->m_sg[i + 3].sRet.GetBuffer(), pDlg->m_sg[i + 3].sRet.GetLength());
					CImgDLL::WriteLog("CAM%d 汇总发送%s", i + 2, pDlg->m_sg[i + 3].sRet);
				}
				pDlg->m_sg[i + 3].iStatus = SIG_NULL;
			}
		}
		//continue;
		long lVal[4];
		CStrs ssTrg;
		ssTrg.push_back("0020");            //左点激光触发
		ssTrg.push_back("0024");            //右点激光触发
		ssTrg.push_back("0030");            //BK补正相机
		//ssTrg.push_back("0032");            //左组装相机
		//ssTrg.push_back("0034");            //右组装相机
		ssTrg.push_back("0036");            //左扫码
		ssTrg.push_back("0038");            //右扫码
		for (int i = 0; i < (int)ssTrg.size(); i++)
		{
			pSng->ReadFromTCP(ssTrg[i], lVal);
			if (lVal[0] == 1 && i < 2 && pDlg->m_sg[i].iStatus == SIG_NULL)  //两个激光器
			{
				pDlg->m_sg[i].iStatus = SIG_ON;
				pDlg->m_sg[i].dwTime = GetTickCount();
			}
			if (lVal[0] == 1 && i == 2 && pDlg->m_sg[2].iStatus == SIG_NULL)
			{
				pDlg->m_sg[2].iStatus = SIG_ON;
				pDlg->m_sg[2].dwTime = GetTickCount();
			}
		}
		//if (pDlg->m_sg[0].iStatus == SIG_NULL/* && pDlg->m_sg[1].iStatus == SIG_NULL*/)
		//{
		//	pDlg->m_sg[0].iStatus = SIG_ON;
		//	//strPos.Format("%s", strSvrRev.Right(1));
		//	//pDlg->m_sg[0].sVal.Format("%d", atoi(strSvrRev.Right(1)) - 1);
		//	//pDlg->m_sg[1].sVal.Format("%d", atoi(strSvrRev.Right(1)) - 1);
		//	pDlg->m_sg[0].dwTime = GetTickCount();
		//}
		//if ((pDlg->m_sg[0].iStatus == SIG_OK || pDlg->m_sg[0].iStatus == SIG_NG) &&
		//	(pDlg->m_sg[1].iStatus == SIG_OK || pDlg->m_sg[1].iStatus == SIG_NG))
		//{
		//	CString strRes;
		//	CImgDLL::WriteLog("取料位%s: %s--检测耗时:%d ms", strPos, strRes, GetTickCount() - dwPLC);
		//	pDlg->m_sg[0].iStatus = SIG_NULL;
		//	pDlg->m_sg[1].iStatus = SIG_NULL;
		//	DWORD dwCurr = GetTickCount();
		//	if (dwCycle > 0)
		//	{
		//		CImgDLL::WriteLog("节拍耗时:%.2f 秒", 0.001*(dwCurr - dwCycle));
		//	}
		//	dwCycle = dwCurr;
		//}
	}
	return 0;
}

UINT ThreadSnap(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgBK* pDlg = (CDlgBK*)p;
	int nCam = pDlg->m_nThread;
	pDlg->m_nThread++;
	CString strThread, strDir, strLog;
	strThread.Format("CAM%d线程", nCam + 1);
	strDir.Format("D:\\XML\\CAM%d", nCam + 1);   //工作流程文件路径
	CImgDLL::WriteLog("相机线程%d创建成功！", nCam + 1);
	vector<CPos> psVal;
	int nExp[10], nSumXML = 0, nNextXML = 0;
	float fGain[10], fGamma[10];
	for (; g_nExt != 1;)
	{
		Sleep(9);
		if (pSng->_nRun == RUN_NULL)
			continue;
		if (pSng->_nRun == RUN_UNLOAD)
		{
			pDlg->UnLoadXML(nCam, nSumXML);
			pDlg->m_nRUNs--;
			do 
			{
				Sleep(9);
			} while (pDlg->m_nRUNs > 0);
			pSng->_nRun = RUN_NULL;
			continue;
		}
		else if (pSng->_nRun == RUN_LOAD)
		{
			CStrs XMLFiles;
			pSng->FindAllFile(strDir, XMLFiles);
			nSumXML = (int)XMLFiles.size();
			if (nSumXML == 0)
			{
				strLog.Format("没有检测到CAM%d 的工作流程XML文件", nCam + 1);
				LogFile(strLog);
				pDlg->MessageBox(strLog, "", MB_ICONERROR);
				continue;
			}
			CSortStringArray strsSort;
			for (int i = 0; i < nSumXML; i++)
				strsSort.Add(XMLFiles[i]);
			strsSort.Sort(true);
			bool bLoad = true;
			for (int i = 0; i < nSumXML && bLoad == true; i++)
			{
				CString strXMLSel = strsSort.GetAt(i);
				bLoad = CImgDLL::LoadXML(nCam * 10 + i, strXMLSel, nExp[i], fGain[i], fGamma[i]);
				if (bLoad)
				{
					CImgDLL::WriteLog("%s 文件载入成功！", strXMLSel);
					continue;
				}
				CImgDLL::WriteLog("%s 文件载入失败,无法自动生产！", strXMLSel);
				break;
			}
			if (bLoad == false)
			{
				pDlg->UnLoadXML(nCam, nSumXML);
				continue;
			}
			bool bFail = CImgDLL::SafeSnap(pDlg->m_HW[nCam], nCam, 2);                      //拍一张照片触发尺寸
			if (bFail)
			{
				pDlg->UnLoadXML(nCam, nSumXML);
				pSng->_nRun = RUN_NULL;
				continue;
			}
			//////////////////触发相机采集图像/////////////////////////////////////
			if (CImgDLL::SetCamPar(nCam, (float)nExp[0], fGain[0], fGamma[0]) == false)
			{
				pDlg->UnLoadXML(nCam, nSumXML);
				pSng->_nRun = RUN_NULL;
				continue;
			}
			pDlg->m_nRUNs++;                 //当前线程已经载入XML完成了
			DWORD dwST = GetTickCount();
			while (true)
			{		
				Sleep(1);
				if (GetTickCount() - dwST > 6000)
				{
					pDlg->m_nRUNs = pDlg->m_nThread;
					pSng->_nRun = RUN_UNLOAD;                     //一直没有完成XML载入，所以要卸载XML
					break;
				}
				if (pDlg->m_nRUNs < pDlg->m_nThread)
					continue;
				pSng->_nRun = RUN_WORK;			//完成了全部XML载入
				pDlg->m_HW[nCam].RealSize();
				break;
			}
			///////////////////////////////无论因为何种原因跳出循环都要清空触发信号状态/////////////////////////////////
			pDlg->m_sg[nCam + 2].iStatus = SIG_NULL;      //无信号状态
			continue;
		}
		if (pDlg->m_sg[nCam + 2].iStatus != SIG_ON)
			continue;
		if (nCam == 0)
		{
			pSng->Write2TCP("0030", 0);
		}
		pDlg->m_sg[nCam + 2].iStatus = SIG_CAL;           //先进入计算阶段
		int nInx = atoi(pDlg->m_sg[nCam + 2].sVal);           //不同的拍照指令决定采用0 或者1
		if (nSumXML > 1)
		{
			if (CImgDLL::SetCamPar(nCam, (float)nExp[nInx], fGain[nInx], fGamma[nInx]) == false)
			{
				CImgDLL::WriteLog("CAM %d 取料%d 相机曝光设置失败", nCam + 1, nInx + 1);
			}
		}
		bool bFailSnap = CImgDLL::SafeSnap(pDlg->m_HW[nCam], nCam, 3);   //采集图片
		if (bFailSnap)
		{
			pDlg->m_sg[nCam + 2].iStatus = SIG_NG;
			continue;
		}
		bool bOK = CImgDLL::IsImageOK(pDlg->m_HW[nCam], 10 * nCam + nInx, false, psVal);
		if (bOK == false)
		{
			if (nCam == 0)
			{
				pSng->Write2TCP("0040", RES_NG);
				CImgDLL::WriteLog("BK来料NG，耗时:%d ms",GetTickCount() - pDlg->m_sg[nCam + 2].dwTime);
				pDlg->m_sg[2].iStatus = SIG_NULL;
				continue;
			}
			pDlg->m_sg[nCam + 2].iStatus = SIG_NG;
			pDlg->m_sg[nCam + 2].sRet.Format("0,0,0,0,0,0");
			//CImgDLL::WriteLog("CAM%d 计算NG，耗时:%d ms", k + 1, GetTickCount() - pDlg->m_sg[nCam + 2].dwTime);
			continue;
		}
		if (nCam > 0) //Cam2 和 Cam3
		{
			double fImage[3] = {0};
			for (int i = 0; i < psVal.size(); i++)
			{
				fImage[i] = atof(psVal[i].sgOut[0].sVal);
			}
			CImgDLL::WriteLog("CAM%d 拍照%d------图像坐标(%.2f,%.2f)",nCam + 1,nInx,fImage[0],fImage[1]);
			if (nInx == 0)
			{
				pDlg->m_sg[nCam + 2].sRet.Format("%.2f,%.2f,0,%.2f", fImage[0], fImage[1], fImage[2]);
			}
			else
			{				
				double fDelta[3];
				CPos ImgPts[9], AxisPts[9];
				for (int j = 0; j < 9; j++)
				{
					ImgPts[j].x = pSng->_psCali[nCam - 1].fImg[j].x;
					ImgPts[j].y = pSng->_psCali[nCam - 1].fImg[j].y;
					AxisPts[j].x = pSng->_psCali[nCam - 1].fAxs[j].x;
					AxisPts[j].y = pSng->_psCali[nCam - 1].fAxs[j].y;
				}
				HTuple hvMX,hvTransY,hvTransX;
				pSng->NineDotCalib(ImgPts, AxisPts, hvMX);
				CStrs Vs;
				pSng->SplitString(pDlg->m_sg[nCam + 2].sRet, ",", Vs);
				if (Vs.size() < 2)
				{
					pDlg->m_sg[nCam + 2].iStatus = SIG_NG;
					continue;
				}
				double fPrImg[3],fPrRBT[3],fFCRBT[3];
				fPrImg[0] = atof(Vs[0]);
				fPrImg[1] = atof(Vs[1]);
				AffineTransPoint2d(hvMX, fPrImg[1], fPrImg[0], &hvTransY, &hvTransX);
				fPrRBT[0] = hvTransX.D();
				fPrRBT[1] = hvTransY.D();
				AffineTransPoint2d(hvMX, fImage[1], fImage[0], &hvTransY, &hvTransX);
				fFCRBT[0] = hvTransX.D();
				fFCRBT[1] = hvTransY.D();
				fDelta[0] = fPrRBT[0] - fFCRBT[0];
				fDelta[1] = fPrRBT[1] - fFCRBT[1];
				CImgDLL::WriteLog("CAM%d发送移动量==(%.2f,%.2f)", nCam + 1,fDelta[0], fDelta[1]);
				pDlg->m_sg[nCam + 2].sRet.Format("1,1,%.2f,%.2f,0,0", fDelta[0], fDelta[1]);
			}
			pDlg->m_sg[nCam + 2].iStatus = SIG_OK;
			continue;
		}
		else   //BK来料相机
		{
			CImgDLL::WriteLog("BK来料OK，耗时:%d ms", GetTickCount() - pDlg->m_sg[2].dwTime);
			pSng->Write2TCP("0040", RES_OK);
			pDlg->m_sg[2].iStatus = SIG_NULL;
		}
	}
	return TRUE;
}

void CDlgBK::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	g_nExt = 1;
	TRACE("\n启动软件退出机制！");
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
	Sleep(800);
	pSng->DeleteInstance();
	CImgDLL::SvrStop();
	CDialogEx::OnOK();
}

void CDlgBK::OnBnClickedBtnCam()
{
	int nCam = m_comboCam.GetCurSel();
	CIDlgCamCfg* pDlgCam = (CIDlgCamCfg*)CImgDLL::GetCamCfgDlg(nCam);
	if (pDlgCam == NULL)
	{
		CImgDLL::WriteLog("相机没有初始化成功！");
	}
}


void CDlgBK::OnBnClickedBtnImg()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam;
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);
	IHpDllWin* pImageWin = (IHpDllWin*)CImgDLL::ImgDlg(strCam, NULL);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CALI);   //是否用保存的图片
	//if (pBtn->GetCheck() == BST_CHECKED)
	//{
	//	pSng->CreateDir("D:\\CALI\\" + strCam);
	//	pImageWin->m_bCali = true;
	//}
	//else
		pSng->CreateDir("D:\\XML\\" + strCam);
	////////触发相机采集图片，最好采用保存图片，用Gain，Exposure，Gamma 命名文件名//////
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMG);   //是否用保存的图片
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		strCam.Format("D:\\Image\\%d\\2 (1)", m_comboCam.GetCurSel() + 1);
		ReadImage(&pImageWin->m_HW.hoImage, strCam.GetBuffer());
		strCam.ReleaseBuffer();
		pImageWin->m_HW.bLoad = true;
		pImageWin->m_HW.bCam = false;
	}
	if (m_nThread > 0)
	{
		int nCam = m_comboCam.GetCurSel();
		bool bNG = CImgDLL::SafeSnap(pImageWin->m_HW, nCam, 4);
		if (bNG)                                                                                                    //采集图片失败	
		{
			pImageWin->m_HW.bCam = false;
			return;
		}
		pImageWin->m_HW.bCam = true;
	}
}

void CDlgBK::GetLaserVal(CMscomm1& m_Com, int nLsr)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_nRun != RUN_WORK)
		return;
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048];                                     //设置BYTE数组 An 8-bit integerthat is not signed.
	Sleep(90);
	variant_inp = m_Com.get_Input();                 //读缓冲区
	safearray_inp = variant_inp;                         //VARIANT型变量转换为ColeSafeArray型变量     
	len = safearray_inp.GetOneDimSize();          //得到有效数据长度    // 接受数据  
	TRACE("\n %d", len);
	CString strTemp;
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //转换为BYTE型数组     
		BYTE bt = *(char*)(rxdata + k);                           //字符型 	                     
		TRACE(" %02X", bt);                                         //输出检测指令
		strTemp.Format("%02X", bt);                            //将字符送入临时变量strtemp存放
	}
	if (len < 9)
	{
		m_sg[nLsr].iStatus = SIG_NG;
	}
	m_sg[nLsr].fVal = strtoul(strTemp.Mid(6, 8), NULL, 16)*0.0001;
}


BEGIN_EVENTSINK_MAP(CDlgBK, CDialogEx)
	ON_EVENT(CDlgBK, IDC_MSCOMM1, 1, CDlgBK::OnCommMscomm1, VTS_NONE)
	ON_EVENT(CDlgBK, IDC_MSCOMM2, 1, CDlgBK::OnCommMscomm2, VTS_NONE)
END_EVENTSINK_MAP()


void CDlgBK::OnCommMscomm1()
{
	GetLaserVal(m_Com[0], SIG_LLSR);
}


void CDlgBK::OnCommMscomm2()
{
	GetLaserVal(m_Com[1], SIG_RLSR);
}

void CDlgBK::SendLaserOrder(CMscomm1& m_Com)
{
	//01 03 00 01 00 02 95 CB
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	Array.Add(0x01);
	Array.Add(0x03);
	Array.Add(0x00);
	Array.Add(0x01);
	Array.Add(0x00);
	Array.Add(0x02);
	Array.Add(0x95);
	Array.Add(0xCB);
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}

bool CDlgBK::InitialCom(int iNO[2], int nBaud)
{
	for (int i = 0; i < 2; i++)
	{
		m_Com[i].put_CommPort(iNO[i]);                      //设定串口为 COM1 
		CString strBaud, strSetting;
		strSetting.Format(_T("%d,n,8,1"), nBaud);
		m_Com[i].put_Settings(strSetting);                    //设定波特率9600，无奇偶校验，8位数据位，1作为停止位         
		m_Com[i].put_InputMode(1);                              //设定数据接收模式，1为二进制，0为文本         
		m_Com[i].put_InputLen(0);                                 //设定当前接收区数据长度 
		m_Com[i].put_InBufferSize(1024);                    //设置输入缓冲区大小为1024 byte         
		m_Com[i].put_OutBufferSize(1024);                 //设置输出缓冲区大小为1024 byte         
		m_Com[i].put_RThreshold(1);                           //每接收到一个字符时，触发OnComm事件         
		m_Com[i].put_SThreshold(0);                           //每发送一个字符时，不触发OnComm事件
		if (!m_Com[i].get_PortOpen())
		{
			try
			{
				m_Com[i].put_PortOpen(true);//打开串口
			}
			catch (CException* e)
			{
				TCHAR   szError[1024];
				e->GetErrorMessage(szError, 1024);         //e.GetErrorMessage(szError,1024); 
				CImgDLL::WriteLog(szError);
				return false;
			}
			CImgDLL::WriteLog("COM %d 串口打开成功！", iNO[i]);
			continue;
		}
		CImgDLL::WriteLog("串口打开失败！");
		return false;
	}
	return true;
}

void CDlgBK::OnBnClickedBtnOffset()
{
	CDlgOffset dlg;
	dlg.SetRadioText("左工位", IDC_RADIO1);
	dlg.SetRadioText("右工位", IDC_RADIO2);
	dlg.DoModal();
}

void CDlgBK::SetEnable(BOOL bEna /* = TRUE */)
{
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_CAM)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_IMG)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_TRIG)->EnableWindow(bEna);
	m_comboCam.EnableWindow(bEna);
}

void CDlgBK::OnBnClickedBtnRun()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText;
	GetDlgItemText(IDC_BTN_RUN, strText);
	int nCam = 0;
	if (strText == "自动生产")
	{		
		m_nRUNs = 0;
		pSng->_nRun = RUN_LOAD;
		while (pSng->_nRun == RUN_LOAD)
		{
			DoEvent();
		}
		if (pSng->_nRun == RUN_NULL)
			return;
		SetEnable(FALSE);
		GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BTN_RUN, "停止生产");
	}
	else
	{
		pSng->_nRun = RUN_UNLOAD;
		m_nRUNs = m_nThread;
		while (pSng->_nRun == RUN_UNLOAD)
		{
			DoEvent();
		}
		SetDlgItemText(IDC_BTN_RUN, "自动生产");
		SetEnable(TRUE);
	}
}

void CDlgBK::OnBnClickedBtnLog()
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pLog = (CIDlgInfo*)CImgDLL::LogDlg();
	//TRACE("\nLog窗口地址：%d", *pSng->_pLog);
}


void CDlgBK::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSingleton* pSng = CSingleton::GetInstance();
	if (nIDEvent == TIM_LNK)
	{
		for (int i = 0; i < 2; i++)
		{
			if (CImgDLL::SvrLinkStatus(i) == 0)
			{
				m_lbRBT[i].SetBkColor(RGB(255, 0, 0));
				m_lbRBT[i].SetText("未连接");
			}
			else
			{
				m_lbRBT[i].SetBkColor(RGB(0, 255, 0));
				if (i == 0)
					m_lbRBT[i].SetText("CAM2机械手");
				else
					m_lbRBT[i].SetText("CAM3机械手");
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDlgBK::OnBnClickedBtnTrig()
{
	int nCam = m_comboCam.GetCurSel();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_XML);
	m_sg[nCam + 2].sVal.Format("%d", pBtn->GetCheck());
	m_sg[nCam + 2].iStatus = SIG_ON;
}


void CDlgBK::OnBnClickedCheckXml()
{
	// TODO:  在此添加控件通知处理程序代码
}
