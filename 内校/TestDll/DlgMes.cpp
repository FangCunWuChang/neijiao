// DlgMes.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDll.h"
#include "DlgMes.h"
#include "afxdialogex.h"
#include "..\DllMod\TCPSocket.h"
#include "..\DllMod\IHpDllWin.h"
#include "..\DllMod\IDlgInfo.h"
#include "DlgOffset.h"
#include "ImgDLL.h"
#include "MESKEY.h"
#include "VAL.h"
#include "MESSZ.h"


#include "WininetHttp.h"
#include <json/json.h>

#include <fstream>
#pragma comment(lib, "Wininet.lib")
#include <tchar.h>
#include "Hc_Modbus_Api.h"
#pragma comment(lib, "StandardModbusApi.lib")
using namespace std;

/** Use Log Output Widget */
#define MESPRINT(_forStr, ...) \
	do \
	{ \
		CString _str; \
		_str.Format(CString(_forStr).GetBuffer(), __VA_ARGS__); \
		\
		auto* _logEditor = dynamic_cast<CEdit*>(GetDlgItem(IDC_LOG)); \
		if (_logEditor) \
		{ \
			CString _currStr; \
			_logEditor->GetWindowText(_currStr); \
			_logEditor->SetWindowText(_currStr + "\r\n" + _str); \
		} \
	} \
	while (false)

/** Wrap Current Log Output */
#define MESLOG(_forStr, ...) \
	do \
	{ \
		CImgDLL::WriteLog((char*)(CString(_forStr).GetBuffer()), __VA_ARGS__); \
		MESPRINT(_forStr, __VA_ARGS__); \
	} \
	while (false)

extern int g_nExt;
extern void DoEvent();

// CDlgMes �Ի���
UINT ThreadWatchIP(LPVOID p);
UINT ThreadWatchCheng(LPVOID p);
UINT ThreadWatchZERO(LPVOID p);
UINT ThreadIP(LPVOID p);
UINT ThreadCheng(LPVOID p);
UINT ThreadZERO(LPVOID p);
UINT ThreadPLC(LPVOID p);
UINT ThreadSLEEP(LPVOID p);
UINT ThreadSLEEP1(LPVOID p);
UINT ThreadSLEEP2(LPVOID p);
IMPLEMENT_DYNAMIC(CDlgMes, CDialogEx)

CDlgMes::CDlgMes(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMes::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pServer[0] = new CTCPSocket(TCP_SOCKET_CLIENT);
	m_pServer[1] = new CTCPSocket(TCP_SOCKET_CLIENT);
	m_pServer[2] = new CTCPSocket(TCP_SOCKET_CLIENT);
}

CDlgMes::~CDlgMes()
{
}

void CDlgMes::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_val[0]);
	DDX_Control(pDX, IDC_MSCOMM2, m_val[1]);
	DDX_Control(pDX, IDC_MSCOMM3, m_val[2]);
	DDX_Control(pDX, IDC_COMBO_LIAO, m_comboLiao);

}


BEGIN_MESSAGE_MAP(CDlgMes, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_RUN, &CDlgMes::OnBnClickedBtnRun)
	ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_VAL, &CDlgMes::OnBnClickedBtnVal)
ON_BN_CLICKED(IDC_BTN_LOG, &CDlgMes::OnBnClickedBtnLog)
ON_BN_CLICKED(IDC_BTN_ZERO, &CDlgMes::OnBnClickedBtnZero)
ON_BN_CLICKED(IDC_BTN_OKZERO, &CDlgMes::OnBnClickedBtnOkzero)
ON_BN_CLICKED(IDC_BTN_JZ, &CDlgMes::OnBnClickedBtnJz)
ON_BN_CLICKED(IDOK, &CDlgMes::OnBnClickedOk)
ON_BN_CLICKED(IDC_BTN_JZ1, &CDlgMes::OnBnClickedBtnJz1)
ON_BN_CLICKED(IDC_BTN_JZ2, &CDlgMes::OnBnClickedBtnJz2)
ON_BN_CLICKED(IDC_BTN_JZ3, &CDlgMes::OnBnClickedBtnJz3)
ON_BN_CLICKED(IDC_BTN_MES, &CDlgMes::OnBnClickedBtnMes)
ON_BN_CLICKED(IDC_CLEARLOG, &CDlgMes::OnBnClickedClearlog)
END_MESSAGE_MAP()


// CDlgMes ��Ϣ�������

bool CDlgMes::InitialCom(CMscomm1 &Com, int iNO, int nBaud, int nLen)
{
	Com.put_CommPort(iNO);                             //�趨����Ϊ?COM1?
	CString strBaud, strSetting;
	strSetting.Format(_T("%d,n,8,1"), nBaud);
	Com.put_Settings(strSetting);                    //�趨������9600������żУ�飬8λ����λ��1��Ϊֹͣλ?????????
	Com.put_InputMode(1);                            //�趨���ݽ���ģʽ��1Ϊ�����ƣ�0Ϊ�ı�?????????
	Com.put_InputLen(0);                              //�趨��ǰ���������ݳ���?9
	Com.put_InBufferSize(1024);                     //�������뻺������СΪ1024?byte?????????
	Com.put_OutBufferSize(1024);                   //���������������СΪ1024?byte?????????
	Com.put_RThreshold(nLen);                         //ÿ���յ�һ���ַ�ʱ������OnComm�¼�?????????
	Com.put_SThreshold(0);                         //ÿ����һ���ַ�ʱ��������OnComm�¼�
	if (!Com.get_PortOpen())
	{
		try
		{
			Com.put_PortOpen(true);//�򿪴���
		}
		catch (CException* e)
		{
			TCHAR   szError[1024];
			e->GetErrorMessage(szError, 1024);         //e.GetErrorMessage(szError,1024); 
			LogFile(szError);
			return false;
		}
		LogFile("���ڴ򿪳ɹ���");
		return true;
	}
	LogFile("���ڴ�ʧ�ܣ�");
	return false;
}


