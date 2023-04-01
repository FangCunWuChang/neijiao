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
	int m_Gradient;	// �����屳��������ϵ��
	float m_Height;  //��ͷ�߶ȣ����Ǳ���,
	int m_fontHeight; //����߶�
	int m_fontWith;   //������
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
		COLORREF m_ptrListCol;     //��������ɫ
		COLORREF m_ptrListItem;    //����Item��ɫ��
		COLORREF m_colTextColor;   //������������ɫ
		COLORREF m_ItemTextColor;  //���浥Ԫ��������ɫ

		int ReWork;                //�����޸ĵ���
		int type;
		LISTSET *next;
	};

	LISTSET *Head;    //��������ɫ
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
	bool FindColColor(int col ,COLORREF &color); //��������ɫ
	bool FindItemColor(int row,int col,COLORREF &color);
	bool FindColTextColor(int col,COLORREF &color); //������������ɫ
	bool FindItemTextColor(int row,int col,COLORREF &color);
    bool FindReWork(int row,int col,int &Type);

	CHeaderCtrlCl m_Header;

	CStringArray m_HChar;




	int m_fontHeight; //����߶�
	int m_fontWith;   //������
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
	void SetReWorkCol(int col,int nReWork,int tpye);         //���ÿ����޸ĵ���
	void SetReWorkItem(int row,int col,int nReWork,int tpye);//���ÿ����޸ĵ�Item


	void SetHeaderBKColor(int R, int G, int B, int Gradient); //���ñ�ͷ����ɫ
    void SetHeaderTextColor(COLORREF color);                  //����ͷ��������ɫ
	void SetHeaderFontHW(int nHeight,int nWith);              //����ͷ�������͸�
	void SetHeaderHeight(float Height);                   //���ñ�ͷ�߶�
	void SetFontHW(int nHeight,int nWith);                //��������ĸߺͿ�
	void SetRowHeight(int nHeight);	                      //�����и�      
	void SetColTextColor(int col,COLORREF color);         //�������ı���ɫ
    void SetColColor(int col,COLORREF color);             //��������ɫ
	void SetItemColor(int row,int col,COLORREF color);    //����Item��ɫ	
	void SetItemTextColor(int row,int col,COLORREF color);//����Item������ɫ
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
