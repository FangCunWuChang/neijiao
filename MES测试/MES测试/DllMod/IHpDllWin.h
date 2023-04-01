#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "Singleton.h"
#include "afxcmn.h"
#include "IDlgInfo.h"


// IHpDllWin 对话框
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace std;
#include "../tinyxml/tinyxml.h"


class IHpDllWin : public CDialogEx
{
	DECLARE_DYNAMIC(IHpDllWin)

public:
	IHpDllWin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~IHpDllWin();

// 对话框数据
	enum { IDD = IDD_IHPDLLWIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool m_bCali;
	CString m_strCam;
	CHWin m_HW;
	afx_msg void OnBnClickedBtnOrg();
	CComboBox m_comboFunc;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CTreeCtrl m_tree;
	afx_msg void OnBnClickedBtnAddFunc();
	HTREEITEM m_hRoot;
	HTREEITEM  finditem(HTREEITEM  item, CString strtext);
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio1();
	int GetKnots(HTREEITEM &hRootItem);
	afx_msg void OnBnClickedBtnTestFunc();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedBtnSavXml();
	void ExpandTree(HTREEITEM hTreeItem, TiXmlElement* pRoot, bool bRoot = false);
	void GetEleValue(TiXmlElement* root);
	afx_msg void OnBnClickedBtnLoadXml();
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuDel();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnMenuRun();
	afx_msg void OnMenuMod();
	afx_msg void OnMenuBf();
	afx_msg void OnBnClickedRadio3();
	void LoadXML(CString sPathName);
	CString m_strXML;                                    //图像文件
	afx_msg void OnMenuAddRc();
	afx_msg void OnMenuInsert();
	void InsertNode(CString strText, HTREEITEM hItemSel,bool bInsert = true);
	void InsertNode(CString strText,CStrs& stsItem, HTREEITEM hItemSel);
	void UpdateIndex(HTREEITEM& hItem, bool bAdd = true);
	afx_msg void OnBnClickedBtnDirXml();
	bool GotoNodeSel(int iNode);
	afx_msg void OnMenuCam();
	double m_fGain, m_fGam, m_fExp;
	afx_msg void OnMenuIni();
	afx_msg void OnMenuDef();
	afx_msg void OnMenuFirst();
	afx_msg void OnBnClickedBtnSnap();
//	afx_msg void OnBnClickedBtnPart();
	afx_msg void OnNMCustomdrawTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool m_bINI;
	void UpdateTest(int nOK = 0);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckBd();
};
