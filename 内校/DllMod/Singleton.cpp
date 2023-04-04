#include "stdafx.h"
#include "..\DllMod\Log.h"
#include "..\DllMod\Singleton.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgJob.h"
#include "..\DllMod\IDlgInfo.h"
#include "..\DllMod\IDlgCamCfg.h"

extern int g_nExt;
extern void DoEvent();

CSingleton* CSingleton::m_pInstance = NULL; // 这句必须有.

void LogFile(CString log)
{
	Log mainLog;
	mainLog.CommonLogInit();//生成文件
	mainLog << mainLog.GetTimeStr() << log.GetBuffer(0) << endl;
	log.ReleaseBuffer();
	CSingleton* pSng = CSingleton::GetInstance();
	CString strS;
	CTime tm = CTime::GetCurrentTime();
	CString strDate;
	strDate = tm.Format("%Y-%m-%d %H:%M:%S");
	strS.Format(_T("[%s] %s"), strDate, log);
	pSng->_csInfo.Lock();
	pSng->_Info.push_back(strS);
	pSng->_csInfo.Unlock();
}

void LogFile(const char *fmt, ...)
{
	va_list args;
	char rt_log_buf[0xFFFF];

	va_start(args, fmt);
	vsprintf_s(rt_log_buf, fmt, args);
	va_end(args);

	Log mainLog;
	mainLog.CommonLogInit();//初始化
	mainLog << mainLog.GetTimeStr() << rt_log_buf << endl;
	CSingleton* pSng = CSingleton::GetInstance();
	CString strS, str(mainLog.GetTimeStr().c_str());
	CTime tm = CTime::GetCurrentTime();
	CString strDate;
	strDate = tm.Format("%Y-%m-%d %H:%M:%S");
	strS.Format(_T("[%s] %s"), strDate, rt_log_buf);
	pSng->_csInfo.Lock();
	pSng->_Info.push_back(strS);
	pSng->_csInfo.Unlock();
}

CSingleton::CSingleton(void)
{
	_nIPs = 0;
	_strINI = "D:\\INI\\FUNC.ini";
	_iSvrPort = GetCfgInt("参数","服务器端口",2000);
   _strIP.Format("192.168.59.210");   //三菱PLC IP地址
   _strPro.Empty();
   _nRun = RUN_NULL;
   _nWnd = 0;
   _fZoom = atof(GetCfgString("参数", "ZOOM", "0.25"));
   _nCamThread = 0;
   for (int i = 0; i < CAM_NUM;i++)
   {
	   _hCam[i] = NULL;
   }
   _bINICam = false;
   _pSvr = NULL;
   for (int i = 0; i < 2; i++)   //拟合两个底板的圆心
   {
	   CString strRot;
	   strRot.Format("CAM%d转轴",i + 2);
	   CStringArray ss;
	   GetSecKeys(strRot, ss);
	   if (ss.GetCount() < 3)
		   continue;
	   HTuple hvRs, hvCs;
	   hvRs = HTuple();
	   hvCs = HTuple();
	   int n = 0;
	   for (int j = 0; j < ss.GetCount();j++)
	   {
		   CString strPos = GetCfgString(strRot,ss[j],"");
		   CStrs XYs;
		   SplitString(strPos,",",XYs);
		   if (XYs.size() < 2)
			   break;
		   hvCs[n] = atof(XYs[0]);
		   hvRs[n] = atof(XYs[1]);
		   n++;
	   }
	   double fXYR[3];
	   if (IsFitCircle(hvRs,hvCs,fXYR) == false)
		   continue;
	   _RotAxs[i + 1].x = fXYR[0];
	   _RotAxs[i + 1].y = fXYR[1];                            //索引和相机编号走，转轴中心确定好了
	   _RotAxs[i + 1].iPrNO = 1;                               //转轴已经计算成功
	   //////////////////////////////////
	    CString strCam;
	    strCam.Format("CAM%d标准位",i + 1);
	    CString strST = GetCfgString("标准位置", strCam, "");
		CStrs sP;
		SplitString(strST, ",", sP);
		if (sP.size() < 3)
		  continue;
		 _PosBZ[i + 1].fPos[0] = atof(sP[0]);
		 _PosBZ[i + 1].fPos[1] = atof(sP[1]);
		 _PosBZ[i + 1].fPos[2] = atof(sP[2]);                    //标准位置的X，Y，R保存起来
		 _PosBZ[i + 1].iPrNO = 1;
   }
   for (int i = 0; i < 2; i++)
   {
	   CString strCam, strP;
	   strCam.Format("CAM%d标定", i + 2);		
	   CStringArray ssSec;
	   GetSecKeys(strCam, ssSec);
	   for (int j = 0; j < ssSec.GetCount(); j++)
	   {
		   CStrs sVal;
		   SplitString(ssSec[j], ",", sVal);
		   if (sVal.size() < 2) 
			   break;				   
		   _psCali[i].fAxs[j].x = atof(sVal[0]);
		   _psCali[i].fAxs[j].y = atof(sVal[1]);		   
		   CString strPos = GetCfgString(strCam, ssSec[j], "");
		   SplitString(strPos, ",", sVal);
		   _psCali[i].fImg[j].x = atof(sVal[0]);
		   _psCali[i].fImg[j].y = atof(sVal[1]);
		   if (j == 8)
			   _psCali[i].bLoad = true;
	   }
   }

   //HTuple HomMat2D, HomMat2DRot, hvTransY, hvTransX, hvMX, hvXST, hvYST;
   //HomMat2dIdentity(&HomMat2D);
   //double fAng[2] = { 95.49*PI / 180, -87.11*PI / 180 };
   ////fAng[0] = fAng[1] = 0;
   //int nCam = 1;
   //HomMat2dRotate(HomMat2D, fAng[nCam], _RotAxs[nCam].y, _RotAxs[nCam].x, &HomMat2DRot);
   //CPos ImgPts[9], AxisPts[9];
   //for (int j = 0; j < 9; j++)
   //{
	  // //CPos ptOne;
	  // //ptOne.x = _psCali[nCam].fImg[j].x;
	  // //ptOne.y = _psCali[nCam].fImg[j].y;
	  // ///////////////////旋转每个图像点得到新的图像点/////////////////////	
	  // //AffineTransPoint2d(HomMat2DRot, ptOne.y, ptOne.x, &hvTransY, &hvTransX);
	  // ImgPts[j].x = _psCali[nCam].fImg[j].x;
	  // ImgPts[j].y = _psCali[nCam].fImg[j].y;
	  // AxisPts[j].x = _psCali[nCam].fAxs[j].x;
	  // AxisPts[j].y = _psCali[nCam].fAxs[j].y;
   //}
   //NineDotCalib(ImgPts, AxisPts, hvMX);
   //AffineTransPoint2d(HomMat2DRot, _RotAxs[nCam].fPos[1], _RotAxs[nCam].fPos[0], &hvYST, &hvXST);
   //double fRotST[2];
   //fRotST[0] = hvXST.D();
   //fRotST[1] = hvYST.D();
   //AffineTransPoint2d(hvMX, hvYST, hvXST, &hvTransY, &hvTransX);
   //double fPosST[2];
   //fPosST[0] = hvTransX.D();
   //fPosST[1] = hvTransY.D();

   //AffineTransPoint2d(HomMat2DRot, _RotAxs[nCam].fPos[1]-1000, _RotAxs[nCam].fPos[0], &hvYST, &hvXST);
   //double fRotPos[2];
   //fRotPos[0] = hvXST.D();
   //fRotPos[1] = hvYST.D();
   //AffineTransPoint2d(hvMX, hvYST, hvXST, &hvTransY, &hvTransX);
   //double fPosCL[2];
   //fPosCL[0] = hvTransX.D();
   //fPosCL[1] = hvTransY.D();

   //得到每个孔径对应的相机号
   //GetSecKeys(_T("孔径"), _strsDia);    //孔径范围
   //_strsDia.Sort();
   //CString s = _strsDia.GetAt(0);
   //_strsDia.Add(s);                                  //如果配置文件没有孔径会报错
   //_strsDia.RemoveAt(0);

   //bool bOK = false;
   //vector<CPos> psVal;
   //CPos p;
   //CSignal s;
   //s.sVal = "0.96";
   //p.sgOut.push_back(s);
   //psVal.push_back(p);

   //CStrs Vals;
   //CString strCam, strTime;
   //strCam.Format("CAM%d", nCam + 1);
   //CTime tm = CTime::GetCurrentTime();
   //strTime = tm.Format("%H:%M:%S");
   //Vals.push_back(strCam);
   //Vals.push_back(strTime);
   //if (nCam == 0 || nCam == 1 || nCam == 2)       //输出直径
   //{
	  // if (psVal.size() == 2 && nCam == 0)              //直径
		 //  Vals.push_back(psVal[1].sgOut[0].sVal);  //输出psVal[0]是--灰度，psVal[1] 才是直径
	  // if (psVal.size() == 1 && nCam > 0)                //灰度或者PIN间距
		 //  Vals.push_back(psVal[0].sgOut[0].sVal);
	  // else
		 //  Vals.push_back("0.00");
   //}
   //else
   //{
	  // for (int i = 0; i < 2; i++)
	  // {
		 //  if (i >= psVal.size())
		 //  {
			//   Vals.push_back("0.00");
			//   continue;
		 //  }
		 //  Vals.push_back(psVal[i].sgOut[0].sVal);  //左右拨片到中心的距离
	  // }
   //}
   //if (bOK)
	  // Vals.push_back("OK");
   //else
	  // Vals.push_back("NG");
   //Write2CSV(nCam, Vals);
   _pCamCfg = _pLog = _pMain = NULL;
   UpdateMarginPar();
}

