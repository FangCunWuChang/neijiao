#pragma once
#include "afxwin.h"
#include "Singleton.h"
#include "afxcmn.h"

// CIDlgCamCfg �Ի���

class CIDlgCamCfg : public CDialogEx
{
	DECLARE_DYNAMIC(CIDlgCamCfg)

public:
	CIDlgCamCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIDlgCamCfg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ICAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMReleasedcaptureSliderPar(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_comboType;
	CComboBox m_comboCam;
	afx_msg void OnCbnSelchangeComboType();
	void InitSlider();
	CHWin m_HW;
	CSliderCtrl m_sldPar;
	double m_fSldCoef;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
//	virtual BOOL DestroyWindow();
	afx_msg void OnCbnSelchangeComboCamSel();
	afx_msg void OnBnClickedOk();
};
