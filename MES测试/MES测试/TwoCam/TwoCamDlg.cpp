
// TwoCamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TwoCam.h"
#include "TwoCamDlg.h"
#include "afxdialogex.h"
#include "soapH.h"
#include "IPubliceMesServiceSoap.nsmap"
#include "soapStub.h"
#include "stdsoap2.h"

#include "Hc_Modbus_Api.h"
#pragma comment(lib, "StandardModbusApi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void DoEvent();
extern int g_nExt;

extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);

typedef bool(*pLoadImageXML)(int nID, CString strXML, int& nExp, float& fGain, float& fGam);
typedef bool(*pUnLoadImageXML)(int nID);
typedef bool(*pIsImageOK)(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut);
typedef IHpDllWin* (*hpDllFun)(CString strTxt, CString stXML);
typedef CIDlgJob* (*hpDllJob)(CString strTxt);
typedef CIDlgInfo* (*hpLogDlg)();
typedef CIDlgCamCfg* (*hpCamCfg)();
typedef CIDlgCamCfg* (*pIsCamCfg)();
typedef void (*pSnap)(CHWin& HW, int nCam, int nTimes);
typedef bool (*pSetCamPar)(int nCam, float fExp, float fGain, float fGam);
typedef bool (*pImageXMLClear)();
typedef void (*pWriteLog)(const char *fmt, ...);
typedef bool (*pInitCam)(int iNum);
typedef bool (*pCloseCam)(int iNum);

pSnap  mSnap;
pSetCamPar mSetCamPar;
pCloseCam mCloseCam;
pInitCam mInitCam;
pLoadImageXML mLoadXML;
pUnLoadImageXML mUnLoadXML;
hpDllFun mImageDlg;
hpDllJob mJobDlg;
hpCamCfg mCamCfg;
pIsCamCfg mIsCamCfg;
pIsImageOK mIsImageOK;
hpLogDlg mLogDlg;
pImageXMLClear mImageXMLClear;
pWriteLog mWriteLog;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTwoCamDlg 对话框

CTwoCamDlg::CTwoCamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTwoCamDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTwoCamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM, m_comboCam);
}

