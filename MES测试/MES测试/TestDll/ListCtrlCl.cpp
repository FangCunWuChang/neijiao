// ListCtrlCl.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlCl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlCl

CListCtrlCl::CListCtrlCl()
{
	m_color=RGB(0,0,0);
	
	nCombo = -1;

	m_fontHeight = 12;
	m_fontWith = 0;

	Head=(LISTSET*)malloc(sizeof(LISTSET));
	Head->nCol=0;
	Head->nRow=0;
	Head->m_colTextColor=0;
	Head->m_ItemTextColor=0;
	Head->m_ptrListCol=0;
	Head->m_ptrListItem=0;
	Head->ReWork=0;
	Head->type=0;
	Head->next=NULL;

	
}

CListCtrlCl::~CListCtrlCl()
{
	if(Head!=NULL)
	{
		LISTSET *P1;
		while(Head->next!=NULL)
		{
			P1=Head->next;
			Head->next=P1->next;
			free(P1);
		}
		free(Head);
	}
}


BEGIN_MESSAGE_MAP(CListCtrlCl, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlCl)
	ON_WM_MEASUREITEM()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM_REFLECT()
	ON_EN_KILLFOCUS(IDC_EDIT, OnKillFocusEdit)
	ON_CBN_KILLFOCUS(IDC_COMBOX, OnKillfocusCombo)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlCl message handlers
void CListCtrlCl::DeleteString(int nIndex, int nCombo)
{
	if(m_combox[nCombo].m_hWnd != NULL)
	{
		m_combox[nCombo].DeleteString(nIndex);
	}
}
void CListCtrlCl::AddString(LPCTSTR lpszString, int nCombo)
{
	if(m_combox[nCombo].m_hWnd != NULL)
	{
		m_combox[nCombo].AddString(lpszString);
		
	}
}
bool CListCtrlCl::FindReWork(int row,int col,int &Type)   //查找Item是否可以修改
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==-1)
		{
			if(p->next->ReWork==1 )
			{
				Type=p->next->type;
				res=true;
			}
			break;
		}
		p=p->next;
	}
    //////////////////////////////////////

	p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			if(p->next->ReWork==0)
			{
				res=false;
			}
			else if(p->next->ReWork==1)
			{
				Type=p->next->type;
				res=true;
			}
			break;
		}
		p=p->next;
	}
	
	return res;
}
bool CListCtrlCl::FindColColor(int col ,COLORREF &color) //查找列颜色
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col)
		{
			if(p->next->m_ptrListCol!=-1)
			{
				color=p->next->m_ptrListCol;
				res=true;
			}
			break;
		}
		p=p->next;
	}
	return res;
}
bool CListCtrlCl::FindItemColor(int row,int col,COLORREF &color)
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			if(p->next->m_ptrListItem!=-1)
			{
				color=p->next->m_ptrListItem;
				res=true;
			}
			break;
		}
		p=p->next;
	}
	return res;
}
bool CListCtrlCl::FindColTextColor(int col,COLORREF &color) //查找列字体颜色
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col)
		{
			if(p->next->m_colTextColor!=-1)
			{
				color=p->next->m_colTextColor;
				res=true;
			}
			break;
		}
		p=p->next;
	}
	return res;
}
bool CListCtrlCl::FindItemTextColor(int row,int col,COLORREF &color)
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			if(p->next->m_ItemTextColor!=-1)
			{
				color=p->next->m_ItemTextColor;
				res=true;
			}
			break;
		}
		p=p->next;
	}
	return res;
}
void CListCtrlCl::SetColTextColor(int col,COLORREF color)
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==-1)
		{
			res=true;
			p->next->m_colTextColor=color;
            break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=-1;
		p->next->m_ItemTextColor=-1;
		p->next->m_ptrListCol=-1;
		p->next->m_ptrListItem=-1;
		p->next->m_colTextColor=color;
		p->next->ReWork=-1;
		p->next->type=-1;
		p->next->next=NULL;
	}
}
void CListCtrlCl::SetHeaderHeight(float Height) //设置表头高度
{
	m_Header.m_Height = Height;
}
void CListCtrlCl::SetHeaderFontHW(int nHeight,int nWith) //设置头部字体宽和高
{
	m_Header.m_fontHeight = nHeight;
	m_Header.m_fontWith = nWith;
}
void CListCtrlCl::SetHeaderTextColor(COLORREF color) //设置头部字体颜色
{
	m_Header.m_TextColor = color;
}
void CListCtrlCl::SetHeaderBKColor(int R, int G, int B, int Gradient) //设置表头背景色
{
	m_Header.m_R = R;
	m_Header.m_G = G;
	m_Header.m_B = B;
	m_Header.m_Gradient = Gradient;
}

