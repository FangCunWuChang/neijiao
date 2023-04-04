#pragma once
#include "MvCameraControl.h"
#include "TCPSocket.h"
#include "Log.h"
#include "ADODB.h"
#ifndef SINGLETON_H
#define SINGLETON_H

#define ESP 0.00001

//#include <vector>
////利用vector数组结构构造一维字符串数组
//typedef ::std::vector<CString> CStrs;
////利用vector数组结构构造二维字符串数组
//typedef ::std::vector<CStrs> CDStrs;
////利用vector数组结构构造三维字符串数组
//typedef ::std::vector<CDStrs> CTStrs;

#include "ListCtrlCl.h"
#include "HSCommPort.h"

#include <HalconCpp.h>
using namespace HalconCpp;

#include <vector>
using namespace std;


extern void LogFile(CString log);
extern void LogFile(const char *fmt, ...);
extern void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);
extern void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);


#define JDW_NULL             0
#define JDW_I2O_FIRST   1   //由内而外，第一条边，可以由初定位判断圆孔的颜色，是白色还是黑色
#define JDW_O2I_FIRST   2   //由外而内，第一条边
#define JDW_I2O_LAST     3   //由内而外，最后一条边
#define JDW_O2I_LAST     4   //由外而内，最后一条边
#define JDW_I2O_MAX      5    //由内而外，最大梯度边
#define JDW_O2I_MAX      6    //由外而内，最大梯度边
#define JDW_W_LNU         7    //白色长条轮廓
#define JDW_B_LNU           8   //黑色长条轮廓，涉及到长条的宽度，长度，角度
#define JDW_TWO_LINE   9    //任意两条直线交点
#define JDW_CONS_C      12
#define JDW_CONS_L       13
#define JDW_CONS_U      14
#define JDW_CONS_R       15
#define JDW_CONS_D       16

#define ZOOM 0.25
#define CAM_NUM 4
#define TIM_INI   1
#define TIM_LOG   2
#define TIM_REAL  3
#define TIM_PLC   4
#define TIM_SND   5
#define TIM_LNK  6
#define TIM_LEFTS 7
#define TIM_LEFTE 8
#define TIM_RIGHTS 9
#define TIM_RIGHTE 10
#define TIM_END 11
#define TIM_MES 12

#define TIME_IP1 1
#define TIME_IP2 2
#define TIME_IP3 3
#define TIME_VAL1 4
#define TIME_VAL2 5
#define TIME_VAL3 6
#define TIME_MES 7
#define TIME_ZERO1 8
#define TIME_ZERO2 9
#define TIME_ZERO3 10
#define TIME_LINK1 11
#define TIME_LINK2 12
#define TIME_LINK3 13
#define TIME_JZ1 14
#define TIME_JZ2 15
#define TIME_JZ3 16





#define RUN_NULL       0
#define RUN_LOAD      1
#define RUN_WORK     2
#define RUN_UNLOAD 3

#define CALIB_LOAD      4
#define CALIB_UNLOAD 5
#define CALIB_WORK     6

#define SIG_NULL  0
#define SIG_ON      1
#define SIG_CAL     2
#define SIG_OK       3
#define SIG_NG       4

#define RES_OK   1
#define RES_NG    2

#define ERR   -1

#define  MAX_TM 4
#define TMP_NCC        0                    //NCC模板
#define TMP_SHM        1                    //Shape模板只有原点



#define JDWPR_DX    0
#define JDWPR_DY     1
#define JDWPR_MW   2
#define JDWPR_MH    3
#define JDWPR_THRES  4
#define JDWPR_MINR      5
#define JDWPR_MAXR     6
#define JDWPR_ANGST   7
#define JDWPR_RANGE   8    //角度范围
#define JDWPR_RADIUS  9    //精定位半径

#define OUT_NULL 0
#define OUT_VAL    1
#define OUT_STR    2



#include <afx.h>
#include <afxcoll.h>
#include <iostream>