BEGIN_MESSAGE_MAP(CTwoCamDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_IMAGE, &CTwoCamDlg::OnBnClickedBtnImage)
	ON_BN_CLICKED(IDC_BTN_REAL, &CTwoCamDlg::OnBnClickedBtnReal)
	ON_BN_CLICKED(IDC_BTN_LOG, &CTwoCamDlg::OnBnClickedBtnLog)
	ON_BN_CLICKED(IDOK, &CTwoCamDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_RUN, &CTwoCamDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_TRG, &CTwoCamDlg::OnBnClickedBtnTrg)
	ON_BN_CLICKED(IDC_BTN_SEND, &CTwoCamDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CTwoCamDlg 消息处理程序
BOOL CTwoCamDlg::GetDLLFunc()
{
	m_hDll = LoadLibrary("DllMod.dll");
	if (NULL == m_hDll)
	{
		MessageBox("加载 MyDLL.dll 失败");
		return FALSE;
	}
	mImageDlg = (hpDllFun)GetProcAddress(m_hDll, "SetImage");
	mIsImageOK = (pIsImageOK)GetProcAddress(m_hDll, "IsImageOK");
	mLoadXML = (pLoadImageXML)GetProcAddress(m_hDll, "LoadImageXML");
	mUnLoadXML = (pUnLoadImageXML)GetProcAddress(m_hDll, "UnLoadImageXML");
	mJobDlg = (hpDllJob)GetProcAddress(m_hDll, "SetThread");
	mLogDlg = (hpLogDlg)GetProcAddress(m_hDll, "GetLogInfo");
	mImageXMLClear = (pImageXMLClear)GetProcAddress(m_hDll, "IsImageXMLClear");
	mInitCam = (pInitCam)GetProcAddress(m_hDll, "InitDLLCam");
	mCloseCam = (pCloseCam)GetProcAddress(m_hDll, "CloseCam");
	mCamCfg = (hpCamCfg)GetProcAddress(m_hDll, "GetDlgCamCfg");
	mIsCamCfg = (pIsCamCfg)GetProcAddress(m_hDll, "GetCamCfgPtr");
	mSnap = (pSnap)GetProcAddress(m_hDll, "Snap");
	if (mSnap == NULL)
	{
		MessageBox("DLL中函数中没有相机采集的函数！");
		return FALSE;
	}
	mSetCamPar = (pSetCamPar)GetProcAddress(m_hDll, "SetCamPar");
	if (NULL == mSetCamPar)
	{
		MessageBox("DLL中函数中没有相机参数设定的函数！");
		return FALSE;
	}
	mWriteLog = (pWriteLog)GetProcAddress(m_hDll, "WriteLog");
	if (NULL == mWriteLog)
	{
		MessageBox("DLL中函数中没有写Log记录的函数！");
		return FALSE;
	}
	if (NULL == mLogDlg)
	{
		MessageBox("DLL中函数中没有Log对话框函数！");
		return FALSE;
	}
	if (NULL == mJobDlg)
	{
		MessageBox("DLL中函数寻找工作流程对话框失败！");
		return FALSE;
	}
	if (NULL == mImageDlg)
	{
		MessageBox("DLL中函数寻找图像窗口对话框失败！");
		return FALSE;
	}
	return TRUE;
}

UINT ThreadCam(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CTwoCamDlg* pDlg = (CTwoCamDlg*)pSng->_pMain;
	int nCam = pDlg->m_nThread;
	pDlg->m_nThread++;
	int nXML = 0;
	int nID[4] = { IDC_STATIC1, IDC_STATIC2, 0, 0 };
	CString strThread, strDir, strLog;
	strThread.Format("CAM%d线程", nCam + 1);
	strDir.Format("D:\\XML\\CAM%d", nCam + 1);   //工作流程文件路径
	bool bUnLoadOK = false;
	BYTE pValue[4] = { 0 };
	vector<CPos> psVal;	
	for (; g_nExt != 1;)
	{
		Sleep(60);
		if (pSng->_nRun == RUN_NULL)
			continue;
		else if (pSng->_nRun == RUN_LOAD)
		{
			CStrs XMLFiles;
			pSng->FindAllFile(strDir, XMLFiles);
			if (XMLFiles.size() == 0)
			{
				pSng->_nRun = RUN_NULL;
				strLog.Format("没有检测到CAM%d 的工作流程XML文件", nCam + 1);
				mWriteLog(strLog);
				pDlg->MessageBox(strLog, "", MB_ICONERROR);
				continue;
			}
			//////////////////////////////////载入工作流程//////////////////////////////
			bool bLoad = false;
			int nExp;
			float fGain, fGamma;
			nXML = 0;
			for (int i = 0; i < (int)XMLFiles.size(); i++)
			{
				bLoad = mLoadXML(nCam * 10 + i, XMLFiles[i], nExp, fGain, fGamma);
				if (bLoad == false)
					break;
				nXML++;
			}
			if (bLoad == false)
			{
				pSng->_nRun = RUN_NULL;
				continue;
			}
			/////////////////XML 文件载入失败////////////////////////////
			if (pDlg->m_nThread == 0)
			{
				ReadImage(&pDlg->m_HWRun[nCam].hoImage, "D:\\1.jpg");
			}
			else
			{
				//把相机曝光，增益，都设置一下
				CString strCam;
				strCam.Format("CAM%d", nCam + 1);
				nExp = atoi(pSng->GetCfgString("曝光", strCam, "3000"));
				fGain = atof(pSng->GetCfgString("增益", strCam, "1.0"));
				fGamma = atof(pSng->GetCfgString("伽马", strCam, "1.0"));
				if (mSetCamPar(nCam, (float)nExp, fGain, fGamma) == false)
				{
					pSng->_nRun = RUN_NULL;
					continue;
				}
				mSnap(pDlg->m_HWRun[nCam], nCam, 4);    //拍一张照片触发尺寸
			}
			///////////////////////////触发相机采集图像图像////////////////////////////////////////////
			CRect rc;
			pDlg->GetDlgItem(nID[nCam])->GetClientRect(&rc);
			pDlg->m_HWRun[nCam].RealSize(rc, "自动生产模式！", "green");
			pSng->_nRun = RUN_WORK;
			bUnLoadOK = false;                                     //已经载入XML
			continue;
		}
		else if (pSng->_nRun == RUN_UNLOAD && bUnLoadOK == false)
		{
			for (int i = 0; i < nXML; i++)			                       //卸载图像检测内容
			{
				mUnLoadXML(10 * nCam + i);
			}
			bUnLoadOK = true;
			continue;
		}
		////////////////////////////////////////////
		if (pDlg->m_nCHK[nCam] == 0)                           //收到检测指令
			continue;			
		pDlg->m_nCHK[nCam] = 0;
		int nShiftTime = pSng->GetCfgInt("参数", "换料时长", 800);			
		psVal.clear();
		int nCount = 1;
		if (nCam == 0)
		{
			mSnap(pDlg->m_HWRun[nCam], nCam, 4);
			int nStartAddr = pSng->GetCfgInt("参数", "旋转角度", 120);
			int nResAddr = pSng->GetCfgInt("参数", "结果地址", 101);
			double fDegST = atof(pSng->GetCfgString("参数", "标准角度", "90"));
			DWORD dwST = GetTickCount();
			bool bOK = mIsImageOK(pDlg->m_HWRun[nCam], 10 * nCam, false, psVal);
			if (bOK)
			{
				double fDeg = atof(psVal[0].sgOut[0].sVal);
				double fDelta[2], fRot;
				fDelta[0] = fDegST - fDeg;
				fDelta[1] = fDegST - fDeg - 360;
				if (abs(fDelta[0]) < abs(fDelta[1]))
					fRot = fDelta[0];
				else
					fRot = fDelta[1];
				int nValue = (int)(fRot * 10);
				memcpy(pValue, &nValue, sizeof(nValue));
				pSng->_csPLC.Lock();
				int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nStartAddr, nCount, pValue);
				nValue = 1;
				memcpy(pValue, &nValue, sizeof(nValue));
				nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nResAddr, nCount, pValue);
				pSng->_csPLC.Unlock();
				mWriteLog("CAM%d 检测OK,发送角度%.1f,耗时：%d ms", nCam + 1, fRot, GetTickCount() - dwST);
			}
			else
			{
				int nValue = 2;
				memcpy(pValue, &nValue, sizeof(nValue));
				pSng->_csPLC.Lock();
				int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nResAddr, nCount, pValue);
				pSng->_csPLC.Unlock();
				mWriteLog("CAM%d 检测NG", nCam + 1);
			}
		}
		else
		{
			char cSend[16];
			ZeroMemory(cSend, sizeof(cSend));
			CString strSend = pSng->GetCfgString("参数", "条码查询", "<?q>");
			for (int i = 0; i < strSend.GetLength();i++)
			{
				cSend[i] = strSend.GetAt(i);
			}
			int ret = pDlg->m_Lasr.MySend(cSend, strSend.GetLength());
			char RecvBuff[64];
			ret = recv(pDlg->m_Lasr.m_socket, RecvBuff, 64, 0);
			bool bCode = false;
			pDlg->m_strCode.Empty();
			for (int i = ret; i >= 0;i--)
			{
				if (bCode == false)
				{
					RecvBuff[i] = '>';
					bCode = true;
					continue;
				}
				if (bCode)
				{
					if (RecvBuff[i] == ',')
						break;
					if (RecvBuff[i] >= '0' && RecvBuff[i] <= '9' || 
						RecvBuff[i] >= 'A' && RecvBuff[i] <= 'Z' || 
						RecvBuff[i] >= 'a' && RecvBuff[i] <= 'z' || 
						RecvBuff[i] == '+' && RecvBuff[i] == '-')
					{
						pDlg->m_strCode.Insert(0, RecvBuff[i]);
					}
				}
			}
			/////////////////////////////////////////启动打标机开始打标///////////////////////////////////////////
			strSend = pSng->GetCfgString("参数", "打标指令", "<X>");
			ZeroMemory(cSend, sizeof(cSend));
			for (int i = 0; i < strSend.GetLength(); i++)
			{
				cSend[i] = strSend.GetAt(i);
			}
			ret = pDlg->m_Lasr.MySend(cSend, strSend.GetLength());
			ZeroMemory(RecvBuff,sizeof(RecvBuff));
			bool bOut = false;
			DWORD dwSend = GetTickCount();
			while (bOut == false && GetTickCount() - dwSend > 3000)
			{
				Sleep(200);
				ret = recv(pDlg->m_Lasr.m_socket, RecvBuff, 64, 0);
				for (int i = 0; i < 63 ;i++)
				{
					if (RecvBuff[i] == 'X' && RecvBuff[i + 1] == 'E')
					{
						bOut = true;
						break;
					}
				}
			}
			mSnap(pDlg->m_HWRun[nCam], nCam, 4); 
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//ReadImage(&pDlg->m_HWRun[nCam].hoImage, "D:\\code.bmp");
			int nResAddr = pSng->GetCfgInt("参数", "激光结果", 111);                                                 //收到检测指令
			bool bOK = mIsImageOK(pDlg->m_HWRun[nCam], 10 * nCam, false, psVal);
			if (bOK == false)
			{
				mWriteLog("条码没有识别出来！  激光机当前打码：%s",pDlg->m_strCode);				//发送NG
				pSng->_csPLC.Lock();
				int nValue = 2;
				memcpy(pValue, &nValue, sizeof(nValue));
				int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nResAddr, nCount, pValue);
				pSng->_csPLC.Unlock();
				Sleep(nShiftTime);
				pDlg->m_nCHK[nCam] = 0;
				continue;
			}
			CString strCog;
			strCog.Format("%s",psVal[0].sgOut[0].sVal);
			if (strCog == pDlg->m_strCode)
			{
				//CString cstrSN, cstrEmpNo, cstrStationNumber, cstrTestResult, cstrTestData;
				//cstrEmpNo = pSng->GetCfgString("参数", "MES工号", "22160037");
				//cstrStationNumber = pSng->GetCfgString("参数", "MES工站", "600039");
				//cstrSN = strCog;
				//cstrTestData.Format("RESULT:PASS; ORTNoise1:1");
				//cstrTestResult.Format("OK");
				//int nMes = 0;
				//nMes =pDlg->sendMESData(cstrSN, cstrStationNumber, cstrEmpNo, cstrTestData);
				//nMes = pDlg->sendMESResult(cstrSN, cstrStationNumber, cstrEmpNo, cstrTestResult);
				//if (nMes ==0)
				//{
				//	pSng->_csPLC.Lock();
				//	int nValue = 2;
				//	memcpy(pValue, &nValue, sizeof(nValue));
				//	int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nResAddr, nCount, pValue);
				//	pSng->_csPLC.Unlock();
				//}
				mWriteLog("识别出的条码：%s  OK！", strCog);				
			}
			else
			{
				pSng->_csPLC.Lock();
				int nValue = 2;
				memcpy(pValue, &nValue, sizeof(nValue));
				int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nResAddr, nCount, pValue);
				pSng->_csPLC.Unlock();
				mWriteLog("识别出的条码：%s 与激光机当前打码：%s 不一致！", strCog,pDlg->m_strCode);				//发送NG
			}
		}
		Sleep(nShiftTime);
	}
	return 0;
}

