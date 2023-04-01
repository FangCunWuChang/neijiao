
// QzdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Qzd.h"
#include "QzdDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

extern void DoEvent();
extern int g_nExt;

extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);

typedef bool(*pLoadImageXML)(int nID, CString strXML, int& nExp, float& fGain, float& fGam);
typedef bool(*pUnLoadImageXML)(int nID);
typedef bool(*pIsImageOK)(CHWin& HW, int nID,bool bLogNG);
typedef IHpDllWin* (*hpDllFun)(CString strTxt, CString stXML);
typedef CIDlgJob* (*hpDllJob)(CString strTxt);
typedef CIDlgInfo* (*hpLog)();
typedef bool(*pImageXMLClear)();
typedef void(*pWriteLog)(const char *fmt, ...);
typedef bool(*pInitCam)(int iNum);
typedef bool (*pCloseCam)(int iNum);

pCloseCam mCloseCam;
pInitCam mInitCam;
pLoadImageXML mLoadXML;
pUnLoadImageXML mUnLoadXML;
hpDllFun mImageDlg;
hpDllJob mJobDlg;
pIsImageOK mIsImageOK;
hpLog mLog;
pImageXMLClear mImageXMLClear;
pWriteLog mWriteLog;

void __stdcall ImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	CQzdDlg* pDlg = (CQzdDlg*)pUser;
	pDlg->SetupCallBack(&pData, pFrameInfo, 0);
}

UINT ThreadCam(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CQzdDlg* pDlg = (CQzdDlg*)pSng->_pMain;
	int nCam = pDlg->m_nThread;
	pDlg->m_nThread++;
	int nXML = 0;
	int nID[4] = { IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4 };
	CString strThread, strDir, strLog;
	strThread.Format("CAM%d线程", nCam + 1);
	strDir.Format("D:\\XML\\CAM%d", nCam + 1);   //工作流程文件路径
	bool bUnLoadOK = false;
	for (; g_nExt != 1;)
	{
		Sleep(0);
		if (pSng->_nRun == RUN_NULL)
			continue;
		else if (pSng->_nRun == RUN_LOAD)
		{
			//载入图像XML文件
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
				bLoad = mLoadXML(nCam * 10 + i, XMLFiles[i],nExp,fGain,fGamma);
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
				if (pDlg->m_nThread == 0)
				{
					ReadImage(&pDlg->m_HWRun[nCam].hoImage, "D:\\1.jpg");
				}
				else
				{
					//把相机曝光，增益，都设置一下
					int nRet = MV_CC_SetFloatValue(pDlg->m_Camhandle[nCam], "ExposureTime", (float)nExp);
					MV_CC_SetFloatValue(pDlg->m_Camhandle[nCam], "Gain", fGain);
					MV_CC_SetFloatValue(pDlg->m_Camhandle[nCam], "Gamma", fGamma);
					pDlg->Snap(pDlg->m_HWRun[nCam]);  //拍一张照片触发尺寸
				}
				///////////////////////////触发相机采集图像图像////////////////////////////////////////////
				CRect rc;
				pDlg->GetDlgItem(nID[nCam])->GetClientRect(&rc);
				pDlg->m_HWRun[nCam].RealSize(rc, "进入自动生产模式！","green");
				pSng->_nRun = RUN_WORK;
				bUnLoadOK = false;                                     //已经载入XML
				pDlg->SetTimer(TIM_SND, 50, NULL);
			}
			continue;
		}
		else if (pSng->_nRun == RUN_UNLOAD && bUnLoadOK == false)
		{
			for (int i = 0; i < nXML; i++)			//卸载图像检测内容
			{
				mUnLoadXML(10 * nCam + i);
			}
			bUnLoadOK = true;
			continue;
		}
		if (pDlg->m_nCHK[nCam] == 0)                           //收到检测指令
			continue;
		pDlg->KillTimer(TIM_SND);                                //不再查询输入
		DWORD dwCurr = GetTickCount();
		int nSigTime = pSng->GetCfgInt("参数", "信号时长", 500);
		int nRotTime = pSng->GetCfgInt("参数", "旋转时长", 5000);
		int nShiftTime = pSng->GetCfgInt("参数", "换料时长", 800);
		pDlg->Snap(pDlg->m_HWRun[nCam],4);
		while (GetTickCount() - dwCurr < nRotTime)
		{
			bool bOK = mIsImageOK(pDlg->m_HWRun[nCam], 10 * nCam,false);
			if (bOK)
			{
				pDlg->SendResult(0, 1);
				mWriteLog("信号上升沿发送");
				Sleep(nSigTime);
				pDlg->ClearResult(0);
				break;
			}			
			pDlg->Snap(pDlg->m_HWRun[nCam],1);
		}
		Sleep(nShiftTime);
		pDlg->SetTimer(TIM_SND,50,NULL);
		pDlg->m_nCHK[nCam] = 0;
		continue;
	}
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
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CQzdDlg 对话框