class CSortStringArray : public CStringArray
{
public:
	void CSortStringArray::Sort(bool bASC = true)
	{
		BOOL bNotDone = TRUE;
		while (bNotDone)
		{
			bNotDone = FALSE;
			for (int pos = 0; pos < GetUpperBound(); pos++)
				bNotDone |= CompareAndSwap(pos,bASC);
		}
	}
private:
	BOOL CSortStringArray::CompareAndSwap(int pos,bool bASC)
	{
		CString temp;
		int posFirst = pos;
		int posNext = pos + 1;

		if (GetAt(posFirst).CompareNoCase(GetAt(posNext)) > 0 && bASC)
		{
			temp = GetAt(posFirst);
			SetAt(posFirst, GetAt(posNext));
			SetAt(posNext, temp);
			return TRUE;
		}
		if (GetAt(posFirst).CompareNoCase(GetAt(posNext)) < 0 && bASC == false)
		{
			temp = GetAt(posFirst);
			SetAt(posFirst, GetAt(posNext));
			SetAt(posNext, temp);
			return TRUE;
		}
		return FALSE;
	}
};


struct CListStyle               //列表信息
{
	void Set(const char* s, int nL)
	{
		str.Format(_T("%s"), s);
		nLen = nL;
	}
	CString str;
	int nLen;
};

struct CWatch
{
	CWatch()
	{
		strCode.Empty();
		ZeroMemory(fDia, sizeof(fDia));
		ZeroMemory(fNum, sizeof(fNum));
		bHas = false;
		bOK = true;
	}
	bool bHas,bOK;
	CString strCode;
	double fDia[6];
	double fNum[6][2];      //每个工站的螺牙数量
};

struct CSignal
{
	CSignal()
	{
		sNick.Empty();
		sSig.Empty();
		iWAT = 0;
		sVal = "0";
		bRev = false;
		bNeg = false;
		dwTime = GetTickCount();
		fVal = 0;
		iStatus = SIG_NULL;
		sRet.Empty();
	}
	CString sNick, sSig, sVal,sRet;         //信号名称,信号片段，信号值,
	int iWAT,iType,iStatus;              //如果是输入，等待信号查询时间，如果是是输出，就是执行完成后，等待时间,信号类型
	int iAddr,iRes[8];                        //信号地址,信号结果
	bool bRev;                                 //这个信号要取反，== sVal 就是没信号，!= sVal 就是有信号
	bool bNeg;                                //这个信号是否屏蔽
	DWORD dwTime;                    // 信号触发时间
	double fVal;                              //信号值
};

struct  CTMP
{
	CTMP()
	{
		for (int i = 0; i < MAX_TM;i ++)
		{
			hvM[i] = HTuple();
			nM[i] = -1;
		}
		ZeroMemory(fPar, sizeof(fPar));
		ZeroMemory(fCenDirLH, sizeof(fCenDirLH));
		fScore = 0.7;
		fRealRC[0] = fRealRC[1] = 0;
		fRealRC[2] = 2700;
		fRealRC[3] = 3800;      //默认1000万像素相机
		strNameTmp.Empty();
		fR = 0;
		iMod = -1;
		nJDW = 0;
	}
	void ClearMod()                                          //清空模板
	{
		for (int i = 0; i < MAX_TM; i++)
		{
			if (nM[i] >= 0)
			{
				try
				{
					if (nM[i] == TMP_NCC)
						ClearNccModel(hvM[i]);
					else if (nM[i] == TMP_SHM)
						ClearShapeModel(hvM[i]);
				}
				catch (HalconCpp::HOperatorException &q)
				{
					CString str;
					const char* pc = q.ErrorMessage().Text();
					str.Format("模板释放异常：%s", pc);
					LogFile(str);
				}
			}
			nM[i] = -1;
		}
	}
	int iMod,nImgSize[2];                                            //窗口显示图像的最大尺寸
	CString strNameTmp;                                          //模板名称 和 模板类型
	HTuple hvM[MAX_TM];                                        //标准模板和备份模板
	int nM[MAX_TM],nJDW;
	double fRealRC[4], fPar[MAX_TM][5],fR;                        //模板搜索区,无缩放,0~1模板原点相对NCC中心点的偏差，2~3模板图长宽,4二值化;(精定位孔的半径),
	double fCenDirLH[5], fScore, fJDWLH[2][5];     //fJDWLH 是精定位的线提取区域的长宽和定位框的角度,模板的标准方向角
};