UINT ThreadWatch(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CTwoCamDlg* pDlg = (CTwoCamDlg*)pSng->_pMain;
	SoftElemType eType = REGI_H3U_DW;
	int nAddr[2];
	nAddr[0] = pSng->GetCfgInt("参数", "角度地址", 100);
	nAddr[1] = pSng->GetCfgInt("参数", "激光地址", 110);
	const int nNetId = 0;
	unsigned short nValueRD[4];
	int nCount = 4;
	BYTE pValue[8];
	DWORD dwHeat = GetTickCount();
	for (; g_nExt != 1;)
	{
		Sleep(100);
		if (GetTickCount() - dwHeat > 60000)
		{
			mWriteLog("-----一个心跳完成----");
			dwHeat = GetTickCount();
		}
		if (pSng->_nRun == RUN_WORK)
		{
			pSng->_csPLC.Lock();
			for (int i = 0; i < 2; i++)
			{
				int nRet = H3u_Read_Soft_Elem(eType, nAddr[i], nCount, pValue, nNetId);
				memcpy(&nValueRD, pValue, sizeof(nValueRD));
				if (nValueRD[0] > 0)
				{
					pDlg->m_nCHK[i] = 1;
					ZeroMemory(pValue, sizeof(pValue));
					int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddr[i], nCount, pValue);
					mWriteLog("D%d 收到检测指令", nAddr[i]);
				}
				else
				{
					pDlg->m_nCHK[i] = 0;
				}
			}
			pSng->_csPLC.Unlock();
		}
	}
	return 0;
}

