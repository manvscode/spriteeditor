// CoolButton.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "CoolButton.h"


// CCoolButton

IMPLEMENT_DYNAMIC(CCoolButton, CButton)

CCoolButton::CCoolButton()
{
	m_bOverControl = FALSE;
	m_nTimerID = 0;
	BITMAP bm;
	HBITMAP hNormalBmp, hOverBitmap, hSelectedBitmap;

	hNormalBmp = LoadBitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BUTTONBITMAP) );
	hOverBitmap = LoadBitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_OVERBUTTONBITMAP) );
	hSelectedBitmap = LoadBitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_SELECTEDBUTTONBITMAP) );
	GetObject( hNormalBmp, sizeof(BITMAP), &bm );
	m_BitmapWidth = bm.bmWidth;
	m_BitmapHeight = bm.bmHeight;
	
	
	
	m_NormalBitmap.Attach( hNormalBmp );
	m_OverBitmap.Attach( hOverBitmap );
	m_SelectedBitmap.Attach( hSelectedBitmap );

		

}

CCoolButton::~CCoolButton()
{
	m_NormalBitmap.DeleteObject( );
	m_OverBitmap.DeleteObject( );
	m_SelectedBitmap.DeleteObject( );
}


BEGIN_MESSAGE_MAP(CCoolButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
//	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCoolButton message handlers



void CCoolButton::OnMouseMove(UINT nFlags, CPoint point)
{
    if( !m_bOverControl )                    // Cursor has just moved over control
    {
        TRACE0("Entering CCoolButton control\n");

        m_bOverControl = TRUE;              // Set flag telling us the mouse is in
        Invalidate();                       // Force a redraw

        SetTimer(m_nTimerID, 100, NULL);    // Keep checking back every 1/10 sec
    }

	CButton::OnMouseMove(nFlags, point);
}

void CCoolButton::OnTimer(UINT_PTR nIDEvent)
{
    // Where is the mouse?
    CPoint p(GetMessagePos());
    ScreenToClient(&p);

    // Get the bounds of the control (just the client area)
    CRect rect;
    GetClientRect(rect);

    // Check the mouse is inside the control
    if (!rect.PtInRect(p))
    {
        TRACE0("Leaving CCoolButton control\n");

        // if not then stop looking...
        m_bOverControl = FALSE;
        KillTimer(m_nTimerID);

        // ...and redraw the control
        Invalidate();
    }

	CButton::OnTimer(nIDEvent);
}

void CCoolButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	
    CRect rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;




    CString strText;
    GetWindowText(strText);


    // draw the control edges (DrawFrameControl is handy!)
    //if (state & ODS_SELECTED)
    //    pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
    //else
    //    pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);

    // Deflate the drawing rect by the size of the button's edges
    rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));
    


	CPoint pt( 0 ,0 );
	CSize size( m_rcBounds.Width( ), m_rcBounds.Height( ) );
		
	int nMode = pDC->SetBkMode(TRANSPARENT);

	if( state & ODS_SELECTED )
	{
			//pDC->DrawState( pt, size, &m_NormalBitmap,  DST_BITMAP | DSS_NORMAL );
		CBitmap *pOld = m_MemoryDC.SelectObject( &m_SelectedBitmap );
		CRect rc1(0, 0, 1, m_rcBounds.bottom );
		CRect rc2(0, 0, m_rcBounds.right, 1 );
		pDC->TransparentBlt( 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), &m_MemoryDC, 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), RGB(255,255,255) );
		//Invalidate( );
		m_MemoryDC.SelectObject( pOld );
	}
	else if( m_bOverControl )
	{
		CBitmap *pOld = m_MemoryDC.SelectObject( &m_OverBitmap );
		//pDC->DrawState( pt, size, &m_NormalBitmap,  DST_BITMAP );  
		pDC->TransparentBlt( 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), &m_MemoryDC, 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), RGB(255,255,255) );
		m_MemoryDC.SelectObject( pOld );
		
	}
	else {
		CBitmap *pOld = m_MemoryDC.SelectObject( &m_NormalBitmap );
		//pDC->DrawState( pt, size, &m_NormalBitmap,  DST_BITMAP );  
		pDC->TransparentBlt( 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), &m_MemoryDC, 0, 0, m_rcBounds.Width(), m_rcBounds.Height(), RGB(255,255,255) );
		m_MemoryDC.SelectObject( pOld );
	}
	
	// Draw a focus outline...
	//if( state & ODS_FOCUS )
	//{
	//	CRect rcClient;
	//	GetClientRect( &rcClient );
	//	pDC->DrawFocusRect( &rcClient );
	//}

	pDC->SetBkMode(nMode);
		


    // Draw the text
    if (!strText.IsEmpty())
    {
        CSize Extent = pDC->GetTextExtent(strText);
        CPoint pt( rect.CenterPoint().x - Extent.cx/2, 
        rect.CenterPoint().y - Extent.cy/2 );

        if (state & ODS_SELECTED) 
            pt.Offset(0,0); //  pt.Offset(1,1);

        int nMode = pDC->SetBkMode(TRANSPARENT);

        if (state & ODS_DISABLED)
            pDC->DrawState(pt, Extent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
        else
            pDC->TextOut(pt.x, pt.y, strText);

        pDC->SetBkMode(nMode);
    }

	    // Fill the interior color if necessary

	
}

void CCoolButton::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
    CButton::PreSubclassWindow( );
	GetClientRect( m_rcBounds );
	m_MemoryDC.CreateCompatibleDC( NULL );
	
	CBitmap *pOld = m_MemoryDC.SelectObject( &m_NormalBitmap );

	//Resize all bitmaps to the adequate size...
	if( pOld != NULL )
	{
		SetStretchBltMode( m_MemoryDC.GetSafeHdc( ), STRETCH_HALFTONE );
		m_MemoryDC.StretchBlt( 0, 0, m_rcBounds.Width( ), m_rcBounds.Height( ), &m_MemoryDC, 0, 0, m_BitmapWidth, m_BitmapHeight, SRCCOPY );
		m_MemoryDC.SelectObject( pOld );
	}

	pOld = m_MemoryDC.SelectObject( &m_OverBitmap );

	if( pOld != NULL )
	{
		SetStretchBltMode( m_MemoryDC.GetSafeHdc( ), STRETCH_HALFTONE );
		m_MemoryDC.StretchBlt( 0, 0, m_rcBounds.Width( ), m_rcBounds.Height( ), &m_MemoryDC, 0, 0, m_BitmapWidth, m_BitmapHeight, SRCCOPY );
		m_MemoryDC.SelectObject( pOld );
	}

	pOld = m_MemoryDC.SelectObject( &m_SelectedBitmap );

	if( pOld != NULL )
	{
		SetStretchBltMode( m_MemoryDC.GetSafeHdc( ), STRETCH_HALFTONE );
		m_MemoryDC.StretchBlt( 0, 0, m_rcBounds.Width( ), m_rcBounds.Height( ), &m_MemoryDC, 0, 0, m_BitmapWidth, m_BitmapHeight, SRCCOPY );
		m_MemoryDC.SelectObject( pOld );
	}

    ModifyStyle( 0, BS_OWNERDRAW );	// make the button owner drawn
}