struct CPos
{
	CPos(double X, double Y)
	{
		x = X;
		y = Y;
		fVec = 20;
		fVal = 0;
		iVal = -1;
		fAcc = 1;
		ZeroMemory(XYZR, sizeof(XYZR));
		sgOut.clear();
		iPrNO = -1;
	}
	CPos()
	{
		ZeroMemory(XYZR, sizeof(XYZR));
		x = y = 0;
		fVec = 20;
		fAcc = 1;
		sgOut.clear();
		iPrNO = -1;
		iVal = -1;
		fVal = 0;
	}
	double x, y, z, r;
	double fVec, fAcc,fVal,fPos[3];
	int XYZR[4];
	CString strNick;                      //点的名称
	CString strMN;                        //该点所属的料号 或者节点的值
	vector<CSignal> sgOut;         //多个输出信号
	int iPrNO,iVal;                           //如果这个点是产品拍照点位，那么这个点是几号点0,
	CTMP TM[2];                          //一个工作节点最多两种类型模板，定位点和方向点
};

struct CPTCALI
{
	CPTCALI()
	{
		//ZeroMemory(fAxs, sizeof(fAxs));
		//ZeroMemory(fImg, sizeof(fImg));
		bLoad = false;
	}
	bool bLoad;
	CPos fAxs[9];
	CPos fImg[9];
};

struct CWinText
{
	CWinText()
	{
		nPosPer[0] = nPosPer[1] = 1;
		nFontSize = 25;
		strTxt.Empty();
		strColo = "yellow";                     //'black', 'blue', 'yellow', 'red', 'green', 'cyan', 'magenta', 'forest green', 'lime green', 'coral', 'slate blue'
	}
	int nPosPer[2];                         //文字在窗口上显示的百分比
	int nFontSize;
	CString strTxt,strColo;
};

