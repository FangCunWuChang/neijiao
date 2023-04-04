#pragma once
#include "MvCameraControl.h"
#include "TCPSocket.h"
#include "Log.h"
#include "ADODB.h"
#ifndef SINGLETON_H
#define SINGLETON_H

#define ESP 0.00001

//#include <vector>
////����vector����ṹ����һά�ַ�������
//typedef ::std::vector<CString> CStrs;
////����vector����ṹ�����ά�ַ�������
//typedef ::std::vector<CStrs> CDStrs;
////����vector����ṹ������ά�ַ�������
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
#define JDW_I2O_FIRST   1   //���ڶ��⣬��һ���ߣ������ɳ���λ�ж�Բ�׵���ɫ���ǰ�ɫ���Ǻ�ɫ
#define JDW_O2I_FIRST   2   //������ڣ���һ����
#define JDW_I2O_LAST     3   //���ڶ��⣬���һ����
#define JDW_O2I_LAST     4   //������ڣ����һ����
#define JDW_I2O_MAX      5    //���ڶ��⣬����ݶȱ�
#define JDW_O2I_MAX      6    //������ڣ�����ݶȱ�
#define JDW_W_LNU         7    //��ɫ��������
#define JDW_B_LNU           8   //��ɫ�����������漰�������Ŀ�ȣ����ȣ��Ƕ�
#define JDW_TWO_LINE   9    //��������ֱ�߽���
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
#define TMP_NCC        0                    //NCCģ��
#define TMP_SHM        1                    //Shapeģ��ֻ��ԭ��



#define JDWPR_DX    0
#define JDWPR_DY     1
#define JDWPR_MW   2
#define JDWPR_MH    3
#define JDWPR_THRES  4
#define JDWPR_MINR      5
#define JDWPR_MAXR     6
#define JDWPR_ANGST   7
#define JDWPR_RANGE   8    //�Ƕȷ�Χ
#define JDWPR_RADIUS  9    //����λ�뾶

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


