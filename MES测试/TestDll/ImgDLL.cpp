#include "stdafx.h"
#include "ImgDLL.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\\IDlgCamCfg.h"
#include "..\DllMod\IDlgInfo.h"

HINSTANCE g_hDLL;
typedef bool(*pLoadImageXML)(int nID, CString strXML, int& nExp, float& fGain, float& fGam);
typedef bool(*pUnLoadImageXML)(int nID);
typedef bool(*pIsImageOK)(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut);
typedef IHpDllWin* (*hpDllFun)(CString strTxt, CString stXML);
typedef CIDlgJob* (*hpDllJob)(CString strTxt);
typedef CIDlgInfo* (*hpLogDlg)();
typedef CIDlgCamCfg* (*hpCamCfg)(int nCam);
typedef CIDlgCamCfg* (*pIsCamCfg)();
typedef void(*pSnap)(CHWin& HW, int nCam, int nTimes);
typedef bool(*pSetCamPar)(int nCam, float fExp, float fGain, float fGam);
typedef bool(*pImageXMLClear)();
typedef void(*pWriteLog)(const char *fmt, ...);
typedef bool(*pInitCam)(int iNum);
typedef bool(*pCloseCam)(int iNum);
typedef bool(*pSvrLink)(int iSvrPort);
typedef int (*pSvrSnd)(int nNo, char* data, int nLen);
typedef bool(*pStopSvr)();
typedef int(*pSvrLnkStatus)(int nNO);
typedef bool (*pSetHeartTime)(int nCam,UINT uTime);
typedef void (*pSvrRev)(int nNO,CString& strSvrRev);
typedef void(*pSvrClear)(int nNO);

pSnap  mSnap;
pSetHeartTime mSetHeartTime;
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
pSvrLink  mSvrLink;
pStopSvr mStopSvr;
pSvrLnkStatus mSvrLnkStatus;
pSvrSnd mSvrSnd;
pSvrRev mSvrRev;
pSvrClear mSvrClr;

CImgDLL::CImgDLL()
{
	g_hDLL = NULL;
}


CImgDLL::~CImgDLL()
{
	FreeLibrary(g_hDLL);
}

BOOL CImgDLL::Init()
{
	g_hDLL = LoadLibrary("DllMod.dll");
	if (NULL == g_hDLL)
	{
		AfxMessageBox("加载 MyDLL.dll 失败");
		return FALSE;
	}
	mImageDlg = (hpDllFun)GetProcAddress(g_hDLL, "SetImage");
	if (NULL == mImageDlg)
	{
		AfxMessageBox("DLL中函数寻找图像窗口对话框失败！");
		return FALSE;
	}
	mIsImageOK = (pIsImageOK)GetProcAddress(g_hDLL, "IsImageOK");
	mLoadXML = (pLoadImageXML)GetProcAddress(g_hDLL, "LoadImageXML");
	mUnLoadXML = (pUnLoadImageXML)GetProcAddress(g_hDLL, "UnLoadImageXML");
	mJobDlg = (hpDllJob)GetProcAddress(g_hDLL, "SetThread");
	if (NULL == mJobDlg)
	{
		AfxMessageBox("DLL中函数寻找工作流程对话框失败！");
		return FALSE;
	}
	mLogDlg = (hpLogDlg)GetProcAddress(g_hDLL, "GetLogInfo");
	if (NULL == mLogDlg)
	{
		AfxMessageBox("DLL中函数中没有Log对话框函数！");
		return FALSE;
	}
	mImageXMLClear = (pImageXMLClear)GetProcAddress(g_hDLL, "IsImageXMLClear");
	mInitCam = (pInitCam)GetProcAddress(g_hDLL, "InitDLLCam");
	mCloseCam = (pCloseCam)GetProcAddress(g_hDLL, "CloseCam");
	mCamCfg = (hpCamCfg)GetProcAddress(g_hDLL, "GetDlgCamCfg");
	mIsCamCfg = (pIsCamCfg)GetProcAddress(g_hDLL, "GetCamCfgPtr");
	mSnap = (pSnap)GetProcAddress(g_hDLL, "Snap");
	if (mSnap == NULL)
	{
		AfxMessageBox("DLL中函数中没有相机采集的函数！");
		return FALSE;
	}
	mSetCamPar = (pSetCamPar)GetProcAddress(g_hDLL, "SetCamPar");
	if (NULL == mSetCamPar)
	{
		AfxMessageBox("DLL中函数中没有相机参数设定的函数！");
		return FALSE;
	}
	mWriteLog = (pWriteLog)GetProcAddress(g_hDLL, "WriteLog");
	if (NULL == mWriteLog)
	{
		AfxMessageBox("DLL中函数中没有写Log记录的函数！");
		return FALSE;
	}
	mSvrLink = (pSvrLink)GetProcAddress(g_hDLL, "SvrLink");
	if (NULL == mSvrLink)
	{
		AfxMessageBox("DLL中没有服务器创建函数！");
		return FALSE;
	}
	mSvrSnd = (pSvrSnd)GetProcAddress(g_hDLL, "SvrSnd");
	if (NULL == mSvrSnd)
	{
		AfxMessageBox("DLL中没有服务器创建函数！");
		return FALSE;
	}
	mStopSvr = (pStopSvr)GetProcAddress(g_hDLL, "StopSvr");
	if (NULL == mStopSvr)
	{
		AfxMessageBox("DLL中没有服务器创建函数！");
		return FALSE;
	}
	mSetHeartTime = (pSetHeartTime)GetProcAddress(g_hDLL, "SetHeartTime");
	if (NULL == mSetHeartTime)
	{
		AfxMessageBox("DLL中没有设置相机心跳函数！");
		return FALSE;
	}
	mSvrRev = (pSvrRev)GetProcAddress(g_hDLL, "SvrRev");
	if (NULL == mSvrRev)
	{
		AfxMessageBox("DLL中没有服务器接收数据函数！");
		return FALSE;
	}
	mSvrClr = (pSvrClear)GetProcAddress(g_hDLL, "SvrClearRev");
	if (NULL == mSvrClr)
	{
		AfxMessageBox("DLL中没有服务器对应客户端接收数据清空函数！");
		return FALSE;
	}
	mSvrLnkStatus = (pSvrLnkStatus)GetProcAddress(g_hDLL, "SvrLinkStatus");
	if (NULL == mSvrLnkStatus)
	{
		AfxMessageBox("DLL中没有服务器对应客户端连接状态函数！");
		return FALSE;
	}
	return TRUE;
}

