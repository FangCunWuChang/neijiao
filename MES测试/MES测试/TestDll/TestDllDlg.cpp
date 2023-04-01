
// TestDllDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDll.h"
#include "TestDllDlg.h"
#include "afxdialogex.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "..\DllMod\IDlgCamCfg.h"
#include "..\DllMod\Singleton.h"

#include "Hc_Modbus_Api.h"
#pragma comment(lib, "StandardModbusApi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void DoEvent();
extern int g_nExt;
extern void LogFile(CString log);
extern void LogFile(const char *fmt, ...);

extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
extern BOOL GetDLLFunc(HINSTANCE& m_hDll);

typedef bool(*pLoadImageXML)(int nID, CString strXML, int& nExp, float& fGain, float& fGam);
typedef bool(*pUnLoadImageXML)(int nID);
typedef bool(*pIsImageOK)(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut);
typedef IHpDllWin* (*hpDllFun)(CString strTxt, CString stXML);
typedef CIDlgJob* (*hpDllJob)(CString strTxt);
typedef CIDlgInfo* (*hpLogDlg)();
typedef CIDlgCamCfg* (*hpCamCfg)();
typedef CIDlgCamCfg* (*pIsCamCfg)();
typedef void(*pSnap)(CHWin& HW, int nCam, int nTimes);
typedef bool(*pSetCamPar)(int nCam, float fExp, float fGain, float fGam);
typedef bool(*pImageXMLClear)();
typedef void(*pWriteLog)(const char *fmt, ...);
typedef bool(*pInitCam)(int iNum);
typedef  bool(*pCloseCam)(int iNum);

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


UINT ThreadWatch(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CTestDllDlg* pDlg = (CTestDllDlg*)pSng->_pMain;			
	SoftElemType eType = REGI_H3U_DW;
	const int nStartAddr = pSng->GetCfgInt("参数","起始地址",100);			
	const int nNetId = 0;			
	unsigned short nValueRD[4];
	int nCount = 4;
	BYTE pValue[8];
	DWORD dwHeart = GetTickCount();
	for (;g_nExt != 1;)
	{
		if (GetTickCount() - dwHeart > 60000)
		{
			dwHeart = GetTickCount();
			mWriteLog("-----------心跳--------------");
		}
		Sleep(500);
		if (pSng->_nRun == RUN_WORK)
		{
			pSng->_csPLC.Lock();
			int nRet = H3u_Read_Soft_Elem(eType, nStartAddr, nCount, pValue, nNetId);
			memcpy(&nValueRD, pValue, sizeof(nValueRD));
			//SoftElemType eType = REGI_H3U_DW;
			//int nStartAddr = pSng->GetCfgInt("参数", "起始地址", 100);
			//int nCount = 2;
			//BYTE pValue[4];
			//unsigned short nValue[2] = { 100 ,10};
			//memcpy(pValue, &nValue, sizeof(nValue));
			//int nRetWR = H3u_Write_Soft_Elem(eType, nStartAddr, nCount, pValue, nNetId);
			//int nRetRD = H3u_Read_Soft_Elem(eType, nStartAddr, nCount, pValue, nNetId);
			////unsigned short nValue;
			//ZeroMemory(nValue, sizeof(nValue));
			//memcpy(&nValue, pValue, sizeof(nValue));
			//两个字符表示一个D字符
			for (int i = 0; i < 4;i++)
			{
				if (nValueRD[i] > 0 && nValueRD[i] < 100)
				{
					pDlg->m_nCHK[i] = 1;
				}
			}
			pSng->_csPLC.Unlock();
		}
	}
	return 0 ;
}

