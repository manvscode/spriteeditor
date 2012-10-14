// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "SpriteEditorView.h"
#include "SpritePropertiesForm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
	ON_COMMAND(ID_HELP_MAKEADONATION, &CMainFrame::OnHelpMakeadonation)
	ON_COMMAND(ID_HELP_OTHERGREATPROGRAMS, &CMainFrame::OnHelpOthergreatprograms)
	ON_COMMAND(ID_ANIMATE_PLAY, &CMainFrame::OnAnimatePlay)
	ON_COMMAND(ID_ANIMATE_STOP, &CMainFrame::OnAnimateStop)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	: m_initSplitters(FALSE),
	  m_pMainView(NULL),
	  m_pFormView(NULL)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{

}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_BITMAP,
		0,0, // cx, cy
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS );
	CBitmap bm;
	bm.Attach(hbm);
	m_ilToolBar.Create( TOOLBARICON_SIZE, TOOLBARICON_SIZE, ILC_COLOR24 | ILC_MASK, 0, 0 );



	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_ilToolBar.Add( &bm, RGB(255,255,255) );
	m_wndToolBar.GetToolBarCtrl( ).SetImageList( &m_ilToolBar );
	bm.DeleteObject( );





	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	} 
	else {
		m_wndStatusBar.SetPaneInfo( 1, ID_SEPARATOR, SBPS_NORMAL, 100 );
		m_wndStatusBar.SetPaneText( 1, _T("Zoom: 1.2 x") );
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	CRect cr;  // client rectangle 
	// used to calculate client sizes
	
	if( !m_wndSplitter.CreateStatic( this, 1, 2, WS_CHILD | WS_VISIBLE ) )
	{
		MessageBox( _T("Error setting up splitter frames!"), _T("Init Error!"), MB_OK | MB_ICONERROR );
		return FALSE;
	}


	GetClientRect( &cr );

	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(CSpriteEditorView), CSize(cr.Width( ) - SPRITEFORM_WIDTH, cr.Height( ) ), pContext ) )
	{
		MessageBox( _T("Error setting up splitter frames!"), _T("Init Error!"), MB_OK | MB_ICONERROR );
		return FALSE;
	}
	else
		m_pMainView = reinterpret_cast<CSpriteEditorView *>( m_wndSplitter.GetPane( 0, 0) );
	
	
	
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(CSpritePropertiesForm), CSize( SPRITEFORM_WIDTH, cr.Height( ) ), pContext ) )
	{
		MessageBox( _T("Error setting up splitter frames!"), _T("Init Error!"), MB_OK | MB_ICONERROR );
		return FALSE;
	}
	else
		m_pFormView = reinterpret_cast<CSpritePropertiesForm *>( m_wndSplitter.GetPane( 0, 1) );

	m_initSplitters = TRUE;
	
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect cr; 	

	if(  m_initSplitters && nType != SIZE_MINIMIZED ){
		// Adjust the row/column info to compensate for
		// the new size

		m_wndSplitter.SetRowInfo( 0, cy, cy );
		m_wndSplitter.SetColumnInfo( 0, abs(cx - SPRITEFORM_WIDTH), 0 );
		m_wndSplitter.SetColumnInfo( 1, SPRITEFORM_WIDTH, 0 );
		/*if( m_ChargeViewUp )
			m_wndSplitter.SetColumnInfo( 1, SPRITEFORM_WIDTH, 0 );
		else
			m_wndSplitter.SetColumnInfo(1,cx,cx);*/




		// Apply our changes to the splitters
		m_wndSplitter.RecalcLayout( );
	}
}

void CMainFrame::setStatusBar( UINT statusPane, const CString &str )
{
	switch( statusPane )
	{
		case 0:
			break;
		case 1:
			m_wndStatusBar.SetPaneText( 1, str );
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			ASSERT( FALSE ); // this is a bug
	}
}
void CMainFrame::setView( ViewType v )
{
	m_CurrentView = v;
	
	if( v == MAIN_VIEW )
	{
		SetActiveView( m_pMainView );
		ASSERT( GetActiveView( )->IsKindOf( RUNTIME_CLASS(CSpriteEditorView) ) );
	}
	else if( v == FORM_VIEW )
	{
		SetActiveView( m_pFormView );
		ASSERT( GetActiveView( )->IsKindOf( RUNTIME_CLASS(CSpriteEditorView) ) );
	}
}

BOOL CMainFrame::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
    UINT nID = pTTTStruct->idFrom;

    if( pTTT->uFlags & TTF_IDISHWND )
    {
        // idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID( (HWND) nID );
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
			wcsncpy( pTTT->szText, _T("test"), 4 );
            pTTT->hinst = AfxGetResourceHandle();
            return TRUE;
        }
    }
	return CFrameWnd::OnToolTipText( id, pTTTStruct, pResult );
}
void CMainFrame::OnHelpMakeadonation()
{
	// Go to homepage and execute donation script...
	::ShellExecute( 0, _T("open"), _T("http://www.l33tprogrammer.com/index.php?page=donation"), NULL, NULL, SW_SHOWNORMAL );
}

void CMainFrame::OnHelpOthergreatprograms()
{
	// Go to homepage...
	::ShellExecute( 0, _T("open"), _T("http://www.l33tprogrammer.com/"), NULL, NULL, SW_SHOWNORMAL );
}

void CMainFrame::OnAnimatePlay()
{
	m_pFormView->OnBnClickedPlaybutton( );
}

void CMainFrame::OnAnimateStop()
{
	m_pFormView->OnBnClickedStopbutton( );
}