bool CSingleton::IsFitCircle(HTuple& hvRs, HTuple& hvCs, double XYR[3])
{
	if (hvCs.TupleLength().I() < 3 || hvRs.TupleLength().I() < 3)
	{
		return false;
	}
	HTuple hvRow, hvCol, hvRadius, hvStartPhi, hvEndPhi, hvPointOrder;
	HObject hoCons;
	GenContourPolygonXld(&hoCons, hvRs, hvCs);
	FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hvRow, &hvCol, &hvRadius, &hvStartPhi, &hvEndPhi, &hvPointOrder);
	XYR[0] = hvCol.D();
	XYR[1] = hvRow.D();
	XYR[2] = hvRadius.D();
	return true;
}

CSingleton::~CSingleton(void)
{
}

void CSingleton::DeleteInstance()
{
	if (NULL != m_pInstance && NULL != m_pInstance->_pLog)
	{
		delete m_pInstance->_pLog;
		m_pInstance->_pLog = NULL;
	}
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
	return;
}

bool CSingleton::GetLeafsValue(CTreeCtrl& tree, HTREEITEM hItem, int nLeaf, CStrs& sLeafs)
{
	sLeafs.clear();
	HTREEITEM hChildItem = tree.GetChildItem(hItem);
	for (int i = 0; i < nLeaf; i++)
	{
		CString strVal = tree.GetItemText(hChildItem);
		CStringArray sVals;
		strVal.TrimRight();
		if (strVal.Right(1) == "=")
		{
			sLeafs.push_back("");
			hChildItem = tree.GetNextSiblingItem(hChildItem);
			continue;
		}
		SplitString(strVal, "=", sVals);
		sLeafs.push_back(sVals[1]);
		hChildItem = tree.GetNextSiblingItem(hChildItem);
		if (hChildItem == NULL && i != nLeaf - 1)
			return false;
	}
	return true;
}

void CSingleton::GetSecKeys(CString strSec, CStringArray& strs)
{
	strs.RemoveAll();
	char szModule[MAX_PATH] = { 0 };
	char sections[2000] = { 0 };

	TCHAR szBuffer[65536] = { 0 };
	TCHAR szKey[1024] = { 0 };
	CString strKey = _T("");
	CString strKeyName = _T("");
	CString strKeyValue = _T("");
	int nBufferSize = GetPrivateProfileSection(strSec, szBuffer, 65536, _strINI);
	for (int n = 0, i = 0; n < nBufferSize; n++)
	{
		if (szBuffer[n] == 0)
		{
			szKey[i] = 0;
			strKey = szKey;
			strKeyName = strKey.Left(strKey.Find('='));
			strKeyValue = strKey.Mid(strKey.Find('=') + 1);
			strs.Add(strKeyName);
			i = 0;
		}
		else
		{
			szKey[i] = szBuffer[n];
			i++;
		}
	}
	//GetPrivateProfileSectionNames(sections, 2000, _strINI);
}

CString CSingleton::LinkString(CStrs & ss, CString strLink, bool bTrim /* = true */)
{
	CString strRet;
	for (int i = 0; i < ss.size();i++)
	{
		if (bTrim && i == ss.size() - 1)
		{
			strRet += ss[i];
			continue;
		}
		strRet += ss[i] + strLink;
	}
	return strRet;
}

int CSingleton::SplitString(const CString &csInput, const CString &csDelimiter,CStringArray &aResults)
{
	aResults.RemoveAll();
	int nPos = -1;
	int nNewPos = -1;	
	int nInputSize = csInput.GetLength();
	int nDelimiterSize = csDelimiter.GetLength();
	int nNumFound = 0;
	CString csDstString = _T("");
	CArray<int, int> aPositions;  // int 型数组
	nNewPos = csInput.Find(csDelimiter, 0);  // 查找分界符的位置
	if ( nNewPos < 0 ) 
	{
		aResults.Add(csInput);  
		return 0; 
	}
	while ( nNewPos > nPos )
	{
		nNumFound++;
		aPositions.Add( nNewPos );  // 将找到的分界符的位置存入数组
		nPos = nNewPos;
		nNewPos = csInput.Find( csDelimiter, nPos+nDelimiterSize );
	}
	for ( int i = 0; i <= aPositions.GetSize(); i++ )
	{

		if ( i == 0 )
		{
			csDstString = csInput.Mid( i, aPositions[i] );  // 获取目标字符串
		}
		else
		{
			int nOffset = aPositions[i-1] + nDelimiterSize;
			if ( nOffset < nInputSize )
			{
				if ( i == aPositions.GetSize() )
				{
					csDstString = csInput.Mid(nOffset);
				}
				else if ( i > 0 )
				{
					csDstString = csInput.Mid( aPositions[i-1] + nDelimiterSize, aPositions[i] - aPositions[i-1] - nDelimiterSize );
				}
				else
				{
					// 这里，不需要做任何事情.
				}    
			}
		}
		if ( csDstString.GetLength() >= 0 )
		{
			aResults.Add(csDstString);  // 将分割后的子字符串添加到结果数组
		}
	}
	return nNumFound;
}

int CSingleton::SplitString(const CString &csInput, const CString &csDelimiter, CStrs &aResults)
{
	aResults.clear();
	int nPos = -1;
	int nNewPos = -1;
	int nInputSize = csInput.GetLength();
	int nDelimiterSize = csDelimiter.GetLength();
	int nNumFound = 0;
	CString csDstString = _T("");
	CArray<int, int> aPositions;  // int 型数组
	nNewPos = csInput.Find(csDelimiter, 0);  // 查找分界符的位置
	if (nNewPos < 0)
	{
		aResults.push_back(csInput);
		return 0;
	}
	while (nNewPos > nPos)
	{
		nNumFound++;
		aPositions.Add(nNewPos);  // 将找到的分界符的位置存入数组
		nPos = nNewPos;
		nNewPos = csInput.Find(csDelimiter, nPos + nDelimiterSize);
	}
	for (int i = 0; i <= aPositions.GetSize(); i++)
	{
		if (i == 0)
		{
			csDstString = csInput.Mid(i, aPositions[i]);  // 获取目标字符串
		}
		else
		{
			int nOffset = aPositions[i - 1] + nDelimiterSize;
			if (nOffset < nInputSize)
			{
				if (i == aPositions.GetSize())
				{
					csDstString = csInput.Mid(nOffset);
				}
				else if (i > 0)
				{
					csDstString = csInput.Mid(aPositions[i - 1] + nDelimiterSize, aPositions[i] - aPositions[i - 1] - nDelimiterSize);
				}
				else
				{
					// 这里，不需要做任何事情.
				}
			}
		}
		if (csDstString.GetLength() >= 0)
		{
			aResults.push_back(csDstString);  // 将分割后的子字符串添加到结果数组
		}
	}
	return nNumFound;
}

void CSingleton::Write2CSV(CString strIP, CString strVAL1, CString strVAL2, CString strVAL3)
{
	CString strT1, strT2;
	CTime tm = CTime::GetCurrentTime();
	strT1 = tm.Format("%y-%m-%d");
	strT2 = tm.Format("%H:%M:%S");
	CreateDir("D:\\DATA");                          //创建一个目录
	CString strPath;
	strPath.Format("D:\\DATA\\%s.csv", strT1);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (!bWorking)  //没有检测到CSV文件
	{
		CString str1;
		//[Code], [Time], [D1], [N1], [D2], [N2], [D3], N3,[D4],D4, [D5], [N5]
		str1.Format("IP,空管重量,注水后管重量,除气后管重量,注水量,抽出量,封存量,时间,结果");
		CFile file(strPath, CFile::modeCreate | CFile::modeWrite);
		file.Write(str1, str1.GetLength());
		file.Flush();
		file.Close();
	}
	CFile fp;
	for (;;)
	{
		if (fp.Open(strPath, CFile::shareDenyWrite | CFile::shareDenyRead) == FALSE)
		{
			AfxMessageBox(_T("请关闭当前EXECL表格！"), 0, MB_ICONWARNING);
			continue;
		}
		break;
	}
	fp.Close();
	CFile file(strPath, CFile::modeWrite);
	file.SeekToEnd();
	CString strData, strbOK;
	double fVal[3],fCha[3];
	fVal[0] = atof(strVAL1);
	fVal[1] = atof(strVAL2);
	fVal[2] = atof(strVAL3);
	fCha[0] = fVal[1] - fVal[0];
	fCha[1] = fVal[1] - fVal[2];
	fCha[2] = fVal[2] - fVal[0];
	double fx[6];
	strbOK.Format("OK");
	fx[0] = atof(GetCfgString(strLiaohao, "注水量下限", "-1.000"));
	fx[1] = atof(GetCfgString(strLiaohao, "注水量上限", "2.000"));
	fx[2] = atof(GetCfgString(strLiaohao, "抽出量下限", "-1.000"));
	fx[3] = atof(GetCfgString(strLiaohao, "抽出量上限", "2.000"));
	fx[4] = atof(GetCfgString(strLiaohao, "封存量下限", "-1.000"));
	fx[5] = atof(GetCfgString(strLiaohao, "封存量上限", "2.000"));
	if (fCha[0]<fx[0] || fCha[0]>fx[1] || fCha[1]<fx[2] || fCha[1]>fx[3] || fCha[2]<fx[4] || fCha[2]>fx[5])
	{
		strbOK.Format("NG");
	}
	//LogFile("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", fCha[0], fCha[1], fCha[2], fx[0], fx[1], fx[2], fx[3], fx[4], fx[5]);
	strData.Format("\r\n%s, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,%s,%s,%s",
		strIP, fVal[0], fVal[1], fVal[2], fCha[0], fCha[1], fCha[2], strT2, strbOK, strLiaohao);
	file.Write(strData, strData.GetLength());
	file.Flush();
	file.Close();
}

