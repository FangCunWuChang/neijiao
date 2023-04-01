//#include "IHpDllWin.h"
// IHpDllWin.cpp : ʵ���ļ�
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

// IHpDllWin �Ի���

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

// IHpDllWin ��Ϣ�������
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
		LogFile("���%d �ɼ�ͼ��ʧ�ܣ�",nCam + 1);
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
		MessageBox(NULL,"�������˿ڱ�ռ�ã�", "", MB_ICONERROR);
		return FALSE;
	}
	if (pSng->_pSvr->StartServer(NULL, NULL, NULL) == FALSE)
	{
		MessageBox(NULL,"����������ʧ�ܣ�", "", MB_ICONERROR);
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

/******************û�и������*******************************/
/*                0----��Ʒ��׼λ�ã�1-- ��Ʒ��λ�ã�
                   0��x��1��Y��2��Phi
/************************************************************************/
extern "C" __declspec(dllexport) int GetOffset(int nInx,double fPosImg[2][3],double fDelta[3])
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_psCali[nInx].bLoad == false)
	{
		LogFile("Cam %d û��ӳ�����",nInx + 1);
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
	pSng->NineDotCalib(ImgPts, AxisPts, hvMX);                                                                         //��ת������ӳ�����
	double fPosST[2],fPosNew[2];
	//AffineTransPoint2d(HomMat2DRot, fPosImg[0][1], fPosImg[0][0], &hvYST, &hvXST);      //��Ʒ����ı�׼λ��
	AffineTransPoint2d(hvMX, fPosImg[0][1], fPosImg[0][0], &hvTransY, &hvTransX);
	fPosST[0] = hvTransX.D();
	fPosST[1] = hvTransY.D();
	//AffineTransPoint2d(HomMat2DRot, fPosImg[1][1], fPosImg[1][0], &hvYST, &hvXST);      //��Ʒ�������λ��
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
		strLog.Format("���%d �ع��������ʧ�ܣ�", nCam + 1,nRet);
		LogFile(strLog);
		return false;
	}
	pSng->_csCam[nCam].Lock();
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gain", fGain);
	pSng->_csCam[nCam].Unlock();
	if (MV_OK != nRet)
	{
		strLog.Format("���%d �����������ʧ�ܣ�", nCam + 1, nRet);
		LogFile(strLog);
		return false;
	}
	pSng->_csCam[nCam].Lock();
	nRet = MV_CC_SetFloatValue(pSng->_hCam[nCam], "Gamma", fGam);
	pSng->_csCam[nCam].Unlock();
	if (MV_OK != nRet)
	{
		strLog.Format("���%d ٤���������ʧ�ܣ�", nCam + 1, nRet);
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
		//pSng->GetSecKeys("����",stsSN);
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
		//		CString strNO = pSng->GetCfgString("����",stsSN[i],"");
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
	// ch:ѡ���豸��������� | Select device and create handle
	nRet = MV_CC_CreateHandle(&pSng->_hCam[iNum], stDeviceList.pDeviceInfo[II]);
	if (MV_OK != nRet)
	{
		strLog.Format("Create Handle fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:���豸 | Open device
	nRet = MV_CC_OpenDevice(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Open Device fail! nRet [0x%x],���豸ʧ��", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
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
	// ch:���ô���ģʽΪoff | eb:Set trigger mode as off
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
		LogFile("���%d�ع�ģʽ�����쳣", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
	if (MV_OK != nRet)
	{
		LogFile("���%d �ر��Զ��ع�ģʽ�쳣����", iNum + 1);
		return false;
	}
	int nExp = pSng->GetCfgInt("�ع�", strCamName, 9000);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "ExposureTime", (float)nExp);
	if (MV_OK != nRet)
	{
		LogFile("���%d �����ع�ֵ�쳣����", iNum + 1);
		return false;
	}
	int nGain = pSng->GetCfgInt("����", strCamName, 6);
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "GainAuto", 0);
	if (MV_OK != nRet)
	{
		LogFile("���%d �ر��Զ������쳣����", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "Gain", (float)nGain);
	if (MV_OK != nRet)
	{
		LogFile("���%d ��������ֵ�쳣����", iNum + 1);
		return false;
	}
	double fGama = atof(pSng->GetCfgString("٤��", strCamName, "1.0"));
	nRet = MV_CC_SetBoolValue(pSng->_hCam[iNum], "GammaEnable", true);
	nRet = MV_CC_SetEnumValue(pSng->_hCam[iNum], "GammaSelector", 1);
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "Gamma", (float)fGama);
	if (MV_OK != nRet)
	{
		LogFile("���%d ٤�������쳣����", iNum + 1);
		return false;
	}
	nRet = MV_CC_SetFloatValue(pSng->_hCam[iNum], "AcquisitionFrameRate", 5);
	if (nRet != MV_OK)
	{
		LogFile("���%d �ɼ�֡��Ϊ5֡ʧ�ܣ���", iNum + 1);
		return false;
	}
	//֡�ʿ���ʹ�ܣ�true��ʾ�򿪣�false��ʶ�ر�
	nRet = MV_CC_SetBoolValue(pSng->_hCam[iNum], "AcquisitionFrameRateEnable", true);
	if (nRet != MV_OK)
	{
		LogFile("���%d �ɼ�֡�����ÿ���ʧ�ܣ���", iNum + 1);
		return false;
	}
	// ch:ע��ץͼ�ص� | en:Register image callback
	//nRet = MV_CC_RegisterImageCallBackEx(m_Camhandle[iNum], ImageCallBackEx, m_Camhandle[iNum]);
	pSng->_iCam[iNum] = iNum;
	nRet = MV_CC_RegisterImageCallBackEx(pSng->_hCam[iNum], ImageCallBack,&pSng->_iCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Register Image CallBack fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:��ʼȡ�� | en:Start grab image
	nRet = MV_CC_StartGrabbing(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Start Grabbing fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	strLog.Format("CAM---%d ��ʼ��OK!", iNum + 1);
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
	// ch:ע��ץͼ�ص� | en:Unregister image callback
	nRet = MV_CC_RegisterImageCallBackEx(pSng->_hCam[iNum], NULL, NULL);
	if (MV_OK != nRet)
	{
		strLog.Format("Unregister Image CallBack fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:�ر��豸 | en:Close device
	nRet = MV_CC_CloseDevice(pSng->_hCam[iNum]);
	if (MV_OK != nRet)
	{
		strLog.Format("Close Device fail! nRet [0x%x]", nRet);
		LogFile(strLog);
		return false;
	}
	// ch:���پ�� | en:Destroy handle
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
	LogFile("CAM%d �ر�OK!", iNum + 1);
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

//ÿ��������6�й�������
extern "C" __declspec(dllexport) bool LoadImageXML(int nID,CString strXML,int& nExp,float& fGain,float& fGam)
{
	CSingleton* pSng = CSingleton::GetInstance();
	TiXmlDocument doc(strXML);
	bool loadOkay = doc.LoadFile();
	if (loadOkay == false)
	{
		MessageBox(NULL,strXML + " �ļ���ʽ����!", "��ȡʧ��", MB_ICONWARNING);
		return false;
	}
	pSng->_XMLImg[nID].clear();
	TiXmlElement* root = doc.RootElement();
	CString strText = root->Value();                                                    //���ڵ�ֻ������
	CString strExp = root->Attribute("EXP");
	CString strGain = root->Attribute("Gain");
	CString strGam = root->Attribute("Gamma");
	CString strTest = root->Attribute("Test");
	if (strTest != "1")
	{
		MessageBox(NULL, strXML + "�ļ�û�в��Գɹ��޷����룡", "��ȡʧ��", MB_ICONWARNING);
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
		NodeImg.strType = pChild->Value();                               //�ڵ�����Sec
		double fMinScore = 0.6;
		while (pleaf != NULL)
		{
			CString strName = pleaf->Value();
			CString strValue = pleaf->GetText();
			if (strName == "��ͷ�")
			{
				fMinScore = atof(strValue);
			}
			CPos sLf;
			sLf.strNick.Format("%s",strName);                                                      //�ڵ����� Key
			sLf.strMN.Format("%s",strValue);                                                        //����ڵ��ֵ
			NodeImg.psLeaf.push_back(sLf);                                                      //���е�Ҷ�ڵ�
			if (strName == "ģ��" || strName == "ģ����")                                    //��Unicode �����ַ�һ������
			{
				CStringArray ssTM,ssXY;
				pSng->SplitString(strValue, ";", ssTM);
				TiXmlElement *pOffset = pleaf->NextSiblingElement();
				CString strOffset = pOffset->GetText();                                        //ģ������ƫ��ֵ
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
					pSng->SplitString(ssXY[i], ",", ssVal);                                    //�Ƿ����������
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
			if (strName == "���" && strValue == "��")    //ֻҪ����ڵ���������ܾ����
			{
				NodeImg.nOTP = OUT_VAL;                  //�����ֵ����CString�����
			}
			else if (strValue == "����ַ�" && strValue == "��")
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
	m_hRoot = m_tree.InsertItem(strRoot, 1, 0, TVI_ROOT);              //���һ�����
	m_tree.SetTextColor(RGB(0, 0, 255));                                               //�����ı���ɫ

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
	LogFile("ͼ����Ի����ʼ���ɹ���");
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
	TRACE("\n ��̬��ͼ�񴰿��˳���ϣ�");
}


void IHpDllWin::OnBnClickedBtnAddFunc()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	m_comboFunc.GetLBText(m_comboFunc.GetCurSel(), str);
	HTREEITEM hChild = m_tree.InsertItem(str, m_hRoot);                                 //����������
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
			m_tree.InsertItem(sKeys[i] + "=" + strs[0], hChild);   //Ĭ�ϵ�һ������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void IHpDllWin::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CString strText;
	m_tree.GetEditControl()->GetWindowText(strText);
	m_tree.SetItemText(m_tree.GetSelectedItem(), strText);//���ñ༭����ı�Ϊ���
	*pResult = 0;
}


void IHpDllWin::OnBnClickedRadio1()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("��ֵ��");
	m_comboFunc.AddString("�Ҷȷ���");
	m_comboFunc.AddString("Բ�ο�������");
	m_comboFunc.AddString("���ο�������");
	m_comboFunc.AddString("���ɸѡ");
	m_comboFunc.AddString("�������");
	m_comboFunc.AddString("������ȡ");
	m_comboFunc.AddString("��������");
	m_comboFunc.AddString("������");
	m_comboFunc.AddString("�����ָ�");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio2()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("����ģ��");
	m_comboFunc.AddString("Բ��ģ��");
	m_comboFunc.AddString("�Ƕ�ģ��");
	m_comboFunc.AddString("��ά��");
	m_comboFunc.AddString("ģ�嶨λ");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio3()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("���Բ");
	m_comboFunc.AddString("���ֱ��");
	m_comboFunc.AddString("Բ��Ե����");
	m_comboFunc.AddString("ֱ�߱�Ե����");
	m_comboFunc.AddString("�㴦��");
	m_comboFunc.AddString("�������");
	m_comboFunc.AddString("ֱ�߽Ƕ�");
	m_comboFunc.AddString("ֱ�߽���");
	m_comboFunc.AddString("�㵽��������");
	m_comboFunc.AddString("ֱ�ߵ���������");
	m_comboFunc.AddString("������С����");
	m_comboFunc.SetCurSel(0);
}

void IHpDllWin::OnBnClickedRadio4()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("��������");
	m_comboFunc.AddString("�Ƕ�������");
	m_comboFunc.AddString("Բ������");
	m_comboFunc.AddString("��������");
	m_comboFunc.AddString("Բ������");
	m_comboFunc.AddString("���λ�����");
	m_comboFunc.AddString("���������");
	m_comboFunc.SetCurSel(0);
}


void IHpDllWin::OnBnClickedRadio5()
{
	m_comboFunc.ResetContent();
	m_comboFunc.AddString("������");
	m_comboFunc.AddString("дINI");
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
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
		n++;
	}
	return n;
}

bool IHpDllWin::GotoNodeSel(int iNode)
{
	HObject hoStep[96];                       //���96���ڵ�
	HTuple hvRes[96];
	HTREEITEM hChildItem = m_tree.GetChildItem(m_hRoot);   //��ø��ڵ���ӽڵ�
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
			strLog.Format("�ڵ�%d %s   NG��", n + 1, strFunc);
			MessageBox(strLog);			
			m_HW.WinTexts.clear();
			int nPosTxt[2] = { 1, 1 };
			m_HW.ShowImage(nPosTxt, strLog, 60, "red");
			return false;
		}
		n++;
		if (iNode == n)
			break;
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
		if (hChildItem == NULL && strFunc == "������")
		{
			int nPosTxt[2] = {80, 1};
			m_HW.ShowImage(nPosTxt,"OK",120, "green", false);
			//������ɺ��޸Ĳ��Ա�־
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
	if (sLeafs[0] == "������")
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
		LogFile("Բ�� %s ����������",sLeafs[0]);
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
	if (strTYP == "������")
		OpeningRectangle1(hoImage[nStep + nImgInx], &hoImage[nStep], fXY[0], fXY[1]);
	else
		ClosingRectangle1(hoImage[nStep + nImgInx], &hoImage[nStep], fXY[0], fXY[1]);
	HTuple hvNs;
	CountObj(hoImage[nStep], &hvNs);
	if (hvNs.I() == 0)
	{
		LogFile("���� %s ����������",strTYP);
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
	//if (strFill == "��")
	//	FillUp(hoBin, &hoImage[nStep]);                           //�����ֵ���Ѿ�������ڿ�
	//else
	//CopyImage(hoBin, &hoImage[nStep]);
	DispObj(hoImage[nStep], HW.WndID);
	HTuple hvNs,hvS,hvR,hvC;
	CountObj(hoImage[nStep], &hvNs);
	hvRes[nStep] = HTuple();
	if (hvNs.I() == 0)
	{
		LogFile("��ֵ����������!");
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
	int nNO = nStep + atoi(sLeafs[0]);                              //�ڵ������
	int nInx = atoi(sLeafs[1]);                                             //�����е�Ԫ������
	int nSum = hvRes[nNO].TupleLength().I();                 //�ڵ�����
	if (nSum <= nInx)
	{
		LogFile("ָ���ڵ����ݴ���");
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
	pSng->SplitString(sLeafs[5], ";", stsTol);      //����
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
	pSng->SplitString(strData, ";", ss);		                                                                           //ÿһ��������Ľ������
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
	HalconCpp::Connection(hoImage[nStep + nImgInx], &hoCons);           //������е�����
	SelectShape(hoCons, &hoReg, "area", "and", fS[0], fS[1]);                    //ɸѡ�õ������С����
	HTuple hvNs,hvS,hvRow,hvCol,hvRadius;
	CountObj(hoReg, &hvNs);
	int nNum = hvNs.I();	
	hvRes[nStep] = HTuple();
	if (nNum == 0)
	{
		SelectShape(hoCons, &hoImage[nStep], "area", "and", fS[0], fS[1]);   //ɸѡ�õ������С����
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
			LogFile("\n����%d�������:%.2f,���ĵ�(%.1f,%.1f)", i + 1, hvS.D(), hvCol.D(), hvRow.D());
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
	CString strMethod = sLeafs[0];                                                                                                                                                    //�õ����򷽷�
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
		hvRes[nStep][n++] = hvC[i].D();        //�����������������
		hvRes[nStep][n++] = hvR[i].D();        //�����������������
		hvRes[nStep][n++] = hvS[i].D();        //�����������������
		if (i != nNum - 1)
		{
			hvRes[nStep][n++] = ";";
		}
		if (strOrder.Right(1) == "*")
			LogFile("\n���������%d�������:%.2f,���ĵ�(%.1f,%.1f)", i + 1, hvS[i].D(), hvC[i].D(), hvR[i].D());
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
	if (strType == "�������Բ")
	{
		InnerCircle(hoImage[nStep + nKnot], &hvRow, &hvCol, &hvRadius);
		GenCircle(&hoImage[nStep], hvRow, hvCol, hvRadius);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvRadius;
	}
	else if (strType == "������Բ")
	{
		SmallestCircle(hoImage[nStep + nKnot], &hvRow, &hvCol, &hvRadius);
		GenCircle(&hoImage[nStep], hvRow, hvCol, hvRadius);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvRadius;
	}
	else if (strType == "͹��")
	{
		ShapeTrans(hoImage[nStep + nKnot], &hoImage[nStep], "convex");
		AreaCenter(hoImage[nStep], &hvS, &hvRow, &hvCol);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvS;
	}
	else if (strType == "���")
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
	else if (strType == "��С��Ӿ���")
	{
		SmallestRectangle2(hoImage[nStep + nKnot], &hvRow, &hvCol,&hvPhi,&hvLen1,&hvLen2);
		GenRectangle2(&hoImage[nStep], hvRow, hvCol, hvPhi,hvLen1,hvLen2);
		hvRes[nStep][0] = hvCol;
		hvRes[nStep][1] = hvRow;
		hvRes[nStep][2] = hvPhi;
		hvRes[nStep][3] = hvLen1;
		hvRes[nStep][4] = hvLen2;
	}
	else if (strType == "����Ӿ���")
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
		LogFile("�����������");
		return false;
	}
	DispObj(hoImage[nStep], HW.WndID);	
	DispCross(HW.WndID, hvRow, hvCol, 36, 0.25*PI);
	if (strOrder.Right(1) == "*")  //��ʾע��
	{
		if (strType == "�������Բ" || strType == "������Բ")
			LogFile("�����������������(%.1,%.1f),�뾶Ϊ��%.1f", hvCol.D(), hvRow.D(), hvRadius.D());
		else if (strType == "͹��" || strType == "���")
			LogFile("�����������������(%.1,%.1f)", hvCol.D(), hvRow.D());
		else if (strType == "��С��Ӿ���")
			LogFile("��Ӿ�����������(%.1,%.1f),�Ƕ�:%.2f,����%.1f,��:%.1f", hvCol.D(), hvRow.D(),hvPhi*180/PI,hvLen1.D(),hvLen2.D());
		else if (strType == "����Ӿ���")
			LogFile("�������Ͻǵ�(%.1,%.1f),���½ǵ�(%.1f,%.1f)", hvRes[nStep][0], hvRes[nStep][1], hvRes[nStep][2], hvRes[nStep][3]);
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
	if (strOrder.Right(1) == "*")  //��ʾע��
	{
		LogFile("����ڽ�Բ��Բ������(%.1,%.1f),�뾶Ϊ��%.1f",hvCol.D(),hvRow.D(),hvRadius.D());
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
			LogFile("ͬһ�������޷��������룡");
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
		LogFile("�����ϵ�(%.1f,%.1f)�͵�(%.1f,%.1f)֮����������%.1f",hvCol1.D(),hvRow1.D(),hvCol2.D(),hvRow2.D(),hvMinDist.D());
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
	CString strMethod = sLeafs[0];                                                                             //�õ����򷽷�
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
			LogFile("������%d ������,ѡ���˵�%d ������",hvNum.I(),nInx + 1);
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
			LogFile("������%d ������,ѡ��ȫ������", hvNum.I());
		}
		if (hvNum.I() < 1)	
			return false;
	}
	DispObj(hoImage[nStep], HW.WndID);
	return true;
}

/************************************************************************/
/* ���������ʱ��Ҫ��                                                                     */
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
	CString strConsSel = sLeafs[0];                              //�õ����򷽷�
	CString strOne = sLeafs[1];
	CString strTwo = sLeafs[2];
	CString strMethod = sLeafs[3];
	CString strDistType = sLeafs[4];                             //�������ͣ���С���룬���������룬Ĭ����С����
	int nDistType = 0;
	if (strDistType == "������")
		nDistType = 1;
	CString strSel = sLeafs[5];                                      //ѡ����Сֵ���������ֵ
	HTuple hvNum;
	CountObj(hoImage[nStep], &hvNum);
	hvRes[nStep] = HTuple();
	int nCons = hvNum.I();
	if (nCons < 2)
	{
		if (strOrder.Right(1) == "*")
			LogFile("������С��2���޷�������룡");
		return false;
	}
	HObject hoOne, hoTwo;
	HTuple hvDistType[2];
	if (strConsSel == "��������")			                     //�������������С����������/////////////////////////////
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
				if (strSel == "���ֵ")
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
		hvRes[nStep][0] = fDist;                                    //������������У�������С�������ľ���ֵ
		hvRes[nStep][1] = (int)(psNum.x);                     //����1����
		hvRes[nStep][2] = (int)(psNum.y);                     //����2����
		//////////////////////////////////��������������������������������С�������/////////////////////////////////////
	}
	else                                                                      //ָ������������֮�����С����
	{
		SelectObj(hoImage[nStep + iImgInx], &hoOne, atoi(strOne) + 1);
		if (strTwo == "���ֵ")
		{
			HTuple hvNO;
			CountObj(hoImage[nStep], &hvNO);
			SelectObj(hoImage[nStep], &hoTwo, hvNO);
		}
		else
			SelectObj(hoImage[nStep], &hoTwo, atoi(strTwo));
		DistanceCc(hoOne, hoTwo, strMethod.GetBuffer(), &hvDistType[0], &hvDistType[1]);
		strMethod.ReleaseBuffer();
		double fDist = hvDistType[nDistType].D();   //��������
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
				if (ssPosTYP[0] != "�����")
					bDraw = true;
			}
			else
			{
				LogFile("%s ���򳤿��������", strFunc);
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
		int nArraySum, iLnSel, iImgInx;                                               //ѡ��������ı�
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
		pSng->GetAffineRect(fRCVal, strPosTYP, hvRes[iKnot], hvTransX, hvTransY);  //TransXY�ǲ����������ĵ�
		fRCCen[0] = hvTransX.D();
		fRCCen[1] = hvTransY.D();
		HTuple hvW, hvH;
		HalconCpp::GetImageSize(HW.hoImage, &hvW, &hvH);                                    //�ٲ��ı䴰��ͼ�����������£�����ֱ�Ӳ��ô���ͼ��
		double fImageSize[2];
		fImageSize[0] = hvW.D();
		fImageSize[1] = hvH.D();
		vector<double> fLosts;                                                                                             //����ȱʧ�ĽǶ�  
		hvRes[nStep] = HTuple();
		fLosts.clear();
		int iRes = 0, iAMT = 0;
		double fStepX, fStepY,fMsrPhi,fDirPhi;
		if (strArryDir == "������")
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
			///////////////////////////////////////////////////////////////////�����Ƕ���ԭʼ�Ƕ� + ģ���ĽǶ�////////////////////////////////////////
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
				fLosts.push_back(1.0*j);              //���û�м�⵽�ͼ�������	
				if (fLosts.size() == 3)
				{
					LogFile("%s �ۼ� 3 ����Ե��û�м�⣡", strFunc);
					return false;
				}
				continue;
			}
			if (iLnSel >= n)                                        //Ҫʹ�õıߴ��� ȫ���������ı�
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
		LogFile("ֱ�߱�Ե����ʧ�ܣ�%s", pc);
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
			if (ssPosTYP[0] != "�����")
				bDraw = true;
		}
		else
		{
			LogFile("%s ���򳤿��������", strFunc);
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
	int nArraySum, iLnSel,iKnot = max(0,nStep - 1),iTimes = 0,iFilterRadius,iImgInx;                                               //ѡ��������ı�
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
			fDegRange = atof(sLeafs[j]);        //���в���
		else if (j == 12)                                   //��Ӧ�Ƕ��Ż�������������
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
	pSng->GetAffineRect(fRCVal,strPosTYP, hvRes[iKnot], hvTransX, hvTransY);  //TransXY�ǲ����������ĵ�
	fRCCen[0] = hvTransX.D();
	fRCCen[1] = hvTransY.D();
	fModPos[1] =  hvRes[iKnot][1];
	fModPos[0] = hvRes[iKnot][0];
	HTuple hvW, hvH;
	HalconCpp::GetImageSize(HW.hoImage, &hvW, &hvH);                                      //�ٲ��ı䴰��ͼ�����������£�����ֱ�Ӳ��ô���ͼ��
	double fImageSize[2],fCenMsr[2];
	fCenMsr[0] = fRCCen[0];
	fCenMsr[1] = fRCCen[1];
	fImageSize[0] = hvW.D();
	fImageSize[1] = hvH.D();
	vector<double> fLosts;                                                                    //����ȱʧ�ĽǶ�  
	int nLostNum = 0;                                                                              //������ʧ�Ĳ�����
A:	hvRes[nStep] = HTuple();
	fLosts.clear();
	double fPhiStep = fDegRange / nArraySum * PI / 180;
	int iRes = 0,iAMT = 0;
	for (int j = 0; j < nArraySum; j++)
	{
		double fMsrPhi = fDegST*PI / 180.0 + fRCVal[2] + j*fPhiStep;      //ģ����������ƫ��Ƕ� + ����Ƕ� + ���в����Ƕ�
		HTuple HomMat2D, HomMat2DRot, hvRow, hvCol;
		HomMat2dIdentity(&HomMat2D);
		//HomMat2dRotate(HomMat2D, fPhiStep * j, fModPos[1], fModPos[0], &HomMat2DRot); //��ת�˶���һ���ĽǶ�
		//AffineTransPoint2d(HomMat2DRot, fRCCen[1], fRCCen[0], &hvRow, &hvCol);
		HomMat2dRotate(HomMat2D, fPhiStep * (nLostNum + 1), fModPos[1], fModPos[0], &HomMat2DRot); //��ת�˶���һ���ĽǶ�
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
			fLosts.push_back(fMsrPhi);         //���û�м�⵽�ͼ���һ�������Ƕ�		
			if (fLosts.size() == 3)
			{
				LogFile("%s �ۼ�û�м�⵽ 3 ����Ե�㣡",strFunc);
				return false;
			}
			continue;
		}
		nLostNum = 0;
		if (iLnSel >= n)                                        //Ҫʹ�õıߴ��� ȫ���������ı�
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
	if (iTimes == 0 || iLnSel < 0)   //û��ָ�������߻����Ż���� == 0 ����Ҫ�Ż�
		return true;
	if (iAMT < 3)
	{
		if (strOrder.Right(1) == "*")
			LogFile("û���㹻�ĵ� %d ���Բ��",iAMT);
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
			LogFile("%s ���򳤿��������", strFunc);
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
	int nArraySum = 1, nArrayCircle = 1, iLnSel;                            //ѡ��������ı�
	int nMsrAngNum = 0;
	double fArrayStep[2];                                                                //���в�����������������о���DX��DY�������Բ�����о�ֻ�в���1 �ǽǶȲ���
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
			fArrayStep[j - 11] = atof(sLeafs[j]); //���в���
		}
		else if (j == 13)                                   //��Ӧ�Ƕ��Ż�������������
			nMsrAngNum = atoi(sLeafs[j]);
		else if (j == 14)
			strArrayDir.Format("%s", sLeafs[j]);
	}
	//////////////////////////��������ʼ��ʽ��ȡ��Ҫ��������/////////////////////////			
	HTuple hvMat2D, hvTransY, hvTransX;
	double fCenRC[2];
	int nInx = atoi(sLeafs[nSumLeaf - 1]);
	pSng->GetAffineRect(fRCVal, sLeafs[nSumLeaf - 1], hvRes[nStep + nInx], hvTransX, hvTransY);
	fCenRC[0] = hvTransX.D();
	fCenRC[1] = hvTransY.D();
	HTuple hvW, hvH;
	GetImageSize(HW.hoImage, &hvW, &hvH);                             //�ٲ��ı䴰��ͼ�����������£�����ֱ�Ӳ��ô���ͼ��
	double fImageSize[2];
	fImageSize[0] = hvW.D();
	fImageSize[1] = hvH.D();
	int iRes = 0, iLnNum = 0;
	double fMsrPhi = hvRes[nStep + nInx][2].D() + fRCVal[2];       //ģ����������ƫ��Ƕ� + ����Ƕ� + ���в����Ƕ�
	vector<double> fDirAngs;                                                           //����ȱʧ�ĽǶ�   
	int nLastLostNum = 0;
	for (int j = 0; j < nArraySum; j++)
	{
		HTuple hvMsr, hvY, hvX, hvAmp, hvDist, hvMaxAmp, Ys, Xs;
		if (nArraySum > 1 && j > 0)
		{
			//Ҫ�ı�ÿһ�������λ��
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
				//	HomMat2dRotate(HomMat2D, fArrayStep[0] * j, fRotCen[1], fRotCen[0], &HomMat2DRot); //��ת�˶���һ���ĽǶ�
				//	AffineTransPoint2d(HomMat2DRot, hvTransY, hvTransX, &hvRow, &hvCol);
				//}
				//else
				{
					HomMat2dRotate(HomMat2D, fArrayStep[0] * (nLastLostNum + 1), fRotCen[1], fRotCen[0], &HomMat2DRot); //��ת�˶���һ���ĽǶ�
					AffineTransPoint2d(HomMat2DRot, fCenRC[1], fCenRC[0], &hvRow, &hvCol);
				}
				fCenRC[0] = hvCol.D();
				fCenRC[1] = hvRow.D();
			}
			else                                                   //ֱ������
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
			if (strArrayDir == "˫��")
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
				fAbsAmp = ABS(hvAmp.D());                //��������ݶ�λ��
			}
			Xs[n] = hvTmpX[0];
			Ys[n] = hvTmpY[0];
			n++;
		}
		if (nMsrAngNum > 0 && nArrayCircle)
		{
			bool bFitOK = pSng->GetFitLineAng(HW.WndID, Ys, Xs, fFitPhi, 6, false);  //���������Ƕ�
			if (bFitOK == false)
			{
				nLastLostNum++;			
				fDirAngs.push_back(fMsrPhi);         //���û�м�⵽�ͼ���һ�������Ƕ�
				if (fDirAngs.size() == 2)
				{
					double fMidAng = 0.5*(fDirAngs[1] + fDirAngs[0]);        //�ж��м��Ǹ��Ƕ��Ƿ����˶��Ŀ��ڷ�Χ֮��
					if (fMidAng < 30 * PI / 180 || fMidAng > 150 * PI / 180)  //���ڷ����ڣ�30�ȵ�150��֮���Ǻ���ģ�
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
			//////���ϴ����Ż�����һ�εĲ�������Ƕ�/////////////////////////
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
				fDirAngs.push_back(fMsrPhi);                              //���û�м�⵽�ͼ���һ�������Ƕ�		
			if (fDirAngs.size() == 3)
			{
				//double fMidAng = 0.5*(fDirAngs[1] + fDirAngs[0]);//�ж��м��Ǹ��Ƕ��Ƿ����˶��Ŀ��ڷ�Χ֮��
				//if (fMidAng < 30 * PI / 180 || fMidAng > 150 * PI / 180)  //���ڷ����ڣ�30�ȵ�150��֮���Ǻ���ģ�
				LogFile("�ۼ�û�м�⵽ 3 ����Ե�㣡");
				return false;
			}
			continue;
		}
		if (nArrayCircle)
		{
			//nLostNum = 0;                                //�ҵ�����գ��ҵ��˾����¼��������������3��ȱʧ�ˣ���˵�����ڷ��򲻶�
			nLastLostNum = 0;
			fCenRC[0] = fMaxAmpCen[0];
			fCenRC[1] = fMaxAmpCen[1];
			fDirAngs.clear();                               //�ҵ��˾����
			//DispRectangle2(HW.WndID, fMaxAmpCen[1], fMaxAmpCen[0], fMsrPhi, fRCVal[3], fRCVal[4]);  //ֻ���������˲���ʾ����
		}	
		if (iLnSel > n)            //Ҫʹ�õıߴ��� ȫ���������ı�
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
	//	LogFile("û�м�⵽%d ���㣡",nLostNum);
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
		if (pSng->_XMLImg[nID][nStep].TMs.nM[0] < 0)	           //��Ȼ������ NCCģ��
		{
			LogFile("û������ģ���ļ���");
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
			HW.ShowImage(nPosTxt,"����ͼ��ģ������");
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
			//�� 1 ���ڵ����ģ������
			if (dlg.strTmpName.GetLength() == 0)
				return false;
			strTmpName.Format("D:\\TMP\\%s_%d", dlg.strTmpName,dlg.m_nTmpTYP);
			tree.SetItemText(hChildItem, "ģ����=" + strTmpName);
			hChildItem = tree.GetNextSiblingItem(hChildItem);                      //��Ӧģ�������ƫ��
			//�� 2 ���ڵ��������ƫ��	
			tree.SetItemText(hChildItem, "����ƫ��=" + dlg.strOffset);
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
		///////////////////////////////��ȡģ���ļ�/////////////////////////////////////////////////
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
		str.Format("ģ���趨ʧ�ܣ�%s", pc);
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
		double fScore = 0.01*atof(sLeafs[0]);                           //��ͷ�
		double fAngST = atof(sLeafs[1]);
		double fAngRange = atof(sLeafs[2]);
		int iFind = atoi(sLeafs[3]);
		int iImgInx = atoi(sLeafs[nSumLeaf - 2]);
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		CString strDef = sLeafs[4];                                           //ȱʡλ��
		HTuple hvR, hvC, hvPhi, hvScore, hvH, hvW, hvColTrans, hvRowTrans, hvMat2D, hvXs, hvYs, hvAs, hvCreds;
		hvXs = HTuple();
		hvYs = HTuple();
		hvAs = HTuple();
		hvCreds = HTuple();
		int nCount = 0;
		if (nStep + iImgInx < 0)
		{
			LogFile(strFunc + "ͼ�ڵ���������");
			return false;
		}
		HObject hoZoom;
		HalconCpp::GetImageSize(hoImage[nStep + iImgInx], &hvW, &hvH);
		ZoomImageSize(hoImage[nStep + iImgInx], &hoZoom, ZOOM*hvW, ZOOM*hvH, "constant");
		int nNum = hvRes[nStep + iKnot].TupleLength().I();
		for (int i = 0; i < nNum && nID < 0; i += 4)
		{
			/************************************************************************/
			/* Ҫ���ж�ģ����NCC����Shape
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
		//////////////////////�����ǲ���״̬����ͼ������״̬/////////////////////////////////////////
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
			double fDXY[2] = { 0 };                                                                                    //��ǰģ�����ģ�����ĵ�ƫ��
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
		hvRes[nStep] = HTuple();                                                                                      //����ѹ�������飬ÿ��4�����
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
					LogFile("NCCģ��û��ƥ��ģ�壬����Ĭ�ϵ㣡");
				return true;
			}
			if (strOrder.Right(1) == "*")
				LogFile("NCCģ��û��ƥ��ģ�");
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
		LogFile("%s ʧ��! : %s", strFunc, pc);
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
			LogFile("�����������½��������");
			return false;
		}
	}
	if (bDraw)                                //�����ú���������
	{
		HTuple hvPos[4];
		int nPosTxt[2] = { 0 };
		HW.ShowImage(nPosTxt,"��ѡ��������");
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
		//������������������ü�ͼƬ
		hChildItem = tree.GetChildItem(hItem);
		pSng->GetLeafsValue(tree, hItem, nSumLeaf, sLeafs);
	}
	//////////////////ÿ������Ķ�����һ��///////////////////////
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
	ReduceDomain(HW.hoImage, hoUnion, &hoImage[nStep]);  //��������ʼ����ԭʼͼ�ü�
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
			LogFile(strFunc + "��������");
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
			MessageBox(NULL, "Բ������ƴ���", "", MB_ICONERROR);
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
	if (strPosTYP == "�̶�λ" || strPosTYP == "�̶���")
	{
		hvTransX = fXYR[0];
		hvTransY = fXYR[1];
	}
	else if (strPosTYP.Left(6) == "ģ���" || strPosTYP.Left(6) == "�����")
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
		LogFile("Բ������(%.1f,%.1f) ���뾶:%.1f",hvTransX.D(),hvTransY.D(),fRadius);
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
			MessageBox(NULL, "Բ��������ƴ��󣡣�", "", MB_ICONERROR);
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
	if (strPosTYP == "�̶�λ" || strPosTYP == "�̶���")
	{
		hvTransX = fRadiusVal[0];
		hvTransY = fRadiusVal[1];
	}
	else if (strPosTYP.Left(6) == "ģ���")
	{
		pSng->GetAffineRect(fRadiusVal, strPosTYP, hvRes[nStep  + iKnot], hvTransX, hvTransY);
	}
	else if (strPosTYP.Left(6) == "�����")
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
		int iSel = atoi(sLeafs[0]);                                 //��ڵ���������ڵ�
		if (iSel < 0)
		{
			LogFile("�����������޷����Բ��");
			return false;
		}
		int iKnot = atoi(sLeafs[nSumLeaf - 1]);
		CString strType = sLeafs[1];
		HTuple hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Row, hv_Col, hvAng;
		if (strType == "��")
		{
			int iRes = hvRes[nStep + iKnot].TupleLength().I();
			if (iRes < 3)
			{
				LogFile("û���㹻�ĵ����Բ��");
				return false;
			}
			CString strData = pSng->GetHTupleString(hvRes[nStep + iKnot]);
			CStringArray ss;
			pSng->SplitString(strData, ";", ss);		                                                 //ÿһ��������Ľ������
			HObject hoCons, hoCir;
			HTuple hvRs, hvCs;
			int iAMT = 0;
			for (int j = 0; j < ss.GetCount(); j++)
			{
				CStringArray Ds, Xs;
				pSng->SplitString(ss[j], ",", Ds);
				if (iSel >= Ds.GetCount() && Ds.GetCount() > 0)
				{
					iSel = Ds.GetUpperBound();                                                      //���һ��Ԫ��
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
				LogFile("û���㹻�ϸ��Բ������%d ���Բ��", iAMT);
				return false;
			}
			/////////////////////////////////////////���������Բ��,ǰ�ڵĵ��Ѿ��Ż����ˣ����Բ���Ҫ��һ������//////////////////////////////////////////////////////////////////
			GenContourPolygonXld(&hoCons, hvRs, hvCs);
			FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
			GenCircle(&hoCir, hv_Row, hv_Col, hv_Radius);
			GenContourRegionXld(hoCir, &hoImage[nStep], "border");
			DispObj(hoImage[nStep], HW.WndID);
		}
		else if (strType == "����")
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
		LogFile("%s ʧ��! : %s", strFunc,pc);
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
	double fDegRef = atof(sLeafs[0]);                                  //���ֱ�ߵĲο��Ƕ�
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
		LogFile("ֱ�ߵ�������̫�ӽ����޷�����Ƕ�");
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
	//ѡ��н�С��ʸ������������Ȼ����ͼ�����꣬���ǵó��ĽǶ��Ƿ������ֶ����
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
		LogFile("�ڵ����ݲ��㣬�޷�����");
		return false;
	}
	HTuple hvCol, hvRow,hvParal;
	IntersectionLines(hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][2], 
		                            hvRes[nStep + iLnot][1], hvRes[nStep + iLnot][0], hvRes[nStep + iLnot][3], hvRes[nStep + iLnot][2],&hvRow,&hvCol,&hvParal);
	if (hvParal.I() == 1)
	{
		LogFile("ֱ��ƽ�У�û�н���");
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
		double fLnLen = atof(sLeafs[nSumLeaf - 3]);                                                          //ֱ�߳���
		CString strTYP = sLeafs[1];
		HTuple hvRowA, hvColA, hvRowB, hvColB, hvDist;
		if (strTYP == "��")
		{
			int iSel = atoi(sLeafs[0]);
			double fFilter = atof(sLeafs[6]);                                                                         //��������
			if (iSel < 0)
			{
				LogFile("%s ѡ��ĵ���������", strFunc);
				return false;
			}
			int iRes = hvRes[iKnot].TupleLength().I();
			if (iRes < 7)
			{
				LogFile("%s ����̫���޷����ֱ�ߣ�");
				return false;
			}
			CString strData = pSng->GetHTupleString(hvRes[iKnot]);
			CStrs ssGrp;
			pSng->SplitString(strData, ";", ssGrp);		                                                    //ÿһ��������Ľ������
			HTuple hvRs, hvCs;
			int iAMT = 0;
			for (int j = 0; j < (int)ssGrp.size(); j++)
			{
				CStrs Ds, Xs;
				pSng->SplitString(ssGrp[j], ",", Ds);
				if (iSel >= (int)Ds.size() && Ds.size() > 0)
				{
					iSel = (int)Ds.size() - 1;                                              //���һ��Ԫ��
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
				LogFile("û���㹻�ϸ�ĵ�%d���ֱ�ߣ�", iAMT);
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
				LogFile("%s ֱ�����ʧ�ܣ�", strFunc);
				return false;
			}
			AngleLx(hvRowA, hvColA, hvRowB, hvColB, &hvPhi);
			fPhi = hvPhi.D() + 0.5*PI;       //�����ʸ������
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
				LogFile("%s GetFitLine���ֱ��ʧ�ܣ�", strFunc);
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
		double fDegRef = atof(sLeafs[7]);                                    //���ֱ�ߵĲο��Ƕ�    
		double fPhiRef = -1.0*fDegRef*PI / 180;                         //����ͼ��Y�����£�����Ҫ����-1
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
		//ѡ��н�С��ʸ������������Ȼ����ͼ�����꣬���ǵó��ĽǶ��Ƿ������ֶ����
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
		LogFile("%s ʧ�ܣ�%s", strFunc, pc);
		return false;
	}	
	return true;
}

/************************************************************************/
/*         ����ĳЩ�ڵ�Ľ�����ݣ����ۼ�������Ĳ���ֵ                
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
			LogFile("�ڵ���Ŀ����������ƥ�䣡");
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
				LogFile("�ڵ� %d ����������%d �����˸ýڵ����������%d !!",nStep + iKnot,iInx,iKnotSum);
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
		LogFile("%s ʧ�ܣ�%s", strFunc, pc);
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
	int iKnot = atoi(sLeafs[nSumLeaf - 1]);                                                         //�㴦��
	int nGrps = stsCof.GetCount();                                                                     //ÿ���ȡ����
	CString strData = pSng->GetHTupleString(hvRes[nStep + iKnot]);
	///////���ϴ�������˵����X*Y*Thres,X*Y*Thres; ���ָ�ʽ/////////////////////////////////////////
	CStringArray ss;
	pSng->SplitString(strData, ";", ss);		                                                          //ÿ��������Ľ������
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
				iSel = Ds.GetUpperBound();                                            //���һ��Ԫ��
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
		LogFile("û���㹻�ĵ�");
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
		LogFile("�Ҷȣ�%.1f �ͷ��%.1f",hvMean.D(),hvDev.D());
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
			LogFile("�ڵ� %d ���ݳ��� %d ���趨�����ݳ��� %d ��һ��!!",nStep + iKnot,iResLen,nNum);
			return false;
		}
		for (int i = 0; i < nNum; i++)
		{
			CString strTmp;
			strTmp.Format("%.2f,", hvRes[nStep + iKnot][i].D());   //��Ҫ�ǽڵ����ʧ��
			strVal += strTmp;
		}
		strVal.TrimRight(",");
		pSng->SetCfgString(strSec, strKey, strVal);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("%s ʧ�ܣ�%s", strFunc, pc);
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
	int iImgInx = atoi(sLeafs[nSumLeaf - 2]);  //ͼ�ڵ�
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
	if (strAtrrib == "��Բ")
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
		if (hvAttrib == -1 && strAtrrib == "�߶�" ||
			hvAttrib == 0 && strAtrrib == "��Բ"  ||
			hvAttrib == 1 && strAtrrib == "Բ��"  || strAtrrib == "ȫ��")
		{
			ConcatObj(hoCons, hoObj, &hoCons);
		}
	}
	CountObj(hoCons, &hvNum);
	nCons = hvNum.I();
	if (nCons < 1)
		return false;
	double fLenSel = 1900000;
	if (strType == "���ֵ")
	{
		fLenSel = 10;
	}
	bool bFind = false;
	for (int i = 0; i < nCons; i++)
	{
		SelectObj(hoCons, &hoObj, i + 1);
		LengthXld(hoObj, &hvLen);
		if (hvLen.D() > fLenSel && strType == "���ֵ" ||
			hvLen.D() < fLenSel && strType == "��Сֵ")
		{
			SelectObj(hoCons, &hoImage[nStep], i + 1);
			fLenSel = hvLen.D();
			bFind = true;
		}
		if (strType == "Բ���뾶" && hvLen.D() > fLenSel)
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
		LogFile("û���ҵ�����������Բ������");
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
		CString strAngRange = sLeafs[2];                                                //�Ƕȷ�Χ
		pSng->SplitString(strAngRange, ",", sDeg);
		double fDeg[2];
		fDeg[0] = atof(sDeg[0]);
		fDeg[1] = atof(sDeg[1]);
		int nPtNum = atoi(sLeafs[0]);                                                         //����ȡƽ���ĵ���
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
		if (strType == "���ֵ")
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
					//�������ж��ǲ������趨�ĽǶ�֮��

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
			//�ָ��������ֶ���ÿһ�������ϵ�Բ������ĵ㣬���������ϵĵ㵽Բ������Ҫ��-90~90��֮��
			CountObj(hoSplit, &hvNum);
			int nCons = hvNum.I();
			double fMinDist = 10000;
			for (int i = 0; i < nCons; i++)
			{
				SelectObj(hoSplit, &hoObj, i + 1);
				DistanceCcMinPoints(hoPtCon, hoObj, "fast_point_to_segment",&hvDist[0],&hvR1,&hvC1,&hvR2,&hvC2);
				psClose.y = hvR2.D();
				psClose.x = hvC2.D();
				//���ϼ���㵽�����ϵ��ֱ�߷���
				AngleLx(fPtPos[1], fPtPos[0], psClose.y, psClose.x, &hvAng);
				DistancePp(hvR1, hvC1, hvR2, hvC2, &hvCurrDist);
				double fCurrDist = hvCurrDist.D();
				if (hvAng.D() >= fDeg[0] * PI / 180 && hvAng.D() <= fDeg[1] * PI / 180 && fCurrDist < fMinDist)					//ֻ��������Ƕȷ�Χ�ڵ������
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
			LogFile("�����������(%.1f,%.1f)������:%.1f,������ˮƽ�нǣ�%.1f",psClose.x,psClose.y,hvDist[nType].D(),hvAng.D()*180/PI);
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("%s ʧ�ܣ�%s", strFunc,pc);
		LogFile(str);
		return false;
	}
	return true;
}

bool IsFuncDistL2C(CHWin& HW, int nID, int nStep, HObject hoImage[96], HTuple hvRes[96], CString strFunc, CTreeCtrl& tree, HTREEITEM hItem)
{	
	//���㷨�����������ϵ�ֱ���������Զ���ĵ�
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
	if (strType == "���ֵ")
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
		/*                     �������Ҫ��סÿ��Ԫ�ص�λ��              
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
		if (strType == "Բ������")
		{
			GenCircle(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2]);
		}
		else if (strType = "����������")
		{
			GenRectangle1(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3]);
		}
		else if (strType == "�Ƕ�������")
		{
			GenRectangle2(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][4]);
		}
		else if (strType == "��Բ����")
		{
			GenEllipse(&hoImage[nStep], hvRes[nStep + iKnot][1], hvRes[nStep + iKnot][0], hvRes[nStep + iKnot][2], hvRes[nStep + iKnot][3], hvRes[nStep + iKnot][4]);
		}
		else if (strType == "��������")
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
				LogFile("��������̫С��");
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
		str.Format("ģ���趨ʧ�ܣ�%s", pc);
		LogFile(str);
	}		
	return false;
}

/************************************************************************/
/* ��Ҫ���������                                                                      */
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
		LogFile("�����Сû�����ã�");
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
		LogFile("%s ����ƫ�����", strFunc);
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
		nSumLeaf = pSng->_XMLImg[nID][nStep].psLeaf.size();                                 //Ҷ�ڵ�����
	bool bRet = true;
	if (strFunc == "������")
	{			
		bRet = IsFuncOut(HW, nID,nStep,hoImage,hvRes,strType,tree,hItem);
	}
	else if (strFunc == "��ֵ��" || strFunc == "!��ֵ��")
	{
		bRet = IsFuncBinVal(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "���ο�������" || strFunc == "!���ο�������")
	{
		bRet = IsFuncRectOpenClose(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "Բ�ο�������" || strFunc == "!Բ�ο�������")
	{
		bRet = IsFuncCircleOpenClose(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "���ɸѡ" || strFunc == "!���ɸѡ")
	{
		bRet = IsFuncRegionSelected(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�������" || strFunc == "!�������")
	{
		bRet = IsFuncRegionSort(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "������")
	{
		bRet = IsFuncRegionProcess(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "������С����" || strFunc == "!������С����")
	{
		bRet = IsFuncRegionMinDist(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "������ȡ" || strFunc == "!������ȡ")
	{
		bRet = IsFuncGetCons(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "��������" || strFunc == "!��������")
	{
		bRet = IsFuncConsDist(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "ģ�嶨λ" || strFunc == "!ģ�嶨λ")
	{
		bRet = IsFuncFindTmp(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "Բ��Ե����" || strFunc == "!Բ��Ե����")
	{
		bRet = IsFuncMarginMsrCircle(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "ֱ�߱�Ե����")
	{
		bRet = IsFuncMarginMsrLine(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "����ģ��")
	{
		bRet = IsFuncLoadRCMod(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "��������")
	{
		bRet = IsFuncRCRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�Ƕ�������")
	{
		bRet = IsFuncDirRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "Բ������")
	{
		bRet = IsFuncRingRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "Բ������")
	{
		bRet = IsFuncCircleRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strType == "���������")
	{
		bRet = IsFuncResultRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "���Բ")
	{
		bRet = IsFuncFitCircle(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "���ֱ��")
	{
		bRet = IsFuncFitLine(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�㴦��")
	{
		bRet = IsFuncProcessPt(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�������")
	{
		bRet = IsFuncProcessData(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "ֱ�߽Ƕ�")
	{
		bRet = IsFuncLineAng(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "ֱ�߽���")
	{
		bRet = IsFuncLineXPoint(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�Ҷȷ���")
	{
		bRet = IsFuncMeanDev(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�����ָ�")
	{
		bRet = IsFuncSplitCons(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�㵽��������")
	{
		bRet = IsFuncDistP2C(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "ֱ�ߵ���������")
	{
		bRet = IsFuncDistL2C(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "�������")
	{
		bRet = IsFuncRelRegion(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "��ά��")
	{
		bRet = IsFuncCode2D(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else if (strFunc == "дINI")
	{
		bRet = IsFuncWriteINI(HW, nID, nStep, hoImage, hvRes, strType, tree, hItem);
	}
	else
	{
		LogFile("û������ָ���Ӧ�Ĵ�������");
		return false;
	}
	return bRet;
}

extern "C" __declspec(dllexport) bool IsImageOK(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut)
{
	//ֱ��������Ĺ������̽��м��
	CSingleton* pSng = CSingleton::GetInstance();
	HObject hoImage[96];
	HTuple hvRes[96];
	CopyImage(HW.hoImage, &pSng->_hoImgOrg[nID]);
	//CString strMs;
	//strMs.Format("����ʱ��:%d", GetTickCount());
	HW.WinTexts.clear();
	int nPosTxt[2] = { 10, 10 };
	CTreeCtrl tree;
	psOut.clear();
	pSng->_bRes = true;  //����һ�����,Ĭ��OK
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
				LogFile("��%d�������NG��", i + 1);
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
				LogFile("����%d��,���OK", i + 1);
			}			
			if (pSng->_bRes == false)
				return false;
			if (pSng->_XMLImg[nID][i].strType == "������")
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
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
	}
}

void IHpDllWin::OnBnClickedBtnSavXml()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strXMLName,strXMLDir;
	GetDlgItemText(IDC_EDIT_NAME, strXMLName);
	if (strXMLName.GetLength() < 1)
	{
		MessageBox("û������Ҫ�����XML�ļ����ƣ�", "", MB_ICONWARNING);
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
			if (IDNO == MessageBox("�Ƿ񸲸�ԭ���ļ�" + strXMLName + " ?", "", MB_YESNO))
			{
				MessageBox(strXMLName + "��Ϊ���������ļ�������?", "", MB_ICONWARNING);
				return;
			}
		}
	}
	else
	{
		strXMLDir.Format("%s", m_strXML);
	}
	TiXmlDocument xml_doc;                                                                           // ���XML����
	xml_doc.LinkEndChild(new TiXmlDeclaration("1.0", "GBK", ""));              // ��Ӹ�Ԫ��
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
	xml_root->SetAttribute("Test", stsCam[1].Right(1));   //��������
	xml_doc.LinkEndChild(xml_root);

	ExpandTree(m_hRoot, xml_root, true);
	int nRet = xml_doc.SaveFile(strXMLDir);
	if (!nRet)
	{
		MessageBox(strXMLDir + "����ʧ�ܣ�", "", MB_ICONWARNING);
		return;
	}
	MessageBox(strXMLDir + "�ɹ����棡", "", MB_OK);
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
		MessageBox("�޷���ָ����XML�ļ���", "��ȡʧ��", MB_ICONWARNING);
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
	m_hRoot = m_tree.InsertItem(strRoot, 1, 0, TVI_ROOT);              //���һ�����
	m_tree.Invalidate();
	TiXmlElement *pChild = root->FirstChildElement();
	while (pChild != NULL)
	{
		m_tree.Expand(m_hRoot, TVE_EXPAND);
		HTREEITEM hChild = m_tree.InsertItem(pChild->Value(), m_hRoot);                               //����������
		TiXmlElement *pleaf = pChild->FirstChildElement();
		while (pleaf != NULL)
		{
			CString strName = pleaf->Value();
			CString strValue = pleaf->GetText();
			HTREEITEM hLeaf = m_tree.InsertItem(strName + "=" + strValue, hChild);                //����������
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
	//����Ҽ�������
	CMenu menu;
	CPoint pt;
	//pt = GetCurrentMessage()->pt;   //��ȡ��ǰ�������Ϣ�������
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //��������Ļ���꣬ת�������οؼ��Ŀͻ�������
	UINT uFlags = 0;
	HTREEITEM hItem = m_tree.HitTest(pt, &uFlags);
	if (hItem == m_hRoot)
	{
		//���ڵ�ֱ������Gain,Gamma,Exposure ����
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
		bool bFind = false;                   //����Ľ�㲻����Ҷ�ڵ�
		while (hChildItem)
		{
			if (hItem == hChildItem)
			{
				bFind = true;			
				break;
			}
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
		}
		if (bFind)
		{
			m_tree.SelectItem(hItem);
			//���ݲ�ͬ���͵Ľڵ㵯���˵�
			CMenu *psubmenu;
			m_tree.ClientToScreen(&pt);
			CString strItem = m_tree.GetItemText(hChildItem);
			psubmenu = menu.GetSubMenu(0);
			if (strItem.Right(4) != "ģ��")
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
	ps.strNick = "�ع�";
	ps.x = m_fExp;
	dlg.m_comboVals.push_back(ps);
	ps.strNick = "����";
	ps.x = m_fGain;
	dlg.m_comboVals.push_back(ps);
	ps.strNick = "٤��";
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
	//���������iInx �Ժ�Ľڵ�
	CSingleton* pSng = CSingleton::GetInstance();
	CString strRoot = m_tree.GetItemText(m_hRoot);
	strRoot.TrimRight("0");
	strRoot.TrimRight("1");
	CStringArray stsCam;
	pSng->SplitString(strRoot, "=", stsCam);
	strRoot.Format("%s", stsCam[0]);
	strRoot.Format("%s=%s0", stsCam[0], stsCam[1]); //����Test:0
	m_tree.SetItemText(m_hRoot, strRoot);
	//////////////////////�������������ÿ���ڵ�///////////////////////////////////
	//GetCursorPos(&pt);
	//m_tree.ScreenToClient(&pt);        //��������Ļ���꣬ת�������οؼ��Ŀͻ�������
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
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
			continue;
		}
		if (bUpdate)
		{
			//�������µ�ÿһ���ڵ㣬����ýڵ������ĸ��N��ô�ı�ڵ�����
			CString strSec = m_tree.GetItemText(hChildItem);
			HTREEITEM hSon = m_tree.GetChildItem(hChildItem);
			while (hSon)
			{
				CString strNodeText = m_tree.GetItemText(hSon),strNewText;
				CStrs ssNode,ssKnot,ssRes;
				if (strNodeText.Left(1) == "N" || strNodeText.Right(4) == "�ڵ�")
				{
					pSng->SplitString(strNodeText, "=", ssNode);
					pSng->SplitString(ssNode[1], ",", ssKnot);
					int nNode = (int)ssKnot.size();
					for (int i = 0; i < nNode; i++)
					{
						int iKnot = atoi(ssKnot[i]) + 1;
						if (bAdd == true)   //����һ���ڵ㷴����Ҫ -1
							iKnot -= 1;
						if (iKnot >= 0)
						{
							//�����ڵ㶼��Ҫɾ��
							if (IDYES == MessageBox(strSec + strNodeText + " ������������Ƿ���Ҫɾ������ڵ㣡","��������",MB_YESNO))
							{
								bDel = true;
							}							
							break;
						}
						////////////////////////�޸Ľڵ�����//////////////////////
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
				hSon = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
			}
		}
		if (bDel)
		{
			HTREEITEM hNext = m_tree.GetNextSiblingItem(hChildItem);
			m_tree.DeleteItem(hChildItem);
			hChildItem = hNext;
			continue;
		}
		hChildItem = m_tree.GetNextSiblingItem(hChildItem); //��ȡ���ڵ���ֵܽڵ�
	}
}

void IHpDllWin::OnMenuDel()
{
	HTREEITEM hItem = m_tree.GetSelectedItem();
	CSingleton* pSng = CSingleton::GetInstance();
	CString str = m_tree.GetItemText(hItem);
	if (IDNO == MessageBox("ȷ��Ҫɾ���ڵ� ? " + str,"",MB_YESNO))
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
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
	}
	m_tree.DeleteItem(hItem);
	UpdateTest();
	////////////////////////////������ɨ��ÿһ���ڵ㣬��������нڵ�ָ��ǰ�ڵ㣬�ͰѸýڵ�������ú�ɫ��ʾ//////////////////////
	if (hNext == NULL)
	{
		//����û�нڵ�ֱ��OK
		return;
	}
	while (hNext)
	{
		//���ɨ���ӽڵ㣬�ж��Ƿ�ڵ������Ƿ�N��ͷ
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
				int iKnot = atoi(ps.strMN);       //�ڵ�ָ��
				hChildItem = m_tree.GetChildItem(m_hRoot);
				int iCurr = -1;
				while (hChildItem)
				{
					iCurr++;
					if (hItem == hChild)
						break;
					hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
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
	bool bFind = false;                   //����Ľ�㲻����Ҷ�ڵ�
	while (hChildItem)
	{
		if (hItem == hChildItem)
		{
			bFind = true;
			break;
		}
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);  //��ȡ���ڵ���ֵܽڵ�
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
	sy.Set("�ڵ�����", 220);
	dlg.sts.push_back(sy);
	sy.Set("�ڵ�ֵ", 320);
	dlg.sts.push_back(sy);
	if (dlg.DoModal() == IDOK)
	{
		//�ı���psLeafֵд���ڵ�������
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
	//�޸����Ҫ�ı�test����
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
		if (ss[0] == "ģ��" || ss[0] == "ģ����")
		{
			pSng->SplitString(ss[1], ";",stsNCC);
			if (stsNCC.size() == MAX_TM)
			{
				MessageBox("�Ѿ����㹻���NCCģ�壡","�޷����NCCģ��",MB_ICONWARNING);
				return;
			}
			//����ģ��Ի���
			HTuple hvPos[4], hvNCC, hvW, hvH;
			int nPosTxt[2] = { 0 };
			m_HW.ShowImage(nPosTxt, "����ͼ��ģ������");
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
			//�������ڵ�ԭ�е�·��,�Ա�ͣ�ڸýڵ㴦
			stsNCC.push_back(strTmpName);
			strNick = pSng->LinkString(stsNCC, ";");
			m_tree.SetItemText(hChildItem, "ģ��=" + strNick);
			//�� 2 ���ڵ��������ƫ��	
			hChildItem = m_tree.GetNextSiblingItem(hChildItem);       //��ȡ���ڵ���ֵܽڵ�
			pSng->SplitString(strItem, "=", stsDelta);
			stsDelta.push_back(dlg.strOffset);
			strOffset = pSng->LinkString(stsDelta, ";");
			m_tree.SetItemText(hChildItem, "����ƫ��=" + strOffset);
			break;
		}
	}
	///////////////////////////////////���������е��ýڵ�����ģ����////////////////
	//bool bOK = GotoNodeSel(iNode);
	//if (bOK == false)
	//{
	//	MessageBox("NCCģ�����ʧ�ܣ�", "",MB_ICONWARNING);
	//	return;
	//}
	//CString strNCC = m_tree.GetItemText(hChildItem);
	//pSng->SplitString(strNCC, "=", ss);
	//if (ss.GetCount() != 2)
	//{
	//	MessageBox("ģ��ڵ��ʽ����","",MB_ICONWARNING);
	//	return;
	//}
	//CString strNodeVal("ģ��=");
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
	if (str == "��������")
	{
		HTuple hvPos[4];
		int nPosTxt[2] = { 0, 0 };
		m_HW.ShowImage(nPosTxt,"��ѡ��������");
		CString strVal;
		DrawRectangle1(m_HW.WndID, &hvPos[0], &hvPos[1], &hvPos[2], &hvPos[3]);
		HTREEITEM hLeafItem = m_tree.GetChildItem(hItem);
		for (int i = 0; i < 4; i++)
		{
			strVal.Format("%.1f", hvPos[i].D());
			CString strKnot = m_tree.GetItemText(hLeafItem);
			CStringArray sts;
			pSng->SplitString(strKnot, "=", sts);
			sts[1].TrimRight();                                    //�޳��ո�
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
			MessageBox(sKeys[i] + " �ڵ���������","",MB_ICONWARNING);
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
		MessageBox(strNodeText  + " �ڵ���������", "", MB_ICONWARNING);
		return;
	}
}

void IHpDllWin::OnMenuFirst()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CPoint pt;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //��������Ļ���꣬ת�������οؼ��Ŀͻ�������
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
	while (hChild)  //���ɾ����һ���ڵ��µ����ݣ���ɾ��Sec��ֻɾ��Ҷ�ڵ㣬ɾ��ǰ��ÿ���ڵ�����ݱ�������
	{
		stsItem.push_back(m_tree.GetItemText(hChild));
		HTREEITEM hCurr = hChild;
		hChild = m_tree.GetNextSiblingItem(hChild);
		m_tree.DeleteItem(hCurr);
	}
	m_tree.SetItemText(hFirstItem, str);          //�޸ĵ�һ���ڵ�����
	InsertNode(str, hFirstItem, false);               //�������½ڵ�
	InsertNode(strFirst, stsItem, hFirstItem);   //�ٰ�ԭ���ĵ�һ���ڵ������
	UpdateTest(0);
}

void IHpDllWin::OnMenuInsert()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CPoint pt;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);        //��������Ļ���꣬ת�������οؼ��Ŀͻ�������
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
			m_tree.InsertItem(sKeys[i] + "=" + strs[0], hChild);   //Ĭ�ϵ�һ������
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
	//����ȱʡ�����������
	CSingleton* pSng = CSingleton::GetInstance();
	m_fGam = atof(pSng->GetCfgString("٤��", m_strCam, "1.0"));
	m_fGain = atof(pSng->GetCfgString("����", m_strCam, "1.0"));
	m_fExp = atof(pSng->GetCfgString("�ع�", m_strCam, "5000"));
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
	/////�����ʼ���ɹ�////////////////////////
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