UINT ThreadCam(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CTestDllDlg* pDlg = (CTestDllDlg*)pSng->_pMain;
	int nCam = pDlg->m_nThread;
	pDlg->m_nThread++;
	int nXML = 0;
	int nID[4] = { IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4 };
	CString strThread,strDir,strLog;
	strThread.Format("CAM%d线程", nCam + 1);
	strDir.Format("D:\\XML\\CAM%d",nCam + 1);   //工作流程文件路径
	bool bUnLoadOK = false;
	SoftElemType eType = REGI_H3U_DW;
	int nStartAddr = pSng->GetCfgInt("参数", "起始地址", 100) + nCam;
	int nCount = 1;
	BYTE pValue[2];
	unsigned short nValue;
	vector<CPos> psVal;    //DLL图像流程的返回值
	int nExp;
	float fGain, fGamma;
	for (;g_nExt != 1;)
	{
		Sleep(9);
		if (pSng->_nRun == RUN_NULL)
			continue;
		else if (pSng->_nRun == RUN_LOAD)
		{
			CStrs XMLFiles;
			pSng->FindAllFile(strDir, XMLFiles);
			if (XMLFiles.size() == 0)
			{
				pSng->_nRun = RUN_NULL;
				strLog.Format("没有检测到CAM%d 的工作流程XML文件",nCam + 1);
				mWriteLog(strLog);
				pDlg->MessageBox(strLog,"",MB_ICONERROR);
				continue;
			}
			//////////////////////////////////载入工作流程//////////////////////////////
			bool bLoad = false;
			nXML = 0;
			for (int i = 0; i < (int)XMLFiles.size();i++)
			{
				if (nCam == 2)
				{
					Sleep(1);
				}
				bLoad = mLoadXML(nCam*10 + i,XMLFiles[i],nExp,fGain,fGamma);                            
				if (bLoad == false)
					break;
				nXML++;
			}
			if (bLoad == false)
			{
				pSng->_nRun = RUN_NULL;
			}
			else
			{
				//CString strBmp;
				//strBmp.Format("D:\\PIC\\%d.bmp",nCam + 1);
				//ReadImage(&pDlg->m_HWRun[nCam].hoImage, strBmp.GetBuffer(0));
				//strBmp.ReleaseBuffer();
				///////////////////////触发相机采集图像图像////////////////////////////////////////////
				if (mSetCamPar(nCam, (float)nExp, fGain, fGamma) == false)
				{
					pSng->_nRun = RUN_NULL;
					continue;
				}
				mSnap(pDlg->m_HWRun[nCam], nCam, 4);                                                //拍一张照片触发尺寸
				CRect rc;
				pDlg->GetDlgItem(nID[nCam])->GetClientRect(&rc);
				pDlg->m_HWRun[nCam].RealSize(rc, "",false);
				pSng->_nRun = RUN_WORK;
				bUnLoadOK = false;
			}				
			continue;
		}
		else if (pSng->_nRun == RUN_UNLOAD && bUnLoadOK == false)
		{
			for (int i = 0; i < nXML;i++)			                      //卸载图像检测内容
			{
				mUnLoadXML(10*nCam + i);
			}
			bUnLoadOK = true;
			continue;
		}
		if (pDlg->m_nCHK[nCam] == 0)                           //收到检测指令
			continue;	
		pDlg->m_nCHK[nCam] = 0;
		if (nCam == 0)
		{
			Sleep(500);
		}
		mSnap(pDlg->m_HWRun[nCam], nCam, 4);      //拍一张照片触发尺寸
		//CString strCam;
		//strCam.Format("D:\\PIC\\%d.bmp", nCam + 1);
		//ReadImage(&pDlg->m_HWRun[nCam].hoImage, strCam.GetBuffer());
		//strCam.ReleaseBuffer();
		//pDlg->m_HWRun[nCam].ShowImage();
		int nTimes = -1;
		int nTmpExp = nExp;
		for (int i = 0; i < nXML;i++)
		{
A:			nTimes++;
			bool bOK = mIsImageOK(pDlg->m_HWRun[nCam], 10 * nCam, false, psVal);
			if (nCam == 0 && bOK == false && psVal.size() > 0)
			{
				if (nTimes > 5)
					break;
				double fMean = atof(psVal[0].sgOut[0].sVal);
				if (fMean < 90)
				{
					nTmpExp += 1000;
					mSetCamPar(nCam, (float)(nTmpExp), fGain, fGamma);
				}
				else if (fMean > 180)
				{
					nTmpExp -= 1000;
					if (nTmpExp < 100)
						nTmpExp = 100;
					mSetCamPar(nCam, (float)(nTmpExp), fGain, fGamma);
				}
				mSnap(pDlg->m_HWRun[nCam], nCam, 4);      //拍一张照片触发尺寸
				goto A;
			}
			nValue = 100;//发送OK信号
			if (bOK == false)
			{
				nValue = 200;  //NG
			}
			memcpy(pValue, &nValue, sizeof(nValue));
			pSng->_csPLC.Lock();
			int nRetWR = H3u_Write_Soft_Elem(eType, nStartAddr, nCount, pValue);
			pSng->_csPLC.Unlock();
			mWriteLog("%s 发送 %d,返回:%d",strThread,nValue,nRetWR);
			if (nCam == 0)   //重新设置相机的曝光增益
				mSetCamPar(nCam, (float)nExp, fGain, fGamma);
			//////////////////////////////////////////////////////////////////////////////////////
			CStrs Vals;
			CString strCam,strTime;
			strCam.Format("CAM%d",nCam + 1);
			CTime tm = CTime::GetCurrentTime();
			strTime = tm.Format("%H:%M:%S");
			Vals.push_back(strCam);
			Vals.push_back(strTime);
			if (nCam == 0 || nCam == 1 || nCam == 2)       //输出直径
			{
				if (nCam == 0)
				{
					if (psVal.size() == 2)              //直径
					{
						CString strVal("0.00");
						double fVal = atof(psVal[1].sgOut[0].sVal);
						if (fVal > 4 && fVal < 6)
							strVal.Format("%.2f",fVal);
						Vals.push_back(strVal);  //输出psVal[0]是--灰度，psVal[1] 才是直径
					}
					else
						Vals.push_back("0.00");
				}
				else if (nCam > 0)        //灰度或者PIN间距
				{
					if (psVal.size() == 1)
					{
						CString strVal;
						double fVal = atof(psVal[0].sgOut[0].sVal);
						strVal.Format("%.2f", fVal);
						Vals.push_back(strVal); 
					}
					else
						Vals.push_back("0.00");
				}
			}
			else
			{
				for (int i = 0; i < 2;i++)
				{
					if (i >= psVal.size())
					{
						Vals.push_back("0.00");
					}
					else
						Vals.push_back(psVal[i].sgOut[0].sVal);  //左右拨片到中心的距离
				}
			}
			if (bOK)
				Vals.push_back("OK");
			else
				Vals.push_back("NG");
			pSng->Write2CSV(nCam,Vals);
		}
		Sleep(600);
	}
	mWriteLog("%s 已经退出！",strThread);
	return 0;
}
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
	//DDX_Control(pDX, IDC_COMBO_FUNC, m_comboFunc);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestDllDlg 对话框

