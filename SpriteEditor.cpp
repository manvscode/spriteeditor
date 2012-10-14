// 2DSpriteEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "MainFrm.h"
#include "memleakcheck.h"
#include "SpriteEditorDoc.h"
#include "SpriteEditorView.h"
#include <ImageIO.h>
#include "SplashDlg.h"
#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpriteEditorApp

BEGIN_MESSAGE_MAP(CSpriteEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSpriteEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSpriteEditorApp construction

CSpriteEditorApp::CSpriteEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSpriteEditorApp object

CSpriteEditorApp theApp;


// CSpriteEditorApp initialization

BOOL CSpriteEditorApp::InitInstance()
{
	MEMLEAK_CHECK;

	// Show splash screen...
	#ifndef _DEBUG
	CSplashDlg splashDlg;
	splashDlg.DoModal( );
	#endif


	// turn on SSE in math lib...
	ImageIO::initialize( );

	_wsetlocale( LC_ALL, _T("") );
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(5);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSpriteEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSpriteEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;


	// The one and only window has been initialized, so show and update it
	//m_pMainWnd->AnimateWindow( 200, AW_CENTER | AW_BLEND );
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	virtual ~CAboutDlg();
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CBitmap m_AboutBmp;
	CLinkCtrl m_WebsiteLink;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();

	
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD), m_WebsiteLink()
{
	m_AboutBmp.LoadBitmap( MAKEINTRESOURCE( IDB_ABOUTBITMAP ) );
}

CAboutDlg::~CAboutDlg()
{
	m_AboutBmp.DeleteObject( );
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rc( 0, 0, 200, 50 );
	LITEM item;
	item.iLink = 0;
	item.mask = LIF_URL | LIF_ITEMID | LIF_STATE;
	item.state = LIS_ENABLED;
	item.stateMask = LIS_ENABLED;
	lstrcpyW( item.szID, _T("website") );
	lstrcpyW( item.szUrl, _T("<a href=\"http://www.fiu.edu/~jmarr002/\">http://www.fiu.edu/~jmarr002/</a>") );

	m_WebsiteLink.Create( WS_CHILD | CCS_ADJUSTABLE | CCS_TOP, rc, this, IDC_WEBSITELINK );
	m_WebsiteLink.SetItem(  &item );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAboutDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// App command to run the dialog
void CSpriteEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSpriteEditorApp message handlers


void CAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	EndDialog( 0 );
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	EndDialog( 0 );
	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	memDC.CreateCompatibleDC( NULL );

	//	WINDOWPLACEMENT wndPlacement;

	//GetWindowPlacement( &wndPlacement );
	//wndPlacement.rcNormalPosition.right = 256;
	//wndPlacement.rcNormalPosition.bottom = 256;
	//SetWindowPlacement( &wndPlacement );

	CBitmap *pOldBmp = memDC.SelectObject( &m_AboutBmp );

	dc.BitBlt( 0, 0, 256,256, &memDC,0, 0, SRCCOPY );
	//dc.TransparentBlt( 0, 0, 256, 256, &memDC, 0, 0, 256, 256, RGB(255,255,255) );
	memDC.SelectObject( pOldBmp );
	// Do not call CDialog::OnPaint() for painting messages
}

int CAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

int CSpriteEditorApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	ImageIO::deinitialize( );

	return CWinApp::ExitInstance();
}