BOOL CDlgMes::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CSingleton* pSng = CSingleton::GetInstance();
	if (CImgDLL::Init() == FALSE)
	{
		MessageBox("����ͼ���DLLʧ��!");
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	//if (CImgDLL::SvrLink(pSng->_iSvrPort) == false)
	//{
	//	SendMessage(WM_CLOSE);
	//	return FALSE;
	//}
	CString strServer[3],strsName[3];
	BOOL b[3];

	m_comboLiao.AddString("111");
	pSng->strLiaohao = "111";
	for (int i = 0; i < 3; i++)
	{
		strsName[i].Format("IP%d",i+1);
		strServer[i] = pSng->GetCfgString("ADDR", strsName[i], "192.168.59.151");
		b[i] = m_pServer[i]->Connect(strServer[i], 2000);
		m_pServer[i]->m_nID=i;
		if (b[i] == FALSE)
		{
			if (i == 0)
				SetTimer(TIME_LINK1, 500, NULL);
			else if (i == 1)
				SetTimer(TIME_LINK2, 500, NULL);
			else if (i == 2)
				SetTimer(TIME_LINK3, 500, NULL);
		}
		else
		{
			m_pServer[i]->StartReceiving(NULL, NULL, NULL);
			MESLOG("���ӷ�����%d�ɹ���", i+1);
		}
	}
	int bComOK = 0;
	bComOK=InitialCom(m_val[0], 2, 19200, 1);
	MESLOG("����1���%d", bComOK);
	bComOK=InitialCom(m_val[1], 3, 19200, 1);
	MESLOG("����2���%d", bComOK);
	bComOK=InitialCom(m_val[2], 4, 19200, 1);
	MESLOG("����3���%d", bComOK);
	m_nIP[0] = 0;
	m_nIP[1] = 0;
	m_nIP[2] = 0;
	m_nIP[0] = 0;
	m_nIP[1] = 0;
	m_nIP[2] = 0;
	bVal[0] = false;
	bVal[1] = false;
	bVal[2] = false;
	bWZERO[0] = false;
	bWZERO[1] = false;
	bWZERO[2] = false;
	bZERO[0] = false;
	bZERO[1] = false;
	bZERO[2] = false;
	pSng->_pMain = this;
	int nNetId = 0;
	int nIpPort = pSng->GetCfgInt("����", "PORT", 502);
	BOOL  bRet;
	pSng->_strIP = pSng->GetCfgString("����", "IP1", "192.168.3.100");
	bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
	pSng->_strIP.ReleaseBuffer();
	if (!bRet)
	{
		AfxMessageBox(_T("PLC����1ʧ��"));
		SendMessage(WM_CLOSE);
		return FALSE;
	}
	//nIpPort = 503;
	//pSng->_strIP = pSng->GetCfgString("����", "IP2", "192.168.3.101");
	//bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
	//pSng->_strIP.ReleaseBuffer();
	//if (!bRet)
	//{
	//	AfxMessageBox(_T("PLC����2ʧ��"));
	//	SendMessage(WM_CLOSE);
	//	return FALSE;
	//}
	AfxBeginThread(ThreadWatchIP, this);
	AfxBeginThread(ThreadWatchCheng, this);
	AfxBeginThread(ThreadWatchZERO, this);
	AfxBeginThread(ThreadIP, this);
	AfxBeginThread(ThreadCheng, this);
	AfxBeginThread(ThreadZERO, this);
	AfxBeginThread(ThreadPLC, this);
	AfxBeginThread(ThreadSLEEP, this);
	AfxBeginThread(ThreadSLEEP1, this);
	AfxBeginThread(ThreadSLEEP2, this);
	SetTimer(TIME_VAL1, 500, NULL);
	SetTimer(TIME_VAL2, 500, NULL);
	SetTimer(TIME_VAL3, 500, NULL);
	GetDlgItem(IDC_BTN_JZ1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_JZ2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_JZ3)->EnableWindow(FALSE);
	int nOK[3], nNG[3];
	nOK[0] = atoi(pSng->GetCfgString("��λ1", "OK", "111"));
	nOK[1] = atoi(pSng->GetCfgString("��λ2", "OK", "111"));
	nOK[2] = atoi(pSng->GetCfgString("��λ3", "OK", "111"));
	nNG[0] = atoi(pSng->GetCfgString("��λ1", "NG", "111"));
	nNG[1] = atoi(pSng->GetCfgString("��λ2", "NG", "111"));
	nNG[2] = atoi(pSng->GetCfgString("��λ3", "NG", "111"));
	float fOK[3];
	if (nOK[0] + nNG[0]==0)
	{
		fOK[0] = 0.00;
	}
	else
	{
		fOK[0] = float(nOK[0] * 100) / float((nOK[0] + nNG[0]));
	}
	if (nOK[1] + nNG[1] == 0)
	{
		fOK[1] = 0.00;
	}
	else
	{
		fOK[1] = float(nOK[1] * 100) / float((nOK[1] + nNG[1]));
	}
	if (nOK[2] + nNG[2] == 0)
	{
		fOK[2] = 0.00;
	}
	else
	{
		fOK[2] = float(nOK[2] * 100) / float((nOK[2] + nNG[2]));
	}
	CString strOK[3];
	strOK[0].Format("%.2f", fOK[0]);
	strOK[1].Format("%.2f", fOK[1]);
	strOK[2].Format("%.2f", fOK[2]);
	SetDlgItemText(IDC_STATIC_IN7, pSng->GetCfgString("��λ1", "OK", "111"));
	SetDlgItemText(IDC_STATIC_IN8, pSng->GetCfgString("��λ1", "NG", "111"));
	SetDlgItemText(IDC_STATIC_IN9, strOK[0]);
	SetDlgItemText(IDC_STATIC_IN10, pSng->GetCfgString("��λ2", "OK", "111"));
	SetDlgItemText(IDC_STATIC_IN11, pSng->GetCfgString("��λ2", "NG", "111"));
	SetDlgItemText(IDC_STATIC_IN12, strOK[1]);
	SetDlgItemText(IDC_STATIC_IN13, pSng->GetCfgString("��λ3", "OK", "111"));
	SetDlgItemText(IDC_STATIC_IN14, pSng->GetCfgString("��λ3", "NG", "111"));
	SetDlgItemText(IDC_STATIC_IN15, strOK[2]);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BEGIN_EVENTSINK_MAP(CDlgMes, CDialogEx)
	ON_EVENT(CDlgMes, IDC_MSCOMM1, 1, CDlgMes::OnCommMscomm1, VTS_NONE)
	ON_EVENT(CDlgMes, IDC_MSCOMM2, 1, CDlgMes::OnCommMscomm2, VTS_NONE)
	ON_EVENT(CDlgMes, IDC_MSCOMM3, 1, CDlgMes::OnCommMscomm3, VTS_NONE)
END_EVENTSINK_MAP()

bool CDlgMes::Check_Json(std::string& str)
{
	CSingleton* pSng = CSingleton::GetInstance();
	Json::Reader reader;
	Json::Value root;
	CString cstr;
	cstr.Format("TRACE ���� �쳣");
	if (reader.parse(str, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��  
	{
		//std::string upload_id = root["uploadid"].asString();  // ���ʽڵ㣬upload_id = "UP000000"  
		string strtoken = root["Token:"].asString();                              //  ���ʽڵ㣬code = 100
		string strContact = root["}"].asString();
		string strError = root["error"].asString();
		if (strtoken != "")
		{
			cstr.Format("TRACE ���� �ɹ�");
			MESLOG(cstr + "\nid = " + pSng->Str2Cstr(strtoken));
			strMEStoken = pSng->Str2Cstr(strtoken);
			return true;
		}
		else
		{
			MESLOG(cstr +
				"\ncontact = " + pSng->Str2Cstr(strContact) +
				"\nerror = " + pSng->Str2Cstr(strError));
			MessageBox(cstr +
				"\ncontact = " + pSng->Str2Cstr(strContact) +
				"\nerror = " + pSng->Str2Cstr(strError));
			return false;
		}
	}
	MESLOG(cstr + "\nMES���ݸ�ʽ�����޷�����" + "\n" + pSng->Str2Cstr(str));
	MessageBox(cstr + "\nMES���ݸ�ʽ�����޷�����" + "\n" + pSng->Str2Cstr(str));
	return false;
}

string CDlgMes::GetTimeMillisecondsStr()
{

	SYSTEMTIME st;
	CString strDate;
	GetLocalTime(&st);

	//�õ����ڵ��ַ���

	CString strTime;
	strTime.Format("%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//string strDateTime = ValueToStr(st.wYear) + "-" +
	//	ValueToStr(st.wMonth) + "-" +
	//	ValueToStr(st.wDay) + " " +
	//	ValueToStr(st.wHour) + ":" +
	//	ValueToStr(st.wMinute) + ":" +
	//	ValueToStr(st.wSecond) /*+ "." +
	//	ValueToStr(st.wMilliseconds)*/;
	string str = strTime.GetBuffer();
	strTime.ReleaseBuffer();
	return str;
}

bool CDlgMes::MES1(CString strmoid, CString strpartID, CString strppid, CString strtestStation)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CWininetHttp whttp = CWininetHttp();
	string url = "http://192.168.180.131:8091/JavaInterfaces/UniServices.asmx";
	CString strMXL;
	strMXL.Format("<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:tem=\"http://tempuri.org/\">\
<soap:Header/>\
<soap:Body>\
<tem:UniRequest>\
<tem:rb>\
<tem:account>user</tem:account>\
<tem:company>avcwh</tem:company>\
<tem:ip>192.168.180.131</tem:ip>\
<tem:optype>0</tem:optype>\
<tem:param> { \"moid\":\"%s\", \"partID\" : \"%s\", \"ppid\" : \"%s\", \"testStation\" : \"%s\"}</tem:param>\
<tem:password>123456</tem:password>\
<tem:sericeName>GET_PROCESS_STATUS</tem:sericeName>\
<tem:sysType>\"\"</tem:sysType>\
</tem:rb>\
</tem:UniRequest>\
</soap:Body>\
</soap:Envelope>", strmoid, strpartID, strppid, strtestStation);
	string xml_string;
	xml_string = strMXL.GetBuffer();
	strMXL.ReleaseBuffer();
	std::ofstream ofs;
	CString strData;
	strData.Format("D:\\DATA\\1\\");
	pSng->CreateDir("D:\\DATA");                          //����һ��Ŀ¼
	pSng->CreateDir("D:\\DATA\\1");                          //����һ��Ŀ¼
	CTime StartTime = CTime::GetCurrentTime();
	CString strStartTime = StartTime.Format("%Y-%m-%d %H_%M_%S");
	CString strWrite = strData + strStartTime + "write.xml";
	ofs.open(strWrite);
	ofs << xml_string;
	ofs.close();
	std::string xml_header = "Content-Type:text/xml";
	string strxmlRtn = whttp.RequestJsonInfo(url, Hr_Post, xml_header, xml_string);
	CString strName = strData + strStartTime + ".xml";
	ofs.open(strName);
	ofs << strxmlRtn;
	ofs.close();
	bool bRet = Check_Json(strxmlRtn);
	return bRet;
}

bool CDlgMes::MES2(CString strtoken, CString strdeptID, CString strpartID, CString strppid, CString strmoid,
	CString strlineID, CString strtestStation, CString strtestResult, CString strmachineSN,
	CString strtestchannelID, CString strempty, CString strfilling, CString strdegassing, CString strfill_empty,
	CString strill_degass, CString strdegass_empty, CString strempty_p1, CString strempty_l1)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strTime = pSng->Str2Cstr(GetTimeMillisecondsStr());
	CWininetHttp whttp = CWininetHttp();
	string url = "http://192.168.180.131:8091/JavaInterfaces/UniServices.asmx";
	CString strMXL;
	strMXL.Format("<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:tem=\"http://tempuri.org/\">\
<soap:Header/>\
<soap:Body>\
<tem:UniRequest>\
<tem:rb>\
<tem:account>user</tem:account>\
<tem:company>avcwh</tem:company>\
<tem:ip>192.168.180.131</tem:ip>\
<tem:optype>0</tem:optype>\
<tem:param> { \"token\":\"%s\", \"deptID\" : \"%s\", \"partID\" : \"%s\", \"ppid\" : \"%s\",\"mo_id\" : \"%s\",\"lineID\" : \"%s\",\"test_Station\" : \"%s\",\"testTime\" : \"%s\",\"testResult\" : \"%s\",\"machineSN\" : \"%s\",\"testchannelID\" : \"%s\",\"measurementData\" : {\"empty\" : \"%s\",\"filling\" : \"%s\",\"degassing\" : \"%s\",\"fill_empty\" : \"%s\",\"fill_degass\" : \"%s\",\"degass_empty\" : \"%s\",\"degass_empty_p1\" : \"%s\",\"degass_empty_l1\" : \"%s\"}</tem:param>\
<tem:password>123456</tem:password>\
<tem:sericeName>GET_PROCESS_STATUS</tem:sericeName>\
<tem:sysType>\"\"</tem:sysType>\
</tem:rb>\
</tem:UniRequest>\
</soap:Body>\
</soap:Envelope>", strtoken, strdeptID, strpartID, strppid, strmoid, strlineID, strtestStation, strTime, strtestResult, strmachineSN, strtestchannelID, strempty, strfilling, strdegassing, strfill_empty, strill_degass, strdegass_empty, strempty_p1, strempty_l1);
	string xml_string;
	xml_string = strMXL.GetBuffer();
	strMXL.ReleaseBuffer();
	std::ofstream ofs;
	CString strData;
	strData.Format("D:\\DATA\\2\\");
	pSng->CreateDir("D:\\DATA");                          //����һ��Ŀ¼
	pSng->CreateDir("D:\\DATA\\2");                          //����һ��Ŀ¼
	CTime StartTime = CTime::GetCurrentTime();
	CString strStartTime = StartTime.Format("%Y-%m-%d %H_%M_%S");
	CString strWrite = strData + strStartTime + "write.xml";
	ofs.open(strWrite);
	ofs << xml_string;
	ofs.close();
	std::string xml_header = "Content-Type:text/xml";
	string strxmlRtn = whttp.RequestJsonInfo(url, Hr_Post, xml_header, xml_string);
	CString strName = strData + strStartTime + ".xml";
	ofs.open(strName);
	ofs << strxmlRtn;
	ofs.close();
	bool bRet = Check_Json(strxmlRtn);
	return bRet;
}

void CDlgMes::OnBnClickedBtnRun()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText,strTime;
	GetDlgItemText(IDC_BTN_RUN, strText);
	if (strText == "����")
	{
		TV.clear();
		TV1.clear();
		TV2.clear();
		SetDlgItemText(IDC_BTN_RUN, "ֹͣ");
		pSng->_nRun = RUN_WORK;
		//MESLOG("������գ���λ��������");

	}
	else
	{
		SetDlgItemText(IDC_BTN_RUN, "����");
		pSng->_nRun = RUN_NULL;
	}
}


void CDlgMes::OnTimer(UINT_PTR nIDEvent)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strCode[3];
	int nTime;
	nTime = pSng->GetCfgInt("����", "ɨ��ʱ��", 6000);
	if (nIDEvent == TIME_IP1)
	{
		int nOK = atoi(pSng->GetCfgString("��λ1", "OK", "111"));
		int nNG = atoi(pSng->GetCfgString("��λ1", "NG", "111"));
		if (GetTickCount() - dwIP[0] > nTime || pSng->_strCliRobot[0] == "R")
		{
			nNG += 1;
			KillTimer(TIME_IP1);
			m_nIP[0] = 2;
			CString strData;
			strData.Format("LOF");
			int nLen = strData.GetLength();
			//pSng->_csClient[0].Lock();
			m_pServer[0]->SendClient(strData.GetBuffer(0), nLen);
			strData.ReleaseBuffer();	
			//pSng->_csClient[0].Unlock();
			MESLOG("�չ�ɨ�볬ʱ��������");
		}
		else
		{
			if (pSng->_strCliRobot[0].GetLength() > 3)
			{
				//nOK += 1;
				MESLOG("��վ1ɨ��ɹ���" + pSng->_strCliRobot[0]);
				/*CString strData;
				strData.Format("LOF");
				int nLen = strData.GetLength();
				pSng->_csClient[0].Lock();
				m_pServer[0]->SendClient(strData.GetBuffer(0), nLen);
				strData.ReleaseBuffer();
				pSng->_csClient[0].Unlock();
				m_nIP[0] = 1;*/
				CString strmoid = pSng->GetCfgString("MES", "mo_id", "111");
				CString strpartID = pSng->GetCfgString("MES", "partID", "111");
				CString strppid = pSng->_strCliRobot[0];
				CString strtestStation = pSng->GetCfgString("MES", "testStation", "111");
				KillTimer(TIME_IP1);
				/*SetDlgItemText(IDC_STATIC_IN1, pSng->_strCliRobot[0]);
				CZ a;
				a.strCode.Format("%s",pSng->_strCliRobot[0]);
				pSng->_csTV.Lock();
				TV.push_back(a);
				pSng->_csTV.Unlock();
				MESLOG("��վ1����һ��δ����");
				CString sql, strNum;
				pSng->_csInfo.Lock();
				sql.Format("Select count(*) from WH where IP = '%s'", pSng->_strCliRobot[0]);
				pSng->_DB.ExecuteQueryValue(sql, strNum);
				pSng->_csInfo.Unlock();
				if (atoi(strNum) > 0)*/
				bool bMESOK = MES1(strmoid, strpartID, strppid, strtestStation);
				if (bMESOK == false)
				{
					nNG += 1;
					m_nIP[0] = 2;
				}
				else
				{
					//MESLOG("�������ݣ���������");
					nOK += 1;
					CString strData;
					strData.Format("LOF");
					int nLen = strData.GetLength();
					m_pServer[0]->SendClient(strData.GetBuffer(0), nLen);
					strData.ReleaseBuffer();
					m_nIP[0] = 1;
					SetDlgItemText(IDC_STATIC_IN1, pSng->_strCliRobot[0]);
					CZ a;
					a.strCode.Format("%s", pSng->_strCliRobot[0]);
					pSng->_csTV.Lock();
					TV.push_back(a);
					pSng->_csTV.Unlock();
					CString sql, strNum;
					pSng->_csInfo.Lock();
					sql.Format("Select count(*) from WH where IP = '%s'", pSng->_strCliRobot[0]);
					pSng->_DB.ExecuteQueryValue(sql, strNum);
					pSng->_csInfo.Unlock();
					if (atoi(strNum) > 0)
					{
						MESLOG("�������ݣ���������");
						pSng->_csInfo.Lock();
						sql.Format("Delete WH where IP = '%s' ", pSng->_strCliRobot[0]);
						pSng->_DB.Execute(sql);
						pSng->_csInfo.Unlock();
					}
					pSng->_csInfo.Lock();
					//sql.Format("Delete from WH where IP = '%s' ", pSng->_strCliRobot[0]);
					sql.Format("insert into WH values ('%s','0.00','0.00','0.00','%s')", pSng->_strCliRobot[0], strMEStoken);
					strMEStoken = "";
					pSng->_DB.Execute(sql);
					pSng->_csInfo.Unlock();
					pSng->_strCliRobot[0].Empty();
				}
				/*pSng->_csInfo.Lock();
				sql.Format("insert into WH values ('%s','0.00','0.00','0.00')", pSng->_strCliRobot[0]);
				pSng->_DB.Execute(sql);
				pSng->_csInfo.Unlock();
				pSng->_strCliRobot[0].Empty();*/
			}
		}
		float fOK = float(nOK * 100) / float((nOK + nNG));
		CString strOK[3];
		strOK[0].Format("%d", nOK);
		strOK[1].Format("%d", nNG);
		strOK[2].Format("%.2f", fOK);
		SetDlgItemText(IDC_STATIC_IN7, strOK[0]);
		SetDlgItemText(IDC_STATIC_IN8, strOK[1]);
		SetDlgItemText(IDC_STATIC_IN9, strOK[2]);
		pSng->SetCfgString("��λ1", "OK", strOK[0]);
		pSng->SetCfgString("��λ1", "NG", strOK[1]);
	}
	if (nIDEvent == TIME_IP2)
	{
		int nOK = atoi(pSng->GetCfgString("��λ2", "OK", "111"));
		int nNG = atoi(pSng->GetCfgString("��λ2", "NG", "111"));
		if (GetTickCount() - dwIP[1] > nTime || pSng->_strCliRobot[1] == "R")
		{
			nNG += 1;
			KillTimer(TIME_IP2);
			m_nIP[1] = 2;
			CString strData;
			strData.Format("LOF");
			int nLen = strData.GetLength();
			//pSng->_csClient[1].Lock();
			m_pServer[1]->SendClient(strData.GetBuffer(0), nLen);
			strData.ReleaseBuffer();
			//pSng->_csClient[1].Unlock();
			MESLOG("עˮɨ�볬ʱ��������");
		}
		else
		{
			if (pSng->_strCliRobot[1].GetLength() > 3)
			{
				nOK += 1;
				MESLOG("��վ2ɨ��ɹ���" + pSng->_strCliRobot[1]);
				CString strData;
				strData.Format("LOF");
				int nLen = strData.GetLength();
				//pSng->_csClient[1].Lock();
				m_pServer[1]->SendClient(strData.GetBuffer(0), nLen);
				strData.ReleaseBuffer();
				//pSng->_csClient[1].Unlock();
				m_nIP[1] = 1;
				KillTimer(TIME_IP2);
				SetDlgItemText(IDC_STATIC_IN2, pSng->_strCliRobot[1]);
				CZ a;
				a.strCode.Format("%s", pSng->_strCliRobot[1]);
				pSng->_csTV.Lock();
				TV1.push_back(a);
				MESLOG("��վ2����һ��δ����");
				pSng->_csTV.Unlock();
				CString sql, strNum;
				pSng->_csInfo.Lock();
				sql.Format("Select count(*) from WH where IP = '%s'", pSng->_strCliRobot[1]);
				pSng->_DB.ExecuteQueryValue(sql, strNum);
				pSng->_csInfo.Unlock();
				if (atoi(strNum) < 1)
				{
					MESLOG("���������ݣ���������");
				}
				pSng->_strCliRobot[1].Empty();
			}
		}
		float fOK = float(nOK * 100) / float((nOK + nNG));
		CString strOK[3];
		strOK[0].Format("%d", nOK);
		strOK[1].Format("%d", nNG);
		strOK[2].Format("%.2f", fOK);
		SetDlgItemText(IDC_STATIC_IN10, strOK[0]);
		SetDlgItemText(IDC_STATIC_IN11, strOK[1]);
		SetDlgItemText(IDC_STATIC_IN12, strOK[2]);
		pSng->SetCfgString("��λ2", "OK", strOK[0]);
		pSng->SetCfgString("��λ2", "NG", strOK[1]);
	}
	if (nIDEvent == TIME_IP3)
	{
		int nOK = atoi(pSng->GetCfgString("��λ3", "OK", "111"));
		int nNG = atoi(pSng->GetCfgString("��λ3", "NG", "111"));
		//pSng->_strCliRobot[2] = "D4V3103H1QD1RM18L";
		if (GetTickCount() - dwIP[2] > nTime || pSng->_strCliRobot[2] == "R")
		{
			nNG += 1;
			KillTimer(TIME_IP3);
			m_nIP[2] = 2;
			CString strData;
			strData.Format("LOF");
			int nLen = strData.GetLength();
			//pSng->_csClient[2].Lock();
			m_pServer[2]->SendClient(strData.GetBuffer(0), nLen);
			strData.ReleaseBuffer();
			//pSng->_csClient[2].Unlock();
			MESLOG("����ɨ�볬ʱ��������");
		}
		else
		{
			if (pSng->_strCliRobot[2].GetLength() > 3)
			{
				nOK += 1;
				MESLOG("��վ3ɨ��ɹ���" + pSng->_strCliRobot[2]);
				CString strData;
				strData.Format("LOF");
				int nLen = strData.GetLength();
				//pSng->_csClient[2].Lock();
				m_pServer[2]->SendClient(strData.GetBuffer(0), nLen);
				strData.ReleaseBuffer();
				//pSng->_csClient[2].Unlock();
				m_nIP[2] = 1;
				KillTimer(TIME_IP3);
				SetDlgItemText(IDC_STATIC_IN3, pSng->_strCliRobot[2]);
				CZ a;
				a.strCode.Format("%s", pSng->_strCliRobot[2]);
				pSng->_csTV.Lock();
				TV2.push_back(a);
				MESLOG("��վ3����һ��δ����");
				pSng->_csTV.Unlock();
				CString sql, strNum;
				pSng->_csInfo.Lock();
				sql.Format("Select count(*) from WH where IP = '%s'", pSng->_strCliRobot[2]);
				pSng->_DB.ExecuteQueryValue(sql, strNum);
				pSng->_csInfo.Unlock();
				if (atoi(strNum) < 1)
				{
					MESLOG("���������ݣ���������");
				}
				pSng->_strCliRobot[2].Empty();
				//bVal[2] = true;
				//CImgDLL::ClearSvrRev(2);
			}
		}
		float fOK = float(nOK * 100) / float((nOK + nNG));
		CString strOK[3];
		strOK[0].Format("%d", nOK);
		strOK[1].Format("%d", nNG);
		strOK[2].Format("%.2f", fOK);
		SetDlgItemText(IDC_STATIC_IN13, strOK[0]);
		SetDlgItemText(IDC_STATIC_IN14, strOK[1]);
		SetDlgItemText(IDC_STATIC_IN15, strOK[2]);
		pSng->SetCfgString("��λ3", "OK", strOK[0]);
		pSng->SetCfgString("��λ3", "NG", strOK[1]);
	}
	if (nIDEvent == TIME_VAL1)
	{
		SendResult(m_val[0]);
	}
	if (nIDEvent == TIME_VAL2)
	{
		SendResult(m_val[1]);
	}
	if (nIDEvent == TIME_VAL3)
	{
		SendResult(m_val[2]);
	}
	if (nIDEvent == TIME_MES)
	{
		KillTimer(TIME_MES);
		CString sql;
		pSng->_csInfo.Lock();
		sql.Format("Select * from WH where  IP = '%s' ", m_strEX);
		CDStrs strs;
		pSng->_DB.ExecuteQuery(sql, strs);
		pSng->_csInfo.Unlock();
		int iLen = strs.size();
		if (iLen==0)
		{
			MESLOG("��Ч���룡������");
		}
		else
		{
			CString strIP = strs.at(0).at(0);
			CString strVal1 = strs.at(0).at(1);
			CString strVal2 = strs.at(0).at(2);
			CString strVal3 = strs.at(0).at(3);
			CString strtoken = strs.at(0).at(4);
			pSng->Write2CSV(strIP, strVal1, strVal2, strVal3);
			double fVal[3], fCha[3];
			fVal[0] = atof(strVal1);
			fVal[1] = atof(strVal2);
			fVal[2] = atof(strVal3);
			fCha[0] = fVal[1] - fVal[0];
			fCha[1] = fVal[1] - fVal[2];
			fCha[2] = fVal[2] - fVal[0];
			CString strdeptID = pSng->GetCfgString("MES", "deptID", "111");
			CString strpartID = pSng->GetCfgString("MES", "partID", "111");
			CString strmo_id = pSng->GetCfgString("MES", "mo_id", "111");
			CString strlineID = pSng->GetCfgString("MES", "lineID", "111");
			CString strtest_Station = pSng->GetCfgString("MES", "test_Station", "111");
			CString strtestResult = pSng->GetCfgString("MES", "testResult", "111");
			CString strmachineSN = pSng->GetCfgString("MES", "machineSN", "111");
			CString strtestchannelID = pSng->GetCfgString("MES", "testchannelID", "111");
			CString strfill_empty;
			strfill_empty.Format("%.3f", fCha[0]);
			CString strfill_degass;
			strfill_degass.Format("%.3f", fCha[1]);
			CString strdegass_empty;
			strdegass_empty.Format("%.3f", fCha[2]);
			CString strdegass_empty_p1 = pSng->GetCfgString(pSng->strLiaohao, "���������", "2.000");
			CString strdegass_empty_l1 = pSng->GetCfgString(pSng->strLiaohao, "���������", "-1.000");
			bool bMESOK = MES2(strtoken, strdeptID, strpartID, strIP, strmo_id, strlineID, strtest_Station, strtestResult, strmachineSN,
				strtestchannelID, strVal1, strVal2, strVal3, strfill_empty, strfill_degass, strdegass_empty, strdegass_empty_p1, strdegass_empty_l1);
			if (bMESOK == true)
			{
				MESLOG("�ϴ��ɹ�������");
			}
			else
			{
				MESLOG("�ϴ�ʧ�ܣ�����");
			}
		}
	}
	if (nIDEvent == TIME_ZERO1)
	{
		SendZero(m_val[0]);
	}
	if (nIDEvent == TIME_ZERO2)
	{
		SendZero(m_val[1]);
	}
	if (nIDEvent == TIME_ZERO3)
	{
		SendZero(m_val[2]);
	}
	if (nIDEvent == TIME_LINK1)
	{
		KillTimer(TIME_LINK1);
		bool b;
		CString strsName, strServer;
		strsName.Format("IP1");
		strServer = pSng->GetCfgString("ADDR", strsName, "192.168.59.151");
		b = m_pServer[0]->Connect(strServer, 2000);
		m_pServer[0]->m_nID = 0;
		if (b==false)
			SetTimer(TIME_LINK1, 500, NULL);
	}
	if (nIDEvent == TIME_LINK2)
	{
		KillTimer(TIME_LINK2);
		bool b;
		CString strsName, strServer;
		strsName.Format("IP2");
		strServer = pSng->GetCfgString("ADDR", strsName, "192.168.59.151");
		b = m_pServer[1]->Connect(strServer, 2000);
		m_pServer[1]->m_nID = 1;
		if (b == false)
			SetTimer(TIME_LINK2, 500, NULL);
	}
	if (nIDEvent == TIME_LINK3)
	{
		KillTimer(TIME_LINK3);
		bool b;
		CString strsName, strServer;
		strsName.Format("IP3");
		strServer = pSng->GetCfgString("ADDR", strsName, "192.168.59.151");
		b = m_pServer[2]->Connect(strServer, 2000);
		m_pServer[2]->m_nID = 2;
		if (b == false)
			SetTimer(TIME_LINK3, 500, NULL);
	}
	if (nIDEvent == TIME_JZ1)
	{
		SendJz(m_val[0]);
	}
	if (nIDEvent == TIME_JZ2)
	{
		SendJz(m_val[1]);
	}
	if (nIDEvent == TIME_JZ3)
	{
		SendJz(m_val[2]);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CDlgMes::OnCommMscomm1()
{
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_nRun != RUN_WORK)
		return;
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048];                                     //����BYTE����?An?8-bit?integerthat?is?not?signed.
	variant_inp = m_val[0].get_Input();                 //��������
	safearray_inp = variant_inp;                         //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���     
	len = safearray_inp.GetOneDimSize();          //�õ���Ч���ݳ���    // ��������  
	TRACE("\n %d", len);
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //ת��ΪBYTE������     
		BYTE bt = *(char*)(rxdata + k);                     //�ַ��� 	                     
		TRACE(" %02x", bt);
		//LogFile(" %02x", bt);//������ָ��
	}
	if (len == 8)
	{
		BYTE btx, btx1, btx2;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		if (btx == 6 && btx1 == 0 && btx2 == 27)
		{
			MESLOG("��վ1����������ɣ������ж�");
			bWZERO[0] = true;
			KillTimer(TIME_ZERO1);
		}
	}
	if (len == 7)
	{
		BYTE btx, btx1, btx2, btx3;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		btx3 = *(char*)(rxdata + 4);
		if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 0)
		{
			MESLOG("��վ1У�����");
			GetDlgItem(IDC_BTN_JZ1)->EnableWindow(TRUE);
			KillTimer(TIME_JZ1);
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 255)

		{
			MESLOG("��վ1У��ʧ��");
			GetDlgItem(IDC_BTN_JZ1)->EnableWindow(TRUE);
			KillTimer(TIME_JZ1);
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 1)

		{
			MESLOG("��վ1У����������");
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 2)

		{
			MESLOG("��վ1У���˹������׶�");
		}
	}
	if (len !=11)
		return;
	//////////�������IO���Ӳ���Ҫָ����////////////////////
	BYTE btNum[4];                 //�ַ��� 
	btNum[0] = *(char*)(rxdata + 5);                 //�ַ��� 
	btNum[1] = *(char*)(rxdata + 6);                 //�ַ��� 
	btNum[2] = *(char*)(rxdata + 7);                 //�ַ��� 
	btNum[3] = *(char*)(rxdata + 8);                 //�ַ��� 
	float fVal = 0.00;
	BYTE* p = (BYTE*)&fVal;
	p[0] = btNum[3];
	p[1] = btNum[2];
	p[2] = btNum[1];
	p[3] = btNum[0];
	float fA = fVal;
	float fB;
	CString strVal;
	fB = atof(pSng->GetCfgString("VAL����", "��վ1", "0.000"));
	fA += fB;
	strVal.Format("%.3f", fA);
	SetDlgItemText(IDC_STATIC_IN4, strVal);
	if (bVal[0] == true)
	{
		bVal[0] = false;
		if (TV.size()<1)
		{
			m_nVAL[0] = 2;
			return;
		}
		m_nVAL[0] = 1;
		MESLOG("��վ1���سɹ���" + strVal);
		CString sql, strNum;
		TV[0].fVal = fA;
		pSng->_csInfo.Lock();
		sql.Format("Select count(*) from WH where IP = '%s'", TV[0].strCode);
		pSng->_DB.ExecuteQueryValue(sql, strNum);
		pSng->_csInfo.Unlock();
		if (atoi(strNum) < 1)
		{
			MESLOG("���������ݣ���������");
		}
		pSng->_csInfo.Lock();
		sql.Format("Update WH set VAL1 = %.3f where IP = '%s'", fA, TV[0].strCode);
		pSng->_DB.Execute(sql);
		pSng->_csInfo.Unlock();
		pSng->_csTV.Lock();
		TV.erase(TV.begin());
		MESLOG("��վ1ȥ��δ�õ�һ����");
		pSng->_csTV.Unlock();
	}
	if (bZERO[0]==true)
	{
		bZERO[0] = false;
		float fVc1, fVc2;
		fVc1 = atof(pSng->GetCfgString("����", "��������", "-0.010"));
		fVc2 = atof(pSng->GetCfgString("����", "��������", "0.010"));
		if (fA < fVc1|| fA > fVc2)
		{
			MESLOG("��վ1��������ʧ�ܣ��ظ�");
			m_nWZERO[0] = 1;
		}
		else
		{
			MESLOG("��վ1��������ɹ�");
			m_nZERO[0] = 1;
		}
	}
}

