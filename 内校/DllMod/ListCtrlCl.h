#if !defined(AFX_LISTCTRLCL_H__F44A5766_98BA_424C_85B4_9F94ED0E8AD3__INCLUDED_)
#define AFX_LISTCTRLCL_H__F44A5766_98BA_424C_85B4_9F94ED0E8AD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlCl.h : header file
//
class CHeaderCtrlCl : public CHeaderCtrl
{
// Construction
public:
	CHeaderCtrlCl();

// Attributes
public:
    int m_R;
	int m_G;
	int m_B;
	int m_Gradient;	// 画立体背景，渐变系数
	float m_Height;  //表头高度，这是倍数,
	int m_fontHeight; //字体高度
	int m_fontWith;   //字体宽度
	COLORREF m_TextColor;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderCtrlCl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHeaderCtrlCl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderCtrlCl)
	afx_msg void OnPaint();
	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
  
#define IDC_EDIT    1
#define IDC_COMBOX  2
#define IDC_COMBOX2  3
#define IDC_COMBOX3  4
class CListCtrlCl : public CListCtrl
{
public:
	struct LISTSET
	{
		int nRow;
		int nCol;
		COLORREF m_ptrListCol;     //保存列颜色
		COLORREF m_ptrListItem;    //保存Item颜色表
		COLORREF m_colTextColor;   //保存列字体颜色
		COLORREF m_ItemTextColor;  //保存单元格字体颜色

		int ReWork;                //可以修改的列
		int type;
		LISTSET *next;
	};

	LISTSET *Head;    //保存列颜色
public:
	CListCtrlCl();

// Attributes
public:
	//void ClearCombo()
	//{
	//	m_combox[3].ResetContent();
	//}
protected:
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	bool FindColColor(int col ,COLORREF &color); //查找列颜色
	bool FindItemColor(int row,int col,COLORREF &color);
	bool FindColTextColor(int col,COLORREF &color); //查找列字体颜色
	bool FindItemTextColor(int row,int col,COLORREF &color);
    bool FindReWork(int row,int col,int &Type);

	CHeaderCtrlCl m_Header;

	CStringArray m_HChar;




	int m_fontHeight; //字体高度
	int m_fontWith;   //字体宽度
	COLORREF m_color;
	int m_nRowHeight;

	CComboBox m_combox[3];
	CEdit m_edit;

	int nItem;
	int nIndex;
	int nCombo;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlCl)
	protected:
	virtual void PreSubclassWindow();
	virtual void OnKillFocusEdit();
	virtual void OnKillfocusCombo();
	//}}AFX_VIRTUAL

// Implementation
public:
	void DeleteString(int nIndex,int nCombo = 0);
	void AddString(LPCTSTR lpszString,int nCombo = 0);
	void ClearCombo(int nCombo = 0);
	void SetReWorkCol(int col,int nReWork,int tpye);         //设置可以修改的列
	void SetReWorkItem(int row,int col,int nReWork,int tpye);//设置可以修改的Item


	void SetHeaderBKColor(int R, int G, int B, int Gradient); //设置表头背景色
    void SetHeaderTextColor(COLORREF color);                  //设置头部字体颜色
	void SetHeaderFontHW(int nHeight,int nWith);              //设置头部字体宽和高
	void SetHeaderHeight(float Height);                   //设置表头高度
	void SetFontHW(int nHeight,int nWith);                //设置字体的高和宽
	void SetRowHeight(int nHeight);	                      //设置行高      
	void SetColTextColor(int col,COLORREF color);         //设置列文本颜色
    void SetColColor(int col,COLORREF color);             //设置列颜色
	void SetItemColor(int row,int col,COLORREF color);    //设置Item颜色	
	void SetItemTextColor(int row,int col,COLORREF color);//设置Item字体颜色
	virtual ~CListCtrlCl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlCl)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlCl window



/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLCL_H__F44A5766_98BA_424C_85B4_9F94ED0E8AD3__INCLUDED_)