bool CImgDLL::SvrLink(int iSvrPort)
{
	bool bLink = mSvrLink(iSvrPort);
	return bLink;
}

int CImgDLL::SvrLinkStatus(int nNO)
{
	int nLink = mSvrLnkStatus(nNO);
	return nLink;
}

bool CImgDLL::SvrStop()
{
	bool bRet = mStopSvr();
	return bRet;
}

int CImgDLL::SvrSnd(int nNo, char* data, int nLen)
{
	int nSnd = mSvrSnd(nNo, data, nLen);
	return nSnd;
}

void CImgDLL::SvrRev(int nNO,CString& strSvrRev)
{
	mSvrRev(nNO,strSvrRev);
}

void CImgDLL::WriteLog(const char *fmt, ...)
{
	va_list args;
	char rt_log_buf[0xFFFF];
	va_start(args, fmt);
	vsprintf_s(rt_log_buf, fmt, args);
	va_end(args);
	CString strLog;
	strLog.Format("%s", rt_log_buf);
	mWriteLog(strLog);
}

void* CImgDLL::LogDlg()
{
	return (void*)mLogDlg();
}

void* CImgDLL::ImgDlg(CString strCam, CString strXML /* = NULL */)
{
	return (void*)mImageDlg(strCam, NULL);
}

bool CImgDLL::InitCam(int nCam)
{
	bool bRet = mInitCam(nCam);
	return bRet;
}

bool CImgDLL::LoadXML(int nID, CString strXML, int& nExp, float& fGain, float& fGam)
{
	bool bLoadXML = mLoadXML(nID, strXML, nExp, fGain, fGam);
	return bLoadXML;
}

bool CImgDLL::SetCamPar(int nCam, float fExp, float fGain, float fGam)
{
	bool bSet = mSetCamPar(nCam, fExp, fGain, fGam);
	return bSet;
}

bool CImgDLL::UnLoadXML(int nID)
{
	bool bUnLoad = mUnLoadXML(nID);
	return bUnLoad;
}

void CImgDLL::Snap(CHWin& HW, int nCam, int nTimes)
{
	mSnap(HW, nCam, nTimes);
}

bool CImgDLL::SafeSnap(CHWin& HW, int nCam, int nTimes)
{
	bool bFailSnap = true;
	HW.bCam = false;
	for (int i = 0; i < 5; i++)
	{
		CImgDLL::Snap(HW, nCam, nTimes);      //拍一张照片触发尺寸
		if (HW.bLoad)
		{
			bFailSnap = false;
			HW.bCam = true;
			break;
		}
		mWriteLog("CAM%d 重新连接--%d！", nCam + 1, i + 1);
		CImgDLL::CloseCam(nCam);
		CImgDLL::InitCam(nCam);
	}
	return bFailSnap;
}

bool CImgDLL::SetHeartTime(int nCam, UINT uTime)
{
	return mSetHeartTime(nCam, uTime);
}

bool CImgDLL::CloseCam(int nCam)
{
	return mCloseCam(nCam);
}

bool CImgDLL::IsImageOK(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut)
{
	return mIsImageOK(HW, nID, bLogNG, psOut);
}

void* CImgDLL::GetCamCfgDlg(int nCam)
{
	CIDlgCamCfg* pDlgCam = mCamCfg(nCam);
	return (void*)pDlgCam;
}

void CImgDLL::ClearSvrRev(int nNO)
{
	mSvrClr(nNO);
}