/************************************************************************/
/* com2 ��com3 ��485ת232�������¶ȴ�����������Э��ο�������ͨѶ�ĵ�
/************************************************************************/
void CDlgMes::OnCommMscomm2()
{
	// TODO:  �ڴ˴������Ϣ����������
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_nRun != RUN_WORK)
		return;
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048];                                     //����BYTE����?An?8-bit?integerthat?is?not?signed.
	variant_inp = m_val[1].get_Input();                 //��������
	safearray_inp = variant_inp;                         //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���     
	len = safearray_inp.GetOneDimSize();          //�õ���Ч���ݳ���    // ��������  
	TRACE("\n %d", len);
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //ת��ΪBYTE������     
		BYTE bt = *(char*)(rxdata + k);                     //�ַ��� 	                     
		TRACE(" %02x", bt);
		//LogFile(" %02x", bt);//������ָ��
	}
	if (len == 8)
	{
		BYTE btx, btx1, btx2;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		if (btx == 6 && btx1 == 0 && btx2 == 27)
		{
			MESLOG("��վ2����������ɣ������ж�");
			bWZERO[1] = true;
			KillTimer(TIME_ZERO2);
		}
	}
	if (len == 7)
	{
		BYTE btx, btx1, btx2, btx3;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		btx3 = *(char*)(rxdata + 4);
		if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 0)
		{
			MESLOG("��վ2У�����");
			GetDlgItem(IDC_BTN_JZ2)->EnableWindow(TRUE);
			KillTimer(TIME_JZ2);
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 255)

		{
			MESLOG("��վ2У��ʧ��");
			GetDlgItem(IDC_BTN_JZ2)->EnableWindow(TRUE);
			KillTimer(TIME_JZ2);
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 1)

		{
			MESLOG("��վ2У����������");
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 2)

		{
			MESLOG("��վ2У���˹������׶�");
		}
	}
	if (len !=11)
		return;
	//////////�������IO���Ӳ���Ҫָ����////////////////////
	BYTE btNum[4];                 //�ַ��� 
	btNum[0] = *(char*)(rxdata + 5);                 //�ַ��� 
	btNum[1] = *(char*)(rxdata + 6);                 //�ַ��� 
	btNum[2] = *(char*)(rxdata + 7);                 //�ַ��� 
	btNum[3] = *(char*)(rxdata + 8);                 //�ַ��� 
	float fVal = 0.00;
	BYTE* p = (BYTE*)&fVal;
	p[0] = btNum[3];
	p[1] = btNum[2];
	p[2] = btNum[1];
	p[3] = btNum[0];
	float fA = fVal;
	float fB;
	CString strVal;
	fB = atof(pSng->GetCfgString("VAL����", "��վ2", "0.000"));
	fA += fB;
	strVal.Format("%.3f", fA);
	SetDlgItemText(IDC_STATIC_IN5, strVal);
	if (bVal[1] == true)
	{
		bVal[1] = false;
		MESLOG("��վ2��ʼ����");
		//m_nVAL[1] = 1;
		if (TV1.size() < 1)
		{
			m_nVAL[1] = 2;
			MESLOG("��վ2����ɰ�");
			return;
		}
		MESLOG("��վ2���سɹ���" + strVal);
		MESLOG("��վ2����ɹ�");
		CString sql, strNum;
		TV1[0].fVal = fA;
		pSng->_csInfo.Lock();
		sql.Format("Select count(*) from WH where IP = '%s'", TV1[0].strCode);
		pSng->_DB.ExecuteQueryValue(sql, strNum);
		pSng->_csInfo.Unlock();
		if (atoi(strNum) < 1)
		{
			MESLOG("������2���ݣ���������");
		}
		pSng->_csInfo.Lock();
		sql.Format("Update WH set VAL2 = %.3f where IP = '%s'", fA, TV1[0].strCode);
		pSng->_DB.Execute(sql);
		pSng->_csInfo.Unlock();
		pSng->_csInfo.Lock();
		sql.Format("Select * from WH where  IP = '%s' ", TV1[0].strCode);
		CDStrs strs;
		pSng->_DB.ExecuteQuery(sql, strs);
		pSng->_csInfo.Unlock();
		int iLen = strs.size();
		CString strVal1, strVal2;
		strVal1 = strs.at(0).at(1);
		strVal2 = strs.at(0).at(2);
		float fVc, fVc1, fVc2;
		fVc = atof(strVal2) - atof(strVal1);
		fVc1 = atof(pSng->GetCfgString(pSng->strLiaohao, "עˮ������", "1.000"));
		fVc2 = atof(pSng->GetCfgString(pSng->strLiaohao, "עˮ������", "2.000"));
		if (fVc < fVc1 || fVc > fVc2)
		{
			m_nVAL[1] = 2;
			MESLOG("��վ2���س�������");
		}
		else
		{
			m_nVAL[1] = 1;
			MESLOG("��վ2��������");
		}
		pSng->_csTV.Lock();
		TV1.erase(TV1.begin());
		MESLOG("��վ2ȥ��δ�õ�һ����");
		pSng->_csTV.Unlock();
	}
	if (bZERO[1] == true)
	{
		bZERO[1] = false;
		float fVc1, fVc2;
		fVc1 = atof(pSng->GetCfgString("����", "��������", "-0.010"));
		fVc2 = atof(pSng->GetCfgString("����", "��������", "0.010"));
		if (fA < fVc1 || fA > fVc2)
		{
			MESLOG("��վ2��������ʧ�ܣ��ظ�");
			m_nWZERO[1] = 1;
		}
		else
		{
			MESLOG("��վ2��������ɹ�");
			m_nZERO[1] = 1;
		}
	}

}