void CListCtrlCl::SetColColor(int col,COLORREF color)  //设置列颜色
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==-1)
		{
			res=true;
			p->next->m_ptrListCol=color;
            break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=-1;

		p->next->m_ItemTextColor=-1;
		p->next->m_colTextColor=-1;
		p->next->m_ptrListItem=-1;
		p->next->m_ptrListCol=color;
		p->next->ReWork=-1;
		p->next->type=-1;
		p->next->next=NULL;
	}
}
void CListCtrlCl::SetItemColor(int row,int col,COLORREF color) //设置Item颜色	
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			res=true;
			p->next->m_ptrListItem=color;
            break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=row;
		p->next->m_ItemTextColor=-1;
		p->next->m_colTextColor=-1;
		p->next->m_ptrListCol=-1;
		p->next->m_ptrListItem=color;
		p->next->ReWork=-1;
		p->next->type=-1;
		p->next->next=NULL;
	}
}

void CListCtrlCl::SetReWorkCol(int col,int ReWork,int type)                 //设置可以修改的列
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==-1)
		{
			res=true;
			p->next->ReWork=ReWork;
			p->next->type=type;
			break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=-1;
		p->next->m_ItemTextColor=-1;
		p->next->m_colTextColor=-1;
		p->next->m_ptrListCol=-1;
		p->next->m_ptrListItem=-1;
        p->next->ReWork=ReWork;
		p->next->type=type;
		p->next->next=NULL;
	}
}
void CListCtrlCl::SetReWorkItem(int row,int col,int ReWork,int type)       //设置可以修改的Item
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			res=true;
			p->next->ReWork=ReWork;
			p->next->type=type;
            break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=row;
		p->next->m_ItemTextColor=-1;
		p->next->m_colTextColor=-1;
		p->next->m_ptrListCol=-1;
		p->next->m_ptrListItem=-1;
		p->next->ReWork=ReWork;
		p->next->type=type;
		p->next->next=NULL;
	}
}
void CListCtrlCl::SetItemTextColor(int row,int col,COLORREF color)
{
	bool res=false;
	LISTSET *p=Head;
	while(p->next!=NULL)
	{
		if(p->next->nCol==col && p->next->nRow==row)
		{
			res=true;
			p->next->m_ItemTextColor=color;
            break;
		}
		p=p->next;
	}
	if(!res)
	{
		p->next=(LISTSET*)malloc(sizeof(LISTSET));
		p->next->nCol=col;
		p->next->nRow=row;
		p->next->m_ItemTextColor=color;
		p->next->m_colTextColor=-1;
		p->next->m_ptrListCol=-1;
		p->next->m_ptrListItem=-1;
		p->next->ReWork=-1;
		p->next->type=-1;
		p->next->next=NULL;
	}
}
void CListCtrlCl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	TCHAR lpBuffer[256];

	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	lvi.iItem = lpDrawItemStruct->itemID ; 
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev ;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rtClient;
	GetClientRect(&rtClient);
	for ( int nCol=0; GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left += lvcprev.cx ;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left; 
		}

		CRect rcItem;   
		if (!GetSubItemRect(lpDrawItemStruct->itemID,nCol,LVIR_LABEL,rcItem))   
			continue;   

		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		CRect rcTemp;
		rcTemp = rcItem;

		if (nCol==0)
		{
			rcTemp.left -=2;
		}

		if ( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			pDC->FillSolidRect(&rcTemp, GetSysColor(COLOR_HIGHLIGHT)) ;
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
		}
		else
		{
			COLORREF color;
			color = GetBkColor();
			pDC->FillSolidRect(rcTemp,color);

			if (FindColColor(nCol,color))
			{
				pDC->FillSolidRect(rcTemp,color);
			}
			if (FindItemColor(lpDrawItemStruct->itemID,nCol,color))
			{
				pDC->FillSolidRect(rcTemp,color);
			}
			
			pDC->SetTextColor(m_color);
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT   uFormat    = DT_LEFT;
		TEXTMETRIC metric;
		pDC->GetTextMetrics(&metric);
		int ofst;
		ofst = rcItem.Height() - metric.tmHeight;
		rcItem.OffsetRect(10,ofst/2);
		pDC->SetTextColor(m_color);
		COLORREF color;
		if (FindColTextColor(nCol,color))
		{
			pDC->SetTextColor(color);
		}
		if (FindItemTextColor(lpDrawItemStruct->itemID,nCol,color))
		{
			pDC->SetTextColor(color);
		}
		CFont nFont ,* nOldFont; 
		nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("宋体"));//创建字体 
		nOldFont = pDC->SelectObject(&nFont);
		DrawText(lpDrawItemStruct->hDC, lpBuffer, strlen(lpBuffer), 
			&rcItem, uFormat) ;

		pDC->SelectStockObject(SYSTEM_FONT) ;
	}
}