void CSingleton::Write2CSV(int nCam,CStrs Vals)
{
	CString strT1, strT2;
	CTime tm = CTime::GetCurrentTime();
	strT1 = tm.Format("%y-%m-%d");
	strT2 = tm.Format("%H:%M:%S");
	CreateDir("D:\\DATA");                          //创建一个目录
	CString strPath;
	strPath.Format("D:\\DATA\\CAM%d %s.csv", nCam + 1,strT1);
	BOOL bFind = PathFileExists(strPath);
	if (bFind == FALSE)                                         //没有检测到CSV文件
	{
		CString strHead;
		if (nCam == 0)
		{
			strHead.Format("相机,检测时间,内径,结果");
		}
		else if (nCam == 1)
		{
			strHead.Format("相机,检测时间,灰度,结果");
		}
		else if (nCam == 2)
		{
			strHead.Format("相机,检测时间,PIN间距,结果");
		}
		else
		{
			strHead.Format("相机,检测时间,左开口距离,右开口距离,结果");
		}
		CFile file(strPath, CFile::modeCreate | CFile::modeWrite);
		file.Write(strHead, strHead.GetLength());
		file.Flush();
		file.Close();
	}
	CFile fp;
	for (;;)
	{
		if (fp.Open(strPath, CFile::shareDenyWrite | CFile::shareDenyRead) == FALSE)
		{
			AfxMessageBox(_T("请关闭当前EXECL表格！"), 0, MB_ICONWARNING);
			continue;
		}
		break;
	}
	fp.Close();
	CFile file(strPath, CFile::modeWrite);
	file.SeekToEnd();
	CString strVal, strData;
	strData.Format("\r\n");
	for (int i = 0; i < (int)Vals.size(); i++)
	{
		strVal.Format("%s,", Vals[i]);
		strData += strVal;
	}
	strVal.TrimRight(",");
	file.Write(strData, strData.GetLength());
	file.Flush();
	file.Close();
	int u = 0;
}

void CSingleton::Write2CSV(CDStrs& DVs)
{
	CString strT1, strT2;
	CTime tm = CTime::GetCurrentTime();
	strT1 = tm.Format("%y-%m-%d");
	strT2 = tm.Format("%H:%M:%S");
	CreateDir("D:\\DATA");                          //创建一个目录
	CString strPath;
	strPath.Format("D:\\DATA\\%s.csv",strT1);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath);
	if(!bWorking)  //没有检测到CSV文件
	{
		CString str1;
		//[Code], [Time], [D1], [N1], [D2], [N2], [D3], N3,[D4],D4, [D5], [N5]
		str1.Format("条码,检测时间,D1,N1,D2,N2,D3,N3,D4,N4,D5,N5,结果");
		CFile file(strPath,CFile::modeCreate|CFile::modeWrite);
		file.Write(str1,str1.GetLength());
		file.Flush();
		file.Close();
	}
	CFile fp;
	for(;;)
	{
		if(fp.Open(strPath,CFile::shareDenyWrite|CFile::shareDenyRead) == FALSE)
		{
			AfxMessageBox(_T("请关闭当前EXECL表格！"),0,MB_ICONWARNING);
			continue;
		}
		break;
	}
	fp.Close();
	CFile file(strPath,CFile::modeWrite);
	file.SeekToEnd();
	CString strCode,strData,strDirty,strMove;
	for (int i = 0; i < (int)DVs.size();i++)
	{
		strData.Format("\r\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", 
			DVs[i][0], DVs[i][1], DVs[i][2], DVs[i][3], DVs[i][4], DVs[i][5], DVs[i][6], DVs[i][7], DVs[i][8], DVs[i][9], DVs[i][10], DVs[i][11],DVs[i][12]);
		file.Write(strData, strData.GetLength());
		file.Flush();
	}
	file.Close();
}

double CSingleton::GetAngle(double fAxisPt[2],double fVecPt[2],bool bImage /* = false */,bool bRad /* = true */)
{
	HTuple hvAng;
	AngleLx(fAxisPt[1],fAxisPt[0],fVecPt[1],fVecPt[0],&hvAng);
	double fPhi,fDeg;
	fPhi = hvAng.D();
	fDeg = fPhi*180/PI;
	if (bImage == false)   //上面的函数试用图像坐标系
	{
		fPhi *= -1;
		fDeg *= -1;
	}
	if (bRad == false)
		return fDeg;
	return fPhi;
}

double CSingleton::GetTwoPtDist(double* fOne,double* fTwo)
{
	double fDist = 0;
	double fMov[2];
	fMov[0] = fTwo[0] - fOne[0];
	fMov[1] = fTwo[1] - fOne[1];
	fDist = sqrt(fMov[0]*fMov[0] + fMov[1]*fMov[1]);
	return fDist;
}

double CSingleton::GetVertAng(double fPhi, double fRange[2],bool bPlus)
{
	double fAng = fPhi + 0.5*PI;
	if (bPlus == false)
		fAng = fPhi - 0.5*PI;
	if (fAng > fRange[1])
		fAng -= 2.0*PI;
	if (fAng < fRange[0])
		fAng += 2.0*PI;
	return fAng;
}

