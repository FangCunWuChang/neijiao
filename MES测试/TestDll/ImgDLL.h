#pragma once
#include "stdafx.h"
#include "..\DllMod\TCPSocket.h"
#include "..\DllMod\Singleton.h"

class CImgDLL
{
public:
	CImgDLL();
	~CImgDLL();
	static BOOL Init(void);   //³õÊ¼»¯Í¼ÏñDLL
	static bool SvrLink(int iSvrPort);
	static int SvrLinkStatus(int nNO);
	static int SvrSnd(int nNO, char* data, int nLen);
	static bool SvrStop();
	static void SvrRev(int nNO,CString& strSvrRev);
	static void ClearSvrRev(int nNO);
	static void WriteLog(const char *fmt, ...);
	static void* LogDlg();
	static void* ImgDlg(CString strCam, CString strXML = NULL);
	static bool InitCam(int nCam);
	static bool CloseCam(int nCam);
	static bool LoadXML(int nID, CString strXML, int& nExp, float& fGain, float& fGam);
	static bool SetCamPar(int nCam, float fExp, float fGain, float fGam);
	static bool UnLoadXML(int nID);
	static void Snap(CHWin& HW, int nCam, int nTimes);
	static bool IsImageOK(CHWin& HW, int nID, bool bLogNG, vector<CPos>& psOut);
	static void* GetCamCfgDlg(int nCam);
	static bool SetHeartTime(int nCam, UINT uTime);
	static bool SafeSnap(CHWin& HW, int nCam, int nTimes);
};