void CDlgMes::OnCommMscomm3()
{
	// TODO:  �ڴ˴������Ϣ����������
	CSingleton* pSng = CSingleton::GetInstance();
	if (pSng->_nRun != RUN_WORK)
		return;
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048];                                     //����BYTE����?An?8-bit?integerthat?is?not?signed.
	variant_inp = m_val[2].get_Input();                 //��������
	safearray_inp = variant_inp;                         //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���     
	len = safearray_inp.GetOneDimSize();          //�õ���Ч���ݳ���    // ��������  
	TRACE("\n %d", len);
	for (k = 0; k < len; k++)
	{
		safearray_inp.GetElement(&k, rxdata + k);       //ת��ΪBYTE������     
		BYTE bt = *(char*)(rxdata + k);                     //�ַ��� 	                     
		TRACE(" %02x", bt);
		//LogFile(" %02x", bt);//������ָ��
	}
	if (len == 8)
	{
		BYTE btx, btx1, btx2;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		if (btx == 6 && btx1 == 0 && btx2 == 27)
		{
			MESLOG("��վ3����������ɣ������ж�");
			bWZERO[2] = true;
			KillTimer(TIME_ZERO3);
		}
	}
	if (len == 7)
	{
		BYTE btx, btx1, btx2, btx3;
		btx = *(char*)(rxdata + 1);
		btx1 = *(char*)(rxdata + 2);
		btx2 = *(char*)(rxdata + 3);
		btx3 = *(char*)(rxdata + 4);
		if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 0)
		{
			MESLOG("��վ3У�����");
			GetDlgItem(IDC_BTN_JZ3)->EnableWindow(TRUE);
			KillTimer(TIME_JZ3);

		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 255)

		{
			MESLOG("��վ3У��ʧ��");
			GetDlgItem(IDC_BTN_JZ3)->EnableWindow(TRUE);
			KillTimer(TIME_JZ3);
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 1)

		{
			MESLOG("��վ3У����������");
		}
		else if (btx == 3 && btx1 == 2 && btx2 == 0 && btx3 == 2)

		{
			MESLOG("��վ3У���˹������׶�");
		}
	}
	if (len !=11)
		return;
	//////////�������IO���Ӳ���Ҫָ����////////////////////
	BYTE btNum[4];                 //�ַ��� 
	btNum[0] = *(char*)(rxdata + 5);                 //�ַ��� 
	btNum[1] = *(char*)(rxdata + 6);                 //�ַ��� 
	btNum[2] = *(char*)(rxdata + 7);                 //�ַ��� 
	btNum[3] = *(char*)(rxdata + 8);                 //�ַ��� 
	float fVal = 0.00;
	BYTE* p = (BYTE*)&fVal;
	p[0] = btNum[3];
	p[1] = btNum[2];
	p[2] = btNum[1];
	p[3] = btNum[0];
	float fA = fVal;
	float fB;
	CString strVal;
	fB = atof(pSng->GetCfgString("VAL����", "��վ3", "0.000"));
	fA += fB;
	strVal.Format("%.3f", fA);
	SetDlgItemText(IDC_STATIC_IN6, strVal);
	if (bVal[2] == true)
	{
		bVal[2] = false;
		MESLOG("��վ3��ʼ����" );
		//m_nVAL[2] = 1;
		if (TV2.size() < 1)
		{
			m_nVAL[2] = 2;
			MESLOG("��վ3����ɰ�");
			return;
		}
		MESLOG("��վ3���سɹ���" + strVal);
		MESLOG("��վ3����ɹ�");
		CString sql, strNum;
		TV2[0].fVal = fA;
		pSng->_csInfo.Lock();
		sql.Format("Select count(*) from WH where IP = '%s'", TV2[0].strCode);
		pSng->_DB.ExecuteQueryValue(sql, strNum);
		pSng->_csInfo.Unlock();
		if (atoi(strNum) < 1)
		{
			MESLOG("������3���ݣ���������");
		}
		pSng->_csInfo.Lock();
		sql.Format("Update WH set VAL3 = %.3f where IP = '%s'", fA, TV2[0].strCode);
		m_strEX = TV2[0].strCode;
		SetTimer(TIME_MES, 100, NULL);
		pSng->_DB.Execute(sql);
		pSng->_csInfo.Unlock();
		pSng->_csInfo.Lock();
		sql.Format("Select * from WH where  IP = '%s' ", TV2[0].strCode);
		CDStrs strs;
		pSng->_DB.ExecuteQuery(sql, strs);
		pSng->_csInfo.Unlock();
		int iLen = strs.size();
		CString strVal1, strVal2, strVal3;
		strVal1 = strs.at(0).at(1);
		strVal2 = strs.at(0).at(2);
		strVal3 = strs.at(0).at(3);
		float fVc, fVc1, fVc2, fVc3, fVc4, fVc5;
		fVc = atof(strVal2) - atof(strVal3);
		fVc3 = atof(strVal3) - atof(strVal1);
		fVc1 = atof(pSng->GetCfgString(pSng->strLiaohao, "���������", "1.000"));
		fVc2 = atof(pSng->GetCfgString(pSng->strLiaohao, "���������", "2.000"));
		fVc4 = atof(pSng->GetCfgString(pSng->strLiaohao, "���������", "1.000"));
		fVc5 = atof(pSng->GetCfgString(pSng->strLiaohao, "���������", "2.000"));
		if (fVc < fVc1 || fVc > fVc2 || fVc3 < fVc4 || fVc3 > fVc5)
		{
			m_nVAL[2] = 2;
			MESLOG("��վ3���س�������");
		}
		else
		{
			m_nVAL[2] = 1;
			MESLOG("��վ3��������");
		}
		pSng->_csTV.Lock();
		TV2.erase(TV2.begin());
		MESLOG("��վ3ȥ��δ�õ�һ����");
		pSng->_csTV.Unlock();
	}
	if (bZERO[2] == true)
	{
		bZERO[2] = false;
		float fVc1, fVc2;
		fVc1 = atof(pSng->GetCfgString("����", "��������", "-0.010"));
		fVc2 = atof(pSng->GetCfgString("����", "��������", "0.010"));
		if (fA < fVc1 || fA > fVc2)
		{
			MESLOG("��վ3��������ʧ�ܣ��ظ�");
			m_nWZERO[2] = 1;
		}
		else
		{
			MESLOG("��վ3��������ɹ�");
			m_nZERO[2] = 1;
		}
	}
}

