#pragma once
#include "mscomm1.h"
#include "..\DllMod\Singleton.h"
#include "MESSZ.h"

struct CZ
{
	CZ()
	{
		strCode.Empty();
		fVal = -1;
	}
	CString strCode;
	float fVal;
};

// CDlgMes 对话框

class CDlgMes : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMes)

public:
	CDlgMes(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMes();


// 对话框数据
	enum { IDD = IDD_DIALOG_MES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	MESSZ messzDlg;
	CComboBox m_comboLiao;
	int m_nWIP[3], m_nWVAL[3], m_nWZERO[3],m_nSleep[8];
	int m_nIP[3], m_nVAL[3], m_nZERO[3];
	bool bVal[3],bZERO[3],bWZERO[3];
	CTCPSocket* m_pServer[3];
	DWORD dwIP[3], dwVal[3],dwZero[3];
	bool Check_Json1(std::string& str);
	bool Check_Json2(std::string& str);
	string GetTimeMillisecondsStr();
	template<typename T> string ValueToStr(T value)
	{
		ostringstream ost;
		ost << value;
		return ost.str();
	}
	CMscomm1 m_val[3];
	CString m_strT[4], m_strCode[3], m_strEX;
	CString strMEStoken;
	bool CDlgMes::InitialCom(CMscomm1 &Com, int iNO, int nBaud, int nLen);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnCommMscomm2();
	void OnCommMscomm3();
	bool MES1(CString strmoid, CString strpartID, CString strppid, CString strtestStation);
	bool MES2(CString strtoken, CString strdeptID, CString strpartID, CString strppid, CString strmoid,
		CString strlineID, CString strtestStation, CString strtestResult, CString strmachineSN,
		CString strtestchannelID, CString strempty, CString strfilling, CString strdegassing, CString strfill_empty,
		CString strill_degass, CString strdegass_empty, CString strempty_p1, CString strempty_l1);
	void SendResult(CMscomm1& Com);
	void SendZero(CMscomm1& Com);
	void SendJz(CMscomm1& Com);
	CString strVal[3];
	afx_msg void OnBnClickedBtnVal();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedBtnZero();
	afx_msg void OnBnClickedBtnOkzero();
	vector<CZ> TV;
	vector<CZ> TV1;
	vector<CZ> TV2;
	afx_msg void OnBnClickedBtnJz();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnJz1();
	afx_msg void OnBnClickedBtnJz2();
	afx_msg void OnBnClickedBtnJz3();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnMes();
	afx_msg void OnBnClickedClearlog();
};

