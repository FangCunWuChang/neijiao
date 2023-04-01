//#include "IHpDllWin.h"
// IHpDllWin.cpp : 实现文件
//

#include "stdafx.h"
#include "DllMod.h"
#include "IHpDllWin.h"
#include "afxdialogex.h"
#include "Singleton.h"
#include "DlgCom.h"
#include "MvCameraControl.h"
#include "DlgItem.h"
#include "DlgTmp.h"


extern void LogFile(CString log);
extern void LogFile(const char *fmt, ...);
extern void DoEvent();
bool RunFunc(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& m_tree, HTREEITEM hItem = NULL);

// IHpDllWin 对话框

IMPLEMENT_DYNAMIC(IHpDllWin, CDialogEx)

IHpDllWin::IHpDllWin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IHpDllWin::IDD, pParent)
{
	m_strXML.Empty();
	m_fExp = 5000;
	m_fGain = 1.0;
	m_fGam = 1.0;
	m_bINI = false;
}

IHpDllWin::~IHpDllWin()
{
}

void IHpDllWin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_COMBO_FUNC, m_comboFunc);
}


BEGIN_MESSAGE_MAP(IHpDllWin, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ORG, &IHpDllWin::OnBnClickedBtnOrg)
	ON_BN_CLICKED(IDOK, &IHpDllWin::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_ADD_FUNC, &IHpDllWin::OnBnClickedBtnAddFunc)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, &IHpDllWin::OnTvnBeginlabeleditTree1)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, &IHpDllWin::OnTvnEndlabeleditTree1)
	ON_BN_CLICKED(IDC_RADIO1, &IHpDllWin::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_BTN_TEST_FUNC, &IHpDllWin::OnBnClickedBtnTestFunc)
	ON_BN_CLICKED(IDC_RADIO4, &IHpDllWin::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &IHpDllWin::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_BTN_SAV_XML, &IHpDllWin::OnBnClickedBtnSavXml)
	ON_BN_CLICKED(IDC_BTN_LOAD_XML, &IHpDllWin::OnBnClickedBtnLoadXml)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &IHpDllWin::OnNMRClickTree1)
	ON_COMMAND(ID_MENU_DEL, &IHpDllWin::OnMenuDel)
	ON_BN_CLICKED(IDC_RADIO2, &IHpDllWin::OnBnClickedRadio2)
	ON_COMMAND(ID_MENU_RUN, &IHpDllWin::OnMenuRun)
	ON_COMMAND(ID_MENU_MOD, &IHpDllWin::OnMenuMod)
	ON_COMMAND(ID_MENU_BF, &IHpDllWin::OnMenuBf)
	ON_BN_CLICKED(IDC_RADIO3, &IHpDllWin::OnBnClickedRadio3)
	//ON_COMMAND(ID_MENU_ADD_RC, &IHpDllWin::OnMenuAddRc)
	ON_COMMAND(ID_MENU_INSERT, &IHpDllWin::OnMenuInsert)
	ON_BN_CLICKED(IDC_BTN_DIR_XML, &IHpDllWin::OnBnClickedBtnDirXml)
	ON_COMMAND(ID_MENU_CAM, &IHpDllWin::OnMenuCam)
	ON_COMMAND(ID_MENU_INI, &IHpDllWin::OnMenuIni)
	ON_COMMAND(ID_MENU_DEF, &IHpDllWin::OnMenuDef)
	ON_COMMAND(IDC_MENU_FIRST, &IHpDllWin::OnMenuFirst)
	ON_BN_CLICKED(IDC_BTN_SNAP, &IHpDllWin::OnBnClickedBtnSnap)
//	ON_BN_CLICKED(IDC_BTN_PART, &IHpDllWin::OnBnClickedBtnPart)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE1, &IHpDllWin::OnNMCustomdrawTree1)
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
//	ON_WM_MBUTTONDOWN()
//	ON_WM_MBUTTONUP()
//	ON_WM_MBUTTONDBLCLK()
ON_BN_CLICKED(IDC_CHECK_BD, &IHpDllWin::OnBnClickedCheckBd)
END_MESSAGE_MAP()

// IHpDllWin 消息处理程序
extern "C" __declspec(dllexport) IHpDllWin* SetImage(CString strWinTxt,CString strXML = NULL)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IHpDllWin *ph = new IHpDllWin;
	ph->m_strCam.Format("%s",strWinTxt);
	ph->m_strXML.Format("%s",strXML);
	ph->Create(IHpDllWin::IDD);
	ph->ShowWindow(SW_SHOW);
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pMain = ph;
	return ph;
}

extern "C" __declspec(dllexport) void Snap(CHWin& HW, int nCam,int nTimes)
{
	CSingleton* pSng = CSingleton::GetInstance();
	HW.bLoad = pSng->CamSnap(HW, nCam, nTimes);
	if (HW.bLoad == false)
	{
		LogFile("相机%d 采集图像失败！",nCam + 1);
	}
}

extern "C" __declspec(dllexport) bool SetHeartTime(int nCam,UINT uHeartBeat)
{
	CSingleton* pSng = CSingleton::GetInstance();
	UINT uTime = uHeartBeat > 500 ? uHeartBeat : 500;
	int temp = MV_CC_SetIntValue(pSng->_hCam[nCam], "GevHeartbeatTimeout", uTime);
	return  (MV_OK == temp);
}

extern "C" __declspec(dllexport) bool SvrLink(int iSvrPort)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pSvr = new CTCPSocket(TCP_SOCKET_SERVER);
	if (pSng->_pSvr->CreateServer(iSvrPort) == FALSE)
	{
		MessageBox(NULL,"服务器端口被占用！", "", MB_ICONERROR);
		return FALSE;
	}
	if (pSng->_pSvr->StartServer(NULL, NULL, NULL) == FALSE)
	{
		MessageBox(NULL,"服务器启动失败！", "", MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) int SvrSnd(int nNo, char* data, int nLen)
{
	CSingleton* pSng = CSingleton::GetInstance();
	int nSnd = pSng->_pSvr->SendServer(nNo, data, nLen);
	return nSnd;
}


extern "C" __declspec(dllexport) bool StopSvr()
{
	CSingleton* pSng = CSingleton::GetInstance();
	BOOL bRet = pSng->_pSvr->StopServer();
	if (bRet == FALSE)
		return FALSE;
	pSng->_pSvr->m_bCreated = FALSE;
	delete pSng->_pSvr;
	return TRUE;
}

extern "C" __declspec(dllexport) void SvrRev(int nNO,CString& strRev)
{
	CSingleton* pSng = CSingleton::GetInstance();
	strRev.Format("%s", pSng->_pSvr->m_strSvrRev[nNO]);
}

extern "C" __declspec(dllexport) void SvrClearRev(int nNO)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pSvr->m_strSvrRev[nNO].Empty();
}

extern "C" __declspec(dllexport) int SvrLinkStatus(int nNO)
{
	CSingleton* pSng = CSingleton::GetInstance();
	return pSng->_pSvr->m_nLnk[nNO];
}

/******************没有辅材相机*******************************/
/*                0----产品标准位置，1-- 产品新位置；
                   0：x，1：Y，2：Phi
/************************************************************************/
extern "C" __declspec(dllexport) int GetOffset(int nInx,double fPosImg[2][3],double fDelta[3])
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_psCali[nInx].bLoad == false)
	{
		LogFile("Cam %d 没有映射矩阵",nInx + 1);
		return false;
	}	
	double fRotAng = fPosImg[1][2] - fPosImg[0][2];
	HTuple HomMat2D, HomMat2DRot, hvTransY, hvTransX, hvMX, hvXST, hvYST;
	HomMat2dIdentity(&HomMat2D);
	HomMat2dRotate(HomMat2D, fRotAng,pSng->_RotAxs[nInx].y, pSng->_RotAxs[nInx].x, &HomMat2DRot);
	CPos ImgPts[9], AxisPts[9];
	for (int j = 0; j < 9 ; j++)
	{
		ImgPts[j].x = pSng->_psCali[nInx].fImg[j].x;
		ImgPts[j].y = pSng->_psCali[nInx].fImg[j].y;
		//AffineTransPoint2d(HomMat2DRot, pSng->_psCali[nInx].fImg[j].y, pSng->_psCali[nInx].fImg[j].x, &hvYST, &hvXST);
		//ImgPts[j].x = hvXST.D();
		//ImgPts[j].y = hvYST.D();
		AxisPts[j].x = pSng->_psCali[nInx].fAxs[j].x;
		AxisPts[j].y = pSng->_psCali[nInx].fAxs[j].y;
	}
	pSng->NineDotCalib(ImgPts, AxisPts, hvMX);                                                                         //旋转后生成映射矩阵
	double fPosST[2],fPosNew[2];
	//AffineTransPoint2d(HomMat2DRot, fPosImg[0][1], fPosImg[0][0], &hvYST, &hvXST);      //产品坐标的标准位置
	AffineTransPoint2d(hvMX, fPosImg[0][1], fPosImg[0][0], &hvTransY, &hvTransX);
	fPosST[0] = hvTransX.D();
	fPosST[1] = hvTransY.D();
	//AffineTransPoint2d(HomMat2DRot, fPosImg[1][1], fPosImg[1][0], &hvYST, &hvXST);      //产品坐标的新位置
	AffineTransPoint2d(hvMX, fPosImg[1][1], fPosImg[1][0], &hvTransY, &hvTransX);
	fPosNew[0] = hvTransX.D();
	fPosNew[1] = hvTransY.D();
	fDelta[0] = fPosNew[0] - fPosST[0];
	fDelta[1] = fPosNew[1] - fPosST[1];
	fDelta[2] = fRotAng;
	return true;
}

extern "C" __declspec(dllexport) bool SetCamPar(int nCam,float fExp,float fGain,float fGam)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_bINICam == false)
		return false;
	CString strLog;
	pSng->_csCam[nCam].Lock();
	int nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "ExposureTime", fExp);
	         //MV_CC_SetFloatValue(pSng->_hCam[nCam], "ExposureTime", fVal);
	pSng->_csCam[nCam].Unlock();
	if (MV_OK != nRet)
	{
		strLog.Format("相机%d 曝光参数设置失败！", nCam + 1,nRet);
		LogFile(strLog);
		return false;
	}
	pSng->_csCam[nCam].Lock();
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gain", fGain);
	pSng->_csCam[nCam].Unlock();
	if (MV_OK != nRet)
	{
		strLog.Format("相机%d 增益参数设置失败！", nCam + 1, nRet);
		LogFile(strLog);
		return false;
	}
	pSng->_csCam[nCam].Lock();
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gamma", fGam);
	pSng->_csCam[nCam].Unlock();
	if (MV_OK != nRet)
	{
		strLog.Format("相机%d 伽马参数设置失败！", nCam + 1, nRet);
		LogFile(strLog);
		return false;
	}
	return true;
}

void __stdcall ImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	CSingleton* pSng = CSingleton::GetInstance();
	int* pICam = (int*)pUser;
	pSng->_csCam[*pICam].Lock();
	pSng->SetupCallBack(&pData, pFrameInfo, *pICam);
	pSng->_csCam[*pICam].Unlock();
}