void CDlgMes::SendResult(CMscomm1& Com)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	//01 03 1F 40 00 03 03 CB
	if (Com == m_val[0])
	{
		Array.Add(0x01);
		Array.Add(0x03);
		Array.Add(0x1F);
		Array.Add(0x40);
		Array.Add(0x00);
		Array.Add(0x03);
		Array.Add(0x03);
		Array.Add(0xCB);
	}
	//02 03 1F 40 00 03 03 F8
	else if (Com == m_val[1])
	{
		Array.Add(0x02);
		Array.Add(0x03);
		Array.Add(0x1F);
		Array.Add(0x40);
		Array.Add(0x00);
		Array.Add(0x03);
		Array.Add(0x03);
		Array.Add(0xF8);
	}
	//03 03 1F 40 00 03 02 29
	else if (Com == m_val[2])
	{
		Array.Add(0x03);
		Array.Add(0x03);
		Array.Add(0x1F);
		Array.Add(0x40);
		Array.Add(0x00);
		Array.Add(0x03);
		Array.Add(0x02);
		Array.Add(0x29);
	}
	Com.put_InBufferCount(0);
	Com.put_Output(COleVariant(Array));
}

void CDlgMes::SendZero(CMscomm1& Com)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	//01 06 00 1B 00 01 38 0D 
	if (Com == m_val[0])
	{
		Array.Add(0x01);
		Array.Add(0x06);
		Array.Add(0x00);
		Array.Add(0x1B);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0x38);
		Array.Add(0x0D);
	}
	//02 06 00 1B 00 01 38 3E
	else if (Com == m_val[1])
	{
		Array.Add(0x02);
		Array.Add(0x06);
		Array.Add(0x00);
		Array.Add(0x1B);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0x38);
		Array.Add(0x3E);
	}
	//03 06 00 1B 00 01 39 EF
	else if (Com == m_val[2])
	{
		Array.Add(0x03);
		Array.Add(0x06);
		Array.Add(0x00);
		Array.Add(0x1B);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0x39);
		Array.Add(0xEF);
	}
	Com.put_InBufferCount(0);
	Com.put_Output(COleVariant(Array));
}