struct CHWin
{
	CHWin()
	{
		bLoad = false;
		psLU.y = psLU.x = 0;
		WndID = NULL;
		psRD.y = 3648;
		psRD.x = 5472;
		hoImage.GenEmptyObj();
		rcWin.left = rcWin.top = 0;
		rcWin.right = rcWin.bottom = 100;
		bCam = false;
		WinTexts.clear();
	}
	void ShowText()
	{
		for (int i = 0; i < (int)WinTexts.size(); i++)
		{
			SetDisplayFont(WndID, WinTexts[i].nFontSize, "mono", "true", "true");
			CString strTxt,strColo;
			strColo.Format("%s", WinTexts[i].strColo);
			strTxt.Format("%s", WinTexts[i].strTxt);
			double fY = 0.01*WinTexts[i].nPosPer[1] * rcWin.Height();
			double fX = 0.01*WinTexts[i].nPosPer[0] * rcWin.Width();
			DispMessage(WndID,strTxt.GetBuffer(), "window",fY,fX , strColo.GetBuffer(), "false");
			//DispLine(WndID, 0, 0, 900, 900);
			strColo.ReleaseBuffer();
			strTxt.ReleaseBuffer();
		}
	}
	void LoadText(int nPosTxt[2], CString strTxt = NULL, int nFontSize = 25, CString strColo = "yellow")
	{
		if (strTxt.GetLength() > 0)
		{
			CWinText WT;
			WT.nFontSize = nFontSize;
			WT.nPosPer[0] = nPosTxt[0];
			WT.nPosPer[1] = nPosTxt[1];
			WT.strColo.Format("%s", strColo);
			WT.strTxt.Format("%s", strTxt);
			WinTexts.push_back(WT);
		}
	}
	void InitWnd(CWnd *pWnd)
	{
		pWnd->GetClientRect(&rcWin);
		if (WndID != 0)
			HalconCpp::CloseWindow(WndID);
		SetWindowAttr("background_color", "gray");
		WinTexts.clear();
		OpenWindow(0, 0, rcWin.Width(), rcWin.Height(), (INT)pWnd->m_hWnd, "visible", "", &WndID);
		SetDraw(WndID, "margin");                                                                                                                          //默认是只显示边缘
		SetColor(WndID, "green");
	}
	void InitWnd(CWnd *pWnd, double fW, double fH, CString strColor, int nLineW = 2,int nX = 0, int nY = 0)
	{
		try
		{
			if (WndID != 0)
				HalconCpp::CloseWindow(WndID);
			SetWindowAttr("background_color", "gray");
			pWnd->GetClientRect(&rcWin);                                         //ID1为第一个图像控件的ID号，第二个窗口类似处理。  
			HWND hImgWnd = pWnd->m_hWnd;
			Hlong MainWndID = (Hlong)hImgWnd;
			double fWCoe = fW / rcWin.Width();
			double fHCoe = fH / rcWin.Height();
			WinTexts.clear();
			if (fWCoe > fHCoe)
			{
				HalconCpp::OpenWindow(0, 0, rcWin.Width(), rcWin.Height()*fHCoe / fWCoe, MainWndID, "", "", &WndID);
			}
			else
			{
				HalconCpp::OpenWindow(0, 0, rcWin.Width()*fWCoe / fHCoe, rcWin.Height(), MainWndID, "", "", &WndID);
			}
			SetPart(WndID, 0, 0, fH, fW);
			SetColor(WndID, strColor.GetBuffer(0));
			strColor.ReleaseBuffer();
			SetLineWidth(WndID, nLineW);
			SetDraw(WndID, "margin");
		}
		catch (HalconCpp::HOperatorException &q)                                                     //采集窗口异常
		{
			CString str;
			const char* pc = q.ErrorMessage().Text();
			str.Format("窗口设定异常：%s", pc);
			//LogFile(str);
		}
	}
	void ShowImage(int nPosTxt[2],CString strTxt = NULL, int nFontSize = 25, CString strColo = "yellow", bool bClear = true)
	{
		if (WndID == 0)
			return;
		if (bClear)
		{
			SetSystem("flush_graphic", "false");
			ClearWindow(WndID);
			SetSystem("flush_graphic", "true");		
		}
		SetPart(WndID, psLU.y, psLU.x, psRD.y, psRD.x);
		if (bLoad && bClear)
		{
			DispObj(hoImage, WndID);
		}
		LoadText(nPosTxt, strTxt, nFontSize, strColo);
		ShowText();
	}
	void ShowImage(HObject& hoPic, int nPosTxt[2], CString strTxt = NULL, int nFontSize = 25, CString strColo = "yellow", bool bShow = true)
	{
		if (WndID == 0)
			return;
		CopyImage(hoPic, &hoImage);		
		bLoad = true;
		if (bShow == true)
		{
			SetPart(WndID, psLU.y, psLU.x, psRD.y, psRD.x);
			DispObj(hoImage, WndID);
		}
		LoadText(nPosTxt, strTxt, nFontSize, strColo);
		ShowText();
	}
	void ShowImage(HObject& hoPic,bool bClearText = true)
	{
			if (WndID == 0)
				return;
			bLoad = true;
			CopyImage(hoPic, &hoImage);
			SetPart(WndID, psLU.y, psLU.x, psRD.y, psRD.x);
			DispObj(hoImage, WndID);
			if (bClearText)
			{
				WinTexts.clear();
			}
			ShowText();
	}
	void ShowImage(bool bShow = true)
	{
		if (WndID == 0)
			return;
		ClearWindow(WndID);
		SetPart(WndID, psLU.y, psLU.x, psRD.y, psRD.x);
		if (bLoad && bShow)
		{
			DispObj(hoImage, WndID);
		}
		ShowText();
	}
	void RealSize(CString strColo = "yellow",bool bShow = true)
	{
		HTuple hvNum;
		CountObj(hoImage, &hvNum);
		if (hvNum.I() == 0)
			return;
		HTuple hvW, hvH;
		GetImageSize(hoImage, &hvW, &hvH);
		double ratioWidth = (1.0) *hvW.D()  / rcWin.Width();
		double ratioHeight = (1.0) *hvH.D()  /rcWin.Height() ;
		double fImage = hvW.D() / hvH.D();
		double fWindow = (double)rcWin.Width() / rcWin.Height();
		double Row0 = 0, Col0 = 0, Row1 = hvH.D() - 1, Col1 = hvW.D() - 1;
		if (fWindow >= fImage)
		{
			double w = fWindow * hvH.D();
			Row0 = 0;
			Col0 = -(w - hvW.D()) / 2;
			Row1 = hvH.D() - 1;
			Col1 = hvW.D() + (w - hvW.D()) / 2;
		}
		else
		{
			double h = hvW / fWindow;
			Row0 = -(h - hvH.D()) / 2;
			Col0 = 0;
			Row1 = hvH.D() + (h - hvH.D()) / 2;
			Col1 = hvW.D() - 1;
		}
		psLU.y = Row0;
		psLU.x = Col0;
		psRD.y = Row1;
		psRD.x = Col1;
		//int nPos[2] = { 10, 10 };
		//LoadText(nPos, "原始图");
		//int nPosTxt[2] = { 70, 10 };
		//ShowImage(nPosTxt, "OK", 60, "green", false);
		if (bShow)
			ShowImage(bShow);
	}
	HTuple WndID;
	HObject hoImage;
	bool bLoad,bCam;
	CPos psLU, psRD;
	vector<CWinText> WinTexts;
	CRect rcWin;
};