extern "C" __declspec(dllexport) bool InitDLLCam(int iNum)
{
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_bINICam = false;
	int nRet = MV_OK;
	pSng->_hCam[iNum] = NULL;
	CString strLog, strCamName;
	MV_CC_DEVICE_INFO_LIST stDeviceList;
	memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
	if (MV_OK != nRet)
	{
		strLog.Format("Enum Devices fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
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
	nRet = MV_CC_CreateHandle(&pSng->_hCam[iNum], stDeviceList.pDeviceInfo[II]);
	if (MV_OK != nRet)
	{
		strLog.Format("Create Handle fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:打开设备 | Open device
	nRet = MV_CC_OpenDevice(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Open Device fail! nRet [0x%x],打开设备失败", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
	if (stDeviceList.pDeviceInfo[II]->nTLayerType == MV_GIGE_DEVICE)
	{
		int nPacketSize = MV_CC_GetOptimalPacketSize(pSng->_hCam[iNum]);
		if (nPacketSize > 0)
		{
			nRet = MV_CC_SetIntValue(pSng->_hCam[iNum], "GevSCPSPacketSize", nPacketSize);
			if (nRet != MV_OK)
			{
				strLog.Format("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
				LogFile(strLog);
			}
		}
		else
		{
			strLog.Format("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
			LogFile(strLog);
		}
	}
	// ch:设置触发模式为off | eb:Set trigger mode as off
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "TriggerMode", MV_TRIGGER_MODE_OFF);
	if (MV_OK != nRet)
	{
		strLog.Format("Set Trigger Mode fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "ExposureMode", MV_EXPOSURE_MODE_TIMED);
	if (MV_OK != nRet)
	{
		LogFile("相机%d曝光模式设置异常", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
	if (MV_OK != nRet)
	{
		LogFile("相机%d 关闭自动曝光模式异常！！", iNum + 1);
		return false;
	}
	int nExp = pSng->GetCfgInt("曝光", strCamName, 9000);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "ExposureTime", (float)nExp);
	if (MV_OK != nRet)
	{
		LogFile("相机%d 设置曝光值异常！！", iNum + 1);
		return false;
	}
	int nGain = pSng->GetCfgInt("增益", strCamName, 6);
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "GainAuto", 0);
	if (MV_OK != nRet)
	{
		LogFile("相机%d 关闭自动增益异常！！", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "Gain", (float)nGain);
	if (MV_OK != nRet)
	{
		LogFile("相机%d 设置增益值异常！！", iNum + 1);
		return false;
	}
	double fGama = atof(pSng->GetCfgString("伽马", strCamName, "1.0"));
	nRet = MV_CC_SetBoolValue(pSng->_hCam[iNum], "GammaEnable", true);
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "GammaSelector", 1);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "Gamma", (float)fGama);
	if (MV_OK != nRet)
	{
		LogFile("相机%d 伽马设置异常！！", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "AcquisitionFrameRate", 5);
	if (nRet != MV_OK)
	{
		LogFile("相机%d 采集帧率为5帧失败！！", iNum + 1);
		return false;
	}
	//帧率控制使能，true表示打开，false标识关闭
	nRet = MV_CC_SetBoolValue(pSng->_hCam[iNum], "AcquisitionFrameRateEnable", true);
	if (nRet != MV_OK)
	{
		LogFile("相机%d 采集帧率设置开启失败！！", iNum + 1);
		return false;
	}
	// ch:注册抓图回调 | en:Register image callback
	//nRet = MV_CC_RegisterImageCallBackEx(m_Camhandle[iNum], ImageCallBackEx, m_Camhandle[iNum]);
	pSng->_iCam[iNum] = iNum;
	nRet = MV_CC_RegisterImageCallBackEx(pSng->_hCam[iNum], ImageCallBack,&pSng->_iCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Register Image CallBack fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:开始取流 | en:Start grab image
	nRet = MV_CC_StartGrabbing(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Start Grabbing fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	strLog.Format("CAM---%d 初始化OK!", iNum + 1);
	LogFile(strLog);
	pSng->_bINICam = true;
	pSng->_nCamThread++;
	return true;
}

extern "C" __declspec(dllexport) bool CloseCam(int iNum)
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_hCam[iNum] == NULL)
		return true;
	int nRet = MV_OK;
	CString strLog;
	nRet = MV_CC_StopGrabbing(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Stop Grabbing fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:注销抓图回调 | en:Unregister image callback
	nRet = MV_CC_RegisterImageCallBackEx(pSng->_hCam[iNum], NULL, NULL);
	if (MV_OK != nRet)
	{
		strLog.Format("Unregister Image CallBack fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:关闭设备 | en:Close device
	nRet = MV_CC_CloseDevice(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Close Device fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:销毁句柄 | en:Destroy handle
	nRet = MV_CC_DestroyHandle(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Destroy Handle fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	if (nRet != MV_OK)
	{
		if (pSng->_hCam[iNum] != NULL)
		{
			MV_CC_DestroyHandle(pSng->_hCam[iNum]);
			pSng->_hCam[iNum] = NULL;
		}
	}
	LogFile("CAM%d 关闭OK!", iNum + 1);
	return true;
}

extern "C" __declspec(dllexport) bool UnLoadImageXML(int nID)
{
	CSingleton* pSng = CSingleton::GetInstance();
	for (int i = 0; i < (int)pSng->_XMLImg[nID].size();i++)
	{
		pSng->_XMLImg[nID][i].TMs.ClearMod();
	}
	pSng->_XMLImg[nID].clear();
	return true;
}

extern "C" __declspec(dllexport) bool IsImageXMLClear()
{
	CSingleton* pSng = CSingleton::GetInstance();
	for (int i = 0; i < 80; i++)
	{
		if (pSng->_XMLImg[i].size() > 0)
			return false;
	}
	return true;
}

//每个相机最多6中工作内容
extern "C" __declspec(dllexport) bool LoadImageXML(int nID,CString strXML,int& nExp,float& fGain,float& fGam)
{
	CSingleton* pSng = CSingleton::GetInstance();
	TiXmlDocument doc(strXML);
	bool loadOkay = doc.LoadFile();
	if (loadOkay == false)
	{
		MessageBox(NULL,strXML + " 文件格式错误!", "读取失败", MB_ICONWARNING);
		return false;
	}
	pSng->_XMLImg[nID].clear();
	TiXmlElement* root = doc.RootElement();
	CString strText = root->Value();                                                    //根节点只有属性
	CString strExp = root->Attribute("EXP");
	CString strGain = root->Attribute("Gain");
	CString strGam = root->Attribute("Gamma");
	CString strTest = root->Attribute("Test");
	if (strTest != "1")
	{
		MessageBox(NULL, strXML + "文件没有测试成功无法载入！", "读取失败", MB_ICONWARNING);
		return false;
	}
	nExp = int(atof(strExp));
	fGain = (float)atof(strGain);
	fGam = (float)atof(strGam);
	TiXmlElement *pChild = root->FirstChildElement();
	while (pChild != NULL)
	{
		TiXmlElement *pleaf = pChild->FirstChildElement();			
		CXMLImage NodeImg;
		NodeImg.strType = pChild->Value();                               //节点名称Sec
		double fMinScore = 0.6;
		while (pleaf != NULL)
		{
			CString strName = pleaf->Value();
			CString strValue = pleaf->GetText();
			if (strName == "最低分")
			{
				fMinScore = atof(strValue);
			}
			CPos sLf;
			sLf.strNick.Format("%s",strName);                                                      //节点名称 Key
			sLf.strMN.Format("%s",strValue);                                                        //这个节点的值
			NodeImg.psLeaf.push_back(sLf);                                                      //所有的叶节点
			if (strName == "模板" || strName == "模板名")                                    //非Unicode 两个字符一个汉字
			{
				CStringArray ssTM,ssXY;
				pSng->SplitString(strValue, ";", ssTM);
				TiXmlElement *pOffset = pleaf->NextSiblingElement();
				CString strOffset = pOffset->GetText();                                        //模板中心偏差值
				pSng->SplitString(strOffset, ";", ssXY);
				//TiXmlElement* pType = pOffset->NextSiblingElement();            
				//CString strTypeTmp = pType->GetText();
				for (int i = 0; i < min(MAX_TM,ssTM.GetCount());i++)
				{
					int nTYP = atoi(ssTM[i].Right(1));
					if (nTYP == TMP_NCC)
					{
						ReadNccModel(ssTM[i].GetBuffer(), &NodeImg.TMs.hvM[i]);
					}
					else if (nTYP == TMP_SHM)
					{
						ReadShapeModel(ssTM[i].GetBuffer(), &NodeImg.TMs.hvM[i]);
					}
					ssTM[i].ReleaseBuffer();
					CStrs ssVal;
					pSng->SplitString(ssXY[i], ",", ssVal);                                    //是否有这个补偿
					for (int j = 0; j < (int)ssVal.size() && j < 5; j++)
					{
						NodeImg.TMs.fPar[i][j] = atof(ssVal[j]);
					}
					/*if (ssXY.GetCount() > i)
					{
					}	*/				
					NodeImg.TMs.nM[i] = nTYP;
				}	
				NodeImg.TMs.fScore = max(0.2,fMinScore);
			}
			if (strName == "输出" && strValue == "是")    //只要这个节点有输出功能就输出
			{
				NodeImg.nOTP = OUT_VAL;                  //输出的值是以CString输出的
			}
			else if (strValue == "输出字符" && strValue == "是")
			{
				NodeImg.nOTP = OUT_STR;
			}
			pleaf = pleaf->NextSiblingElement();       
		}
		pChild = pChild->NextSiblingElement();
		pSng->_XMLImg[nID].push_back(NodeImg);
	}
	return true;
}

extern "C" __declspec(dllexport) void WriteLog(const char *fmt, ...)
{
	va_list args;
	char rt_log_buf[0xFFFF];

	va_start(args, fmt);
	vsprintf_s(rt_log_buf, fmt, args);
	va_end(args);
	CString strLog;
	strLog.Format("%s",rt_log_buf);

	LogFile(strLog);
}


void IHpDllWin::OnBnClickedBtnOrg()
{
	m_HW.WinTexts.clear();
	m_HW.RealSize("yellow");
}


HHOOK hHook = NULL;
HWND hwndDllDlg = NULL;
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSG lpMsg = (LPMSG)lParam;

	if (nCode >= 0 && PM_REMOVE == wParam)
	{
		if ((lpMsg->message == WM_MBUTTONDOWN))
		{
			CSingleton* pSng = CSingleton::GetInstance();
			IHpDllWin* pWin = (IHpDllWin*)pSng->_pMain;
			pWin->m_HW.RealSize("yellow");
		}
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

BOOL IHpDllWin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSingleton* pSng = CSingleton::GetInstance();
	int nID = IDC_STATIC_IMG;

	hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
	NULL, GetCurrentThreadId());

	hwndDllDlg = GetSafeHwnd();

	m_HW.InitWnd(GetDlgItem(nID));
	SetDlgItemText(IDC_EDIT_WL,"0");
	SetDlgItemText(IDC_EDIT_WU, "0");

	CString strR, strD;
	strR.Format("%d",(int)(m_HW.psRD.x));
	strD.Format("%d",(int)(m_HW.psRD.y));
	SetDlgItemText(IDC_EDIT_WR, strR);
	SetDlgItemText(IDC_EDIT_WD, strD);

	//OnBnClickedBtnOrg();

	SetColor(m_HW.WndID, "green");
	SetLineWidth(m_HW.WndID, 1);

	CString strRoot;
	strRoot.Format("%s=Exp:%.1f;Gain:%.1f;Gamma:%.1f;Test:0", m_strCam, m_fExp, m_fGain, m_fGam);
	m_hRoot = m_tree.InsertItem(strRoot, 1, 0, TVI_ROOT);              //添加一级结点
	m_tree.SetTextColor(RGB(0, 0, 255));                                               //设置文本颜色

	OnBnClickedRadio1();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO1);
	pBtn->SetCheck(BST_CHECKED);
	if (m_strXML.GetLength() > 0)
	{
		LoadXML(m_strXML);
		GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
		SetDlgItemText(IDC_EDIT_NAME, m_strXML);
	}
	CStrs sFiles;
	CString strXMLDir;
	if (m_bCali)
	{
		strXMLDir.Format("D:\\CALI\\%s", m_strCam);
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_BD);
		pBtn->SetCheck(BST_CHECKED);
	}
	else
		strXMLDir.Format("D:\\XML\\%s", m_strCam);
	pSng->FindAllFile(strXMLDir, sFiles);
	if (sFiles.size() == 1)
	{
		LoadXML(sFiles[0]);
		CString strExt = ".xml";
		CString strNick = sFiles[0].TrimRight("." + strExt);
		CString strEdit = strNick.Mid(strXMLDir.GetLength() + 1, 100);
		SetDlgItemText(IDC_EDIT_NAME, strEdit);
	}
	pSng->_pMain = this;
	LogFile("图像处理对话框初始化成功！");
	SetTimer(TIM_INI, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void IHpDllWin::OnBnClickedOk()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDialogEx::OnOK();
	UnhookWindowsHookEx(hHook);
	delete pSng->_pMain;
	pSng->_pMain = NULL;
	TRACE("\n 动态库图像窗口退出完毕！");
}


void IHpDllWin::OnBnClickedBtnAddFunc()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	m_comboFunc.GetLBText(m_comboFunc.GetCurSel(), str);
	HTREEITEM hChild = m_tree.InsertItem(str, m_hRoot);                                 //添加三级结点
	m_tree.Expand(m_hRoot, TVE_EXPAND);
	CStringArray sKeys;
	pSng->GetSecKeys(str, sKeys);
	for (int i = 0; i < sKeys.GetCount();i++)
	{
		CString strVal = pSng->GetCfgString(str,sKeys[i],NULL);
		CStringArray strs;
		pSng->SplitString(strVal, ";", strs);
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

void IHpDllWin::OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

void IHpDllWin::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CString strText;
	m_tree.GetEditControl()->GetWindowText(strText);
	m_tree.SetItemText(m_tree.GetSelectedItem(), strText);//设置编辑后的文本为结点
	*pResult = 0;
}


void IHpDllWin::OnBnClickedRadio1()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("二值化");
	m_comboFunc.AddString("灰度方差");
	m_comboFunc.AddString("圆形开闭运算");
	m_comboFunc.AddString("方形开闭运算");
	m_comboFunc.AddString("面积筛选");
	m_comboFunc.AddString("面积排序");
	m_comboFunc.AddString("轮廓提取");
	m_comboFunc.AddString("轮廓距离");
	m_comboFunc.AddString("区域处理");
	m_comboFunc.AddString("轮廓分割");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio2()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("方形模板");
	m_comboFunc.AddString("圆形模板");
	m_comboFunc.AddString("角度模板");
	m_comboFunc.AddString("二维码");
	m_comboFunc.AddString("模板定位");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio3()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("拟合圆");
	m_comboFunc.AddString("拟合直线");
	m_comboFunc.AddString("圆边缘测量");
	m_comboFunc.AddString("直线边缘测量");
	m_comboFunc.AddString("点处理");
	m_comboFunc.AddString("数据组合");
	m_comboFunc.AddString("直线角度");
	m_comboFunc.AddString("直线交点");
	m_comboFunc.AddString("点到轮廓距离");
	m_comboFunc.AddString("直线到轮廓距离");
	m_comboFunc.AddString("区域最小距离");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio4()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("方形区域");
	m_comboFunc.AddString("角度形区域");
	m_comboFunc.AddString("圆形区域");
	m_comboFunc.AddString("扇形区域");
	m_comboFunc.AddString("圆环区域");
	m_comboFunc.AddString("扇形环区域");
	m_comboFunc.AddString("结果类区域");
	m_comboFunc.SetCurSel(0);
}


void IHpDllWin::OnBnClickedRadio5()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("输出结果");
	m_comboFunc.AddString("写INI");
	m_comboFunc.SetCurSel(0);
}

int IHpDllWin::GetKnots(HTREEITEM &hRootItem)
{
	HTREEITEM hChildItem = m_tree.GetChildItem(hRootItem);
	HTREEITEM hNextItem;
	int n = 0;
	while (hChildItem)
	{
		hNextItem = hChildItem;
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
		n++;
	}
	return n;
}

bool IHpDllWin::GotoNodeSel(int iNode)
{
	HObject hoStep[96];                       //最多96个节点
	HTuple hvRes[96];
	HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);   //获得根节点的子节点
	int n = 0;
	CopyImage(m_HW.hoImage, &hoStep[0]);
	m_HW.WinTexts.clear();
	//OnBnClickedBtnOrg();
	while (hChildItem)
	{
		CString strFunc = m_tree.GetItemText(hChildItem);
		if (RunFunc(m_HW, -1, n, hoStep, hvRes, strFunc, m_tree, hChildItem) == false)
		{
			CString strLog;
			strLog.Format("节点%d %s   NG！", n + 1, strFunc);
			MessageBox(strLog);			
			m_HW.WinTexts.clear();
			int nPosTxt[2] = { 1, 1 };
			m_HW.ShowImage(nPosTxt, strLog, 60, "red");
			return false;
		}
		n++;
		if (iNode == n)
			break;
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
		if (hChildItem == NULL && strFunc == "输出结果")
		{
			int nPosTxt[2] = {80, 1};
			m_HW.ShowImage(nPosTxt,"OK",120, "green", false);
			//测试完成后修改测试标志
			CString strRoot = m_tree.GetItemText(m_hRoot);
			strRoot.TrimRight("1");
			strRoot.TrimRight("0");
			m_tree.SetItemText(m_hRoot, strRoot + "1");
			break;
		}
	}
	return true;
}

void IHpDllWin::OnBnClickedBtnTestFunc()
{
	DWORD dwSt = GetTickCount();	
	GotoNodeSel(10000);	
	GetDlgItem(IDC_BTN_TEST_FUNC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SNAP)->EnableWindow(FALSE);
	while (GetTickCount() - dwSt < 800)
	{
		DoEvent();
	}
	GetDlgItem(IDC_BTN_TEST_FUNC)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SNAP)->EnableWindow(TRUE);
}

bool IsFuncCircleOpenClose(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	double fRadius = atof(sLeafs[1]);
	int nImgInx = atoi(sLeafs[nSumLeaf - 2]);
	if (sLeafs[0] == "开运算")
	{	
		OpeningCircle(hoImage[nStep + nImgInx], &hoImage[nStep], fRadius);
	}
	else
	{
		ClosingCircle(hoImage[nStep + nImgInx], &hoImage[nStep], fRadius);
	}
	HTuple hvNs,hvS, hvR, hvC;
	CountObj(hoImage[nStep], &hvNs);
	if (hvNs.I() == 0)
	{
		LogFile("圆形 %s 后无轮廓！",sLeafs[0]);
		return false;
	}	
	//AreaCenter(hoImage[nStep], &hvS, &hvR, &hvC);	
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvNs.I();
	return true;
}

bool IsFuncRectOpenClose(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 5;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	CString strTYP = sLeafs[0];
	int nImgInx = atoi(sLeafs[nSumLeaf - 2]);
	double fXY[2];
	fXY[0] = atof(sLeafs[1]);
	fXY[1] = atof(sLeafs[2]);
	if (strTYP == "开运算")
		OpeningRectangle1(hoImage[nStep + nImgInx], &hoImage[nStep], fXY[0], fXY[1]);
	else
		ClosingRectangle1(hoImage[nStep + nImgInx], &hoImage[nStep], fXY[0], fXY[1]);
	HTuple hvNs;
	CountObj(hoImage[nStep], &hvNs);
	if (hvNs.I() == 0)
	{
		LogFile("方形 %s 后无轮廓！",strTYP);
		return false;
	}
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvNs.I();
	return true;
}

bool IsFuncBinVal(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);                 
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	double fMin = atof(sLeafs[0]);
	double fMax = atof(sLeafs[1]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	HObject hoBin;
	Threshold(hoImage[nStep + iImgInx], &hoImage[nStep], fMin, fMax);
	//if (strFill == "是")
	//	FillUp(hoBin, &hoImage[nStep]);                           //这个二值化已经填充了内孔
	//else
	//CopyImage(hoBin, &hoImage[nStep]);
	DispObj(hoImage[nStep], HW.WndID);
	HTuple hvNs,hvS,hvR,hvC;
	CountObj(hoImage[nStep], &hvNs);
	hvRes[nStep] = HTuple();
	if (hvNs.I() == 0)
	{
		LogFile("二值化后无轮廓!");
		hvRes[nStep][0] = 0;
		hvRes[nStep][1] = 0;
		hvRes[nStep][2] = 0;
		return true;
	}
	AreaCenter(hoImage[nStep], &hvS, &hvR, &hvC);
	hvRes[nStep][0] = hvC.D();
	hvRes[nStep][1] = hvR.D();
	hvRes[nStep][2] = hvS.D();
	return true;
}

bool IsFuncOut(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 10;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	CString strRes;
	CStringArray stsPos, stsTol, stsTxt;
	int nNO = nStep + atoi(sLeafs[0]);                              //节点的索引
	int nInx = atoi(sLeafs[1]);                                             //数组中的元素索引
	int nSum = hvRes[nNO].TupleLength().I();                 //节点数量
	if (nSum <= nInx)
	{
		LogFile("指定节点数据错误！");
		return false;
	}
	pSng->SplitString(sLeafs[2], ",", stsPos);
	int nPosTxt[2] = { 0 };
	if (stsPos.GetCount() > 1)
	{
		nPosTxt[0] = atoi(stsPos[0]);
		nPosTxt[1] = atoi(stsPos[1]);
	}
	pSng->SplitString(sLeafs[3], ";", stsTxt);
	double fCoef = atof(sLeafs[4]);
	pSng->SplitString(sLeafs[5], ";", stsTol);      //公差
	double fOffset = atof(sLeafs[6]);
	double fSTVal = atof(sLeafs[7]);
	double fSTCof = atof(sLeafs[8]);
	bool bOK = true;
	double fVal = hvRes[nNO][nInx].D()*fCoef;
	double fOutVal = fSTCof*fSTVal + (1.0 - fSTCof)*fVal + fOffset;
	fVal = fOutVal;
	strRes.Format("%s=%.2f ", stsTxt[0], fVal);
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = fVal;
	if (stsTol[0] != "_,_")
	{
		CStrs sTols;
		pSng->SplitString(stsTol[0], ",", sTols);
		double fMin = atof(sTols[0]);
		double fMax = atof(sTols[1]);
		if (fVal < fMin || fVal > fMax)
		{
			HW.ShowImage(nPosTxt,strRes,25,"red", false);
			nPosTxt[0] = 70;
			nPosTxt[1] = 10;
			HW.ShowImage(nPosTxt,"NG",60, "red", false);
			return false;
		}
	}
	HW.ShowImage(nPosTxt,strRes,25, "green", false);
	return true;
}

bool IsFuncRegionsCen(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 1;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	int iKnot = atoi(sLeafs[0]);
	CString strData = pSng->GetHTupleString(hvRes[nStep + iKnot]);
	CStringArray ss;
	pSng->SplitString(strData, ";", ss);		                                                                           //每一个检测区的结果数组
	if (strData.GetLength() < 2)
	{
		return false;
	}
	int iAMT = 0;
	hvRes[nStep] = HTuple();
	double fMX = 0, fMY = 0;
	for (int j = 0; j < ss.GetCount(); j++)
	{
		CStringArray XYs;
		pSng->SplitString(ss[j], ",", XYs);
		fMX += atof(XYs[0]);
		fMY += atof(XYs[1]);
	}
	hvRes[nStep][0] = fMX / ss.GetCount();
	hvRes[nStep][1] = fMY / ss.GetCount();
	return true;
}

bool IsFuncRegionSelected(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	int nImgInx = atoi(sLeafs[nSumLeaf - 2]);
	double fS[2];
	fS[0] = atof(sLeafs[0]);
	fS[1] = atof(sLeafs[1]);
	HObject hoCons,hoSel,hoReg,hoOpen;
	HalconCpp::Connection(hoImage[nStep + nImgInx], &hoCons);           //获得所有的轮廓
	SelectShape(hoCons, &hoReg, "area", "and", fS[0], fS[1]);                    //筛选得到最大最小轮廓
	HTuple hvNs,hvS,hvRow,hvCol,hvRadius;
	CountObj(hoReg, &hvNs);
	int nNum = hvNs.I();	
	hvRes[nStep] = HTuple();
	if (nNum == 0)
	{
		SelectShape(hoCons, &hoImage[nStep], "area", "and", fS[0], fS[1]);   //筛选得到最大最小轮廓
		hvRes[nStep][0] = 0;
		hvRes[nStep][1] = 0;
		hvRes[nStep][2] = 0;
		return true;
	}
	int n = 0;
	for (int i = 0; i < nNum;i++)
	{
		SelectObj(hoReg, &hoImage[nStep], i + 1);
		AreaCenter(hoImage[nStep], &hvS, &hvRow, &hvCol);
		//if (hvS.D() > 30000)
		//{
		//	OpeningCircle(hoImage[nStep], &hoOpen, 9);
		//	SmallestCircle(hoOpen, &hvRow, &hvCol, &hvRadius);
		//	GenCircle(&hoImage[nStep], hvRow, hvCol, hvRadius);
		//}
		hvRes[nStep][n++] = hvCol;
		hvRes[nStep][n++] = hvRow;
		hvRes[nStep][n++] = hvS;
		if (strOrder.Right(1) == "*")
			LogFile("\n轮廓%d的面积是:%.2f,中心点(%.1f,%.1f)", i + 1, hvS.D(), hvCol.D(), hvRow.D());
		if (i != nNum - 1)
		{
			hvRes[nStep][n++] = ";";
		}
		DispCross(HW.WndID, hvRow, hvCol, 36, 0);
	}
	return true;
}

bool IsFuncRegionSort(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 5;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	CString strMethod = sLeafs[0];                                                                                                                                                    //得到排序方法
	CString strASC = sLeafs[1];
	CString strDir = sLeafs[2];
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	SortRegion(hoImage[nStep + iImgInx], &hoImage[nStep], strMethod.GetBuffer(0), strASC.GetBuffer(), strDir.GetBuffer(0));
	strDir.ReleaseBuffer();
	strMethod.ReleaseBuffer();
	strASC.ReleaseBuffer();
	HTuple hvNs;
	CountObj(hoImage[nStep], &hvNs);
	HTuple hvS, hvR, hvC;
	AreaCenter(hoImage[nStep], &hvS, &hvR, &hvC);	
	int nNum = hvNs.I();
	if (nNum == 0)
		return false;
	hvRes[nStep] = HTuple();
	int n = 0;
	for (int i = 0; i < nNum; i++)
	{
		hvRes[nStep][n++] = hvC[i].D();        //符合条件的面积数量
		hvRes[nStep][n++] = hvR[i].D();        //符合条件的面积数量
		hvRes[nStep][n++] = hvS[i].D();        //符合条件的面积数量
		if (i != nNum - 1)
		{
			hvRes[nStep][n++] = ";";
		}
		if (strOrder.Right(1) == "*")
			LogFile("\n排序后轮廓%d的面积是:%.2f,中心点(%.1f,%.1f)", i + 1, hvS[i].D(), hvC[i].D(), hvR[i].D());
	}
	return true;
}

bool IsFuncRegionProcess(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	CString strType = sLeafs[0];
	double fPara = atof(sLeafs[1]);                                
	int nKnot = atoi(sLeafs[nSumLeaf - 1]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	HTuple hvRow, hvCol, hvRadius,hvLen1,hvLen2,hvPhi;
	HTuple hvU, hvL, hvD, hvR,hvS;
	hvRes[nStep] = HTuple();
	if (strType == "最大内切圆")
	{
		InnerCircle(hoImage[nStep + nKnot], &hvRow, &hvCol, &hvRadius);
		GenCircle(&hoImage[nStep], hvRow, hvCol, hvRadius);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvRadius;
	}
	else if (strType == "最大外接圆")
	{
		SmallestCircle(hoImage[nStep + nKnot], &hvRow, &hvCol, &hvRadius);
		GenCircle(&hoImage[nStep], hvRow, hvCol, hvRadius);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvRadius;
	}
	else if (strType == "凸包")
	{
		ShapeTrans(hoImage[nStep + nKnot], &hoImage[nStep], "convex");
		AreaCenter(hoImage[nStep], &hvS, &hvRow, &hvCol);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvS;
	}
	else if (strType == "填充")
	{
		if (fPara < 0)
		{
			FillUp(hoImage[nStep + nKnot], &hoImage[nStep]);
		}
		else
		{
			FillUpShape(hoImage[nStep + nKnot], &hoImage[nStep], "area", 0, fPara);
		}
		AreaCenter(hoImage[nStep], &hvS, &hvRow, &hvCol);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvS;
	}
	else if (strType == "最小外接矩形")
	{
		SmallestRectangle2(hoImage[nStep + nKnot], &hvRow, &hvCol,&hvPhi,&hvLen1,&hvLen2);
		GenRectangle2(&hoImage[nStep], hvRow, hvCol, hvPhi,hvLen1,hvLen2);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvPhi;
		hvRes[nStep][3] = hvLen1;
		hvRes[nStep][4] = hvLen2;
	}
	else if (strType == "正外接矩形")
	{
		SmallestRectangle1(hoImage[nStep + nKnot], &hvU, &hvL, &hvD, &hvR);
		GenRectangle1(&hoImage[nStep], hvU, hvL, hvD, hvR);
		hvRes[nStep][0] = hvL;
		hvRes[nStep][1] = hvU;
		hvRes[nStep][2] = hvD;
		hvRes[nStep][3] = hvR;
	}
	else
	{
		LogFile("类型输入错误");
		return false;
	}
	DispObj(hoImage[nStep], HW.WndID);	
	DispCross(HW.WndID, hvRow, hvCol, 36, 0.25*PI);
	if (strOrder.Right(1) == "*")  //显示注释
	{
		if (strType == "最大内切圆" || strType == "最大外接圆")
			LogFile("处理后轮廓中心坐标(%.1,%.1f),半径为：%.1f", hvCol.D(), hvRow.D(), hvRadius.D());
		else if (strType == "凸包" || strType == "填充")
			LogFile("处理后轮廓中心坐标(%.1,%.1f)", hvCol.D(), hvRow.D());
		else if (strType == "最小外接矩形")
			LogFile("外接矩形中心坐标(%.1,%.1f),角度:%.2f,长：%.1f,宽:%.1f", hvCol.D(), hvRow.D(),hvPhi*180/PI,hvLen1.D(),hvLen2.D());
		else if (strType == "正外接矩形")
			LogFile("矩形左上角点(%.1,%.1f),右下角点(%.1f,%.1f)", hvRes[nStep][0], hvRes[nStep][1], hvRes[nStep][2], hvRes[nStep][3]);
	}
	return true;
}

bool IsFuncMaxInnerCircle(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 2;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
	{
		return false;
	}
	int nKnot = atoi(sLeafs[nSumLeaf - 1]); 
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	HTuple hvRow, hvCol, hvRadius;
	InnerCircle(hoImage[nStep + nKnot], &hvRow, &hvCol, &hvRadius);
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvCol;
	hvRes[nStep][1] = hvRow;
	hvRes[nStep][2] = hvRadius;
	if (strOrder.Right(1) == "*")  //显示注释
	{
		LogFile("最大内接圆的圆心坐标(%.1,%.1f),半径为：%.1f",hvCol.D(),hvRow.D(),hvRadius.D());
	}
	DispCross(HW.WndID, hvRow, hvCol, 36, 0.25*PI);
	DispCircle(HW.WndID, hvRow, hvCol, hvRadius);
	return true;
}

bool IsFuncRegionMinDist(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iOne = atoi(sLeafs[0]);
	int iTwo = atoi(sLeafs[1]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	HTuple hvNum, hvNO;
	HObject hoTwo, hoOne;
	CountObj(hoImage[nStep + iImgInx], &hvNum);
	int nNum = hvNum.I();
	if (nNum < 2 || iOne < 0 || iTwo < 0)
		return false;
	if (iTwo >= nNum)
	{
		iTwo = nNum - 1;
	}
	if (iOne >= nNum)
	{
		iOne = nNum - 1;
	}
	if (iOne == iTwo)
	{
		if (strOrder.Right(1) == "*")
			LogFile("同一个轮廓无法测量距离！");
		return false;
	}
	SelectObj(hoImage[nStep + iImgInx], &hoOne, iOne + 1);
	SelectObj(hoImage[nStep + iImgInx], &hoTwo, iTwo + 1);
	HTuple hvMinDist, hvRow1, hvCol1, hvRow2, hvCol2;
	DistanceRrMin(hoOne, hoTwo, &hvMinDist, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
	DispLine(HW.WndID, hvRow1, hvCol1, hvRow2, hvCol2);
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvCol1;
	hvRes[nStep][1] = hvRow1;
	hvRes[nStep][2] = hvCol2;
	hvRes[nStep][3] = hvRow2;	
	hvRes[nStep][4] = hvMinDist;
	if (strOrder.Right(1) == "*")
	{
		LogFile("轮廓上点(%.1f,%.1f)和点(%.1f,%.1f)之间距离最近：%.1f",hvCol1.D(),hvRow1.D(),hvCol2.D(),hvRow2.D(),hvMinDist.D());
	}
	return true;
}

bool IsFuncGetCons(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	CString strMethod = sLeafs[0];                                                                             //得到排序方法
	int nInx = atoi(sLeafs[1]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);		
	HTuple hvNum;
	if (nInx >= 0)
	{
		HObject hoXLD;
		GenContourRegionXld(hoImage[nStep + iImgInx], &hoXLD, strMethod.GetBuffer());
		strMethod.ReleaseBuffer();
		CountObj(hoXLD, &hvNum);
		if (strOrder.Right(1) == "*")
		{
			LogFile("共计有%d 个轮廓,选择了第%d 个轮廓",hvNum.I(),nInx + 1);
		}
		if (hvNum.I() < 1)
		{
			return false;
		}
		if (hvNum.I() > nInx + 1)
			SelectObj(hoXLD, &hoImage[nStep], hvNum.I());
		else
			SelectObj(hoXLD, &hoImage[nStep], nInx + 1);
	}
	else
	{
		GenContourRegionXld(hoImage[nStep + iImgInx], &hoImage[nStep], strMethod.GetBuffer());
		strMethod.ReleaseBuffer();
		CountObj(hoImage[nStep], &hvNum);
		if (strOrder.Right(1) == "*")
		{
			LogFile("共计有%d 个轮廓,选择全部轮廓", hvNum.I());
		}
		if (hvNum.I() < 1)	
			return false;
	}
	DispObj(hoImage[nStep], HW.WndID);
	return true;
}

/************************************************************************/
/* 这个函数暂时不要用                                                                     */
/************************************************************************/
bool IsFuncConsDist(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 8;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iImgInx = atoi(sLeafs[nSumLeaf - 1]);
	CString strConsSel = sLeafs[0];                              //得到排序方法
	CString strOne = sLeafs[1];
	CString strTwo = sLeafs[2];
	CString strMethod = sLeafs[3];
	CString strDistType = sLeafs[4];                             //距离类型，最小距离，还是最大距离，默认最小距离
	int nDistType = 0;
	if (strDistType == "最大距离")
		nDistType = 1;
	CString strSel = sLeafs[5];                                      //选择最小值，还是最大值
	HTuple hvNum;
	CountObj(hoImage[nStep], &hvNum);
	hvRes[nStep] = HTuple();
	int nCons = hvNum.I();
	if (nCons < 2)
	{
		if (strOrder.Right(1) == "*")
			LogFile("轮廓数小于2，无法计算距离！");
		return false;
	}
	HObject hoOne, hoTwo;
	HTuple hvDistType[2];
	if (strConsSel == "所有轮廓")			                     //所有轮廓间的最小或者最大距离/////////////////////////////
	{
		double fDist = 90000;
		CPos psNum;
		psNum.x = psNum.y = -1;
		for (int i = 0; i < nCons - 1; i++)
		{
			SelectObj(hoImage[nStep + iImgInx], &hoOne, i + 1);
			for (int j = 0; j < nCons; j++)
			{
				if (i == j)
					continue;
				SelectObj(hoImage[nStep + iImgInx], &hoTwo, j + 1);
				DistanceCc(hoOne, hoTwo, strMethod.GetBuffer(), &hvDistType[0], &hvDistType[1]);
				strMethod.ReleaseBuffer();
				if (strSel == "最大值")
				{
					if (fDist < hvDistType[nDistType])
					{
						fDist = hvDistType[nDistType].D();
						psNum.x = i;
						psNum.y = j;
					}
				}
				else
				{
					if (fDist > hvDistType[nDistType])
					{
						fDist = hvDistType[nDistType].D();
						psNum.x = i;
						psNum.y = j;
					}
				}
			}
		}
		hvRes[nStep][0] = fDist;                                    //输出所有轮廓中，距离最小或者最大的距离值
		hvRes[nStep][1] = (int)(psNum.x);                     //轮廓1索引
		hvRes[nStep][2] = (int)(psNum.y);                     //轮廓2索引
		//////////////////////////////////接下来搜索这两个轮廓上哪两个点最小或者最大/////////////////////////////////////
	}
	else                                                                      //指定的两个轮廓之间的最小距离
	{
		SelectObj(hoImage[nStep + iImgInx], &hoOne, atoi(strOne) + 1);
		if (strTwo == "最大值")
		{
			HTuple hvNO;
			CountObj(hoImage[nStep], &hvNO);
			SelectObj(hoImage[nStep], &hoTwo, hvNO);
		}
		else
			SelectObj(hoImage[nStep], &hoTwo, atoi(strTwo));
		DistanceCc(hoOne, hoTwo, strMethod.GetBuffer(), &hvDistType[0], &hvDistType[1]);
		strMethod.ReleaseBuffer();
		double fDist = hvDistType[nDistType].D();   //轮廓距离
		hvRes[nStep][0] = fDist;
		hvRes[nStep][1] = atoi(strOne);
		hvRes[nStep][2] = atoi(strTwo);
	}
	return true;
}

bool IsFuncMarginMsrLine(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nStep < 1)
			return false;
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 14;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		CString strPosTYP = sLeafs[nSumLeaf - 1];				
		CStrs ssPosTYP;
		pSng->SplitString(strPosTYP, ",", ssPosTYP);
		double fLen[2] = { 0 };
		fLen[0] = atof(sLeafs[3]);
		fLen[1] = atof(sLeafs[4]);
		bool bDraw = false;
		if (fLen[0] < 1 || fLen[1] < 1)
		{
			if (nID < 0)
			{
				if (ssPosTYP[0] != "结果点")
					bDraw = true;
			}
			else
			{
				LogFile("%s 检测框长宽参数有误！", strFunc);
				return false;
			}
		}
		int iKnot = max(0, nStep - 1);
		if (ssPosTYP.size() >= 2)
		{
			iKnot = max(0, nStep + atoi(ssPosTYP[1]));
		}
		HTuple hvR, hvC, hvPhi, hvLen1, hvLen2;
		double fRCVal[7];                                                                                                                          //X,Y,Phi,L1,L2,Sigma,Thres	
		if (bDraw == true)
		{
			HTREEITEM hChildItem = tree.GetChildItem(hItem);
			pSng->GetDrawRect2(HW, tree, fRCVal, strPosTYP, hChildItem, hvRes[iKnot]);
			pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs);
		}
		CString strMethod;
		int nArraySum, iLnSel, iImgInx;                                               //选择测量出的边
		CString strArryDir;
		double fStepLen = 1;
		for (int j = 0; j < nSumLeaf; j++)
		{
			if (j < 7)
			{
				fRCVal[j] = atof(sLeafs[j]);
				if (j < 2)
					fRCVal[j] *= ZOOM;
			}
			else if (j == 7)
				strMethod.Format("%s", sLeafs[j]);
			else if (j == 8)
				iLnSel = atoi(sLeafs[j]);
			else if (j == 9)
				nArraySum = atoi(sLeafs[j]);
			else if (j == 10)
				strArryDir.Format("%s", sLeafs[j]);
			else if (j == 11)
				fStepLen = atof(sLeafs[j]);
			else if (j == nSumLeaf - 2)
				iImgInx = atoi(sLeafs[j]);
		}
		HTuple hvMat2D, hvTransY, hvTransX, hvMsr;
		double fRCCen[2];
		pSng->GetAffineRect(fRCVal, strPosTYP, hvRes[iKnot], hvTransX, hvTransY);  //TransXY是测量区的中心点
		fRCCen[0] = hvTransX.D();
		fRCCen[1] = hvTransY.D();
		HTuple hvW, hvH;
		HalconCpp::GetImageSize(HW.hoImage, &hvW, &hvH);                                    //再不改变窗口图像参数的情况下，可以直接采用窗口图像
		double fImageSize[2];
		fImageSize[0] = hvW.D();
		fImageSize[1] = hvH.D();
		vector<double> fLosts;                                                                                             //检索缺失的角度  
		hvRes[nStep] = HTuple();
		fLosts.clear();
		int iRes = 0, iAMT = 0;
		double fStepX, fStepY,fMsrPhi,fDirPhi;
		if (strArryDir == "正方向")
		{
			fDirPhi = fRCVal[2] + 0.5*PI;
		}
		else
			fDirPhi = fRCVal[2] - 0.5*PI;
		fStepX = fStepLen*cos(-1.0*fDirPhi);
		fStepY = fStepLen*sin(-1.0*fDirPhi);
		fMsrPhi = hvRes[iKnot][2].D() + fRCVal[2];
		for (int j = 0; j < nArraySum; j++)
		{
			double fCenMsr[2];
			fCenMsr[0] = fRCCen[0] + j*fStepX;
			fCenMsr[1] = fRCCen[1] + j*fStepY;
			double fAbsAmp = fRCVal[6], fRange[2] = { -PI, PI };
			HTuple hvY, hvX, hvAmp, hvDist;			
			///////////////////////////////////////////////////////////////////测量角度是原始角度 + 模板差的角度////////////////////////////////////////
			//DispRectangle2(HW.WndID, fCenMsr[1], fCenMsr[0], fMsrAng, fRCVal[3], fRCVal[4]);
			GenMeasureRectangle2(fCenMsr[1], fCenMsr[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
			if (iImgInx == 0 || nStep + iImgInx < 0)
			{
				MeasurePos(HW.hoImage, hvMsr, fRCVal[5], fAbsAmp, strMethod.GetBuffer(), "all", &hvY, &hvX, &hvAmp, &hvDist);
			}
			else
			{
				MeasurePos(hoImage[nStep + iImgInx], hvMsr, fRCVal[5], fAbsAmp, strMethod.GetBuffer(), "all", &hvY, &hvX, &hvAmp, &hvDist);
			}
			strMethod.ReleaseBuffer();
			CloseMeasure(hvMsr);
			int n = hvY.TupleLength().I();
			if (n == 0)
			{
				fLosts.push_back(1.0*j);              //如果没有检测到就加入索引	
				if (fLosts.size() == 3)
				{
					LogFile("%s 累计 3 个边缘点没有检测！", strFunc);
					return false;
				}
				continue;
			}
			if (iLnSel >= n)                                        //要使用的边大于 全部搜索到的边
				iLnSel = n - 1;
			if (iLnSel >= 0)
			{
				hvRes[nStep][iRes++] = hvX[iLnSel].D();
				hvRes[nStep][iRes++] = hvY[iLnSel].D();
				hvRes[nStep][iRes++] = hvAmp[iLnSel].D();
				hvRes[nStep][iRes++] = ";";
				iAMT++;
				DispCross(HW.WndID, hvY[iLnSel], hvX[iLnSel], 36, 0);
				continue;
			}
			DispCross(HW.WndID, hvY, hvX, 36, 0);
			for (int k = 0; k < n; k++)
			{
				hvRes[nStep][iRes++] = hvX[k].D();
				hvRes[nStep][iRes++] = hvY[k].D();
				hvRes[nStep][iRes++] = hvAmp[k].D();
				if (k < n - 1)
					hvRes[nStep][iRes++] = ",";
			}
			hvRes[nStep][iRes++] = ";";
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		LogFile("直线边缘测量失败：%s", pc);
		return false;
	}
	return true;
}

bool IsFuncMarginMsrCircle(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nStep < 1)
		return false;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 16;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	double fLen[2] = { 0 };
	fLen[0] = atof(sLeafs[3]);
	fLen[1] = atof(sLeafs[4]);
	CString strPosTYP;
	strPosTYP.Format("%s", sLeafs[nSumLeaf - 1]);
	CStrs ssPosTYP;
	pSng->SplitString(strPosTYP, ",", ssPosTYP);
	bool bDraw = false;
	if (fLen[0] < 1 || fLen[1] < 1)
	{
		if (nID < 0)
		{	
			if (ssPosTYP[0] != "结果点")
				bDraw = true;
		}
		else
		{
			LogFile("%s 检测框长宽参数有误！", strFunc);
			return false;
		}
	}
	HTuple hvR, hvC, hvPhi, hvLen1, hvLen2;
	double fRCVal[7];                                                                                                                          //X,Y,Phi,L1,L2,Sigma,Thres	
	if (bDraw == true)
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		int nInx = 0;
		if (ssPosTYP.size() == 2)
		{
			nInx = atoi(ssPosTYP[1]);
		}
		pSng->GetDrawRect2(HW, tree, fRCVal, strPosTYP, hChildItem, hvRes[max(0,nStep + nInx)]);
		pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs);
	}
	CString strMethod;
	int nArraySum, iLnSel,iKnot = max(0,nStep - 1),iTimes = 0,iFilterRadius,iImgInx;                                               //选择测量出的边
	double fDegST, fDegRange;
	for (int j = 0; j < nSumLeaf; j++)
	{
		if (j < 7)
		{
			fRCVal[j] = atof(sLeafs[j]);
			if (j < 2)
				fRCVal[j] *= ZOOM;
		}
		else if (j == 7)
			strMethod.Format("%s", sLeafs[j]);
		else if (j == 8)
			iLnSel = atoi(sLeafs[j]);
		else if (j == 9)
			nArraySum = atoi(sLeafs[j]);
		else if (j == 10)
			fDegST = atof(sLeafs[j]);
		else if (j == 11)
			fDegRange = atof(sLeafs[j]);        //阵列步长
		else if (j == 12)                                   //对应角度优化，测量的数量
			iTimes = atoi(sLeafs[j]);
		else if (j == 13)
			iFilterRadius = atoi(sLeafs[j]);
		else if (j == nSumLeaf - 2)
			iImgInx = atoi(sLeafs[j]);
	}
	////////////////////////////////////////////////////////////////////////////////////////
	HTuple hvMat2D, hvTransY, hvTransX,hvMsr,hvRs,hvCs;
	hvRs = HTuple();
	hvCs = HTuple();
	double fRCCen[2],fModPos[2];
	if (ssPosTYP.size() >= 2)
	{
		iKnot = max(0,nStep + atoi(ssPosTYP[1]));
	}
	pSng->GetAffineRect(fRCVal,strPosTYP, hvRes[iKnot], hvTransX, hvTransY);  //TransXY是测量区的中心点
	fRCCen[0] = hvTransX.D();
	fRCCen[1] = hvTransY.D();
	fModPos[1] =  hvRes[iKnot][1];
	fModPos[0] = hvRes[iKnot][0];
	HTuple hvW, hvH;
	HalconCpp::GetImageSize(HW.hoImage, &hvW, &hvH);                                      //再不改变窗口图像参数的情况下，可以直接采用窗口图像
	double fImageSize[2],fCenMsr[2];
	fCenMsr[0] = fRCCen[0];
	fCenMsr[1] = fRCCen[1];
	fImageSize[0] = hvW.D();
	fImageSize[1] = hvH.D();
	vector<double> fLosts;                                                                    //检索缺失的角度  
	int nLostNum = 0;                                                                              //连续丢失的测量数
A:	hvRes[nStep] = HTuple();
	fLosts.clear();
	double fPhiStep = fDegRange / nArraySum * PI / 180;
	int iRes = 0,iAMT = 0;
	for (int j = 0; j < nArraySum; j++)
	{
		double fMsrPhi = fDegST*PI / 180.0 + fRCVal[2] + j*fPhiStep;      //模板搜索到的偏差角度 + 检测框角度 + 阵列步长角度
		HTuple HomMat2D, HomMat2DRot, hvRow, hvCol;
		HomMat2dIdentity(&HomMat2D);
		//HomMat2dRotate(HomMat2D, fPhiStep * j, fModPos[1], fModPos[0], &HomMat2DRot); //旋转了多少一定的角度
		//AffineTransPoint2d(HomMat2DRot, fRCCen[1], fRCCen[0], &hvRow, &hvCol);
		HomMat2dRotate(HomMat2D, fPhiStep * (nLostNum + 1), fModPos[1], fModPos[0], &HomMat2DRot); //旋转了多少一定的角度
		AffineTransPoint2d(HomMat2DRot, fCenMsr[1], fCenMsr[0], &hvRow, &hvCol);
		fCenMsr[0] = hvCol.D();
		fCenMsr[1] = hvRow.D();
		double fAbsAmp = fRCVal[6],fRange[2] = { -PI, PI };
		HTuple hvY, hvX, hvAmp, hvDist;
		//DispRectangle2(HW.WndID, fCenMsr[1], fCenMsr[0], fMsrPhi, fRCVal[3], fRCVal[4]);
		GenMeasureRectangle2(fCenMsr[1], fCenMsr[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
		MeasurePos(hoImage[nStep + iImgInx], hvMsr, fRCVal[5], fAbsAmp, strMethod.GetBuffer(), "all", &hvY, &hvX, &hvAmp, &hvDist);
		strMethod.ReleaseBuffer();
		CloseMeasure(hvMsr);
		int n = hvY.TupleLength().I();
		if (n == 0)
		{
			nLostNum++;
			fLosts.push_back(fMsrPhi);         //如果没有检测到就加入一个测量角度		
			if (fLosts.size() == 3)
			{
				LogFile("%s 累计没有检测到 3 个边缘点！",strFunc);
				return false;
			}
			continue;
		}
		nLostNum = 0;
		if (iLnSel >= n)                                        //要使用的边大于 全部搜索到的边
			iLnSel = n - 1;
		if (iLnSel >= 0)
		{
			hvRes[nStep][iRes++] = hvX[iLnSel].D();
			hvRes[nStep][iRes++] = hvY[iLnSel].D();
			hvRes[nStep][iRes++] = hvAmp[iLnSel].D();
			hvRes[nStep][iRes++] = ";";
			hvRs[iAMT] = hvY[iLnSel].D();
			hvCs[iAMT] = hvX[iLnSel].D();
			iAMT++;
			DispCross(HW.WndID, hvY[iLnSel], hvX[iLnSel], 36, 0);
			continue;
		}
		DispCross(HW.WndID, hvY, hvX, 36, 0);
		for (int k = 0; k < n; k++)
		{
			hvRes[nStep][iRes++] = hvX[k].D();
			hvRes[nStep][iRes++] = hvY[k].D();
			hvRes[nStep][iRes++] = hvAmp[k].D();
			if (k < n - 1)
				hvRes[nStep][iRes++] = ",";
		}
		hvRes[nStep][iRes++] = ";";
	}
	if (iTimes == 0 || iLnSel < 0)   //没有指定测量边或者优化拟合 == 0 不需要优化
		return true;
	if (iAMT < 3)
	{
		if (strOrder.Right(1) == "*")
			LogFile("没有足够的点 %d 拟合圆！",iAMT);
		return false;
	}
	HObject hoCons,hoCir;
	HTuple hv_Row, hv_Col, hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
	HalconCpp::GenContourPolygonXld(&hoCons, hvRs, hvCs);
	HalconCpp::FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	HalconCpp::GenCircle(&hoCir, hv_Row, hv_Col, hv_Radius);
	iTimes--;
	if (iTimes <= 0)
	{
		GenContourRegionXld(hoCir, &hoImage[nStep], "border");
		DispObj(hoImage[nStep], HW.WndID);
	}
	fModPos[0] = hv_Col.D();
	fModPos[1] = hv_Row.D();
	if (iTimes > 0)
		goto A;
	return true;
}

bool IsFuncMarginMsr(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nStep < 1)
		return false;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 16;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	hvRes[nStep] = HTuple();
	double fLen[2] = { 0 };
	fLen[0] = atof(sLeafs[3]);
	fLen[1] = atof(sLeafs[4]);
	bool bDraw = false;
	if (fLen[0] < 1 || fLen[1] < 1)
	{
		if (nID < 0)
			bDraw = true;
		else
		{
			LogFile("%s 检测框长宽参数有误！", strFunc);
			return false;
		}
	}
	HTuple hvR, hvC, hvPhi, hvLen1, hvLen2;
	double fRCVal[7];                                                                       //X,Y,Phi,L1,L2,Sigma,Thres	
	if (bDraw == true)
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		CStrs Ps;
		pSng->SplitString(sLeafs[nSumLeaf - 1], ",", Ps);
		int nInx = 0;
		if (Ps.size() == 2)
		{
			nInx = atoi(Ps[1]);
		}
		pSng->GetDrawRect2(HW, tree, fRCVal,sLeafs[nSumLeaf - 1], hChildItem, hvRes[nStep + nInx]);
		pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs);
	}
	CString strMethod, strArrayDir;
	int nArraySum = 1, nArrayCircle = 1, iLnSel;                            //选择测量出的边
	int nMsrAngNum = 0;
	double fArrayStep[2];                                                                //阵列步长，如果是线性阵列就是DX，DY，如果是圆形阵列就只有参数1 是角度步长
	for (int j = 0; j < nSumLeaf; j++)
	{
		if (j < 7)
		{
			fRCVal[j] = atof(sLeafs[j]);
			if (j < 2)
				fRCVal[j] *= pSng->_fZoom;
		}
		else if (j == 7)
			strMethod.Format("%s", sLeafs[j]);
		else if (j == 8)
		{
			iLnSel = atoi(sLeafs[j]);
		}
		else if (j == 9)
		{
			nArraySum = atoi(sLeafs[j]);
		}
		else if (j == 10)
		{
			nArrayCircle = atoi(sLeafs[j]);
		}
		else if (j == 11 || j == 12)
		{
			fArrayStep[j - 11] = atof(sLeafs[j]); //阵列步长
		}
		else if (j == 13)                                   //对应角度优化，测量的数量
			nMsrAngNum = atoi(sLeafs[j]);
		else if (j == 14)
			strArrayDir.Format("%s", sLeafs[j]);
	}
	//////////////////////////接下来开始正式提取想要的那条边/////////////////////////			
	HTuple hvMat2D, hvTransY, hvTransX;
	double fCenRC[2];
	int nInx = atoi(sLeafs[nSumLeaf - 1]);
	pSng->GetAffineRect(fRCVal, sLeafs[nSumLeaf - 1], hvRes[nStep + nInx], hvTransX, hvTransY);
	fCenRC[0] = hvTransX.D();
	fCenRC[1] = hvTransY.D();
	HTuple hvW, hvH;
	GetImageSize(HW.hoImage, &hvW, &hvH);                             //再不改变窗口图像参数的情况下，可以直接采用窗口图像
	double fImageSize[2];
	fImageSize[0] = hvW.D();
	fImageSize[1] = hvH.D();
	int iRes = 0, iLnNum = 0;
	double fMsrPhi = hvRes[nStep + nInx][2].D() + fRCVal[2];       //模板搜索到的偏差角度 + 检测框角度 + 阵列步长角度
	vector<double> fDirAngs;                                                           //检索缺失的角度   
	int nLastLostNum = 0;
	for (int j = 0; j < nArraySum; j++)
	{
		HTuple hvMsr, hvY, hvX, hvAmp, hvDist, hvMaxAmp, Ys, Xs;
		if (nArraySum > 1 && j > 0)
		{
			//要改变每一个检测框的位置
			if (nArrayCircle)
			{
				fMsrPhi += fArrayStep[0];
				HTuple HomMat2D, HomMat2DRot, hvRow, hvCol;
				HomMat2dIdentity(&HomMat2D);
				double fRotCen[2];
				fRotCen[0] = hvRes[nStep + nInx][0];
				fRotCen[1] = hvRes[nStep + nInx][1];
				//if (1)
				//{
				//	HomMat2dRotate(HomMat2D, fArrayStep[0] * j, fRotCen[1], fRotCen[0], &HomMat2DRot); //旋转了多少一定的角度
				//	AffineTransPoint2d(HomMat2DRot, hvTransY, hvTransX, &hvRow, &hvCol);
				//}
				//else
				{
					HomMat2dRotate(HomMat2D, fArrayStep[0] * (nLastLostNum + 1), fRotCen[1], fRotCen[0], &HomMat2DRot); //旋转了多少一定的角度
					AffineTransPoint2d(HomMat2DRot, fCenRC[1], fCenRC[0], &hvRow, &hvCol);
				}
				fCenRC[0] = hvCol.D();
				fCenRC[1] = hvRow.D();
			}
			else                                                   //直线阵列
			{
				fCenRC[0] += fArrayStep[0];
				fCenRC[1] += fArrayStep[1];
			}
		}
		bool bGet = false;
		double fDelta[2], fMaxAmpCen[2], fAbsAmp = fRCVal[6], fFitPhi, fRange[2] = { -PI, PI };
		fDelta[0] = sin(fMsrPhi);
		fDelta[1] = cos(fMsrPhi);
		Xs = HTuple();
		Ys = HTuple();
		int n = 0;
		fMaxAmpCen[0] = fCenRC[0];
		fMaxAmpCen[1] = fCenRC[1];
		for (int k = 0; k < nMsrAngNum && nArrayCircle; k++)
		{
			HTuple hvTmpX, hvTmpY;
			if (strArrayDir == "双向")
			{
				fDelta[0] = sin(fMsrPhi)*pow(-1, k);
				fDelta[1] = cos(fMsrPhi)*pow(-1, k);
			}
			else if (strArrayDir == "X+" && fDelta[0] < 0 ||
				strArrayDir == "X-" && fDelta[0] > 0 ||
				strArrayDir == "Y+" && fDelta[1] < 0 ||
				strArrayDir == "Y-" && fDelta[1] > 0)
			{
				fDelta[0] *= -1;
				fDelta[1] *= -1;
			}
			double fMsrCen[2];
			fMsrCen[0] = fCenRC[0] + fArrayStep[1] * fDelta[0];
			fMsrCen[1] = fCenRC[1] + fArrayStep[1] * fDelta[1];
			if (fMsrCen[1] < 0 || fMsrCen[1] > fImageSize[1] - 1 || fMsrCen[0] < 0 || fMsrCen[0] > fImageSize[0] - 1)
				continue;
			GenMeasureRectangle2(fMsrCen[1], fMsrCen[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
			MeasurePos(HW.hoImage, hvMsr, fRCVal[5], fRCVal[6], strMethod.GetBuffer(), "all", &hvTmpY, &hvTmpX, &hvAmp, &hvDist);
			strMethod.ReleaseBuffer();
			CloseMeasure(hvMsr);
			int m = hvTmpX.TupleLength().I();
			if (m == 0)
				continue;
			if (ABS(hvAmp.D()) > fAbsAmp)
			{
				fMaxAmpCen[0] = hvTmpX[0].D();
				fMaxAmpCen[1] = hvTmpY[0].D();
				fAbsAmp = ABS(hvAmp.D());                //搜索最大梯度位置
			}
			Xs[n] = hvTmpX[0];
			Ys[n] = hvTmpY[0];
			n++;
		}
		if (nMsrAngNum > 0 && nArrayCircle)
		{
			bool bFitOK = pSng->GetFitLineAng(HW.WndID, Ys, Xs, fFitPhi, 6, false);  //修正测量角度
			if (bFitOK == false)
			{
				nLastLostNum++;			
				fDirAngs.push_back(fMsrPhi);         //如果没有检测到就加入一个测量角度
				if (fDirAngs.size() == 2)
				{
					double fMidAng = 0.5*(fDirAngs[1] + fDirAngs[0]);        //判断中间那个角度是否在运动的开口范围之内
					if (fMidAng < 30 * PI / 180 || fMidAng > 150 * PI / 180)  //开口方向在（30度到150度之间是合理的）
						return false;
				}
				continue;
			}
			double fAngErr = 0.3;
			for (int k = 0; k < 2; k++)
			{
				double fAng = pSng->GetVertAng(fFitPhi, fRange, k == 0);
				if (ABS(fAng - fMsrPhi) < fAngErr || ABS(fAng - fMsrPhi) > 2.0* PI - fAngErr)
				{
					fMsrPhi = fAng;
					break;
				}
			}
			//////以上代码优化的下一次的测量方向角度/////////////////////////
		}
		GenMeasureRectangle2(fMaxAmpCen[1], fMaxAmpCen[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
		MeasurePos(HW.hoImage, hvMsr, fRCVal[5], max(fRCVal[6], 0.8*fAbsAmp), strMethod.GetBuffer(), "all", &hvY, &hvX, &hvAmp, &hvDist);
		strMethod.ReleaseBuffer();
		CloseMeasure(hvMsr);
		n = hvY.TupleLength().I();
		if (n == 0)
		{
			hvRes[nStep][iRes++] = 0;
			hvRes[nStep][iRes++] = 0;
			hvRes[nStep][iRes++] = 0;
			hvRes[nStep][iRes++] = ";";
			if (nArrayCircle && nMsrAngNum == 0)
				fDirAngs.push_back(fMsrPhi);                              //如果没有检测到就加入一个测量角度		
			if (fDirAngs.size() == 3)
			{
				//double fMidAng = 0.5*(fDirAngs[1] + fDirAngs[0]);//判断中间那个角度是否在运动的开口范围之内
				//if (fMidAng < 30 * PI / 180 || fMidAng > 150 * PI / 180)  //开口方向在（30度到150度之间是合理的）
				LogFile("累计没有检测到 3 个边缘点！");
				return false;
			}
			continue;
		}
		if (nArrayCircle)
		{
			//nLostNum = 0;                                //找到了清空，找到了就重新计数，如果有连续3个缺失了，就说明开口方向不对
			nLastLostNum = 0;
			fCenRC[0] = fMaxAmpCen[0];
			fCenRC[1] = fMaxAmpCen[1];
			fDirAngs.clear();                               //找到了就清空
			//DispRectangle2(HW.WndID, fMaxAmpCen[1], fMaxAmpCen[0], fMsrPhi, fRCVal[3], fRCVal[4]);  //只有搜索到了才显示检测框
		}	
		if (iLnSel > n)            //要使用的边大于 全部搜索到的边
			iLnSel = n - 1;
		if (iLnSel >= 0)
		{
			hvRes[nStep][iRes++] = hvX[iLnSel].D();
			hvRes[nStep][iRes++] = hvY[iLnSel].D();
			hvRes[nStep][iRes++] = hvAmp[iLnSel].D();
			hvRes[nStep][iRes++] = ";";
			DispCross(HW.WndID, hvY[iLnSel], hvX[iLnSel], 36, 0);
			continue;
		}
		DispCross(HW.WndID, hvY, hvX, 36, 0);
		for (int k = 0; k < n; k++)
		{
			hvRes[nStep][iRes++] = hvX[k].D();
			hvRes[nStep][iRes++] = hvY[k].D();
			hvRes[nStep][iRes++] = hvAmp[k].D();
			if (k < n - 1)
				hvRes[nStep][iRes++] = ",";
		}
		hvRes[nStep][iRes++] = ";";
	}
	//if (nLostNum > 2)
	//{
	//	LogFile("没有检测到%d 个点！",nLostNum);
	//	return false;
	//}
	return true;
}

bool IsFuncLoadRCMod(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID >= 0)
	{
		if (pSng->_XMLImg[nID][nStep].TMs.nM[0] < 0)	           //必然载入了 NCC模板
		{
			LogFile("没有载入模板文件！");
			return false;
		}
		return true;
	}
	int nSumLeaf = 2;
	if (pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs) == false)
		return false;		
	try
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		CString strTmpName, strVal,strOffset;
		if (sLeafs[0].GetLength() == 0)
		{		
			HTuple hvPos[4],hvNCC, hvW, hvH;
			int nPosTxt[2] = { 0 };
			HW.ShowImage(nPosTxt,"设置图像模板区域");
			HW.WinTexts.clear();
			DrawRectangle1(HW.WndID, &hvPos[0], &hvPos[1], &hvPos[2], &hvPos[3]);
			HObject hoRC, hoCrop, hoPart, hoCurr, hoZoom;
			CopyImage(HW.hoImage, &hoCurr);
			GetImageSize(HW.hoImage,&hvW, &hvH);
			ZoomImageSize(hoCurr, &hoZoom, ZOOM*hvW, ZOOM*hvH, "constant");
			GenRectangle1(&hoRC, ZOOM*hvPos[0], ZOOM*hvPos[1], ZOOM*hvPos[2], ZOOM*hvPos[3]);
			ReduceDomain(hoZoom, hoRC, &hoPart);
			CropDomain(hoPart, &hoCrop);
			CDlgTmp dlg;
			CopyImage(hoCrop, &dlg.hoTMP);	
			HW.ShowImage();
			if (dlg.DoModal() != IDOK)
			{
				return false;
			}
			//第 1 个节点就是模板名称
			if (dlg.strTmpName.GetLength() == 0)
				return false;
			strTmpName.Format("D:\\TMP\\%s_%d", dlg.strTmpName,dlg.m_nTmpTYP);
			tree.SetItemText(hChildItem, "模板名=" + strTmpName);
			hChildItem = tree.GetNextSiblingItem(hChildItem);                      //对应模板的中心偏差
			//第 2 个节点就是中心偏差	
			tree.SetItemText(hChildItem, "中心偏差=" + dlg.strOffset);
			if (dlg.m_nTmpTYP == TMP_NCC)
			{			
				CreateNccModel(hoCrop, "auto", HTuple(-20).TupleRad(), HTuple(40).TupleRad(), "auto", "use_polarity", &hvRes[nStep]);
				WriteNccModel(hvRes[nStep], strTmpName.GetBuffer(0));
				strTmpName.ReleaseBuffer();
			}
			else if (dlg.m_nTmpTYP == TMP_SHM)
			{
				CreateShapeModelXld(dlg.hoXLD, "auto", -0.5, 1, "auto", "point_reduction_high", "ignore_local_polarity", 5, &hvRes[nStep]);
				WriteShapeModel(hvRes[nStep], strTmpName.GetBuffer(0));
				strTmpName.ReleaseBuffer();
			}
			sLeafs[0].Format("%s",strTmpName);
			sLeafs[1].Format("%s", dlg.strOffset);
		}
		///////////////////////////////读取模板文件/////////////////////////////////////////////////
		strOffset.Format("%s", sLeafs[1]);
		strTmpName.Format("%s", sLeafs[0]);
		CStrs ssName, ssDelta;
		pSng->SplitString(strTmpName, ";", ssName);
		pSng->SplitString(strOffset, ";", ssDelta);
		if (ssName.size() != ssDelta.size())
		{
			return false;
		}
		hvRes[nStep] = HTuple();
		for (int i = 0; i < (int)ssName.size(); i++)
		{
			HTuple hvTM;
			int nTYP = atoi(ssName[i].Right(1));
			if (nTYP == TMP_NCC)
			{
				ReadNccModel(ssName[i].GetBuffer(), &hvTM);
				ssName[i].ReleaseBuffer();
			}
			else if (nTYP == TMP_SHM)
			{
				ReadShapeModel(ssName[i].GetBuffer(), &hvTM);
				ssName[i].ReleaseBuffer();
			}
			hvRes[nStep][4 * i] = hvTM;
			CStrs ssXY;
			pSng->SplitString(ssDelta[i], ",", ssXY);
			hvRes[nStep][4 * i + 1] = atof(ssXY[0]);
			hvRes[nStep][4 * i + 2] = atof(ssXY[1]);
			hvRes[nStep][4 * i + 3] = nTYP;
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("模板设定失败：%s", pc);
		LogFile(str);
		return false;
	}
	return true;
}

bool IsFuncFindTmp(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nStep < 1)
			return false;
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 7;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		double fScore = 0.01*atof(sLeafs[0]);                           //最低分
		double fAngST = atof(sLeafs[1]);
		double fAngRange = atof(sLeafs[2]);
		int iFind = atoi(sLeafs[3]);
		int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		CString strDef = sLeafs[4];                                           //缺省位置
		HTuple hvR, hvC, hvPhi, hvScore, hvH, hvW, hvColTrans, hvRowTrans, hvMat2D, hvXs, hvYs, hvAs, hvCreds;
		hvXs = HTuple();
		hvYs = HTuple();
		hvAs = HTuple();
		hvCreds = HTuple();
		int nCount = 0;
		if (nStep + iImgInx < 0)
		{
			LogFile(strFunc + "图节点索引错误！");
			return false;
		}
		HObject hoZoom;
		HalconCpp::GetImageSize(hoImage[nStep + iImgInx], &hvW, &hvH);
		ZoomImageSize(hoImage[nStep + iImgInx], &hoZoom, ZOOM*hvW, ZOOM*hvH, "constant");
		int nNum = hvRes[nStep + iKnot].TupleLength().I();
		for (int i = 0; i < nNum && nID < 0; i += 4)
		{
			/************************************************************************/
			/* 要先判断模板是NCC还是Shape
			/************************************************************************/
			if (hvRes[nStep + iKnot][i + 3].I() == TMP_NCC)
			{
				FindNccModel(hoZoom, hvRes[nStep + iKnot][i], fAngST, fAngRange, fScore, iFind, 0.2, "true", 0, &hvR, &hvC, &hvPhi, &hvScore);
				ClearNccModel(hvRes[nStep + iKnot][i]);
			}
			else
			{
				FindShapeModel(hoZoom, hvRes[nStep + iImgInx][i], fAngST, fAngRange, fScore, iFind, 0.2, "least_squares", 0, 0.8, &hvR, &hvC, &hvPhi, &hvScore);
				ClearShapeModel(hvRes[nStep + iKnot][i]);
			}
			int nFind = hvScore.TupleLength().I();
			if (nFind == 0)
				continue;
			for (int j = 0; j < nFind; j++)
			{
				VectorAngleToRigid(0, 0, 0, hvR[j], hvC[j], hvPhi, &hvMat2D);
				AffineTransPoint2d(hvMat2D, hvRes[nStep + iKnot][i + 2] * ZOOM, hvRes[nStep + iKnot][i + 1] * ZOOM, &hvRowTrans, &hvColTrans);
				double fPt[2];
				fPt[0] = hvColTrans.D() / ZOOM;
				fPt[1] = hvRowTrans.D() / ZOOM;
				hvXs[nCount] = fPt[0];
				hvYs[nCount] = fPt[1];
				hvAs[nCount] = hvPhi;
				hvCreds[nCount] = hvScore;
				nCount++;
			}
			break;
		}
		//////////////////////以上是测试状态，下图是生产状态/////////////////////////////////////////
		for (int i = 0; i < MAX_TM && nID >= 0; i++)
		{
			int nTYP = pSng->_XMLImg[nID][nStep + iKnot].TMs.nM[i];
			if (nTYP == TMP_NCC)
			{
				FindNccModel(hoZoom, pSng->_XMLImg[nID][nStep + iKnot].TMs.hvM[i], fAngST, fAngRange, fScore, iFind, 0.5, "true", 0, &hvR, &hvC, &hvPhi, &hvScore);
			}
			else if (nTYP == TMP_SHM)
			{
				FindShapeModel(hoZoom, pSng->_XMLImg[nID][nStep + iKnot].TMs.hvM[i], fAngST, fAngRange, fScore, iFind, 0.2, "least_squares", 0, 0.8, &hvR, &hvC, &hvPhi, &hvScore);
			}
			else
				continue;
			int nFind = hvScore.TupleLength().I();
			if (nFind == 0)
				continue;
			double fDXY[2] = { 0 };                                                                                    //当前模板相对模板中心的偏差
			fDXY[0] = pSng->_XMLImg[nID][nStep + iKnot].TMs.fPar[i][0];
			fDXY[1] = pSng->_XMLImg[nID][nStep + iKnot].TMs.fPar[i][1];
			for (int j = 0; j < nFind; j++)
			{
				VectorAngleToRigid(0, 0, 0, hvR[j], hvC[j], hvPhi, &hvMat2D);
				AffineTransPoint2d(hvMat2D, fDXY[1] * ZOOM, fDXY[0] * ZOOM, &hvRowTrans, &hvColTrans);
				double fPt[2];
				fPt[0] = hvColTrans.D() / ZOOM;
				fPt[1] = hvRowTrans.D() / ZOOM;
				hvXs[nCount] = fPt[0];
				hvYs[nCount] = fPt[1];
				hvAs[nCount] = hvPhi;
				hvCreds[nCount] = hvScore;
				nCount++;
			}
			break;
		}
		hvRes[nStep] = HTuple();                                                                                      //分组压入结果数组，每组4个结果
		if (nCount < 1)
		{
			bool bGet = false;
			CStrs ssDef, ssVal;
			pSng->SplitString(strDef, ";", ssDef);
			for (int j = 0; j < (int)ssDef.size(); j++)
			{
				pSng->SplitString(ssDef[j], ",", ssVal);
				if (ssVal.size() >= 3)
				{
					hvRes[nStep][j * 5 + 0] = atof(ssVal[0]);
					hvRes[nStep][j * 5 + 1] = atof(ssVal[1]);
					hvRes[nStep][j * 5 + 2] = atof(ssVal[2]);
					hvRes[nStep][j * 5 + 3] = 0.0;
					hvRes[nStep][j * 5 + 4] = ";";
					bGet = true;
				}
			}
			if (bGet)
			{
				if (strOrder.Right(1) == "*")
					LogFile("NCC模板没有匹配模板，采用默认点！");
				return true;
			}
			if (strOrder.Right(1) == "*")
				LogFile("NCC模板没有匹配的！");
			return false;
		}
		for (int j = 0; j < nCount; j++)
		{
			hvRes[nStep][j * 5 + 0] = hvXs[j];
			hvRes[nStep][j * 5 + 1] = hvYs[j];
			hvRes[nStep][j * 5 + 2] = hvAs[j];
			hvRes[nStep][j * 5 + 3] = hvCreds[j];
			hvRes[nStep][j * 5 + 4] = ";";
			DispCross(HW.WndID, hvYs[j], hvXs[j], 66, 0);
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		LogFile("%s 失败! : %s", strFunc, pc);
		return false;
	}	
	return true;
}

bool IsFuncRCRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 5;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	bool bDraw = false;
	if (atof(sLeafs[2]) < 9 || atof(sLeafs[3]) < 9)
	{
		if (nID < 0)
			bDraw = true;
		else
		{
			LogFile("方形区域右下角坐标错误");
			return false;
		}
	}
	if (bDraw)                                //先设置好搜索区域
	{
		HTuple hvPos[4];
		int nPosTxt[2] = { 0 };
		HW.ShowImage(nPosTxt,"框选方形区域");
		HW.WinTexts.clear();
		DrawRectangle1(HW.WndID, &hvPos[0], &hvPos[1], &hvPos[2], &hvPos[3]);
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		CString strVal;
		for (int i = 0; i < 4; i++)
		{
			strVal.Format("%.1f", hvPos[i].D());
			CString strKnot = tree.GetItemText(hChildItem);
			CStringArray sts;
			pSng->SplitString(strKnot, "=", sts);
			tree.SetItemText(hChildItem, sts[0] + "=" + strVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
		//接下来根据搜索区域裁剪图片
		hChildItem = tree.GetChildItem(hItem);
		pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs);
	}
	//////////////////每个区域的都分析一遍///////////////////////
	HObject hoRC, hoUnion,hoPart; 
	CStringArray stsRC[4];
	for (int i = 0; i < 4; i++)
		pSng->SplitString(sLeafs[i], ",", stsRC[i]);
	int nRegs = stsRC[0].GetCount();
	for (int i = 0; i < nRegs; i++)
	{
		GenRectangle1(&hoRC, atof(stsRC[0][i]), atof(stsRC[1][i]), atof(stsRC[2][i]), atof(stsRC[3][i]));
		HObject hoTwo;
		if (i > 0)
		{
			Union2(hoUnion, hoRC, &hoTwo);
			Union1(hoTwo, &hoUnion);
		}
		else
		{
			Union1(hoRC, &hoUnion);
		}
		DispRectangle1(HW.WndID, atof(stsRC[0][i]), atof(stsRC[1][i]), atof(stsRC[2][i]), atof(stsRC[3][i]));
	}
	ReduceDomain(HW.hoImage, hoUnion, &hoImage[nStep]);  //方向区域始终用原始图裁剪
	//CropDomain(hoImage[nStep], &hoPart);
	//HTuple hvYs, hvXs, hvVals,hvW,hvH;
	////GetGrayval(hoPart, hvYs, hvXs, &hvVals);
	//GetImageSize(hoImage[nStep], &hvW, &hvH);
	//GenRectangle1(&hoRC, 0, 0, hvH - 1, hvW - 1);
	//CRect rc;
	//int i = 0;
	//rc.top = atof(stsRC[0][i]);
	//rc.left = atof(stsRC[1][i]);
	//rc.bottom = atof(stsRC[2][i]);
	//rc.right = atof(stsRC[3][i]);
	//OverpaintRegion(hoImage[nStep], hoRC, 0, "fill");
	//for (int i = 0; i < hvW.I();i ++)
	//{
	//	for (int j = 0; j < hvH.I();j++)
	//	{
	//		CPoint p;
	//		p.x = i;
	//		p.y = j;
	//		if (PtInRect(rc, p))
	//		{
	//			GetGrayval(HW.hoImage, j, i, &hvVals);
	//			SetGrayval(hoImage[nStep], j, i, hvVals);
	//		}
	//	}
	//}
	//DispObj(hoImage[nStep], HW.WndID);
	return true;
}

bool IsFuncDirRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 6;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	hvRes[nStep] = HTuple();
	double fLen[2] = { 0 };
	fLen[0] = atof(sLeafs[3]);
	fLen[1] = atof(sLeafs[4]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	bool bDraw = false;
	if (fLen[0] < 1 || fLen[1] < 1)
	{
		if (nID < 0)
			bDraw = true;
		else
		{
			LogFile(strFunc + "参数错误！");
			return false;
		}
	}
	int nKnot = -1;
	double fRCVal[5];  //X,Y,A,L1,L2
	CStrs Ps;
	pSng->SplitString(sLeafs[nSumLeaf - 1], ",", Ps);
	if (Ps.size() == 2)
	{
		nKnot = atoi(Ps[1]);
	}
	if (bDraw == true)
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		pSng->GetDrawRect2(HW, tree, fRCVal, sLeafs[nSumLeaf - 1], hChildItem, hvRes[nStep + nKnot]);
	}
	for (int j = 0; j < 5; j++)
	{
		fRCVal[j] = atof(sLeafs[j]);
		if (j < 2)
			fRCVal[j] *= ZOOM;
	}
	HObject hoRC;
	HTuple hvTransX, hvTransY;
	pSng->GetAffineRect(fRCVal, sLeafs[nSumLeaf - 1], hvRes[nStep + nKnot], hvTransX, hvTransY);
	GenRectangle2(&hoRC, hvTransY, hvTransX, fRCVal[2], fRCVal[3], fRCVal[4]);
	ReduceDomain(HW.hoImage, hoRC, &hoImage[nStep]);
	DispObj(hoRC, HW.WndID);
	return true;
}

bool IsFuncCircleRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	double fRadius = atof(sLeafs[2]);
	bool bDraw = false;
	if (fRadius < 1)
	{
		if (nID < 0)
			bDraw = true;
		else
			return false;
	}
	double fXYR[3];
	if (bDraw)
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		CStrs Ps;
		pSng->SplitString(sLeafs[nSumLeaf - 1],",",Ps);
		int iInx = 0;
		if (Ps.size() == 2)
		{
			iInx = atoi(Ps[1]);
		}
		bool bRet = pSng->GetDrawCircle(HW, tree, fXYR, sLeafs[nSumLeaf - 1], hChildItem, hvRes[nStep + iInx]);
		if (bRet == false)
		{
			MessageBox(NULL, "圆区域绘制错误", "", MB_ICONERROR);
			return false;
		}
		pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem);
	}
	/////////////////////////////////////////////////////////////////////
	for (int j = 0; j < 3; j++)
	{
		fXYR[j] = atof(sLeafs[j]);
		if (j < 2)
			fXYR[j] *= ZOOM;
	}
	HObject hoCir;
	HTuple hvTransX, hvTransY;
	CString strPosTYP;
	strPosTYP.Format("%s", sLeafs[3]);
	CStrs Ps;
	pSng->SplitString(strPosTYP, ",", Ps);
	int iKnot = -1;
	if (Ps.size() >= 2)
	{
		iKnot = atoi(Ps[1]);
	}
	if (strPosTYP == "固定位" || strPosTYP == "固定点")
	{
		hvTransX = fXYR[0];
		hvTransY = fXYR[1];
	}
	else if (strPosTYP.Left(6) == "模板差" || strPosTYP.Left(6) == "结果点")
	{
		pSng->GetAffineRect(fXYR, strPosTYP, hvRes[nStep + iKnot], hvTransX, hvTransY);
	}
	else
		return false;
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvTransX;
	hvRes[nStep][1] = hvTransY;
	hvRes[nStep][2] = fRadius;
	if (strOrder.Right(1) == "*")
		LogFile("圆心坐标(%.1f,%.1f) ，半径:%.1f",hvTransX.D(),hvTransY.D(),fRadius);
	GenCircle(&hoCir, hvTransY, hvTransX, fXYR[2]);
	ReduceDomain(HW.hoImage, hoCir, &hoImage[nStep]);
	DispObj(hoCir, HW.WndID);
	return true;
}

bool IsFuncRingRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 5;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	hvRes[nStep] = HTuple();
	double fRadius[2] = { 0 };
	fRadius[0] = atof(sLeafs[2]);
	fRadius[1] = atof(sLeafs[3]);
	bool bDraw = false;
	if (fRadius[0] < 1 || fRadius[1] < 1)
	{
		if (nID < 0)
			bDraw = true;
		else
			return false;
	}
	double fRadiusVal[4];
	if (bDraw)
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);
		CStrs Ps;
		pSng->SplitString(sLeafs[nSumLeaf - 1], ",", Ps);
		int iKnot = -1;
		if (Ps.size() >= 2)
		{
			iKnot = atoi(Ps[1]);
		}
		bool bRet = pSng->GetDrawRing(HW, tree, fRadiusVal, sLeafs[nSumLeaf - 1], hChildItem, hvRes[nStep + iKnot]);
		if (bRet == false)
		{
			MessageBox(NULL, "圆环区域绘制错误！！", "", MB_ICONERROR);
			return false;
		}
		pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem);
	}
	for (int j = 0; j < 4; j++)
	{
		fRadiusVal[j] = atof(sLeafs[j]);
		if (j < 2)
			fRadiusVal[j] *= ZOOM;
	}
	HObject hoOCir, hoICir, hoRing;
	HTuple hvTransX, hvTransY;		
	CString strPosTYP;
	strPosTYP.Format("%s", sLeafs[nSumLeaf - 1]);
	CStrs Ps;
	pSng->SplitString(strPosTYP, ",", Ps);
	int iKnot = -1;
	if (Ps.size() >= 2)
	{
		iKnot = atoi(Ps[1]);
	}
	if (strPosTYP == "固定位" || strPosTYP == "固定点")
	{
		hvTransX = fRadiusVal[0];
		hvTransY = fRadiusVal[1];
	}
	else if (strPosTYP.Left(6) == "模板差")
	{
		pSng->GetAffineRect(fRadiusVal, strPosTYP, hvRes[nStep  + iKnot], hvTransX, hvTransY);
	}
	else if (strPosTYP.Left(6) == "结果点")
	{
		hvTransX = hvRes[nStep + iKnot][0];
		hvTransY = hvRes[nStep + iKnot][1];
	}
	GenCircle(&hoOCir, hvTransY, hvTransX, fRadiusVal[2]);
	GenCircle(&hoICir, hvTransY, hvTransX, fRadiusVal[3]);
	Difference(hoOCir, hoICir, &hoRing);
	ReduceDomain(HW.hoImage, hoRing, &hoImage[nStep]);
	DispObj(hoRing, HW.WndID);
	return true;
}