CTestDllDlg::CTestDllDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDllDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nThread = 0;
}

void CTestDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM, m_comboCam);
	DDX_Control(pDX, IDC_MSCOMM1, m_Com);
}

BEGIN_MESSAGE_MAP(CTestDllDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestDllDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_IMAGE, &CTestDllDlg::OnBnClickedBtnImage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_JOB, &CTestDllDlg::OnBnClickedBtnJob)
	ON_BN_CLICKED(IDC_BTN_RUN, &CTestDllDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_MANU, &CTestDllDlg::OnBnClickedBtnManu)
	ON_BN_CLICKED(IDC_BTN_TRIG, &CTestDllDlg::OnBnClickedBtnTrig)
	ON_BN_CLICKED(IDC_BTN_LOG, &CTestDllDlg::OnBnClickedBtnLog)
	ON_BN_CLICKED(IDC_BTN_CAM, &CTestDllDlg::OnBnClickedBtnCam)
END_MESSAGE_MAP()


// CTestDllDlg 消息处理程序

BOOL CTestDllDlg::OnInitDialog()
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


	int nPos = 256*11 + 99;
	int nVal = nPos >> 8;
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pMain = this;
	if (GetDLLFunc(m_hDll) == FALSE)
	{
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	return TRUE;
	int nNetId = 0;
	int nIpPort = 502;
	BOOL bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
	pSng->_strIP.ReleaseBuffer();
	if (!bRet)
	{
		AfxMessageBox(_T("PLC连接失败"));
		SendMessage(WM_CLOSE);
		return FALSE;
	}

	CIDlgInfo* pDlg = mLogDlg();
	g_nExt = 0;
	mWriteLog("PLC连接OK，软件主对话框初始化成功！");
	pSng->CreateDir("D:\\INI");
	pSng->CreateDir("D:\\TMP");
	pSng->CreateDir("D:\\Image");
	pSng->CreateDir("D:\\XML");                      //工作流程xml文件
	vector<int> nIDs;
	nIDs.push_back(IDC_STATIC1);
	nIDs.push_back(IDC_STATIC2);
	nIDs.push_back(IDC_STATIC3);
	nIDs.push_back(IDC_STATIC4);
	m_comboCam.ResetContent();
	for (int i = 0; i < (int)nIDs.size(); i++)
	{
		m_HWRun[i].psRD.x = 4024;
		m_HWRun[i].psRD.y = 3036;
		m_HWRun[i].InitWnd(GetDlgItem(nIDs[i]), 50, 10);
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		m_comboCam.AddString(strCam);
		SetColor(m_HWRun[i].WndID, "green");
		SetLineWidth(m_HWRun[i].WndID, 1);
		m_HWRun[i].ShowImage(strCam);
	}
	//return TRUE;
	SetEnable(FALSE);
	for (int i = 0; i < (int)nIDs.size(); i++)
	{		
		CString strCam;
		strCam.Format("CAM%d", i + 1);
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
	}
	m_comboCam.SetCurSel(0);
	SetEnable(TRUE);
	//SetTimer(TIM_INI, 100, NULL);
	//return 1;
	AfxBeginThread(ThreadWatch,this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestDllDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestDllDlg::OnPaint()
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
HCURSOR CTestDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "afxdll_.h"
void CTestDllDlg::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	g_nExt = 1;
	TRACE("\n 启动软件退出机制！");
	for (int i = 0; i < m_nThread;i++)
	{
		mCloseCam(i);
	}
	Sleep(100);
	Exit_ETH();
	FreeLibrary(m_hDll);
	pSng->DeleteInstance();
	CDialogEx::OnOK();
}


void CTestDllDlg::OnBnClickedBtnImage()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam;
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);	
	IHpDllWin* pImageWin = mImageDlg(strCam, NULL);
	pSng->CreateDir("D:\\XML\\" + strCam);
	//////触发相机采集图片，最好采用保存图片，用Gain，Exposure，Gamma 命名文件名//////
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMG);   //是否用保存的图片
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		strCam.Format("D:\\PIC\\%d.bmp", m_comboCam.GetCurSel() + 1);
		ReadImage(&pImageWin->m_HW.hoImage, strCam.GetBuffer());
		strCam.ReleaseBuffer();
		pImageWin->m_HW.bCam = false;
	}
	if (m_nThread > 0)
	{
		int nCam = m_comboCam.GetCurSel();
		mSnap(pImageWin->m_HW, nCam, 4);
		pImageWin->m_HW.bCam = true;
	}
}