struct CXMLImage
{
	CXMLImage()
	{
		nOTP = OUT_NULL;
	}
	CString strType;             //节点工作内容
	CTMP TMs;                    //图像模板名称
	vector<CPos> psLeaf;   //叶子节点
	int nOTP;                          //输出类型
};

class CSingleton
{
public:
	CSingleton(void);
	static CSingleton* GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CSingleton;
		}
		return m_pInstance;
	}
	~CSingleton(void);
	static void CSingleton::DeleteInstance();
	//static int GetLetterValue(char C)//字符串转化成值
	//{
	//	if (C == 'A' || C == 'a')
	//		return 10;
	//	if (C == 'B' || C == 'b')
	//		return 11;
	//	if (C == 'C' || C == 'c')
	//		return 12;
	//	if (C == 'D' || C == 'd')
	//		return 13;
	//	if (C == 'E' || C == 'e')
	//		return 14;
	//	if (C == 'F' || C == 'f')
	//		return 15;
	//	int i = C - 48;//0的ASCII码是48
	//	if (i > 9 || i < 0)
	//		return -1;
	//	return i;
	//}
	//static int Hex2int(CString strVal)
	//{
	//	int iLen = strVal.GetLength();
	//	int cVal = 0;
	//	for (int i = 0;i < iLen;i++)
	//	{
	//		cVal += (int)(GetLetterValue(strVal.GetAt(i))*pow(16.0,iLen - i - 1) + 0.5);
	//	}
	//	return cVal;
	//}
	//static void ConvertAddress(CString strAddr,char* Addrs)
	//{
	//	int i = 0;
	//	for (;i < strAddr.GetLength();i++)
	//	{
	//		if (strAddr.GetAt(i) == '.')
	//			break;
	//	}	
	//	int iVal = atoi(strAddr.Left(i));                             //获得整数部分的数字
	//	CString strOver;
	//	strOver.Format(_T("%04x"),iVal);                     //转化成16进制的数据   
	//	Addrs[0] = _tcstoul(strOver.Left(2),0,16);
	//	Addrs[1] = _tcstoul(strOver.Mid(2,2),0,16);
	//	Addrs[2] = 0;
	//	if (i < strAddr.GetLength())
	//	{
	//		iVal = atoi(strAddr.Right(strAddr.GetLength() - i - 1));
	//		Addrs[2] = iVal%256;//如果大于256就是错误的数据,这里对256取余好了
	//	}
	//}
	void CreateDir(CString strDir)
	{
		if( CreateDirectory(strDir, NULL) ) //判断是否存在，否则创建
		{
			SetFileAttributes(strDir,FILE_ATTRIBUTE_DIRECTORY); //隐藏(设置为隐藏)
		}
	}
	void CheckNumber(CString& str);
	static int SplitString(const CString &csInput, const CString &csDelimiter,CStringArray &aResults);
	static int SplitString(const CString &csInput, const CString &csDelimiter, CStrs &aResults);
	CString LinkString(CStrs & ss, CString strLink,bool bTrim = true);
	CString GetCfgString(CString strSec,CString strKey,CString strDef,int nLen = 64)
	{
		CString strOffset;
		GetPrivateProfileString(strSec,strKey,strDef,strOffset.GetBuffer(nLen),nLen,_strINI);
		strOffset.ReleaseBuffer();
		return strOffset;
	}
	void SetCfgString(CString strSec,CString strKey,CString strVal)
	{
		WritePrivateProfileString(strSec,strKey,strVal,_strINI);
	}
	void Write2CSV(CDStrs& DVs);
	double GetTwoPtDist(double* fOne,double* fTwo);
	double GetAngle(double fAxisPt[2],double fVecPt[2],bool bImage = false,bool bRad = true);
	bool GetFitLine(HTuple& WndID,HTuple& Ys,HTuple& Xs,HTuple& hv_RowA,HTuple& hv_ColA,HTuple& hv_RowB,HTuple& hv_ColB,double fMaxD = 8.5,bool bShow = false);
	bool GetFitLineAng(HTuple& WndID, HTuple& Ys, HTuple& Xs, double& fFitPhi, double fMaxD = 8.5, bool bShow = false);
	double GetVertAng(double fPhi, double fRange[2],bool bPlus = true);
	bool GetFitLineVal(HTuple& WndID,HTuple& Ys,HTuple& Xs,
		                            HTuple& hv_RowA,HTuple& hv_ColA,HTuple& hv_RowB,HTuple& hv_ColB,
		                        double &fVal,double fMaxD = 12,bool bFilter = false,bool bShow = true);
	//根据外接矩形计算最靠近指定点的角点,最远距离300个像素
	bool GetRectCorner(HTuple& WndID,HObject& hoRect,double& fR,double& fC,double fDist = 300,bool bShow = true);
	//精定位圆心点，更新圆轮廓模板的中心点坐标 
	bool GetCircleTmpXY(HTuple& WndID,HObject& hoPic, double fCen[2], double& fR, double fPar[9],int nJDW);
	void UpdateMarginPar()
	{
		_iLen[0] = GetCfgInt("边缘点", "L1", 30);
		_iLen[1] = GetCfgInt("边缘点", "L2", 90);
		_iGray = GetCfgInt("边缘点", "Gray", 30);
		_iDev = GetCfgInt("边缘点", "Dev", 35);
	}
	//根据边缘梯度，找所有边缘中梯度最大的，赋值给Xs，Ys
	void GetMaxAmpPt(HTuple& hvAmplitude, HTuple& hvRowEdge, HTuple &hvColEdge, HTuple& hvXs, HTuple &hvYs, int& n, double fDir, HObject& hoImage);
	int GetCfgInt(CString strSec, CString strKey, int nDef = 0)
	{
		int II = GetPrivateProfileInt(strSec, strKey, nDef, _strINI);
		return II;
	}
	//根据精定位类型重新计算定位点坐标
	bool GetJDWPos(HTuple& WndID, HObject& hoImage, HTuple& hvMat2D, double fPar[10], CTMP& tm, bool bShow = false);
	//根据边界点集的平均灰度和方差排除不合适的点,fL[0] 主方向长度，fL[1] 宽度
	bool FilterPts(HTuple& WndID, HObject& hoImage, HTuple& Ys, HTuple& Xs, double fDir, int iLen[2]);
	//当前模板有没有设置模板方向,或者模板信息正确吗？
	int GetTmpDirOrgJDW(CString strNCC, CStrs& Vs);
	//初始化列表信息
	void InitList(CListCtrlCl& m_list, vector<CListStyle>& Style);
	//查询【sec】字段下有多少内容 strs初始化清空的 
	void GetSecKeys(CString strSec, CStringArray& strs);
	//专门针对模板圆心提取
	bool GetCircleCen(HTuple& WndID, HObject& hoImage, double fCen[2], double fS = 7500, int nThres = 130, bool bShow = true);
	bool NineDotCalib(CPos ImgPts[9], CPos AxisPts[9], HTuple& HomMat2D);
	double ToDeg(double fPhi)
	{
		return fPhi * 180 / PI;
	}
	double ToPhi(double fDeg)
	{
		return fDeg*PI / 180;
	}
	bool GetDirPoint(HTuple& WndID, HObject& hoImage, double fY, double fX, double fPhi, double fLen[2],double fVec[2]);
	//传入进去粗定位点，返回精定位点，fPar[2],fPar[3] 传入起始搜索角度
	bool GetDirPoint(HTuple& WndID, HObject& hoImage, double& fY, double& fX, double fRC[2],double fPar[4],int nJDW = 0);
	//得到和水平轴夹角,传入的是机械坐标，
	//bImage = true 说明机械坐标系可以通过旋转图像坐标系的得到一致方向，
	//bImage = false 说明不能得到一致坐标方向
	double GetHorzAngle(double fAxisPt[2], double fVecPt[2], bool bImage = false, bool bRad = true);
	//根据节点查询
	bool GetLeafsValue(CTreeCtrl& tree,HTREEITEM hItem, int nLeaf, CStrs& sLeafs);
	//初始化叶节点
	bool LoadLeafs(int nID, int nStep, CString strFunc, int nSumLeaf, CTreeCtrl& tree, CStrs& sLeafs, HTREEITEM hItem);
	CString GetHTupleString(HTuple& hvRes);
	void GetAffineRect(double fRCVal[], CString strPosTYP, HTuple& hvRes, HTuple& hvTransX, HTuple& hvTransY);
	void GetDrawRect2(CHWin& HW, CTreeCtrl& tree, double fRCVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	bool GetDrawCircle(CHWin& HW, CTreeCtrl& tree, double fXYR[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	bool GetDrawRing(CHWin& HW, CTreeCtrl& tree, double fRadiusVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	void SetupCallBack(unsigned char ** pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, int II);
	bool CamSnap(CHWin& HW, int nCam, int nTimes = 4);
	void FindAllFile(CString path, CStrs& filenames);
	//拟合圆的算法
	bool IsFitCircle(HTuple& hvRs, HTuple& hvCs, double XYR[3]);
	//把测量输入写入到CSV文档内
	void Write2CSV(int nCam,CStrs Vals);
	void Write2CSV(CString strIP, CString strVAL1, CString strVAL2, CString strVAL3);
	int ReadFromTCP(CString cStart, long lVal[4]);
	int Write2TCP(CString cStart, long lVal);
	//////////////////////////////////////20230225/////////////////////////////
	CString Str2Cstr(string str)
	{
		return CString(str.c_str());
	}
	string Cstr2Str(CString cstr)
	{
		string str = cstr.GetBuffer();
		cstr.ReleaseBuffer();
		return str;
	}
	//////////////////////////////////////////////////////////////////////////
protected:
	static CSingleton* m_pInstance;
public:	
	double _fZoom;
	HObject _hoImgOrg[80];                              //每个相机的原始图像
	vector<CXMLImage> _XMLImg[80];                 //图像算法流程,每个相机最多10种工作内容
	int _nIPs;
	bool _bRun;
	int _iLen[2];                                                  //边缘点平均灰度长度计算
	int _iDev, _iGray;                                          //边缘点方差计算,灰度差
	CString _strINI;
	CString _strCalib;                                  //标定配置文件
	vector<char> _chs;                               //收到的数据
	CString _strHostName,_strIP;
	UINT _iSvrPort;
	int _iAng;
	CStrs _Info;
	bool _bPLC;
	int _nRun,_nCCD,_nTimes;                   //nTimes表示下相机拍几次，主要针对2个下CCD横拍照，还是竖拍照
	double _fPixDist[13];                              //像素距离
	double _fCalibPos[2],_fVImg[4][2];        //标定产品的位置,单个胶条拍照用于计算角度的虚拟点,虚拟点 索引从1开始，分别是1，2，3
	double _fImgPr[6][2];                         
	CString _strPro;                  
	HTuple _MX[4];
	int m_nCHK[6];
public:
	CAdoDB _DB;
	CString _strRevRobot[3], _strCliRobot[3];
	CTCPSocket* _pSvr;
	bool _bINICam;
	void* _hCam[CAM_NUM];
	int _iCam[CAM_NUM];
	CWnd* _pMain,*_pLog,*_pCamCfg,*_pJob;
	int _nWnd;                                                //== 0 主界面，==1 单相机界面
	CSortStringArray _strsDia;                                //孔径数组
	CCriticalSection _csSvr, _csInfo, _csCam[CAM_NUM], _csPLC, _csClient[8], _csIP, _csTV;
	CHSCommPort _Lasr,_PLC;
	double _fFCSnap[2][4];                                   //两个拍照点机械坐标，每个坐标四个参数
	HObject _hoImg[CAM_NUM];
	int _iReal[CAM_NUM];
	int _iSnap[CAM_NUM];
	int _nCamThread;                                        //启动的相机数量
	CString _strSnd[3];                                 //每个相机的每一个XML有一个结果
	BOOL _bNewIP[9];                                        //每一个联网的IP地址就位信号
	CPos _RotAxs[5],_PosST[5][2],_PosBZ[3];                   //标定出来的转轴,和标准位置
	CPTCALI _psCali[2];                                     //标定点位置
	bool _bRes;
	CString strLiaohao;
};

#endif