bool IsFuncFitCircle(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 4;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		int iSel = atoi(sLeafs[0]);                                 //点节点或者轮廓节点
		if (iSel < 0)
		{
			LogFile("点索引错误，无法拟合圆！");
			return false;
		}
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		CString strType = sLeafs[1];
		HTuple hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Row, hv_Col, hvAng;
		if (strType == "点")
		{
			int iRes = hvRes[nStep + iKnot].TupleLength().I();
			if (iRes < 3)
			{
				LogFile("没有足够的点拟合圆！");
				return false;
			}
			CString strData = pSng->GetHTupleString(hvRes[nStep + iKnot]);
			CStringArray ss;
			pSng->SplitString(strData, ";", ss);		                                                 //每一个检测区的结果数组
			HObject hoCons, hoCir;
			HTuple hvRs, hvCs;
			int iAMT = 0;
			for (int j = 0; j < ss.GetCount(); j++)
			{
				CStringArray Ds, Xs;
				pSng->SplitString(ss[j], ",", Ds);
				if (iSel >= Ds.GetCount() && Ds.GetCount() > 0)
				{
					iSel = Ds.GetUpperBound();                                                      //最后一个元素
				}
				pSng->SplitString(Ds[iSel], "*", Xs);
				double fX, fY;
				fX = atof(Xs[0]);
				fY = atof(Xs[1]);
				if (fX < ESP || fY < ESP)
					continue;
				hvRs[iAMT] = fY;
				hvCs[iAMT] = fX;
				iAMT++;
			}
			if (iAMT < 3)
			{
				LogFile("没有足够合格的圆轮廓点%d 拟合圆！", iAMT);
				return false;
			}
			/////////////////////////////////////////接下来拟合圆心,前期的点已经优化过了，所以不需要进一步过滤//////////////////////////////////////////////////////////////////
			GenContourPolygonXld(&hoCons, hvRs, hvCs);
			FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
			GenCircle(&hoCir, hv_Row, hv_Col, hv_Radius);
			GenContourRegionXld(hoCir, &hoImage[nStep], "border");
			DispObj(hoImage[nStep], HW.WndID);
		}
		else if (strType == "轮廓")
		{
			int iImgInx = -1;
			CStrs Ps;
			pSng->SplitString(strType, ",", Ps);
			if (Ps.size() >= 2)
			{
				iImgInx = atoi(Ps[1]);
			}
			FitCircleContourXld(hoImage[nStep + iImgInx], "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
		}
		hvRes[nStep] = HTuple();
		hvRes[nStep][0] = hv_Col;
		hvRes[nStep][1] = hv_Row;
		hvRes[nStep][2] = hv_Radius;
		double fCenR[3];
		fCenR[0] = hv_Col.D();
		fCenR[1] = hv_Row.D();
		fCenR[2] = hv_Radius.D();
		DispCircle(HW.WndID, hv_Row, hv_Col, hv_Radius);
		DispCross(HW.WndID, hv_Row, hv_Col, 60, 0);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		LogFile("%s 失败! : %s", strFunc,pc);
		return false;
	}
	return true;
}