BOOL CTwoCamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pMain = this;
	/************************************************************************/
	/* 字符串转化成2，8，10，16进制数                                          
	/************************************************************************/
	CString strHex = "FFFEF";
   unsigned long lVal = _tcstol(strHex, 0, 16);
   /////////////////////////////////////
	for (int i = 0; i < 2;i++)
	{
		CString strCam;
		strCam.Format("CAM%d",i + 1);
		m_comboCam.AddString(strCam);
	}
	m_comboCam.SetCurSel(0);
	BOOL bOK = m_Lasr.InitSocket();
	if (bOK == false)
	{
		MessageBox(_T("激光器初始化Socket失败"), _T("系统提示"), MB_ICONWARNING);
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	int nPort = pSng->GetCfgInt("参数", "激光端口", 2000);
	CString strIP = pSng->GetCfgString("参数", "激光IP", "192.168.1.106");
	if (m_Lasr.ConnectPort(strIP, nPort) == false)
	{
		MessageBox(_T("激光器连接失败！"), _T("系统提示"), MB_ICONWARNING);
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	////////////////////////////////////////////////////////////////////////////////
	int nNetId = 0;
	int nIpPort = pSng->GetCfgInt("参数","PORT",502);
	pSng->_strIP = pSng->GetCfgString("参数","IP","192.168.3.111");
	BOOL  bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
	pSng->_strIP.ReleaseBuffer();
	if (!bRet)
	{
		AfxMessageBox(_T("PLC连接失败"));
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	//pSng->_csPLC.Lock();
	//int nCount = 1;
	//BYTE pValue[2];
	//int nValue = 1;
	//memcpy(pValue, &nValue, sizeof(nValue));
	//int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, 101, nCount, pValue);
	//nValue = -30;
	//memcpy(pValue, &nValue, sizeof(nValue));
	//nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, 120, nCount, pValue);
	//pSng->_csPLC.Unlock();
	///////////////////////////////////////////////////////////////
	if (GetDLLFunc() == FALSE)
	{
		MessageBox("载入DLL失败!");
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	CIDlgInfo* pDlg = mLogDlg();
	//////////////////////////////////////////////////////////////////////
	mWriteLog("软件主对话框初始化成功！");
	pSng->CreateDir("D:\\INI");
	pSng->CreateDir("D:\\TMP");
	pSng->CreateDir("D:\\Image");
	pSng->CreateDir("D:\\XML");
	vector<int> nIDs;
	nIDs.push_back(IDC_STATIC1);
	nIDs.push_back(IDC_STATIC2);	
	for (int i = 0; i < (int)nIDs.size(); i++)
	{
		m_HWRun[i].psRD.x = 1280;
		m_HWRun[i].psRD.y = 1024;
		m_HWRun[i].InitWnd(GetDlgItem(nIDs[i]));
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		SetColor(m_HWRun[i].WndID, "green");
		SetLineWidth(m_HWRun[i].WndID, 1);
		int nPosTxt[2] = { 0 };
		m_HWRun[i].ShowImage(nPosTxt,strCam);
	}
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_IMAGE)->EnableWindow(FALSE);
	m_comboCam.ResetContent();
	for (int i = 0; i < (int)nIDs.size(); i++)
	{
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		m_comboCam.AddString(strCam);
		pSng->CreateDir("D:\\XML\\" + strCam);
		m_nThread = i;
		m_pThr[i] = AfxBeginThread(ThreadCam, this);
		bool bOK = mInitCam(i);
		if (bOK == false)
		{
			AfxMessageBox(_T("相机初始化失败"));
			SendMessage(WM_CLOSE);
			return FALSE;
		}
		while (i == m_nThread)
		{
			DoEvent();
		}
		m_comboCam.SetCurSel(0);
	}
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_IMAGE)->EnableWindow(TRUE);
	AfxBeginThread(ThreadWatch, this);
	//CString cstrEmpNo, cstrStationNumber;
	//cstrEmpNo = pSng->GetCfgString("参数", "MES工号", "22160037");
	//cstrStationNumber = pSng->GetCfgString("参数", "MES工站", "600039");
	//UpMESLoad(cstrStationNumber, cstrEmpNo);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTwoCamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTwoCamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTwoCamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CTwoCamDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTwoCamDlg::OnBnClickedBtnImage()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam("CAM1");
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);
	IHpDllWin* pImageWin = mImageDlg(strCam,NULL);
	int nCam = m_comboCam.GetCurSel();
	mSnap(pImageWin->m_HW, nCam, 4);
	//strCam.Format("D:\\code.bmp", m_comboCam.GetCurSel() + 1);
	//ReadImage(&pImageWin->m_HW.hoImage, strCam.GetBuffer());
	//strCam.ReleaseBuffer();
}