void CDlgMes::SendJz(CMscomm1& Com)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	if (Com == m_val[0])
	{
		//01 03 01 91 00 01 D4 1B 
		Array.Add(0x01);
		Array.Add(0x03);
		Array.Add(0x01);
		Array.Add(0x91);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0xD4);
		Array.Add(0x1B);
	}
	else if (Com == m_val[1])
	{
		//02 03 01 91 00 01 D4 28
		Array.Add(0x02);
		Array.Add(0x03);
		Array.Add(0x01);
		Array.Add(0x91);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0xD4);
		Array.Add(0x28);
	}
	else if (Com == m_val[2])
	{
		//03 03 01 91 00 01 D5 F9
		Array.Add(0x03);
		Array.Add(0x03);
		Array.Add(0x01);
		Array.Add(0x91);
		Array.Add(0x00);
		Array.Add(0x01);
		Array.Add(0xD5);
		Array.Add(0xF9);
	}
	Com.put_InBufferCount(0);
	Com.put_Output(COleVariant(Array));
}

void CDlgMes::OnBnClickedBtnVal()
{
	VAL dlg;
	if (dlg.DoModal()==IDOK)
	{
	}
}


void CDlgMes::OnBnClickedBtnLog()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CSingleton* pSng = CSingleton::GetInstance();
	pSng->_pLog = (CIDlgInfo*)CImgDLL::LogDlg();
}


void CDlgMes::OnBnClickedBtnZero()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	SetTimer(TIME_ZERO1, 500, NULL);
	SetTimer(TIME_ZERO2, 500, NULL);
	SetTimer(TIME_ZERO3, 500, NULL);
}


void CDlgMes::OnBnClickedBtnOkzero()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CSingleton* pSng = CSingleton::GetInstance();
	CString strZero;
	strZero.Format("0");
	SetDlgItemText(IDC_STATIC_IN7, strZero);
	SetDlgItemText(IDC_STATIC_IN8, strZero);
	SetDlgItemText(IDC_STATIC_IN9, strZero);
	pSng->SetCfgString("��λ1", "OK", strZero);
	pSng->SetCfgString("��λ1", "NG", strZero);
	SetDlgItemText(IDC_STATIC_IN10, strZero);
	SetDlgItemText(IDC_STATIC_IN11, strZero);
	SetDlgItemText(IDC_STATIC_IN12, strZero);
	pSng->SetCfgString("��λ2", "OK", strZero);
	pSng->SetCfgString("��λ2", "NG", strZero);
	SetDlgItemText(IDC_STATIC_IN13, strZero);
	SetDlgItemText(IDC_STATIC_IN14, strZero);
	SetDlgItemText(IDC_STATIC_IN15, strZero);
	pSng->SetCfgString("��λ3", "OK", strZero);
	pSng->SetCfgString("��λ3", "NG", strZero);
}