bool IsFuncLineAng(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 2;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	HTuple hvA1, hvA2, hvAx, hvRowA, hvColA, hvRowB, hvColB;
	int iKnot = atoi(sLeafs[nSumLeaf - 1]);
	double fDegRef = atof(sLeafs[0]);                                  //拟合直线的参考角度
	hvColA = hvRes[nStep + iKnot][0];
	hvRowA = hvRes[nStep + iKnot][1];
	hvColB = hvRes[nStep + iKnot][2];
	hvRowB = hvRes[nStep + iKnot][3];
	double fP[4];
	for (int i = 0; i < 4;i++)
	{
		fP[i] = hvRes[nStep + iKnot][i].D();
	}
	if (abs(fP[0] - fP[2]) < ESP && abs(fP[1] - fP[3]) < ESP)
	{
		LogFile("直线的两个点太接近，无法计算角度");
		return false;
	}
	DispLine(HW.WndID, hvRowA, hvColA, hvRowB, hvColB);
	double fPtRef[2];
	fPtRef[0] = cos(-1.0*fDegRef*PI / 180);
	fPtRef[1] = sin(-1.0*fDegRef*PI / 180);
	AngleLl(0, 0, fPtRef[1], fPtRef[0], hvRowA, hvColA, hvRowB, hvColB, &hvA1);
	AngleLl(0, 0, fPtRef[1], fPtRef[0], hvRowB, hvColB, hvRowA, hvColA, &hvA2);
	double fDeg[2], fDirDeg;
	fDeg[0] = hvA1.D() * 180 / PI;
	fDeg[1] = hvA2.D() * 180 / PI;
	//选择夹角小的矢量方向，坐标虽然是用图像坐标，但是得出的角度是符合右手定则的
	if (abs(fDeg[0]) < abs(fDeg[1]))
	{
		fDirDeg = fDegRef + fDeg[0];
	}
	else
	{
		fDirDeg = fDegRef + fDeg[1];
	}	
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = fDirDeg;
	return true;
}