void CTestDllDlg::OnBnClickedBtnJob()
{
	if (m_comboCam.GetCount() == 0)
	{
		CIDlgJob* pJob = mJobDlg("");
		return;
	}
	CString strCam;
	m_comboCam.GetLBText(m_comboCam.GetCurSel(), strCam);
	CIDlgJob* pJob = mJobDlg(strCam);
}

void CTestDllDlg::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (nIDEvent == TIM_INI)  //初始化
	{
	}
	if (nIDEvent == TIM_REAL)
	{
		if (mIsCamCfg() == NULL)
		{
			KillTimer(nIDEvent);
			SetEnable(TRUE);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CTestDllDlg, CDialogEx)
	ON_EVENT(CTestDllDlg, IDC_MSCOMM1, 1, CTestDllDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CTestDllDlg::OnCommMscomm1()
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_nRun != RUN_WORK)
		return;
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048];                                     //设置BYTE数组 An 8-bit integerthat is not signed.
	variant_inp = m_Com.get_Input();                 //读缓冲区
	safearray_inp = variant_inp;                         //VARIANT型变量转换为ColeSafeArray型变量     
	len = safearray_inp.GetOneDimSize();          //得到有效数据长度    // 接受数据  
	TRACE("\n %d", len);
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //转换为BYTE型数组     
		BYTE bt = *(char*)(rxdata + k);                     //字符型 	                     
		TRACE(" %02x", bt);                                   //输出检测指令
	}
	///////////////////基于这个IO盒子就不需要指令了////////////////////
	BYTE btNum = *(char*)(rxdata + 3);                 //字符型 
	if (btNum != 0x00)                                              //确定有检测指令
	{
		for (int i = 0; i < CAM_THREAD;i++)
		{
			CString strCam;
			strCam.Format("CAM%d",i + 1);
			int iNOVal = pSng->GetCfgInt("信号线程",strCam,0);
			if (iNOVal & btNum)
			{
				m_nCHK[i] = 1;
			}
			else
			{
				m_nCHK[i] = 0;
			}
		}
	}
}

