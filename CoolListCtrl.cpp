// CoolListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "CoolListCtrl.h"


// CCoolListCtrl

IMPLEMENT_DYNAMIC(CCoolListCtrl, CListCtrl)

CCoolListCtrl::CCoolListCtrl()
 : 	m_bOverControl(FALSE),
	m_nTimerID(0),
	m_bColor(FALSE),
	m_bInFocus(FALSE)
{

}

CCoolListCtrl::~CCoolListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCoolListCtrl, CListCtrl)
	ON_WM_DRAWITEM()
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, &CCoolListCtrl::OnHdnItemchanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, &CCoolListCtrl::OnHdnItemchanged)
	ON_NOTIFY_REFLECT(LVN_ODSTATECHANGED, &CCoolListCtrl::OnLvnOdstatechanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, &CCoolListCtrl::OnLvnBeginrdrag)
END_MESSAGE_MAP()



// CCoolListCtrl message handlers



void CCoolListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;
	UINT itemID = lpDrawItemStruct->itemID;
	UINT itemAction = lpDrawItemStruct->itemAction;
	UINT itemState = lpDrawItemStruct->itemState;

	//Draw the focus rectangle...
	if( m_bInFocus )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->DrawFocusRect( rcClient );
	}

	CImageList *pImageList = GetImageList( LVSIL_SMALL );

	CString frameText, filenameText, dimensions;
	CBrush blackBrush( RGB(0,0,0) );
	CPen blackPen( PS_SOLID | PS_ENDCAP_SQUARE, 0, RGB(0,0,0) );

	CBrush *pOldBrush = pDC->SelectObject( &blackBrush );
	CPen *pOldPen = pDC->SelectObject( &blackPen );

	CPoint pt;
	GetItemPosition( itemID, &pt );
	
	CRect col1( 0, 0, (GetColumnWidth( 0 ) > 0 ? GetColumnWidth( 0 ) - 3 : GetColumnWidth( 0 )) , rect.Height( ) );
	CRect col2( GetColumnWidth( 0 ), 0, GetColumnWidth( 1 ) - 5, rect.Height( ) );

	int nMode = pDC->SetBkMode(TRANSPARENT);

	pDC->SetTextColor( RGB(0,0,0) );
	if( itemAction & ODA_DRAWENTIRE )
	{
		if( itemState & ODS_SELECTED )
		{
			pDC->FillSolidRect( rect, RGB(163,188,219) );
			pDC->SetTextColor( RGB(255,0,0) );
		}
		else if( itemID % 2 == 0  )
			pDC->FillSolidRect( rect, RGB(238,238,236) );
		else
			pDC->FillSolidRect( rect, RGB(251,250,240) );
			
		if( col1.Width( ) > 0 )
			pImageList->DrawEx( pDC, itemID, pt, col1.Size( ), CLR_DEFAULT, CLR_DEFAULT, ILD_IMAGE );

		// draw the frame number...
		frameText = GetItemText( itemID, 0 );
		col1.left = pt.x + 45;
		col1.top = rect.CenterPoint().y + pt.y;
		pDC->DrawText( frameText, col1, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS );
		
		// draw the filename...
		filenameText = GetItemText( itemID, 1 );
		col2.left = GetColumnWidth( 0 ) + pt.x;
		col2.top = rect.CenterPoint().y + pt.y;
		pDC->DrawText( filenameText, col2, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS );
		
		// draw the orignal dimensions...
		dimensions = GetItemText( itemID, 2 );
		//col2.left = GetColumnWidth( 0 ) + pt.x;
		col2.top = rect.CenterPoint().y + pt.y - 25;
		pDC->DrawText( dimensions, col2, DT_NOCLIP | DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS );

		// Draw outline
		rect.DeflateRect( 0, 1 );
		pDC->DrawEdge( rect, BDR_RAISEDOUTER, BF_BOTTOM/*BF_RECT*/ );
	}
	

	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );
	pDC->SetBkMode(nMode);
}

void CCoolListCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CListCtrl::PreSubclassWindow( );
	ModifyStyle( 0, LVS_OWNERDRAWFIXED );
	SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER
					  | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_SINGLEROW  );
}

void CCoolListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CCoolListCtrl::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	Invalidate( );
	*pResult = 0;
}

void CCoolListCtrl::OnLvnOdstatechanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVODSTATECHANGE pStateChanged = reinterpret_cast<LPNMLVODSTATECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	Invalidate( );
	*pResult = 0;
}

void CCoolListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);

	////clear all the flags
	//m_MovingIndex = LB_ERR;
	//m_MoveToIndex = LB_ERR;
	//m_Captured = FALSE;
	//m_Interval = 0;


	////Find the item that they want to drag and keep track of it.
	////Later in the mouse move, we will capture the mouse if this
	////value is set.
	//UINT flags = LVHT_ONITEMSTATEICON | LVHT_ONITEMLABEL | LVHT_ONITEMICON;
	//int Index = this->HitTest( point, &flags );
	//
	//if( Index != LB_ERR )
	//{
	//	m_MovingIndex = Index;
	//	//SetCurSel(Index);
	//	SetItemState( Index, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
	//}
}

void CCoolListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//if( m_bDragging )
	//{
	//	//RELEASE THE MOUSE CAPTURE AND END THE DRAGGING
	//	::ReleaseCapture( );
	//	m_bDragging = FALSE;
	//	m_pDragImage->DragLeave( GetDesktopWindow( ) );
	//	m_pDragImage->EndDrag( );

	//	//GET THE WINDOW UNDER THE DROP POINT
	//	CPoint pt( point );
	//	ClientToScreen( &pt );
	//	m_pDropWnd = WindowFromPoint( pt );

	//	//DROP THE ITEM ON THE LIST
	//	//if( m_pDropWnd->IsKindOf( RUNTIME_CLASS(CListCtrl) ) )
	//		//DropFrameItemOnList( );
	//}
	CListCtrl::OnLButtonUp(nFlags, point);
}

void CCoolListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	//if( m_bDragging )
	//{
	//	m_ptDropPoint = point;
	//	ClientToScreen( &m_ptDropPoint );

	//	//m_FrameListCtrl.ModifyStyle( LVS_OWNERDRAWFIXED, 0 );
	//	//MOVE THE DRAG IMAGE
	//	m_pDragImage->DragMove( m_ptDropPoint );

	//	//TEMPORARILY UNLOCK WINDOW UPDATES
	//	m_pDragImage->DragShowNolock( FALSE );
	//	//m_FrameListCtrl.ModifyStyle( 0, LVS_OWNERDRAWFIXED );

	//	//CONVERT THE DROP POINT TO CLIENT CO-ORDIANTES
	//	m_pDropWnd = WindowFromPoint( m_ptDropPoint );
	//	m_pDropWnd->ScreenToClient( &m_ptDropPoint );

	//	//LOCK WINDOW UPDATES
	//	m_pDragImage->DragShowNolock( TRUE );
	//}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CCoolListCtrl::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	////RECORD THE INDEX OF THE ITEM BEIGN DRAGGED AS m_nDragIndex
	//m_nDragIndex = pNMLV->iItem; //((NM_LISTVIEW *)pnmhdr)->iItem;

	////CREATE A DRAG IMAGE FROM THE CENTER POINT OF THE ITEM IMAGE
	//POINT pt;
	//pt.x = 8;
	//pt.y = 8;
	//
	////disable owner draw
	////m_FrameListCtrl.ModifyStyle( LVS_OWNERDRAWFIXED, 0 );
	//m_pDragImage = CreateDragImage( m_nDragIndex, &pt );
	////m_FrameListCtrl.ModifyStyle( 0, LVS_OWNERDRAWFIXED );

	//m_pDragImage->BeginDrag( 0, CPoint( 8, 8 ) );
	//m_pDragImage->DragEnter( GetDesktopWindow( ), pNMLV->ptAction );

	////SET THE FLAGS INDICATING A DRAG IN PROGRESS
	//m_bDragging = TRUE;
	//
	////m_hDropItem = NULL;
	//m_nDropIndex = -1;
	//m_pDropWnd = this;

	////CAPTURE ALL MOUSE MESSAGES IN CASE THE USER DRAGS OUTSIDE OF THE VIEW
	//SetCapture( );
	*pResult = 0;
}

void CCoolListCtrl::InsertDraggedItem( )
{

}

void CCoolListCtrl::DoTheScrolling( CPoint Point, CRect ClientRect )
{
}
void CCoolListCtrl::OnTimer(UINT_PTR nIDEvent)
{

	CListCtrl::OnTimer(nIDEvent);
}

BOOL CCoolListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	return TRUE;/*CListCtrl::Create(dwStyle, rect, pParentWnd, nID);*/
}