bool IsFuncLineXPoint(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 2;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iKnot = atoi(sLeafs[nSumLeaf - 1]);
	int iLnot = atoi(sLeafs[nSumLeaf - 2]);
	if (hvRes[nStep + iKnot].TupleLength().I() < 4 || hvRes[nStep + iLnot].TupleLength().I() < 4)
	{
		LogFile("节点数据不足，无法计算");
		return false;
	}
	HTuple hvCol, hvRow,hvParal;
	IntersectionLines(hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2], 
		                            hvRes[nStep + iLnot][1], hvRes[nStep + iLnot][0], hvRes[nStep + iLnot][3], hvRes[nStep + iLnot][2],&hvRow,&hvCol,&hvParal);
	if (hvParal.I() == 1)
	{
		LogFile("直线平行，没有交点");
		return false;
	}
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvCol.D();
	hvRes[nStep][1] = hvRow.D();
	DispCross(HW.WndID, hvRow, hvCol, 160, 0.25*PI);
	return true;
}

bool IsFuncFitLine(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 11;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		int iKnot = max(0, nStep + atoi(sLeafs[nSumLeaf - 1]));
		int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
		double fLnLen = atof(sLeafs[nSumLeaf - 3]);                                                          //直线长度
		CString strTYP = sLeafs[1];
		HTuple hvRowA, hvColA, hvRowB, hvColB, hvDist;
		if (strTYP == "点")
		{
			int iSel = atoi(sLeafs[0]);
			double fFilter = atof(sLeafs[6]);                                                                         //跳动过滤
			if (iSel < 0)
			{
				LogFile("%s 选择的点索引错误！", strFunc);
				return false;
			}
			int iRes = hvRes[iKnot].TupleLength().I();
			if (iRes < 7)
			{
				LogFile("%s 点数太少无法拟合直线！");
				return false;
			}
			CString strData = pSng->GetHTupleString(hvRes[iKnot]);
			CStrs ssGrp;
			pSng->SplitString(strData, ";", ssGrp);		                                                    //每一个检测区的结果数组
			HTuple hvRs, hvCs;
			int iAMT = 0;
			for (int j = 0; j < (int)ssGrp.size(); j++)
			{
				CStrs Ds, Xs;
				pSng->SplitString(ssGrp[j], ",", Ds);
				if (iSel >= (int)Ds.size() && Ds.size() > 0)
				{
					iSel = (int)Ds.size() - 1;                                              //最后一个元素
				}
				pSng->SplitString(Ds[iSel], "*", Xs);
				double fX = atof(Xs[0]);
				double fY = atof(Xs[1]);
				if (fX < ESP && fY < ESP)
					continue;
				hvRs[iAMT] = fY;
				hvCs[iAMT] = fX;
				iAMT++;
			}
			if (iAMT < 3)
			{
				LogFile("没有足够合格的点%d拟合直线！", iAMT);
				return false;
			}
			double fGrayLWMD[4], fPhi, fMean = 0;
			fGrayLWMD[0] = atof(sLeafs[2]);
			fGrayLWMD[1] = atof(sLeafs[3]);
			fGrayLWMD[2] = atof(sLeafs[4]);
			fGrayLWMD[3] = atof(sLeafs[5]);
			HTuple hvPhi, hvXs, hvYs, hvMeans, hvDevs, hvDist;
			bool bFitOK = pSng->GetFitLine(HW.WndID, hvRs, hvCs, hvRowA, hvColA, hvRowB, hvColB, 2.0*fFilter);
			if (bFitOK == false)
			{
				LogFile("%s 直线拟合失败！", strFunc);
				return false;
			}
			AngleLx(hvRowA, hvColA, hvRowB, hvColB, &hvPhi);
			fPhi = hvPhi.D() + 0.5*PI;       //检测框的矢量方向
			hvDevs = HTuple();
			hvMeans = HTuple();
			for (int i = 0; i < iAMT; i++)
			{
				HObject hoRC;
				HTuple hvMean, hvDev;
				GenRectangle2(&hoRC, hvRs[i], hvCs[i], fPhi, fGrayLWMD[0], fGrayLWMD[1]);
				if (iImgInx <= 0)
					Intensity(hoRC, HW.hoImage, &hvMean, &hvDev);
				else
					Intensity(hoRC, hoImage[iImgInx + nStep], &hvMean, &hvDev);
				fMean += hvMean.D();
				hvDevs[i] = hvDev;
				hvMeans[i] = hvMean;
			}
			fMean /= iAMT;
			int n = 0;
			hvXs = HTuple();
			hvYs = HTuple();
			for (int i = 0; i < iAMT; i++)
			{
				if (hvDevs[i].D() < fGrayLWMD[3])
					continue;
				if (abs(hvMeans[i].D() - fMean) > fGrayLWMD[2])
					continue;
				hvXs[n] = hvCs[i];
				hvYs[n] = hvRs[i];
				n++;
			}
			bFitOK = pSng->GetFitLine(HW.WndID, hvYs, hvXs, hvRowA, hvColA, hvRowB, hvColB, fFilter);
			if (bFitOK == false)
			{
				LogFile("%s GetFitLine拟合直线失败！", strFunc);
				return false;
			}
		}
		else
		{
			HTuple hvNr, hvNc;
			FitLineContourXld(hoImage[iImgInx], "tukey", -1, 0, 5, 2, &hvRowA, &hvColA, &hvRowB, &hvColB, &hvNr, &hvNc, &hvDist);
		}
		double fLnCen[2], fDelta[2],fVal[2][2];
		fVal[0][0] = hvColA.D();
		fVal[0][1] = hvRowA.D();
		fVal[1][0] = hvColB.D();
		fVal[1][1] = hvRowB.D();
		fLnCen[0] = 0.5*(hvColB.D() + hvColA.D());
		fLnCen[1] = 0.5*(hvRowB.D() + hvRowA.D());
		DistancePp(hvRowA, hvColA, hvRowB, hvColB, &hvDist);
		fDelta[0] = (hvColB.D() - hvColA.D()) / hvDist.D();
		fDelta[1] = (hvRowB.D() - hvRowA.D()) / hvDist.D();
		hvColA = fLnCen[0] - 0.5*fLnLen*fDelta[0];
		hvRowA = fLnCen[1] - 0.5*fLnLen*fDelta[1];
		hvColB = fLnCen[0] + 0.5*fLnLen*fDelta[0];
		hvRowB = fLnCen[1] + 0.5*fLnLen*fDelta[1];
		hvRes[nStep] = HTuple();
		hvRes[nStep][0] = hvColA;
		hvRes[nStep][1] = hvRowA;
		hvRes[nStep][2] = hvColB;
		hvRes[nStep][3] = hvRowB;
		//////////////////////////////////////////////////////
		HTuple hvA1, hvA2, hvAx;
		double fDegRef = atof(sLeafs[7]);                                    //拟合直线的参考角度    
		double fPhiRef = -1.0*fDegRef*PI / 180;                         //基于图像Y轴向下，所以要乘上-1
		AngleLl(0, 0, sin(fPhiRef), cos(fPhiRef), hvRowA, hvColA, hvRowB, hvColB, &hvA1);
		AngleLl(0, 0, sin(fPhiRef), cos(fPhiRef), hvRowB, hvColB, hvRowA, hvColA, &hvA2);
		double fDeg[2], fDirDeg;
		fDeg[0] = hvA1.D() * 180 / PI;
		fDeg[1] = hvA2.D() * 180 / PI;
		double fP[4] = { -1, 1, 0, 0 };
		for (int i = 0; i < 4; i++)
		{
			fP[i] = hvRes[nStep][i].D();
		}
		//选择夹角小的矢量方向，坐标虽然是用图像坐标，但是得出的角度是符合右手定则的
		if (abs(fDeg[0]) < abs(fDeg[1]))
		{
			fDirDeg = fDegRef + fDeg[0];
		}
		else
		{
			fDirDeg = fDegRef + fDeg[1];
		}
		hvRes[nStep][4] = fDirDeg;
		DispLine(HW.WndID, hvRowA, hvColA, hvRowB, hvColB);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		const char* pc = q.ErrorMessage().Text();
		LogFile("%s 失败：%s", strFunc, pc);
		return false;
	}	
	return true;
}