void CListCtrlCl::PreSubclassWindow() 
{
	if(m_edit.m_hWnd == NULL)
	{
		int sty=WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|ES_AUTOHSCROLL;//WS_BORDER|WS_TABSTOP;//设置编辑框样式
		m_edit.CreateEx(WS_EX_CLIENTEDGE,"EDIT",NULL,sty,CRect(0,0,0,0),this,IDC_EDIT);

	    m_edit.ShowWindow(SW_HIDE);
	
	}
	for (int i = 0; i < 3;i++)
	{
		if (m_combox[i].m_hWnd == NULL)
		{
			int sty = WS_CHILD | CBS_DROPDOWN | WS_TABSTOP | ES_AUTOHSCROLL | WS_HSCROLL;
			m_combox[i].CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", NULL, sty, CRect(0, 0, 0, 0), this, IDC_COMBOX);
			m_combox[i].ShowWindow(SW_HIDE);
		}
	}
    ModifyStyle(0,LVS_OWNERDRAWFIXED);	
	CListCtrl::PreSubclassWindow();
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}

void CListCtrlCl::SetRowHeight(int nHeight)
{
	m_nRowHeight = nHeight;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CListCtrlCl::SetFontHW(int nHeight,int nWith) //设置字体高和宽
{
	m_fontHeight = nHeight;
	m_fontWith = nWith;
}

void CListCtrlCl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
void CListCtrlCl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}

void CListCtrlCl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rcCtrl;
	LVHITTESTINFO lvhti;
    lvhti.pt = point;
	nItem = CListCtrl::SubItemHitTest(&lvhti);
	if(nItem == -1)
	   return;
	nIndex = lvhti.iSubItem;
	GetSubItemRect(nItem,nIndex,LVIR_LABEL,rcCtrl); 
	int Type=-1;
	if(FindReWork(nItem,nIndex,Type))
	{
		if(Type == IDC_EDIT)   //CEdit
		{
			if(m_edit.m_hWnd != NULL)
			{
				m_edit.MoveWindow(rcCtrl);
				
				m_edit.ShowWindow(SW_SHOW);
				m_edit.SetWindowText(GetItemText(nItem,nIndex));
				::SetFocus(m_edit.GetSafeHwnd());
				m_edit.SetSel(-1); 
			}
		}
		else if(Type== IDC_COMBOX || Type == IDC_COMBOX2 || Type == IDC_COMBOX3)  //CComboBox
		{
			if(m_combox[Type - 2].m_hWnd != NULL)
			{
				CRect rect=rcCtrl;
				rect.bottom+=200;
				nCombo = Type - IDC_COMBOX;
				m_combox[nCombo].MoveWindow(rect);
				m_combox[nCombo].ShowWindow(SW_SHOW);
				//m_combox[Type - 2].AddString(GetItemText(nItem,nIndex));
				m_combox[nCombo].SetWindowText(GetItemText(nItem, nIndex));
				m_combox[nCombo].SetFocus();
				
			}
		}
	}
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CListCtrlCl::OnKillFocusEdit()
{
	if(m_edit.m_hWnd != NULL)
	{
		char strText[255]="";
		m_edit.GetWindowText(strText,255);
		SetItemText(nItem,nIndex,strText);
		m_edit.ShowWindow(SW_HIDE);
	}
}

