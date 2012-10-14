// SpritePlaybackThread.cpp : implementation file
//

#include "stdafx.h"
#include "2DSpriteEditor.h"
#include "SpritePlaybackThread.h"

#include "Mainfrm.h"


// CSpritePlaybackThread

IMPLEMENT_DYNCREATE(CSpritePlaybackThread, CWinThread)

CSpritePlaybackThread::CSpritePlaybackThread()
{}


CSpritePlaybackThread::CSpritePlaybackThread( HWND hwndParent/*, CMy2DSpriteEditorDoc *pDoc */)
  : m_hwndParent( hwndParent ), m_pDoc(NULL), m_isAnimating(FALSE)
{
}

CSpritePlaybackThread::~CSpritePlaybackThread()
{
	TRACE0( "Sprite thread ended..." );
}

BOOL CSpritePlaybackThread::InitInstance()
{
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	m_pMainWnd = pParent;

	//6m_bAutoDelete = TRUE;
	//CMainFrame *mainWnd = reinterpret_cast<CMainFrame *>( pParent );
	//m_pDoc = CMy2DSpriteEditorDoc::getDoc( );

	return TRUE;
}

int CSpritePlaybackThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSpritePlaybackThread, CWinThread)
END_MESSAGE_MAP()


// CSpritePlaybackThread message handlers

BOOL CSpritePlaybackThread::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinThread::OnIdle(lCount);
}

int CSpritePlaybackThread::Run()
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT( m_pMainWnd != NULL );
	//CMainFrame *wnd = reinterpret_cast<CMainFrame *>( m_pMainWnd );
	CMy2DSpriteEditorDoc *m_pDoc = CMy2DSpriteEditorDoc::getDoc( );
		//reinterpret_cast<CMy2DSpriteEditorDoc *>( CMy2DSpriteEditorDoc::getDoc( ) );


	int i = m_pDoc->getCurrentFrame( );

	while( m_isAnimating )
	{
		
		for( ; i <= m_pDoc->getNumberOfFrames( ); i++ )
		{
			m_pDoc->setCurrentFrame( i );
			m_pDoc->SetModifiedFlag( );
			////wnd->RedrawWindow( 0, 0, RDW_INVALIDATE | RDW_NOERASE );
			m_pDoc->UpdateAllViews( NULL );
			Sleep( (DWORD) m_PlaybackSpeed );
		}
		i = 1;
	}
	
	TRACE0( "Sprite thread about to commit suicide..." );
	killThread( );

	return CWinThread::Run();
}


void CSpritePlaybackThread::setPlaybackSpeed( UINT speed )
{
	m_PlaybackSpeed = speed;
}

void CSpritePlaybackThread::killThread( )
{
	PostQuitMessage( 0 );
}

void CSpritePlaybackThread::setAnimation( const BOOL animate )
{
	m_isAnimating = animate;
}