/************************************************************************/
/*         汇总某些节点的结果数据，在累加上自身的补偿值                
/************************************************************************/
bool IsFuncProcessData(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 4;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		CStrs ssKnot, ssInx, ssAdd;
		pSng->SplitString(sLeafs[0], ",", ssKnot);
		pSng->SplitString(sLeafs[1], ",", ssInx);
		pSng->SplitString(sLeafs[2], ",", ssAdd);
		if (ssInx.size() != ssKnot.size())
		{
			LogFile("节点数目和索引数不匹配！");
			return false;
		}
		int nPt = (int)ssKnot.size();
		hvRes[nStep] = HTuple();
		for (int i = 0; i < nPt; i++)
		{		
			int iKnot = atoi(ssKnot[i]);		
			int iInx = atoi(ssInx[i]);
			int iKnotSum = hvRes[nStep + iKnot].TupleLength().I();
			if (iKnotSum <= iInx)
			{
				LogFile("节点 %d 的数据索引%d 超过了该节点的数据总数%d !!",nStep + iKnot,iInx,iKnotSum);
				return false;
			}
			double fVal = hvRes[nStep + iKnot][iInx].D();
			double fAdd = 0;
			if ((int)ssAdd.size() > i)
			{
				fAdd = atof(ssAdd[i]);
			}
			hvRes[nStep][i] = hvRes[nStep + iKnot][iInx] + fAdd;
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		const char* pc = q.ErrorMessage().Text();
		LogFile("%s 失败：%s", strFunc, pc);
		return false;
	}		
	return true;
}

bool IsFuncProcessPt(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 3;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	CStringArray stsCof, stsInx;
	pSng->SplitString(sLeafs[0], ",", stsInx);
	pSng->SplitString(sLeafs[1], ",", stsCof);
	int iKnot = atoi(sLeafs[nSumLeaf - 1]);                                                         //点处理
	int nGrps = stsCof.GetCount();                                                                     //每组点取几个
	CString strData = pSng->GetHTupleString(hvRes[nStep + iKnot]);
	///////以上代码完成了点分组X*Y*Thres,X*Y*Thres; 这种格式/////////////////////////////////////////
	CStringArray ss;
	pSng->SplitString(strData, ";", ss);		                                                          //每个检测区的结果数组
	HTuple hvRs, hvCs;
	hvRs = HTuple();
	hvCs = HTuple();
	int iNO = 0;
	for (int j = 0; j < ss.GetCount(); j++)
	{
		CStringArray Ds, XYs;
		pSng->SplitString(ss[j], ",", Ds);
		if (Ds.GetCount() < stsCof.GetCount())
			continue;
		hvRs[iNO] = 0;
		hvCs[iNO] = 0;
		for (int k = 0; k < stsInx.GetCount(); k++)
		{
			int iSel = atoi(stsInx[k]);
			if (iSel >= Ds.GetCount() && Ds.GetCount() > 0)
			{
				iSel = Ds.GetUpperBound();                                            //最后一个元素
			}
			pSng->SplitString(Ds[iSel], "*", XYs);
			double fX, fY;
			fX = atof(XYs[0]);
			fY = atof(XYs[1]);
			if (fX < ESP && fY < ESP)
				continue;
			double fCoef = atof(stsCof[k]);
			hvRs[iNO] += fY*fCoef;
			hvCs[iNO] += fX*fCoef;
		}
		iNO++;
	}
	DispCross(HW.WndID, hvRs, hvCs, 36, 0);
	if (iNO == 0)
	{
		LogFile("没有足够的点");
		return false;
	}	
	hvRes[nStep] = HTuple();
	int iRes = 0;
	for (int j = 0; j < iNO; j++)
	{
		hvRes[nStep][iRes++] = hvCs[j];
		hvRes[nStep][iRes++] = hvRs[j];
		hvRes[nStep][iRes++] = ";";
	}
	return true;
}

bool IsFuncMeanDev(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 3;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	HTuple hvMean, hvDev;
	Intensity(hoImage[nStep + iImgInx], HW.hoImage, &hvMean, &hvDev);
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvMean.D();
	hvRes[nStep][1] = hvDev.D();
	if (strOrder.Right(1) == "*")
	{
		LogFile("灰度：%.1f 和方差：%.1f",hvMean.D(),hvDev.D());
	}
	return true;
}

bool IsFuncWriteINI(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 4;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		CString strSec, strKey, strVal;
		strSec.Format("%s", sLeafs[0]);
		strKey.Format("%s", sLeafs[1]);
		int nNum = atoi(sLeafs[2]);
		int iResLen = hvRes[nStep + iKnot].TupleLength().I();
		if (iResLen != nNum)
		{
			LogFile("节点 %d 数据长度 %d 与设定的数据长度 %d 不一致!!",nStep + iKnot,iResLen,nNum);
			return false;
		}
		for (int i = 0; i < nNum; i++)
		{
			CString strTmp;
			strTmp.Format("%.2f,", hvRes[nStep + iKnot][i].D());   //主要是节点访问失败
			strVal += strTmp;
		}
		strVal.TrimRight(",");
		pSng->SetCfgString(strSec, strKey, strVal);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("%s 失败：%s", strFunc, pc);
		LogFile(str);
		return false;
	}
	return true;
}


bool IsFuncSplitCons(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 8;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);  //图节点
	int nPar[3];
	nPar[0] = atoi(sLeafs[0]);
	nPar[1] = atoi(sLeafs[1]);
	nPar[2] = atoi(sLeafs[2]);
	CString strAtrrib = sLeafs[3];
	CString strType = sLeafs[4];
	CString strArcRange = sLeafs[5];
	CStrs sRs;
	pSng->SplitString(strArcRange,",",sRs);
	HObject hoSplit, hoObj, hoSel, hoCons;
	if (strAtrrib == "椭圆")
		SegmentContoursXld(hoImage[nStep + iImgInx], &hoSplit, "lines_ellipses", nPar[0], nPar[1], nPar[2]);
	else
		SegmentContoursXld(hoImage[nStep + iImgInx], &hoSplit, "lines_circles", nPar[0], nPar[1], nPar[2]);
	HTuple hvNum, hvLen, hvYs, hvXs, hvAttrib;
	CountObj(hoSplit, &hvNum);
	int nCons = hvNum.I();
	GenEmptyObj(&hoCons);
	for (int i = 0; i < nCons; i++)
	{
		SelectObj(hoSplit, &hoObj, i + 1);
		GetContourGlobalAttribXld(hoObj, "cont_approx", &hvAttrib);
		if (hvAttrib == -1 && strAtrrib == "线段" ||
			hvAttrib == 0 && strAtrrib == "椭圆"  ||
			hvAttrib == 1 && strAtrrib == "圆弧"  || strAtrrib == "全部")
		{
			ConcatObj(hoCons, hoObj, &hoCons);
		}
	}
	CountObj(hoCons, &hvNum);
	nCons = hvNum.I();
	if (nCons < 1)
		return false;
	double fLenSel = 1900000;
	if (strType == "最大值")
	{
		fLenSel = 10;
	}
	bool bFind = false;
	for (int i = 0; i < nCons; i++)
	{
		SelectObj(hoCons, &hoObj, i + 1);
		LengthXld(hoObj, &hvLen);
		if (hvLen.D() > fLenSel && strType == "最大值" ||
			hvLen.D() < fLenSel && strType == "最小值")
		{
			SelectObj(hoCons, &hoImage[nStep], i + 1);
			fLenSel = hvLen.D();
			bFind = true;
		}
		if (strType == "圆弧半径" && hvLen.D() > fLenSel)
		{
			HTuple hv_Row, hv_Col, hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
			FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
			double fRadius = hv_Radius.D();
			if (fRadius > atof(sRs[0]) && fRadius < atof(sRs[1]))
			{
				SelectObj(hoCons, &hoImage[nStep], i + 1);
				fLenSel = hvLen.D();
				bFind = true;
			}
		}
	}
	if (bFind == false)
	{
		LogFile("没有找到符合条件的圆轮廓！");
		return false;
	}
	SetColor(HW.WndID, "blue");
	DispObj(hoImage[nStep], HW.WndID);
	SetColor(HW.WndID, "green");
	return true;
}

bool IsFuncDistP2C(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs,sDeg;
		CString strOrder;
		strOrder.Format("%s", strFunc);
		strFunc.TrimRight("*");
		strFunc.TrimLeft("!");
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 5;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
		CString strAngRange = sLeafs[2];                                                //角度范围
		pSng->SplitString(strAngRange, ",", sDeg);
		double fDeg[2];
		fDeg[0] = atof(sDeg[0]);
		fDeg[1] = atof(sDeg[1]);
		int nPtNum = atoi(sLeafs[0]);                                                         //用于取平均的点数
		CString strType = sLeafs[1];
		if (nStep + iKnot < 0 || nStep + iImgInx < 0)
			return false;
		HTuple hvDist[2];
		double fPtPos[2];
		for (int i = 0; i < 2; i++)
		{
			fPtPos[i] = hvRes[nStep + iKnot][i].D();
		}
		//DistancePc(hoImage[nStep + iImgInx], fPtPos[1], fPtPos[0], &hvDist[0], &hvDist[1]);
		double fDistVal;
		CPos psClose;
		psClose.fVal = 10000;
		bool bFind = false;
		int nType = 0;			
		HTuple hvNum, hvPrjY, hvPrjX, hvYs, hvXs, hvTmp, hvAng;
		if (strType == "最大值")
		{
			nType = 1;
			fDistVal = hvDist[1].D();
			HObject hoSplit, hoObj;
			SegmentContoursXld(hoImage[nStep + iImgInx], &hoSplit, "lines_circles", 5, 4, 2);
			CountObj(hoSplit, &hvNum);
			int nCons = hvNum.I();
			double fMaxDist = 0;
			for (int i = 0; i < nCons; i++)
			{
				SelectObj(hoSplit, &hoObj, i + 1);
				DistancePc(hoObj, fPtPos[1], fPtPos[0], &hvDist[0], &hvDist[1]);
				if (hvDist[nType].D() < fMaxDist)
					continue;
				fMaxDist = hvDist[nType].D();
				//DispObj(hoObj, HW.WndID);
				GetContourXld(hoObj, &hvYs, &hvXs);
				int nPts = hvXs.TupleLength().I();
				int nAdd = nPtNum / 2;
				if (nAdd == 0)
					nAdd = 1;
				for (int j = 0; j < nPts; j += nAdd)
				{
					double fPt[2] = { 0 };
					int II = 0;
					for (int k = 0; k < nPtNum; k++)
					{
						if (j + k == nPts)
							break;
						fPt[0] += hvXs[j + k].D();
						fPt[1] += hvYs[j + k].D();
						II++;
					}
					fPt[0] /= II;
					fPt[1] /= II;
					DistancePp(fPt[1], fPt[0], fPtPos[1], fPtPos[0], &hvTmp);
					double fDistDiff = abs(hvTmp.D() - fMaxDist);
					if (fDistDiff > 0.1)
						continue;
					//接下来判断是不是在设定的角度之内

					psClose.x = fPt[0];
					psClose.y = fPt[1];
					psClose.fVal = hvTmp.D();
					bFind = true;
				}
			}
			if (bFind == false)
				return false;
		}
		else
		{
			HObject hoPtCon,hoSplit,hoObj;
			HTuple hvR1, hvC1, hvR2, hvC2,hvCurrDist;
			GenContourPolygonXld(&hoPtCon, fPtPos[1], fPtPos[0]);
			SegmentContoursXld(hoImage[nStep + iImgInx], &hoSplit, "lines_circles", 5, 4, 2);  
			//分割轮廓，分段找每一段轮廓上到圆心最近的点，而且轮廓上的点到圆心连线要在-90~90度之间
			CountObj(hoSplit, &hvNum);
			int nCons = hvNum.I();
			double fMinDist = 10000;
			for (int i = 0; i < nCons; i++)
			{
				SelectObj(hoSplit, &hoObj, i + 1);
				DistanceCcMinPoints(hoPtCon, hoObj, "fast_point_to_segment",&hvDist[0],&hvR1,&hvC1,&hvR2,&hvC2);
				psClose.y = hvR2.D();
				psClose.x = hvC2.D();
				//马上计算点到轮廓上点的直线方向
				AngleLx(fPtPos[1], fPtPos[0], psClose.y, psClose.x, &hvAng);
				DistancePp(hvR1, hvC1, hvR2, hvC2, &hvCurrDist);
				double fCurrDist = hvCurrDist.D();
				if (hvAng.D() >= fDeg[0] * PI / 180 && hvAng.D() <= fDeg[1] * PI / 180 && fCurrDist < fMinDist)					//只接受这个角度范围内的最近点
				{
					fMinDist = fCurrDist;
					psClose.x = hvC2.D();
					psClose.y = hvR2.D();
					bFind = true;
				}
			}
			if (bFind == false)
				return false;
		}
		DispLine(HW.WndID, psClose.y, psClose.x, fPtPos[1], fPtPos[0]);
		hvRes[nStep] = HTuple();
		hvRes[nStep][0] = psClose.x;
		hvRes[nStep][1] = psClose.y;
		hvRes[nStep][2] = hvDist[nType];
		AngleLx(fPtPos[1], fPtPos[0], psClose.y, psClose.x, &hvAng);
		hvRes[nStep][3] = hvAng.D() * 180 / PI;
		if (strOrder.Right(1) == "*")
		{
			LogFile("轮廓上最近点(%.1f,%.1f)，距离:%.1f,该连线水平夹角：%.1f",psClose.x,psClose.y,hvDist[nType].D(),hvAng.D()*180/PI);
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("%s 失败：%s", strFunc,pc);
		LogFile(str);
		return false;
	}
	return true;
}

bool IsFuncDistL2C(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{	
	//该算法将搜索轮廓上到直线最近（最远）的点
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iKnot = atoi(sLeafs[nSumLeaf - 1]);
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
	int iPtNum = atoi(sLeafs[0]);
	CString strType = sLeafs[1];
	if (nStep + iImgInx < 0 || nStep + iKnot < 0)
		return false;
	HTuple hvDist[2];
	double fPtVal[4];
	for (int i = 0; i < 4; i++)
	{
		fPtVal[i] = hvRes[nStep + iKnot][i].D();
	}
	DistanceLc(hoImage[nStep + iImgInx], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0],
		                                                               hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2], &hvDist[0], &hvDist[1]);
	double fDistVal;
	int nType = 0;
	if (strType == "最大值")
	{
		fDistVal = hvDist[1].D();
		nType = 1;
	}
	else
	{
		fDistVal = hvDist[0].D();
		nType = 0;
	}
	HObject hoSplit, hoObj;
	SegmentContoursXld(hoImage[nStep + iImgInx], &hoSplit, "lines_circles", 5, 4, 2);
	HTuple hvNum, hvPrjY, hvPrjX, hvYs, hvXs, hvTmp;
	CountObj(hoSplit, &hvNum);
	int nCons = hvNum.I();
	CPos psClose;
	bool bFind = false;
	psClose.fVal = 10000;
	for (int i = 0; i < nCons; i++)
	{
		SelectObj(hoSplit, &hoObj, i + 1);
		DistanceLc(hoObj, hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0],
			                             hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2],&hvDist[0], &hvDist[1]);
		if (abs(fDistVal - hvDist[nType].D()) > 0.5)
			continue;
		GetContourXld(hoObj, &hvYs, &hvXs);
		int nPts = hvXs.TupleLength().I();
		int nAdd = iPtNum / 2;
		if (nAdd == 0)
			nAdd = 1;
		for (int j = 0; j < nPts; j += nAdd)
		{
			double fPt[2] = { 0 };
			int II = 0;
			for (int k = 0; k < iPtNum; k++)
			{
				if (j + k == nPts)
					break;
				fPt[0] += hvXs[j + k].D();
				fPt[1] += hvYs[j + k].D();
				II++;
			}
			fPt[0] /= II;
			fPt[1] /= II;
			DistancePl(fPt[1], fPt[0], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0],
				                                      hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2], &hvTmp);
			double fCloseVal = abs(hvTmp.D() - fDistVal);
			if (fCloseVal < psClose.fVal)
			{
				psClose.x = fPt[0];
				psClose.y = fPt[1];
				psClose.fVal = fCloseVal;
				bFind = true;
			}
		}
		if (bFind == false)
			return false;
		ProjectionPl(psClose.y, psClose.x, hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0],
			                                                        hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2], &hvPrjY, &hvPrjX);
		DispLine(HW.WndID, psClose.y, psClose.x, hvPrjY, hvPrjX);
		/************************************************************************/
		/*                     结果数组要记住每个元素的位置              
		/************************************************************************/
		hvRes[nStep] = HTuple();
		hvRes[nStep][0] = psClose.x;
		hvRes[nStep][1] = psClose.y;
		hvRes[nStep][2] = hvPrjX.D();
		hvRes[nStep][3] = hvPrjY.D();
		hvRes[nStep][4] = hvDist[nType];
		HTuple hvAng;
		AngleLx(hvPrjY, hvPrjX, psClose.y, psClose.x, &hvAng);
		hvRes[nStep][5] = hvAng.D() * 180 / PI;
		break;
	}
	return true;
}

