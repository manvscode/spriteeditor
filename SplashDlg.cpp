// SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "SplashDlg.h"


// CSplashDlg dialog

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashDlg::IDD, pParent)
{
	TRACE0("CSplashDlg Created...");
}

CSplashDlg::~CSplashDlg()
{
	TRACE0("CSplashDlg Destroyed...");
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSplashDlg message handlers

void CSplashDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	memDC.CreateCompatibleDC( NULL );

	CBitmap bmp;
	bmp.LoadBitmap( MAKEINTRESOURCE(IDB_ABOUTBITMAP) );
	CBitmap *pOldBitmap = memDC.SelectObject( &bmp );

	dc.BitBlt( 0, 0, SPLASHBMP_WIDTH, SPLASHBMP_HEIGHT, &memDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOldBitmap );
	
	bmp.DeleteObject( );
	ReleaseDC( &memDC );
}

BOOL CSplashDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::Create(lpszTemplateName, pParentWnd);
}

BOOL CSplashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcDesktop;
	CRect rcClient;
	GetClientRect( &rcClient );
	HWND hwndDesktop = ::GetDesktopWindow( );
	::GetClientRect( hwndDesktop, &rcDesktop );

	unsigned int center_left = (rcDesktop.Width( ) - rcClient.Width( ) ) >> 1; // half of desktop minus half of the splash width...
	unsigned int center_top = (rcDesktop.Height( ) - rcClient.Height( ) ) >> 1; 

	MoveWindow( center_left, center_top, SPLASHBMP_WIDTH, SPLASHBMP_HEIGHT );
	ShowWindow( SW_SHOW );

	SetTimer( 1, 2000, NULL ); // wait for 3 seconds

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSplashDlg::OnTimer(UINT_PTR nIDEvent)
{
	EndDialog( 0 );
	CDialog::OnTimer(nIDEvent);
}