void CTwoCamDlg::OnBnClickedBtnReal()
{
	CIDlgCamCfg* pDlgCam = mCamCfg();
	if (pDlgCam == NULL)
	{
		mWriteLog("相机没有初始化成功！");
	}
	SetTimer(TIM_REAL, 100, NULL);
	SetEnable(FALSE);
}

void CTwoCamDlg::OnBnClickedBtnLog()
{
	CIDlgInfo* pDlg = mLogDlg();
}

void CTwoCamDlg::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	g_nExt = 1;
	TRACE("\n 启动软件退出机制！");
	CIDlgInfo* pDlg = mLogDlg();
	pDlg->ShowWindow(SW_HIDE);
	Sleep(60);
	for (int i = 0; i < m_nThread;i++)
	{
		mCloseCam(i);
	}
	Sleep(100);
	FreeLibrary(m_hDll);
	pSng->DeleteInstance();
	CDialogEx::OnOK();
}


void CTwoCamDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIM_REAL)
	{
		if (mIsCamCfg() == NULL)
		{
			KillTimer(nIDEvent);		
			SetEnable(TRUE);
		}
	}
	//else if (nIDEvent == TIM_PLC)
	//{
	//	//定期访问PLC
	//}
	CDialogEx::OnTimer(nIDEvent);
}