bool IsFuncResultRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	try
	{
		CSingleton* pSng = CSingleton::GetInstance();
		CStringArray sKeys;
		CStrs sLeafs;
		if (nID < 0)
			pSng->GetSecKeys(strFunc, sKeys);
		int nSumLeaf = 3;
		if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
			return false;
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
		CString strType = sLeafs[0];
		if (strType == "圆形区域")
		{
			GenCircle(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2]);
		}
		else if (strType = "正方形区域")
		{
			GenRectangle1(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3]);
		}
		else if (strType == "角度型区域")
		{
			GenRectangle2(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][4]);
		}
		else if (strType == "椭圆区域")
		{
			GenEllipse(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][4]);
		}
		else if (strType == "扇形区域")
		{
			double fRadius[2],fCen[2];
			fCen[0] = hvRes[nStep + iKnot][0].D();
			fCen[1] = hvRes[nStep + iKnot][1].D();
		    fRadius[0] = hvRes[nStep + iKnot][2].D();
			fRadius[1] = hvRes[nStep + iKnot][3].D();			
			double fStart = hvRes[nStep + iKnot][4].D();
			double fAng = hvRes[nStep + iKnot][5].D();
			if (fAng*fRadius[0] < 6 && fAng * fRadius[1] < 6)
			{
				LogFile("扇形区域太小！");
				return false;
			}
			HObject hoArc[2], hoLn[2],hoAll,hoUnion;
			double LnPt[4][2];
			ZeroMemory(LnPt, sizeof(LnPt));
			for (int i = 0; i < 2;i++)
			{
				double fArcLen = fAng*fRadius[i];
				int nDot = (int)(fArcLen*0.5);
				if (fArcLen < 6)
				{
					continue;
				}
				double fStepPhi = fAng / nDot;
				HTuple hvXs, hvYs;
				hvXs = HTuple();
				hvYs = HTuple();
				for (int j = 0; j <= nDot;j++)
				{
					hvXs[j] = hvRes[nStep + iKnot][0] + fRadius[i] * cos(fStart - j*fStepPhi);
					hvYs[j] = hvRes[nStep + iKnot][1] + fRadius[i] * sin(fStart - j*fStepPhi);
				}
				LnPt[i][0] = hvXs[0].D();
				LnPt[i][1] = hvYs[0].D();
				LnPt[i + 2][0] = hvXs[nDot].D();
				LnPt[i + 2][1] = hvYs[nDot].D();
				GenContourPolygonXld(&hoArc[i], hvYs, hvXs);
				ConcatObj(hoAll,hoArc[i],&hoAll);
			}
			for (int i = 0; i < 2;i++)
			{
				HTuple hvXs, hvYs;
				hvXs = HTuple();
				hvYs = HTuple();
				hvXs[0] = LnPt[i][0];
				hvYs[0] = LnPt[i][1];
				hvXs[1] = LnPt[i + 1][0];
				hvYs[1] = LnPt[i + 1][1];
				GenContourPolygonXld(&hoLn[i],hvYs,hvXs);
				ConcatObj(hoAll, hoLn[i], &hoAll);
			}
			UnionAdjacentContoursXld(hoAll, &hoUnion, 10, 1, "attr_keep");
			GenRegionContourXld(hoUnion, &hoImage[nStep], "filled");
		}
		return true;
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("模板设定失败：%s", pc);
		LogFile(str);
	}		
	return false;
}

/************************************************************************/
/* 不要用这个函数                                                                      */
/************************************************************************/
bool IsFuncRelRegion(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	CString strOrder;
	strOrder.Format("%s", strFunc);
	strFunc.TrimRight("*");
	strFunc.TrimLeft("!");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 4;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	int iKnot = nStep + atoi(sLeafs[nSumLeaf - 1]);
	if (iKnot < 0)
		return false;
	double fCen[2][2], fLen[2];
	fCen[0][0] = 0.5*(hvRes[iKnot][0].D() + hvRes[iKnot][2].D());
	fCen[0][1] = 0.5*(hvRes[iKnot][1].D() + hvRes[iKnot][3].D());
	fLen[0] = atof(sLeafs[1]);
	fLen[1] = atof(sLeafs[2]);
	if (fLen[0] < 1 || fLen[1] < 1)
	{
		LogFile("区域大小没有设置！");
		return false;
	}
	HTuple hvPhi, hvH, hvW;
	GetImageSize(HW.hoImage, &hvW, &hvH);
	AngleLx(hvRes[iKnot][1], hvRes[iKnot][0], hvRes[iKnot][3], hvRes[iKnot][2], &hvPhi);
	double fPhi = hvPhi.D();
	if (fPhi > 0.5*PI)
		fPhi -= PI;
	else if (fPhi < -0.5*PI)
		fPhi += PI;
	////////////////////////////////////////////
	CStringArray stsOffset;
	pSng->SplitString(sLeafs[0], ",", stsOffset);
	if (stsOffset.GetCount() != 3)
	{
		LogFile("%s 参数偏差错误！", strFunc);
		return false;
	}
	double fOffset[3];
	fOffset[0] = atof(stsOffset[0]);
	fOffset[1] = atof(stsOffset[1]);
	fOffset[2] = atof(stsOffset[2]);
	fCen[1][0] = fCen[0][0] + fOffset[0];
	fCen[1][1] = fCen[0][1] + fOffset[1];
	if (fCen[1][0] > hvW.D() - 10 || fCen[1][0] < 10 || fCen[1][1] > hvH.D() - 10 || fCen[1][1] < 10)
	{
		return false;
	}
	HObject hoRC;
	GenRectangle2(&hoRC, fCen[1][1], fCen[1][0], fPhi + fOffset[2], fLen[0], fLen[1]);
	ReduceDomain(HW.hoImage, hoRC, &hoImage[nStep]);
	DispObj(hoRC, HW.WndID);
	return true;
}

bool IsFuncCode2D(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	CStrs sLeafs;
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	int nSumLeaf = 3;
	if (pSng->LoadLeafs(nID, nStep, strFunc, nSumLeaf, tree, sLeafs, hItem) == false)
		return false;
	HTuple hvDecode, hvCode, hvOut, hvString;
	CreateDataCode2dModel(sLeafs[0].GetBuffer(), "default_parameters", sLeafs[1].GetBuffer(0), &hvDecode);
	sLeafs[0].ReleaseBuffer();
	sLeafs[1].ReleaseBuffer();
	FindDataCode2d(HW.hoImage, &hoImage[nStep], hvDecode, HTuple(), HTuple(), &hvOut, &hvString);
	ClearDataCode2dModel(hvDecode);	
	int nLen = hvString.TupleLength().I();
	if (nLen == 0)
		return false;	
	hvRes[nStep] = HTuple();
	hvRes[nStep][0] = hvString;
	DispObj(hoImage[nStep], HW.WndID);
	CString strShow = (CString)hvString[0].S();
	int nPosTxt[2] = { 1, 1};
	HW.ShowImage(nPosTxt,strShow,25,"green", false);
	return true;
}