void CTestDllDlg::SendFindOrder()
{
	//查询指令 = 0x01 0x02 0x00 0x00 0x00 0x08 0x79 0xCC ,接下来可以采用
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	Array.Add(0x01);
	Array.Add(0x02);
	Array.Add(0x00);
	Array.Add(0x00);
	Array.Add(0x00);
	Array.Add(0x08);
	Array.Add(0x79);
	Array.Add(0xCC);
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}

void CTestDllDlg::SendResult(int nSTS,int nOK)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	if (nOK == 1)
	{	
		Array.Add(0x01);
		Array.Add(0x05);
		Array.Add(0x00);
		Array.Add(0x00 + 2*nSTS);
		Array.Add(0xFF);
		Array.Add(0x00);
		if (nSTS == 0)
		{
			//01 05 00 00 FF 00 8C 3A
			Array.Add(0x8C);
			Array.Add(0x3A);
		}
		else if (nSTS = 1)
		{
			//01 05 00 02 FF 00 2D FA
			Array.Add(0x2D);
			Array.Add(0xFA);
		}
		else if (nSTS == 2)
		{
			//01 05 00 04 FF 00 CD FB
			Array.Add(0xCD);
			Array.Add(0xFB);
		}
	}
	else
	{
		Array.Add(0x01);
		Array.Add(0x05);
		Array.Add(0x00);
		Array.Add(0x01 + 2*nSTS);
		Array.Add(0xFF);
		Array.Add(0x00);
		if (nSTS == 0)
		{
			//01 05 00 01 FF 00 DD FA
			Array.Add(0xDD);
			Array.Add(0xFA);
		}
		else if (nSTS = 1)
		{
			//01 05 00 03 FF 00 7C 3A
			Array.Add(0x7C);
			Array.Add(0x3A);
		}
		else if (nSTS == 2)
		{
			//01 05 00 05 FF 00 9C 3B
			Array.Add(0x9C);
			Array.Add(0x3B);
		}
	}
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}