void CTwoCamDlg::SetEnable(BOOL bEna)
{
	GetDlgItem(IDC_BTN_IMAGE)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_REAL)->EnableWindow(bEna);
	//GetDlgItem(IDC_BTN_TRG)->EnableWindow(bEna);
	//GetDlgItem(IDOK)->EnableWindow(bEna);
}

void CTwoCamDlg::OnBnClickedBtnRun()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText;
	int nID = IDC_BTN_RUN;
	GetDlgItemText(nID, strText);
	if (strText == "自动生产")
	{
		pSng->_nRun = RUN_LOAD;
		while (pSng->_nRun == RUN_LOAD)
		{
			DoEvent();
		}
		if (pSng->_nRun == RUN_NULL)
			return;
		SetEnable(FALSE);
		GetDlgItem(nID)->EnableWindow(TRUE);
		SetDlgItemText(nID, "停止生产");
		int nNum = m_comboCam.GetCount();
		for (int i = 0; i < nNum;i++)
		{
			m_nCHK[i] = 0;
		}
	}
	else
	{
		pSng->_nRun = RUN_UNLOAD;
		Sleep(500);
		while (false == mImageXMLClear())
		{
			Sleep(100);
			DoEvent();
		}
		pSng->_nRun = RUN_NULL;
		SetEnable(TRUE);
		SetDlgItemText(nID, "自动生产");
	}
}