int CQzdDlg::InitCam(int iNum)
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nRet = MV_OK;
	m_Camhandle[iNum] = NULL;
	CString strLog, strCamName;
	MV_CC_DEVICE_INFO_LIST stDeviceList;
	memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
	if (MV_OK != nRet)
	{
		strLog.Format("Enum Devices fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	strCamName.Format("CAM%d", iNum + 1);
	int II = 0;
	if (stDeviceList.nDeviceNum > 0)
	{
		for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
		{
			int iRes = strcmp(strCamName.GetBuffer(0), (LPCSTR)(stDeviceList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName));
			strCamName.ReleaseBuffer();
			if (iRes == 0)
			{
				II = i;
				break;
			}
		}
		//CStringArray stsSN;
		//pSng->GetSecKeys("序列",stsSN);
		//for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
		//{
		//	bool bFindSN = false;
		//	for (int j = 0; j < stsSN.GetCount(); j++)
		//	{
		//		CString strSN;
		//		for (int k = 0; k < 16;k++)
		//		{
		//			if (stDeviceList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chSerialNumber[k] == '\0')
		//				break;
		//			CString str;
		//			str.Format("%02X",stDeviceList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chSerialNumber[k] + k);
		//			strSN += str;
		//		}
		//		CString strNO = pSng->GetCfgString("序列",stsSN[i],"");
		//		if (strSN == strNO)
		//		{
		//			bFindSN = true;
		//			break;
		//		}
		//	}
		//	if (bFindSN == false)
		//		return false;
		//}
	}
	// ch:选择设备并创建句柄 | Select device and create handle
	nRet = MV_CC_CreateHandle(&m_Camhandle[iNum], stDeviceList.pDeviceInfo[II]);
	if (MV_OK != nRet)
	{
		strLog.Format("Create Handle fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}

	// ch:打开设备 | Open device
	nRet = MV_CC_OpenDevice(m_Camhandle[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Open Device fail! nRet [0x%x],打开设备失败", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
	if (stDeviceList.pDeviceInfo[II]->nTLayerType == MV_GIGE_DEVICE)
	{
		int nPacketSize = MV_CC_GetOptimalPacketSize(m_Camhandle[iNum]);
		if (nPacketSize > 0)
		{
			nRet = MV_CC_SetIntValue(m_Camhandle[iNum], "GevSCPSPacketSize", nPacketSize);
			if (nRet != MV_OK)
			{
				strLog.Format("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
				mWriteLog(strLog);
			}
		}
		else
		{
			strLog.Format("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
			mWriteLog(strLog);
		}
	}
	// ch:设置触发模式为off | eb:Set trigger mode as off
	nRet = MV_CC_SetEnumValue(m_Camhandle[iNum], "TriggerMode", MV_TRIGGER_MODE_OFF);
	if (MV_OK != nRet)
	{
		strLog.Format("Set Trigger Mode fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	nRet = MV_CC_SetEnumValue(m_Camhandle[iNum], "ExposureMode", MV_EXPOSURE_MODE_TIMED);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d曝光模式设置异常", iNum + 1);
		return nRet;
	}
	nRet = MV_CC_SetEnumValue(m_Camhandle[iNum], "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d 关闭自动曝光模式异常！！", iNum + 1);
		return nRet;
	}
	int nExp = pSng->GetCfgInt("曝光", strCamName, 9000);
	nRet = MV_CC_SetFloatValue(m_Camhandle[iNum], "ExposureTime", (float)nExp);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d 设置曝光值异常！！", iNum + 1);
		return nRet;
	}
	int nGain = pSng->GetCfgInt("增益", strCamName, 6);
	nRet = MV_CC_SetEnumValue(m_Camhandle[iNum], "GainAuto", 0);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d 关闭自动增益异常！！", iNum + 1);
		return nRet;
	}
	nRet = MV_CC_SetFloatValue(m_Camhandle[iNum], "Gain", (float)nGain);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d 设置增益值异常！！", iNum + 1);
		return nRet;
	}
	double fGama = atof(pSng->GetCfgString("伽马", strCamName, "1.0"));
	nRet = MV_CC_SetBoolValue(m_Camhandle[iNum], "GammaEnable", true);
	nRet = MV_CC_SetEnumValue(m_Camhandle[iNum], "GammaSelector", 1);
	nRet = MV_CC_SetFloatValue(m_Camhandle[iNum], "Gamma", (float)fGama);
	if (MV_OK != nRet)
	{
		mWriteLog("相机%d 伽马设置异常！！", iNum + 1);
		return nRet;
	}
	// ch:注册抓图回调 | en:Register image callback
	//nRet = MV_CC_RegisterImageCallBackEx(m_Camhandle[iNum], ImageCallBackEx, m_Camhandle[iNum]);
	nRet = MV_CC_RegisterImageCallBackEx(m_Camhandle[iNum], ImageCallBack, this);
	if (MV_OK != nRet)
	{
		strLog.Format("Register Image CallBack fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	// ch:开始取流 | en:Start grab image
	nRet = MV_CC_StartGrabbing(m_Camhandle[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Start Grabbing fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	strLog.Format("CAM---%d 初始化OK!", iNum + 1);
	mWriteLog(strLog);
	return MV_OK;
}

int CQzdDlg::CloseCam(int iNum)
{
	int nRet = MV_OK;
	CString strLog;
	nRet = MV_CC_StopGrabbing(m_Camhandle[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Stop Grabbing fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	// ch:注销抓图回调 | en:Unregister image callback
	nRet = MV_CC_RegisterImageCallBackEx(m_Camhandle[iNum], NULL, NULL);
	if (MV_OK != nRet)
	{
		strLog.Format("Unregister Image CallBack fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}

	// ch:关闭设备 | en:Close device
	nRet = MV_CC_CloseDevice(m_Camhandle[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Close Device fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}

	// ch:销毁句柄 | en:Destroy handle
	nRet = MV_CC_DestroyHandle(m_Camhandle[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Destroy Handle fail! nRet [0x%x]", nRet);
		mWriteLog(strLog);
		return nRet;
	}
	if (nRet != MV_OK)
	{
		if (m_Camhandle[iNum] != NULL)
		{
			MV_CC_DestroyHandle(m_Camhandle[iNum]);
			m_Camhandle[iNum] = NULL;
		}
	}
	return MV_OK;
}

void CQzdDlg::SetupCallBack(unsigned char ** pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, int II)
{
	if (g_nExt == 1)
		return;
	CSingleton* pSng = CSingleton::GetInstance();
	CString strLog;
	if (pFrameInfo)
	{
		strLog.Format("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
			pFrameInfo->nWidth, pFrameInfo->nHeight, pFrameInfo->nFrameNum);
		if (pSng->_iReal[II] > 0)
		{
			pSng->_csCam[II].Lock();
			if (II % 2 == 1)  //辅材相机
			{
				HObject hoImage;
				GenImage1(&hoImage, "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
				MirrorImage(hoImage, &pSng->_hoImg[II], "column");
			}
			else
				GenImage1(&pSng->_hoImg[II], "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
			pSng->_csCam[II].Unlock();
			if (pSng->_iReal[II] == 1)
			{				
				CRect rc;
				GetDlgItem(IDC_BTN_IMG)->GetClientRect(&rc);
				m_HWRun[II].RealSize(rc, "");
				pSng->_iReal[II]++;
			}
			return;
		}
		if (pSng->_iSnap[II] > 0)
		{
			pSng->_csCam[II].Lock();
			if (II % 2 == 1)  //辅材相机
			{
				HObject hoImage;
				GenImage1(&hoImage, "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
				MirrorImage(hoImage, &pSng->_hoImg[II], "column");
			}
			else
				GenImage1(&pSng->_hoImg[II], "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
			pSng->_csCam[II].Unlock();
			pSng->_iSnap[II]--;
			return;
		}
	}
}


CQzdDlg::CQzdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQzdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQzdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);
	DDX_Control(pDX, IDC_SLIDER_PAR, m_sldPar);
	DDX_Control(pDX, IDC_MSCOMM1, m_Com);
}

BEGIN_MESSAGE_MAP(CQzdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CQzdDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_IMG, &CQzdDlg::OnBnClickedBtnImg)
	ON_BN_CLICKED(IDC_BTN_LOG, &CQzdDlg::OnBnClickedBtnLog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_RUN, &CQzdDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_REAL, &CQzdDlg::OnBnClickedBtnReal)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PAR, &CQzdDlg::OnNMReleasedcaptureSliderPar)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CQzdDlg::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BTN_TRG, &CQzdDlg::OnBnClickedBtnTrg)
END_MESSAGE_MAP()


// CQzdDlg 消息处理程序
BOOL CQzdDlg::GetDLLFunc()
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
	mLog = (hpLog)GetProcAddress(m_hDll, "GetLogInfo");
	mImageXMLClear = (pImageXMLClear)GetProcAddress(m_hDll, "IsImageXMLClear");
	mWriteLog = (pWriteLog)GetProcAddress(m_hDll, "WriteLog");
	mInitCam = (pInitCam)GetProcAddress(m_hDll, "InitDLLCam");
	mCloseCam = (pCloseCam)GetProcAddress(m_hDll, "CloseCam");
	if (NULL == mWriteLog)
	{
		MessageBox("DLL中函数中没有写Log记录的函数！");
		return FALSE;
	}
	if (NULL == mLog)
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

BOOL CQzdDlg::OnInitDialog()
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
	if (GetDLLFunc() == FALSE)
	{
		MessageBox("载入图像DLL失败!");
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	int nComNO = pSng->GetCfgInt("参数", "COM", 1);
	int nBaud = pSng->GetCfgInt("参数", "波特率", 9600);
	if (InitialCom(nComNO,nBaud)==FALSE)
	{
		MessageBox("串口打开失败!");
		SendMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}
	g_nExt = 0;
	mWriteLog("软件主对话框初始化成功！%d", 5);
	pSng->CreateDir("D:\\INI");
	pSng->CreateDir("D:\\TMP");
	pSng->CreateDir("D:\\Image");
	pSng->CreateDir("D:\\XML");
	m_comboType.AddString("曝光");
	m_comboType.AddString("增益");
	m_comboType.AddString("伽马");
	m_comboType.EnableWindow(FALSE);
	m_sldPar.EnableWindow(FALSE);
	for (int i = 0; i < 1; i++)
	{
		CString strCam;
		strCam.Format("CAM%d", i + 1);
		pSng->CreateDir("D:\\XML\\" + strCam);
		m_nThread = i;
		m_pThr[i] = AfxBeginThread(ThreadCam, this,THREAD_PRIORITY_TIME_CRITICAL);
		while (i == m_nThread)
		{
			DoEvent();
		}
	}
	m_nThread = 0;
	vector<int> nID;
	nID.push_back(IDC_STATIC1);
	for (int i = 0; i < (int)nID.size(); i++)
	{
		m_HWRun[i].psRD.x = 1280;
		m_HWRun[i].psRD.y = 1024;
		m_HWRun[i].InitWnd(GetDlgItem(nID[i]));
	}	
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_IMG)->EnableWindow(FALSE);
	mInitCam(0);
	return 1;
	SetTimer(TIM_INI, 100, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQzdDlg::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (nIDEvent == TIM_INI)  //初始化
	{
		KillTimer(TIM_INI);
		int nRet = InitCam(m_nThread);
		if (nRet != MV_OK)
		{
			mWriteLog("相机%d初始化失败！", m_nThread + 1);
			return;
		}
		DWORD dwST = GetTickCount();
		while (GetTickCount() - dwST < 500)
		{
			DoEvent();
		}
		m_nThread++;
		if (m_nThread == 1)//CAM_THREAD
		{
			KillTimer(TIM_INI);
			m_comboType.SetCurSel(0);
			m_comboType.EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_IMG)->EnableWindow(TRUE);
		}
		else
			SetTimer(TIM_INI, 10, NULL);
	}
	else if (nIDEvent == TIM_REAL)
	{
		int nCam = 0;
		if (pSng->_iReal[nCam] > 1)
		{
			pSng->_csCam[nCam].Lock();
			CopyImage(pSng->_hoImg[nCam], &m_HWRun[nCam].hoImage);
			m_HWRun[nCam].RealSize();
			m_HWRun[nCam].ShowImage();
			SetColor(m_HWRun[nCam].WndID, "blue");
			DispCross(m_HWRun[nCam].WndID, m_HWRun[nCam].psRD.y / 2, m_HWRun[nCam].psRD.x / 2, 6000, 0);
			pSng->_csCam[nCam].Unlock();
		}
	}
	else if (nIDEvent == TIM_SND)
	{
		SendFindOrder();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CQzdDlg::SendFindOrder()
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

void CQzdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if (nID == SC_CLOSE)
		{
			OnBnClickedOk();
			return;
		}
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQzdDlg::OnPaint()
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
HCURSOR CQzdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQzdDlg::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	g_nExt = 1;
	Sleep(300);
	TRACE("\n 启动软件退出机制！");
	for (int i = m_nThread - 1; i >= 0; i--)
	{
		CloseCam(i);
	}	
	mCloseCam(0);
	FreeLibrary(m_hDll);
	EndDialog(0);
	//CDialogEx::OnOK();
}

void CQzdDlg::Snap(CHWin& HW, int nTimes)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_iSnap[0] = nTimes;
	while (pSng->_iSnap[0] > 0)
	{
		DoEvent();
	}
	CopyImage(pSng->_hoImg[0],&HW.hoImage);
}

void CQzdDlg::OnBnClickedBtnImg()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCam("CAM1");
	IHpDllWin* pImageWin = mImageDlg(strCam, NULL);
	pSng->CreateDir("D:\\XML\\" + strCam);
	//触发相机采集图片，最好采用保存图片，用Gain，Exposure，Gamma 命名文件名
	if (m_nThread > 0)
	{
		Snap(pImageWin->m_HW);
		//CopyImage(pSng->_hoImg[0], &pImageWin->m_HW.hoImage);
	}
	else
	{
		ReadImage(&pImageWin->m_HW.hoImage, "D:\\1.jpg");
	}
	pImageWin->m_HW.bLoad = true;
}


void CQzdDlg::OnBnClickedBtnLog()
{
	CIDlgInfo* pDlg = mLog();
}


void CQzdDlg::OnBnClickedBtnRun()
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
		m_nCHK[0] = 0;
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


void CQzdDlg::OnBnClickedBtnReal()
{
	if (m_nThread == 0)
		return;
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText;
	GetDlgItemText(IDC_BTN_REAL, strText);
	int nCam = 0;
	if (strText == "开启实时")
	{
		SetDlgItemText(IDC_BTN_REAL, "关闭实时");
		SetEnable(FALSE);
		GetDlgItem(IDC_BTN_REAL)->EnableWindow(TRUE);
		m_comboType.EnableWindow(TRUE);
		CString strType;
		m_comboType.GetLBText(m_comboType.GetCurSel(), strType);
		InitSlider();
		int nVal = (int)(atof(pSng->GetCfgString(strType, "CAM1","500")) / m_fSldCoef + 0.5);
		m_sldPar.SetPos(nVal);
		pSng->_iReal[0] = 1;
		SetTimer(TIM_REAL, 100, NULL);
	}
	else
	{
		KillTimer(TIM_REAL);
		pSng->_iReal[nCam] = 0;
		SetDlgItemText(IDC_BTN_REAL, "开启实时");
		SetEnable(TRUE);
	}
}

void CQzdDlg::SetEnable(BOOL bEna /* = TRUE */)
{
	GetDlgItem(IDC_BTN_REAL)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_RUN)->EnableWindow(bEna);
	GetDlgItem(IDC_BTN_IMG)->EnableWindow(bEna);
	m_sldPar.EnableWindow(!bEna);  //这个就是要取反
}

void CQzdDlg::OnNMReleasedcaptureSliderPar(NMHDR *pNMHDR, LRESULT *pResult)
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nCam = 0,nRet;
	CString strType, strVal;
	m_comboType.GetLBText(m_comboType.GetCurSel(), strType);
	float fVal = (float)(m_sldPar.GetPos()*m_fSldCoef);	
	strVal.Format("%.1f", fVal);
	if (m_comboType.GetCurSel() == 0)
	{
		strVal.Format("%d",m_sldPar.GetPos());
		nRet = MV_CC_SetFloatValue(m_Camhandle[nCam], "ExposureTime", fVal);
	}
	else if (m_comboType.GetCurSel() == 1)
	{
		nRet = MV_CC_SetFloatValue(m_Camhandle[nCam], "Gain", fVal);
	}
	else if (m_comboType.GetCurSel() == 2)
	{
		nRet = MV_CC_SetFloatValue(m_Camhandle[nCam], "Gamma", fVal);
	}
	pSng->SetCfgString(strType, "CAM1", strVal);
	*pResult = 0;
}

void CQzdDlg::InitSlider()
{
	if (m_comboType.GetCurSel() == 0)
	{
		m_sldPar.SetRange(100, 20000);
		m_fSldCoef = 1.0;
	}
	else if (m_comboType.GetCurSel() == 1)
	{
		m_sldPar.SetRange(10, 200);
		m_fSldCoef = 0.1;
	}
	else if (m_comboType.GetCurSel() == 2)
	{
		m_sldPar.SetRange(2, 30);
		m_fSldCoef = 0.1;
	}
}

void CQzdDlg::OnCbnSelchangeComboType()
{
	InitSlider();
}


void CQzdDlg::OnBnClickedBtnTrg()
{
	Snap(m_HWRun[0]);
	m_nCHK[0] = 1;
}

BEGIN_EVENTSINK_MAP(CQzdDlg, CDialogEx)
	ON_EVENT(CQzdDlg, IDC_MSCOMM1, 1, CQzdDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CQzdDlg::OnCommMscomm1()
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
	if (len != 6)
		return;
	if (m_nCHK[0] == 1)                                       //有检测指令就不用判断
		return;
	mWriteLog("%d---收到%d 字符！",GetTickCount(),len);
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //转换为BYTE型数组     
		BYTE bt = *(char*)(rxdata + k);                     //字符型 	                     
		TRACE(" %02x", bt);                                   //输出检测指令
	}
	//////////基于这个IO盒子不需要指令了////////////////////
	BYTE btNum = *(char*)(rxdata + 3);                 //字符型 
	if (btNum != 0x00)                                              //确定有检测指令
	{
		m_nCHK[0] = 1;
	}
}

void CQzdDlg::SendResult(int nSTS, int nOK)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	if (nOK == 1)
	{
		Array.Add(0x01);
		Array.Add(0x05);
		Array.Add(0x00);
		Array.Add(0x00 + 2 * nSTS);
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
		//else if (nSTS == 2)
		//{
		//	//01 05 00 04 FF 00 CD FB
		//	Array.Add(0xCD);
		//	Array.Add(0xFB);
		//}
	}
	else
	{
		Array.Add(0x01);
		Array.Add(0x05);
		Array.Add(0x00);
		Array.Add(0x01 + 2 * nSTS);
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
		//else if (nSTS == 2)
		//{
		//	//01 05 00 05 FF 00 9C 3B
		//	Array.Add(0x9C);
		//	Array.Add(0x3B);
		//}
	}
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}

void CQzdDlg::ClearResult(int nSTS)
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
	//else if (nSTS == 2)
	//{
	//	//01 05 00 04 00 00 8C 0B
	//	Array.Add(0x8C);
	//	Array.Add(0x0B);
	//	m_Com.put_InBufferCount(0);
	//	m_Com.put_Output(COleVariant(Array));
	//	Sleep(100);
	//	//01 05 00 05 00 00 DD CB
	//	Array.SetAt(3, 0x01 + 2 * nSTS);
	//	Array.SetAt(6, 0xDD);
	//	Array.SetAt(7, 0xCB);
	//}
	m_Com.put_InBufferCount(0);
	m_Com.put_Output(COleVariant(Array));
}


bool CQzdDlg::InitialCom(int iNO, int nBaud)
{
	m_Com.put_CommPort(iNO);                             //设定串口为 COM1 
	CString strBaud, strSetting;
	strSetting.Format(_T("%d,n,8,1"), nBaud);
	m_Com.put_Settings(strSetting);                    //设定波特率9600，无奇偶校验，8位数据位，1作为停止位         
	m_Com.put_InputMode(1);                            //设定数据接收模式，1为二进制，0为文本         
	m_Com.put_InputLen(0);                              //设定当前接收区数据长度 
	m_Com.put_InBufferSize(1024);                     //设置输入缓冲区大小为1024 byte         
	m_Com.put_OutBufferSize(1024);                   //设置输出缓冲区大小为1024 byte         
	m_Com.put_RThreshold(1);                         //每接收到一个字符时，触发OnComm事件         
	m_Com.put_SThreshold(0);                         //每发送一个字符时，不触发OnComm事件
	if (!m_Com.get_PortOpen())
	{
		try
		{
			m_Com.put_PortOpen(true);//打开串口
		}
		catch (CException* e)
		{
			TCHAR   szError[1024];
			e->GetErrorMessage(szError, 1024);         //e.GetErrorMessage(szError,1024); 
			mWriteLog(szError);
			return false;
		}
		mWriteLog("串口打开成功！");
		return true;
	}
	mWriteLog("串口打开失败！");
	return false;
}