void CTestDllDlg::ResetResult(int nSTS)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;	
	Array.Add(0x01);
	Array.Add(0x05);
	Array.Add(0x00);		
	Array.Add(0x00 + 2 * nSTS);		
	Array.Add(0x00);
	Array.Add(0x00);
	if (nSTS == 0)
	{
		//01 05 00 00 00 00 CD CA
		Array.Add(0xCD);
		Array.Add(0xCA);
		m_Com.put_InBufferCount(0);
		m_Com.put_Output(COleVariant(Array));
		Sleep(100);
		//01 05 00 01 00 00 9C 0A
		Array.SetAt(3, 0x01 + 2 * nSTS);
		Array.SetAt(6, 0x9C);
		Array.SetAt(7, 0x0A);
	}
	else if (nSTS == 1)
	{
		//01 05 00 02 00 00 6C 0A
		Array.Add(0x6C);
		Array.Add(0x0A);
		m_Com.put_InBufferCount(0);
		m_Com.put_Output(COleVariant(Array));
		Sleep(100);
		//01 05 00 03 00 00 3D CA
		Array.SetAt(3, 0x01 + 2 * nSTS);
		Array.SetAt(6, 0x3D);
		Array.SetAt(7, 0xCA);
	}
	else if (nSTS == 2)
	{
		//01 05 00 04 00 00 8C 0B
		Array.Add(0x8C);
		Array.Add(0x0B);
		m_Com.put_InBufferCount(0);
		m_Com.put_Output(COleVariant(Array));
		Sleep(100);
		//01 05 00 05 00 00 DD CB
		Array.SetAt(3, 0x01 + 2 * nSTS);
		Array.SetAt(6, 0xDD);
		Array.SetAt(7, 0xCB);
	}
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}

void CTestDllDlg::OnBnClickedBtnRun()
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
		mWriteLog("进入自动生产状态");
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
		mWriteLog("退出自动生产状态");
	}
}

void CTestDllDlg::SetEnable(BOOL bEna)
{
	GetDlgItem(IDC_BTN_CAM)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_IMAGE)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_JOB)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(bEna);
	//GetDlgItem(IDC_BTN_TRIG)->EnableWindow(bEna);
}

void CTestDllDlg::OnBnClickedBtnManu()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strDir;
	strDir.Format("D:\\XML\\CAM%d",m_comboCam.GetCurSel() + 1);
	CStrs XMLs;
	pSng->FindAllFile(strDir,XMLs);
	if (XMLs.size() == 0)
		return;
	//////////////////////否则载入/////////////////////////////
	CString strFile;
	if (XMLs.size() > 1)
	{
	}
	else
	{
		strFile.Format("%s\\%s",strDir,XMLs[0]);               //得到文件
	}
	//////////////////////////触发相机拍照，然后打开图像对话框/////////////////////////
	IHpDllWin* pImageWin = mImageDlg(strFile,strFile);
	//触发相机采集图片，最好采用保存图片，用Gain，Exposure，Gamma 命名文件名
	ReadImage(&pImageWin->m_HW.hoImage, "E:\\7.bmp");
	pImageWin->m_HW.bLoad = true;
}


void CTestDllDlg::OnBnClickedBtnTrig()
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nCam = m_comboCam.GetCurSel();
	SoftElemType eType = REGI_H3U_DW;
	int nStartAddr = pSng->GetCfgInt("参数", "起始地址", 100) + nCam;
	int nCount = 1;
	BYTE pValue[2];
	unsigned short nValue = 1;
	memcpy(pValue, &nValue, sizeof(nValue));
	int nRetWR = H3u_Write_Soft_Elem(eType, nStartAddr, nCount, pValue);
	TRACE("\n 手动发送指令，返回:%d",nRetWR);
	m_nCHK[nCam] = 1;
}


void CTestDllDlg::OnBnClickedBtnLog()
{
	CIDlgInfo* pDlg = mLogDlg();
}

void CTestDllDlg::OnBnClickedBtnCam()
{
	CIDlgCamCfg* pDlgCam = mCamCfg();
	if (pDlgCam == NULL)
	{
		mWriteLog("相机没有初始化成功！");
	}
	SetTimer(TIM_REAL, 100, NULL);
	SetEnable(FALSE);
}