void CTwoCamDlg::OnBnClickedBtnTrg()
{
	int nSum = m_comboCam.GetCount();
	for (int i = 0; i < nSum; i++)
	{
		if (i == m_comboCam.GetCurSel())
		{
			m_nCHK[i] = 1;
			break;
		}
	}
}

int CTwoCamDlg::UpMESLoad(CString cstrStationNumber, CString cstrEmpNo)
{
	int iRet = 1;
	struct soap userinfoSoap;
	soap_init(&userinfoSoap);

	const char *soap_endpoint;
	const char *soap_action;
	_ns1__checkEmpForStationNumber sendStr;
	_ns1__checkEmpForStationNumberResponse RcvStr;

	//////////////////////////////////////////////////////////////////////////

	CString cstr;
	string strEmp_No, strStation_Number;

	cstr.Format(cstrEmpNo);
	strEmp_No = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Emp_USCORENo = &strEmp_No;

	cstr.Format(cstrStationNumber);
	strStation_Number = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Station_USCORENumber = &strStation_Number;


	iRet = soap_call___ns1__checkEmpForStationNumber(&userinfoSoap, NULL, NULL, &sendStr, RcvStr);
	CString strLog, strRcv;
	if (SOAP_OK != iRet)   //不等于0 函数执行失败
	{
		strLog.Format("MES登录失败！");
		MessageBox(strLog);
		mWriteLog(strLog);
		return 0;
	}
	else
	{
		strRcv.Format("%s", RcvStr.checkEmpForStationNumberResult->c_str());
		soap_destroy(&userinfoSoap);
		soap_end(&userinfoSoap);
		soap_done(&userinfoSoap);
		mWriteLog("MES登录成功，Web 返回iRet = %d , Rcv = %s", iRet, strRcv);
		return 1;
	}
}

int CTwoCamDlg::sendMESData(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestData)
{
	struct soap userinfoSoap;
	soap_init(&userinfoSoap);
	const char *soap_endpoint;
	const char *soap_action;
	_ns1__sendDataForStationNumber sendStr;
	_ns1__sendDataForStationNumberResponse RcvStr;


	string strSN, strStationNumber, strEmpNo, strTestData;
	CString cstr;

	cstr.Format(cstrEmpNo);
	strEmpNo = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Emp_USCORENo = &strEmpNo;


	cstr.Format(cstrSN);
	strSN = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.SN = &strSN;


	cstr.Format(cstrStationNumber);
	strStationNumber = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Station_USCORENumber = &strStationNumber;


	cstr.Format(cstrTestData);
	strTestData = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.TestData = &strTestData;



	int iRet;
	iRet = soap_call___ns1__sendDataForStationNumber(&userinfoSoap, NULL, NULL, &sendStr, RcvStr);
	CString strLog, strRcv;
	if (SOAP_OK != iRet)   //不等于0 函数执行失败
	{
		strLog.Format("MES发送数据失败！");
		MessageBox(strLog);		
		mWriteLog(strLog);
		return 0;
	}
	else
	{
		strRcv.Format("%s", RcvStr.sendDataForStationNumberResult->c_str());
		soap_destroy(&userinfoSoap);
		soap_end(&userinfoSoap);
		soap_done(&userinfoSoap);
		mWriteLog("MES上传成功！Web 返回iRet = %d , Rcv = %s", iRet, strRcv);
		return 1;
	}
}