UINT ThreadWatchCheng(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	SoftElemType eType = REGI_H3U_DW;
	int nAddr[3];
	nAddr[0] = pSng->GetCfgInt("����", "����1��ַ", 110);
	nAddr[1] = pSng->GetCfgInt("����", "����2��ַ", 130);
	nAddr[2] = pSng->GetCfgInt("����", "����3��ַ", 150);
	const int nNetId = 0;
	unsigned short nValueRD[4];
	int nCount = 4;
	BYTE pValue[8];
	bool bPLc = true;
	DWORD dwHeart = GetTickCount();
	for (; g_nExt != 1;)
	{
		Sleep(500);
		if (GetTickCount() - dwHeart > 60000)
		{
			MESLOG("--------��������������---------");
			dwHeart = GetTickCount();
		}
		if (pSng->_nRun == RUN_WORK)
		{
			//if (bPLc == false)
			//{
			//	int nIpPort = pSng->GetCfgInt("����", "PORT", 502);
			//	pSng->_strIP = pSng->GetCfgString("����", "IP", "192.168.3.100");
			//	pSng->_csPLC.Lock();
			//	BOOL  bRet = Exit_ETH(nNetId);
			//	pSng->_csPLC.Unlock();
			//	Sleep(200);
			//	pSng->_csPLC.Lock();
			//	bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
			//	pSng->_csPLC.Unlock();
			//	pSng->_strIP.ReleaseBuffer();
			//}
			for (int i = 0; i < 3; i++)
			{
				pSng->_csPLC.Lock();
				int nRet = H3u_Read_Soft_Elem(eType, nAddr[i], nCount, pValue, nNetId);
				pSng->_csPLC.Unlock();
				if (nRet == 0)
				{
					MESLOG("�޷��յ�PLC���ݣ����������⣡������");
					bPLc = false;
					break;
				}
				memcpy(&nValueRD, pValue, sizeof(nValueRD));
				if (nValueRD[0] == 1)
				{
					MESLOG("�յ�����%d��������", nValueRD[0]);
					ZeroMemory(pValue, sizeof(pValue));
					DWORD deT = GetTickCount();
					int nRetWR = 0;
					while (nRetWR == 0 && GetTickCount() - deT < 3000)
					{
						pSng->_csPLC.Lock();
						if (nAddr[i] == 150)
						{
							int nValue = 1;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, 152, nCount, plValue);
							pDlg->m_nSleep[1] = 1;
						}
						else
						{
							int nValue = 0;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddr[i], nCount, plValue);
						}
						pSng->_csPLC.Unlock();
					}
					if (nRetWR == 0)
					{
						pDlg->m_nWVAL[i] = 0;
						MESLOG("�޷�д��PLC�����������⣡������");
						bPLc = false;
						break;
					}
					else
					{
						pDlg->m_nWVAL[i] = 1;
						MESLOG("���D%d�ɹ������������%d", nAddr[i], nRetWR);
						MESLOG("D%d �յ����ָ��", nAddr[i]);
						bPLc = true;
					}
				}
			}
		}
	}
	return 0;
}

UINT ThreadCheng(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	CString strThread;
	int nCount = 1;
	int nAddrVal[3];
	nAddrVal[0] = pSng->GetCfgInt("����", "��վ1VAL", 111);
	nAddrVal[1] = pSng->GetCfgInt("����", "��վ2VAL", 131);
	nAddrVal[2] = pSng->GetCfgInt("����", "��վ3VAL", 151);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		for (int i = 0; i < 3; i++)
		{
			if (pDlg->m_nWVAL[i] == 0)                           //�յ����ָ��
				continue;
			pDlg->m_nWVAL[i] = 0;
			pDlg->bVal[i] = true;
		}
		for (int j = 0; j < 3; j++)
		{
			if (pDlg->m_nVAL[j] == 0)
				continue;
			int nValue = pDlg->m_nVAL[j];
			pDlg->m_nVAL[j] = 0;
			pSng->_csSvr.Lock();
			BYTE pValue[4] = { 0 };
			memcpy(pValue, &nValue, sizeof(nValue));
			const int nNetId = 0;
			unsigned short nValueRD[4];
			int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddrVal[j], nCount, pValue);
			if (j == 2)
			{
				pDlg->m_nSleep[3] = 1;
			}
			pSng->_csSvr.Unlock();
		}
	}
	return 0;
}

UINT ThreadWatchIP(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	SoftElemType eType = REGI_H3U_DW;
	int nAddr[3];
	nAddr[0] = pSng->GetCfgInt("����", "ɨ��1��ַ", 100);
	nAddr[1] = pSng->GetCfgInt("����", "ɨ��2��ַ", 120);
	nAddr[2] = pSng->GetCfgInt("����", "ɨ��3��ַ", 140);
	const int nNetId = 0;
	unsigned short nValueRD[4];
	int nCount = 1;
	BYTE pValue[8];
	bool bPLc = true;
	DWORD dwHeart = GetTickCount();
	for (; g_nExt != 1;)
	{
		Sleep(500);
		if (GetTickCount() - dwHeart > 60000)
		{
			MESLOG("--------���IP��������---------");
			dwHeart = GetTickCount();
		}
		if (pSng->_nRun == RUN_WORK)
		{
			//if (bPLc == false)
			//{
			//	int nIpPort = pSng->GetCfgInt("����", "PORT", 502);
			//	pSng->_strIP = pSng->GetCfgString("����", "IP", "192.168.3.100");
			//	pSng->_csPLC.Lock();
			//	BOOL  bRet = Exit_ETH(nNetId);
			//	pSng->_csPLC.Unlock();
			//	Sleep(200);
			//	pSng->_csPLC.Lock();
			//	bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
			//	pSng->_csPLC.Unlock();
			//	pSng->_strIP.ReleaseBuffer();
			//}
			for (int i = 0; i < 3; i++)
			{
				pSng->_csPLC.Lock();
				int nRet = H3u_Read_Soft_Elem(eType, nAddr[i], nCount, pValue, nNetId);
				pSng->_csPLC.Unlock();
				if (nRet == 0)
				{
					MESLOG("�޷��յ�PLC���ݣ����������⣡������");
					bPLc = false;
					break;
				}
				memcpy(&nValueRD, pValue, sizeof(nValueRD));
				if (nValueRD[0] == 1)
				{
					MESLOG("�յ�����%d��������", nValueRD[0]);
					ZeroMemory(pValue, sizeof(pValue));
					DWORD deT = GetTickCount();
					int nRetWR = 0;
					while (nRetWR == 0 && GetTickCount() - deT < 3000)
					{
						pSng->_csPLC.Lock();
						if (nAddr[i] == 140)
						{
							int nValue = 1;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, 142, nCount, plValue);
							pDlg->m_nSleep[0] = 1;
						}
						else
						{
							int nValue = 0;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddr[i], nCount, plValue);
						}
						pSng->_csPLC.Unlock();
					}
					if (nRetWR == 0)
					{
						pDlg->m_nWIP[i] = 0;
						MESLOG("�޷�д��PLC�����������⣡������");
						bPLc = false;
						break;
					}
					else
					{
						pDlg->m_nWIP[i] = 1;
						MESLOG("���D%d�ɹ������������%d", nAddr[i], nRetWR);
						MESLOG("D%d �յ����ָ��", nAddr[i]);
						bPLc = true;
					}
				}
			}

		}
	}
	return 0;
}

UINT ThreadIP(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	CString strThread;
	int nCount = 1;
	int nAddrIP[3];
	nAddrIP[0] = pSng->GetCfgInt("����", "��վ1IP", 101);
	nAddrIP[1] = pSng->GetCfgInt("����", "��վ2IP", 121);
	nAddrIP[2] = pSng->GetCfgInt("����", "��վ3IP", 141);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		for (int i = 0; i < 3; i++)
		{
			if (pDlg->m_nWIP[i] == 0)                           //�յ����ָ��
				continue;
			pDlg->m_nWIP[i] = 0;
			pSng->_strCliRobot[i].Empty();
			CString strData;
			strData.Format("LON");
			int nLen = strData.GetLength();
			//pSng->_csClient[i].Lock();
			pDlg->m_pServer[i]->SendClient(strData.GetBuffer(0), nLen);
			strData.ReleaseBuffer();
			//pSng->_csClient[i].Unlock();
			if (i == 0)
			{
				MESLOG("��ʼɨ��1������");
				pDlg->SetTimer(TIME_IP1, 200, NULL);
			}
			else if (i == 1)
			{
				MESLOG("��ʼɨ��2������");
				pDlg->SetTimer(TIME_IP2, 200, NULL);
			}
			else if (i == 2)
			{
				MESLOG("��ʼɨ��3������");
				pDlg->SetTimer(TIME_IP3, 200, NULL);
			}
			pDlg->dwIP[i] = GetTickCount();
		}
		for (int j = 0; j < 3; j++)
		{
			if (pDlg->m_nIP[j] == 0)
				continue;
			int nValue = pDlg->m_nIP[j];
			pDlg->m_nIP[j] = 0;
			pSng->_csSvr.Lock();
			BYTE pValue[4] = { 0 };
			memcpy(pValue, &nValue, sizeof(nValue));
			int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddrIP[j], nCount, pValue);
			if (j == 2)
			{
				pDlg->m_nSleep[2] = 1;
			}
			pSng->_csSvr.Unlock();
		}
	}
	return 0;
}