bool RunFunc(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96],CString strType,CTreeCtrl& tree,HTREEITEM hItem)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CStringArray sKeys;
	int nSumLeaf;
	CString strFunc;
	strFunc.Format("%s",strType);
	strFunc.TrimRight("*");
	if (nID < 0)
		pSng->GetSecKeys(strFunc, sKeys);
	else
		nSumLeaf = pSng->_XMLImg[nID][nStep].psLeaf.size();                                 //叶节点数量
	bool bRet = true;
	if (strFunc == "输出结果")
	{			
		bRet = IsFuncOut(HW, nID,nStep,hoImage,hvRes,strType,tree,hItem);
	}
	else if (strFunc == "二值化" || strFunc == "!二值化")
	{
		bRet = IsFuncBinVal(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "方形开闭运算" || strFunc == "!方形开闭运算")
	{
		bRet = IsFuncRectOpenClose(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "圆形开闭运算" || strFunc == "!圆形开闭运算")
	{
		bRet = IsFuncCircleOpenClose(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "面积筛选" || strFunc == "!面积筛选")
	{
		bRet = IsFuncRegionSelected(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "面积排序" || strFunc == "!面积排序")
	{
		bRet = IsFuncRegionSort(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "区域处理")
	{
		bRet = IsFuncRegionProcess(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "区域最小距离" || strFunc == "!区域最小距离")
	{
		bRet = IsFuncRegionMinDist(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "轮廓提取" || strFunc == "!轮廓提取")
	{
		bRet = IsFuncGetCons(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "轮廓距离" || strFunc == "!轮廓距离")
	{
		bRet = IsFuncConsDist(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "模板定位" || strFunc == "!模板定位")
	{
		bRet = IsFuncFindTmp(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "圆边缘测量" || strFunc == "!圆边缘测量")
	{
		bRet = IsFuncMarginMsrCircle(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "直线边缘测量")
	{
		bRet = IsFuncMarginMsrLine(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "方形模板")
	{
		bRet = IsFuncLoadRCMod(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "方形区域")
	{
		bRet = IsFuncRCRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "角度形区域")
	{
		bRet = IsFuncDirRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "圆环区域")
	{
		bRet = IsFuncRingRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "圆形区域")
	{
		bRet = IsFuncCircleRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strType == "结果类区域")
	{
		bRet = IsFuncResultRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "拟合圆")
	{
		bRet = IsFuncFitCircle(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "拟合直线")
	{
		bRet = IsFuncFitLine(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "点处理")
	{
		bRet = IsFuncProcessPt(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "数据组合")
	{
		bRet = IsFuncProcessData(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "直线角度")
	{
		bRet = IsFuncLineAng(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "直线交点")
	{
		bRet = IsFuncLineXPoint(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "灰度方差")
	{
		bRet = IsFuncMeanDev(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "轮廓分割")
	{
		bRet = IsFuncSplitCons(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "点到轮廓距离")
	{
		bRet = IsFuncDistP2C(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "直线到轮廓距离")
	{
		bRet = IsFuncDistL2C(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "结果区域")
	{
		bRet = IsFuncRelRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "二维码")
	{
		bRet = IsFuncCode2D(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "写INI")
	{
		bRet = IsFuncWriteINI(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else
	{
		LogFile("没有输入指令对应的处理函数！");
		return false;
	}
	return bRet;
}

extern "C" __declspec(dllexport) bool IsImageOK(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut)
{
	//直接由载入的工作流程进行检测
	CSingleton* pSng = CSingleton::GetInstance();
	HObject hoImage[96];
	HTuple hvRes[96];
	CopyImage(HW.hoImage, &pSng->_hoImgOrg[nID]);
	//CString strMs;
	//strMs.Format("拍照时间:%d", GetTickCount());
	HW.WinTexts.clear();
	int nPosTxt[2] = { 10, 10 };
	CTreeCtrl tree;
	psOut.clear();
	pSng->_bRes = true;  //申明一个结果,默认OK
	for (int i = 0; i < (int)pSng->_XMLImg[nID].size(); i++)
	{
		bool bRet = RunFunc(HW, nID, i, hoImage, hvRes, pSng->_XMLImg[nID][i].strType, tree, NULL);
		if (pSng->_XMLImg[nID][i].nOTP != OUT_NULL)
		{
			CPos p;
			p.iPrNO = i;
			p.strNick.Format("%s", pSng->_XMLImg[nID][i].strType);
			int nNum = hvRes[i].TupleLength().I();
			for (int j = 0; j < nNum; j++)
			{
				CSignal sg;
				if (pSng->_XMLImg[nID][i].nOTP == OUT_STR)
				{
					sg.sVal.Format("%s", (CString)hvRes[i][j].S());
				}
				else
				{
					sg.sVal.Format("%.3f", hvRes[i][j].D());
				}
				p.sgOut.push_back(sg);
			}
			if (nNum == 0)
			{
				CSignal sg;
				sg.sVal.Format("0.00");
				p.sgOut.push_back(sg);
			}
			psOut.push_back(p);
		}
		if (bRet == false)
		{
			if (nID < 0 || bLogNG)
			{
				LogFile("第%d步检测结果NG！", i + 1);
			}
			if (nID >= 0)
			{
				nPosTxt[0] = 75;
				HW.ShowImage(nPosTxt, "NG", 60, "red", false);
			}
			return false;
		}
		if (bRet && i == pSng->_XMLImg[nID].size() - 1)
		{
			if (bLogNG)
			{
				LogFile("共计%d步,检测OK", i + 1);
			}			
			if (pSng->_bRes == false)
				return false;
			if (pSng->_XMLImg[nID][i].strType == "输出结果")
			{
				nPosTxt[0] = 75;
				HW.ShowImage(nPosTxt,"OK", 60,"green", false);
			}
		}
	}
	return true;
}

void IHpDllWin::ExpandTree(HTREEITEM hTreeItem, TiXmlElement* pRoot, bool bRoot)
{	
	CSingleton* pSng = CSingleton::GetInstance();
	CString strNodeText = m_tree.GetItemText(hTreeItem);
	CStringArray sts;
	strNodeText.TrimRight("=");
	int nFind = pSng->SplitString(strNodeText, "=", sts);
	CString strVal, strNode;
	strVal.Empty();
	strNode.Format("%s", strNodeText);
	if (sts.GetCount() > 1)
	{
		strNode.Format("%s", sts[0]);
		if (hTreeItem == m_hRoot)
			strVal.Empty();
		else
			strVal.Format("%s", sts[1]);
	}
	HTREEITEM hChildItem = m_tree.GetChildItem(hTreeItem);
	TiXmlElement* xml_child = new TiXmlElement(strNode);
	if (bRoot == false)
	{
		(pRoot)->LinkEndChild(xml_child);
		if (!hChildItem)
			xml_child->LinkEndChild(new TiXmlText(strVal));
	}
	else
	{
		pRoot->LinkEndChild(new TiXmlText(strVal));
	}
	while (hChildItem)
	{
		if (bRoot == false)
			ExpandTree(hChildItem, xml_child);
		else
			ExpandTree(hChildItem, pRoot);
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
	}
}

void IHpDllWin::OnBnClickedBtnSavXml()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strXMLName,strXMLDir;
	GetDlgItemText(IDC_EDIT_NAME, strXMLName);
	if (strXMLName.GetLength() < 1)
	{
		MessageBox("没有输入要保存的XML文件名称！", "", MB_ICONWARNING);
		return;
	}
	if (GetDlgItem(IDC_EDIT_NAME)->IsWindowEnabled())
	{
		if (m_bCali)
			strXMLDir.Format("D:\\CALI\\%s\\%s.xml", m_strCam, strXMLName);
		else
			strXMLDir.Format("D:\\XML\\%s\\%s.xml", m_strCam,strXMLName);
		BOOL bFind = PathFileExists(strXMLDir);
		if (bFind)
		{
			if (IDNO == MessageBox("是否覆盖原有文件" + strXMLName + " ?", "", MB_YESNO))
			{
				MessageBox(strXMLName + "作为流程树形文件名保存?", "", MB_ICONWARNING);
				return;
			}
		}
	}
	else
	{
		strXMLDir.Format("%s", m_strXML);
	}
	TiXmlDocument xml_doc;                                                                           // 添加XML声明
	xml_doc.LinkEndChild(new TiXmlDeclaration("1.0", "GBK", ""));              // 添加根元素
	CString strRoot = m_tree.GetItemText(m_hRoot);
	//int nTest = atoi(strRoot.Right(1));
	CStrs stsCam;
	pSng->SplitString(strRoot, "=", stsCam);
	strRoot.Format("%s", stsCam[0]);
	TiXmlElement * xml_root = new TiXmlElement(strRoot);

	CString strGain, strGam;
	strGain.Format("%.1f",m_fGain);
	strGam.Format("%.1f", m_fGam);
	xml_root->SetAttribute("EXP", (int)m_fExp);
	xml_root->SetAttribute("Gain", strGain);
	xml_root->SetAttribute("Gamma", strGam);
	xml_root->SetAttribute("Test", stsCam[1].Right(1));   //测试属性
	xml_doc.LinkEndChild(xml_root);

	ExpandTree(m_hRoot, xml_root, true);
	int nRet = xml_doc.SaveFile(strXMLDir);
	if (!nRet)
	{
		MessageBox(strXMLDir + "保存失败！", "", MB_ICONWARNING);
		return;
	}
	MessageBox(strXMLDir + "成功保存！", "", MB_OK);
	//delete xml_root;
}

void IHpDllWin::GetEleValue(TiXmlElement* root)
{
	for (TiXmlNode*  item = root->FirstChild("item");item;item = item->NextSibling("item"))
	{
		TiXmlNode* tmpele = item;
		if (item->ToElement()->GetText() != NULL)
		{
			//cout << currentele->Name() << ":" << currentele->GetText() << endl;
		}
		if (!tmpele->NoChildren())
			GetEleValue(tmpele->ToElement());
	}
}

void IHpDllWin::LoadXML(CString sPathName)
{
	TiXmlDocument doc(sPathName);
	bool loadOkay = doc.LoadFile();
	if (loadOkay == false)
	{
		MessageBox("无法打开指定的XML文件！", "读取失败", MB_ICONWARNING);
		return;
	}
	m_tree.SetRedraw(FALSE);
	m_tree.DeleteAllItems();
	m_tree.SetRedraw(TRUE);

	TiXmlElement* root = doc.RootElement();
	CString strText = root->Value();
	CString strExp = root->Attribute("EXP");
	CString strGain = root->Attribute("Gain");
	CString strGam = root->Attribute("Gamma");
	CString strTest = root->Attribute("Test");
	m_fExp = atof(strExp);
	m_fGain = atof(strGain);
	m_fGam = atof(strGam);
	CString strRoot;
	strRoot.Format("%s=Exp:%.1f;Gain:%.1f;Gamma:%.1f;Test:%s", strText, m_fExp, m_fGain, m_fGam,strTest);
	m_hRoot = m_tree.InsertItem(strRoot, 1, 0, TVI_ROOT);              //添加一级结点
	m_tree.Invalidate();
	TiXmlElement *pChild = root->FirstChildElement();
	while (pChild != NULL)
	{
		m_tree.Expand(m_hRoot, TVE_EXPAND);
		HTREEITEM hChild = m_tree.InsertItem(pChild->Value(), m_hRoot);                               //添加三级结点
		TiXmlElement *pleaf = pChild->FirstChildElement();
		while (pleaf != NULL)
		{
			CString strName = pleaf->Value();
			CString strValue = pleaf->GetText();
			HTREEITEM hLeaf = m_tree.InsertItem(strName + "=" + strValue, hChild);                //添加三级结点
			pleaf = pleaf->NextSiblingElement();
		}
		m_tree.Expand(hChild, TVE_EXPAND);
		pChild = pChild->NextSiblingElement();
	}
	m_tree.Expand(m_hRoot, TVE_EXPAND);
	m_tree.Invalidate();
}

void IHpDllWin::OnBnClickedBtnLoadXml()
{
	if (GetDlgItem(IDC_EDIT_NAME)->IsWindowEnabled())
	{
		LPCTSTR lpszFilter = "XML (*.xml)|*.xml|XML (*.XML)|*.XML||";
		CFileDialog fDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter);
		CString path = "D:\\XML\\" + m_strCam;
		if (m_bCali)
			path = "D:\\CALI\\" + m_strCam;
		fDlg.m_ofn.lpstrInitialDir = (LPCTSTR)path;
		if (fDlg.DoModal() == IDCANCEL)
			return;
		CString sPathName = fDlg.GetPathName();
		LoadXML(sPathName);
		CString strExt = fDlg.GetFileExt();
		CString strNick = fDlg.GetFileName().TrimRight("." + strExt);
		SetDlgItemText(IDC_EDIT_NAME, strNick);
	}
	else
	{
		LoadXML(m_strXML);
	}
}

void IHpDllWin::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//添加右键处理函数
	CMenu menu;
	CPoint pt;
	//pt = GetCurrentMessage()->pt;   //获取当前鼠标点击消息的坐标点
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	UINT uFlags = 0;
	HTREEITEM hItem = m_tree.HitTest(pt, &uFlags);
	if (hItem == m_hRoot)
	{
		//根节点直接设置Gain,Gamma,Exposure 参数
		menu.LoadMenu(IDR_MENU2);
		CMenu *psubmenu;
		psubmenu = menu.GetSubMenu(0);
		m_tree.ClientToScreen(&pt);
		psubmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
		*pResult = 0;
		return;
	}
	menu.LoadMenu(IDR_MENU1);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);
		bool bFind = false;                   //点击的结点不能是叶节点
		while (hChildItem)
		{
			if (hItem == hChildItem)
			{
				bFind = true;			
				break;
			}
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
		}
		if (bFind)
		{
			m_tree.SelectItem(hItem);
			//根据不同类型的节点弹出菜单
			CMenu *psubmenu;
			m_tree.ClientToScreen(&pt);
			CString strItem = m_tree.GetItemText(hChildItem);
			psubmenu = menu.GetSubMenu(0);
			if (strItem.Right(4) != "模板")
			{
				psubmenu->EnableMenuItem(ID_MENU_BF, MF_GRAYED);
			}
			psubmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*pResult = 0;
}

void IHpDllWin::OnMenuCam()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgCom dlg;
	CPos ps;
	ps.strNick = "曝光";
	ps.x = m_fExp;
	dlg.m_comboVals.push_back(ps);
	ps.strNick = "增益";
	ps.x = m_fGain;
	dlg.m_comboVals.push_back(ps);
	ps.strNick = "伽马";
	ps.x = m_fGam;
	dlg.m_comboVals.push_back(ps);
	if (dlg.DoModal() == IDOK)
	{
		m_fExp = dlg.m_comboVals[0].x;
		m_fGain = dlg.m_comboVals[1].x;
		m_fGam = dlg.m_comboVals[2].x;
		CString strRoot = m_tree.GetItemText(m_hRoot);
		CStringArray stsCam;
		pSng->SplitString(strRoot, "=", stsCam);
		strRoot.Format("%s=Exp:%.1f;Gain:%.1f;Gamma:%.1f;Test:0", stsCam[0], m_fExp, m_fGain, m_fGam);
		m_tree.SetItemText(m_hRoot, strRoot);
	}
}

void IHpDllWin::UpdateIndex(HTREEITEM& hItem, bool bAdd)
{
	//逐个遍历，iInx 以后的节点
	CSingleton* pSng = CSingleton::GetInstance();
	CString strRoot = m_tree.GetItemText(m_hRoot);
	strRoot.TrimRight("0");
	strRoot.TrimRight("1");
	CStringArray stsCam;
	pSng->SplitString(strRoot, "=", stsCam);
	strRoot.Format("%s", stsCam[0]);
	strRoot.Format("%s=%s0", stsCam[0], stsCam[1]); //设置Test:0
	m_tree.SetItemText(m_hRoot, strRoot);
	//////////////////////接下来逐个遍历每个节点///////////////////////////////////
	//GetCursorPos(&pt);
	//m_tree.ScreenToClient(&pt);        //将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	//UINT uFlags = 0;
	//HTREEITEM hItem = m_tree.HitTest(pt, &uFlags);
	HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);
	bool bUpdate = false;
	int nStep = -1,nNode;
	while (hChildItem)
	{			
		bool bDel = false;
		nStep++;
		if (hItem == hChildItem)
		{
			nNode = nStep;
			bUpdate = true;
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
			continue;
		}
		if (bUpdate)
		{
			//检索旗下的每一个节点，如果该节点的首字母带N那么改变节点索引
			CString strSec = m_tree.GetItemText(hChildItem);
			HTREEITEM hSon = m_tree.GetChildItem(hChildItem);
			while (hSon)
			{
				CString strNodeText = m_tree.GetItemText(hSon),strNewText;
				CStrs ssNode,ssKnot,ssRes;
				if (strNodeText.Left(1) == "N" || strNodeText.Right(4) == "节点")
				{
					pSng->SplitString(strNodeText, "=", ssNode);
					pSng->SplitString(ssNode[1], ",", ssKnot);
					int nNode = (int)ssKnot.size();
					for (int i = 0; i < nNode; i++)
					{
						int iKnot = atoi(ssKnot[i]) + 1;
						if (bAdd == true)   //插入一个节点反而需要 -1
							iKnot -= 1;
						if (iKnot >= 0)
						{
							//整个节点都需要删除
							if (IDYES == MessageBox(strSec + strNodeText + " 索引变成正数是否需要删除这个节点！","索引错误",MB_YESNO))
							{
								bDel = true;
							}							
							break;
						}
						////////////////////////修改节点内容//////////////////////
						CString strNew;
						strNew.Format("%d",iKnot);
						ssRes.push_back(strNew);
					}
					for (int i = 0; i < nNode;i++)
					{
						strNewText += ssRes[i] + ",";
					}
					strNewText.TrimRight(",");
					m_tree.SetItemText(hSon, strNewText);
					if (bDel == true)
						break;
				}
				hSon = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
			}
		}
		if (bDel)
		{
			HTREEITEM hNext = m_tree.GetNextSiblingItem(hChildItem);
			m_tree.DeleteItem(hChildItem);
			hChildItem = hNext;
			continue;
		}
		hChildItem = m_tree.GetNextSiblingItem(hChildItem); //获取根节点的兄弟节点
	}
}

void IHpDllWin::OnMenuDel()
{
	HTREEITEM hItem = m_tree.GetSelectedItem();
	CSingleton* pSng = CSingleton::GetInstance();
	CString str = m_tree.GetItemText(hItem);
	if (IDNO == MessageBox("确认要删除节点 ? " + str,"",MB_YESNO))
	{
		return;
	}
    HTREEITEM	hNext = m_tree.GetNextSiblingItem(hItem);
	int nStep = -1;
	HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);
	while (hChildItem)
	{
		nStep++;
		if (hItem == hChildItem)
			break;
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
	}
	m_tree.DeleteItem(hItem);
	UpdateTest();
	////////////////////////////接下来扫描每一个节点，如果后续有节点指向当前节点，就把该节点的文字用红色表示//////////////////////
	if (hNext == NULL)
	{
		//后续没有节点直接OK
		return;
	}
	while (hNext)
	{
		//逐个扫描子节点，判断是否节点名称是否N开头
		HTREEITEM hChild = m_tree.GetChildItem(hNext);
		while (hChild)
		{
			CString strName = m_tree.GetItemText(hChild);
			if (strName.Left(1) == "N")
			{
				CStrs ss;
				pSng->SplitString(str, "=", ss);
				CPos ps;
				ps.strNick = ss[0];
				ps.strMN = ss[1];
				int iKnot = atoi(ps.strMN);       //节点指向
				hChildItem = m_tree.GetChildItem(m_hRoot);
				int iCurr = -1;
				while (hChildItem)
				{
					iCurr++;
					if (hItem == hChild)
						break;
					hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
				}
				if (iKnot + iCurr )
				{
				}
			}
			hChild = m_tree.GetNextSiblingItem(hChild);
		}
		hNext = m_tree.GetNextSiblingItem(hItem);
	}
}

void IHpDllWin::OnMenuRun()
{
	HTREEITEM hItem = m_tree.GetSelectedItem();
	HTREEITEM hChild = m_tree.GetChildItem(m_hRoot);
	int n = 0;
	while (hChild)
	{
		if (hItem == hChild)
			break;
		hChild = m_tree.GetNextSiblingItem(hChild);
		n++;
	}
	GotoNodeSel(n);
}

void IHpDllWin::OnMenuMod()
{		
	CSingleton* pSng = CSingleton::GetInstance();
	HTREEITEM hItem = m_tree.GetSelectedItem();
	HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);
	bool bFind = false;                   //点击的结点不能是叶节点
	while (hChildItem)
	{
		if (hItem == hChildItem)
		{
			bFind = true;
			break;
		}
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //获取根节点的兄弟节点
	}
	if (bFind == false)
		return;
	CDlgItem dlg;
	CString strNodeNick = m_tree.GetItemText(hItem);
	hChildItem = m_tree.GetChildItem(hItem);
	while (hChildItem)
	{
		CString str = m_tree.GetItemText(hChildItem);
		CStrs ss;
		pSng->SplitString(str,"=",ss);
		CPos ps;
		ps.strNick = ss[0];
		ps.strMN = ss[1];
		dlg.psLeaf.push_back(ps);
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);
	}
	dlg.m_strNick.Format("%s",strNodeNick);
	CListStyle sy;
	sy.Set("节点名称", 220);
	dlg.sts.push_back(sy);
	sy.Set("节点值", 320);
	dlg.sts.push_back(sy);
	if (dlg.DoModal() == IDOK)
	{
		//改变了psLeaf值写到节点里买来
		hChildItem = m_tree.GetChildItem(hItem);
		for (int i = 0; i < (int)dlg.psLeaf.size();i++)
		{
			CString strItem,strName,strVal;
			strName = dlg.psLeaf[i].strNick;
			strVal = dlg.psLeaf[i].strMN;
			strItem.Format("%s=%s",strName,strVal);
			m_tree.SetItemText(hChildItem,strItem);
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);
		}
		UpdateTest(0);
	}
}

void IHpDllWin::UpdateTest(int nOK)
{
	//修改完成要改变test属性
	CString strRoot = m_tree.GetItemText(m_hRoot);
	strRoot.TrimRight("1");
	strRoot.TrimRight("0");
	if (nOK)
		m_tree.SetItemText(m_hRoot, strRoot + "1");
	else
		m_tree.SetItemText(m_hRoot, strRoot + "0");
}

void IHpDllWin::OnMenuBf()
{	
	CSingleton* pSng = CSingleton::GetInstance();
	HTREEITEM hItem = m_tree.GetSelectedItem();
	HTREEITEM hChildItem = m_tree.GetChildItem(hItem);		
	CStrs ss, stsNCC,stsDelta;
	CString strNick, strOffset;
	int iNode = 0;
	while (hChildItem)
	{
		CString strItem = m_tree.GetItemText(hChildItem);
		pSng->SplitString(strItem, "=", ss);
		ss[0].TrimRight();
		if (ss[0] == "模板" || ss[0] == "模板名")
		{
			pSng->SplitString(ss[1], ";",stsNCC);
			if (stsNCC.size() == MAX_TM)
			{
				MessageBox("已经有足够多的NCC模板！","无法添加NCC模板",MB_ICONWARNING);
				return;
			}
			//弹出模板对话框
			HTuple hvPos[4], hvNCC, hvW, hvH;
			int nPosTxt[2] = { 0 };
			m_HW.ShowImage(nPosTxt, "新增图像模板区域");
			m_HW.WinTexts.clear();
			DrawRectangle1(m_HW.WndID, &hvPos[0], &hvPos[1], &hvPos[2], &hvPos[3]);
			HObject hoRC, hoCrop, hoPart, hoCurr, hoZoom;
			CopyImage(m_HW.hoImage, &hoCurr);
			GetImageSize(m_HW.hoImage, &hvW, &hvH);
			ZoomImageSize(hoCurr, &hoZoom, ZOOM*hvW, ZOOM*hvH, "constant");
			GenRectangle1(&hoRC, ZOOM*hvPos[0], ZOOM*hvPos[1], ZOOM*hvPos[2], ZOOM*hvPos[3]);
			ReduceDomain(hoZoom, hoRC, &hoPart);
			CropDomain(hoPart, &hoCrop);
			CDlgTmp dlg;
			CopyImage(hoCrop, &dlg.hoTMP);
			m_HW.ShowImage();
			if (dlg.DoModal() != IDOK)
			{
				return;
			}
			if (dlg.strTmpName.GetLength() == 0)
				return;
			CString strTmpName;
			strTmpName.Format("D:\\TMP\\%s_%d", dlg.strTmpName, dlg.m_nTmpTYP);
			//清空这个节点原有的路径,以便停在该节点处
			stsNCC.push_back(strTmpName);
			strNick = pSng->LinkString(stsNCC, ";");
			m_tree.SetItemText(hChildItem, "模板=" + strNick);
			//第 2 个节点就是中心偏差	
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);       //获取根节点的兄弟节点
			pSng->SplitString(strItem, "=", stsDelta);
			stsDelta.push_back(dlg.strOffset);
			strOffset = pSng->LinkString(stsDelta, ";");
			m_tree.SetItemText(hChildItem, "中心偏差=" + strOffset);
			break;
		}
	}
	///////////////////////////////////接下来运行到该节点启动模板检测////////////////
	//bool bOK = GotoNodeSel(iNode);
	//if (bOK == false)
	//{
	//	MessageBox("NCC模板添加失败！", "",MB_ICONWARNING);
	//	return;
	//}
	//CString strNCC = m_tree.GetItemText(hChildItem);
	//pSng->SplitString(strNCC, "=", ss);
	//if (ss.GetCount() != 2)
	//{
	//	MessageBox("模板节点格式错误！","",MB_ICONWARNING);
	//	return;
	//}
	//CString strNodeVal("模板=");
	//for (int i = 0; i < stsNCC.GetCount();i++)
	//{
	//	strNodeVal += stsNCC[i] + ";";
	//}
	//strNodeVal += ss[1];
	//m_tree.SetItemText(hChildItem, strNodeVal);
}

void IHpDllWin::OnMenuAddRc()
{
	HTREEITEM hItem = m_tree.GetSelectedItem();
	CString str = m_tree.GetItemText(hItem);
	CSingleton* pSng = CSingleton::GetInstance();
	if (str == "方形区域")
	{
		HTuple hvPos[4];
		int nPosTxt[2] = { 0, 0 };
		m_HW.ShowImage(nPosTxt,"框选方形区域");
		CString strVal;
		DrawRectangle1(m_HW.WndID, &hvPos[0], &hvPos[1], &hvPos[2], &hvPos[3]);
		HTREEITEM hLeafItem = m_tree.GetChildItem(hItem);
		for (int i = 0; i < 4; i++)
		{
			strVal.Format("%.1f", hvPos[i].D());
			CString strKnot = m_tree.GetItemText(hLeafItem);
			CStringArray sts;
			pSng->SplitString(strKnot, "=", sts);
			sts[1].TrimRight();                                    //剔除空格
			if (sts[1].GetLength() > 0)
			{
				m_tree.SetItemText(hLeafItem, strKnot + "," + strVal);
			}
			else
			{
				m_tree.SetItemText(hLeafItem, sts[0] + "=" + strVal);
			}
			hLeafItem = m_tree.GetNextSiblingItem(hLeafItem);
		}
	}
}

void IHpDllWin::OnMenuIni()
{
	CSingleton* pSng = CSingleton::GetInstance();
	HTREEITEM hNode = m_tree.GetSelectedItem();
	CString strNodeText = m_tree.GetItemText(hNode);
	m_tree.Expand(m_hRoot, TVE_EXPAND);
	CStringArray sKeys;
	pSng->GetSecKeys(strNodeText, sKeys);
	HTREEITEM hChild = m_tree.GetChildItem(hNode);
	bool bNumOK = false;
	for (int i = 0; i < sKeys.GetCount();i++)
	{
		if (hChild == NULL)
		{
			MessageBox(sKeys[i] + " 节点数量有误！","",MB_ICONWARNING);
			return;
		}
		CString sLeaf = pSng->GetCfgString(strNodeText, sKeys[i], "");
		CStrs ss;
		pSng->SplitString(sLeaf, ";", ss);
		CString strVal = sKeys[i] + "=" + ss[0];
		m_tree.SetItemText(hChild, strVal);
		hChild = m_tree.GetNextSiblingItem(hChild);
		if (i == sKeys.GetCount() - 1 && hChild == NULL)
			bNumOK = true;
	}
	if (bNumOK == false)
	{
		MessageBox(strNodeText  + " 节点数量有误！", "", MB_ICONWARNING);
		return;
	}
}

void IHpDllWin::OnMenuFirst()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CPoint pt;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	UINT uFlags = 0;
	HTREEITEM hItem = m_tree.HitTest(pt, &uFlags);
	HTREEITEM hMidItem, hFirstItem;
	if (hItem == m_hRoot)
		return;
	hMidItem = m_tree.GetSelectedItem();
	HTREEITEM hTempItem = m_tree.GetChildItem(m_hRoot);
	bool bFind = false;
	while (hTempItem)
	{
		if (hMidItem == hTempItem)
		{
			bFind = true;
			break;
		}
		hTempItem = m_tree.GetNextSiblingItem(hTempItem);
	}
	if (bFind == false)
		return;
	hFirstItem = m_tree.GetChildItem(m_hRoot);	
	if (hMidItem != hFirstItem)
		return;
	CString str;
	m_comboFunc.GetLBText(m_comboFunc.GetCurSel(), str);
	CString strFirst = m_tree.GetItemText(hFirstItem);
	HTREEITEM hChild = m_tree.GetChildItem(hFirstItem);
	CStrs stsItem;
	while (hChild)  //逐个删除第一个节点下的内容，不删除Sec，只删除叶节点，删除前把每个节点的内容保存起来
	{
		stsItem.push_back(m_tree.GetItemText(hChild));
		HTREEITEM hCurr = hChild;
		hChild = m_tree.GetNextSiblingItem(hChild);
		m_tree.DeleteItem(hCurr);
	}
	m_tree.SetItemText(hFirstItem, str);          //修改第一个节点文字
	InsertNode(str, hFirstItem, false);               //不插入新节点
	InsertNode(strFirst, stsItem, hFirstItem);   //再把原来的第一个节点的内容
	UpdateTest(0);
}

void IHpDllWin::OnMenuInsert()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CPoint pt;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	UINT uFlags = 0;
	HTREEITEM hItem = m_tree.HitTest(pt, &uFlags);
	HTREEITEM hMidItem;
	if (hItem == m_hRoot)
		return;
	hMidItem = m_tree.GetSelectedItem();
	HTREEITEM hTempItem = m_tree.GetChildItem(m_hRoot);
	bool bFind = false;
	while (hTempItem)
	{
		if (hMidItem == hTempItem)
		{
			bFind = true;
			break;
		}
		hTempItem = m_tree.GetNextSiblingItem(hTempItem);
	}
	if (bFind == false)
		return;
	CString str;
	m_comboFunc.GetLBText(m_comboFunc.GetCurSel(), str);
	InsertNode(str, hMidItem);
	UpdateTest();
}

void IHpDllWin::InsertNode(CString strText,CStrs& stsItem, HTREEITEM hItemSel)
{
	HTREEITEM hChild = m_tree.InsertItem(strText, m_hRoot, hItemSel);
	CString strRoot = m_tree.GetItemText(m_hRoot);
	strRoot.TrimRight("0");
	strRoot.TrimRight("1");
	m_tree.SetItemText(m_hRoot,strRoot + "0");
	m_tree.Expand(m_hRoot, TVE_EXPAND);
	for (int i = 0; i < (int)stsItem.size(); i++)
	{
		m_tree.InsertItem(stsItem[i], hChild);
	}
	m_tree.Expand(hChild, TVE_EXPAND);
}

void IHpDllWin::InsertNode(CString strText, HTREEITEM hItemSel,bool bInsert)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strRoot = m_tree.GetItemText(m_hRoot);
	strRoot.TrimRight("0");
	strRoot.TrimRight("1");
	m_tree.SetItemText(m_hRoot, strRoot + "0");
	HTREEITEM hChild = hItemSel;
	if (bInsert)
	{
		hChild = m_tree.InsertItem(strText, m_hRoot, hItemSel);
		m_tree.Expand(m_hRoot, TVE_EXPAND);
	}
	CStringArray sKeys;
	pSng->GetSecKeys(strText, sKeys);
	for (int i = 0; i < sKeys.GetCount(); i++)
	{
		CString strVal = pSng->GetCfgString(strText, sKeys[i], NULL);
		CStringArray strs;
		pSng->SplitString(strVal, ";", strs);
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
}

void IHpDllWin::OnBnClickedBtnDirXml()
{
	CString strXMLDir;
	if (m_bCali)
		strXMLDir.Format("D:\\CALI\\%s", m_strCam);
	else
		strXMLDir.Format("D:\\XML\\%s", m_strCam);
	ShellExecute(NULL, _T("open"), strXMLDir, NULL, NULL, SW_SHOWNORMAL);
}

void IHpDllWin::OnMenuDef()
{
	//按照缺省相机参数配置
	CSingleton* pSng = CSingleton::GetInstance();
	m_fGam = atof(pSng->GetCfgString("伽马", m_strCam, "1.0"));
	m_fGain = atof(pSng->GetCfgString("增益", m_strCam, "1.0"));
	m_fExp = atof(pSng->GetCfgString("曝光", m_strCam, "5000"));
	CString strRoot;
	strRoot.Format("%s=Exp:%.1f;Gain:%.1f;Gamma:%.1f;Test:0", m_strCam, m_fExp, m_fGain, m_fGam);
	m_tree.SetItemText(m_hRoot, strRoot);
}

void IHpDllWin::OnBnClickedBtnSnap()
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (m_HW.bCam == false)
	{
		OnBnClickedBtnOrg();
		return;
	}
	if (pSng->_bINICam == false)
		return;
	/////相机初始化成功////////////////////////
	int nCam = atoi(m_strCam.Right(1)) - 1;
	int nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "ExposureTime", (float)m_fExp);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gain", (float)m_fGain);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gamma", (float)m_fGam);
	pSng->CamSnap(m_HW, nCam);
	OnBnClickedBtnOrg();
}

void IHpDllWin::OnNMCustomdrawTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	*pResult = 0;
}

BOOL IHpDllWin::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_HW.rcWin.PtInRect(pt) && m_bINI)
	{
		double  ImagePtX, ImagePtY,Row0, Col0, Row1, Col1;
		double Scale = 0.1;
		if (zDelta > 0)
		{
			ImagePtX = m_HW.psLU.x + (pt.x - m_HW.rcWin.left) / (m_HW.rcWin.Width() - 1.0)*(m_HW.psRD.x - m_HW.psLU.x);
			ImagePtY = m_HW.psLU.y + (pt.y - m_HW.rcWin.top) / (m_HW.rcWin.Height() - 1.0)*(m_HW.psRD.y - m_HW.psLU.y);
			Row0 = ImagePtY - 1 / (1 - Scale)*(ImagePtY - m_HW.psLU.y);
			Row1 = ImagePtY - 1 / (1 - Scale)*(ImagePtY - m_HW.psRD.y);
			Col0 = ImagePtX - 1 / (1 - Scale)*(ImagePtX - m_HW.psLU.x);
			Col1 = ImagePtX - 1 / (1 - Scale)*(ImagePtX - m_HW.psRD.x);
		}
		else
		{
			ImagePtX = m_HW.psLU.x + (pt.x - m_HW.rcWin.left) / (m_HW.rcWin.Width() - 1.0)*(m_HW.psRD.x - m_HW.psLU.x);
			ImagePtY = m_HW.psLU.y + (pt.y - m_HW.rcWin.top) / (m_HW.rcWin.Height() - 1.0)*(m_HW.psRD.y - m_HW.psLU.y);
			Row0 = ImagePtY - 1 / (1 + Scale)*(ImagePtY - m_HW.psLU.y);
			Row1 = ImagePtY - 1 / (1 + Scale)*(ImagePtY - m_HW.psRD.y);
			Col0 = ImagePtX - 1 / (1 + Scale)*(ImagePtX - m_HW.psLU.x);
			Col1 = ImagePtX - 1 / (1 + Scale)*(ImagePtX - m_HW.psRD.x);
		}
		m_HW.psLU.y = Row0;
		m_HW.psLU.x = Col0;
		m_HW.psRD.y = Row1;
		m_HW.psRD.x = Col1;
		m_HW.ShowImage();
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void IHpDllWin::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIM_INI)
	{
		m_HW.RealSize("yellow",true);
		m_bINI = true;
		KillTimer(TIM_INI);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void IHpDllWin::OnBnClickedCheckBd()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_BD);
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		m_bCali = true;
	}
	else
		m_bCali = false;
}