void CListCtrlCl::OnKillfocusCombo()
{
	//for (int i = 0; i < 3;i++)
	{
		if (m_combox[nCombo].m_hWnd != NULL)
		{
			char strText[255] = "";
			m_combox[nCombo].GetWindowText(strText, 255);
			SetItemText(nItem, nIndex, strText);
			m_combox[nCombo].ShowWindow(SW_HIDE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlCl

CHeaderCtrlCl::CHeaderCtrlCl()
{
	m_R=171;
    m_G=199;
    m_B=235;
	m_Gradient=8;
	m_Height=float(1.4);
	m_fontHeight=14;
	m_TextColor=RGB(255,0,0);
}

CHeaderCtrlCl::~CHeaderCtrlCl()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlCl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlCl)
	ON_WM_PAINT()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlCl message handlers

void CHeaderCtrlCl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	int nItem; 
	nItem = GetItemCount();//得到有几个单元 
	for(int i = 0; i<nItem;i ++) 
	{ 
		CRect tRect;
		GetItemRect(i,&tRect);//得到Item的尺寸
		int R = m_R,G = m_G,B = m_B;
		CRect nRect(tRect);//拷贝尺寸到新的容器中 
		nRect.left++;//留出分割线的地方 
		//绘制立体背景 
		for(int j = tRect.top;j<=tRect.bottom;j++) 
		{ 
			nRect.bottom = nRect.top+1; 
			CBrush _brush; 
			_brush.CreateSolidBrush(RGB(R,G,B));//创建画刷 
			dc.FillRect(&nRect,&_brush); //填充背景 
			_brush.DeleteObject(); //释放画刷 
			R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;
			if (R<0)R = 0;
			if (G<0)G = 0;
			if (B<0)B= 0;
			nRect.top = nRect.bottom; 
		} 
		dc.SetBkMode(TRANSPARENT);
		CFont nFont ,* nOldFont; 

		dc.SetTextColor(m_TextColor);
		nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("Cambria"));//创建字体 
		nOldFont = dc.SelectObject(&nFont);

		TCHAR	szText[255];
		HDITEM hditem;
		hditem.mask=HDI_WIDTH|HDI_FORMAT|HDI_TEXT|HDI_IMAGE|HDI_BITMAP;
		hditem.pszText=szText;
		hditem.cchTextMax=sizeof(szText);
		VERIFY(GetItem(i, &hditem));

		TEXTMETRIC metric;
		dc.GetTextMetrics(&metric);
		int ofst = 0;
		ofst = tRect.Height() - metric.tmHeight;
		tRect.OffsetRect(0,ofst/2);

		dc.DrawText(hditem.pszText,&tRect,DT_CENTER);
		dc.SelectObject(nOldFont); 
		nFont.DeleteObject(); //释放字体 
	} 
	//画头部剩余部分
	CRect rtRect;
	CRect clientRect;
	GetItemRect(nItem - 1,rtRect);
	GetClientRect(clientRect);
	rtRect.left = rtRect.right+1;
	rtRect.right = clientRect.right;
	int R = m_R,G = m_G,B = m_B;
	CRect nRect(rtRect);
	//绘制立体背景 
	for(int j = rtRect.top;j<=rtRect.bottom;j++) 
	{ 
		nRect.bottom = nRect.top+1; 
		CBrush _brush; 
		_brush.CreateSolidBrush(RGB(R,G,B));//创建画刷 
		dc.FillRect(&nRect,&_brush); //填充背景 
		_brush.DeleteObject(); //释放画刷 
		R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;
		if (R<0)R = 0;
		if (G<0)G = 0;
		if (B<0)B= 0;
		nRect.top = nRect.bottom; 
	} 
}

LRESULT CHeaderCtrlCl::OnLayout( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam; 
	RECT *prc = hdl.prc; 
	WINDOWPOS *pwpos = hdl.pwpos; 

	//表头高度为原来1.5倍，如果要动态修改表头高度的话，将1.5设成一个全局变量 
	int nHeight = (int)(pwpos->cy * m_Height);
	pwpos->cy = nHeight; 
	prc->top = nHeight; 
	return lResult; 
}