int CTwoCamDlg::checkMESSN(CString cstrSN, CString cstrStationNumber)
{
	struct soap userinfoSoap;
	soap_init(&userinfoSoap);

	const char *soap_endpoint;
	const char *soap_action;
	_ns1__checkSN_USCOREStationNumber sendStr;
	_ns1__checkSN_USCOREStationNumberResponse RcvStr;

	CString cstr;
	string strSN, strStation_Number;

	cstr.Format(cstrSN);
	strSN = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.SN = &strSN;

	cstr.Format(cstrStationNumber);
	strStation_Number = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Station_USCORENumber = &strStation_Number;

	int iRet;
	iRet = soap_call___ns1__checkSN_USCOREStationNumber(&userinfoSoap, NULL, NULL, &sendStr, RcvStr);
	CString strLog, strRcv;
	if (SOAP_OK != iRet)   //不等于0 函数执行失败
	{
		strLog.Format("MES条码工序校验失败！");
		MessageBox(strLog);
		mWriteLog(strLog);
		return 0;
	}
	else
	{
		strRcv.Format("%s", RcvStr.checkSN_USCOREStationNumberResult->c_str());
		soap_destroy(&userinfoSoap);
		soap_end(&userinfoSoap);
		soap_done(&userinfoSoap);
		mWriteLog("MES条码工序校验成功！Web 返回iRet = %d , Rcv = %s", iRet, strRcv);
		return 1;
	}


}

int CTwoCamDlg::sendMESResult(CString cstrSN, CString cstrStationNumber, CString cstrEmpNo, CString cstrTestResult)
{
	struct soap userinfoSoap;
	soap_init(&userinfoSoap);
	const char *soap_endpoint;
	const char *soap_action;
	_ns1__sendResultForStationNumber sendStr;
	_ns1__sendResultForStationNumberResponse RcvStr;


	string strSN, strStationNumber, strEmpNo, strTestResult;
	CString cstr;

	cstr.Format(cstrEmpNo);
	strEmpNo = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Emp_USCORENo = &strEmpNo;


	cstr.Format(cstrSN);
	strSN = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.SN = &strSN;


	cstr.Format(cstrStationNumber);
	strStationNumber = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.Station_USCORENumber = &strStationNumber;


	cstr.Format(cstrTestResult);
	strTestResult = cstr.GetBuffer();
	cstr.ReleaseBuffer();
	sendStr.TestResult = &strTestResult;



	int iRet;
	iRet = soap_call___ns1__sendResultForStationNumber(&userinfoSoap, NULL, NULL, &sendStr, RcvStr);
	CString strLog, strRcv;
	if (SOAP_OK != iRet)   //不等于0 函数执行失败
	{
		strLog.Format("MES条码過站失败！");
		MessageBox(strLog);
		mWriteLog(strLog);
		return 0; 
	}
	else
	{
		strRcv.Format("%s", RcvStr.sendResultForStationNumberResult->c_str());
		soap_destroy(&userinfoSoap);
		soap_end(&userinfoSoap);
		soap_done(&userinfoSoap);
		mWriteLog("MES条码過站成功！Web 返回iRet = %d , Rcv = %s", iRet, strRcv);
		return 1;
	}

}


void CTwoCamDlg::OnBnClickedBtnSend()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString cstrSN, cstrEmpNo, cstrStationNumber, cstrTestResult, cstrTestData;
	cstrEmpNo = pSng->GetCfgString("参数", "MES工号", "22160037");
	cstrStationNumber = pSng->GetCfgString("参数", "MES工站", "600039");
	//cstrStationNumber.Format("A客户 ORT Noise1");
	cstrSN.Format("1930M0124G00005");
	cstrTestData.Format("RESULT:PASS; ORTNoise1:1");
	cstrTestResult.Format("OK");
	checkMESSN(cstrSN, cstrStationNumber);
	sendMESData(cstrSN, cstrStationNumber, cstrEmpNo, cstrTestData);
	sendMESResult(cstrSN, cstrStationNumber, cstrEmpNo, cstrTestResult);
}