UINT ThreadWatchZERO(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	SoftElemType eType = REGI_H3U_DW;
	int nAddr[3];
	nAddr[0] = pSng->GetCfgInt("����", "����1��ַ", 115);
	nAddr[1] = pSng->GetCfgInt("����", "����2��ַ", 135);
	nAddr[2] = pSng->GetCfgInt("����", "����3��ַ", 155);
	const int nNetId = 0;
	unsigned short nValueRD[4];
	int nCount = 1;
	BYTE pValue[8];
	bool bPLc = true;
	DWORD dwHeart = GetTickCount();
	for (; g_nExt != 1;)
	{
		Sleep(500);
		if (GetTickCount() - dwHeart > 60000)
		{
			MESLOG("--------���ZERO��������---------");
			dwHeart = GetTickCount();
		}
		if (pSng->_nRun == RUN_WORK)
		{
			if (bPLc == false)
			{
				int nIpPort = pSng->GetCfgInt("����", "PORT", 502);
				pSng->_strIP = pSng->GetCfgString("����", "IP", "192.168.3.100");
				pSng->_csPLC.Lock();
				BOOL  bRet = Exit_ETH(nNetId);
				pSng->_csPLC.Unlock();
				Sleep(200);
				pSng->_csPLC.Lock();
				bRet = Init_ETH_String(pSng->_strIP.GetBuffer(0), nNetId, nIpPort);
				pSng->_csPLC.Unlock();
				pSng->_strIP.ReleaseBuffer();
			}
			for (int i = 0; i < 3; i++)
			{
				pSng->_csPLC.Lock();
				int nRet = H3u_Read_Soft_Elem(eType, nAddr[i], nCount, pValue, nNetId);
				pSng->_csPLC.Unlock();
				if (nRet == 0)
				{
					MESLOG("�޷��յ�PLC���ݣ����������⣡������");
					bPLc = false;
					break;
				}
				memcpy(&nValueRD, pValue, sizeof(nValueRD));
				if (nValueRD[0] == 1)
				{
					MESLOG("�յ�����%d��������", nValueRD[0]);
					ZeroMemory(pValue, sizeof(pValue));
					DWORD deT = GetTickCount();
					int nRetWR = 0;
					while (nRetWR == 0 && GetTickCount() - deT < 3000)
					{
						pSng->_csPLC.Lock();
						if (nAddr[i]==155)
						{
							int nValue = 1;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, 157, nCount, plValue);
							pDlg->m_nSleep[7] = 1;

						}
						else
						{
							int nValue = 0;
							BYTE plValue[4] = { 0 };
							memcpy(plValue, &nValue, sizeof(nValue));
							nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddr[i], nCount, plValue);
						}

					}
					pSng->_csPLC.Unlock();
					if (nRetWR == 0)
					{
						pDlg->m_nWZERO[i] = 0;
						MESLOG("�޷�д��PLC�����������⣡������");
						bPLc = false;
						break;
					}
					else
					{
						pDlg->m_nWZERO[i] = 1;
						MESLOG("���D%d�ɹ������������%d", nAddr[i], nRetWR);
						MESLOG("D%d �յ����ָ��", nAddr[i]);
						bPLc = true;
					}
				}
			}
		}
	}

	return 0;
}

UINT ThreadZERO(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	CString strThread;
	int nCount = 1;
	int nAddrZero[3];
	nAddrZero[0] = pSng->GetCfgInt("����", "��վ1ZWEO", 116);
	nAddrZero[1] = pSng->GetCfgInt("����", "��վ2ZWEO", 136);
	nAddrZero[2] = pSng->GetCfgInt("����", "��վ3ZWEO", 156);
	int nTime;
	nTime = pSng->GetCfgInt("����", "����ʱ��", 1000);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		for (int i = 0; i < 3; i++)
		{
			if (pDlg->m_nWZERO[i] == 0)                           //�յ����ָ��
				continue;
			pDlg->m_nWZERO[i] = 0;
			int nValue = 0;
			pSng->_csSvr.Lock();
			BYTE pValue[4] = { 0 };
			memcpy(pValue, &nValue, sizeof(nValue));
			int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddrZero[i], nCount, pValue);
			pSng->_csSvr.Unlock();
			if (i == 0)
			{
				pDlg->SetTimer(TIME_ZERO1, 500, NULL);
			}
			else if (i == 1)
			{
				pDlg->SetTimer(TIME_ZERO2, 200, NULL);
			}
			else if (i == 2)
			{
				pDlg->SetTimer(TIME_ZERO3, 200, NULL);
			}
		}
		for (int j = 0; j < 3; j++)
		{
			if (pDlg->m_nZERO[j] == 0)
				continue;
			int nValue = pDlg->m_nZERO[j];
			pDlg->m_nZERO[j] = 0;
			pSng->_csSvr.Lock();
			BYTE pValue[4] = { 0 };
			memcpy(pValue, &nValue, sizeof(nValue));
			int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddrZero[j], nCount, pValue);
			pSng->_csSvr.Unlock();
			int a = j + 4;
			pDlg->m_nSleep[a] = 1;
		}
	}
	return 0;
}

UINT ThreadPLC(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	SoftElemType eType = REGI_H3U_DW;
	int nAddr[8];
	nAddr[0] = 142;
	nAddr[1] = 152;
	nAddr[2] = pSng->GetCfgInt("����", "��վ3IP", 141);
	nAddr[3] = pSng->GetCfgInt("����", "��վ3VAL", 151);
	nAddr[4] = pSng->GetCfgInt("����", "��վ1ZWEO", 116);
	nAddr[5] = pSng->GetCfgInt("����", "��վ2ZWEO", 136);
	nAddr[6] = pSng->GetCfgInt("����", "��վ3ZWEO", 156);
	nAddr[7] = 157;
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		for (int i = 0; i < 8; i++)
		{
			if (pDlg->m_nSleep[i] == 0)                           //�յ����ָ��
				continue;
			pDlg->m_nSleep[i] = 0;
			Sleep(1000);
			int nValue = 0;
			BYTE plValue[4] = { 0 };
			memcpy(plValue, &nValue, sizeof(nValue));
			int nCount = 1;
			int nRetWR = H3u_Write_Soft_Elem(REGI_H3U_DW, nAddr[i], nCount, plValue);
		}
	}
	return 0;
}

UINT ThreadSLEEP(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	int nTime;
	nTime = pSng->GetCfgInt("����", "����ʱ��", 6000);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		if (pDlg->bWZERO[0] == false)                           //�յ����ָ��
			continue;
		Sleep(nTime);
		pDlg->bWZERO[0] = false;
		pDlg->bZERO[0] = true;
	}
	return 0;
}

UINT ThreadSLEEP1(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	int nTime;
	nTime = pSng->GetCfgInt("����", "����ʱ��", 6000);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		if (pDlg->bWZERO[1] == false)                           //�յ����ָ��
			continue;
		Sleep(nTime);
		pDlg->bWZERO[1] = false;
		pDlg->bZERO[1] = true;
	}
	return 0;
}

UINT ThreadSLEEP2(LPVOID p)
{
	CSingleton* pSng = CSingleton::GetInstance();
	CDlgMes* pDlg = (CDlgMes*)pSng->_pMain;
	int nTime;
	nTime = pSng->GetCfgInt("����", "����ʱ��", 6000);
	for (; g_nExt != 1;)
	{
		Sleep(10);
		if (pSng->_nRun == RUN_NULL)
			continue;
		if (pDlg->bWZERO[2] == false)                           //�յ����ָ��
			continue;
		Sleep(nTime);
		pDlg->bWZERO[2] = false;
		pDlg->bZERO[2] = true;
	}
	return 0;
}


void CDlgMes::OnBnClickedBtnJz()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CString strText, strTime;
	GetDlgItemText(IDC_BTN_JZ, strText);
	if (strText == "��ʼУ��")
	{
		SetDlgItemText(IDC_BTN_JZ, "У�����");
		KillTimer(TIME_VAL1);
		KillTimer(TIME_VAL2);
		KillTimer(TIME_VAL3);
		GetDlgItem(IDC_BTN_JZ1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_JZ2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_JZ3)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_BTN_JZ, "��ʼУ��");
		SetTimer(TIME_VAL1, 500, NULL);
		SetTimer(TIME_VAL2, 500, NULL);
		SetTimer(TIME_VAL3, 500, NULL);
		GetDlgItem(IDC_BTN_JZ1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_JZ2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_JZ3)->EnableWindow(FALSE);
	}
}


void CDlgMes::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	g_nExt = 0;
	MESLOG("�˳����򣡣���");
}

BOOL CDlgMes::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)  //����ESC/ENTER
		{
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgMes::OnBnClickedBtnJz1()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	//01 06 23 2A 00 01 62 46 
	Array.Add(0x01);
	Array.Add(0x06);
	Array.Add(0x23);
	Array.Add(0x2A);
	Array.Add(0x00);
	Array.Add(0x01);
	Array.Add(0x62);
	Array.Add(0x46);
	m_val[0].put_InBufferCount(0);
	m_val[0].put_Output(COleVariant(Array));
	MESLOG("��ʼ��У1");
	GetDlgItem(IDC_BTN_JZ1)->EnableWindow(FALSE);
	SetTimer(TIME_JZ1, 500, NULL);
}


void CDlgMes::OnBnClickedBtnJz2()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	//02 06 23 2A 00 01 62 75
	Array.Add(0x02);
	Array.Add(0x06);
	Array.Add(0x23);
	Array.Add(0x2A);
	Array.Add(0x00);
	Array.Add(0x01);
	Array.Add(0x62);
	Array.Add(0x75);
	m_val[1].put_InBufferCount(0);
	m_val[1].put_Output(COleVariant(Array));
	MESLOG("��ʼ��У2");
	GetDlgItem(IDC_BTN_JZ2)->EnableWindow(FALSE);
	SetTimer(TIME_JZ2, 500, NULL);
}


void CDlgMes::OnBnClickedBtnJz3()
{
	CSingleton* pSng = CSingleton::GetInstance();
	CByteArray Array;
	//03 06 23 2A 00 01 63 A4
	Array.Add(0x03);
	Array.Add(0x06);
	Array.Add(0x23);
	Array.Add(0x2A);
	Array.Add(0x00);
	Array.Add(0x01);
	Array.Add(0x63);
	Array.Add(0xA4);
	m_val[2].put_InBufferCount(0);
	m_val[2].put_Output(COleVariant(Array));
	MESLOG("��ʼ��У3");
	GetDlgItem(IDC_BTN_JZ3)->EnableWindow(FALSE);
	SetTimer(TIME_JZ3, 500, NULL);

}

void CDlgMes::OnBnClickedBtnMes()
{
	MESSZ dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
}


void CDlgMes::OnBnClickedClearlog()
{
	/** Clear The Log Editor */
	auto* logEditor = dynamic_cast<CEdit*>(GetDlgItem(IDC_LOG));
	if (logEditor)
	{
		logEditor->Clear();
	}
}