bool CSingleton::GetFitLineAng(HTuple& WndID, HTuple& Ys, HTuple& Xs, double& fFitPhi,double fMaxD, bool bShow)
{
	try
	{
		HTuple hv_RowA,  hv_ColA,  hv_RowB,  hv_ColB;
		int nSum = Xs.TupleLength().I();
		if (nSum < 2)
			return false;
		HTuple hv_Nr, hv_Nc, hv_Dist, hv_S, hv_R, hv_C;
		HObject hoL;
		GenContourPolygonXld(&hoL, Ys, Xs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		HTuple Ds, ASC;
		Ds = HTuple();
		for (int i = 0; i < nSum; i++)
		{
			DistancePl(Ys[i], Xs[i], hv_RowA, hv_ColA, hv_RowB, hv_ColB, &hv_Dist);
			double fDist = hv_Dist.D();
			Ds[i] = hv_Dist;
		}
		TupleSortIndex(Ds, &ASC);  //升序排序索引
		HTuple Rs, Cs;
		Rs = HTuple();
		Cs = HTuple();
		int n = 0;
		for (int i = 0; i < nSum; i++)
		{
			int II = ASC[i].I();
			double fDist = Ds[II].D();
			if (fMaxD < fDist) //因为元素是无序的，所以不能break
				continue;
			Rs[n] = Ys[II];
			Cs[n] = Xs[II];
			//TRACE("\n%.2f,%.2f",Xs[i].D(),Ys[i].D());
			n++;
		}
		if (n < 2)
		{
			return false;
		}
		GenContourPolygonXld(&hoL, Rs, Cs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		//Xs.Clear();
		//Ys.Clear();
		//for (int i = 0;i < n;i++)
		//{
		//	Ys[i] = Rs[i];
		//	Xs[i] = Cs[i];	
		//}
		HTuple hvPhi;
		AngleLx(hv_RowA, hv_ColA, hv_RowB, hv_ColB, &hvPhi);
		fFitPhi = hvPhi.D();
		if (bShow)
			DispCross(WndID, Rs, Cs, 36, 0);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("直线拟合过滤失败：%s", pc);
		LogFile(str);
		return false;
	}
	return true;
}


bool CSingleton::GetFitLine(HTuple& WndID,HTuple& Ys,HTuple& Xs,HTuple& hv_RowA,HTuple& hv_ColA,HTuple& hv_RowB,HTuple& hv_ColB,double fMaxD,bool bShow)
{
	try
	{
		int nSum = Xs.TupleLength().I();
		if (nSum < 2)
			return false;
		HTuple hv_Nr, hv_Nc, hv_Dist,hv_S,hv_R,hv_C;
		HObject hoL;
		GenContourPolygonXld(&hoL,Ys, Xs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		HTuple Ds,ASC;
		Ds = HTuple();
		for (int i = 0;i < nSum;i++)
		{
			DistancePl(Ys[i],Xs[i],hv_RowA, hv_ColA, hv_RowB, hv_ColB,&hv_Dist);
			double fDist = hv_Dist.D();
			Ds[i] = hv_Dist;
		}
		TupleSortIndex(Ds,&ASC);  //升序排序索引
		HTuple Rs,Cs;
		Rs = HTuple();
		Cs = HTuple();
		int n = 0;
		for (int i = 0;i < nSum;i++)
		{
			int II = ASC[i].I();
			double fDist = Ds[II].D();
			if (fMaxD < fDist) //因为元素是无序的，所以不能break
				continue;
			Rs[n] = Ys[II];
			Cs[n] = Xs[II];
			//TRACE("\n%.2f,%.2f",Xs[i].D(),Ys[i].D());
			n++;
		}
		if (n < 2)
		{
			return false;
		}
		GenContourPolygonXld(&hoL,Rs, Cs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		//Xs.Clear();
		//Ys.Clear();
		//for (int i = 0;i < n;i++)
		//{
		//	Ys[i] = Rs[i];
		//	Xs[i] = Cs[i];	
		//}
		if (bShow)
			DispCross(WndID,Rs,Cs,36,0);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("直线拟合过滤失败：%s",pc);
		LogFile(str);
		return false;
	}
	return true;
}


bool CSingleton::GetFitLineVal(HTuple& WndID,HTuple& Ys,HTuple& Xs, 
	HTuple& hv_RowA,HTuple& hv_ColA,HTuple& hv_RowB,HTuple& hv_ColB, 
	double &fVal,double fMaxD /* = 6 */,bool bFilter /* = true */,bool bShow /* = true */)
{
	try
	{
		int nSum = Xs.TupleLength().I();
		if (nSum < 2)
			return false;
		HTuple hv_Nr, hv_Nc, hv_Dist,hv_S,hv_R,hv_C;
		HObject hoL;
		GenContourPolygonXld(&hoL,Ys, Xs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		HTuple Ds,ASC;
		Ds = HTuple();
		for (int i = 0;i < nSum;i++)
		{
			DistancePl(Ys[i],Xs[i],hv_RowA, hv_ColA, hv_RowB, hv_ColB,&hv_Dist);
			double fDist = hv_Dist.D();
			Ds[i] = hv_Dist;
		}
		TupleSortIndex(Ds,&ASC);  //升序排序索引
		HTuple Rs,Cs;
		Rs = HTuple();
		Cs = HTuple();
		int n = 0;
		for (int i = 0;i < nSum;i++)
		{
			int II = ASC[i].I();
			double fDist = Ds[II].D();
			if (fDist > fMaxD) //因为元素是无序的，所以不能break
				continue;
			Rs[n] = Ys[II];
			Cs[n] = Xs[II];
			n++;
		}
		if (n < 2)
		{
			return false;
		}
		GenContourPolygonXld(&hoL,Rs, Cs);
		FitLineContourXld(hoL, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hv_Dist);
		fVal = 0;
		HTuple hv_Row, hv_Col;
		TRACE("\n(%.2f,%.2f),(%.2f,%.2f)",hv_ColA.D(),hv_RowA.D(),  hv_ColB.D(),hv_RowB.D());
		double fDX = hv_ColB.D() - hv_ColA.D();
		double fDY = hv_RowB.D() - hv_RowA.D();
		double fMod = sqrt(fDX*fDX + fDY*fDY);
		fDX /= fMod;
		fDY /= fMod;
		//TRACE("\n(%.3f,%.3f),(%.2f)",hv_Nr.D(),hv_Nc.D(), hv_Dist.D());
		double fDist = hv_Dist.D();
		double fNr = hv_Nr.D();
		double fNc = hv_Nc.D();
		for (int i = 0;i < nSum;i++)
		{		
			//double fPt[2],fArm = 0;
			//fPt[0] = Xs[i].D() + fDY*fArm;
			//fPt[1] = Ys[i].D() - fDX*fArm;
			double fTmp = abs(Ys[i].D()*fNr + Xs[i].D()*fNc - fDist);  //平面直线方程
			fVal += abs(fTmp);
			//TRACE("\nNO%d Dist:%.1f ",i + 1,fTmp);
		}
		fVal /= nSum;
		if (bFilter)
		{
			Xs.Clear();
			Ys.Clear();
			for (int i = 0;i < n;i++)
			{
				Ys[i] = Rs[i];
				Xs[i] = Cs[i];
			}
		}
		if (bShow)
		{
			if (bFilter)
				DispCross(WndID,Ys,Xs,50,0);
			else
				DispCross(WndID,Rs,Cs,66,0);
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("直线逼近过滤失败：%s",pc);
		LogFile(str);
		return false;
	}
	return true;
}

//只用判断新输入的字符是什么即可
void CSingleton::CheckNumber(CString& str)
{
	if (str.GetLength() == 0)
		return;
	char e = str.GetAt(str.GetLength() - 1);
	if (e >= '0' && e <= '9')
		return;
	if (e == '.')
	{
		for (int i = 0;i < str.GetLength() - 1;i++) //以前字符输入是否有小数点
		{
			if (str.GetAt(i) == '.')
			{
				CString strTmp = str.Left(str.GetLength() - 1);
				str.Format("%s",strTmp);
				return;
			}
		}
	}
	else if (e == '-' )
	{
		if (str.GetLength() > 1)
		{
			CString strTmp = str.Left(str.GetLength() - 1);
			str.Format("%s",strTmp);
		}
	}
	else //其他符号直接过滤
	{
		CString strTmp = str.Left(str.GetLength() - 1);
		str.Format("%s",strTmp);
	}
}

bool CSingleton::GetRectCorner(HTuple& WndID,HObject& hoRect,double& fR,double& fC,double fDist,bool bShow)
{
	try
	{
		HTuple hvXs,hvYs,hvLen,hvL1,hvL2,hv_Row, hv_Col, hv_Phi,hv_IsOverlapping,hvDist;
		hvXs = HTuple();
		hvYs = HTuple();
		SmallestRectangle2(hoRect, &hv_Row, &hv_Col, &hv_Phi, &hvL1, &hvL2); //的到轮廓的外接矩形
		for (int i = 0; i <= 3; i++)
		{
			hvLen = hvL1;
			if (i%2 == 1)
			{
				hvLen = hvL2;
			}
			hvXs[i] = hv_Col+(hvLen*(((-hv_Phi)-((i*PI)*0.5)).TupleCos()));
			hvYs[i] = hv_Row+(hvLen*(((-hv_Phi)-((i*PI)*0.5)).TupleSin()));
			//DispCross(3600, HTuple(hvYs[i]), HTuple(hvXs[i]), 96, hv_Phi);
		}
		//hv_N = hv_Xs.TupleLength();
		bool bGet = false;
		double fHorn[2];		
		for (int i = 0; i <= 3; i++)
		{
			int j = (i + 1)%4;
			IntersectionLines(HTuple(hvYs[i]), HTuple(hvXs[i]), HTuple(hvYs[i])+(((-hv_Phi)-((i*PI)*0.5)).TupleCos()), HTuple(hvXs[i])-(((-hv_Phi)-((i*PI)*0.5)).TupleSin()), 
									  HTuple(hvYs[j]), HTuple(hvXs[j]), HTuple(hvYs[j])+(((-hv_Phi)-((j*PI)*0.5)).TupleCos()), HTuple(hvXs[j])-(((-hv_Phi)-((j*PI)*0.5)).TupleSin()), 
									  &hv_Row, &hv_Col, &hv_IsOverlapping);
			DistancePp(fR,fC,hv_Row,hv_Col,&hvDist);
			if (hvDist.D() < fDist)
			{
				fDist = hvDist.D();
				bGet = true;
				fHorn[0] = hv_Col.D();
				fHorn[1] = hv_Row.D();
			}
		}
		if (bGet == false)
			return false;
		fR = fHorn[1];
		fC = fHorn[0];
		if (bShow)
			DispCross(WndID, fHorn[1], fHorn[0], 96, 0.25*PI);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("最小外接矩形角点检测失败：%s！",pc);
		LogFile(str);
		return false;
	}
	return true;
}

bool CSingleton::GetCircleTmpXY(HTuple& WndID, HObject& hoPic, double fCen[2], double& fR, double fPar[9],int nJDW)
{
	try
	{
		int nW = (int)fPar[2];
		int nH = (int)fPar[3];
		int nThres = (int)fPar[4];
		HTuple hvN, hvW, hvH, hvX, hvY, hvS, hvDist, hvRowEdge, hvColEdge, hvAmp,hvXs,hvYs;
		HObject hoImage, hoZoom, hoReg, hoConsReg, hoSel, hoFill, hoOpen, hoRegions, hoClose, hoRC, hoCrop,hoCons;
		CopyImage(hoPic, &hoImage);
		GenRectangle2(&hoRC, fCen[1], fCen[0], 0, 0.5*nW, 0.5*nH);   //方法二生成的矩形区域，长宽只需要一半
		ReduceDomain(hoImage, hoRC, &hoCrop);
		GetImageSize(hoImage, &hvW, &hvH);
		if (nThres > 0)
		{
			Threshold(hoCrop, &hoReg, abs(nThres),255 );
		}
		else
		{
			Threshold(hoCrop, &hoReg, 0, abs(nThres));
		}
		ClosingCircle(hoReg, &hoClose, 3);
		FillUp(hoClose, &hoFill);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HalconCpp::Connection(hoFill, &hoConsReg);
		SelectShape(hoConsReg, &hoRegions, "area", "and", 500, 3999999);
		CountObj(hoRegions, &hvN);
		int nCons = hvN.I();
		CPos m;
		double fMinDist = 100;		
		double fRadius = -1;
		for (int i = 0; i < nCons; i++)
		{
			SelectObj(hoRegions, &hoSel, i + 1);
			FillUp(hoSel, &hoFill);
			OpeningCircle(hoFill, &hoOpen, 3);
			AreaCenter(hoOpen, &hvS, &hvY, &hvX);
			DistancePp(hvY, hvX, fCen[1], fCen[0], &hvDist);
			if (hvDist.D() < fMinDist)
			{
				fMinDist = hvDist.D();
				m.iPrNO = i;                                            //最接近的轮廓索引
				m.x = hvX.D();
				m.y = hvY.D();
				fRadius = sqrt(hvS.D() / PI);                 //轮廓的大体半径
			}
		}
		if (fRadius < 0)                                               //找到了最接近设定原点的圆心坐标
		{
			LogFile("没有找到合适的圆轮廓！");
			return false;
		}			
		//fCen[0] = m.x;
		//fCen[1] = m.y;
		int nS = 60,n = 0;
		fRadius = 0.5*(fPar[JDWPR_MAXR] + fPar[JDWPR_MINR]);
		double fAngMax = fPar[JDWPR_ANGST] + fPar[JDWPR_RANGE];
		int nMax = (int)(fAngMax / (2 * PI / nS) + 0.5);
		for (int i = 0; i < nMax; i += 1)
		{
			double fMCen[2];
			double fPhi = 2 * i * PI / nS;
			if (fPhi < fPar[JDWPR_ANGST])
				continue;
			fMCen[0] = fCen[0] + fRadius*cos(fPhi);
			fMCen[1] = fCen[1] -  fRadius*sin(fPhi);
			if (fMCen[0] > hvW || fMCen[0] < 10 || fMCen[1] > hvH || fMCen[1] < 0)
				continue;
			if (nJDW == JDW_O2I_FIRST || nJDW == JDW_O2I_LAST || nJDW == JDW_O2I_MAX)
				fPhi += PI;
			//DispRectangle2(WndID, fMCen[1], fMCen[0], fPhi, 0.5*abs(fPar[JDWPR_MAXR] - fPar[JDWPR_MINR]), 25);
			HTuple hvMsr;
			GenMeasureRectangle2(fMCen[1], fMCen[0], fPhi, 0.5*abs(fPar[JDWPR_MAXR] - fPar[JDWPR_MINR]) , 25, hvW, hvH, "nearest_neighbor", &hvMsr);
			double fThr = 25;
			if (nThres > 0)
			{
				if (nJDW == JDW_O2I_FIRST || nJDW == JDW_O2I_LAST || nJDW == JDW_O2I_MAX)
					MeasurePos(hoImage, hvMsr, 1, fThr, "positive", "all", &hvRowEdge, &hvColEdge, &hvAmp, &hvDist);
				else
					MeasurePos(hoImage, hvMsr, 1, fThr, "negative", "all", &hvRowEdge, &hvColEdge, &hvAmp, &hvDist);
			}
			else
			{
				if (nJDW == JDW_O2I_FIRST || nJDW == JDW_O2I_LAST || nJDW == JDW_O2I_MAX)
					MeasurePos(hoImage, hvMsr, 1, fThr, "negative", "all", &hvRowEdge, &hvColEdge, &hvAmp, &hvDist);
				else
					MeasurePos(hoImage, hvMsr, 1, fThr, "positive","all", &hvRowEdge, &hvColEdge, &hvAmp, &hvDist);
			}
			CloseMeasure(hvMsr);
			int nT = hvRowEdge.TupleLength().I();
			if (nT == 0)
				continue;
			if (nJDW == JDW_I2O_FIRST || nJDW == JDW_O2I_FIRST)
			{
				hvXs[n] = HTuple(hvColEdge[0]);
				hvYs[n] = HTuple(hvRowEdge[0]);
				n++;
			}
			else if (nJDW == JDW_I2O_LAST || nJDW == JDW_O2I_LAST)
			{
				hvXs[n] = HTuple(hvColEdge[nT - 1]);
				hvYs[n] = HTuple(hvRowEdge[nT - 1]);
				n++;
			}
			else
			{
				GetMaxAmpPt(hvAmp, hvRowEdge,hvColEdge,hvXs,hvYs,n,fPhi,hoImage);
			}
			
			//DWORD dw = GetTickCount();
			//while (GetTickCount() - dw < 900)
			//{
			//	DispCross(WndID, hvYs[n - 1], hvXs[n - 1], 30, 0.25*PI);
			//	DoEvent();
			//}
		}
		if (n < 4)
		{
			LogFile("圆孔边界点数太少");
			return false;
		}
		HTuple hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Row, hv_Col;
		GenContourPolygonXld(&hoCons, hvYs, hvXs);
		FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
		double fAve = hv_Radius.D();
		HTuple Rs, Cs;
		Rs = HTuple();
		Cs = HTuple();
		nS = n;
		n = 0;
		for (int i = 0; i < nS; i++)
		{
			DistancePp(hvYs[i], hvXs[i], hv_Row, hv_Col, &hvDist);
			double fDiff = abs(hvDist.D() - fAve);
			if (abs(fDiff) < 12)
			{
				Cs[n] = hvXs[i];
				Rs[n] = hvYs[i];
				n++;
				DispCross(WndID, hvYs[i], hvXs[i], 12, 0);
			}
		}
		if (n < 3)
		{
			//LogFile("找到的产品圆孔边界点不合格！");
			return false;
		}
		GenContourPolygonXld(&hoCons, Rs, Cs);
		FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
		////////////////////////////////////////////////////////剩下的区域内搜索最大圆弧轮廓/////////////////////////////////////////////////////////
		DispCircle(WndID, hv_Row, hv_Col, hv_Radius + 2);		
		fCen[1] = hv_Row.D();
		fCen[0] = hv_Col.D();
		fR = hv_Radius.D();
		return true;
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("精定位圆孔轮廓的中心点异常：%s！", pc);
		LogFile(str);
	}
	return false;
}

void CSingleton::GetMaxAmpPt(HTuple& hvAmplitude, HTuple& hvRowEdge, HTuple &hvColEdge, HTuple& hvXs, HTuple &hvYs, int& n, double fDir, HObject& hoImage)
{
	try
	{
		int nNum = hvRowEdge.TupleLength().I();
		if (nNum == 0)
			return;
		bool bGet = false;
		double fMaxDev = _iDev;
		for (int j = 0; j < nNum; j++)    //找到满足方差条件的边缘点
		{
			HObject hoRC;
			HTuple hvMean, hvDev;
			GenRectangle2(&hoRC, hvRowEdge[j], hvColEdge[j], fDir, _iLen[0], _iLen[1]);
			Intensity(hoRC, hoImage, &hvMean, &hvDev);
			if (hvDev.D() < fMaxDev)    //方差太小不予理会
				continue;
			fMaxDev = hvDev.D();
			hvXs[n] = hvColEdge[j];
			hvYs[n] = hvRowEdge[j];
			bGet = true;
		}
		if (bGet == true)
			n++;
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("边缘点搜索失败：%s！", pc);
		LogFile(str);
	}
}


bool CSingleton::GetJDWPos(HTuple& WndID, HObject& hoImage, HTuple& hvMat2D, double fPar[10], CTMP& tm, bool bShow)
{
	try
	{
		int nJDW = tm.nJDW;
		double fXYLen[2][5];
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 5;j++)
			{
				fXYLen[i][j] = tm.fJDWLH[i][j];
			}
		}
		CString strLog;
		SetColor(WndID, "blue");
		HObject hoH, hoV;
		HTuple hvW, hvH, hvMsr, hvRowEdge, hvColEdge, hvAmplitude, hvDist;
		HTuple hv_RowA, hv_ColA, hv_RowB, hv_ColB, hv_RowC, hv_ColC, hv_RowD, hv_ColD, hv_Nr, hv_Nc;
		HTuple hvRow, hvCol, hvIs;
		GetImageSize(hoImage, &hvW, &hvH);
		HTuple hvXs, hvYs;
		hvXs = HTuple();
		hvYs = HTuple();
		CPos PX, PY;
		int nW = 90, nH = 90, n = 0, nStep = max(2, (int)(fXYLen[0][0] * 0.048));
		if (nJDW == JDW_I2O_FIRST || nJDW == JDW_O2I_FIRST ||
			nJDW == JDW_I2O_LAST   || nJDW == JDW_O2I_LAST  ||
			nJDW == JDW_I2O_MAX || nJDW == JDW_O2I_MAX)
		{
			SetColor(WndID, "green");
			AffineTransPoint2d(hvMat2D, fPar[1], fPar[0], &hvRow, &hvCol);
			double fCen[2],fR;           
			fCen[0] = hvCol.D() / ZOOM;
			fCen[1] = hvRow.D() / ZOOM;
			fPar[2] /=  ZOOM;
			fPar[3] /= ZOOM;
			bool bRet = GetCircleTmpXY(WndID,hoImage, fCen,fR,fPar,nJDW);         //精定位进一步提高圆心精度
			if (bRet)
			{
				fPar[0] = fCen[0];
				fPar[1] = fCen[1];
				fPar[9] = fR;         //得到精定位的半径
			}
			return bRet;
		}
		else if (nJDW == JDW_TWO_LINE)
		{
			CPos rcCen[2];                                                                                                     //精定位检测框的中心点
			for (int i = 0; i < 2; i++)
			{
				n = 0;
				hvXs.Clear();
				hvYs.Clear();
				double fPhi = fXYLen[i][2];
				nStep = 30;
				int nCount = (int)(2 * fXYLen[i][4] / nStep - 3);
				nH = (int)(fXYLen[i][3] + 0.5);
				HTuple hvR, hvC;
				AffineTransPoint2d(hvMat2D, ZOOM*fXYLen[i][1], ZOOM*fXYLen[i][0], &hvR, &hvC);
				rcCen[i].x = hvC.D() / ZOOM;
				rcCen[i].y = hvR.D() / ZOOM;
				for (int j = -nCount; j < nCount; j++)
				{
					if (rcCen[i].y + nStep*j*cos(fPhi) < 0 || rcCen[i].y + nStep*j*cos(fPhi) > hvH ||
						rcCen[i].x + nStep*j*sin(fPhi) < 0 ||  rcCen[i].x + nStep*j*sin(fPhi) > hvW )
						continue;
					//DispRectangle2(WndID, rcCen[i].y + nStep*j*cos(fPhi), rcCen[i].x + nStep*j*sin(fPhi), fPhi,nW,nH);
					GenMeasureRectangle2(rcCen[i].y + nStep*j*cos(fPhi), rcCen[i].x + nStep*j*sin(fPhi), fPhi, nH, nW, hvW, hvH, "nearest_neighbor", &hvMsr);
					MeasurePos(hoImage, hvMsr, 1, 25, "all", "all", &hvRowEdge, &hvColEdge, &hvAmplitude, &hvDist);
					CloseMeasure(hvMsr);
					if (hvColEdge.TupleLength() > 0)  //第一条边
					{
						hvXs[n] = hvColEdge[0];
						hvYs[n] = hvRowEdge[0];
						n++;
					}
					//GetMaxAmpPt(hvAmplitude, hvRowEdge, hvColEdge, hvXs, hvYs, n, fPhi, hoImage);
				}
				if (n < 3)
				{
					SetColor(WndID, "green");
					strLog.Format("精定位线检测框%d 中没有检索到定位", i + 1);
					LogFile(strLog);
					return false;
				}
				bool bRet = FilterPts(WndID, hoImage, hvYs, hvXs, fPhi, _iLen);             //过滤不合适的点
				if (bRet == false)
					return false;
				DispCross(WndID, hvYs, hvXs, 20, 0.25*PI);
				if (i == 0)
				{
					GenContourPolygonXld(&hoH, hvYs, hvXs);
					FitLineContourXld(hoH, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hvDist);
					if (bShow)
					{
						DispObj(hoH, WndID);
						DispLine(WndID, hv_RowA, hv_ColA, hv_RowB, hv_ColB);
					}
				}
				else
				{
					GenContourPolygonXld(&hoV, hvYs, hvXs);
					FitLineContourXld(hoV, "tukey", -1, 0, 5, 2, &hv_RowC, &hv_ColC, &hv_RowD, &hv_ColD, &hv_Nr, &hv_Nc, &hvDist);
					if (bShow)
					{
						DispObj(hoV, WndID);
						DispLine(WndID, hv_RowC, hv_ColC, hv_RowD, hv_ColD);
					}
				}
			}
			IntersectionLines(hv_RowA, hv_ColA, hv_RowB, hv_ColB, hv_RowC, hv_ColC, hv_RowD, hv_ColD, &hvRow, &hvCol, &hvIs);
			SetColor(WndID, "green");
			if (hvRow.TupleLength() > 0)
			{
				if (bShow)
					DispCross(WndID, hvRow, hvCol, 180, 0.25*PI);
				fPar[0] = hvCol.D();
				fPar[1] = hvRow.D();
				return true;
			}
			else
				return false;
		}
		else if (nJDW == JDW_CONS_C)  //轮廓中心点
		{
			//搜索模板轮廓
			//FindShapeModel(hoImage, tm.hvXLD,);
		}
		else
		{
			SetColor(WndID, "green");
			return false;
		}
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("精定位失败：%s", pc);
		LogFile(str);
	}
	return false;
}

bool CSingleton::FilterPts(HTuple& WndID, HObject& hoImage, HTuple& Ys, HTuple& Xs, double fDir, int iLen[2])
{
	int nCount = Ys.TupleLength().I();
	if (nCount < 5)
	{
		return false;
	}
	double fGray = 0;
	int n = 0;
	vector<CPos> Pts;
	for (int i = 0; i < nCount; i++)
	{
		CPos p;
		HTuple hvMean, hvDev;
		HObject hoRC;
		GenRectangle2(&hoRC, Ys[i], Xs[i], fDir, iLen[0], iLen[1]);
		Intensity(hoRC, hoImage, &hvMean, &hvDev);
		p.r = hvMean.D();
		p.x = Xs[i].D();
		p.y = Ys[i].D();
		Pts.push_back(p);
		fGray += hvMean.D();
		n++;
	}
	if (n < 3)
		return false;
	fGray /= n;                                                                                          //得到平均灰度
	int nDots = nCount;
	for (int i = 0; i < nCount; i++)
	{
		if (Pts[i].r > fGray + _iGray || Pts[i].r < fGray - _iGray)                //灰度差太大或者太小
		{
			Pts[i].iPrNO = i;
			nDots--;
		}
	}
	if (nDots < 3)
	{
		//LogFile("合格点:%d太少了无法过滤！ ",nDots);
		return false;
	}
	Xs.Clear();
	Ys.Clear();
	n = 0;
	for (int i = 0; i < nCount; i++)
	{
		if (Pts[i].iPrNO >= 0)
			continue;
		Xs[n] = Pts[i].x;
		Ys[n] = Pts[i].y;
		n++;
	}
	if (nCount - nDots > 0)
	{
		//LogFile("过滤了%d个不合格点！", nCount - nDots);
	}
	return true;
}


void CSingleton::InitList(CListCtrlCl& m_list, vector<CListStyle>& Style)
{
	for (int i = 0; i < 13; i++)
		m_list.DeleteColumn(0);
	m_list.SetBkColor(RGB(255, 255, 255));        //设置背景色
	m_list.SetRowHeight(26);                                  //设置行高度
	m_list.SetHeaderHeight((float)1.1);                 //设置头部高度
	m_list.SetHeaderFontHW(22, 0);                     //设置头部字体高度,和宽度,0表示缺省，自适应 
	m_list.SetHeaderTextColor(RGB(255, 255, 255)); //设置头部字体颜色
	m_list.SetTextColor(RGB(0, 0, 0));                 //设置文本颜色
	m_list.SetFontHW(20, 0);                              //设置字体高度，和宽度,0表示缺省宽度
	for (int i = 0; i < (int)Style.size(); i++)
	{
		m_list.InsertColumn(i, (CString)Style[i].str, LVCFMT_CENTER, Style[i].nLen);
	}
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES);
}

bool CSingleton::GetCircleCen(HTuple& WndID, HObject& hoImage, double fCen[2], double fS /* = 35000 */, int nThres/* = 130*/,bool bShow)
{
	HObject hoCpy, hoReg, hoClose, hoCons, hoRegions,hoSel;
	HTuple hvN,hvS,hvR,hvC;
	CopyImage(hoImage, &hoCpy);
	Threshold(hoCpy, &hoReg, 0, abs(nThres));
	ClosingCircle(hoReg, &hoClose, 5);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	HalconCpp::Connection(hoClose, &hoCons);
	SelectShape(hoCons, &hoSel, (HTuple("area").Append("circularity")),"and", (HTuple(0.5*fS).Append(0.9)), (HTuple(2.0*fS).Append(1)));
	CountObj(hoSel,&hvN);
	int nNum = hvN.I();
	if (nNum != 1)
	{
		//没有找到唯一的定位圆
		//AreaCenter(hoCons, &hvS, &hvR, &hvC);
		//for (int i = 0;i < hvS.)
		//{
		//}
		return false;
	}
	AreaCenter(hoSel, &hvS, &hvR, &hvC);
	fCen[0] = hvC.D();
	fCen[1] = hvR.D();
	if (bShow)
	{
		DispCross(WndID, fCen[1], fCen[0], 96, 0);
	}
	return true;
}

bool CSingleton::NineDotCalib(CPos ImgPts[9], CPos AxisPts[9], HTuple& HomMat2D)
{
	try
	{
		HTuple hv_x, hv_y;
		HTuple hv_Row, hv_Col;
		hv_x.Clear();
		hv_y.Clear();
		hv_Row.Clear();
		hv_Col.Clear();
		for (int i = 0; i < 9; i++)
		{
			hv_x[i] = AxisPts[i].x;
			hv_y[i] = AxisPts[i].y;
			hv_Col[i] = ImgPts[i].x;
			hv_Row[i] = ImgPts[i].y;
		}
		VectorToHomMat2d(hv_Row, hv_Col, hv_y, hv_x, &HomMat2D);
	}
	catch (HalconCpp::HOperatorException &q)
	{
		CString str;
		const char* pc = q.ErrorMessage().Text();
		str.Format("9点标定失败：%s", pc);
		LogFile(str);
		return false;
	}
	return true;
}

bool CSingleton::GetDirPoint(HTuple& WndID, HObject& hoImage, double fY, double fX, double fPhi, double fLen[2],double fVec[2])
{
	HObject hoCopy, hoCir, hoSub, hoRC, hoFillUp, hoOpen, hoCons, hoReg, hoSel,hoLN;
	HTuple hvW, hvH, hvMsr, hvRowEdge, hvColEdge, hvAmplitude, hvDist, hvXs, hvYs;
	CopyImage(hoImage, &hoCopy);
	GenRectangle2(&hoRC, fY, fX,fPhi,fLen[0],fLen[1]);
	DispObj(hoRC, WndID);
	GetImageSize(hoImage, &hvW, &hvH);
	int n = 0;
	hvXs.Clear();
	hvYs.Clear();
	int nStep = 30;
	int nCount = (int)(2 * fLen[1] / nStep - 3);
	int nH = (int)(fLen[0] + 0.5);
	int nW = 90;
	CString strLog;
	//HTuple hvR, hvC;
	//AffineTransPoint2d(hvMat2D, ZOOM*fXYLen[i][1], ZOOM*fXYLen[i][0], &hvR, &hvC);
	//rcCen[i].x = hvC.D() / ZOOM;
	//rcCen[i].y = hvR.D() / ZOOM;
	for (int j = -nCount; j < nCount; j++)
	{
		if (fY + nStep*j*cos(fPhi) < 0 || fY + nStep*j*cos(fPhi) > hvH.D() ||
			fX + nStep*j*sin(fPhi) < 0 || fX + nStep*j*sin(fPhi) > hvW.D())
			continue;
		//DispRectangle2(WndID, fY + nStep*j*cos(fPhi), fX + nStep*j*sin(fPhi),fPhi,nW,nH);
		GenMeasureRectangle2(fY + nStep*j*cos(fPhi), fX + nStep*j*sin(fPhi), fPhi, nH, nW, hvW, hvH, "nearest_neighbor", &hvMsr);
		MeasurePos(hoImage, hvMsr, 1, 25, "all", "all", &hvRowEdge, &hvColEdge, &hvAmplitude, &hvDist);
		CloseMeasure(hvMsr);
		if (hvColEdge.TupleLength() > 0)  //第一条边
		{
			hvXs[n] = hvColEdge[0];
			hvYs[n] = hvRowEdge[0];
			n++;
		}
		//GetMaxAmpPt(hvAmplitude, hvRowEdge, hvColEdge, hvXs, hvYs, n, fPhi, hoImage);
	}
	if (n < 3)
	{
		SetColor(WndID, "green");
		strLog.Format("精定位线没有足够点");
		LogFile(strLog);
		return false;
	}
	bool bRet = FilterPts(WndID, hoImage, hvYs, hvXs, fPhi, _iLen);             //过滤不合适的点
	if (bRet == false)
		return false;
	DispCross(WndID, hvYs, hvXs, 20, 0.25*PI);
	GenContourPolygonXld(&hoLN, hvYs, hvXs);
	HTuple hv_RowA, hv_ColA, hv_RowB, hv_ColB, hv_Nr, hv_Nc;
	FitLineContourXld(hoLN, "tukey", -1, 0, 5, 2, &hv_RowA, &hv_ColA, &hv_RowB, &hv_ColB, &hv_Nr, &hv_Nc, &hvDist);
	//if (bShow)
	{
		DispObj(hoLN, WndID);
		DispLine(WndID, hv_RowA, hv_ColA, hv_RowB, hv_ColB);
	}
	fVec[0] = hv_ColB.D() - hv_ColA.D();
	fVec[1] = hv_RowB.D() - hv_RowA.D();
	if (fVec[0] < 0)
	{
		fVec[0] *= -1;
		fVec[1] *= -1;
	}
	return true;
}

bool CSingleton::GetDirPoint(HTuple& WndID, HObject& hoImage, double& fY, double& fX, double fRC[2], double fPar[4],int nJDW)
{
	HObject hoCopy,hoCir,hoSub,hoRC,hoFillUp,hoOpen,hoCons,hoReg,hoSel;
	CopyImage(hoImage, &hoCopy);
	GenRectangle1(&hoCir, fY - 0.5*fRC[1], fX - 0.5*fRC[0], fY + 0.5*fRC[1], fX + 0.5*fRC[0]);
	DispObj(hoCir,WndID);
	ReduceDomain(hoCopy, hoCir, &hoSub);
	Threshold(hoSub, &hoRC, 0, 105);
	FillUp(hoRC, &hoFillUp);
	OpeningCircle(hoFillUp, &hoOpen, 3);
	HalconCpp::Connection(hoOpen, &hoCons);
	HTuple hvN,hvS,hvY,hvX,hvDist;
	SelectShape(hoCons, &hoReg, "area", "and", 900, 399999);
	CountObj(hoReg, &hvN);
	int nCons = hvN.I();
	if (nCons == 0)
		return false;
	CPos m;
	double fMinDist = 100;
	double fRadius = -1;
	for (int i = 0; i < nCons; i++)
	{
		SelectObj(hoReg, &hoSel, i + 1);
		DispObj(hoSel, WndID);
		AreaCenter(hoSel, &hvS, &hvY, &hvX);
		DistancePp(hvY, hvX, fY, fX, &hvDist);
		if (hvDist.D() < fMinDist)
		{
			fMinDist = hvDist.D();
			m.iPrNO = i;                                            //最接近的轮廓索引
			m.x = hvX.D();
			m.y = hvY.D();
			fRadius = sqrt(hvS.D() / PI);                 //轮廓的大体半径
		}
	}
	if (fRadius < 0)
	{
		return false;
	}		
	if (nJDW == 0)
	{
		fY = m.y;
		fX = m.x;
		DispCross(WndID, fY, fX, 90, 0.25*PI);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HTuple hvW, hvH, hvRowEdge, hvColEdge, hvAmp, hvXs, hvYs;
	GetImageSize(hoImage, &hvW, &hvH);
	double fMinR = fPar[0];
	double fMaxR = fPar[1];
	fRadius = 0.5*(fMaxR + fMinR);
	double fAngST = fPar[2];
	double fAngED = fPar[3] + fPar[2];
	int nS = 60,n = 0;
	int nMax = (int)(fAngED / (2 * PI / nS)  + 0.5);
	for (int i = 0; i < nMax; i += 1)
	{
		HTuple hvMsr;
		double fMCen[2];
		double fPhi = 2 * i * PI / nS;		
		if (fPhi < fAngST)
			continue;
		fMCen[0] = fX + fRadius*cos(fPhi);
		fMCen[1] = fY -  fRadius*sin(fPhi);
		if (fMCen[0] > hvW || fMCen[0] < 10 || fMCen[1] > hvH || fMCen[1] < 0)
			continue;
		if (nJDW == JDW_O2I_FIRST || nJDW == JDW_O2I_LAST || nJDW == JDW_O2I_MAX)
			fPhi += PI;
		//DispRectangle2(WndID, fMCen[1], fMCen[0], fPhi, 0.5*abs(fMaxR - fMinR), 25);
		GenMeasureRectangle2(fMCen[1], fMCen[0], fPhi, 0.5*abs(fMaxR - fMinR), 25, hvW, hvH, "nearest_neighbor", &hvMsr);
		/////////////////////////////////////////////////////////////////////该项目全是白色铁片向内搜索轮廓边缘//////////////////////////////////////////////////////////////////
		MeasurePos(hoImage, hvMsr, 1, 35, "negative", "all", &hvRowEdge, &hvColEdge, &hvAmp, &hvDist);
		CloseMeasure(hvMsr);
		int nT = hvRowEdge.TupleLength().I();
		if (nT == 0)
			continue;
		if (nJDW == JDW_I2O_FIRST || nJDW == JDW_O2I_FIRST)
		{
			hvXs[n] = HTuple(hvColEdge[0]);
			hvYs[n] = HTuple(hvRowEdge[0]);
			n++;
		}
		else if (nJDW == JDW_I2O_LAST || nJDW == JDW_O2I_LAST)
		{
			hvXs[n] = HTuple(hvColEdge[nT - 1]);
			hvYs[n] = HTuple(hvRowEdge[nT - 1]);
			n++;
		}
		else
		{
			GetMaxAmpPt(hvAmp, hvRowEdge, hvColEdge, hvXs, hvYs, n, fPhi, hoImage);
		}

		//DWORD dw = GetTickCount();
		//while (GetTickCount() - dw < 900)
		//{
		//	DispCross(WndID, hvYs[n - 1], hvXs[n - 1], 30, 0.25*PI);
		//	DoEvent();
		//}
	}
	if (n < 4)
	{
		LogFile("圆孔边界点数太少");
		return true;
	}
	HTuple hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Row, hv_Col;
	GenContourPolygonXld(&hoCons, hvYs, hvXs);
	FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	double fAve = hv_Radius.D();
	HTuple Rs, Cs;
	Rs = HTuple();
	Cs = HTuple();
	nS = n;
	n = 0;
	for (int i = 0; i < nS; i++)
	{
		DistancePp(hvYs[i], hvXs[i], hv_Row, hv_Col, &hvDist);
		double fDiff = abs(hvDist.D() - fAve);
		if (abs(fDiff) < 13)
		{
			Cs[n] = hvXs[i];
			Rs[n] = hvYs[i];
			n++;
			DispCross(WndID, hvYs[i], hvXs[i], 16, 0);
		}
	}
	if (n < 3)
	{
		LogFile("找到的产品圆孔边界点合格点太少！");
		return true;
	}
	GenContourPolygonXld(&hoCons, Rs, Cs);
	FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	////////////////////////////////////////////////////////剩下的区域内搜索最大圆弧轮廓/////////////////////////////////////////////////////////
	DispCircle(WndID, hv_Row, hv_Col, hv_Radius);
	fY = hv_Row.D();
	fX = hv_Col.D();
	fRadius = hv_Radius.D();
	DispCross(WndID, fY, fX, 90, 0.25*PI);
	return true;
}

double CSingleton::GetHorzAngle(double fAxisPt[2], double fVecPt[2], bool bImage /* = false */, bool bRad /* = true */)
{
	HTuple hvAng;
	AngleLx(fAxisPt[1], fAxisPt[0], fVecPt[1], fVecPt[0], &hvAng);
	double fPhi, fDeg;
	fPhi = hvAng.D();
	fDeg = fPhi * 180 / PI;
	if (bImage == false)   //上面的函数试用图像坐标系
	{
		fPhi *= -1;
		fDeg *= -1;
	}
	if (bRad == false)
		return fDeg;
	return fPhi;
}

bool CSingleton::LoadLeafs(int nID,int nStep,CString strFunc,int nSumLeaf, CTreeCtrl& tree, CStrs& sLeafs, HTREEITEM hItem)
{
	sLeafs.clear();
	if (nID < 0)
	{
		if (GetLeafsValue(tree, hItem, nSumLeaf, sLeafs) == false)
			return false;
	}
	else
	{
		if (nSumLeaf != _XMLImg[nID][nStep].psLeaf.size())
		{
			LogFile("%s XML文件节点数不匹配！", strFunc);
			return false;
		}
		for (int i = 0; i < nSumLeaf; i++)
		{
			sLeafs.push_back(_XMLImg[nID][nStep].psLeaf[i].strMN);
		}
	}
	return true;
}

CString CSingleton::GetHTupleString(HTuple& hvRes)
{
	CString strData;
	int iRes = hvRes.TupleLength().I();
	for (int j = 0; j < iRes; j++)
	{
		CString str, sText;
		double fVal;
		if (hvRes[j] == "," || hvRes[j] == ";")
		{
			strData.TrimRight("*");
			str.Format("%s", hvRes[j].S().Text());
		}
		else
		{
			fVal = hvRes[j].D();
			str.Format("%.2f*", fVal);
		}
		strData.Append(str);
	}
	strData.TrimRight(";");  //不需要最后一个分号
	strData.TrimRight("*");
	return strData;
}

void CSingleton::GetAffineRect(double fRCVal[], CString strPosTYP, HTuple& hvRes, HTuple& hvTransX, HTuple& hvTransY)
{
	CSingleton* pSng = CSingleton::GetInstance();
	HTuple hvMat2D;
	CStrs ssPosTYP;
	SplitString(strPosTYP, ",", ssPosTYP);
	if (strPosTYP == "固定点" || strPosTYP == "固定位")
	{
		hvTransX = fRCVal[0];
		hvTransY = fRCVal[1];
	}
	else if (ssPosTYP[0] == "模板差")
	{
		double fMT[3], fRes[2];
		fMT[0] = hvRes[0].D();
		fMT[1] = hvRes[1].D();
		fMT[2] = hvRes[2].D();
		VectorAngleToRigid(0, 0, 0, fMT[1] * ZOOM, fMT[0] * ZOOM, fMT[2], &hvMat2D);
		AffineTransPoint2d(hvMat2D, fRCVal[1], fRCVal[0], &hvTransY, &hvTransX);
		fRes[0] = hvTransX.D() / ZOOM;
		fRes[1] = hvTransY.D() / ZOOM;
		hvTransX = fRes[0] ;
		hvTransY = fRes[1] ;
	}
	else if (ssPosTYP[0] == "结果点")
	{
		hvTransX = hvRes[0];
		hvTransY = hvRes[1];
		fRCVal[2] = hvRes[2].D();
	}
}


bool CSingleton::GetDrawCircle(CHWin& HW, CTreeCtrl& tree, double fXYR[], CString strType, HTREEITEM& hChildItem, HTuple& hvRes)
{
	CSingleton* pSng = CSingleton::GetInstance();
	HTuple hvR, hvC, hvRadius;
	int nPosTxt[2] = { 0 };
	HW.WinTexts.clear();
	HW.ShowImage(nPosTxt,"绘制圆区域：");
	DrawCircle(HW.WndID, &hvR, &hvC, &hvRadius);
	fXYR[0] = hvC.D();
	fXYR[1] = hvR.D();
	fXYR[2] = hvRadius.D();
	CStringArray ss;
	if (strType == "固定位" || strType == "固定点")                                  //固定区域点
	{
		for (int j = 0; j < 3; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			strTreeVal.Format("%.2f", fXYR[j]);
			tree.SetItemText(hChildItem, ss[0] + "=" + strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
	}
	////////////////////以下是模板差///////////////
	else if (strType.Left(6) == "模板差")                                                    //固定区域点
	{
		fXYR[0] = ZOOM*(hvC.D() - hvRes[0].D());                //相对于第一个搜索到的模板
		fXYR[1] = ZOOM*(hvR.D() - hvRes[1].D());                //（检测框中心点）相对于模板点的的偏差
		for (int j = 0; j < 3; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			if (j < 2)
			{
				strTreeVal.Format("%s=%.2f", ss[0], fXYR[j] / ZOOM);
			}
			else
				strTreeVal.Format("%s=%.2f", ss[0], fXYR[j]);
			tree.SetItemText(hChildItem, strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
	}
	else if (strType.Left(6) == "结果点")
	{
		for (int j = 0; j < 3; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			if (j < 2)
			{
				strTreeVal.Format("%s=0", ss[0]);
			}
			else
				strTreeVal.Format("%s=%.1f", ss[0], fXYR[j]);
			tree.SetItemText(hChildItem, strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
	}
	else
		return false;
	return true;
}

void CSingleton::GetDrawRect2(CHWin& HW, CTreeCtrl& tree, double fRCVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes)
{
	CSingleton* pSng = CSingleton::GetInstance();
	HTuple hvR, hvC, hvPhi, hvLen1, hvLen2;
	int nPosTxt[2] = { 2,2 };
	HW.WinTexts.clear();
	HW.ShowImage(nPosTxt,"绘制区域：");
	DrawRectangle2(HW.WndID, &hvR, &hvC, &hvPhi, &hvLen1, &hvLen2);
	fRCVal[0] = hvC.D();
	fRCVal[1] = hvR.D();
	fRCVal[2] = hvPhi.D();
	fRCVal[3] = hvLen1.D();
	fRCVal[4] = hvLen2.D();
	CStringArray ss;
	if (strPosTYP == "固定点" || strPosTYP == "固定位")                                       //固定区域点
	{
		for (int j = 0; j < 5; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			strTreeVal.Format("%.2f", fRCVal[j]);
			tree.SetItemText(hChildItem, ss[0] + "=" + strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
	}
	else //绘图前提下只有固定点和模板差两种类型
	{
		fRCVal[0] = ZOOM*(hvC.D() - hvRes[0].D());                                       //相对于第一个搜索到的模板
		fRCVal[1] = ZOOM*(hvR.D() - hvRes[1].D());                                       //(检测框中心点）相对于模板点的的偏差
		for (int j = 0; j < 5; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			if (j < 2)
				strTreeVal.Format("%s=%.2f", ss[0], fRCVal[j] / ZOOM);
			else
				strTreeVal.Format("%s=%.2f", ss[0], fRCVal[j]);
			tree.SetItemText(hChildItem, strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
	}
}

void CSingleton::FindAllFile(CString path, CStrs& filenames)
{
	filenames.clear();
	CFileFind finder;
	BOOL working = finder.FindFile(path + "\\*.*");
	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
		{
			//FindAllFile(finder.GetFilePath(), filenames, count);
		}
		else
		{
			CString filename = finder.GetFilePath();
			filenames.push_back(filename);
		}
	}
}

void CSingleton::SetupCallBack(unsigned char ** pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, int II)
{
	if (g_nExt == 1)
		return;
	CString strLog;
	if (pFrameInfo)
	{
		//strLog.Format("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
		//	pFrameInfo->nWidth, pFrameInfo->nHeight, pFrameInfo->nFrameNum);
		if (_iReal[II] > 0)
		{
			GenImage1(&_hoImg[II], "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
			if (_iReal[II] == 1)
			{
				_iReal[II]++;
			}
			return;
		}
		if (_iSnap[II] > 0)
		{
			GenImage1(&_hoImg[II], "byte", pFrameInfo->nWidth, pFrameInfo->nHeight, (Hlong)(*pData));
			_iSnap[II]--;
		}
	}
}

bool CSingleton::CamSnap(CHWin& HW, int nCam, int nTimes)
{
	_iSnap[nCam] = nTimes;
	DWORD st = GetTickCount();
	while (_iSnap[nCam] > 0)
	{
		if (GetTickCount() - st > 2000)
			return false;
		DoEvent();
	}
	_csCam[nCam].Lock();
	//CopyImage(_hoImg[nCam], &HW.hoImage);
	HW.ShowImage(_hoImg[nCam]);
	_csCam[nCam].Unlock();
	return true;
}

bool CSingleton::GetDrawRing(CHWin& HW, CTreeCtrl& tree, double fRadiusVal[], CString strPosTYP, HTREEITEM& hChildItem, HTuple& hvRes)
{
	CSingleton* pSng = CSingleton::GetInstance();
	HTuple hvR, hvC, hvRadius;
	int nPosTxt[2] = { 0 };
	HW.WinTexts.clear();
	HW.ShowImage(nPosTxt,"绘制外圆区域：");
	DrawCircle(HW.WndID, &hvR, &hvC, &hvRadius);
	fRadiusVal[0] = hvC.D();
	fRadiusVal[1] = hvR.D();
	fRadiusVal[2] = hvRadius.D();
	HW.WinTexts.clear();
	HW.ShowImage(nPosTxt,"绘制内圆区域：");
	DrawCircle(HW.WndID, &hvR, &hvC, &hvRadius);
	fRadiusVal[3] = hvRadius.D();
	if (fRadiusVal[3] >= fRadiusVal[2])
	{
		return false;
	}
	CStringArray ss;
	if (strPosTYP == "固定位" || strPosTYP == "固定点")                                                                  //固定区域点
	{
		for (int j = 0; j < 4; j++)
		{
			CString strKnot = tree.GetItemText(hChildItem);
			pSng->SplitString(strKnot, "=", ss);
			CString strTreeVal;
			strTreeVal.Format("%.2f", fRadiusVal[j]);
			tree.SetItemText(hChildItem, ss[0] + "=" + strTreeVal);
			hChildItem = tree.GetNextSiblingItem(hChildItem);
		}
		return true;
	}
	fRadiusVal[0] = ZOOM*(hvC.D() - hvRes[0].D());                //相对于第一个搜索到的模板
	fRadiusVal[1] = ZOOM*(hvR.D() - hvRes[1].D());                //（检测框中心点）相对于模板点的的偏差
	for (int j = 0; j < 4; j++)
	{
		CString strKnot = tree.GetItemText(hChildItem);
		pSng->SplitString(strKnot, "=", ss);
		CString strTreeVal;
		if (j < 2)
			strTreeVal.Format("%s=%.2f", ss[0], fRadiusVal[j] / ZOOM);
		else
			strTreeVal.Format("%s=%.2f", ss[0], fRadiusVal[j]);
		tree.SetItemText(hChildItem, strTreeVal);
		hChildItem = tree.GetNextSiblingItem(hChildItem);
	}
	return true;
}

int CSingleton::ReadFromTCP(CString cStart, long lVal[4])
{
	char cSend[25];
	ZeroMemory(cSend, sizeof(cSend));
	int i = 0;
	cSend[i++] = 'R';
	cSend[i++] = 'D';
	cSend[i++] = 'S';
	cSend[i++] = ' ';
	cSend[i++] = 'D';
	cSend[i++] = 'M';
	cSend[i++] = cStart.GetAt(0);
	cSend[i++] = cStart.GetAt(1);
	cSend[i++] = cStart.GetAt(2);
	cSend[i++] = cStart.GetAt(3);
	cSend[i++] = '.';
	cSend[i++] = 'L';
	cSend[i++] = ' ';
	cSend[i++] = '4';
	cSend[i++] = 0x0D;
	int ret = _PLC.MySend(cSend, 15);
	char RecvBuff[50];
	CString strRecv;
	strRecv.Empty();
	ret = recv(_PLC.m_socket, RecvBuff, 50, 0);
	for (int n = 0; n < ret; n++)
		strRecv.AppendChar(RecvBuff[n]);
	CStringArray strArr;
	strArr.RemoveAll();
	SplitString(RecvBuff, " ", strArr);
	int iCount = strArr.GetCount();
	if (iCount != 4)
		return -1;
	for (int i = 0; i < 4; i++)
		lVal[i] = atol(strArr[i]);
	return ret;
}

int CSingleton::Write2TCP(CString cStart, long lVal)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString str;
	str.Format("写入D%s : Start", cStart);
	LogFile(str);
	char cSend[50];
	ZeroMemory(cSend, sizeof(cSend));
	int i = 0;
	cSend[i++] = 'W';
	cSend[i++] = 'R';
	cSend[i++] = ' ';
	cSend[i++] = 'D';
	cSend[i++] = 'M';
	cSend[i++] = cStart.GetAt(0);
	cSend[i++] = cStart.GetAt(1);
	cSend[i++] = cStart.GetAt(2);
	cSend[i++] = cStart.GetAt(3);
	cSend[i++] = '.';
	cSend[i++] = 'L';
	cSend[i++] = ' ';
	CString strData;
	strData.Format("%d", lVal);
	int iLen = strData.GetLength();
	for (int n = 0; n < iLen; n++)
		cSend[i++] = strData.GetAt(n);
	cSend[i++] = 0x0D;
	int ret = pSng->_PLC.MySend(cSend, 13 + iLen);
	char RecvBuff[50];
	CString strRecv;
	strRecv.Empty();
	ret = recv(pSng->_PLC.m_socket, RecvBuff, 50, 0);
	for (int n = 0; n < ret - 2; n++)
		strRecv.AppendChar(RecvBuff[n]);
	if (strRecv.Compare("OK") == 0)
	{
		str.Format("写入D%s : %d", cStart, lVal);
		LogFile(str);
		return ret;
	}
	else
	{
		ret = -1;
		str.Format("写入D%s : 失败", cStart);
		LogFile(str);
		return ret;
	}
	//strRecv.Format("%s",RecvBuff);
	//////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
}