struct CListStyle               //�б���Ϣ
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
	double fNum[6][2];      //ÿ����վ����������
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
	CString sNick, sSig, sVal,sRet;         //�ź�����,�ź�Ƭ�Σ��ź�ֵ,
	int iWAT,iType,iStatus;              //��������룬�ȴ��źŲ�ѯʱ�䣬����������������ִ����ɺ󣬵ȴ�ʱ��,�ź�����
	int iAddr,iRes[8];                        //�źŵ�ַ,�źŽ��
	bool bRev;                                 //����ź�Ҫȡ����== sVal ����û�źţ�!= sVal �������ź�
	bool bNeg;                                //����ź��Ƿ�����
	DWORD dwTime;                    // �źŴ���ʱ��
	double fVal;                              //�ź�ֵ
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
		fRealRC[3] = 3800;      //Ĭ��1000���������
		strNameTmp.Empty();
		fR = 0;
		iMod = -1;
		nJDW = 0;
	}
	void ClearMod()                                          //���ģ��
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
					str.Format("ģ���ͷ��쳣��%s", pc);
					LogFile(str);
				}
			}
			nM[i] = -1;
		}
	}
	int iMod,nImgSize[2];                                            //������ʾͼ������ߴ�
	CString strNameTmp;                                          //ģ������ �� ģ������
	HTuple hvM[MAX_TM];                                        //��׼ģ��ͱ���ģ��
	int nM[MAX_TM],nJDW;
	double fRealRC[4], fPar[MAX_TM][5],fR;                        //ģ��������,������,0~1ģ��ԭ�����NCC���ĵ��ƫ�2~3ģ��ͼ����,4��ֵ��;(����λ�׵İ뾶),
	double fCenDirLH[5], fScore, fJDWLH[2][5];     //fJDWLH �Ǿ���λ������ȡ����ĳ���Ͷ�λ��ĽǶ�,ģ��ı�׼�����
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
	CString strNick;                      //�������
	CString strMN;                        //�õ��������Ϻ� ���߽ڵ��ֵ
	vector<CSignal> sgOut;         //�������ź�
	int iPrNO,iVal;                           //���������ǲ�Ʒ���յ�λ����ô������Ǽ��ŵ�0,
	CTMP TM[2];                          //һ�������ڵ������������ģ�壬��λ��ͷ����
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
	int nPosPer[2];                         //�����ڴ�������ʾ�İٷֱ�
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
		SetDraw(WndID, "margin");                                                                                                                          //Ĭ����ֻ��ʾ��Ե
		SetColor(WndID, "green");
	}
	void InitWnd(CWnd *pWnd, double fW, double fH, CString strColor, int nLineW = 2,int nX = 0, int nY = 0)
	{
		try
		{
			if (WndID != 0)
				HalconCpp::CloseWindow(WndID);
			SetWindowAttr("background_color", "gray");
			pWnd->GetClientRect(&rcWin);                                         //ID1Ϊ��һ��ͼ��ؼ���ID�ţ��ڶ����������ƴ���  
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
		catch (HalconCpp::HOperatorException &q)                                                     //�ɼ������쳣
		{
			CString str;
			const char* pc = q.ErrorMessage().Text();
			str.Format("�����趨�쳣��%s", pc);
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
		//LoadText(nPos, "ԭʼͼ");
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
	CString strType;             //�ڵ㹤������
	CTMP TMs;                    //ͼ��ģ������
	vector<CPos> psLeaf;   //Ҷ�ӽڵ�
	int nOTP;                          //�������
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
	//static int GetLetterValue(char C)//�ַ���ת����ֵ
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
	//	int i = C - 48;//0��ASCII����48
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
	//	int iVal = atoi(strAddr.Left(i));                             //����������ֵ�����
	//	CString strOver;
	//	strOver.Format(_T("%04x"),iVal);                     //ת����16���Ƶ�����   
	//	Addrs[0] = _tcstoul(strOver.Left(2),0,16);
	//	Addrs[1] = _tcstoul(strOver.Mid(2,2),0,16);
	//	Addrs[2] = 0;
	//	if (i < strAddr.GetLength())
	//	{
	//		iVal = atoi(strAddr.Right(strAddr.GetLength() - i - 1));
	//		Addrs[2] = iVal%256;//�������256���Ǵ��������,�����256ȡ�����
	//	}
	//}
	void CreateDir(CString strDir)
	{
		if( CreateDirectory(strDir, NULL) ) //�ж��Ƿ���ڣ����򴴽�
		{
			SetFileAttributes(strDir,FILE_ATTRIBUTE_DIRECTORY); //����(����Ϊ����)
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
	//������Ӿ��μ������ָ����Ľǵ�,��Զ����300������
	bool GetRectCorner(HTuple& WndID,HObject& hoRect,double& fR,double& fC,double fDist = 300,bool bShow = true);
	//����λԲ�ĵ㣬����Բ����ģ������ĵ����� 
	bool GetCircleTmpXY(HTuple& WndID,HObject& hoPic, double fCen[2], double& fR, double fPar[9],int nJDW);
	void UpdateMarginPar()
	{
		_iLen[0] = GetCfgInt("��Ե��", "L1", 30);
		_iLen[1] = GetCfgInt("��Ե��", "L2", 90);
		_iGray = GetCfgInt("��Ե��", "Gray", 30);
		_iDev = GetCfgInt("��Ե��", "Dev", 35);
	}
	//���ݱ�Ե�ݶȣ������б�Ե���ݶ����ģ���ֵ��Xs��Ys
	void GetMaxAmpPt(HTuple& hvAmplitude, HTuple& hvRowEdge, HTuple &hvColEdge, HTuple& hvXs, HTuple &hvYs, int& n, double fDir, HObject& hoImage);
	int GetCfgInt(CString strSec, CString strKey, int nDef = 0)
	{
		int II = GetPrivateProfileInt(strSec, strKey, nDef, _strINI);
		return II;
	}
	//���ݾ���λ�������¼��㶨λ������
	bool GetJDWPos(HTuple& WndID, HObject& hoImage, HTuple& hvMat2D, double fPar[10], CTMP& tm, bool bShow = false);
	//���ݱ߽�㼯��ƽ���ҶȺͷ����ų������ʵĵ�,fL[0] �����򳤶ȣ�fL[1] ���
	bool FilterPts(HTuple& WndID, HObject& hoImage, HTuple& Ys, HTuple& Xs, double fDir, int iLen[2]);
	//��ǰģ����û������ģ�巽��,����ģ����Ϣ��ȷ��
	int GetTmpDirOrgJDW(CString strNCC, CStrs& Vs);
	//��ʼ���б���Ϣ
	void InitList(CListCtrlCl& m_list, vector<CListStyle>& Style);
	//��ѯ��sec���ֶ����ж������� strs��ʼ����յ� 
	void GetSecKeys(CString strSec, CStringArray& strs);
	//ר�����ģ��Բ����ȡ
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
	//�����ȥ�ֶ�λ�㣬���ؾ���λ�㣬fPar[2],fPar[3] ������ʼ�����Ƕ�
	bool GetDirPoint(HTuple& WndID, HObject& hoImage, double& fY, double& fX, double fRC[2],double fPar[4],int nJDW = 0);
	//�õ���ˮƽ��н�,������ǻ�е���꣬
	//bImage = true ˵����е����ϵ����ͨ����תͼ������ϵ�ĵõ�һ�·���
	//bImage = false ˵�����ܵõ�һ�����귽��
	double GetHorzAngle(double fAxisPt[2], double fVecPt[2], bool bImage = false, bool bRad = true);
	//���ݽڵ��ѯ
	bool GetLeafsValue(CTreeCtrl& tree,HTREEITEM hItem, int nLeaf, CStrs& sLeafs);
	//��ʼ��Ҷ�ڵ�
	bool LoadLeafs(int nID, int nStep, CString strFunc, int nSumLeaf, CTreeCtrl& tree, CStrs& sLeafs, HTREEITEM hItem);
	CString GetHTupleString(HTuple& hvRes);
	void GetAffineRect(double fRCVal[], CString strPosTYP, HTuple& hvRes, HTuple& hvTransX, HTuple& hvTransY);
	void GetDrawRect2(CHWin& HW, CTreeCtrl& tree, double fRCVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	bool GetDrawCircle(CHWin& HW, CTreeCtrl& tree, double fXYR[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	bool GetDrawRing(CHWin& HW, CTreeCtrl& tree, double fRadiusVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes);
	void SetupCallBack(unsigned char ** pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, int II);
	bool CamSnap(CHWin& HW, int nCam, int nTimes = 4);
	void FindAllFile(CString path, CStrs& filenames);
	//���Բ���㷨
	bool IsFitCircle(HTuple& hvRs, HTuple& hvCs, double XYR[3]);
	//�Ѳ�������д�뵽CSV�ĵ���
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
	HObject _hoImgOrg[80];                              //ÿ�������ԭʼͼ��
	vector<CXMLImage> _XMLImg[80];                 //ͼ���㷨����,ÿ��������10�ֹ�������
	int _nIPs;
	bool _bRun;
	int _iLen[2];                                                  //��Ե��ƽ���Ҷȳ��ȼ���
	int _iDev, _iGray;                                          //��Ե�㷽�����,�ҶȲ�
	CString _strINI;
	CString _strCalib;                                  //�궨�����ļ�
	vector<char> _chs;                               //�յ�������
	CString _strHostName,_strIP;
	UINT _iSvrPort;
	int _iAng;
	CStrs _Info;
	bool _bPLC;
	int _nRun,_nCCD,_nTimes;                   //nTimes��ʾ������ļ��Σ���Ҫ���2����CCD�����գ�����������
	double _fPixDist[13];                              //���ؾ���
	double _fCalibPos[2],_fVImg[4][2];        //�궨��Ʒ��λ��,���������������ڼ���Ƕȵ������,����� ������1��ʼ���ֱ���1��2��3
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
	int _nWnd;                                                //== 0 �����棬==1 ���������
	CSortStringArray _strsDia;                                //�׾�����
	CCriticalSection _csSvr, _csInfo, _csCam[CAM_NUM], _csPLC, _csClient[8], _csIP, _csTV;
	CHSCommPort _Lasr,_PLC;
	double _fFCSnap[2][4];                                   //�������յ��е���꣬ÿ�������ĸ�����
	HObject _hoImg[CAM_NUM];
	int _iReal[CAM_NUM];
	int _iSnap[CAM_NUM];
	int _nCamThread;                                        //�������������
	CString _strSnd[3];                                 //ÿ�������ÿһ��XML��һ�����
	BOOL _bNewIP[9];                                        //ÿһ��������IP��ַ��λ�ź�
	CPos _RotAxs[5],_PosST[5][2],_PosBZ[3];                   //�궨������ת��,�ͱ�׼λ��
	CPTCALI _psCali[2];                                     //�궨��λ��
	bool _bRes;
	CString strLiaohao;
};

#endif